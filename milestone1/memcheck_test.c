#include "memcheck_test.h"
#include "memcheck.h"

#include <unistd.h>
#include <sys/mman.h>

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#define SAMPLE_SIZE 4096

const char *test_memcheck_malloc_simple()
{
    void *addrs[SAMPLE_SIZE];
    size_t i;

    for (i = 0; i < SAMPLE_SIZE; i++)
        addrs[i] = mc_malloc(i);

    for (i = 0; i < SAMPLE_SIZE; i++)
        mc_free(addrs[i]);

    return NULL;
}

const char *test_memcheck_malloc_complex()
{
    void *addrs[SAMPLE_SIZE];
    size_t i, idx, rng;

    for (i = 0; i < SAMPLE_SIZE; i++)
    {
        rng = ((size_t)rand()) % 3;

        switch (rng)
        {
            case 0: 
                addrs[i] = mc_malloc(i);        
                break;

            case 1: 
                addrs[i] = mc_calloc(10, i);    
                break;

            case 2:
                if (i > 0)
                {
                    idx = ((size_t)rand()) % i;
                    addrs[idx] = mc_realloc(addrs[idx], i);
                    i--;
                }
                else 
                    addrs[i] = mc_malloc(i);
                break;

            default:
                abort();
        }
    }

    for (i = 0; i < SAMPLE_SIZE; i++)
        mc_free(addrs[i]);

    return NULL;
}

const char *test_memcheck_mmap_simple()
{
    void *addrs[SAMPLE_SIZE];
    size_t i;

    for (i = 0; i < SAMPLE_SIZE; i++)
        addrs[i] = mc_mmap(NULL, i * sysconf(_SC_PAGE_SIZE), 
                           PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 
                           -1, 0);

    for (i = 0; i < SAMPLE_SIZE; i++)
        mc_munmap(addrs[i], i * sysconf(_SC_PAGE_SIZE));

    return NULL;
}

const char *test_memcheck_complex()
{
    void *addrs[SAMPLE_SIZE];
    bool ismmap[SAMPLE_SIZE];
    size_t i, rng;

    for (i = 0; i < SAMPLE_SIZE; i++)
    {
        rng = ((size_t)rand()) % 3;

        switch (rng)
        {
            case 0: 
                addrs[i] = mc_malloc(i);        
                ismmap[i] = false;
                break;

            case 1: 
                addrs[i] = mc_calloc(10, i);    
                ismmap[i] = false;
                break;

            case 2:
                addrs[i] = mc_mmap(NULL, i * sysconf(_SC_PAGE_SIZE),
                                   PROT_READ | PROT_WRITE, 
                                   MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
                ismmap[i] = true;
                break;

            default:
                abort();
        }
    }

    for (i = 0; i < SAMPLE_SIZE; i++)
        if (ismmap[i])
            mc_munmap(addrs[i], i * sysconf(_SC_PAGE_SIZE));
        else
            mc_free(addrs[i]);

    return NULL;
}
