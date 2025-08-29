#include <linux/sched.h>
#include <malloc.h>
#include <sched.h>
#include <asm/signal.h>
#include <stdlib.h>
#include "include/create_thread.h"

/**
 * Create a thread by using clone.
 * In case of the pthread_create function is tempered.
 *
 * @param child_func
 * @return
 */
int create_thread(int (* child_func)(void*)) {
    // Allocate stack for child task.
    const int STACK_SIZE = 65536;
    char* stack = (char *)malloc(STACK_SIZE);
    if (!stack) {
        perror("malloc");
        exit(1);
    }

    // When called with the command-line argument "vm", set the CLONE_VM flag on.
    int flags = CLONE_SIGHAND|CLONE_FS|CLONE_VM|CLONE_FILES|CLONE_THREAD;

//    strcpy(buf, "hello from parent");
    if (clone(child_func, (void *)(stack + STACK_SIZE), flags | SIGCHLD, NULL) == -1) {
        perror("clone");
        exit(1);
    }
    return 0;
}
