#ifndef HW_AVR32_AVR32EXPC_H
#define HW_AVR32_AVR32EXPC_H
#include "target/avr32/cpu.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#define TYPE_AVR32EXP_MCU "AVR32EXP" // CPU
#define TYPE_AVR32EXPS_MCU "AVR32EXPS" //system on chip (SOC)

typedef struct AVR32EXPMcuState AVR32EXPMcuState;

DECLARE_INSTANCE_CHECKER(AVR32EXPMcuState, AVR32EXP_MCU, TYPE_AVR32EXP_MCU)

struct AVR32EXPMcuState {
    /*< public >*/
    SysBusDevice parent_boj;

     /*< public >*/
     // CPU,  part 2에서 구현함
     AVR32ACPU cpu;

     // 펌웨어를 포함하는 플래시 메모리
     MemoryRegion flash;
}
#endif // HW_AVR32_AVR32EXPC_H