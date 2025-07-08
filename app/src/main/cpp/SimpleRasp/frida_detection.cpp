//
// Created by Ding Cheng DONG on 14/05/2025 A.
//
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <android/log.h>
#include <cstdio>
#include <dirent.h>
#include "include/frida_detection.h"

#define LOG_TAG "FridaDetection"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define MAX_PATH 256
#define MAX_BUFFER 65536

/**
 * https://www.52pojie.cn/thread-1921073-1-1.html
 */
bool check_maps() {
    // 定义一个足够大的字符数组line，用于存储读取的行
    char line[512];
    // 打开当前进程的内存映射文件/proc/self/maps进行读取
    FILE* fp = fopen("/proc/self/maps", "r");
    if (fp) {
        // 如果文件成功打开，循环读取每一行
        while (fgets(line, sizeof(line), fp)) {
            // 使用strstr函数检查当前行是否包含"frida"字符串
            if (strstr(line, "frida") || strstr(line, "gadget")) {
                // 如果找到了"frida"，关闭文件并返回true，表示检测到了恶意库
                fclose(fp);
                return true; // Evil library is loaded.
            }
        }
        // 遍历完文件后，关闭文件
        fclose(fp);
    } else {
        // 如果无法打开文件，记录错误。这可能意味着系统状态异常
        // 注意：这里的代码没有处理错误，只是注释说明了可能的情况
    }
    // 如果没有在内存映射文件中找到"frida"，返回false，表示没有检测到恶意库
    return false; // No evil library detected.
}

bool read_file(const char* path, char* buffer, size_t buffer_size) {
    FILE* file = fopen(path, "r");
    if (!file) {
        LOGD("Failed to open file: %s", path);
        return false;
    }

    size_t bytesRead = fread(buffer, 1, buffer_size - 1, file);
    buffer[bytesRead] = '\0'; // Null-terminate the buffer
    fclose(file);
    return true;
}

bool check_status() {
    DIR *dir = opendir("/proc/self/task/");
    struct dirent *entry;
    char status_path[MAX_PATH];
    char buffer[MAX_BUFFER];
    int found = false;

    if (dir) {
        while ((entry = readdir(dir)) != nullptr) {
            if (entry->d_type == DT_DIR) {
                if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
                    continue;
                }
                snprintf(status_path, sizeof(status_path), "/proc/self/task/%s/status", entry->d_name);
                if (read_file(status_path, buffer, sizeof(buffer)) == -1) {
                    continue;
                }
                if (strcmp(buffer, "null") == 0) {
                    continue;
                }
                char *line = strtok(buffer, "\n");
                while (line) {
                    if (strstr(line, "Name:") != nullptr) {
                        if (strstr(line, "gmain") || strstr(line, "gum-js-loop") || strstr(line, "pool-frida") || strstr(line, "gdbus")) {
                            found = true;
                            break;
                        }
                    }
                    line = strtok(nullptr, "\n");
                }
                if (found) break;
            }
        }
        closedir(dir);
    }
    return found;
}
