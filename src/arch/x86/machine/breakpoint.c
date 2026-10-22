/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API

#شامل <arch/machine/debug.h>
#شامل <mode/machine/debug.h>
#شامل <arch/machine.h>
#شامل <machine/registerset.h>
#شامل <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#تعریف_کرو X86_DEBUG_BP_سائز_1_B                (0x0u)
#تعریف_کرو X86_DEBUG_BP_سائز_2_B                (0x1u)
#تعریف_کرو X86_DEBUG_BP_سائز_4_B                (0x3u)
#تعریف_کرو X86_DEBUG_BP_سائز_8_B                (0x2u)

#تعریف_کرو X86_DEBUG_BP0_سائز_سرکاؤ            (18)
#تعریف_کرو X86_DEBUG_BP1_سائز_سرکاؤ            (22)
#تعریف_کرو X86_DEBUG_BP2_سائز_سرکاؤ            (26)
#تعریف_کرو X86_DEBUG_BP3_سائز_سرکاؤ            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#تعریف_کرو X86_DEBUG_BP_TYPE_IO                (0x2u)
#تعریف_کرو X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#تعریف_کرو X86_DEBUG_BP_TYPE_ڈیٹا_WRITE        (0x1u)
#تعریف_کرو X86_DEBUG_BP_TYPE_ڈیٹا_READWRITE    (0x3u)

#تعریف_کرو X86_DEBUG_BP0_TYPE_سرکاؤ            (16)
#تعریف_کرو X86_DEBUG_BP1_TYPE_سرکاؤ            (20)
#تعریف_کرو X86_DEBUG_BP2_TYPE_سرکاؤ            (24)
#تعریف_کرو X86_DEBUG_BP3_TYPE_سرکاؤ            (28)

#تعریف_کرو X86_DEBUG_EFLAGS_TRAP_FLAG    ((لفظ_t)ثنائی_ہندسہ(8))
#تعریف_کرو X86_DEBUG_EFLAGS_RESUME_FLAG    ((لفظ_t)ثنائی_ہندسہ(16))
#تعریف_کرو X86_DEBUG_DR6_SINGLE_قدم_FLAG  ((لفظ_t)ثنائی_ہندسہ(14))

#تعریف_کرو X86_DEBUG_DR6_BP_نقاب     (0xFu)

ساکن bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(خالی)
{
    واپس byte8_bps_supported;
}

ساکن اندرونی خالی bitwiseAndDr6Reg(لفظ_t نقاب_2)
{
    لفظ_t tmp;

    tmp = readDr6Reg() & نقاب_2;
    writeDr6Reg(tmp);
}

ساکن اندرونی لفظ_t readDr7Context(tcb_t *t)
{
    واپس t->tcbArch.tcbContext.breakpointState.dr[5];
}

ساکن اندرونی خالی bitwiseOrDr7Context(tcb_t *t, لفظ_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ساکن اندرونی خالی bitwiseAndDr7Context(tcb_t *t, لفظ_t نقاب_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= نقاب_2;
}

ساکن خالی unset_Dr7_بٹ_For(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t نقاب_2;

    انتخاب (bp_عدد) {
    حالت 0:
        نقاب_2 = (0x3u << X86_DEBUG_BP0_سائز_سرکاؤ) | (0x3u << X86_DEBUG_BP0_TYPE_سرکاؤ);
        توڑو;
    حالت 1:
        نقاب_2 = (0x3u << X86_DEBUG_BP1_سائز_سرکاؤ) | (0x3u << X86_DEBUG_BP1_TYPE_سرکاؤ);
        توڑو;
    حالت 2:
        نقاب_2 = (0x3u << X86_DEBUG_BP2_سائز_سرکاؤ) | (0x3u << X86_DEBUG_BP2_TYPE_سرکاؤ);
        توڑو;
    طےشدہ: /* 3 */
        أكد(bp_عدد == 3);
        نقاب_2 = (0x3u << X86_DEBUG_BP3_سائز_سرکاؤ) | (0x3u << X86_DEBUG_BP3_TYPE_سرکاؤ);
        توڑو;
    }

    نقاب_2 = ~نقاب_2;
    bitwiseAndDr7Context(t, نقاب_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
خالص ساکن اندرونی لفظ_t convert_Type_And_Access_مقصد_Arch(uint16_t bp_عدد, لفظ_t type, لفظ_t rw)
{
    انتخاب (type) {
    حالت seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        توڑو;
    طےشدہ: /* seL4_DataBreakpoint */
        أكد(type == se_L4_ڈیٹا_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_ڈیٹا_WRITE
               : X86_DEBUG_BP_TYPE_ڈیٹا_READWRITE;
    }

    انتخاب (bp_عدد) {
    حالت 0:
        واپس type << X86_DEBUG_BP0_TYPE_سرکاؤ;
    حالت 1:
        واپس type << X86_DEBUG_BP1_TYPE_سرکاؤ;
    حالت 2:
        واپس type << X86_DEBUG_BP2_TYPE_سرکاؤ;
    طےشدہ: /* 3 */
        أكد(bp_عدد == 3);
        واپس type << X86_DEBUG_BP3_TYPE_سرکاؤ;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
قسمتعریف ساخت {
    لفظ_t type, rw;
} convertedTypeAndAccess_t;

خالص ساکن اندرونی convertedTypeAndAccess_t convert_Arch_مقصد_Type_And_Access(لفظ_t dr7, uint16_t bp_عدد)
{
    convertedTypeAndAccess_t ret;

    انتخاب (bp_عدد) {
    حالت 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_سرکاؤ;
        dr7 >>= X86_DEBUG_BP0_TYPE_سرکاؤ;
        توڑو;
    حالت 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_سرکاؤ;
        dr7 >>= X86_DEBUG_BP1_TYPE_سرکاؤ;
        توڑو;
    حالت 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_سرکاؤ;
        dr7 >>= X86_DEBUG_BP2_TYPE_سرکاؤ;
        توڑو;
    طےشدہ: /* 3 */
        أكد(bp_عدد == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_سرکاؤ;
        dr7 >>= X86_DEBUG_BP3_TYPE_سرکاؤ;
    }

    انتخاب (dr7) {
    حالت X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        توڑو;
    حالت X86_DEBUG_BP_TYPE_ڈیٹا_WRITE:
        ret.type = se_L4_ڈیٹا_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        توڑو;
    طےشدہ: /* Read-write */
        أكد(dr7 == X86_DEBUG_BP_TYPE_ڈیٹا_READWRITE);
        ret.type = se_L4_ڈیٹا_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        توڑو;
    }
    واپس ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
خالص ساکن اندرونی لفظ_t convert_سائز_مقصد_Arch(uint16_t bp_عدد, لفظ_t type, لفظ_t سائز)
{
    اگر (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        سائز = 0;
    } ورنہ {
        انتخاب (سائز) {
        حالت 1:
            سائز = X86_DEBUG_BP_سائز_1_B;
            توڑو;
        حالت 2:
            سائز = X86_DEBUG_BP_سائز_2_B;
            توڑو;
        حالت 8:
            سائز = X86_DEBUG_BP_سائز_8_B;
            توڑو;
        طےشدہ: /* 4B */
            أكد(سائز == 4);
            سائز = X86_DEBUG_BP_سائز_4_B;
        }
    }

    انتخاب (bp_عدد) {
    حالت 0:
        واپس سائز << X86_DEBUG_BP0_سائز_سرکاؤ;
    حالت 1:
        واپس سائز << X86_DEBUG_BP1_سائز_سرکاؤ;
    حالت 2:
        واپس سائز << X86_DEBUG_BP2_سائز_سرکاؤ;
    طےشدہ: /* 3 */
        أكد(bp_عدد == 3);
        واپس سائز << X86_DEBUG_BP3_سائز_سرکاؤ;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
خالص ساکن اندرونی لفظ_t convert_Arch_مقصد_سائز(لفظ_t dr7, uint16_t bp_عدد)
{
    لفظ_t type;

    انتخاب (bp_عدد) {
    حالت 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_سرکاؤ);
        type >>= X86_DEBUG_BP0_TYPE_سرکاؤ;
        dr7 &= 0x3u << X86_DEBUG_BP0_سائز_سرکاؤ;
        dr7 >>= X86_DEBUG_BP0_سائز_سرکاؤ;
        توڑو;
    حالت 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_سرکاؤ);
        type >>= X86_DEBUG_BP1_TYPE_سرکاؤ;
        dr7 &= 0x3u << X86_DEBUG_BP1_سائز_سرکاؤ;
        dr7 >>= X86_DEBUG_BP1_سائز_سرکاؤ;
        توڑو;
    حالت 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_سرکاؤ);
        type >>= X86_DEBUG_BP2_TYPE_سرکاؤ;
        dr7 &= 0x3u << X86_DEBUG_BP2_سائز_سرکاؤ;
        dr7 >>= X86_DEBUG_BP2_سائز_سرکاؤ;
        توڑو;
    طےشدہ: /* 3 */
        أكد(bp_عدد == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_سرکاؤ);
        type >>= X86_DEBUG_BP3_TYPE_سرکاؤ;
        dr7 &= 0x3u << X86_DEBUG_BP3_سائز_سرکاؤ;
        dr7 >>= X86_DEBUG_BP3_سائز_سرکاؤ;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    اگر (type == X86_DEBUG_BP_TYPE_INSTR) {
        واپس 0;
    }

    انتخاب (dr7) {
    حالت X86_DEBUG_BP_سائز_1_B:
        واپس 1;
    حالت X86_DEBUG_BP_سائز_2_B:
        واپس 2;
    حالت X86_DEBUG_BP_سائز_8_B:
        واپس 8;
    طےشدہ: /* 4B */
        أكد(dr7 == X86_DEBUG_BP_سائز_4_B);
        واپس 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساکن خالی enableBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t enable_ثنائی_ہندسہ;

    أكد(t != تہی);
    أكد(bp_عدد < X86_DEBUG_BP_N_REGS);

    انتخاب (bp_عدد) {
    حالت 0:
        enable_ثنائی_ہندسہ = X86_DEBUG_BP0_ENABLE_ثنائی_ہندسہ;
        توڑو;
    حالت 1:
        enable_ثنائی_ہندسہ = X86_DEBUG_BP1_ENABLE_ثنائی_ہندسہ;
        توڑو;
    حالت 2:
        enable_ثنائی_ہندسہ = X86_DEBUG_BP2_ENABLE_ثنائی_ہندسہ;
        توڑو;
    طےشدہ:
        enable_ثنائی_ہندسہ = X86_DEBUG_BP3_ENABLE_ثنائی_ہندسہ;
        توڑو;
    }

    bitwiseOrDr7Context(t, enable_ثنائی_ہندسہ);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساکن خالی disableBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t disable_نقاب;

    أكد(t != تہی);
    أكد(bp_عدد < X86_DEBUG_BP_N_REGS);

    انتخاب (bp_عدد) {
    حالت 0:
        disable_نقاب = ~X86_DEBUG_BP0_ENABLE_ثنائی_ہندسہ;
        توڑو;
    حالت 1:
        disable_نقاب = ~X86_DEBUG_BP1_ENABLE_ثنائی_ہندسہ;
        توڑو;
    حالت 2:
        disable_نقاب = ~X86_DEBUG_BP2_ENABLE_ثنائی_ہندسہ;
        توڑو;
    طےشدہ:
        disable_نقاب = ~X86_DEBUG_BP3_ENABLE_ثنائی_ہندسہ;
        توڑو;
    }

    bitwiseAndDr7Context(t, disable_نقاب);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساکن bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t dr7;

    أكد(t != تہی);
    أكد(bp_عدد < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    انتخاب (bp_عدد) {
    حالت 0:
        واپس !!(dr7 & X86_DEBUG_BP0_ENABLE_ثنائی_ہندسہ);
    حالت 1:
        واپس !!(dr7 & X86_DEBUG_BP1_ENABLE_ثنائی_ہندسہ);
    حالت 2:
        واپس !!(dr7 & X86_DEBUG_BP2_ENABLE_ثنائی_ہندسہ);
    طےشدہ:
        واپس !!(dr7 & X86_DEBUG_BP3_ENABLE_ثنائی_ہندسہ);
    }
}

ساکن خالی سیٹ_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_عدد, لفظ_t vaddr)
{
    أكد(t != تہی);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    انتخاب (bp_عدد) {
    حالت 0:
        ubs->dr[0] = vaddr;
        توڑو;
    حالت 1:
        ubs->dr[1] = vaddr;
        توڑو;
    حالت 2:
        ubs->dr[2] = vaddr;
        توڑو;
    طےشدہ:
        أكد(bp_عدد == 3);
        ubs->dr[3] = vaddr;
        توڑو;
    }
    واپس;
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
خالی سیٹ_Breakpoint(tcb_t *t,
                   uint16_t bp_عدد, لفظ_t vaddr, لفظ_t types, لفظ_t سائز, لفظ_t rw)
{
    لفظ_t dr7val;

    أكد(t != تہی);

    dr7val = convert_Type_And_Access_مقصد_Arch(bp_عدد, types, rw);
    dr7val |= convert_سائز_مقصد_Arch(bp_عدد, types, سائز);

    سیٹ_Bp_Vaddr_Context(t, bp_عدد, vaddr);
    unset_Dr7_بٹ_For(t, bp_عدد);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_عدد);
}

ساکن لفظ_t getBpVaddrContext(tcb_t *t, uint16_t bp_عدد)
{
    أكد(t != تہی);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    انتخاب (bp_عدد) {
    حالت 0:
        واپس ubs->dr[0];
    حالت 1:
        واپس ubs->dr[1];
    حالت 2:
        واپس ubs->dr[2];
    طےشدہ:
        أكد(bp_عدد == 3);
        واپس ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_عدد);
    ret.سائز = convert_Arch_مقصد_سائز(dr7val, bp_عدد);
    res = convert_Arch_مقصد_Type_And_Access(dr7val, bp_عدد);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_عدد);
    واپس ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
خالی unsetBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    disableBreakpoint(t, bp_عدد);
    unset_Dr7_بٹ_For(t, bp_عدد);
    سیٹ_Bp_Vaddr_Context(t, bp_عدد, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
قسمتعریف ساخت {
    bool_t ret;
    لفظ_t instr_vaddr;
} test_And_Reset_Single_قدم_Exception_t;

ساکن test_And_Reset_Single_قدم_Exception_t test_And_Reset_Single_قدم_Exception(tcb_t *t)
{
    test_And_Reset_Single_قدم_Exception_t ret;
    لفظ_t dr6;

    dr6 = readDr6Reg();
    اگر (!(dr6 & X86_DEBUG_DR6_SINGLE_قدم_FLAG)) {
        ret.ret = false;
        واپس ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_قدم_FLAG);

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
    واپس ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_عدد, لفظ_t n_instr,
                               غیر_مستعمل bool_t is_جواب)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    اگر (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } ورنہ {
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    واپس false;
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
قسمتعریف ساخت {
    صحیح bp_عدد;
    لفظ_t vaddr, reason;
} get_And_Reset_فعال_Breakpoint_t;

ساکن get_And_Reset_فعال_Breakpoint_t get_And_Reset_فعال_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_فعال_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    لفظ_t dr6 = readDr6Reg();
    اگر (dr6 & ثنائی_ہندسہ(0)) {
        ret.bp_عدد = 0;
    } ورنہ اگر (dr6 & ثنائی_ہندسہ(1)) {
        ret.bp_عدد = 1;
    } ورنہ اگر (dr6 & ثنائی_ہندسہ(2)) {
        ret.bp_عدد = 2;
    } ورنہ اگر (dr6 & ثنائی_ہندسہ(3)) {
        ret.bp_عدد = 3;
    } ورنہ {
        ret.bp_عدد = -1;
        واپس ret;
    }

    tmp = convert_Arch_مقصد_Type_And_Access(readDr7Context(t), ret.bp_عدد);
    ret.vaddr = getBpVaddrContext(t, ret.bp_عدد);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~ثنائی_ہندسہ(ret.bp_عدد));
    واپس ret;
}

exception_t handleUserLevelDebugException(صحیح int_سمتیہ)
{
    tcb_t *ct;
    get_And_Reset_فعال_Breakpoint_t فعال_bp;
    test_And_Reset_Single_قدم_Exception_t single_قدم_info;

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_User_Level_Fault;
    ks_Kernel_اندراج.لفظ = int_سمتیہ;
#اگر_ختم /* DEBUG */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_آغاز();
#اگر_ختم

    ct = گرہ_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    اگر (int_سمتیہ == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(گرہ_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } ورنہ {
        /* Hardware breakpoint trigger is detected using DR6 */
        فعال_bp = get_And_Reset_فعال_Breakpoint(ct);
        اگر (فعال_bp.bp_عدد >= 0) {
            current_fault = seL4_Fault_DebugException_new(فعال_bp.vaddr,
                                                          فعال_bp.bp_عدد,
                                                          فعال_bp.reason);
        } ورنہ {
            single_قدم_info = test_And_Reset_Single_قدم_Exception(ct);
            اگر (single_قدم_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                اگر (single_قدم_Fault_شمار_Ready(ct) == false) {
                    واپس EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_قدم_info.instr_vaddr,
                                                              0, se_L4_Single_قدم);
            } ورنہ {
                واپس EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(گرہ_STATE(ksCurThread));

    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}

BOOT_رمز bool_t Arch_initHardwareBreakpoints(خالی)
{
    x86_cpu_شناختی_عنصر_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    اگر (modelinfo->family == 15) {
        اگر (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    اگر (modelinfo->family == 6) {
        اگر (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    واپس true;
}

خالی Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, حجم(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(ثنائی_ہندسہ(0)
                   | ثنائی_ہندسہ(1)
                   | ثنائی_ہندسہ(2)
                   | ثنائی_ہندسہ(3)
                   | X86_DEBUG_DR6_SINGLE_قدم_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_ثنائی_ہندسہ | X86_DEBUG_BP1_ENABLE_ثنائی_ہندسہ
                   | X86_DEBUG_BP2_ENABLE_ثنائی_ہندسہ
                   | X86_DEBUG_BP3_ENABLE_ثنائی_ہندسہ);
}

#اگر_ختم
