//
// Created by Ding Cheng DONG on 16/05/2025 A.
//
#include "include/root_detection.h"
#include <cstdlib>
#include <fstream>
#include <android/log.h>
#define LOG_TAG "RootCheck"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)


bool check_su_exists() {
    const char* paths[] = {
            "/system/bin/su",
            "/system/xbin/su",
            "/sbin/su",
            "/system/su",
            "/system/bin/.ext/.su",
            "/system/usr/we-need-root/su",
            "/system/app/Superuser.apk",
            "/system/app/SuperSU.apk"
    };

    for (const char* path : paths) {
        std::ifstream file(path);
        if (file.good()) {
            LOGD("Root binary found at: %s", path);
            return true;
        }
    }
    return false;
}

bool check_su_command() {
    int result = system("su -c exit");
    if (result == 0) {
        LOGD("Device is rooted: 'su' command executed successfully.");
        return true;
    }
    return false;
}

bool isRoot() {
    if (check_su_exists() || check_su_command()) {
        LOGD("Root detected.");
        return true;
    }
    LOGD("No root detected.");
    return false;
}