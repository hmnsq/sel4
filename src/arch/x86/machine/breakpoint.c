/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API

#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <mode/machine/debug.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#నిర్వచించు X86_DEBUG_BP_కొలత_1_B                (0x0u)
#నిర్వచించు X86_DEBUG_BP_కొలత_2_B                (0x1u)
#నిర్వచించు X86_DEBUG_BP_కొలత_4_B                (0x3u)
#నిర్వచించు X86_DEBUG_BP_కొలత_8_B                (0x2u)

#నిర్వచించు X86_DEBUG_BP0_కొలత_జరుపు            (18)
#నిర్వచించు X86_DEBUG_BP1_కొలత_జరుపు            (22)
#నిర్వచించు X86_DEBUG_BP2_కొలత_జరుపు            (26)
#నిర్వచించు X86_DEBUG_BP3_కొలత_జరుపు            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#నిర్వచించు X86_DEBUG_BP_TYPE_IO                (0x2u)
#నిర్వచించు X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#నిర్వచించు X86_DEBUG_BP_TYPE_దత్తాంశం_WRITE        (0x1u)
#నిర్వచించు X86_DEBUG_BP_TYPE_దత్తాంశం_READWRITE    (0x3u)

#నిర్వచించు X86_DEBUG_BP0_TYPE_జరుపు            (16)
#నిర్వచించు X86_DEBUG_BP1_TYPE_జరుపు            (20)
#నిర్వచించు X86_DEBUG_BP2_TYPE_జరుపు            (24)
#నిర్వచించు X86_DEBUG_BP3_TYPE_జరుపు            (28)

#నిర్వచించు X86_DEBUG_EFLAGS_TRAP_FLAG    ((మాట_t)బిట్టు(8))
#నిర్వచించు X86_DEBUG_EFLAGS_RESUME_FLAG    ((మాట_t)బిట్టు(16))
#నిర్వచించు X86_DEBUG_DR6_SINGLE_అడుగు_FLAG  ((మాట_t)బిట్టు(14))

#నిర్వచించు X86_DEBUG_DR6_BP_ముసుగు     (0xFu)

స్థిర bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(శూన్య)
{
    తిరిగి byte8_bps_supported;
}

స్థిర అంతఃవరుస శూన్య bitwiseAndDr6Reg(మాట_t ముసుగు_2)
{
    మాట_t tmp;

    tmp = readDr6Reg() & ముసుగు_2;
    writeDr6Reg(tmp);
}

స్థిర అంతఃవరుస మాట_t readDr7Context(tcb_t *t)
{
    తిరిగి t->tcbArch.tcbContext.breakpointState.dr[5];
}

స్థిర అంతఃవరుస శూన్య bitwiseOrDr7Context(tcb_t *t, మాట_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

స్థిర అంతఃవరుస శూన్య bitwiseAndDr7Context(tcb_t *t, మాట_t ముసుగు_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= ముసుగు_2;
}

స్థిర శూన్య unset_Dr7_బిట్లు_For(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t ముసుగు_2;

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        ముసుగు_2 = (0x3u << X86_DEBUG_BP0_కొలత_జరుపు) | (0x3u << X86_DEBUG_BP0_TYPE_జరుపు);
        ఆపు;
    స్థితి 1:
        ముసుగు_2 = (0x3u << X86_DEBUG_BP1_కొలత_జరుపు) | (0x3u << X86_DEBUG_BP1_TYPE_జరుపు);
        ఆపు;
    స్థితి 2:
        ముసుగు_2 = (0x3u << X86_DEBUG_BP2_కొలత_జరుపు) | (0x3u << X86_DEBUG_BP2_TYPE_జరుపు);
        ఆపు;
    అప్రమేయ: /* 3 */
        ప్రతిజ్ఞా(bp_అంకె == 3);
        ముసుగు_2 = (0x3u << X86_DEBUG_BP3_కొలత_జరుపు) | (0x3u << X86_DEBUG_BP3_TYPE_జరుపు);
        ఆపు;
    }

    ముసుగు_2 = ~ముసుగు_2;
    bitwiseAndDr7Context(t, ముసుగు_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
శుద్ధ స్థిర అంతఃవరుస మాట_t convert_Type_And_Access_గమ్యం_Arch(uint16_t bp_అంకె, మాట_t type, మాట_t rw)
{
    ఎంపిక (type) {
    స్థితి seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        ఆపు;
    అప్రమేయ: /* seL4_DataBreakpoint */
        ప్రతిజ్ఞా(type == se_L4_దత్తాంశం_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_దత్తాంశం_WRITE
               : X86_DEBUG_BP_TYPE_దత్తాంశం_READWRITE;
    }

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        తిరిగి type << X86_DEBUG_BP0_TYPE_జరుపు;
    స్థితి 1:
        తిరిగి type << X86_DEBUG_BP1_TYPE_జరుపు;
    స్థితి 2:
        తిరిగి type << X86_DEBUG_BP2_TYPE_జరుపు;
    అప్రమేయ: /* 3 */
        ప్రతిజ్ఞా(bp_అంకె == 3);
        తిరిగి type << X86_DEBUG_BP3_TYPE_జరుపు;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
రకనిర్వచనం నిర్మాణం {
    మాట_t type, rw;
} convertedTypeAndAccess_t;

శుద్ధ స్థిర అంతఃవరుస convertedTypeAndAccess_t convert_Arch_గమ్యం_Type_And_Access(మాట_t dr7, uint16_t bp_అంకె)
{
    convertedTypeAndAccess_t ret;

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_జరుపు;
        dr7 >>= X86_DEBUG_BP0_TYPE_జరుపు;
        ఆపు;
    స్థితి 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_జరుపు;
        dr7 >>= X86_DEBUG_BP1_TYPE_జరుపు;
        ఆపు;
    స్థితి 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_జరుపు;
        dr7 >>= X86_DEBUG_BP2_TYPE_జరుపు;
        ఆపు;
    అప్రమేయ: /* 3 */
        ప్రతిజ్ఞా(bp_అంకె == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_జరుపు;
        dr7 >>= X86_DEBUG_BP3_TYPE_జరుపు;
    }

    ఎంపిక (dr7) {
    స్థితి X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        ఆపు;
    స్థితి X86_DEBUG_BP_TYPE_దత్తాంశం_WRITE:
        ret.type = se_L4_దత్తాంశం_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        ఆపు;
    అప్రమేయ: /* Read-write */
        ప్రతిజ్ఞా(dr7 == X86_DEBUG_BP_TYPE_దత్తాంశం_READWRITE);
        ret.type = se_L4_దత్తాంశం_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        ఆపు;
    }
    తిరిగి ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
శుద్ధ స్థిర అంతఃవరుస మాట_t convert_కొలత_గమ్యం_Arch(uint16_t bp_అంకె, మాట_t type, మాట_t కొలత)
{
    ఒకవేళ (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        కొలత = 0;
    } లేకపోతే {
        ఎంపిక (కొలత) {
        స్థితి 1:
            కొలత = X86_DEBUG_BP_కొలత_1_B;
            ఆపు;
        స్థితి 2:
            కొలత = X86_DEBUG_BP_కొలత_2_B;
            ఆపు;
        స్థితి 8:
            కొలత = X86_DEBUG_BP_కొలత_8_B;
            ఆపు;
        అప్రమేయ: /* 4B */
            ప్రతిజ్ఞా(కొలత == 4);
            కొలత = X86_DEBUG_BP_కొలత_4_B;
        }
    }

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        తిరిగి కొలత << X86_DEBUG_BP0_కొలత_జరుపు;
    స్థితి 1:
        తిరిగి కొలత << X86_DEBUG_BP1_కొలత_జరుపు;
    స్థితి 2:
        తిరిగి కొలత << X86_DEBUG_BP2_కొలత_జరుపు;
    అప్రమేయ: /* 3 */
        ప్రతిజ్ఞా(bp_అంకె == 3);
        తిరిగి కొలత << X86_DEBUG_BP3_కొలత_జరుపు;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
శుద్ధ స్థిర అంతఃవరుస మాట_t convert_Arch_గమ్యం_కొలత(మాట_t dr7, uint16_t bp_అంకె)
{
    మాట_t type;

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_జరుపు);
        type >>= X86_DEBUG_BP0_TYPE_జరుపు;
        dr7 &= 0x3u << X86_DEBUG_BP0_కొలత_జరుపు;
        dr7 >>= X86_DEBUG_BP0_కొలత_జరుపు;
        ఆపు;
    స్థితి 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_జరుపు);
        type >>= X86_DEBUG_BP1_TYPE_జరుపు;
        dr7 &= 0x3u << X86_DEBUG_BP1_కొలత_జరుపు;
        dr7 >>= X86_DEBUG_BP1_కొలత_జరుపు;
        ఆపు;
    స్థితి 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_జరుపు);
        type >>= X86_DEBUG_BP2_TYPE_జరుపు;
        dr7 &= 0x3u << X86_DEBUG_BP2_కొలత_జరుపు;
        dr7 >>= X86_DEBUG_BP2_కొలత_జరుపు;
        ఆపు;
    అప్రమేయ: /* 3 */
        ప్రతిజ్ఞా(bp_అంకె == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_జరుపు);
        type >>= X86_DEBUG_BP3_TYPE_జరుపు;
        dr7 &= 0x3u << X86_DEBUG_BP3_కొలత_జరుపు;
        dr7 >>= X86_DEBUG_BP3_కొలత_జరుపు;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    ఒకవేళ (type == X86_DEBUG_BP_TYPE_INSTR) {
        తిరిగి 0;
    }

    ఎంపిక (dr7) {
    స్థితి X86_DEBUG_BP_కొలత_1_B:
        తిరిగి 1;
    స్థితి X86_DEBUG_BP_కొలత_2_B:
        తిరిగి 2;
    స్థితి X86_DEBUG_BP_కొలత_8_B:
        తిరిగి 8;
    అప్రమేయ: /* 4B */
        ప్రతిజ్ఞా(dr7 == X86_DEBUG_BP_కొలత_4_B);
        తిరిగి 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
స్థిర శూన్య enableBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t enable_బిట్టు;

    ప్రతిజ్ఞా(t != ఖాళీ);
    ప్రతిజ్ఞా(bp_అంకె < X86_DEBUG_BP_N_REGS);

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        enable_బిట్టు = X86_DEBUG_BP0_ENABLE_బిట్టు;
        ఆపు;
    స్థితి 1:
        enable_బిట్టు = X86_DEBUG_BP1_ENABLE_బిట్టు;
        ఆపు;
    స్థితి 2:
        enable_బిట్టు = X86_DEBUG_BP2_ENABLE_బిట్టు;
        ఆపు;
    అప్రమేయ:
        enable_బిట్టు = X86_DEBUG_BP3_ENABLE_బిట్టు;
        ఆపు;
    }

    bitwiseOrDr7Context(t, enable_బిట్టు);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
స్థిర శూన్య disableBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t disable_ముసుగు;

    ప్రతిజ్ఞా(t != ఖాళీ);
    ప్రతిజ్ఞా(bp_అంకె < X86_DEBUG_BP_N_REGS);

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        disable_ముసుగు = ~X86_DEBUG_BP0_ENABLE_బిట్టు;
        ఆపు;
    స్థితి 1:
        disable_ముసుగు = ~X86_DEBUG_BP1_ENABLE_బిట్టు;
        ఆపు;
    స్థితి 2:
        disable_ముసుగు = ~X86_DEBUG_BP2_ENABLE_బిట్టు;
        ఆపు;
    అప్రమేయ:
        disable_ముసుగు = ~X86_DEBUG_BP3_ENABLE_బిట్టు;
        ఆపు;
    }

    bitwiseAndDr7Context(t, disable_ముసుగు);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
స్థిర bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t dr7;

    ప్రతిజ్ఞా(t != ఖాళీ);
    ప్రతిజ్ఞా(bp_అంకె < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        తిరిగి !!(dr7 & X86_DEBUG_BP0_ENABLE_బిట్టు);
    స్థితి 1:
        తిరిగి !!(dr7 & X86_DEBUG_BP1_ENABLE_బిట్టు);
    స్థితి 2:
        తిరిగి !!(dr7 & X86_DEBUG_BP2_ENABLE_బిట్టు);
    అప్రమేయ:
        తిరిగి !!(dr7 & X86_DEBUG_BP3_ENABLE_బిట్టు);
    }
}

స్థిర శూన్య సమితి_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_అంకె, మాట_t vaddr)
{
    ప్రతిజ్ఞా(t != ఖాళీ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        ubs->dr[0] = vaddr;
        ఆపు;
    స్థితి 1:
        ubs->dr[1] = vaddr;
        ఆపు;
    స్థితి 2:
        ubs->dr[2] = vaddr;
        ఆపు;
    అప్రమేయ:
        ప్రతిజ్ఞా(bp_అంకె == 3);
        ubs->dr[3] = vaddr;
        ఆపు;
    }
    తిరిగి;
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
శూన్య సమితి_Breakpoint(tcb_t *t,
                   uint16_t bp_అంకె, మాట_t vaddr, మాట_t types, మాట_t కొలత, మాట_t rw)
{
    మాట_t dr7val;

    ప్రతిజ్ఞా(t != ఖాళీ);

    dr7val = convert_Type_And_Access_గమ్యం_Arch(bp_అంకె, types, rw);
    dr7val |= convert_కొలత_గమ్యం_Arch(bp_అంకె, types, కొలత);

    సమితి_Bp_Vaddr_Context(t, bp_అంకె, vaddr);
    unset_Dr7_బిట్లు_For(t, bp_అంకె);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_అంకె);
}

స్థిర మాట_t getBpVaddrContext(tcb_t *t, uint16_t bp_అంకె)
{
    ప్రతిజ్ఞా(t != ఖాళీ);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    ఎంపిక (bp_అంకె) {
    స్థితి 0:
        తిరిగి ubs->dr[0];
    స్థితి 1:
        తిరిగి ubs->dr[1];
    స్థితి 2:
        తిరిగి ubs->dr[2];
    అప్రమేయ:
        ప్రతిజ్ఞా(bp_అంకె == 3);
        తిరిగి ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_అంకె);
    ret.కొలత = convert_Arch_గమ్యం_కొలత(dr7val, bp_అంకె);
    res = convert_Arch_గమ్యం_Type_And_Access(dr7val, bp_అంకె);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_అంకె);
    తిరిగి ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
శూన్య unsetBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    disableBreakpoint(t, bp_అంకె);
    unset_Dr7_బిట్లు_For(t, bp_అంకె);
    సమితి_Bp_Vaddr_Context(t, bp_అంకె, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
రకనిర్వచనం నిర్మాణం {
    bool_t ret;
    మాట_t instr_vaddr;
} test_And_Reset_Single_అడుగు_Exception_t;

స్థిర test_And_Reset_Single_అడుగు_Exception_t test_And_Reset_Single_అడుగు_Exception(tcb_t *t)
{
    test_And_Reset_Single_అడుగు_Exception_t ret;
    మాట_t dr6;

    dr6 = readDr6Reg();
    ఒకవేళ (!(dr6 & X86_DEBUG_DR6_SINGLE_అడుగు_FLAG)) {
        ret.ret = false;
        తిరిగి ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_అడుగు_FLAG);

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
    తిరిగి ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_అంకె, మాట_t n_instr,
                               వాడని bool_t is_జవాబు)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    ఒకవేళ (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } లేకపోతే {
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    తిరిగి false;
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
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం bp_అంకె;
    మాట_t vaddr, reason;
} get_And_Reset_చురుకైన_Breakpoint_t;

స్థిర get_And_Reset_చురుకైన_Breakpoint_t get_And_Reset_చురుకైన_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_చురుకైన_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    మాట_t dr6 = readDr6Reg();
    ఒకవేళ (dr6 & బిట్టు(0)) {
        ret.bp_అంకె = 0;
    } లేకపోతే ఒకవేళ (dr6 & బిట్టు(1)) {
        ret.bp_అంకె = 1;
    } లేకపోతే ఒకవేళ (dr6 & బిట్టు(2)) {
        ret.bp_అంకె = 2;
    } లేకపోతే ఒకవేళ (dr6 & బిట్టు(3)) {
        ret.bp_అంకె = 3;
    } లేకపోతే {
        ret.bp_అంకె = -1;
        తిరిగి ret;
    }

    tmp = convert_Arch_గమ్యం_Type_And_Access(readDr7Context(t), ret.bp_అంకె);
    ret.vaddr = getBpVaddrContext(t, ret.bp_అంకె);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~బిట్టు(ret.bp_అంకె));
    తిరిగి ret;
}

exception_t handleUserLevelDebugException(పూర్ణాంకం int_సదిశ)
{
    tcb_t *ct;
    get_And_Reset_చురుకైన_Breakpoint_t చురుకైన_bp;
    test_And_Reset_Single_అడుగు_Exception_t single_అడుగు_info;

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_User_Level_Fault;
    ks_Kernel_నమోదు.మాట = int_సదిశ;
#అయితే_ముగింపు /* DEBUG */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_ప్రారంభం();
#అయితే_ముగింపు

    ct = కణుపు_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    ఒకవేళ (int_సదిశ == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(కణుపు_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } లేకపోతే {
        /* Hardware breakpoint trigger is detected using DR6 */
        చురుకైన_bp = get_And_Reset_చురుకైన_Breakpoint(ct);
        ఒకవేళ (చురుకైన_bp.bp_అంకె >= 0) {
            current_fault = seL4_Fault_DebugException_new(చురుకైన_bp.vaddr,
                                                          చురుకైన_bp.bp_అంకె,
                                                          చురుకైన_bp.reason);
        } లేకపోతే {
            single_అడుగు_info = test_And_Reset_Single_అడుగు_Exception(ct);
            ఒకవేళ (single_అడుగు_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                ఒకవేళ (single_అడుగు_Fault_Counter_Ready(ct) == false) {
                    తిరిగి EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_అడుగు_info.instr_vaddr,
                                                              0, se_L4_Single_అడుగు);
            } లేకపోతే {
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(కణుపు_STATE(ksCurThread));

    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}

BOOT_సంకేతం bool_t Arch_initHardwareBreakpoints(శూన్య)
{
    x86_cpu_తటస్థం_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    ఒకవేళ (modelinfo->family == 15) {
        ఒకవేళ (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    ఒకవేళ (modelinfo->family == 6) {
        ఒకవేళ (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    తిరిగి true;
}

శూన్య Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, పరిమాణం(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(బిట్టు(0)
                   | బిట్టు(1)
                   | బిట్టు(2)
                   | బిట్టు(3)
                   | X86_DEBUG_DR6_SINGLE_అడుగు_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_బిట్టు | X86_DEBUG_BP1_ENABLE_బిట్టు
                   | X86_DEBUG_BP2_ENABLE_బిట్టు
                   | X86_DEBUG_BP3_ENABLE_బిట్టు);
}

#అయితే_ముగింపు
