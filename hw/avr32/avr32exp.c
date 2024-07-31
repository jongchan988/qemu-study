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
#include "avr32exp.h"

struct AVR32EXPMcuClass {
    /*< private >*/
    SysBusDeviceClass parent_class;

    /*< public >*/
    const char *cpu_type;

    size_t flash_size;
};
typedef struct AVR32EXPMcuClass AVR32EXPMcuClass;
DECLARE_CLASS_CHECKERS(AVR32EXPMcuClass, AVR32EXP_MCU, TYPE_AVR32EXP_MCU)

// realize 함수 정의
static void avr32exp_realize(DeviceState *dev, Error **errp)
{
    // AVR32EXPMcuState 인스턴스 생성
    AVR32EXPMcuState *s = AVR32EXP_MCU(dev);

    // AVR32EXPMcuClass 인스턴스 생성
    const AVR32EXPMcuClass *mc = AVR32EXP_MCU_GET_CLASS(dev);

    // AVR32 CPU정의
    object_initialize_child(OBJECT(dev), "cpu", &s->cpu, mc->cpu_type);

    // CPU 객체를 realized 설정
    object_property_set_bool(OBJECT(&s->cpu), "realized", true, &error_abort);

    // 플래시 메모리 영역을 초기화
    memory_region_init_rom(&s->flash, OBJECT(dev), "flash", mc->flash_size, &error_fatal);

    // 메모리 영역의 시작 주소를 설정 (offset 에 subregion 을 추가)
    memory_region_add_subregion(get_system_memory(), 0xd0000000, &s->flash);
}


static void avr32exp_class_init(ObjectClass *oc, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(oc);

    dc->realize = avr32exp_realize;
    dc->user_creatable = false;
}

static void avr32exps_class_init(ObjectClass *oc, void *data){

    AVR32EXPMcuClass* avr32exp = AVR32EXP_MCU_CLASS(oc);

    avr32exp->cpu_type = AVR32A_CPU_TYPE_NAME("AVR32EXPC");
    avr32exp->flash_size = 1024 * KiB;
}

static const TypeInfo avr32exp_mcu_types[] = {
        //CPU: 단일 처리 장치
        {
                .name           = TYPE_AVR32EXPS_MCU,
                .parent         = TYPE_AVR32EXP_MCU,
                .class_init     = avr32exps_class_init,
        },
        // system on chip (SOC): CPU뿐만 아니라 메모리, 입출력 포트, GPU, 네트워크 모듈 등 여러 기능이 하나의 칩에 통합된 시스템
        {
                .name           = TYPE_AVR32EXP_MCU,
                .parent         = TYPE_SYS_BUS_DEVICE,
                .instance_size  = sizeof(AVR32EXPMcuState),
                .class_size     = sizeof(AVR32EXPMcuClass),
                .class_init     = avr32exp_class_init,
                .abstract       = true,
        }
};

DEFINE_TYPES(avr32exp_mcu_types)