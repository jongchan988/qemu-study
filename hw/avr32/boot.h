#ifndef HW_AVR32_BOOT_H
#define HW_AVR32_BOOT_H

#include "hw/boards.h"
#include "cpu.h"

bool avr32_load_firmware(AVR32ACPU *cpu, MachineState *ms, MemoryRegion *mr, const char *firmware);

#endif