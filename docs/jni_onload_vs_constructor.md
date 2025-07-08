### 关于jni_onload和__attribute__((constructor))哪个先执行， Copliot的解答：

在 Android NDK 中，constructor（即 .so 文件中的全局或静态对象的构造函数）会先于 JNI_OnLoad 执行。  
执行顺序：
Constructor:  
当 .so 文件被加载时（通过 System.loadLibrary 或 dlopen），全局或静态对象的构造函数会首先执行。
这是因为动态链接器在加载共享库时会初始化全局和静态对象。
JNI_OnLoad:
在 .so 文件加载完成后，虚拟机会调用 JNI_OnLoad（如果它存在）。
JNI_OnLoad 通常用于注册 JNI 方法或执行其他初始化逻辑。
示例代码：

#include <jni.h>
#include <android/log.h>

#define LOG_TAG "NDKExample"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

// Global constructor
__attribute__((constructor)) void global_constructor() {
LOGI("Global constructor executed");
}

// JNI_OnLoad
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
LOGI("JNI_OnLoad executed");
return JNI_VERSION_1_6;
}

输出顺序：
Global constructor executed
JNI_OnLoad executed
结论：
Constructor 会先于 JNI_OnLoad 执行。