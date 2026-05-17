#ifndef MEMORY_H
#define MEMORY_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>

#define NUM_STARS   30
#define INIT_MEM_SIZE (1024*1024*1024)
#define START_WORD  0xDEADBEEF
#define CANARY 0xCAFEBABE
typedef struct {
    size_t size;
    size_t size_total;
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

void* smart_malloc(size_t size);
void* smart_realloc(void* ptr, size_t size);
size_t smart_size(void* ptr);
void  smart_free(void* ptr);
void* smart_calloc(size_t n, size_t size);
char* smart_strdup(const char *ptr);
int smart_asprintf(char** strp, const char* fmt, va_list args);
#endif // MEMORY_H
