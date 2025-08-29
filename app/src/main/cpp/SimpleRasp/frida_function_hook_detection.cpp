//
// Created by Ding Cheng DONG on 16/05/2025 A.
//
#include <jni.h>
#include <string>
#include <dlfcn.h>
#include "dlfcn/local_dlfcn.h"

/**
 * Check if "pthread_create" is hooked
 * @return true if the method is hooked
 */
bool check_inlinehook() {
    // 根据系统架构选择对应的libc.so库路径
    const char *lib_path;

    // https://android.googlesource.com/platform/bionic/+/HEAD/docs/defines.md
#ifdef __LP64__
    lib_path = "/system/lib64/libc.so";
#else
    lib_path = "/system/lib/libc.so";
#endif

    // 定义要比较的字节数
    const int CMP_COUNT = 8;
    // 指定要查找的符号名，这里是"open"函数
    const char *sym_name = "pthread_create";

    // 使用local_dlopen函数打开指定的共享库，并获取操作句柄
    struct local_dlfcn_handle *handle = static_cast<local_dlfcn_handle *>(local_dlopen(lib_path));
    if (!handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 获取目标函数在libc.so中的偏移量
    off_t offset = local_dlsym(handle, sym_name);

    // 关闭handle，因为我们接下来使用标准的dlopen/dlsy来获取函数地址
    local_dlclose(handle);

    // 打开libc.so文件，准备读取数据
    FILE *fp = fopen(lib_path, "rb");
    if (!fp) {
        return JNI_FALSE; // 如果无法打开文件，返回false
    }

    // 定义一个缓冲区，用于存储读取的文件内容
    char file_bytes[CMP_COUNT] = {0};
    // 读取指定偏移量处的CMP_COUNT个字节
    fseek(fp, offset, SEEK_SET);
    fread(file_bytes, 1, CMP_COUNT, fp);
    fclose(fp);

    // 使用dlopen函数打开libc.so共享库，并获取操作句柄
    void *dl_handle = dlopen(lib_path, RTLD_NOW);
    if (!dl_handle) {
        return JNI_FALSE; // 如果无法打开共享库，返回false
    }

    // 使用dlsym函数获取目标函数的地址
    void *sym = dlsym(dl_handle, sym_name);
    if (!sym) {
        dlclose(dl_handle);
        return JNI_FALSE; // 如果无法找到符号，返回false
    }

    // 比较原libc.so中的"open"函数内容与通过dlsym获取的"open"函数内容是否一致
    int is_hook = memcmp(file_bytes, sym, CMP_COUNT) != 0;

    // 关闭dlopen打开的共享库句柄
    dlclose(dl_handle);

    // 返回比较结果，如果函数被hook则返回JNI_TRUE，否则返回JNI_FALSE
    return is_hook ? JNI_TRUE : JNI_FALSE;
}
