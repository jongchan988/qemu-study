#ifndef AVR32A_CPU_QOM_H
#define AVR32A_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_AVR32A_CPU "avr32a-cpu"
#define TYPE_AVR32B_CPU "avr32b-cpu"

OBJECT_DECLARE_CPU_TYPE(AVR32ACPU, AVR32ACPUClass, AVR32A_CPU)

typedef struct AVR32ACPUDef AVR32ACPUDef;
AVR32ACPUClass {
    /*< private >*/
    CPUClass parent_class;

    /*< public >*/
    DeviceRealize parent_realize;
    DeviceReset parent_reset;

    AVR32ACPUDef* cpu_def;
};

#endif // AVR32A_CPU_QOM_H