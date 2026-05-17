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
            memory.size_total += use;
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
    printf("%s: size = %d, times = %d\n", __FUNCTION__, memory.size_initial - memory.size, memory.times);
    printf("%s: size_total = %d\n", __FUNCTION__, memory.size_total);
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
//////////////////////////////////////////////////
void* smart_malloc(size_t size) {
    if (size == 0) return NULL;

    // Выделяем память: заголовок (размер) + данные
    size_t total_size = 3*sizeof(size_t) + size;
    size_t* header = (size_t*)malloc(total_size);
    if (!header) return NULL;
    use_memory(total_size);
    // Сохраняем размер в заголовке
    header[0] = START_WORD;
    header[1] = size;
    header[2] = CANARY;
    // Возвращаем указатель на начало данных (после заголовка)
    return (void*)(header + 3);
}
void* smart_realloc(void* ptr, size_t size) {
    size_t size_old = smart_size(ptr);
    int64_t size_delta = (size - size_old);
    int result = 0;
    if(size_delta > 0)
         result = use_memory(size_delta);
    else {
        memory.size += size_delta;
        memory.size_total += size_delta;
        if((int64_t)(memory.size) > 0)
            memory.isfull = 0;
    }
    if(0 != result)
        return NULL;

    size_t* header = malloc(3*sizeof(size_t) + size);
    if (!header) return NULL;

//    size_t size_copy = (size_old < size) ? size_old : size;
//    memcpy(header, ptr, size_copy);
    memcpy(header, ptr, size);

    smart_free(ptr);

    header[0] = START_WORD;
    header[1] = size;
    header[2] = CANARY;
    return (void*)(header + 3);
}

// Получить указатель на заголовок по указателю на данные
static size_t* get_header(void* ptr) {
    if (!ptr) return NULL;
    return ((size_t*)ptr) - 3;
}

// Получить размер выделенного блока (полезная нагрузка, без заголовка)
size_t smart_size(void* ptr) {
    if (!ptr) return 0;

    size_t* header = get_header(ptr);
    return header[1];
}

void  smart_free(void* ptr) {
    if (!ptr) return;
    size_t* header = get_header(ptr);

//    fprintf(stderr, "smart_free(%p): header=%p, header[0]=0x%zx, header[1]=%zu,"
//                    "header[2]=0x%zx\n",
//            ptr, header, header[0], header[1], header[2]);

    if(header && (START_WORD == header[0]) && (CANARY == header[2])) {
        memory.size += smart_size(ptr);
        //fprintf(stderr, "  -> наш блок, free(header)\n");
        header[0] = 0;
        header[1] = 0;
        header[2] = 0;
        free(header);
        header = NULL;
    }
    else {
        //fprintf(stderr, "  -> чужой блок, free(ptr)\n");
        free(ptr);
    }
    ptr = NULL;
}

void* smart_calloc(size_t n, size_t size) {
    void* ptr = smart_malloc(n * size);
    if(!ptr)    return NULL;
    memset(ptr, 0, n * size);
    return ptr;
}

char* smart_strdup(const char* ptr) {
    if(!ptr)  {
//        fprintf(stderr, "ERROR: smart_strdup(NULL) called from %s:%d\n",
//                __FILE__, __LINE__);
        return NULL;
    }
    size_t length = strlen(ptr) + 1;
    char* ptr_res = (char* )smart_malloc(length);
    if(!ptr_res)    return NULL;
    memcpy(ptr_res, ptr, length);
    return ptr_res;
}

int smart_asprintf(char** strp, const char* format, va_list args) {
    va_list args_copy;
    va_copy(args_copy, args);

    int len = vsnprintf(NULL, 0, format, args_copy);
    va_end(args_copy);

    if (len < 0) return -1;

    *strp = (char*)smart_malloc(len + 1);
    if (!*strp) return -1;

    vsnprintf(*strp, len + 1, format, args);
    return len;
}
