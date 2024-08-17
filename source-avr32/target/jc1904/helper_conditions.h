
#ifndef QEMU_JC1904_HELPER_CP_INST_H
#define QEMU_JC1904_HELPER_CP_INST_H
#include "qemu/osdep.h"
#include "qemu/qemu-print.h"
#include "tcg/tcg.h"
#include "cpu.h"
#include "exec/exec-all.h"
#include "tcg/tcg-op.h"
#include "exec/cpu_ldst.h"
#include "exec/helper-proto.h"
#include "exec/helper-gen.h"
#include "exec/log.h"
#include "exec/translator.h"
#include "exec/gen-icount.h"

int checkCondition(int condition, TCGv returnReg, TCGv cpu_r[], TCGv cpu_sflags[]);

void set_v_flag_add(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);
void set_c_flag_add(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void set_v_flag_cp(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);
void set_c_flag_cp(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void set_flags_cpc(TCGv op1, TCGv op2, TCGv result, TCGv cpu_sflags[]);

void cpw_instruction(TCGv Rd, TCGv Rs, TCGv cpu_sflags[]);

#endif //QEMU_JC1904_HELPER_CP_INST_H
