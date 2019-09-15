#include "xv6/asm.h"
#include "xv6/memlayout.h"
#include "xv6/mmu.h"

.code 32
#Disbale the interrupts
cli

#Turn off paging
movl    %cr0, %eax
xorl    $CR0_PE, %eax
movl    %eax, %cr0

#Use 16bit GDT
lgdt    gdtdesc16


# 16bit GDT
.p2align 2                                # force 4 byte alignment
gdt16:
  SEG_NULLASM                               # null seg
  SEG_ASM16_1B(STA_X|STA_R, 0x0, 0xfffff)   # code seg
  SEG_ASM16_1B(STA_W      , 0x0, 0xfffff)   # data seg

gdtdesc16:
  .word   (gdtdesc16 - gdt16 - 1)           # sizeof(gdt) - 1
  .long   gdt      16                       # address gdt
