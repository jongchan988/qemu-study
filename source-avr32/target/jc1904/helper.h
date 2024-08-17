
DEF_HELPER_1(raise_illegal_instruction, noreturn, env)
DEF_HELPER_1(debug, noreturn, env)
DEF_HELPER_1(break, noreturn, env)
DEF_HELPER_4(macsathhw, void, env, i32, i32, i32)

#ifndef QEMU_JC1904_HELPER
#define QEMU_JC1904_HELPER
#include "tcg/tcg.h"
#define sflagC 0
#define sflagZ 1
#define sflagN 2
#define sflagV 3
#define sflagQ 4
#define sflagL 5
#define sflagT 14
#define sflagR 15
#define sflagGM 16
#define sflagEM 21


#endif //QEMU_JC1904_HELPER
