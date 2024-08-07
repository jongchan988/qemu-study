#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/exec-all.h"
#include "cpu.h"
#include "disas/dis-asm.h"
#include "hw/core/tcg-cpu-ops.h"
#include "exec/address-spaces.h"
#include "exec/helper-proto.h"

static AVR32ACPU * cpu_self;
static bool first_reset = true;
static void avr32_cpu_disas_set_info(CPUState *cpu, disassemble_info *info)
{
    printf("[AVR32-DISAS] avr32_cpu_disas_set_info\n");
    //This was defined in the last article
    info->mach = bfd_arch_avr32;
}

//An init function, just like the one of the example micorcontroller
static void avr32a_cpu_init(Object* obj)
{
    //Generic CPU state from object
    CPUState *cs = CPU(obj);
    //Specific CPU from object
    AVR32ACPU *cpu = AVR32A_CPU(obj);
    //Specific state from CPU
    CPUAVR32AState *env = &cpu->env;

    cpu_set_cpustate_pointers(cpu);
    cs->env_ptr = env;
}

//Realize function, sets up the CPU
static void avr32_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState* cs = CPU(dev);
    cpu_self = AVR32A_CPU(cs);
    AVR32ACPUClass* acc = AVR32A_CPU_GET_CLASS(dev);
    Error *local_err = NULL;

    cpu_exec_realizefn(cs, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cs);
    cpu_reset(cs);

    acc->parent_realize(dev, errp);
}

//CPU reset function sets the CPU state to the default state that is
//defined in the instruction set manual.
static void avr32_cpu_reset(DeviceState *dev)
{
    CPUState *cs = CPU(dev);
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    AVR32ACPUClass* acc = AVR32A_CPU_GET_CLASS(dev);
    CPUAVR32AState* env = &cpu->env;

    env->isInInterrupt = 0;
    env->intlevel = 0;
    env->intsrc = -1;
    acc->parent_reset(dev);

    if(first_reset) {
        memset(env->r, 0, sizeof(env->r));
        memset(env->sflags, 1, sizeof(env->sflags));
        first_reset = false;
    }

    env->sr = 0;

    for(int i= 0; i< 32; i++){
        env->sflags[i] = 0;
    }
    env->sflags[16] = 1;
    env->sflags[21] = 1;
    env->sflags[22] = 1;


    for(int i= 0; i< AVR32A_SYS_REG; i++){
        env->sysr[i] = 0;
    }

    for(int i= 0; i< AVR32A_REG_PAGE_SIZE; i++){
        env->r[i] = 0;
    }
    //Start address of the execution. Notice, that this is the start of the flash memory address
    //from the microcontroller implementation.
    env->r[AVR32A_PC_REG] = 0xd0000000;
    env->r[AVR32A_LR_REG] = 0;
    env->r[AVR32A_SP_REG] = 0;
}

static ObjectClass* avr32_cpu_class_by_name(const char *cpu_model){
    ObjectClass *oc;
    oc = object_class_by_name(AVR32A_CPU_TYPE_NAME("AVR32EXPC"));
    return oc;
}

static bool avr32_cpu_has_work(CPUState *cs)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    CPUAVR32AState *env = &cpu->env;

    return (cs->interrupt_request & CPU_INTERRUPT_HARD) &&
           cpu_interrupts_enabled(env);
}

//When using the '-d cpu' paramater, QEMU executes this function after every translation block
//to give us th CPU state at the begining of the block.
static void avr32_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    //The CPU enviourment is used to access the content of the emulated registers.
    CPUAVR32AState *env = &cpu->env;

    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->r[AVR32A_PC_REG]);
    qemu_fprintf(f, "SP:    " TARGET_FMT_lx "\n", env->r[AVR32A_SP_REG]);
    qemu_fprintf(f, "LR:    " TARGET_FMT_lx "\n", env->r[AVR32A_LR_REG]);

    //Print general purpose registers.
    int i;
    for(i = 0;i < AVR32A_REG_PAGE_SIZE-3; ++i) {
        qemu_fprintf(f, "r%d:    " TARGET_FMT_lx "\n", i, env->r[i]);
    }

    //Print status register
    for(i= 0; i< 32; i++){
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n", avr32_cpu_sr_flag_names[i], env->sflags[i]);
    }
    qemu_fprintf(f, "\n");
}

static void avr32_cpu_set_pc(CPUState *cs, vaddr value)
{
    AVR32ACPU *cpu = AVR32A_CPU(cs);

    printf("[AVR32-CPU] avr32_cpu_set_pc, pc: %04lx\n", value);
    cpu->env.r[AVR32A_PC_REG] = value;
}

static bool avr32_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    //TODO: Later
    return false;
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps avr32_sysemu_ops = {
        .get_phys_page_debug = avr32_cpu_get_phys_page_debug,
};

//Here we map the above function to their purpose, so that QEMU knows when to execute them.
static const struct TCGCPUOps avr32_tcg_ops = {
        .initialize = avr32_tcg_init,
        .synchronize_from_tb = avr32_cpu_synchronize_from_tb,
        .cpu_exec_interrupt = avr32_cpu_exec_interrupt,
        .tlb_fill = avr32_cpu_tlb_fill,
        .do_interrupt = avr32_cpu_do_interrupt,
};

void avr32_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb){
    AVR32ACPU *cpu = AVR32A_CPU(cs);
    cpu->env.r[AVR32A_PC_REG] = tb->pc;
}

//The class init function has the same purpose than the one in the example board.
static void avr32a_cpu_class_init(ObjectClass *oc, void *data)
{
    printf("CPU-INIT!\n");
    AVR32ACPUClass *acc = AVR32A_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_set_parent_realize(dc, avr32_cpu_realizefn, &acc->parent_realize);
    device_class_set_parent_reset(dc, avr32_cpu_reset, &acc->parent_reset);

    cc->class_by_name = avr32_cpu_class_by_name;

    cc->has_work = avr32_cpu_has_work;
    cc->dump_state = avr32_cpu_dump_state;
    cc->set_pc = avr32_cpu_set_pc;
    cc->memory_rw_debug = avr32_cpu_memory_rw_debug;
    cc->sysemu_ops = &avr32_sysemu_ops;
    cc->disas_set_info = avr32_cpu_disas_set_info;
    cc->tcg_ops = &avr32_tcg_ops;
}

static const AVR32ACPUDef avr32_cpu_defs[] = {
        {
                //This is the TYPE that was defined in avr32emp.h in the /hw folder
                //Here we tell QEMU that this CPU logic should be used with the example board/microncontroller
                .name = "AVR32EXPC",
                .parent_microarch = TYPE_AVR32A_CPU,
                .core_type = AVR32_EXP,
                .series_type = AVR32_EXP_S,
                .clock_speed = 66 * 1000 * 1000, /* 66 MHz */
                .audio = false,
                .aes = false
        }
};

static void avr32_cpu_cpudef_class_init(ObjectClass *oc, void *data)
{
    AVR32ACPUClass* acc = AVR32A_CPU_CLASS(oc);
    acc->cpu_def = data;
}

static char* avr32_cpu_type_name(const char* model_name)
{
    return g_strdup_printf(AVR32A_CPU_TYPE_NAME("%s"), model_name);
}

static void avr32_register_cpudef_type(const struct AVR32ACPUDef* def)
{
    char* cpu_model_name = avr32_cpu_type_name(def->name);
    TypeInfo ti = {
            .name = cpu_model_name,
            .parent = def->parent_microarch,
            .class_init = avr32_cpu_cpudef_class_init,
            .class_data = (void *)def,
    };

    type_register(&ti);
    g_free(cpu_model_name);
}

// We also have type definitions.
static const TypeInfo avr32_cpu_arch_types[] = {
        {
                .name = TYPE_AVR32A_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(AVR32ACPU),
                .instance_init = avr32a_cpu_init,
                .abstract = true,
                .class_size = sizeof(AVR32ACPUClass),
                .class_init = avr32a_cpu_class_init,
        },
        {
                .name = TYPE_AVR32B_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(AVR32ACPU),
                .instance_init = avr32b_cpu_init,
                .abstract = true,
                .class_size = sizeof(AVR32ACPUClass),
                .class_init = avr32b_cpu_class_init,
        }
};

static void avr32_cpu_register_types(void)
{
    int i;

    type_register_static_array(avr32_cpu_arch_types, 2);
    for (i = 0; i < ARRAY_SIZE(avr32_cpu_defs); i++) {
        avr32_register_cpudef_type(&avr32_cpu_defs[i]);
    }
}

type_init(avr32_cpu_register_types)