#include <jni.h>
#include <string>
#include <android/log.h>
#include <pthread.h>
#include <sys/system_properties.h>
#include <unistd.h>
#include <fstream>
#include <cstdlib> // For exit()
#include <sys/socket.h>
#include <linux/in.h>
#include <arpa/inet.h>
#include "include/frida_detection.h"
#include "include/frida_function_hook_detection.h"
#include "include/emulator_detection.h"

#define LOG_TAG "haha rasp c"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#include "../common/common.h"
#include "include/create_thread.h"
#include "include/lib_patch_detection.h"
#define PORT 12345

JavaVM* gJavaVM = nullptr; // Global reference to JavaVM for JNI in another thread

static int connect_local_socket(int code);

extern "C"
JNIEXPORT jstring JNICALL
Java_com_handhandlab_handyAndroidHackThis_jni_RaspInterface_entryPoint(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("Hello from C++");
}

void callback(JNIEnv* env, void* arg, jmethodID callbackMethod, int code, const char* msg) {
    jstring message = env->NewStringUTF(msg);
    env->CallVoidMethod(static_cast<jobject>(arg), callbackMethod, code, message);
    env->DeleteLocalRef(message);
}

void* doCheck(void* arg) {
    // prepare jni env for thread
    JNIEnv* env = nullptr;
    // Attach the thread to the JVM
    if (gJavaVM->AttachCurrentThread(&env, nullptr) != 0) {
        LOGD("Failed to attach thread to JVM");
        return nullptr;
    }
    // find jni method
    jclass callbackClass = env->GetObjectClass(static_cast<jobject>(arg));
    jmethodID callbackMethod = env->GetMethodID(callbackClass, "onJniCallback", "(ILjava/lang/String;)V");

    while(true) {
        if (isEmulator()) {
            callback(env, arg, callbackMethod, CODE_EMULATOR, "Emulator - DETECTED!");
//        LOGD("Emulator detected, exiting...");
            // Exit the process if an emulator is detected
//        exit(0);
        } else {
            LOGD("No emulator detected.");
            callback(env, arg, callbackMethod, CODE_EMULATOR, "Emulator detection - PASS");
        }

        LOGD("check pthread_create hooking");
        if (check_status() || check_maps()) {
            callback(env, arg, callbackMethod, CODE_FRIDA, "Frida - DETECTED");
        } else {
            LOGD("No frida detected.");
            callback(env, arg, callbackMethod, CODE_FRIDA, "Frida detection - PASS");
        }
        sleep(CHECK_TIME);
    }

    // cleanup
    env->DeleteGlobalRef(static_cast<jobject>(arg));
    gJavaVM->DetachCurrentThread(); // Detach the thread from the JVM
    return nullptr;
}

void libPatchDetectionCallback(int result) {
    connect_local_socket(result);
    LOGD("ok!");
}

int doLibPatchDetection(void* arg) {
    start_lib_scan(reinterpret_cast<void*>(libPatchDetectionCallback));
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_handhandlab_handyAndroidHackThis_jni_RaspInterface_startRuntimeApplicationSelfProtection(JNIEnv *env, jobject thiz, jobject jniCallback) {
    // Store the JavaVM reference
    env->GetJavaVM(&gJavaVM);
    // Create a global reference for the callback
    // JNI local references are tied to the thread that created them. Passing a local reference to another thread can lead to undefined behavior.
    jobject globalCallback = env->NewGlobalRef(jniCallback);

    pthread_t thread1;
    pthread_create(&thread1, nullptr, doCheck, globalCallback);
    LOGD("After pthread_create");

    LOGD("check method hook");
    if (check_inlinehook()) {
        LOGD("pthread_create is hooked");
        jclass callbackClass = env->GetObjectClass(jniCallback);
        jmethodID callbackMethod = env->GetMethodID(callbackClass, "onJniCallback", "(ILjava/lang/String;)V");
        jstring message = env->NewStringUTF("Method hooked detected");
        env->CallVoidMethod(jniCallback, callbackMethod, CODE_FRIDA, message);
    }

    LOGD("start custom thread for lib patch detection");
    create_thread(doLibPatchDetection);
}

/**
 * Using JNI in clone thread cause a lot of issue. So use socket to communicate with java/kotlin code.
 * PORT: 12345
 * see [RaspInterface.kt]
 *
 * @param code the result code to send to java, for lib patch detection it's CODE_LIB_PATCH, see common.h
 * @return
 */
int connect_local_socket(int code) {
    int sock;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char message[10];
    snprintf(message, sizeof(message), "%d\n", code);

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0) {
        perror("Invalid address or address not supported");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Send message to server
    send(sock, message, strlen(message), 0);
    LOGD("Message sent to server\n");

    // Read response from server
    read(sock, buffer, sizeof(buffer));
    LOGD("Received: %s\n", buffer);

    close(sock);
    return 0;
}