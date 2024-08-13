#include "qemu/osdep.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "jc1904exp.h"
#include "boot.h"
#include "qom/object.h"
#include "hw/boards.h"

struct JC1904ExampleBoardMachineState {
    MachineState parent_obj;
    JC1904EXPMcuState mcu;
};
typedef struct JC1904ExampleBoardMachineState JC1904ExampleBoardMachineState;

struct JC1904ExampleBoardMachineClass {
    MachineClass parent_class;
};

#define TYPE_JC1904EXAMPLE_BOARD_BASE_MACHINE MACHINE_TYPE_NAME("jc1904example-board-base")
#define TYPE_JC1904EXAMPLE_BOARD_MACHINE MACHINE_TYPE_NAME("jc1904example-board")

DECLARE_OBJ_CHECKERS(JC1904ExampleBoardMachineState, JC1904ExampleBoardMachineClass,
        JC1904EXAMPLE_BOARD_MACHINE, TYPE_JC1904EXAMPLE_BOARD_MACHINE)

//The generic MachineState is passed by QEMU
static void jc1904example_board_init(MachineState *machine)
{
    //Make a specific MachineState out of the generic one
    JC1904ExampleBoardMachineState* m_state = JC1904EXAMPLE_BOARD_MACHINE(machine);

    //We initialize the mocrocontroller that is part of the board
    object_initialize_child(OBJECT(machine), "mcu", &m_state->mcu, TYPE_JC1904EXPS_MCU);
    //And we connect it via QEMU's SYSBUS.
    sysbus_realize(SYS_BUS_DEVICE(&m_state->mcu), &error_abort);

    //Here we load the firmware file with a load function that we will implment in boot.c
    if (machine->firmware) {
        if (!jc1904_load_firmware(&m_state->mcu.cpu, machine,
                                 &m_state->mcu.flash, machine->firmware)) {
            exit(1);
        }
    }
}

//Generic Objectc is passed by QEMU
static void jc1904example_board_class_init(ObjectClass *oc, void *data)
{
    //The generic machine class from object
    MachineClass *mc = MACHINE_CLASS(oc);
    mc->desc = "JC1904 Example Board";
    mc->alias = "jc1904example-board";

    //Notice that we tell QEMU what function is used to initialize our board here.
    mc->init = jc1904example_board_init;
    mc->default_cpus = 1;
    mc->min_cpus = mc->default_cpus;
    mc->max_cpus = mc->default_cpus;
    // Our board does not have any media drive
    mc->no_floppy = 1;
    mc->no_cdrom = 1;
    //We also will not have threads
    mc->no_parallel = 1;
}

static const TypeInfo jc1904example_board_machine_types[] = {
        {
                                //Notice that this is the TYPE that we defined above.
                .name           = TYPE_JC1904EXAMPLE_BOARD_MACHINE,
                                //Our machine is a direct child of QEMU generic machine
                .parent         = TYPE_MACHINE,
                .instance_size  = sizeof(JC1904ExampleBoardMachineState),
                .class_size     = sizeof(JC1904ExampleBoardMachineClass),
                //We need to registers the class inti function
                .class_init     = jc1904example_board_class_init,
        }
};
DEFINE_TYPES(jc1904example_board_machine_types)