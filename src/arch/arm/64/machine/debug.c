/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API

#ಅನ್ತರ್ಭಾವ <machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <armv/debug.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <util.h>

#ವ್ಯಾಖ್ಯಾನಿಸು MDSCR_MDE (ಬಿಟ್(15))
#ವ್ಯಾಖ್ಯಾನಿಸು MDSCR_SS  (ಬಿಟ್(0))
#ವ್ಯಾಖ್ಯಾನಿಸು SPSR_SS   (ಬಿಟ್(21))

#ವ್ಯಾಖ್ಯಾನಿಸು ESR_EXCEPTION_CLASS_ಮುಖವಾಡ 0xFC000000
#ವ್ಯಾಖ್ಯಾನಿಸು ESR_EXCEPTION_CLASS_OFF 26

#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ನಮೂದು_BREAKPOINT      0x30
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ನಮೂದು_SINGLE_ಹೆಜ್ಜೆ     0x32
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ನಮೂದು_WATCHPOINT      0x34
#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_ನಮೂದು_EXPLICIT_BKPT   0x3C

#ವ್ಯಾಖ್ಯಾನಿಸು OSDLR_ಬೀಗ (ಬಿಟ್(0))
#ವ್ಯಾಖ್ಯಾನಿಸು OSLAR_ಬೀಗ (ಬಿಟ್(0))

bool_t byte8WatchpointsSupported(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ true;
}

exception_t handleDebugFaultEvent(ಶಬ್ದ_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(ಗಂಟು_STATE(ksCurThread)));
        ಆದರೆ (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(ಗಂಟು_STATE(ksCurThread));
        }
    })
    ವೇಳಾಪಟ್ಟಿ();
    activateThread();

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t n_instr,
                               bool_t is_ಉತ್ತರ)
{
    ಆದರೆ (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = true;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ಹಿಂತಿರುಗಿ true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_ಸಂಕೇತ bool_t Arch_initHardwareBreakpoints(ಶೂನ್ಯ)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    ಶಬ್ದ_t mdscr = 0;
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
    ಶಬ್ದ_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_ಬೀಗ;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    ಶಬ್ದ_t oslar = 0;
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
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    ಹಿಂತಿರುಗಿ true;
}

/* Abstract wrapper around the ESR fault status value */

ಸ್ಥಿರ ಶಬ್ದ_t get_Fault_ಸ್ಥಿತಿ(ಶಬ್ದ_t esr)
{
    ಹಿಂತಿರುಗಿ (esr & ESR_EXCEPTION_CLASS_ಮುಖವಾಡ) >> ESR_EXCEPTION_CLASS_OFF;
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
bool_t isDebugFault(ಶಬ್ದ_t esr)
{
    ಶಬ್ದ_t exception_class = get_Fault_ಸ್ಥಿತಿ(esr);
    ಹಿಂತಿರುಗಿ (exception_class == DEBUG_ನಮೂದು_BREAKPOINT ||
            exception_class == DEBUG_ನಮೂದು_SINGLE_ಹೆಜ್ಜೆ ||
            exception_class == DEBUG_ನಮೂದು_WATCHPOINT ||
            exception_class == DEBUG_ನಮೂದು_EXPLICIT_BKPT);
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
seL4_Fault_t handleUserLevelDebugException(ಶಬ್ದ_t esr, ಶಬ್ದ_t fault_vaddr)
{
    ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ_bp;
    ಶಬ್ದ_t bp_reason, bp_vaddr;
    ಶಬ್ದ_t exception_class = get_Fault_ಸ್ಥಿತಿ(esr);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Debug_Fault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = exception_class;
#ಆದರೆ_ಅಂತ್ಯ

    ಆಯ್ಕೆ (exception_class) {
    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_WATCHPOINT:
        bp_reason = se_L4_ದತ್ತಾಂಶ_Breakpoint;
        bp_vaddr = getFAR();
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_SINGLE_ಹೆಜ್ಜೆ:
        bp_reason = se_L4_Single_ಹೆಜ್ಜೆ;
        bp_vaddr = fault_vaddr;
        ಸಕ್ರಿಯ_bp = 0;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ: /* EXPLICIT_BKPT: BKPT instruction */
        ಪ್ರತಿಜ್ಞಾ(exception_class == DEBUG_ನಮೂದು_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        ಸಕ್ರಿಯ_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    ಆದರೆ (bp_reason != seL4_SoftwareBreakRequest && bp_reason != se_L4_Single_ಹೆಜ್ಜೆ) {
        ಸಕ್ರಿಯ_bp = get_And_Reset_ಸಕ್ರಿಯ_Breakpoint(bp_vaddr, bp_reason);
        ಸಕ್ರಿಯ_bp = get_Bp_ಅಂಕೆ_From_Type(ಸಕ್ರಿಯ_bp, bp_reason);
        ಪ್ರತಿಜ್ಞಾ(ಸಕ್ರಿಯ_bp >= 0);
    }

    ಆದರೆ (bp_reason == se_L4_Single_ಹೆಜ್ಜೆ && !single_ಹೆಜ್ಜೆ_Fault_Counter_Ready(ಗಂಟು_STATE(ksCurThread))) {
        ಹಿಂತಿರುಗಿ seL4_Fault_NullFault_new();
    }

    ಹಿಂತಿರುಗಿ seL4_Fault_DebugException_new(bp_vaddr, ಸಕ್ರಿಯ_bp, bp_reason);
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_ತಳ_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
ಶೂನ್ಯ aarch64_restore_user_debug_context(tcb_t *ಗುರಿ_thread)
{
    ಪ್ರತಿಜ್ಞಾ(ಗುರಿ_thread != ಖಾಲಿ);

    /* Set/unset single stepping if applicable */
    ಶಬ್ದ_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = get_ರಿಜಿಸ್ಟರ್(ಗುರಿ_thread, SPSR_EL1);
    ಆದರೆ (ಗುರಿ_thread->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    ಗಣ_ರಿಜಿಸ್ಟರ್(ಗುರಿ_thread, SPSR_EL1, spsr);
}

#ಆದರೆ_ಅಂತ್ಯ /* ARM_BASE_CP14_SAVE_AND_RESTORE */
