#ifndef HW_JC1904_BOOT_H
#define HW_JC1904_BOOT_H

#include "hw/boards.h"
#include "cpu.h"

bool jc1904_load_firmware(JC1904ACPU *cpu, MachineState *ms, MemoryRegion *mr, const char *firmware);

#endif