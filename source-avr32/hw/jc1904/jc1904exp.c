#include "qemu/osdep.h"
#include "qemu/module.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "exec/memory.h"
#include "exec/address-spaces.h"
#include "sysemu/sysemu.h"
#include "hw/qdev-properties.h"
#include "hw/sysbus.h"
#include "qom/object.h"
#include "hw/misc/unimp.h"
#include "jc1904exp.h"

struct JC1904EXPMcuClass {
    /*< private >*/
    SysBusDeviceClass parent_class;

    /*< public >*/
    const char *cpu_type;

    size_t flash_size;
};
typedef struct JC1904EXPMcuClass JC1904EXPMcuClass;
DECLARE_CLASS_CHECKERS(JC1904EXPMcuClass, JC1904EXP_MCU, TYPE_JC1904EXP_MCU)

// realize 함수 정의
static void jc1904exp_realize(DeviceState *dev, Error **errp)
{
    // JC1904EXPMcuState 인스턴스 생성
    JC1904EXPMcuState *s = JC1904EXP_MCU(dev);

    // JC1904EXPMcuClass 인스턴스 생성
    const JC1904EXPMcuClass *mc = JC1904EXP_MCU_GET_CLASS(dev);

    // JC1904 CPU정의
    object_initialize_child(OBJECT(dev), "cpu", &s->cpu, mc->cpu_type);

    // CPU 객체를 realized 설정
    object_property_set_bool(OBJECT(&s->cpu), "realized", true, &error_abort);

    // 플래시 메모리 영역을 초기화
    memory_region_init_rom(&s->flash, OBJECT(dev), "flash", mc->flash_size, &error_fatal);

    // 메모리 영역의 시작 주소를 설정 (offset 에 subregion 을 추가)
    memory_region_add_subregion(get_system_memory(), 0xd0000000, &s->flash);
}


static void jc1904exp_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = jc1904exp_realize;
    dc->user_creatable = false;
}

static void jc1904exps_class_init(ObjectClass *oc, void *data){

    JC1904EXPMcuClass* jc1904exp = JC1904EXP_MCU_CLASS(oc);

    jc1904exp->cpu_type = JC1904A_CPU_TYPE_NAME("JC1904EXPC");
    jc1904exp->flash_size = 1024 * KiB;
}

static const TypeInfo jc1904exp_mcu_types[] = {
        //CPU: 단일 처리 장치
        {
                .name           = TYPE_JC1904EXPS_MCU,
                .parent         = TYPE_JC1904EXP_MCU,
                .class_init     = jc1904exps_class_init,
        },
        // system on chip (SOC): CPU뿐만 아니라 메모리, 입출력 포트, GPU, 네트워크 모듈 등 여러 기능이 하나의 칩에 통합된 시스템
        {
                .name           = TYPE_JC1904EXP_MCU,
                .parent         = TYPE_SYS_BUS_DEVICE,
                .instance_size  = sizeof(JC1904EXPMcuState),
                .class_size     = sizeof(JC1904EXPMcuClass),
                .class_init     = jc1904exp_class_init,
                .abstract       = true,
        }
};

DEFINE_TYPES(jc1904exp_mcu_types)