#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "avr32exp.h"
#include "boot.h"
#include "qom/object.h"
#include "hw/boards.h"

// VR32 바이너리 이미지를 실행할 수 있는 하드웨어 보드
struct AVR32ExampleBoardMachineState {
    // 에뮬레이션 중 보드의 상태
    MachineState parent_obj;
    // 마이크로컨트롤러의 상태
    AVR32EXPMcuState mcu;
};
typedef struct AVR32ExampleBoardMachineState AVR32ExampleBoardMachineState;

struct AVR32ExampleBoardMachineClass {
    MachineClass parent_class;
};

// 보드를 식별하는데 필요한 정의 추가
#define TYPE_AVR32EXAMPLE_BOARD_BASE_MACHINE MACHINE_TYPE_NAME("avr32example-board-base")
#define TYPE_AVR32EXAMPLE_BOARD_MACHINE MACHINE_TYPE_NAME("avr32example-board")

DECLARE_OBJ_CHECKERS(AVR32ExampleBoardMachineState, AVR32ExampleBoardMachineClass,
        AVR32EXAMPLE_BOARD_MACHINE, TYPE_AVR32EXAMPLE_BOARD_MACHINE)

// 머신의 새 인스턴스가 생성되면 QEMU는 초기화 함수를 호출함
// 해당 함수 내부에서 MachineState, 모든 메모리 영역을 생성해야 하며 다른 장치를 머신에 연결해야 함
static void avr32example_board_init(MachineState *machine)
{
    // 일반적인 MachineState를 특정 MachineState로 만듬
    AVR32ExampleBoardMachineState* m_state = AVR32EXAMPLE_BOARD_MACHINE(machine);

    // 마이크로컨트롤러를 초기화
    object_initialize_child(OBJECT(machine), "mcu", &m_state->mcu, TYPE_AVR32EXPS_MCU);

    // QEMU의 SYSBUS를 통해 연결
    sysbus_realize(SYS_BUS_DEVICE(&m_state->mcu), &error_abort);

    // 로드 함수를 사용하여 펌웨어 파일을 로드 (boot.c에서 구현)
    if (machine->firmware) {
        if (!avr32_load_firmware(&m_state->mcu.cpu, machine,
                                 &m_state->mcu.flash, machine->firmware)) {
            exit(1);
        }
    }
}

// QEMU에 의해 객체가 전달
static void avr32example_board_class_init(ObjectClass *oc, void *data)
{
    // 객체로부터 가져온 일반적인 머신 클래스
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "AVR32 예제 보드";
    mc->alias = "avr32example-board";
    mc->init = avr32example_board_init;
    mc->default_cpus = 1;
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    // 미디어 드라이브 존재하지 않는 여부
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    // 스레드도 사용하지 않는 여부
    mc->no_parallel = 1;
}

static const TypeInfo avr32example_board_machine_types[] = {
        {
                .name           = TYPE_AVR32EXAMPLE_BOARD_MACHINE,
                // QEMU 기본 머신을 parent로 지정
                .parent         = TYPE_MACHINE,
                .instance_size  = sizeof(AVR32ExampleBoardMachineState),
                .class_size     = sizeof(AVR32ExampleBoardMachineClass),
                // 클래스 초기화 함수를 등록
                .class_init     = avr32example_board_class_init,
        }
};
DEFINE_TYPES(avr32example_board_machine_types)
