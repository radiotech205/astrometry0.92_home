#ifndef MEMORY_H
#define MEMORY_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define NUM_STARS   30
#define INIT_MEM_SIZE (530*1024)

typedef struct {
    size_t size;
    size_t size_initial;
    uint32_t times;
    uint8_t isfull;
} memory_s;

void init_memory(size_t size);
int use_memory(size_t use);
int use_memory2(size_t use, void (*caller)(void));
void print_memory(void);
void* usage_memory(size_t use, int* result);
void* usage_memory2(size_t use, int* result, void (*caller)(void));
int is_full_memory(void);
#endif // MEMORY_H

/*
#include "memory.h"

if(use_memory()) {
    print_memory();
    printf("%s: EXIT\n", __FUNCTION__);
    }

    int result;
    vf = usage_memory(sizeof(verify_field_t), &result);
    if(!vf || result)   return NULL;
*/


