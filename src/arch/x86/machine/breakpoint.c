/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API

#अन्तर्भाव <arch/machine/debug.h>
#अन्तर्भाव <mode/machine/debug.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <machine/registerset.h>
#अन्तर्भाव <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#परिभाषय X86_DEBUG_BP_आकार_1_B                (0x0u)
#परिभाषय X86_DEBUG_BP_आकार_2_B                (0x1u)
#परिभाषय X86_DEBUG_BP_आकार_4_B                (0x3u)
#परिभाषय X86_DEBUG_BP_आकार_8_B                (0x2u)

#परिभाषय X86_DEBUG_BP0_आकार_सरणम्            (18)
#परिभाषय X86_DEBUG_BP1_आकार_सरणम्            (22)
#परिभाषय X86_DEBUG_BP2_आकार_सरणम्            (26)
#परिभाषय X86_DEBUG_BP3_आकार_सरणम्            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#परिभाषय X86_DEBUG_BP_TYPE_IO                (0x2u)
#परिभाषय X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#परिभाषय X86_DEBUG_BP_TYPE_दत्त_WRITE        (0x1u)
#परिभाषय X86_DEBUG_BP_TYPE_दत्त_READWRITE    (0x3u)

#परिभाषय X86_DEBUG_BP0_TYPE_सरणम्            (16)
#परिभाषय X86_DEBUG_BP1_TYPE_सरणम्            (20)
#परिभाषय X86_DEBUG_BP2_TYPE_सरणम्            (24)
#परिभाषय X86_DEBUG_BP3_TYPE_सरणम्            (28)

#परिभाषय X86_DEBUG_EFLAGS_TRAP_FLAG    ((word_t)BIT(8))
#परिभाषय X86_DEBUG_EFLAGS_RESUME_FLAG    ((word_t)BIT(16))
#परिभाषय X86_DEBUG_DR6_SINGLE_चरणः_FLAG  ((word_t)BIT(14))

#परिभाषय X86_DEBUG_DR6_BP_आवरणम्     (0xFu)

स्थिर bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(शून्य)
{
    प्रत्यागम byte8_bps_supported;
}

स्थिर अन्तःस्थ शून्य bitwiseAndDr6Reg(word_t आवरणम्_2)
{
    word_t tmp;

    tmp = readDr6Reg() & आवरणम्_2;
    writeDr6Reg(tmp);
}

स्थिर अन्तःस्थ word_t readDr7Context(tcb_t *t)
{
    प्रत्यागम t->tcbArch.tcbContext.breakpointState.dr[5];
}

स्थिर अन्तःस्थ शून्य bitwiseOrDr7Context(tcb_t *t, word_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

स्थिर अन्तःस्थ शून्य bitwiseAndDr7Context(tcb_t *t, word_t आवरणम्_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= आवरणम्_2;
}

स्थिर शून्य unset_Dr7_द्वयङ्काः_For(tcb_t *t, uint16_t bp_अङ्कः)
{
    word_t आवरणम्_2;

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        आवरणम्_2 = (0x3u << X86_DEBUG_BP0_आकार_सरणम्) | (0x3u << X86_DEBUG_BP0_TYPE_सरणम्);
        भङ्ग;
    पक्ष 1:
        आवरणम्_2 = (0x3u << X86_DEBUG_BP1_आकार_सरणम्) | (0x3u << X86_DEBUG_BP1_TYPE_सरणम्);
        भङ्ग;
    पक्ष 2:
        आवरणम्_2 = (0x3u << X86_DEBUG_BP2_आकार_सरणम्) | (0x3u << X86_DEBUG_BP2_TYPE_सरणम्);
        भङ्ग;
    स्वभाव: /* 3 */
        प्रतिज्ञा(bp_अङ्कः == 3);
        आवरणम्_2 = (0x3u << X86_DEBUG_BP3_आकार_सरणम्) | (0x3u << X86_DEBUG_BP3_TYPE_सरणम्);
        भङ्ग;
    }

    आवरणम्_2 = ~आवरणम्_2;
    bitwiseAndDr7Context(t, आवरणम्_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
विशुद्धम् स्थिर अन्तःस्थ word_t convert_Type_And_Access_गन्तव्यम्_Arch(uint16_t bp_अङ्कः, word_t type, word_t rw)
{
    विकल्प (type) {
    पक्ष seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        भङ्ग;
    स्वभाव: /* seL4_DataBreakpoint */
        प्रतिज्ञा(type == se_L4_दत्त_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_दत्त_WRITE
               : X86_DEBUG_BP_TYPE_दत्त_READWRITE;
    }

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        प्रत्यागम type << X86_DEBUG_BP0_TYPE_सरणम्;
    पक्ष 1:
        प्रत्यागम type << X86_DEBUG_BP1_TYPE_सरणम्;
    पक्ष 2:
        प्रत्यागम type << X86_DEBUG_BP2_TYPE_सरणम्;
    स्वभाव: /* 3 */
        प्रतिज्ञा(bp_अङ्कः == 3);
        प्रत्यागम type << X86_DEBUG_BP3_TYPE_सरणम्;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
प्रकारनिर्देश संरचना {
    word_t type, rw;
} convertedTypeAndAccess_t;

विशुद्धम् स्थिर अन्तःस्थ convertedTypeAndAccess_t convert_Arch_गन्तव्यम्_Type_And_Access(word_t dr7, uint16_t bp_अङ्कः)
{
    convertedTypeAndAccess_t ret;

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_सरणम्;
        dr7 >>= X86_DEBUG_BP0_TYPE_सरणम्;
        भङ्ग;
    पक्ष 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_सरणम्;
        dr7 >>= X86_DEBUG_BP1_TYPE_सरणम्;
        भङ्ग;
    पक्ष 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_सरणम्;
        dr7 >>= X86_DEBUG_BP2_TYPE_सरणम्;
        भङ्ग;
    स्वभाव: /* 3 */
        प्रतिज्ञा(bp_अङ्कः == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_सरणम्;
        dr7 >>= X86_DEBUG_BP3_TYPE_सरणम्;
    }

    विकल्प (dr7) {
    पक्ष X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        भङ्ग;
    पक्ष X86_DEBUG_BP_TYPE_दत्त_WRITE:
        ret.type = se_L4_दत्त_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        भङ्ग;
    स्वभाव: /* Read-write */
        प्रतिज्ञा(dr7 == X86_DEBUG_BP_TYPE_दत्त_READWRITE);
        ret.type = se_L4_दत्त_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        भङ्ग;
    }
    प्रत्यागम ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
विशुद्धम् स्थिर अन्तःस्थ word_t convert_आकार_गन्तव्यम्_Arch(uint16_t bp_अङ्कः, word_t type, word_t आकार)
{
    यदि (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        आकार = 0;
    } अन्यथा {
        विकल्प (आकार) {
        पक्ष 1:
            आकार = X86_DEBUG_BP_आकार_1_B;
            भङ्ग;
        पक्ष 2:
            आकार = X86_DEBUG_BP_आकार_2_B;
            भङ्ग;
        पक्ष 8:
            आकार = X86_DEBUG_BP_आकार_8_B;
            भङ्ग;
        स्वभाव: /* 4B */
            प्रतिज्ञा(आकार == 4);
            आकार = X86_DEBUG_BP_आकार_4_B;
        }
    }

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        प्रत्यागम आकार << X86_DEBUG_BP0_आकार_सरणम्;
    पक्ष 1:
        प्रत्यागम आकार << X86_DEBUG_BP1_आकार_सरणम्;
    पक्ष 2:
        प्रत्यागम आकार << X86_DEBUG_BP2_आकार_सरणम्;
    स्वभाव: /* 3 */
        प्रतिज्ञा(bp_अङ्कः == 3);
        प्रत्यागम आकार << X86_DEBUG_BP3_आकार_सरणम्;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
विशुद्धम् स्थिर अन्तःस्थ word_t convert_Arch_गन्तव्यम्_आकार(word_t dr7, uint16_t bp_अङ्कः)
{
    word_t type;

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_सरणम्);
        type >>= X86_DEBUG_BP0_TYPE_सरणम्;
        dr7 &= 0x3u << X86_DEBUG_BP0_आकार_सरणम्;
        dr7 >>= X86_DEBUG_BP0_आकार_सरणम्;
        भङ्ग;
    पक्ष 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_सरणम्);
        type >>= X86_DEBUG_BP1_TYPE_सरणम्;
        dr7 &= 0x3u << X86_DEBUG_BP1_आकार_सरणम्;
        dr7 >>= X86_DEBUG_BP1_आकार_सरणम्;
        भङ्ग;
    पक्ष 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_सरणम्);
        type >>= X86_DEBUG_BP2_TYPE_सरणम्;
        dr7 &= 0x3u << X86_DEBUG_BP2_आकार_सरणम्;
        dr7 >>= X86_DEBUG_BP2_आकार_सरणम्;
        भङ्ग;
    स्वभाव: /* 3 */
        प्रतिज्ञा(bp_अङ्कः == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_सरणम्);
        type >>= X86_DEBUG_BP3_TYPE_सरणम्;
        dr7 &= 0x3u << X86_DEBUG_BP3_आकार_सरणम्;
        dr7 >>= X86_DEBUG_BP3_आकार_सरणम्;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    यदि (type == X86_DEBUG_BP_TYPE_INSTR) {
        प्रत्यागम 0;
    }

    विकल्प (dr7) {
    पक्ष X86_DEBUG_BP_आकार_1_B:
        प्रत्यागम 1;
    पक्ष X86_DEBUG_BP_आकार_2_B:
        प्रत्यागम 2;
    पक्ष X86_DEBUG_BP_आकार_8_B:
        प्रत्यागम 8;
    स्वभाव: /* 4B */
        प्रतिज्ञा(dr7 == X86_DEBUG_BP_आकार_4_B);
        प्रत्यागम 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
स्थिर शून्य enableBreakpoint(tcb_t *t, uint16_t bp_अङ्कः)
{
    word_t enable_bit;

    प्रतिज्ञा(t != रिक्त);
    प्रतिज्ञा(bp_अङ्कः < X86_DEBUG_BP_N_REGS);

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        enable_bit = X86_DEBUG_BP0_ENABLE_BIT;
        भङ्ग;
    पक्ष 1:
        enable_bit = X86_DEBUG_BP1_ENABLE_BIT;
        भङ्ग;
    पक्ष 2:
        enable_bit = X86_DEBUG_BP2_ENABLE_BIT;
        भङ्ग;
    स्वभाव:
        enable_bit = X86_DEBUG_BP3_ENABLE_BIT;
        भङ्ग;
    }

    bitwiseOrDr7Context(t, enable_bit);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
स्थिर शून्य disableBreakpoint(tcb_t *t, uint16_t bp_अङ्कः)
{
    word_t disable_आवरणम्;

    प्रतिज्ञा(t != रिक्त);
    प्रतिज्ञा(bp_अङ्कः < X86_DEBUG_BP_N_REGS);

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        disable_आवरणम् = ~X86_DEBUG_BP0_ENABLE_BIT;
        भङ्ग;
    पक्ष 1:
        disable_आवरणम् = ~X86_DEBUG_BP1_ENABLE_BIT;
        भङ्ग;
    पक्ष 2:
        disable_आवरणम् = ~X86_DEBUG_BP2_ENABLE_BIT;
        भङ्ग;
    स्वभाव:
        disable_आवरणम् = ~X86_DEBUG_BP3_ENABLE_BIT;
        भङ्ग;
    }

    bitwiseAndDr7Context(t, disable_आवरणम्);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
स्थिर bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_अङ्कः)
{
    word_t dr7;

    प्रतिज्ञा(t != रिक्त);
    प्रतिज्ञा(bp_अङ्कः < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        प्रत्यागम !!(dr7 & X86_DEBUG_BP0_ENABLE_BIT);
    पक्ष 1:
        प्रत्यागम !!(dr7 & X86_DEBUG_BP1_ENABLE_BIT);
    पक्ष 2:
        प्रत्यागम !!(dr7 & X86_DEBUG_BP2_ENABLE_BIT);
    स्वभाव:
        प्रत्यागम !!(dr7 & X86_DEBUG_BP3_ENABLE_BIT);
    }
}

स्थिर शून्य समुच्चयः_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_अङ्कः, word_t vaddr)
{
    प्रतिज्ञा(t != रिक्त);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        ubs->dr[0] = vaddr;
        भङ्ग;
    पक्ष 1:
        ubs->dr[1] = vaddr;
        भङ्ग;
    पक्ष 2:
        ubs->dr[2] = vaddr;
        भङ्ग;
    स्वभाव:
        प्रतिज्ञा(bp_अङ्कः == 3);
        ubs->dr[3] = vaddr;
        भङ्ग;
    }
    प्रत्यागम;
}

/** Backend for the seL4_TCB_SetBreakpoint invocation.
 *
 * @param uds Arch TCB register context structure.
 * @param bp_num Hardware breakpoint ID.
 * @param vaddr USerspace virtual address on which you'd like this breakpoint
 *        to trigger.
 * @param types One of the seL4_BreakpointType values.
 * @param size positive integer indicating the byte-range size that should
 *        trigger the breakpoint. 0 is valid for Instruction breakpoints.
 * @param rw Access type that should trigger the BP (read/write).
 */
शून्य समुच्चयः_Breakpoint(tcb_t *t,
                   uint16_t bp_अङ्कः, word_t vaddr, word_t types, word_t आकार, word_t rw)
{
    word_t dr7val;

    प्रतिज्ञा(t != रिक्त);

    dr7val = convert_Type_And_Access_गन्तव्यम्_Arch(bp_अङ्कः, types, rw);
    dr7val |= convert_आकार_गन्तव्यम्_Arch(bp_अङ्कः, types, आकार);

    समुच्चयः_Bp_Vaddr_Context(t, bp_अङ्कः, vaddr);
    unset_Dr7_द्वयङ्काः_For(t, bp_अङ्कः);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_अङ्कः);
}

स्थिर word_t getBpVaddrContext(tcb_t *t, uint16_t bp_अङ्कः)
{
    प्रतिज्ञा(t != रिक्त);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    विकल्प (bp_अङ्कः) {
    पक्ष 0:
        प्रत्यागम ubs->dr[0];
    पक्ष 1:
        प्रत्यागम ubs->dr[1];
    पक्ष 2:
        प्रत्यागम ubs->dr[2];
    स्वभाव:
        प्रतिज्ञा(bp_अङ्कः == 3);
        प्रत्यागम ubs->dr[3];
    }
}

/** Backend for the x86 seL4_TCB_GetBreakpoint invocation.
 *
 * Returns information about a particular breakpoint ID, including whether or
 * not it's enabled.
 *
 * @param uds Arch TCB register context pointer.
 * @param bp_num Hardware breakpoint ID of the BP you'd like to query.
 * @return Structure containing information about the status of the breakpoint.
 */
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_अङ्कः)
{
    word_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_अङ्कः);
    ret.आकार = convert_Arch_गन्तव्यम्_आकार(dr7val, bp_अङ्कः);
    res = convert_Arch_गन्तव्यम्_Type_And_Access(dr7val, bp_अङ्कः);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_अङ्कः);
    प्रत्यागम ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
शून्य unsetBreakpoint(tcb_t *t, uint16_t bp_अङ्कः)
{
    disableBreakpoint(t, bp_अङ्कः);
    unset_Dr7_द्वयङ्काः_For(t, bp_अङ्कः);
    समुच्चयः_Bp_Vaddr_Context(t, bp_अङ्कः, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
प्रकारनिर्देश संरचना {
    bool_t ret;
    word_t instr_vaddr;
} test_And_Reset_Single_चरणः_Exception_t;

स्थिर test_And_Reset_Single_चरणः_Exception_t test_And_Reset_Single_चरणः_Exception(tcb_t *t)
{
    test_And_Reset_Single_चरणः_Exception_t ret;
    word_t dr6;

    dr6 = readDr6Reg();
    यदि (!(dr6 & X86_DEBUG_DR6_SINGLE_चरणः_FLAG)) {
        ret.ret = false;
        प्रत्यागम ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_चरणः_FLAG);

    /* And that's not all: if the breakpoint is an instruction breakpoint, we
     * also need to set EFLAGS.RF. The processor raises the #DB exception BEFORE
     * the instruction executes. This means that when we IRET to userspace, the
     * SAME breakpoint will trigger again, and so on ad infinitum. EFLAGS.RF
     * solves this problem:
     *
     * When EFLAGS.RF is set, the processor will ignore instruction breakpoints
     * that should be raised, for one instruction. After that instruction
     * executes, the processor will also automatically unset EFLAGS.RF. See
     * Intel manuals, vol3, section 17.3.1.1.
     */
    /* This will automatically be popped by restore_user_context() */
    t->tcbArch.tcbContext.registers[FLAGS] |= X86_DEBUG_EFLAGS_RESUME_FLAG;
    प्रत्यागम ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_अङ्कः, word_t n_instr,
                               अप्रयुक्तम् bool_t is_प्रत्युत्तरम्)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    यदि (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_चरणः_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } अन्यथा {
        t->tcbArch.tcbContext.breakpointState.single_चरणः_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    प्रत्यागम false;
}

/** Used in the exception path to determine which breakpoint triggered the
 * exception.
 *
 * First, checks to see which hardware breakpoint was triggered, and saves
 * the ID of that breakpoint. Secondly, resets that breakpoint such that its
 * "triggered" bit is no longer in the asserted state -- whatever that means
 * for the arch. So on x86, that means clearing the indicator bit in DR6.
 *
 * Aside from the ID of the breakpoint that was raised, also returns
 * information about the breakpoint (vaddr, access, type, etc).
 *
 * @param uc Arch TCB register context pointer.
 * @return Structure with a "bp_num" member that states which hardware
 *          breakpoint was triggered, and gives information describing the
 *          breakpoint.
 */
प्रकारनिर्देश संरचना {
    पूर्णाङ्क bp_अङ्कः;
    word_t vaddr, reason;
} get_And_Reset_सक्रिय_Breakpoint_t;

स्थिर get_And_Reset_सक्रिय_Breakpoint_t get_And_Reset_सक्रिय_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_सक्रिय_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    word_t dr6 = readDr6Reg();
    यदि (dr6 & BIT(0)) {
        ret.bp_अङ्कः = 0;
    } अन्यथा यदि (dr6 & BIT(1)) {
        ret.bp_अङ्कः = 1;
    } अन्यथा यदि (dr6 & BIT(2)) {
        ret.bp_अङ्कः = 2;
    } अन्यथा यदि (dr6 & BIT(3)) {
        ret.bp_अङ्कः = 3;
    } अन्यथा {
        ret.bp_अङ्कः = -1;
        प्रत्यागम ret;
    }

    tmp = convert_Arch_गन्तव्यम्_Type_And_Access(readDr7Context(t), ret.bp_अङ्कः);
    ret.vaddr = getBpVaddrContext(t, ret.bp_अङ्कः);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~BIT(ret.bp_अङ्कः));
    प्रत्यागम ret;
}

exception_t handleUserLevelDebugException(पूर्णाङ्क int_सदिशम्)
{
    tcb_t *ct;
    get_And_Reset_सक्रिय_Breakpoint_t सक्रिय_bp;
    test_And_Reset_Single_चरणः_Exception_t single_चरणः_info;

#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_User_Level_Fault;
    ks_Kernel_प्रविष्टिः.word = int_सदिशम्;
#यदि_अन्तः /* DEBUG */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_start();
#यदि_अन्तः

    ct = ग्रन्थिः_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    यदि (int_सदिशम् == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ग्रन्थिः_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } अन्यथा {
        /* Hardware breakpoint trigger is detected using DR6 */
        सक्रिय_bp = get_And_Reset_सक्रिय_Breakpoint(ct);
        यदि (सक्रिय_bp.bp_अङ्कः >= 0) {
            current_fault = seL4_Fault_DebugException_new(सक्रिय_bp.vaddr,
                                                          सक्रिय_bp.bp_अङ्कः,
                                                          सक्रिय_bp.reason);
        } अन्यथा {
            single_चरणः_info = test_And_Reset_Single_चरणः_Exception(ct);
            यदि (single_चरणः_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                यदि (single_चरणः_Fault_गणक_Ready(ct) == false) {
                    प्रत्यागम EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_चरणः_info.instr_vaddr,
                                                              0, se_L4_Single_चरणः);
            } अन्यथा {
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ग्रन्थिः_STATE(ksCurThread));

    कालसारणी();
    activateThread();

    प्रत्यागम EXCEPTION_NONE;
}

BOOT_संहिता bool_t Arch_initHardwareBreakpoints(शून्य)
{
    x86_cpu_तटस्थः_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    यदि (modelinfo->family == 15) {
        यदि (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    यदि (modelinfo->family == 6) {
        यदि (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    प्रत्यागम true;
}

शून्य Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    स्मृतिं_पूरय(uds, 0, परिमाण(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(BIT(0)
                   | BIT(1)
                   | BIT(2)
                   | BIT(3)
                   | X86_DEBUG_DR6_SINGLE_चरणः_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_BIT | X86_DEBUG_BP1_ENABLE_BIT
                   | X86_DEBUG_BP2_ENABLE_BIT
                   | X86_DEBUG_BP3_ENABLE_BIT);
}

#यदि_अन्तः
