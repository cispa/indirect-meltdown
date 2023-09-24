#include <stdio.h>
#include <stdlib.h>
#include "cacheutils.h"

#define inject(x) if(!setjmp(trycatch_buf)) { asm volatile(x); }

int main(int argc, char* argv[]) {
    signal(SIGSEGV, trycatch_segfault_handler);
    
    while(1) {
        printf("Press key to send interrupt...\n");
        getchar();
        inject("int $120");
        sched_yield();
    }
}
 
