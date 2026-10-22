/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API

#ضمن <arch/machine/debug.h>
#ضمن <mode/machine/debug.h>
#ضمن <arch/machine.h>
#ضمن <machine/registerset.h>
#ضمن <sel4/plat/api/constants.h> /* seL4_NumHWBReakpoints */

/* Intel manual Vol3, 17.2.4 */
#تعریف_کن X86_DEBUG_BP_اندازه_1_B                (0x0u)
#تعریف_کن X86_DEBUG_BP_اندازه_2_B                (0x1u)
#تعریف_کن X86_DEBUG_BP_اندازه_4_B                (0x3u)
#تعریف_کن X86_DEBUG_BP_اندازه_8_B                (0x2u)

#تعریف_کن X86_DEBUG_BP0_اندازه_لغزش            (18)
#تعریف_کن X86_DEBUG_BP1_اندازه_لغزش            (22)
#تعریف_کن X86_DEBUG_BP2_اندازه_لغزش            (26)
#تعریف_کن X86_DEBUG_BP3_اندازه_لغزش            (30)

/* NOTE: Intel manual 17.2.4:
 * I/O breakpoints are supported by every processor later than i486, but only
 * when CR4.DE=1.
 * When CR4.DE=0, or if processor is earlier than i586, this bit is "Undefined",
 * which is not the same as "Reserved", so it won't trigger an exception - it
 * will just cause an undefined reaction from the CPU.
 */
#تعریف_کن X86_DEBUG_BP_TYPE_IO                (0x2u)
#تعریف_کن X86_DEBUG_BP_TYPE_INSTR             (0x0u)
#تعریف_کن X86_DEBUG_BP_TYPE_داده_WRITE        (0x1u)
#تعریف_کن X86_DEBUG_BP_TYPE_داده_READWRITE    (0x3u)

#تعریف_کن X86_DEBUG_BP0_TYPE_لغزش            (16)
#تعریف_کن X86_DEBUG_BP1_TYPE_لغزش            (20)
#تعریف_کن X86_DEBUG_BP2_TYPE_لغزش            (24)
#تعریف_کن X86_DEBUG_BP3_TYPE_لغزش            (28)

#تعریف_کن X86_DEBUG_EFLAGS_TRAP_FLAG    ((کلمه_t)بیت(8))
#تعریف_کن X86_DEBUG_EFLAGS_RESUME_FLAG    ((کلمه_t)بیت(16))
#تعریف_کن X86_DEBUG_DR6_SINGLE_گام_FLAG  ((کلمه_t)بیت(14))

#تعریف_کن X86_DEBUG_DR6_BP_ماسک     (0xFu)

ساكن bool_t byte8_bps_supported = false;

bool_t byte8BreakpointsSupported(فراغ)
{
    أرجع byte8_bps_supported;
}

ساكن مضمن فراغ bitwiseAndDr6Reg(کلمه_t ماسک_2)
{
    کلمه_t tmp;

    tmp = readDr6Reg() & ماسک_2;
    writeDr6Reg(tmp);
}

ساكن مضمن کلمه_t readDr7Context(tcb_t *t)
{
    أرجع t->tcbArch.tcbContext.breakpointState.dr[5];
}

ساكن مضمن فراغ bitwiseOrDr7Context(tcb_t *t, کلمه_t val)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] |= val;
}

ساكن مضمن فراغ bitwiseAndDr7Context(tcb_t *t, کلمه_t ماسک_2)
{
    t->tcbArch.tcbContext.breakpointState.dr[5] &= ماسک_2;
}

ساكن فراغ unset_Dr7_بیتها_For(tcb_t *t, uint16_t bp_رقم)
{
    کلمه_t ماسک_2;

    بدل (bp_رقم) {
    حالة 0:
        ماسک_2 = (0x3u << X86_DEBUG_BP0_اندازه_لغزش) | (0x3u << X86_DEBUG_BP0_TYPE_لغزش);
        اكسر;
    حالة 1:
        ماسک_2 = (0x3u << X86_DEBUG_BP1_اندازه_لغزش) | (0x3u << X86_DEBUG_BP1_TYPE_لغزش);
        اكسر;
    حالة 2:
        ماسک_2 = (0x3u << X86_DEBUG_BP2_اندازه_لغزش) | (0x3u << X86_DEBUG_BP2_TYPE_لغزش);
        اكسر;
    افتراضي: /* 3 */
        أكد(bp_رقم == 3);
        ماسک_2 = (0x3u << X86_DEBUG_BP3_اندازه_لغزش) | (0x3u << X86_DEBUG_BP3_TYPE_لغزش);
        اكسر;
    }

    ماسک_2 = ~ماسک_2;
    bitwiseAndDr7Context(t, ماسک_2);
}

/** Converts an seL4_BreakpointType value into the underlying hardware
 * equivalent.
 * @param bp_num Breakpoint number.
 * @param type One of the values of seL4_BreakpointType.
 * @param rw Access trigger condition (read/write).
 * @return Hardware specific register value representing the inputs.
 */
خالص ساكن مضمن کلمه_t convert_Type_And_Access_مقصد_Arch(uint16_t bp_رقم, کلمه_t type, کلمه_t rw)
{
    بدل (type) {
    حالة seL4_InstructionBreakpoint:
        type = X86_DEBUG_BP_TYPE_INSTR;
        اكسر;
    افتراضي: /* seL4_DataBreakpoint */
        أكد(type == se_L4_داده_Breakpoint);
        type = (rw == seL4_BreakOnWrite)
               ? X86_DEBUG_BP_TYPE_داده_WRITE
               : X86_DEBUG_BP_TYPE_داده_READWRITE;
    }

    بدل (bp_رقم) {
    حالة 0:
        أرجع type << X86_DEBUG_BP0_TYPE_لغزش;
    حالة 1:
        أرجع type << X86_DEBUG_BP1_TYPE_لغزش;
    حالة 2:
        أرجع type << X86_DEBUG_BP2_TYPE_لغزش;
    افتراضي: /* 3 */
        أكد(bp_رقم == 3);
        أرجع type << X86_DEBUG_BP3_TYPE_لغزش;
    }
}

/** Reverse of convertTypeAndAccessToArch(): converts hardware values into
 * seL4 API values.
 * @param dr7 Hardware register value as input for conversion.
 * @param bp_num Breakpoint number.
 * @param type[out] Converted type value.
 * @param rw[out] Converted output access trigger value.
 */
تعريفنوع بنية {
    کلمه_t type, rw;
} convertedTypeAndAccess_t;

خالص ساكن مضمن convertedTypeAndAccess_t convert_Arch_مقصد_Type_And_Access(کلمه_t dr7, uint16_t bp_رقم)
{
    convertedTypeAndAccess_t ret;

    بدل (bp_رقم) {
    حالة 0:
        dr7 &= 0x3u << X86_DEBUG_BP0_TYPE_لغزش;
        dr7 >>= X86_DEBUG_BP0_TYPE_لغزش;
        اكسر;
    حالة 1:
        dr7 &= 0x3u << X86_DEBUG_BP1_TYPE_لغزش;
        dr7 >>= X86_DEBUG_BP1_TYPE_لغزش;
        اكسر;
    حالة 2:
        dr7 &= 0x3u << X86_DEBUG_BP2_TYPE_لغزش;
        dr7 >>= X86_DEBUG_BP2_TYPE_لغزش;
        اكسر;
    افتراضي: /* 3 */
        أكد(bp_رقم == 3);
        dr7 &= 0x3u << X86_DEBUG_BP3_TYPE_لغزش;
        dr7 >>= X86_DEBUG_BP3_TYPE_لغزش;
    }

    بدل (dr7) {
    حالة X86_DEBUG_BP_TYPE_INSTR:
        ret.type = seL4_InstructionBreakpoint;
        ret.rw = seL4_BreakOnRead;
        اكسر;
    حالة X86_DEBUG_BP_TYPE_داده_WRITE:
        ret.type = se_L4_داده_Breakpoint;
        ret.rw = seL4_BreakOnWrite;
        اكسر;
    افتراضي: /* Read-write */
        أكد(dr7 == X86_DEBUG_BP_TYPE_داده_READWRITE);
        ret.type = se_L4_داده_Breakpoint;
        ret.rw = seL4_BreakOnReadWrite;
        اكسر;
    }
    أرجع ret;
}

/** Converts an integer size number into an equivalent hardware register value.
 * @param n Breakpoint number.
 * @param type One value from seL4_BreakpointType.
 * @param size An integer for the operand size of the breakpoint.
 * @return Converted, hardware-specific value.
 */
خالص ساكن مضمن کلمه_t convert_اندازه_مقصد_Arch(uint16_t bp_رقم, کلمه_t type, کلمه_t اندازه)
{
    إذا (type == seL4_InstructionBreakpoint) {
        /* Intel manual vol3 17.2.4:
         * "If the corresponding RWn field in register DR7 is 00 (instruction
         * execution), then the LENn field should also be 00"
         */
        اندازه = 0;
    } وإلا {
        بدل (اندازه) {
        حالة 1:
            اندازه = X86_DEBUG_BP_اندازه_1_B;
            اكسر;
        حالة 2:
            اندازه = X86_DEBUG_BP_اندازه_2_B;
            اكسر;
        حالة 8:
            اندازه = X86_DEBUG_BP_اندازه_8_B;
            اكسر;
        افتراضي: /* 4B */
            أكد(اندازه == 4);
            اندازه = X86_DEBUG_BP_اندازه_4_B;
        }
    }

    بدل (bp_رقم) {
    حالة 0:
        أرجع اندازه << X86_DEBUG_BP0_اندازه_لغزش;
    حالة 1:
        أرجع اندازه << X86_DEBUG_BP1_اندازه_لغزش;
    حالة 2:
        أرجع اندازه << X86_DEBUG_BP2_اندازه_لغزش;
    افتراضي: /* 3 */
        أكد(bp_رقم == 3);
        أرجع اندازه << X86_DEBUG_BP3_اندازه_لغزش;
    }
}

/** Reverse of convertSizeToArch(): converts a hardware-specific size value
 * into an integer representation.
 * @param dr7 Hardware register value as input.
 * @param n Breakpoint number.
 * @return Converted size value.
 */
خالص ساكن مضمن کلمه_t convert_Arch_مقصد_اندازه(کلمه_t dr7, uint16_t bp_رقم)
{
    کلمه_t type;

    بدل (bp_رقم) {
    حالة 0:
        type = dr7 & (0x3u << X86_DEBUG_BP0_TYPE_لغزش);
        type >>= X86_DEBUG_BP0_TYPE_لغزش;
        dr7 &= 0x3u << X86_DEBUG_BP0_اندازه_لغزش;
        dr7 >>= X86_DEBUG_BP0_اندازه_لغزش;
        اكسر;
    حالة 1:
        type = dr7 & (0x3u << X86_DEBUG_BP1_TYPE_لغزش);
        type >>= X86_DEBUG_BP1_TYPE_لغزش;
        dr7 &= 0x3u << X86_DEBUG_BP1_اندازه_لغزش;
        dr7 >>= X86_DEBUG_BP1_اندازه_لغزش;
        اكسر;
    حالة 2:
        type = dr7 & (0x3u << X86_DEBUG_BP2_TYPE_لغزش);
        type >>= X86_DEBUG_BP2_TYPE_لغزش;
        dr7 &= 0x3u << X86_DEBUG_BP2_اندازه_لغزش;
        dr7 >>= X86_DEBUG_BP2_اندازه_لغزش;
        اكسر;
    افتراضي: /* 3 */
        أكد(bp_رقم == 3);
        type = dr7 & (0x3u << X86_DEBUG_BP3_TYPE_لغزش);
        type >>= X86_DEBUG_BP3_TYPE_لغزش;
        dr7 &= 0x3u << X86_DEBUG_BP3_اندازه_لغزش;
        dr7 >>= X86_DEBUG_BP3_اندازه_لغزش;
    }

    /* Force size to 0 if type is instruction breakpoint. */
    إذا (type == X86_DEBUG_BP_TYPE_INSTR) {
        أرجع 0;
    }

    بدل (dr7) {
    حالة X86_DEBUG_BP_اندازه_1_B:
        أرجع 1;
    حالة X86_DEBUG_BP_اندازه_2_B:
        أرجع 2;
    حالة X86_DEBUG_BP_اندازه_8_B:
        أرجع 8;
    افتراضي: /* 4B */
        أكد(dr7 == X86_DEBUG_BP_اندازه_4_B);
        أرجع 4;
    }
}

/** Enables a breakpoint.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساكن فراغ enableBreakpoint(tcb_t *t, uint16_t bp_رقم)
{
    کلمه_t enable_بیت;

    أكد(t != عدم);
    أكد(bp_رقم < X86_DEBUG_BP_N_REGS);

    بدل (bp_رقم) {
    حالة 0:
        enable_بیت = X86_DEBUG_BP0_ENABLE_بیت;
        اكسر;
    حالة 1:
        enable_بیت = X86_DEBUG_BP1_ENABLE_بیت;
        اكسر;
    حالة 2:
        enable_بیت = X86_DEBUG_BP2_ENABLE_بیت;
        اكسر;
    افتراضي:
        enable_بیت = X86_DEBUG_BP3_ENABLE_بیت;
        اكسر;
    }

    bitwiseOrDr7Context(t, enable_بیت);
}

/** Disables a breakpoint without clearing its configuration.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساكن فراغ disableBreakpoint(tcb_t *t, uint16_t bp_رقم)
{
    کلمه_t disable_ماسک;

    أكد(t != عدم);
    أكد(bp_رقم < X86_DEBUG_BP_N_REGS);

    بدل (bp_رقم) {
    حالة 0:
        disable_ماسک = ~X86_DEBUG_BP0_ENABLE_بیت;
        اكسر;
    حالة 1:
        disable_ماسک = ~X86_DEBUG_BP1_ENABLE_بیت;
        اكسر;
    حالة 2:
        disable_ماسک = ~X86_DEBUG_BP2_ENABLE_بیت;
        اكسر;
    افتراضي:
        disable_ماسک = ~X86_DEBUG_BP3_ENABLE_بیت;
        اكسر;
    }

    bitwiseAndDr7Context(t, disable_ماسک);
}

/** Returns a boolean for whether or not a breakpoint is enabled.
 * @param bp_num Hardware breakpoint ID. Usually an integer from 0..N.
 */
ساكن bool_t breakpointIsEnabled(tcb_t *t, uint16_t bp_رقم)
{
    کلمه_t dr7;

    أكد(t != عدم);
    أكد(bp_رقم < X86_DEBUG_BP_N_REGS);

    dr7 = readDr7Context(t);
    بدل (bp_رقم) {
    حالة 0:
        أرجع !!(dr7 & X86_DEBUG_BP0_ENABLE_بیت);
    حالة 1:
        أرجع !!(dr7 & X86_DEBUG_BP1_ENABLE_بیت);
    حالة 2:
        أرجع !!(dr7 & X86_DEBUG_BP2_ENABLE_بیت);
    افتراضي:
        أرجع !!(dr7 & X86_DEBUG_BP3_ENABLE_بیت);
    }
}

ساكن فراغ مجموعه_Bp_Vaddr_Context(tcb_t *t, uint16_t bp_رقم, کلمه_t vaddr)
{
    أكد(t != عدم);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    بدل (bp_رقم) {
    حالة 0:
        ubs->dr[0] = vaddr;
        اكسر;
    حالة 1:
        ubs->dr[1] = vaddr;
        اكسر;
    حالة 2:
        ubs->dr[2] = vaddr;
        اكسر;
    افتراضي:
        أكد(bp_رقم == 3);
        ubs->dr[3] = vaddr;
        اكسر;
    }
    أرجع;
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
فراغ مجموعه_Breakpoint(tcb_t *t,
                   uint16_t bp_رقم, کلمه_t vaddr, کلمه_t types, کلمه_t اندازه, کلمه_t rw)
{
    کلمه_t dr7val;

    أكد(t != عدم);

    dr7val = convert_Type_And_Access_مقصد_Arch(bp_رقم, types, rw);
    dr7val |= convert_اندازه_مقصد_Arch(bp_رقم, types, اندازه);

    مجموعه_Bp_Vaddr_Context(t, bp_رقم, vaddr);
    unset_Dr7_بیتها_For(t, bp_رقم);
    bitwiseOrDr7Context(t, dr7val);
    enableBreakpoint(t, bp_رقم);
}

ساكن کلمه_t getBpVaddrContext(tcb_t *t, uint16_t bp_رقم)
{
    أكد(t != عدم);
    user_breakpoint_state_t *ubs = &t->tcbArch.tcbContext.breakpointState;

    بدل (bp_رقم) {
    حالة 0:
        أرجع ubs->dr[0];
    حالة 1:
        أرجع ubs->dr[1];
    حالة 2:
        أرجع ubs->dr[2];
    افتراضي:
        أكد(bp_رقم == 3);
        أرجع ubs->dr[3];
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_رقم)
{
    کلمه_t dr7val;
    getBreakpoint_t ret;
    convertedTypeAndAccess_t res;

    dr7val = readDr7Context(t);
    ret.vaddr = getBpVaddrContext(t, bp_رقم);
    ret.اندازه = convert_Arch_مقصد_اندازه(dr7val, bp_رقم);
    res = convert_Arch_مقصد_Type_And_Access(dr7val, bp_رقم);
    ret.type = res.type;
    ret.rw = res.rw;
    ret.is_enabled = breakpointIsEnabled(t, bp_رقم);
    أرجع ret;
}

/** Backend for the x86 seL4_TCB_UnsetBreakpoint invocation.
 *
 * Unsets and *clears* a hardware breakpoint.
 * @param uds Arch TCB register context pointer.
 * @param bp_num The hardware breakpoint ID you'd like to clear.
 */
فراغ unsetBreakpoint(tcb_t *t, uint16_t bp_رقم)
{
    disableBreakpoint(t, bp_رقم);
    unset_Dr7_بیتها_For(t, bp_رقم);
    مجموعه_Bp_Vaddr_Context(t, bp_رقم, 0);
}

/** Used in the exception path to determine if an exception was caused by
 * single-stepping being active.
 *
 * @param uc Arch TCB register context structure.
 * @return a structure stating whether or not the exception was caused by
 *          hardware single-stepping, and what the instruction vaddr was.
 */
تعريفنوع بنية {
    bool_t ret;
    کلمه_t instr_vaddr;
} test_And_Reset_Single_گام_Exception_t;

ساكن test_And_Reset_Single_گام_Exception_t test_And_Reset_Single_گام_Exception(tcb_t *t)
{
    test_And_Reset_Single_گام_Exception_t ret;
    کلمه_t dr6;

    dr6 = readDr6Reg();
    إذا (!(dr6 & X86_DEBUG_DR6_SINGLE_گام_FLAG)) {
        ret.ret = false;
        أرجع ret;
    }

    ret.ret = true;
    ret.instr_vaddr = t->tcbArch.tcbContext.registers[FaultIP];
    bitwiseAndDr6Reg(~X86_DEBUG_DR6_SINGLE_گام_FLAG);

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
    أرجع ret;
}

bool_t configureSingleStepping(tcb_t *t, uint16_t bp_رقم, کلمه_t n_instr,
                               استفادهنشده bool_t is_پاسخ)
{
    /* On x86 no hardware breakpoints are needed for single stepping. */
    إذا (n_instr == 0) {
        /* If n_instr (number of instructions to single-step) is 0, that is the
          * same as requesting that single-stepping be disabled.
          */
        t->tcbArch.tcbContext.breakpointState.single_گام_enabled = false;
        t->tcbArch.tcbContext.registers[FLAGS] &= ~X86_DEBUG_EFLAGS_TRAP_FLAG;
    } وإلا {
        t->tcbArch.tcbContext.breakpointState.single_گام_enabled = true;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    أرجع false;
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
تعريفنوع بنية {
    صحيح bp_رقم;
    کلمه_t vaddr, reason;
} get_And_Reset_فعال_Breakpoint_t;

ساكن get_And_Reset_فعال_Breakpoint_t get_And_Reset_فعال_Breakpoint(tcb_t *t)
{
    convertedTypeAndAccess_t tmp;
    get_And_Reset_فعال_Breakpoint_t ret;

    /* Read from the hardware regs, not user context */
    کلمه_t dr6 = readDr6Reg();
    إذا (dr6 & بیت(0)) {
        ret.bp_رقم = 0;
    } وإلا إذا (dr6 & بیت(1)) {
        ret.bp_رقم = 1;
    } وإلا إذا (dr6 & بیت(2)) {
        ret.bp_رقم = 2;
    } وإلا إذا (dr6 & بیت(3)) {
        ret.bp_رقم = 3;
    } وإلا {
        ret.bp_رقم = -1;
        أرجع ret;
    }

    tmp = convert_Arch_مقصد_Type_And_Access(readDr7Context(t), ret.bp_رقم);
    ret.vaddr = getBpVaddrContext(t, ret.bp_رقم);
    ret.reason = tmp.type;

    bitwiseAndDr6Reg(~بیت(ret.bp_رقم));
    أرجع ret;
}

exception_t handleUserLevelDebugException(صحيح int_وکتور)
{
    tcb_t *ct;
    get_And_Reset_فعال_Breakpoint_t فعال_bp;
    test_And_Reset_Single_گام_Exception_t single_گام_info;

#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_User_Level_Fault;
    ks_Kernel_مدخل.کلمه = int_وکتور;
#پایان_اگر /* DEBUG */

#اگر_تعریف_شده CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    benchmark_track_شروع();
#پایان_اگر

    ct = گره_STATE(ksCurThread);

    /* Software break request (INT3) is detected by the vector number */
    إذا (int_وکتور == int_software_break_request) {
        current_fault = seL4_Fault_DebugException_new(getRestartPC(گره_STATE(ksCurThread)),
                                                      0, seL4_SoftwareBreakRequest);
    } وإلا {
        /* Hardware breakpoint trigger is detected using DR6 */
        فعال_bp = get_And_Reset_فعال_Breakpoint(ct);
        إذا (فعال_bp.bp_رقم >= 0) {
            current_fault = seL4_Fault_DebugException_new(فعال_bp.vaddr,
                                                          فعال_bp.bp_رقم,
                                                          فعال_bp.reason);
        } وإلا {
            single_گام_info = test_And_Reset_Single_گام_Exception(ct);
            إذا (single_گام_info.ret == true) {
                /* If the caller asked us to skip over N instructions before
                 * generating the next single-step breakpoint, we shouldn't
                 * bother to construct a fault message until we've skipped N
                 * instructions.
                 */
                إذا (single_گام_Fault_Counter_Ready(ct) == false) {
                    أرجع EXCEPTION_NONE;
                }
                current_fault = seL4_Fault_DebugException_new(single_گام_info.instr_vaddr,
                                                              0, se_L4_Single_گام);
            } وإلا {
                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }
    }

    handleFault(گره_STATE(ksCurThread));

    زمانبندی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

BOOT_کد bool_t Arch_initHardwareBreakpoints(فراغ)
{
    x86_cpu_عنصر_خنثی_t *modelinfo;

    modelinfo = x86_cpuid_get_model_info();
    /* Intel manuals, vol3, section 17.2.4, "NOTES". */
    إذا (modelinfo->family == 15) {
        إذا (modelinfo->model == 3 || modelinfo->model == 4
            || modelinfo->model == 6) {
            byte8_bps_supported = true;
        }
    }
    إذا (modelinfo->family == 6) {
        إذا (modelinfo->model == 15 || modelinfo->model == 23
            || modelinfo->model == 0x1C) {
            byte8_bps_supported = true;
        }
    }
    أرجع true;
}

فراغ Arch_initBreakpointContext(user_breakpoint_state_t *uds)
{
    memset(uds, 0, حجم(*uds));

    /* Preload reserved values into register context */
    uds->dr[4] = readDr6Reg() &
                 ~(بیت(0)
                   | بیت(1)
                   | بیت(2)
                   | بیت(3)
                   | X86_DEBUG_DR6_SINGLE_گام_FLAG);

    uds->dr[5] = readDr7Reg() &
                 ~(X86_DEBUG_BP0_ENABLE_بیت | X86_DEBUG_BP1_ENABLE_بیت
                   | X86_DEBUG_BP2_ENABLE_بیت
                   | X86_DEBUG_BP3_ENABLE_بیت);
}

#پایان_اگر
