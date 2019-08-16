#if !defined(__INCLUDE_XV6_DRIVERS_LAPIC_H)
#define __INCLUDE_XV6_DRIVERS_LAPIC_H

#include "xv6/types.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// Local APIC registers, divided by 4 for use as uint[] indices.
#define ID (0x0020 / 4)  // ID
#define VER (0x0030 / 4) // Version
#define TPR (0x0080 / 4) // Task Priority
#define EOI (0x00B0 / 4) // EOI
#define SVR (0x00F0 / 4) // Spurious Interrupt Vector
    enum SVR_FLAGS
    {
        ENABLE = 0x00000100, // Unit Enable
    };
#define ESR (0x0280 / 4)   // Error Status
#define ICRLO (0x0300 / 4) // Interrupt Command
    enum ICRLO_FLAGS
    {
        INIT = 0x00000500,    // INIT/RESET
        STARTUP = 0x00000600, // Startup IPI
        DELIVS = 0x00001000,  // Delivery status
        ASSERT = 0x00004000,  // Assert interrupt (vs deassert)
        DEASSERT = 0x00000000,
        LEVEL = 0x00008000, // Level triggered
        BCAST = 0x00080000, // Send to all APICs, including self.
        BUSY = 0x00001000,
        FIXED = 0x00000000,
    };
#define ICRHI (0x0310 / 4) // Interrupt Command [63:32]
#define TIMER (0x0320 / 4) // Local Vector Table 0 (TIMER)
    enum TIMER_FLAGS
    {
        X1 = 0x0000000B,       // divide counts by 1
        PERIODIC = 0x00020000, // Periodic
    };
#define PCINT (0x0340 / 4) // Performance Counter LVT
#define LINT0 (0x0350 / 4) // Local Vector Table 1 (LINT0)
#define LINT1 (0x0360 / 4) // Local Vector Table 2 (LINT1)
#define ERROR (0x0370 / 4) // Local Vector Table 3 (ERROR)
    enum LINT_FLAGS
    {
        MASKED = 0x00010000, // Interrupt masked
    };
#define TICR (0x0380 / 4) // Timer Initial Count
#define TCCR (0x0390 / 4) // Timer Current Count
#define TDCR (0x03E0 / 4) // Timer Divide Configuration

    // lapic.c
    void cmostime(struct rtcdate *r);
    int lapicid(void);
    extern volatile uint *lapic;
    void lapiceoi(void);
    void lapicinit(void);
    void lapicstartap(uchar, uint);
    void microdelay(int);

#if defined(__cplusplus)
}
#endif

#endif // __INCLUDE_XV6_DRIVERS_LAPIC_H
