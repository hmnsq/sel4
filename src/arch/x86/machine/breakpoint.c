/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API

#समावेश <arch/machine/debug.h>
#समावेश <mode/machine/debug.h>
#समावेश <arch/machine.h>
#समावेश <machine/registerset.h>
#समावेश <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#घोषणा X86_DEBUG_BP_आकार_1_B                (0x0u)
#घोषणा X86_DEBUG_BP_आकार_2_B                (0x1u)
#घोषणा X86_DEBUG_BP_आकार_4_B                (0x3u)
#घोषणा X86_DEBUG_BP_आकार_8_B                (0x2u)

#घोषणा X86_DEBUG_BP0_आकार_SHIFT            (18)
#घोषणा X86_DEBUG_BP1_आकार_SHIFT            (22)
#घोषणा X86_DEBUG_BP2_आकार_SHIFT            (26)
#घोषणा X86_DEBUG_BP3_आकार_SHIFT            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#घोषणा X86_DEBUG_BP_TYPE_IO                (0x2u)
#घोषणा X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#घोषणा X86_DEBUG_BP_TYPE_तथ्य_WRITE        (0x1u)
#घोषणा X86_DEBUG_BP_TYPE_तथ्य_READWRITE    (0x3u)

#घोषणा X86_DEBUG_BP0_TYPE_SHIFT            (16)
#घोषणा X86_DEBUG_BP1_TYPE_SHIFT            (20)
#घोषणा X86_DEBUG_BP2_TYPE_SHIFT            (24)
#घोषणा X86_DEBUG_BP3_TYPE_SHIFT            (28)

#घोषणा X86_DEBUG_EFLAGS_TRAP_FLAG    ((word_t)BIT(8))
#घोषणा X86_DEBUG_EFLAGS_RESUME_FLAG    ((word_t)BIT(16))
#घोषणा X86_DEBUG_DR6_SINGLE_STEP_FLAG  ((word_t)BIT(14))

#घोषणा X86_DEBUG_DR6_BP_MASK     (0xFu)

अटल bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(व्योम)
{
    वापस byte8_bps_supported;
}

अटल अंतरभूत व्योम bitwiseAndDr6Reg(word_t mask)
{
    word_t tmp;

    tmp = readDr6Reg() & mask;
    writeDr6Reg(tmp);
}

अटल अंतरभूत word_t readDr7Context(tcb_t *t)
{
    वापस t->tcbArch.tcbContext.breakpointState.dr[5];
}

अटल अंतरभूत व्योम bitwiseOrDr7Context(tcb_t *t, word_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

अटल अंतरभूत व्योम bitwiseAndDr7Context(tcb_t *t, word_t mask)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= mask;
}

अटल व्योम unsetDr7BitsFor(tcb_t *t, uint16_t bp_अंक)
{
    word_t mask;

    चयन (bp_अंक) {
    हाल 0:
        mask = (0x3u << X86_DEBUG_BP0_आकार_SHIFT) | (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        अवरोध;
    हाल 1:
        mask = (0x3u << X86_DEBUG_BP1_आकार_SHIFT) | (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        अवरोध;
    हाल 2:
        mask = (0x3u << X86_DEBUG_BP2_आकार_SHIFT) | (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        अवरोध;
    शेष: /* 3 */
        निश्चित(bp_अंक == 3);
        mask = (0x3u << X86_DEBUG_BP3_आकार_SHIFT) | (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        अवरोध;
    }

    mask = ~mask;
    bitwiseAndDr7Context(t, mask);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
PURE अटल अंतरभूत word_t convertTypeAndAccessToArch(uint16_t bp_अंक, word_t type, word_t rw)
{
    चयन (type) {
    हाल seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        अवरोध;
    शेष: /* seL4_DataBreakpoint */
        निश्चित(type == se_L4_तथ्य_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_तथ्य_WRITE
               : X86_DEBUG_BP_TYPE_तथ्य_READWRITE;
    }

    चयन (bp_अंक) {
    हाल 0:
        वापस type << X86_DEBUG_BP0_TYPE_SHIFT;
    हाल 1:
        वापस type << X86_DEBUG_BP1_TYPE_SHIFT;
    हाल 2:
        वापस type << X86_DEBUG_BP2_TYPE_SHIFT;
    शेष: /* 3 */
        निश्चित(bp_अंक == 3);
        वापस type << X86_DEBUG_BP3_TYPE_SHIFT;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
प्रकार काष्ठा {
    word_t type, rw;
} convertedTypeAndAccess_t;

PURE अटल अंतरभूत convertedTypeAndAccess_t convertArchToTypeAndAccess(word_t dr7, uint16_t bp_अंक)
{
    convertedTypeAndAccess_t ret;

    चयन (bp_अंक) {
    हाल 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_TYPE_SHIFT;
        अवरोध;
    हाल 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_TYPE_SHIFT;
        अवरोध;
    हाल 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_TYPE_SHIFT;
        अवरोध;
    शेष: /* 3 */
        निश्चित(bp_अंक == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_TYPE_SHIFT;
    }

    चयन (dr7) {
    हाल X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        अवरोध;
    हाल X86_DEBUG_BP_TYPE_तथ्य_WRITE:
        ret.type = se_L4_तथ्य_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        अवरोध;
    शेष: /* Read-write */
        निश्चित(dr7 == X86_DEBUG_BP_TYPE_तथ्य_READWRITE);
        ret.type = se_L4_तथ्य_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        अवरोध;
    }
    वापस ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
PURE अटल अंतरभूत word_t convert_आकार_To_Arch(uint16_t bp_अंक, word_t type, word_t आकार)
{
    अगर (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        आकार = 0;
    } अन्यथा {
        चयन (आकार) {
        हाल 1:
            आकार = X86_DEBUG_BP_आकार_1_B;
            अवरोध;
        हाल 2:
            आकार = X86_DEBUG_BP_आकार_2_B;
            अवरोध;
        हाल 8:
            आकार = X86_DEBUG_BP_आकार_8_B;
            अवरोध;
        शेष: /* 4B */
            निश्चित(आकार == 4);
            आकार = X86_DEBUG_BP_आकार_4_B;
        }
    }

    चयन (bp_अंक) {
    हाल 0:
        वापस आकार << X86_DEBUG_BP0_आकार_SHIFT;
    हाल 1:
        वापस आकार << X86_DEBUG_BP1_आकार_SHIFT;
    हाल 2:
        वापस आकार << X86_DEBUG_BP2_आकार_SHIFT;
    शेष: /* 3 */
        निश्चित(bp_अंक == 3);
        वापस आकार << X86_DEBUG_BP3_आकार_SHIFT;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
PURE अटल अंतरभूत word_t convert_Arch_To_आकार(word_t dr7, uint16_t bp_अंक)
{
    word_t type;

    चयन (bp_अंक) {
    हाल 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        type >>= X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP0_आकार_SHIFT;
        dr7 >>= X86_DEBUG_BP0_आकार_SHIFT;
        अवरोध;
    हाल 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        type >>= X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP1_आकार_SHIFT;
        dr7 >>= X86_DEBUG_BP1_आकार_SHIFT;
        अवरोध;
    हाल 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        type >>= X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP2_आकार_SHIFT;
        dr7 >>= X86_DEBUG_BP2_आकार_SHIFT;
        अवरोध;
    शेष: /* 3 */
        निश्चित(bp_अंक == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        type >>= X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP3_आकार_SHIFT;
        dr7 >>= X86_DEBUG_BP3_आकार_SHIFT;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    अगर (type == X86_DEBUG_BP_TYPE_INSTR) {
        वापस 0;
    }

    चयन (dr7) {
    हाल X86_DEBUG_BP_आकार_1_B:
        वापस 1;
    हाल X86_DEBUG_BP_आकार_2_B:
        वापस 2;
    हाल X86_DEBUG_BP_आकार_8_B:
        वापस 8;
    शेष: /* 4B */
        निश्चित(dr7 == X86_DEBUG_BP_आकार_4_B);
        वापस 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
अटल व्योम enableBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    word_t enable_bit;

    निश्चित(t != शून्य);
    निश्चित(bp_अंक < X86_DEBUG_BP_N_REGS);

    चयन (bp_अंक) {
    हाल 0:
        enable_bit = X86_DEBUG_BP0_ENABLE_BIT;
        अवरोध;
    हाल 1:
        enable_bit = X86_DEBUG_BP1_ENABLE_BIT;
        अवरोध;
    हाल 2:
        enable_bit = X86_DEBUG_BP2_ENABLE_BIT;
        अवरोध;
    शेष:
        enable_bit = X86_DEBUG_BP3_ENABLE_BIT;
        अवरोध;
    }

    bitwiseOrDr7Context(t, enable_bit);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
अटल व्योम disableBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    word_t disable_mask;

    निश्चित(t != शून्य);
    निश्चित(bp_अंक < X86_DEBUG_BP_N_REGS);

    चयन (bp_अंक) {
    हाल 0:
        disable_mask = ~X86_DEBUG_BP0_ENABLE_BIT;
        अवरोध;
    हाल 1:
        disable_mask = ~X86_DEBUG_BP1_ENABLE_BIT;
        अवरोध;
    हाल 2:
        disable_mask = ~X86_DEBUG_BP2_ENABLE_BIT;
        अवरोध;
    शेष:
        disable_mask = ~X86_DEBUG_BP3_ENABLE_BIT;
        अवरोध;
    }

    bitwiseAndDr7Context(t, disable_mask);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
अटल bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_अंक)
{
    word_t dr7;

    निश्चित(t != शून्य);
    निश्चित(bp_अंक < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    चयन (bp_अंक) {
    हाल 0:
        वापस !!(dr7 & X86_DEBUG_BP0_ENABLE_BIT);
    हाल 1:
        वापस !!(dr7 & X86_DEBUG_BP1_ENABLE_BIT);
    हाल 2:
        वापस !!(dr7 & X86_DEBUG_BP2_ENABLE_BIT);
    शेष:
        वापस !!(dr7 & X86_DEBUG_BP3_ENABLE_BIT);
    }
}

अटल व्योम setBpVaddrContext(tcb_t *t, uint16_t bp_अंक, word_t vaddr)
{
    निश्चित(t != शून्य);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    चयन (bp_अंक) {
    हाल 0:
        ubs->dr[0] = vaddr;
        अवरोध;
    हाल 1:
        ubs->dr[1] = vaddr;
        अवरोध;
    हाल 2:
        ubs->dr[2] = vaddr;
        अवरोध;
    शेष:
        निश्चित(bp_अंक == 3);
        ubs->dr[3] = vaddr;
        अवरोध;
    }
    वापस;
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
व्योम setBreakpoint(tcb_t *t,
                   uint16_t bp_अंक, word_t vaddr, word_t types, word_t आकार, word_t rw)
{
    word_t dr7val;

    निश्चित(t != शून्य);

    dr7val = convertTypeAndAccessToArch(bp_अंक, types, rw);
    dr7val |= convert_आकार_To_Arch(bp_अंक, types, आकार);

    setBpVaddrContext(t, bp_अंक, vaddr);
    unsetDr7BitsFor(t, bp_अंक);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_अंक);
}

अटल word_t getBpVaddrContext(tcb_t *t, uint16_t bp_अंक)
{
    निश्चित(t != शून्य);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    चयन (bp_अंक) {
    हाल 0:
        वापस ubs->dr[0];
    हाल 1:
        वापस ubs->dr[1];
    हाल 2:
        वापस ubs->dr[2];
    शेष:
        निश्चित(bp_अंक == 3);
        वापस ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    word_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_अंक);
    ret.आकार = convert_Arch_To_आकार(dr7val, bp_अंक);
    res = convertArchToTypeAndAccess(dr7val, bp_अंक);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_अंक);
    वापस ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
व्योम unsetBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    disableBreakpoint(t, bp_अंक);
    unsetDr7BitsFor(t, bp_अंक);
    setBpVaddrContext(t, bp_अंक, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
प्रकार काष्ठा {
    bool_t ret;
    word_t instr_vaddr;
} testAndResetSingleStepException_t;

अटल testAndResetSingleStepException_t testAndResetSingleStepException(tcb_t *t)
{
    testAndResetSingleStepException_t ret;
    word_t dr6;

    dr6 = readDr6Reg();
    अगर (!(dr6 & X86_DEBUG_DR6_SINGLE_STEP_FLAG)) {
        ret.ret = false;
        वापस ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_STEP_FLAG);

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
    वापस ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_अंक, word_t n_instr,
                               UNUSED bool_t is_reply)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    अगर (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } अन्यथा {
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    वापस false;
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
प्रकार काष्ठा {
    पूर्णांक bp_अंक;
    word_t vaddr, reason;
} getAndResetActiveBreakpoint_t;

अटल getAndResetActiveBreakpoint_t getAndResetActiveBreakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    getAndResetActiveBreakpoint_t ret;

    /* Read from the hardware regs, not user context */
    word_t dr6 = readDr6Reg();
    अगर (dr6 & BIT(0)) {
        ret.bp_अंक = 0;
    } अन्यथा अगर (dr6 & BIT(1)) {
        ret.bp_अंक = 1;
    } अन्यथा अगर (dr6 & BIT(2)) {
        ret.bp_अंक = 2;
    } अन्यथा अगर (dr6 & BIT(3)) {
        ret.bp_अंक = 3;
    } अन्यथा {
        ret.bp_अंक = -1;
        वापस ret;
    }

    tmp = convertArchToTypeAndAccess(readDr7Context(t), ret.bp_अंक);
    ret.vaddr = getBpVaddrContext(t, ret.bp_अंक);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~BIT(ret.bp_अंक));
    वापस ret;
}

exception_t handleUserLevelDebugException(पूर्णांक int_vector)
{
    tcb_t *ct;
    getAndResetActiveBreakpoint_t active_bp;
    testAndResetSingleStepException_t single_step_info;

#अगर_घोषित TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = int_vector;
#पूर्ण_अगर /* DEBUG */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_start();
#पूर्ण_अगर

    ct = गाँठ_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    अगर (int_vector == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(गाँठ_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } अन्यथा {
        /* Hardware breakpoint trigger is detected using DR6 */
        active_bp = getAndResetActiveBreakpoint(ct);
        अगर (active_bp.bp_अंक >= 0) {
            current_fault = seL4_Fault_DebugException_new(active_bp.vaddr,
                                                          active_bp.bp_अंक,
                                                          active_bp.reason);
        } अन्यथा {
            single_step_info = testAndResetSingleStepException(ct);
            अगर (single_step_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                अगर (single_Step_Fault_गणक_Ready(ct) == false) {
                    वापस EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_step_info.instr_vaddr,
                                                              0, seL4_SingleStep);
            } अन्यथा {
                वापस EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(गाँठ_STATE(ksCurThread));

    schedule();
    activateThread();

    वापस EXCEPTION_NONE;
}

BOOT_CODE bool_t Arch_initHardwareBreakpoints(व्योम)
{
    x86_cpu_identity_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    अगर (modelinfo->family == 15) {
        अगर (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    अगर (modelinfo->family == 6) {
        अगर (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    वापस true;
}

व्योम Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    स_रखो(uds, 0, माप(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(BIT(0)
                   | BIT(1)
                   | BIT(2)
                   | BIT(3)
                   | X86_DEBUG_DR6_SINGLE_STEP_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_BIT | X86_DEBUG_BP1_ENABLE_BIT
                   | X86_DEBUG_BP2_ENABLE_BIT
                   | X86_DEBUG_BP3_ENABLE_BIT);
}

#पूर्ण_अगर
