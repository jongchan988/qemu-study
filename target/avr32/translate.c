static uint32_t decode_insn_load_bytes(DisasContext *ctx, uint32_t insn, int i, int n){
    //If
    if(i == 0){
    //cpu_lduw loads an unsigned word(16 bit in QEMU) from the emulated memory
        insn = cpu_lduw_be_data(ctx->env, ctx->base.pc_next + i) << 16;
    }
    else if (i== 2){
        insn |= cpu_lduw_be_data(ctx->env, ctx->base.pc_next + i);
    }

    //No instruction was loaded. This should ne happen.
    if(insn == 0x0){
        gen_helper_raise_illegal_instruction(cpu_env);
    }
    return insn;
}

static void avr32_tr_translate_insn(DisasContextBase *dcbase, CPUState *cs)
{
    DisasContext *ctx = container_of(dcbase, DisasContext, base);
    uint32_t insn;

    //Moce the curent location into the emulated program counter register
    tcg_gen_movi_tl(cpu_r[PC_REG], ctx->base.pc_next);

    //load the next instruction
    insn = decode_insn_load(ctx);
    //Call the decode function
    if (!decode_insn(ctx, insn)) {
        error_report("[AVR32-TCG] avr32_tr_translate_insn, illegal instr, pc: 0x%04x\n", ctx->base.pc_next);
        gen_helper_raise_illegal_instruction(cpu_env);
    }
}

static const TranslatorOps avr32_tr_ops = {
        .init_disas_context = avr32_tr_init_disas_context,
        .tb_start           = avr32_tr_tb_start,
        .insn_start         = avr32_tr_insn_start,
        .translate_insn     = avr32_tr_translate_insn,
        .tb_stop            = avr32_tr_tb_stop,
        .disas_log          = avr32_tr_disas_log,
};

static bool trans_ADD_rd_rs(DisasContext *ctx, arg_ADD_rd_rs *a){
    TCGv res = tcg_temp_new_i32();
    TCGv Rd = tcg_temp_new_i32();
    TCGv Rs = tcg_temp_new_i32();

    tcg_gen_mov_i32(Rd,  cpu_r[a->rd]);
    tcg_gen_mov_i32(Rs, cpu_r[a->rs]);


    tcg_gen_add_i32(res,  cpu_r[a->rd], cpu_r[a->rs]);
    tcg_gen_add_i32(cpu_r[a->rd],  cpu_r[a->rd], cpu_r[a->rs]);

    // set N flag: N ← RES[31]
    tcg_gen_shri_i32(cpu_sflags[sflagN], res, 31);

    // set Z flag: Z ← (RES[31:0] == 0)
    tcg_gen_setcondi_tl(TCG_COND_EQ, cpu_sflags[sflagZ], res, 0); /* Zf = R == 0 */

    tcg_gen_shri_i32(Rd, Rd, 31);
    tcg_gen_shri_i32(Rs, Rs, 31);
    tcg_gen_shri_i32(res, res, 31);

    // V-flag
    set_v_flag_add(Rd, Rs, res, cpu_sflags);

    // C-flag
    set_c_flag_add(Rd, Rs, res, cpu_sflags);


    if(a->rd == PC_REG){
        ctx->base.is_jmp = DISAS_JUMP;
    }

    ctx->base.pc_next += 2;
    return true;
}