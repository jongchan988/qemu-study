#include "qemu/osdep.h"
#include "cpu.h"

#define REG(x) jc1904_cpu_r_names[x]

#define INSN(opcode, mnemonic, format, ...)

typedef struct DisasContext {
    // Provided by QEMU
    disassemble_info *dis;

    uint32_t addr;
    uint32_t pc;

    uint8_t bytes[4];
} DisasContext;


INSN(ADD_f1,    ADD,   "%s, %s",                            REG(a->rd), REG(a->rs))
INSN(ADD_f2,    ADC,   "%s, %s, %s",                        REG(a->rd), REG(a->rx), REG(a->ry))

INSN(BR_f1, BR, "cond3: %d, disp: [0x%04x]", a->rd, (a->disp))