//
// Created by Ding Cheng DONG on 21/05/2025 A.
//

#include <sys/system_properties.h>
#include <string.h>
#include <unistd.h>
#include <android/log.h>
#include "include/emulator_detection.h"

#define LOG_TAG "haha"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

bool isEmulator() {
    // Check system property "ro.kernel.qemu"
    char value[PROP_VALUE_MAX];
    if (__system_property_get("ro.kernel.qemu", value) > 0) {
        if (strcmp(value, "1") == 0) {
            LOGD("Detected emulator via ro.kernel.qemu");
            return true;
        }
    }

    // Check for common emulator files
    if (access("/dev/qemu_pipe", F_OK) == 0) {
        LOGD("Detected emulator via /dev/qemu_pipe");
        return true;
    }

    // Check system property "ro.product.device"
    if (__system_property_get("ro.product.device", value) > 0) {
        if (strstr(value, "emulator") || strstr(value, "goldfish") || strstr(value, "ranchu")) {
            LOGD("Detected emulator via ro.product.device: %s", value);
            return true;
        }
    }

    // Check for other emulator-specific properties
    if (__system_property_get("ro.hardware", value) > 0) {
        if (strstr(value, "goldfish") || strstr(value, "ranchu")) {
            LOGD("Detected emulator via ro.hardware: %s", value);
            return true;
        }
    }

    return false;
}