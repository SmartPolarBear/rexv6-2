#include "xv6/traps.h"
#include "drivers/timer/timer.h"

//Divide configuration register
//------------------------------------
//|  3   |   2   |   1     |    0    |
//------------------------------------
//|      | MUST 0|         |         |
//------------------------------------
// Divide Value (bits 0, 1 and 3)000: Divide by 2
// 001: Divide by 4
// 010: Divide by 8
// 011: Divide by 16100: Divide by 32
// 101: Divide by 64
// 110: Divide by 128
// 111: Divide by 1

enum class DIVISION : uint
{
    X1 = 0b1011, // divide counts by 1
    X2 = 0b0000,
    X4 = 0b1000,
    X8 = 0b0010,
    X16 = 0b1010,
    X32 = 0b0001,
    X64 = 0b1001,
    X128 = 0b0011,
};

//LVT Bits[18:17]
enum class TIMER_MODE : uint
{
    ONESHOT = 0b000'0000'0000'0000'0000,      // One-shot
    PERIODIC = 0b010'0000'0000'0000'0000,     // Periodic
    TSC_DEADLINE = 0b100'0000'0000'0000'0000, // TSC-Deadline
    RESERVED = 0b110'0000'0000'0000'0000,     // Reserved
};

#define TICR (0x0380 / 4) // Timer Initial Count
#define TCCR (0x0390 / 4) // Timer Current Count
#define TDCR (0x03E0 / 4) // Timer Divide Configuration

extern "C" void lapictimer_init(void)
{
    // OLD IMPLEMENTATION
    // The timer repeatedly counts down at bus frequency
    // from lapic[TICR] and then issues an interrupt.
    // If xv6 cared more about precise timekeeping,
    // TICR would be calibrated using an external time source.
    //          lapicw(TDCR, static_cast<uint>(DIVISION::X1));
    //          lapicw(TIMER, static_cast<uint>(TIMER_MODE::PERIODIC) | (T_IRQ0 + IRQ_TIMER));
    //          lapicw(TICR, 10000000);

    lapicw(TDCR, static_cast<uint>(DIVISION::X1));
    lapicw(TIMER, static_cast<uint>(TIMER_MODE::PERIODIC) | (T_IRQ0 + IRQ_TIMER));
    lapicw(TICR, 10000000);
}