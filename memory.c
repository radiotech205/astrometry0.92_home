#include "memory.h"

memory_s memory;

void init_memory(size_t size) {
    memory.size = size;
    memory.size_initial = size;
    memory.times = 0;
    memory.isfull = 0;
}

int use_memory(size_t use) {
    if(!memory.isfull) {
        if(((int64_t)(memory.size) - (int64_t)use) > 0) {
            memory.size -= use;
            memory.times++;
//            printf("\t\t\t");
//            print_memory();
            return 0;
        }
        else {
            memory.isfull = 1;
            return 1;
        }
    }
    else
        return 1;
}

int use_memory2(size_t use, void (*caller)(void)) {
    if(!memory.isfull) {
        if(((int64_t)(memory.size) - (int64_t)use) > 0) {
            memory.size -= use;
            memory.times++;
            printf("\t\t\t%p:", (void*)caller);
            print_memory();
            return 0;
        }
        else {
            memory.isfull = 1;
            return 1;
        }
    }
    else
        return 1;
}

void print_memory(void) {
    printf("%s: %d, %d\n", __FUNCTION__, memory.size_initial - memory.size, memory.times);
}

void* usage_memory(size_t use, int* result) {
    void* ptr = malloc(use);
    *result = use_memory(use);
    if(*result) {
        print_memory();
        printf("%s: EXIT\n", __FUNCTION__);
    }
    return ptr;
}
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
const char* get_func_name(void* func_addr) {
    Dl_info info;
    if (dladdr(func_addr, &info) && info.dli_sname) {
        return info.dli_sname;
    }
    return "unknown";
}

void* usage_memory2(size_t use, int* result, void (*caller)(void)) {
    static char last_function[32];
    void* ptr = malloc(use);
    *result = use_memory(use);
    if(*result) {
        print_memory();
        printf("\t\t\t%p:", (void*)caller);
        printf("\tnow:%s, last:%s\n", get_func_name(caller), last_function);
        printf("%s: EXIT\n", __FUNCTION__);

    }
    else {
        char* now_function = get_func_name(caller);
        strcpy(last_function, now_function);
    }
    return ptr;
}

int is_full_memory(void) {
    if((memory.size * 100/INIT_MEM_SIZE) <= 10)
        return 1;
    else
        return 0;
}
