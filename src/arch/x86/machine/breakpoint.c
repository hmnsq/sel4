/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#ifdef CONFIG_HARDWARE_DEBUG_API

#અન્તર્ભાવ <arch/machine/debug.h>
#અન્તર્ભાવ <mode/machine/debug.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <machine/registerset.h>
#અન્તર્ભાવ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#define X86_DEBUG_BP_SIZE_1B                (0x0u)
#define X86_DEBUG_BP_SIZE_2B                (0x1u)
#define X86_DEBUG_BP_SIZE_4B                (0x3u)
#define X86_DEBUG_BP_SIZE_8B                (0x2u)

#define X86_DEBUG_BP0_SIZE_SHIFT            (18)
#define X86_DEBUG_BP1_SIZE_SHIFT            (22)
#define X86_DEBUG_BP2_SIZE_SHIFT            (26)
#define X86_DEBUG_BP3_SIZE_SHIFT            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#define X86_DEBUG_BP_TYPE_IO                (0x2u)
#define X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#define X86_DEBUG_BP_TYPE_DATA_WRITE        (0x1u)
#define X86_DEBUG_BP_TYPE_DATA_READWRITE    (0x3u)

#define X86_DEBUG_BP0_TYPE_SHIFT            (16)
#define X86_DEBUG_BP1_TYPE_SHIFT            (20)
#define X86_DEBUG_BP2_TYPE_SHIFT            (24)
#define X86_DEBUG_BP3_TYPE_SHIFT            (28)

#define X86_DEBUG_EFLAGS_TRAP_FLAG    ((word_t)BIT(8))
#define X86_DEBUG_EFLAGS_RESUME_FLAG    ((word_t)BIT(16))
#define X86_DEBUG_DR6_SINGLE_STEP_FLAG  ((word_t)BIT(14))

#define X86_DEBUG_DR6_BP_MASK     (0xFu)

સ્થિર bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(શૂન્ય)
{
    પાછા byte8_bps_supported;
}

સ્થિર અંતઃરેખ શૂન્ય bitwiseAndDr6Reg(word_t mask)
{
    word_t tmp;

    tmp = readDr6Reg() & mask;
    writeDr6Reg(tmp);
}

સ્થિર અંતઃરેખ word_t readDr7Context(tcb_t *t)
{
    પાછા t->tcbArch.tcbContext.breakpointState.dr[5];
}

સ્થિર અંતઃરેખ શૂન્ય bitwiseOrDr7Context(tcb_t *t, word_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

સ્થિર અંતઃરેખ શૂન્ય bitwiseAndDr7Context(tcb_t *t, word_t mask)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= mask;
}

સ્થિર શૂન્ય unsetDr7BitsFor(tcb_t *t, uint16_t bp_અંક)
{
    word_t mask;

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        mask = (0x3u << X86_DEBUG_BP0_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        તોડો;
    સ્થિતિ 1:
        mask = (0x3u << X86_DEBUG_BP1_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        તોડો;
    સ્થિતિ 2:
        mask = (0x3u << X86_DEBUG_BP2_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        તોડો;
    મૂળભૂત: /* 3 */
        પ્રતિજ્ઞા(bp_અંક == 3);
        mask = (0x3u << X86_DEBUG_BP3_SIZE_SHIFT) | (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        તોડો;
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
PURE સ્થિર અંતઃરેખ word_t convertTypeAndAccessToArch(uint16_t bp_અંક, word_t type, word_t rw)
{
    પસંદગી (type) {
    સ્થિતિ seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        તોડો;
    મૂળભૂત: /* seL4_DataBreakpoint */
        પ્રતિજ્ઞા(type == seL4_DataBreakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_DATA_WRITE
               : X86_DEBUG_BP_TYPE_DATA_READWRITE;
    }

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        પાછા type << X86_DEBUG_BP0_TYPE_SHIFT;
    સ્થિતિ 1:
        પાછા type << X86_DEBUG_BP1_TYPE_SHIFT;
    સ્થિતિ 2:
        પાછા type << X86_DEBUG_BP2_TYPE_SHIFT;
    મૂળભૂત: /* 3 */
        પ્રતિજ્ઞા(bp_અંક == 3);
        પાછા type << X86_DEBUG_BP3_TYPE_SHIFT;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
પ્રકારવ્યાખ્યા સંરચના {
    word_t type, rw;
} convertedTypeAndAccess_t;

PURE સ્થિર અંતઃરેખ convertedTypeAndAccess_t convertArchToTypeAndAccess(word_t dr7, uint16_t bp_અંક)
{
    convertedTypeAndAccess_t ret;

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_TYPE_SHIFT;
        તોડો;
    સ્થિતિ 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_TYPE_SHIFT;
        તોડો;
    સ્થિતિ 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_TYPE_SHIFT;
        તોડો;
    મૂળભૂત: /* 3 */
        પ્રતિજ્ઞા(bp_અંક == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_TYPE_SHIFT;
    }

    પસંદગી (dr7) {
    સ્થિતિ X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        તોડો;
    સ્થિતિ X86_DEBUG_BP_TYPE_DATA_WRITE:
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnWrite;
        તોડો;
    મૂળભૂત: /* Read-write */
        પ્રતિજ્ઞા(dr7 == X86_DEBUG_BP_TYPE_DATA_READWRITE);
        ret.type = seL4_DataBreakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        તોડો;
    }
    પાછા ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
PURE સ્થિર અંતઃરેખ word_t convertSizeToArch(uint16_t bp_અંક, word_t type, word_t size)
{
    જો (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        size = 0;
    } નહીંતર {
        પસંદગી (size) {
        સ્થિતિ 1:
            size = X86_DEBUG_BP_SIZE_1B;
            તોડો;
        સ્થિતિ 2:
            size = X86_DEBUG_BP_SIZE_2B;
            તોડો;
        સ્થિતિ 8:
            size = X86_DEBUG_BP_SIZE_8B;
            તોડો;
        મૂળભૂત: /* 4B */
            પ્રતિજ્ઞા(size == 4);
            size = X86_DEBUG_BP_SIZE_4B;
        }
    }

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        પાછા size << X86_DEBUG_BP0_SIZE_SHIFT;
    સ્થિતિ 1:
        પાછા size << X86_DEBUG_BP1_SIZE_SHIFT;
    સ્થિતિ 2:
        પાછા size << X86_DEBUG_BP2_SIZE_SHIFT;
    મૂળભૂત: /* 3 */
        પ્રતિજ્ઞા(bp_અંક == 3);
        પાછા size << X86_DEBUG_BP3_SIZE_SHIFT;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
PURE સ્થિર અંતઃરેખ word_t convertArchToSize(word_t dr7, uint16_t bp_અંક)
{
    word_t type;

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_SHIFT);
        type >>= X86_DEBUG_BP0_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP0_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP0_SIZE_SHIFT;
        તોડો;
    સ્થિતિ 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_SHIFT);
        type >>= X86_DEBUG_BP1_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP1_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP1_SIZE_SHIFT;
        તોડો;
    સ્થિતિ 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_SHIFT);
        type >>= X86_DEBUG_BP2_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP2_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP2_SIZE_SHIFT;
        તોડો;
    મૂળભૂત: /* 3 */
        પ્રતિજ્ઞા(bp_અંક == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_SHIFT);
        type >>= X86_DEBUG_BP3_TYPE_SHIFT;
        dr7 &= 0x3u << X86_DEBUG_BP3_SIZE_SHIFT;
        dr7 >>= X86_DEBUG_BP3_SIZE_SHIFT;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    જો (type == X86_DEBUG_BP_TYPE_INSTR) {
        પાછા 0;
    }

    પસંદગી (dr7) {
    સ્થિતિ X86_DEBUG_BP_SIZE_1B:
        પાછા 1;
    સ્થિતિ X86_DEBUG_BP_SIZE_2B:
        પાછા 2;
    સ્થિતિ X86_DEBUG_BP_SIZE_8B:
        પાછા 8;
    મૂળભૂત: /* 4B */
        પ્રતિજ્ઞા(dr7 == X86_DEBUG_BP_SIZE_4B);
        પાછા 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
સ્થિર શૂન્ય enableBreakpoint(tcb_t *t, uint16_t bp_અંક)
{
    word_t enable_bit;

    પ્રતિજ્ઞા(t != ખાલી);
    પ્રતિજ્ઞા(bp_અંક < X86_DEBUG_BP_N_REGS);

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        enable_bit = X86_DEBUG_BP0_ENABLE_BIT;
        તોડો;
    સ્થિતિ 1:
        enable_bit = X86_DEBUG_BP1_ENABLE_BIT;
        તોડો;
    સ્થિતિ 2:
        enable_bit = X86_DEBUG_BP2_ENABLE_BIT;
        તોડો;
    મૂળભૂત:
        enable_bit = X86_DEBUG_BP3_ENABLE_BIT;
        તોડો;
    }

    bitwiseOrDr7Context(t, enable_bit);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
સ્થિર શૂન્ય disableBreakpoint(tcb_t *t, uint16_t bp_અંક)
{
    word_t disable_mask;

    પ્રતિજ્ઞા(t != ખાલી);
    પ્રતિજ્ઞા(bp_અંક < X86_DEBUG_BP_N_REGS);

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        disable_mask = ~X86_DEBUG_BP0_ENABLE_BIT;
        તોડો;
    સ્થિતિ 1:
        disable_mask = ~X86_DEBUG_BP1_ENABLE_BIT;
        તોડો;
    સ્થિતિ 2:
        disable_mask = ~X86_DEBUG_BP2_ENABLE_BIT;
        તોડો;
    મૂળભૂત:
        disable_mask = ~X86_DEBUG_BP3_ENABLE_BIT;
        તોડો;
    }

    bitwiseAndDr7Context(t, disable_mask);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
સ્થિર bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_અંક)
{
    word_t dr7;

    પ્રતિજ્ઞા(t != ખાલી);
    પ્રતિજ્ઞા(bp_અંક < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        પાછા !!(dr7 & X86_DEBUG_BP0_ENABLE_BIT);
    સ્થિતિ 1:
        પાછા !!(dr7 & X86_DEBUG_BP1_ENABLE_BIT);
    સ્થિતિ 2:
        પાછા !!(dr7 & X86_DEBUG_BP2_ENABLE_BIT);
    મૂળભૂત:
        પાછા !!(dr7 & X86_DEBUG_BP3_ENABLE_BIT);
    }
}

સ્થિર શૂન્ય setBpVaddrContext(tcb_t *t, uint16_t bp_અંક, word_t vaddr)
{
    પ્રતિજ્ઞા(t != ખાલી);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        ubs->dr[0] = vaddr;
        તોડો;
    સ્થિતિ 1:
        ubs->dr[1] = vaddr;
        તોડો;
    સ્થિતિ 2:
        ubs->dr[2] = vaddr;
        તોડો;
    મૂળભૂત:
        પ્રતિજ્ઞા(bp_અંક == 3);
        ubs->dr[3] = vaddr;
        તોડો;
    }
    પાછા;
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
શૂન્ય setBreakpoint(tcb_t *t,
                   uint16_t bp_અંક, word_t vaddr, word_t types, word_t size, word_t rw)
{
    word_t dr7val;

    પ્રતિજ્ઞા(t != ખાલી);

    dr7val = convertTypeAndAccessToArch(bp_અંક, types, rw);
    dr7val |= convertSizeToArch(bp_અંક, types, size);

    setBpVaddrContext(t, bp_અંક, vaddr);
    unsetDr7BitsFor(t, bp_અંક);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_અંક);
}

સ્થિર word_t getBpVaddrContext(tcb_t *t, uint16_t bp_અંક)
{
    પ્રતિજ્ઞા(t != ખાલી);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    પસંદગી (bp_અંક) {
    સ્થિતિ 0:
        પાછા ubs->dr[0];
    સ્થિતિ 1:
        પાછા ubs->dr[1];
    સ્થિતિ 2:
        પાછા ubs->dr[2];
    મૂળભૂત:
        પ્રતિજ્ઞા(bp_અંક == 3);
        પાછા ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_અંક)
{
    word_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_અંક);
    ret.size = convertArchToSize(dr7val, bp_અંક);
    res = convertArchToTypeAndAccess(dr7val, bp_અંક);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_અંક);
    પાછા ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
શૂન્ય unsetBreakpoint(tcb_t *t, uint16_t bp_અંક)
{
    disableBreakpoint(t, bp_અંક);
    unsetDr7BitsFor(t, bp_અંક);
    setBpVaddrContext(t, bp_અંક, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
પ્રકારવ્યાખ્યા સંરચના {
    bool_t ret;
    word_t instr_vaddr;
} testAndResetSingleStepException_t;

સ્થિર testAndResetSingleStepException_t testAndResetSingleStepException(tcb_t *t)
{
    testAndResetSingleStepException_t ret;
    word_t dr6;

    dr6 = readDr6Reg();
    જો (!(dr6 & X86_DEBUG_DR6_SINGLE_STEP_FLAG)) {
        ret.ret = false;
        પાછા ret;
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
    પાછા ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_અંક, word_t n_instr,
                               UNUSED bool_t is_reply)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    જો (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } નહીંતર {
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    પાછા false;
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
પ્રકારવ્યાખ્યા સંરચના {
    પૂર્ણાંક bp_અંક;
    word_t vaddr, reason;
} getAndResetActiveBreakpoint_t;

સ્થિર getAndResetActiveBreakpoint_t getAndResetActiveBreakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    getAndResetActiveBreakpoint_t ret;

    /* Read from the hardware regs, not user context */
    word_t dr6 = readDr6Reg();
    જો (dr6 & BIT(0)) {
        ret.bp_અંક = 0;
    } નહીંતર જો (dr6 & BIT(1)) {
        ret.bp_અંક = 1;
    } નહીંતર જો (dr6 & BIT(2)) {
        ret.bp_અંક = 2;
    } નહીંતર જો (dr6 & BIT(3)) {
        ret.bp_અંક = 3;
    } નહીંતર {
        ret.bp_અંક = -1;
        પાછા ret;
    }

    tmp = convertArchToTypeAndAccess(readDr7Context(t), ret.bp_અંક);
    ret.vaddr = getBpVaddrContext(t, ret.bp_અંક);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~BIT(ret.bp_અંક));
    પાછા ret;
}

exception_t handleUserLevelDebugException(પૂર્ણાંક int_vector)
{
    tcb_t *ct;
    getAndResetActiveBreakpoint_t active_bp;
    testAndResetSingleStepException_t single_step_info;

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_UserLevelFault;
    ksKernelEntry.word = int_vector;
#endif /* DEBUG */

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_start();
#endif

    ct = ગાંઠ_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    જો (int_vector == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(ગાંઠ_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } નહીંતર {
        /* Hardware breakpoint trigger is detected using DR6 */
        active_bp = getAndResetActiveBreakpoint(ct);
        જો (active_bp.bp_અંક >= 0) {
            current_fault = seL4_Fault_DebugException_new(active_bp.vaddr,
                                                          active_bp.bp_અંક,
                                                          active_bp.reason);
        } નહીંતર {
            single_step_info = testAndResetSingleStepException(ct);
            જો (single_step_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                જો (singleStepFaultCounterReady(ct) == false) {
                    પાછા EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_step_info.instr_vaddr,
                                                              0, seL4_SingleStep);
            } નહીંતર {
                પાછા EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(ગાંઠ_STATE(ksCurThread));

    schedule();
    activateThread();

    પાછા EXCEPTION_NONE;
}

BOOT_CODE bool_t Arch_initHardwareBreakpoints(શૂન્ય)
{
    x86_cpu_identity_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    જો (modelinfo->family == 15) {
        જો (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    જો (modelinfo->family == 6) {
        જો (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    પાછા true;
}

શૂન્ય Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, કદ(*uds));

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

#endif
