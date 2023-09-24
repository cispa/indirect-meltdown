#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <asm/mtrr.h>
#include "cacheutils.h"
#include "ptedit_header.h"

typedef struct {
  uint16_t limit;
  size_t base __attribute__((packed));
} idt_t;


int main() {
    idt_t idt;
    asm volatile("sidt %0" : "=m"(idt) : : "memory"); 
    printf("IDT base: 0x%zx\n", idt.base);

    if (ptedit_init()) {
        fprintf(stderr, "Could not init PTEditor\n");
        return 1;
    }  
    size_t pfn = ptedit_pte_get_pfn((void *)(idt.base), 0);
    printf("IDT PFN: 0x%zx\n", pfn);

    char* idt_vaddr = ptedit_pmap(pfn * 4096, 4096);
    printf("IDT vaddr: %p\n", idt_vaddr);
    
    size_t sum = 0;
    if(idt_vaddr != (char*)-1) {
        for(int i = 0; i < 1000; i++) {
            sum += reload_t(idt_vaddr);
        }
        printf("IDT access time: %zd\n", sum / 1000);
    }
    
    int fd;
    struct mtrr_sentry sentry;
    sentry.base = pfn * 4096;
    sentry.size = 4096;
    sentry.type = 0; // UC
    
    if((fd = open("/proc/mtrr", O_WRONLY, 0)) == -1) {
        printf("Could not open /proc/mtrr\n");
        exit(1);
    }
    if(ioctl(fd, MTRRIOC_ADD_ENTRY, &sentry) == -1) {
        printf("Could not add MTRRR\n");
        perror("MTRR");

	system("echo 'disable=9' > /proc/mtrr");
        if(ioctl(fd, MTRRIOC_ADD_ENTRY, &sentry) == -1) {
        	printf("Could not add MTRRR\n");
        	perror("MTRR");
        exit(1);

	}


    }
    
        
    if(idt_vaddr != (char*)-1) {
        sum = 0;
        for(int i = 0; i < 1000; i++) {
            sum += reload_t(idt_vaddr);
        }
        printf("IDT access time: %zd\n", sum / 1000);
    }
    ptedit_cleanup();
    system("cat /proc/mtrr");
  
    printf("Press any key to restore IDT to cachable\n");
    getchar();
    printf("Done\n");
    
}
