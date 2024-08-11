#define REG(x) avr32_cpu_r_names[x]

INSN(ADD_rd_rs,          ADD,      "%s, %s",                   REG(a->rd), REG(a->rs))
INSN(ADD_rd_rx_ry_sa,    ADD,      "%s, %s, %s",               REG(a->rd), REG(a->rx), REG(a->ry))

