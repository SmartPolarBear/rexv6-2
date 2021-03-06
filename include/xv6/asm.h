#if !defined(__INCLUDE_XV6_ASM_H)
#define __INCLUDE_XV6_ASM_H
#if defined(__cplusplus)
extern "C"
{
#endif
    //
    // assembler macros to create x86 segments
    //

#define SEG_NULLASM \
    .word 0, 0;     \
    .byte 0, 0, 0, 0

// The 0b11000000 means the limit is in 4096-byte units
// and (for executable segments) 32-bit mode.
#define SEG_ASM(type, base, lim)                    \
    .word(((lim) >> 12) & 0xffff), ((base)&0xffff); \
    .byte(((base) >> 16) & 0xff), (0x90 | (type)),  \
        (0b11000000 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

// The 0b00000000 means the limit is in 1-byte units
// and (for executable segments) 16-bit mode.
#define SEG_ASM16_1B(type, base, lim)               \
    .word(((lim) >> 12) & 0xffff), ((base)&0xffff); \
    .byte(((base) >> 16) & 0xff), (0x90 | (type)),  \
        (0b00000000 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)

#define STA_X 0x8 // Executable segment
#define STA_W 0x2 // Writeable (non-executable segments)
#define STA_R 0x2 // Readable (executable segments)
#if defined(__cplusplus)
}
#endif
#endif // __INCLUDE_XV6_ASM_H
