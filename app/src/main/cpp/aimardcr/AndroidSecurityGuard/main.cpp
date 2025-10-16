#include <cstdio>
#include <unistd.h>
#include <pthread.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <thread>

#include <jni.h>

#include "Utils/Log.h"

#include "Modules/AntiDebug/AntiDebug.h"
#include "Modules/FridaDetect/FridaDetect.h"
#include "Modules/RiGisk/RiGisk.h"
#include "Modules/RootDetect/RootDetect.h"
#include "Modules/AntiDump/AntiDump.h"
#include "Modules/AntiLibPatch/AntiLibPatch.h"

JavaVM *g_VM = nullptr;

jclass mainActivityClass = nullptr;
jmethodID addLogMethod = nullptr;
void addLog(std::string log, int code = 0) {

    while (g_VM == nullptr || mainActivityClass == nullptr || addLogMethod == nullptr) {
        __android_log_print(ANDROID_LOG_ERROR, "AndroidNativeGuard", "Waiting for JavaVM and methods to be initialized...");
        sleep(5);
    }

    JNIEnv *env;
    g_VM->AttachCurrentThread(&env, nullptr);

    time_t now = time(nullptr);
    tm *ltm = localtime(&now);

    char date[20];
    sprintf(date, "%02d:%02d:%02d", ltm->tm_hour, ltm->tm_min, ltm->tm_sec);

//    log = "[" + std::string(date) + "] " + log;

    env->CallStaticVoidMethod(mainActivityClass, addLogMethod, code, env->NewStringUTF(log.c_str()));

    g_VM->DetachCurrentThread();
}

// ==================== Callbacks ==================== //
void onDebuggerDetected() {
    addLog("DETECTED", CODE_DEBUGGER);
}

void onFridaDetected() {
    addLog("DETECTED - Frida is running on this device", CODE_FRIDA);
}

void onDumpDetected() {
    addLog("An attempt to access/dump memory detected.");
}

void onLibTampered(const char *libPath, uint32_t old_checksum, uint32_t new_checksum) {
    char log[1024];
    sprintf(log, "%s has been tampered, 0x%08X -> 0x%08X", libPath, old_checksum, new_checksum);
    addLog(log, CODE_LIB_PATCH);
}

// ==================== Main ==================== //
std::vector<IModule *> services;
std::vector<std::thread> threads;

void AndroidNativeGuard() {
    __android_log_print(ANDROID_LOG_ERROR, "AndroidNativeGuard", "Android Native Guard service started.");

    RootDetect rootDetect;
    if (rootDetect.execute()) {
        addLog("Root detected", CODE_ROOTED);
    }

    RiGisk riGisk;
    if (riGisk.execute()) {
        addLog("RiGisk: Zygote injection detected.");
    }

    services.push_back(new AntiDebug(onDebuggerDetected));
    services.push_back(new FridaDetect(onFridaDetected));
    services.push_back(new AntiDump(onDumpDetected));
    services.push_back(new AntiLibPatch(onLibTampered));

    for (auto &service : services) {
        threads.emplace_back([&]() {
            while (true) {
                service->execute();
                sleep(CHECK_TIME);
            }
        });
    }

    for (auto &thread : threads) {
        thread.detach();
    }
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_VM = vm;

    JNIEnv *env;
    vm->GetEnv((void **)&env, JNI_VERSION_1_6);

    jclass clazz = env->FindClass("com/handhandlab/handyAndroidHackThis/AndroidSecurityGuard");
    addLogMethod = env->GetStaticMethodID(clazz, "addLog", "(ILjava/lang/String;)V");
    mainActivityClass = (jclass)env->NewGlobalRef(clazz);

    // execute security initializer in a background thread,
    // since AntiLibPatch constructor may take a significant time
    // main thread may become unresponsive
    std::thread(AndroidNativeGuard).detach();
    // checking is started in constructor

    return JNI_VERSION_1_6;
}