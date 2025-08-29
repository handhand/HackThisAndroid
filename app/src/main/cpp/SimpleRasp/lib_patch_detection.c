#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <elf.h>
#include <dirent.h>
#include <ctype.h>

#include <sys/prctl.h>
#include <sys/stat.h>
#include <asm/unistd.h>
#include <android/log.h>

#include <sys/system_properties.h>
#include "../common/common.h"

#define MAX_LINE 512
#define MAX_LENGTH 256
static const char *APPNAME = "DetectLibPatch";
static const char *PROC_MAPS = "/proc/self/maps";
#define LIBC "libc.so"

//Structure to hold the details of executable section of library
typedef struct stExecSegment {
    unsigned long offset;
    unsigned long memsize;
    unsigned long checksum;
} execSegment;


#define NUM_LIBS 1

//Include more libs as per your need, but beware of the performance bottleneck especially
//when the size of the libraries are > few MBs
static const char *libstocheck[NUM_LIBS] = {"libSimpleRasp.so"};
static execSegment *elfSectionArr[NUM_LIBS] = {NULL};


#ifdef __LP64__
typedef Elf64_Ehdr Elf_Ehdr;
typedef Elf64_Shdr Elf_Shdr;
typedef Elf64_Phdr Elf_Phdr;
#else
typedef Elf32_Ehdr Elf_Ehdr;
typedef Elf32_Shdr Elf_Shdr;
typedef Elf32_Phdr Elf_Phdr;
#endif

static inline void parse_proc_maps_to_fetch_path(char **filepaths);

static inline bool fetch_checksum_from_program_header(const char *filePath, execSegment **pTextSection);

static inline bool
scan_executable_segments(char *map, execSegment *pTextSection, const char *libraryName);

static inline ssize_t read_one_line(int fd, char *buf, unsigned int max_len);

static inline unsigned long checksum(void *buffer, size_t len);

static inline void detect_frida_memdiskcompare();

static inline void detect_frida_loop(void *pargs);

static void(* libScanCallback)(int) = NULL;

/**
 * Check if the so is tempered
 * 1. read the /proc/self/maps to get the path of the library
 * 2. read the program header to get the executable section offset and size, then calculate the checksum
 * 3. read the /proc/self/maps to get the executable segment memory and calculate the checksum
 * @param arg callback
 * @return
 */
//__attribute__((constructor))
int start_lib_scan(void* arg) {
    libScanCallback = arg;

    char *filePaths[NUM_LIBS];

    parse_proc_maps_to_fetch_path(filePaths);
    __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "lib path %s", filePaths[0]);
    for (int i = 0; i < NUM_LIBS; i++) {
        fetch_checksum_from_program_header(filePaths[i], &elfSectionArr[i]);
        if (filePaths[i] != NULL)
            free(filePaths[i]);
    }

    detect_frida_loop(NULL);
    return 0;
}

__attribute__((always_inline))
static inline void parse_proc_maps_to_fetch_path(char **filepaths) {
    int fd = 0;
    char map[MAX_LINE];
    int counter = 0;
    if ((fd = openat(AT_FDCWD, PROC_MAPS, O_RDONLY | O_CLOEXEC, 0)) != 0) {

        while ((read_one_line(fd, map, MAX_LINE)) > 0) {
            for (int i = 0; i < NUM_LIBS; i++) {
                if (strstr(map, libstocheck[i]) != NULL) {
                    char tmp[MAX_LENGTH] = "";
                    char path[MAX_LENGTH] = "";
                    char buf[5] = "";
                    sscanf(map, "%s %s %s %s %s %s", tmp, buf, tmp, tmp, tmp, path);
                    if (buf[2] == 'x') {
                        size_t size = strlen(path) + 1;
                        filepaths[i] = malloc(size);
                        strlcpy(filepaths[i], path, size);
                        __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "get path: %s", path);
                        counter++;
                    }
                }
            }
            if (counter == NUM_LIBS)
                break;
        }
        close(fd);
    }
}

/**
 * Fetch the executable segment offset and size from program header, and read the content to
 * get the checksum.
 *
 * @param filePath
 * @param pTextSection
 * @return
 */
__attribute__((always_inline))
static inline bool fetch_checksum_from_program_header(const char *filePath, execSegment **pTextSection) {

    Elf_Ehdr ehdr;
    Elf_Phdr  progHdr;
    int fd;
    fd = openat(AT_FDCWD, filePath, O_RDONLY | O_CLOEXEC, 0);
    if (fd < 0) {
        return NULL;
    }

    read(fd, &ehdr, sizeof(Elf_Ehdr));
    lseek(fd, (off_t) ehdr.e_phoff, SEEK_SET);

    unsigned long memsize = 0;
    unsigned long offset = 0;

    for (int i = 0; i < ehdr.e_phnum; i++) {
        memset(&progHdr, 0, sizeof(Elf_Phdr));
        read(fd, &progHdr, sizeof(Elf_Phdr));

        //Typically PLT and Text Sections are executable sections which are protected
        if (progHdr.p_flags & PF_X) {
            offset = progHdr.p_offset;
            memsize = progHdr.p_memsz;
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "ProgramHeader found offset:[%x] size:[%x]", progHdr.p_offset, progHdr.p_memsz);
            break;
        }
    }
    //This memory is not released as the checksum is checked in a thread
    *pTextSection = malloc(sizeof(execSegment));

    lseek(fd, (off_t) offset, SEEK_SET);
    u_int8_t *buffer = malloc(memsize * sizeof(uint8_t));
    read(fd, buffer, memsize);
    (*pTextSection)->offset = offset;
    (*pTextSection)->memsize = memsize;
    (*pTextSection)->checksum = checksum(buffer, memsize);
    free(buffer);

    close(fd);
    return true;
}

__attribute__((always_inline))
static inline ssize_t read_one_line(int fd, char *buf, unsigned int max_len) {
    char b;
    ssize_t ret;
    ssize_t bytes_read = 0;

    memset(buf, 0, max_len);

    do {
        ret = read(fd, &b, 1);

        if (ret != 1) {
            if (bytes_read == 0) {
                // error or EOF
                return -1;
            } else {
                return bytes_read;
            }
        }

        if (b == '\n') {
            return bytes_read;
        }

        *(buf++) = b;
        bytes_read += 1;

    } while (bytes_read < max_len - 1);

    return bytes_read;
}

__attribute__((always_inline))
static inline unsigned long checksum(void *buffer, size_t len) {
    unsigned long seed = 0;
    uint8_t *buf = (uint8_t *) buffer;
    size_t i;
    for (i = 0; i < len; ++i)
        seed += (unsigned long) (*buf++);
    return seed;
}

/**
 * Parse the /proc/self/maps to get the executable segment memory, and read that memory
 * and calculate the checksum.
 * Compare the checksum of the executable memory with the checksum of the executable segment on the
 * file.
 *
 * @param map 
 * @param pElfSectArr
 * @param libraryName
 * @return
 */
__attribute__((always_inline))
static inline bool
scan_executable_segments(char *map, execSegment *pElfSectArr, const char *libraryName) {
    unsigned long start, end;
    char buf[MAX_LINE] = "";
    char path[MAX_LENGTH] = "";
    char tmp[100] = "";

    sscanf(map, "%lx-%lx %s %s %s %s %s", &start, &end, buf, tmp, tmp, tmp, path);

    if (buf[2] == 'x') {
        if (buf[0] == 'r') {
            uint8_t *buffer = NULL;
            // start address on the memory
            buffer = (uint8_t *) start;
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "checkpoint: buffer:[%lx], offset:[%lx], memsize:[%lx]", buffer, pElfSectArr->offset, pElfSectArr->memsize);
            unsigned long output = checksum(buffer, pElfSectArr->memsize);
            __android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Checksum:[%ld][%ld]", output,
                                pElfSectArr->checksum);

            if (output != pElfSectArr->checksum) {
                __android_log_print(ANDROID_LOG_VERBOSE, APPNAME,
                                    "Executable Section Manipulated, "
                                    "maybe due to Frida or other hooking framework."
                                    "Act Now!!!");
                libScanCallback(CODE_LIB_PATCH);
                return true;
            }

        }
    }
    return false;
}

__attribute__((always_inline))
static inline void detect_frida_memdiskcompare() {
    int fd = 0;
    char map[MAX_LINE];

    if ((fd = openat(AT_FDCWD, PROC_MAPS, O_RDONLY | O_CLOEXEC, 0)) != 0) {

        while ((read_one_line(fd, map, MAX_LINE)) > 0) {
            for (int i = 0; i < NUM_LIBS; i++) {
                if (strstr(map, libstocheck[i]) != NULL) {
                    if (scan_executable_segments(map, elfSectionArr[i], libstocheck[i])) {
                        break;
                    }
                }
            }
        }
    } else {
        __android_log_print(ANDROID_LOG_WARN, APPNAME,
                            "Error opening /proc/self/maps. That's usually a bad sign.");

    }
    close(fd);

}

void detect_frida_loop(void *pargs) {

    struct timespec timereq;
    timereq.tv_sec = 5; //Changing to 5 seconds from 1 second
    timereq.tv_nsec = 0;

    while (true) {
        detect_frida_memdiskcompare();
        nanosleep(&timereq, NULL);
    }
}
