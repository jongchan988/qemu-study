#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/qemu-print.h"
#include "exec/exec-all.h"
#include "cpu.h"
#include "disas/dis-asm.h"
#include "hw/core/tcg-cpu-ops.h"
#include "exec/address-spaces.h"
#include "exec/helper-proto.h"
#include "helper.c"

static JC1904ACPU * cpu_self;
static bool first_reset = true;
static void jc1904_cpu_disas_set_info(CPUState *cpu, disassemble_info *info)
{
    printf("[JC1904-DISAS] jc1904_cpu_disas_set_info\n");
    //This was defined in the last article
    info->mach = bfd_arch_jc1904;
}

//An init function, just like the one of the example micorcontroller
static void jc1904a_cpu_init(Object* obj)
{
    //Generic CPU state from object
    CPUState *cs = CPU(obj);
    //Specific CPU from object
    JC1904ACPU *cpu = JC1904A_CPU(obj);
    //Specific state from CPU
    CPUJC1904AState *env = &cpu->env;

    cpu_set_cpustate_pointers(cpu);
    cs->env_ptr = env;
}

static void jc1904b_cpu_init(Object* obj)
{
    // TODO
}

//Realize function, sets up the CPU
static void jc1904_cpu_realizefn(DeviceState *dev, Error **errp)
{
    CPUState* cs = CPU(dev);
    cpu_self = JC1904A_CPU(cs);
    JC1904ACPUClass* acc = JC1904A_CPU_GET_CLASS(dev);
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
static void jc1904_cpu_reset(DeviceState *dev)
{
    CPUState *cs = CPU(dev);
    JC1904ACPU *cpu = JC1904A_CPU(cs);
    JC1904ACPUClass* acc = JC1904A_CPU_GET_CLASS(dev);
    CPUJC1904AState* env = &cpu->env;

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


    for(int i= 0; i< JC1904A_SYS_REG; i++){
        env->sysr[i] = 0;
    }

    for(int i= 0; i< JC1904A_REG_PAGE_SIZE; i++){
        env->r[i] = 0;
    }
    //Start address of the execution. Notice, that this is the start of the flash memory address
    //from the microcontroller implementation.
    env->r[JC1904A_PC_REG] = 0xd0000000;
    env->r[JC1904A_LR_REG] = 0;
    env->r[JC1904A_SP_REG] = 0;
}

static ObjectClass* jc1904_cpu_class_by_name(const char *cpu_model){
    ObjectClass *oc;
    oc = object_class_by_name(JC1904A_CPU_TYPE_NAME("JC1904EXPC"));
    return oc;
}

static bool jc1904_cpu_has_work(CPUState *cs)
{
    JC1904ACPU *cpu = JC1904A_CPU(cs);
    CPUJC1904AState *env = &cpu->env;

    return (cs->interrupt_request & CPU_INTERRUPT_HARD) &&
           cpu_interrupts_enabled(env);
}

//When using the '-d cpu' paramater, QEMU executes this function after every translation block
//to give us th CPU state at the begining of the block.
static void jc1904_cpu_dump_state(CPUState *cs, FILE *f, int flags)
{
    JC1904ACPU *cpu = JC1904A_CPU(cs);
    //The CPU enviourment is used to access the content of the emulated registers.
    CPUJC1904AState *env = &cpu->env;

    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->r[JC1904A_PC_REG]);
    qemu_fprintf(f, "SP:    " TARGET_FMT_lx "\n", env->r[JC1904A_SP_REG]);
    qemu_fprintf(f, "LR:    " TARGET_FMT_lx "\n", env->r[JC1904A_LR_REG]);

    //Print general purpose registers.
    int i;
    for(i = 0;i < JC1904A_REG_PAGE_SIZE-3; ++i) {
        qemu_fprintf(f, "r%d:    " TARGET_FMT_lx "\n", i, env->r[i]);
    }

    //Print status register
    for(i= 0; i< 32; i++){
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n", jc1904_cpu_sr_flag_names[i], env->sflags[i]);
    }
    qemu_fprintf(f, "\n");
}

static void jc1904_cpu_set_pc(CPUState *cs, vaddr value)
{
    JC1904ACPU *cpu = JC1904A_CPU(cs);

    printf("[JC1904-CPU] jc1904_cpu_set_pc, pc: %04lx\n", value);
    cpu->env.r[JC1904A_PC_REG] = value;
}

static bool jc1904_cpu_exec_interrupt(CPUState *cs, int interrupt_request)
{
    //TODO: Later
    return false;
}

#include "hw/core/sysemu-cpu-ops.h"

static const struct SysemuCPUOps jc1904_sysemu_ops = {
        .get_phys_page_debug = jc1904_cpu_get_phys_page_debug,
};

//Here we map the above function to their purpose, so that QEMU knows when to execute them.
static const struct TCGCPUOps jc1904_tcg_ops = {
        .initialize = jc1904_tcg_init,
        .synchronize_from_tb = jc1904_cpu_synchronize_from_tb,
        .cpu_exec_interrupt = jc1904_cpu_exec_interrupt,
        .tlb_fill = jc1904_cpu_tlb_fill,
        .do_interrupt = jc1904_cpu_do_interrupt,
};

void jc1904_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb){
    JC1904ACPU *cpu = JC1904A_CPU(cs);
    cpu->env.r[JC1904A_PC_REG] = tb->pc;
}

//The class init function has the same purpose than the one in the example board.
static void jc1904a_cpu_class_init(ObjectClass *oc, void *data)
{
    printf("CPU-INIT!\n");
    JC1904ACPUClass *acc = JC1904A_CPU_CLASS(oc);
    CPUClass *cc = CPU_CLASS(oc);
    DeviceClass *dc = DEVICE_CLASS(oc);

    device_class_set_parent_realize(dc, jc1904_cpu_realizefn, &acc->parent_realize);
    device_class_set_parent_reset(dc, jc1904_cpu_reset, &acc->parent_reset);

    cc->class_by_name = jc1904_cpu_class_by_name;

    cc->has_work = jc1904_cpu_has_work;
    cc->dump_state = jc1904_cpu_dump_state;
    cc->set_pc = jc1904_cpu_set_pc;
    cc->memory_rw_debug = jc1904_cpu_memory_rw_debug;
    cc->sysemu_ops = &jc1904_sysemu_ops;
    cc->disas_set_info = jc1904_cpu_disas_set_info;
    cc->tcg_ops = &jc1904_tcg_ops;
}

static void jc1904b_cpu_class_init(ObjectClass *oc, void *data)
{
    // TODO
}
static const JC1904ACPUDef jc1904_cpu_defs[] = {
        {
                //This is the TYPE that was defined in jc1904emp.h in the /hw folder
                //Here we tell QEMU that this CPU logic should be used with the example board/microncontroller
                .name = "JC1904EXPC",
                .parent_microarch = TYPE_JC1904A_CPU,
                .core_type = JC1904_EXP,
                .series_type = JC1904_EXP_S,
                .clock_speed = 66 * 1000 * 1000, /* 66 MHz */
                .audio = false,
                .aes = false
        }
};

static void jc1904_cpu_cpudef_class_init(ObjectClass *oc, void *data)
{
    JC1904ACPUClass* acc = JC1904A_CPU_CLASS(oc);
    acc->cpu_def = data;
}

static char* jc1904_cpu_type_name(const char* model_name)
{
    return g_strdup_printf(JC1904A_CPU_TYPE_NAME("%s"), model_name);
}

static void jc1904_register_cpudef_type(const struct JC1904ACPUDef* def)
{
    char* cpu_model_name = jc1904_cpu_type_name(def->name);
    TypeInfo ti = {
            .name = cpu_model_name,
            .parent = def->parent_microarch,
            .class_init = jc1904_cpu_cpudef_class_init,
            .class_data = (void *)def,
    };

    type_register(&ti);
    g_free(cpu_model_name);
}

// We also have type definitions.
static const TypeInfo jc1904_cpu_arch_types[] = {
        {
                .name = TYPE_JC1904A_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(JC1904ACPU),
                .instance_init = jc1904a_cpu_init,
                .abstract = true,
                .class_size = sizeof(JC1904ACPUClass),
                .class_init = jc1904a_cpu_class_init,
        },
        {
                .name = TYPE_JC1904B_CPU,
                .parent = TYPE_CPU,
                .instance_size = sizeof(JC1904ACPU),
                .instance_init = jc1904b_cpu_init,
                .abstract = true,
                .class_size = sizeof(JC1904ACPUClass),
                .class_init = jc1904b_cpu_class_init,
        }
};

static void jc1904_cpu_register_types(void)
{
    int i;

    type_register_static_array(jc1904_cpu_arch_types, 2);
    for (i = 0; i < ARRAY_SIZE(jc1904_cpu_defs); i++) {
        jc1904_register_cpudef_type(&jc1904_cpu_defs[i]);
    }
}

type_init(jc1904_cpu_register_types)