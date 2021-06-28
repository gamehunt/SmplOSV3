#ifndef K_SMPLOS_PIT_H
#define K_SMPLOS_PIT_H

#include <stdint.h>
#include <util.h>

#define PIT_A 0x40
#define PIT_B 0x41
#define PIT_C 0x42
#define PIT_CONTROL 0x43

#define PIT_MASK 0xFF
#define PIT_SCALE 1193180
#define PIT_SET 0x34

#define TIMER_IRQ 0

#define MAX_PIT_LISTENERS 65536

CH_START

void pit_phase(int hz);

CH_END

#endif