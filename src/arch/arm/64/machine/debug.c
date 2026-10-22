/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API

#شامل <machine/debug.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/machine/debug.h>
#شامل <arch/machine/debug_conf.h>
#شامل <arch/machine/registerset.h>
#شامل <arch/model/statedata.h>
#شامل <armv/debug.h>
#شامل <mode/machine/debug.h>
#شامل <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#شامل <string.h>
#شامل <util.h>

#تعریف_کرو MDSCR_MDE (ثنائی_ہندسہ(15))
#تعریف_کرو MDSCR_SS  (ثنائی_ہندسہ(0))
#تعریف_کرو SPSR_SS   (ثنائی_ہندسہ(21))

#تعریف_کرو ESR_EXCEPTION_CLASS_نقاب 0xFC000000
#تعریف_کرو ESR_EXCEPTION_CLASS_OFF 26

#تعریف_کرو DEBUG_اندراج_BREAKPOINT      0x30
#تعریف_کرو DEBUG_اندراج_SINGLE_قدم     0x32
#تعریف_کرو DEBUG_اندراج_WATCHPOINT      0x34
#تعریف_کرو DEBUG_اندراج_EXPLICIT_BKPT   0x3C

#تعریف_کرو OSDLR_تالا (ثنائی_ہندسہ(0))
#تعریف_کرو OSLAR_تالا (ثنائی_ہندسہ(0))

bool_t byte8WatchpointsSupported(خالی)
{
    واپس true;
}

exception_t handleDebugFaultEvent(لفظ_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(گرہ_STATE(ksCurThread)));
        اگر (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(گرہ_STATE(ksCurThread));
        }
    })
    جدول_وقت();
    activateThread();

    واپس EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_عدد, لفظ_t n_instr,
                               bool_t is_جواب)
{
    اگر (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = true;
    } ورنہ {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    واپس true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_رمز bool_t Arch_initHardwareBreakpoints(خالی)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    لفظ_t mdscr = 0;
    MRS("MDSCR_EL1", mdscr);
    mdscr |= MDSCR_MDE;
    MSR("MDSCR_EL1", mdscr);

    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.4: A debug exception can be taken only if all the following are true:
     *    - The OS Lock is unlocked
     *    - DoubleLockStatus() = False
     *    - The debug exception is enabled from the current exception level
     *    - The debug exception is enabled from the current security state
     */

    /* Ensure that the OS double lock is unset */
    لفظ_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_تالا;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    لفظ_t oslar = 0;
    MSR("oslar_el1", oslar);

    /* Ensure that all the breakpoint and watchpoint registers are initially disabled */
    disableAllBpsAndWps();

    /* Ensure that single stepping is initially disabled */
    MRS("MDSCR_EL1", mdscr);
    mdscr &= ~MDSCR_SS;
    MSR("MDSCR_EL1", mdscr);

    /* Finally, also pre-load some initial register state that can be used
     * for all new threads so that their initial saved debug register state
     * is valid when it's first loaded onto the CPU.
     */
    برائے (صحیح i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    برائے (صحیح i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    واپس true;
}

/* Abstract wrapper around the ESR fault status value */

ساکن لفظ_t get_Fault_حالت(لفظ_t esr)
{
    واپس (esr & ESR_EXCEPTION_CLASS_نقاب) >> ESR_EXCEPTION_CLASS_OFF;
}

/** Called to determine if an abort was a debug exception.
 *
 * The ARM debug exceptions look like Prefetch Aborts or Data Aborts, and you
 * have to examine some extra register state to determine whether or not the
 * abort you currently have on your hands is actually a debug exception.
 *
 * This routine takes care of the checks.
 * @param fs An abstraction of the DFSR/IFSR values, meant to make it irrelevant
 *           whether we're using the long/short descriptors. Bit positions and
 *           values change. This also makes the debug code forward compatible
 *           aarch64.
 */
bool_t isDebugFault(لفظ_t esr)
{
    لفظ_t exception_class = get_Fault_حالت(esr);
    واپس (exception_class == DEBUG_اندراج_BREAKPOINT ||
            exception_class == DEBUG_اندراج_SINGLE_قدم ||
            exception_class == DEBUG_اندراج_WATCHPOINT ||
            exception_class == DEBUG_اندراج_EXPLICIT_BKPT);
}

/** Called to process a debug exception.
 *
 * On x86, you're told which breakpoint register triggered the exception. On
 * ARM, you're told the virtual address that triggered the exception and what
 * type of access (data access vs instruction execution) triggered the exception
 * and you have to figure out which register triggered it.
 *
 * For watchpoints, it's not very complicated: just check to see which
 * register matches the virtual address.
 *
 * For breakpoints, it's a bit more complex: since both breakpoints and single-
 * stepping are configured using the same registers, we need to first detect
 * whether single-stepping is enabled. If not, then we check for a breakpoint.
 * @param fault_vaddr The instruction vaddr which triggered the exception, as
 *                    extracted by the kernel.
 */
seL4_Fault_t handleUserLevelDebugException(لفظ_t esr, لفظ_t fault_vaddr)
{
    صحیح فعال_bp;
    لفظ_t bp_reason, bp_vaddr;
    لفظ_t exception_class = get_Fault_حالت(esr);

#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_Debug_Fault;
    ks_Kernel_اندراج.لفظ = exception_class;
#اگر_ختم

    انتخاب (exception_class) {
    حالت DEBUG_اندراج_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        توڑو;
    حالت DEBUG_اندراج_WATCHPOINT:
        bp_reason = se_L4_ڈیٹا_Breakpoint;
        bp_vaddr = getFAR();
        توڑو;
    حالت DEBUG_اندراج_SINGLE_قدم:
        bp_reason = se_L4_Single_قدم;
        bp_vaddr = fault_vaddr;
        فعال_bp = 0;
        توڑو;
    طےشدہ: /* EXPLICIT_BKPT: BKPT instruction */
        أكد(exception_class == DEBUG_اندراج_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        فعال_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    اگر (bp_reason != seL4_SoftwareBreakRequest && bp_reason != se_L4_Single_قدم) {
        فعال_bp = get_And_Reset_فعال_Breakpoint(bp_vaddr, bp_reason);
        فعال_bp = get_Bp_عدد_From_Type(فعال_bp, bp_reason);
        أكد(فعال_bp >= 0);
    }

    اگر (bp_reason == se_L4_Single_قدم && !single_قدم_Fault_شمار_Ready(گرہ_STATE(ksCurThread))) {
        واپس seL4_Fault_NullFault_new();
    }

    واپس seL4_Fault_DebugException_new(bp_vaddr, فعال_bp, bp_reason);
}

#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */

#اگر_تعریف_شدہ ARM_بنیاد_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
خالی aarch64_restore_user_debug_context(tcb_t *ہدف_thread)
{
    أكد(ہدف_thread != تہی);

    /* Set/unset single stepping if applicable */
    لفظ_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = get_رجسٹر(ہدف_thread, SPSR_EL1);
    اگر (ہدف_thread->tcbArch.tcbContext.breakpointState.single_قدم_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } ورنہ {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    سیٹ_رجسٹر(ہدف_thread, SPSR_EL1, spsr);
}

#اگر_ختم /* ARM_BASE_CP14_SAVE_AND_RESTORE */
