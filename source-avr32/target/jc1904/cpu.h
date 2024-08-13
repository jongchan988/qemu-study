#ifndef QEMU_JC1904A_CPU_H
#define QEMU_JC1904A_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"

#define JC1904_EXP 0x100
#define JC1904_EXP_S    JC1904_EXP | 0x30

#define JC1904A_REG_PAGE_SIZE 16 // r0 - r12 + LR + SP + PC
#define JC1904A_PC_REG 15
#define JC1904A_LR_REG 14
#define JC1904A_SP_REG 13
// 시스템 레지스터 개수
#define JC1904A_SYS_REG 256

// CPU 정의
struct JC1904ACPUDef {
    const char* name;
    const char* parent_microarch; // JC1904A or JC1904B, two variants of the JC1904 instruction set.
    size_t core_type;
    size_t series_type;
    size_t clock_speed;
    bool audio; // MCUs with 'AU' suffix
    bool aes;  // MCUs with 'S' suffix
};
#define JC1904A_CPU_TYPE_SUFFIX "-" TYPE_JC1904A_CPU
#define JC1904A_CPU_TYPE_NAME(name) (name JC1904A_CPU_TYPE_SUFFIX)
#define CPU_RESOLVING_TYPE TYPE_JC1904A_CPU
// Global Interrupt Mask
#define JC1904_GM_FLAG(sr)       (sr & 0x10000) >> 16

//JC1904 instructions are 16 or 32 bits long. 32 bit instrucitons ALWAYS start with b'111'
#define JC1904_EXTENDED_INSTR_FORMAT_MASK 0b1110000000000000
#define JC1904_EXTENDED_INSTR_FORMAT_MASK_LE 0b11100000

static const char jc1904_cpu_r_names[JC1904A_REG_PAGE_SIZE][8] = {
        "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",
        "r8",  "r9",  "r10",  "r11",  "r12",  "SP",  "LR",  "PC"
};

static const char jc1904_cpu_sr_flag_names[32][8] = {
        "sregC", "sregZ", "sregN", "sregV", "sregQ", "sregL",
        "sreg6", "sreg7", "sreg8", "sreg9", "sreg10", "sreg11","sreg12","sreg13",
        "sregT", "sregR",
        "sregGM","sregI0M","sregI1M", "sregI2M", "sregI3M","sregEM","sregM0",
        "sregM1",
        "sregM2", "sreg25","sregD","sregDM","sregJ","sregH", "sreg30", "sregSS"
};

typedef struct CPUArchState {
    // Status Register
    uint sr;
    uint pc_w;

    uint32_t sflags[32];

    // Register File Registers
    uint32_t r[JC1904A_REG_PAGE_SIZE]; // 32 bits each

    //System registers
    uint32_t sysr[JC1904A_SYS_REG];

    //interrupt source
    int intsrc;
    int intlevel;
    uint64_t autovector;
    int isInInterrupt;

} CPUJC1904AState;

struct ArchCPU {
    /*< private >*/
    CPUState parent_obj;
    /*< public >*/

    CPUNegativeOffsetState neg;
    CPUJC1904AState env;
};

int jc1904_print_insn(bfd_vma addr, disassemble_info *info);


static inline int cpu_interrupts_enabled(CPUJC1904AState* env)
{
    return JC1904_GM_FLAG(env->sr);
}

static inline int cpu_mmu_index(CPUJC1904AState *env, bool ifetch)
{
    // There is only one MMU, so that should be fine
    return 0;
}

static inline void cpu_get_tb_cpu_state(CPUJC1904AState *env, target_ulong *pc,
                                        target_ulong *cs_base, uint32_t *pflags)
{
    *pc = env->r[JC1904A_PC_REG];
    *cs_base = 0;
    *pflags = 0;
}

void jc1904_tcg_init(void);
bool jc1904_cpu_tlb_fill(CPUState *cs, vaddr address, int size,
                        MMUAccessType access_type, int mmu_idx,
                        bool probe, uintptr_t retaddr);
void jc1904_cpu_do_interrupt(CPUState *cpu);
void jc1904_cpu_set_int(void *opaque, int irq, int level);
hwaddr jc1904_cpu_get_phys_page_debug(CPUState *cs, vaddr addr);
int jc1904_cpu_memory_rw_debug(CPUState *cs, vaddr addr, uint8_t *buf, int len, bool is_write);

void jc1904_cpu_synchronize_from_tb(CPUState *cs, const TranslationBlock *tb);

#include "exec/cpu-all.h"

#endif /* !defined (QEMU_JC1904A_CPU_H) */