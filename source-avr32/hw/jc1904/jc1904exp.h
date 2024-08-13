#ifndef HW_JC1904_JC1904EXPC_H
#define HW_JC1904_JC1904EXPC_H
#include "target/jc1904/cpu.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#define TYPE_JC1904EXP_MCU "JC1904EXP" // CPU
#define TYPE_JC1904EXPS_MCU "JC1904EXPS" //system on chip (SOC)

typedef struct JC1904EXPMcuState JC1904EXPMcuState;

DECLARE_INSTANCE_CHECKER(JC1904EXPMcuState, JC1904EXP_MCU, TYPE_JC1904EXP_MCU)

struct JC1904EXPMcuState {
    /*< public >*/
    SysBusDevice parent_boj;

     /*< public >*/
     // CPU,  part 2에서 구현함
     JC1904ACPU cpu;

     // 펌웨어를 포함하는 플래시 메모리
     MemoryRegion flash;
}
#endif // HW_JC1904_JC1904EXPC_H