/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug_conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MDSCR_MDE (ਦੋਆਧਾਰੀ_ਅੰਕ(15))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MDSCR_SS  (ਦੋਆਧਾਰੀ_ਅੰਕ(0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SPSR_SS   (ਦੋਆਧਾਰੀ_ਅੰਕ(21))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESR_EXCEPTION_CLASS_ਨਕਾਬ 0xFC000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ESR_EXCEPTION_CLASS_OFF 26

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਇੰਦਰਾਜ_BREAKPOINT      0x30
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਇੰਦਰਾਜ_SINGLE_ਕਦਮ     0x32
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਇੰਦਰਾਜ_WATCHPOINT      0x34
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_ਇੰਦਰਾਜ_EXPLICIT_BKPT   0x3C

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSDLR_ਤਾਲਾ (ਦੋਆਧਾਰੀ_ਅੰਕ(0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OSLAR_ਤਾਲਾ (ਦੋਆਧਾਰੀ_ਅੰਕ(0))

bool_t byte8WatchpointsSupported(ਖਾਲੀ)
{
    ਵਾਪਸ true;
}

exception_t handleDebugFaultEvent(ਸ਼ਬਦ_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(ਗੰਢ_STATE(ksCurThread)));
        ਜੇ (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(ਗੰਢ_STATE(ksCurThread));
        }
    })
    ਸਮਾਂ_ਸਾਰਣੀ();
    activateThread();

    ਵਾਪਸ EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ਅੰਕ, ਸ਼ਬਦ_t n_instr,
                               bool_t is_ਜਵਾਬ)
{
    ਜੇ (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = true;
    } ਨਹੀਂਤਾਂ {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ਵਾਪਸ true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_ਕੋਡ bool_t Arch_initHardwareBreakpoints(ਖਾਲੀ)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    ਸ਼ਬਦ_t mdscr = 0;
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
    ਸ਼ਬਦ_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_ਤਾਲਾ;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    ਸ਼ਬਦ_t oslar = 0;
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
    ਲਈ (ਪੂਰਨ i = 0; i < se_L4_ਅੰਕ_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    ਲਈ (ਪੂਰਨ i = 0; i < se_L4_ਅੰਕ_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    ਵਾਪਸ true;
}

/* Abstract wrapper around the ESR fault status value */

ਸਥਿਰ ਸ਼ਬਦ_t get_Fault_ਸਥਿਤੀ(ਸ਼ਬਦ_t esr)
{
    ਵਾਪਸ (esr & ESR_EXCEPTION_CLASS_ਨਕਾਬ) >> ESR_EXCEPTION_CLASS_OFF;
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
bool_t isDebugFault(ਸ਼ਬਦ_t esr)
{
    ਸ਼ਬਦ_t exception_class = get_Fault_ਸਥਿਤੀ(esr);
    ਵਾਪਸ (exception_class == DEBUG_ਇੰਦਰਾਜ_BREAKPOINT ||
            exception_class == DEBUG_ਇੰਦਰਾਜ_SINGLE_ਕਦਮ ||
            exception_class == DEBUG_ਇੰਦਰਾਜ_WATCHPOINT ||
            exception_class == DEBUG_ਇੰਦਰਾਜ_EXPLICIT_BKPT);
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
seL4_Fault_t handleUserLevelDebugException(ਸ਼ਬਦ_t esr, ਸ਼ਬਦ_t fault_vaddr)
{
    ਪੂਰਨ ਸਰਗਰਮ_bp;
    ਸ਼ਬਦ_t bp_reason, bp_vaddr;
    ਸ਼ਬਦ_t exception_class = get_Fault_ਸਥਿਤੀ(esr);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Debug_Fault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = exception_class;
#ਜੇ_ਅੰਤ

    ਚੋਣ (exception_class) {
    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        ਰੋਕੋ;
    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_WATCHPOINT:
        bp_reason = se_L4_ਅੰਕੜੇ_Breakpoint;
        bp_vaddr = getFAR();
        ਰੋਕੋ;
    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_SINGLE_ਕਦਮ:
        bp_reason = se_L4_Single_ਕਦਮ;
        bp_vaddr = fault_vaddr;
        ਸਰਗਰਮ_bp = 0;
        ਰੋਕੋ;
    ਮੂਲ: /* EXPLICIT_BKPT: BKPT instruction */
        ਪ੍ਰਤਿਜ੍ਞਾ(exception_class == DEBUG_ਇੰਦਰਾਜ_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        ਸਰਗਰਮ_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    ਜੇ (bp_reason != seL4_SoftwareBreakRequest && bp_reason != se_L4_Single_ਕਦਮ) {
        ਸਰਗਰਮ_bp = get_And_Reset_ਸਰਗਰਮ_Breakpoint(bp_vaddr, bp_reason);
        ਸਰਗਰਮ_bp = get_Bp_ਅੰਕ_From_Type(ਸਰਗਰਮ_bp, bp_reason);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਰਗਰਮ_bp >= 0);
    }

    ਜੇ (bp_reason == se_L4_Single_ਕਦਮ && !single_ਕਦਮ_Fault_Counter_Ready(ਗੰਢ_STATE(ksCurThread))) {
        ਵਾਪਸ seL4_Fault_NullFault_new();
    }

    ਵਾਪਸ seL4_Fault_DebugException_new(bp_vaddr, ਸਰਗਰਮ_bp, bp_reason);
}

#ਜੇ_ਅੰਤ /* CONFIG_HARDWARE_DEBUG_API */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_ਅਧਾਰ_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
ਖਾਲੀ aarch64_restore_user_debug_context(tcb_t *ਨਿਸ਼ਾਨਾ_thread)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(ਨਿਸ਼ਾਨਾ_thread != ਨਿਰਾ);

    /* Set/unset single stepping if applicable */
    ਸ਼ਬਦ_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = get_ਰਜਿਸਟਰ(ਨਿਸ਼ਾਨਾ_thread, SPSR_EL1);
    ਜੇ (ਨਿਸ਼ਾਨਾ_thread->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } ਨਹੀਂਤਾਂ {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    ਸੈੱਟ_ਰਜਿਸਟਰ(ਨਿਸ਼ਾਨਾ_thread, SPSR_EL1, spsr);
}

#ਜੇ_ਅੰਤ /* ARM_BASE_CP14_SAVE_AND_RESTORE */
