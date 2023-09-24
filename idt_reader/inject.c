#include <stdio.h>
#include <stdlib.h>
#include "cacheutils.h"

#define inject(x) if(!setjmp(trycatch_buf)) { asm volatile(x); }

int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: %s <count> <wait time (cycles)>\n", argv[0]);
        return 1;
    }
    signal(SIGSEGV, trycatch_segfault_handler);
    
    int count = atoi(argv[1]);
    size_t waittime = strtoull(argv[2], NULL, 0);

    for(int i = 0; i < count; i++) {

        inject("int $120");

        size_t start = rdtsc();
        while(rdtsc() - start < waittime);
    }
}
 
