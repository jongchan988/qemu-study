#ifndef JC1904A_CPU_QOM_H
#define JC1904A_CPU_QOM_H

#include "hw/core/cpu.h"
#include "qom/object.h"

#define TYPE_JC1904A_CPU "jc1904a-cpu"
#define TYPE_JC1904B_CPU "jc1904b-cpu"

OBJECT_DECLARE_CPU_TYPE(JC1904ACPU, JC1904ACPUClass, JC1904A_CPU)

typedef struct JC1904ACPUDef JC1904ACPUDef;
JC1904ACPUClass {
    /*< private >*/
    CPUClass parent_class;

    /*< public >*/
    DeviceRealize parent_realize;
    DeviceReset parent_reset;

    JC1904ACPUDef* cpu_def;
};

#endif // JC1904A_CPU_QOM_H