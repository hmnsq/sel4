/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ifdef CONFIG_HARDWARE_DEBUG_API

#ଅନ୍ତର୍ଭାଵ <machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/debug_conf.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/registerset.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <armv/debug.h>
#ଅନ୍ତର୍ଭାଵ <mode/machine/debug.h>
#ଅନ୍ତର୍ଭାଵ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

#define MDSCR_MDE (BIT(15))
#define MDSCR_SS  (BIT(0))
#define SPSR_SS   (BIT(21))

#define ESR_EXCEPTION_CLASS_MASK 0xFC000000
#define ESR_EXCEPTION_CLASS_OFF 26

#define DEBUG_ENTRY_BREAKPOINT      0x30
#define DEBUG_ENTRY_SINGLE_STEP     0x32
#define DEBUG_ENTRY_WATCHPOINT      0x34
#define DEBUG_ENTRY_EXPLICIT_BKPT   0x3C

#define OSDLR_LOCK (BIT(0))
#define OSLAR_LOCK (BIT(0))

bool_t byte8WatchpointsSupported(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ true;
}

exception_t handleDebugFaultEvent(word_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(ଗଣ୍ଠି_STATE(ksCurThread)));
        ଯଦି (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(ଗଣ୍ଠି_STATE(ksCurThread));
        }
    })
    schedule();
    activateThread();

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_ଅଙ୍କ, word_t n_instr,
                               bool_t is_reply)
{
    ଯଦି (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = true;
    } ଅନ୍ୟଥା {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_step_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    ଫେରନ୍ତୁ true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_CODE bool_t Arch_initHardwareBreakpoints(ଶୂନ୍ୟ)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    word_t mdscr = 0;
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
    word_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_LOCK;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    word_t oslar = 0;
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
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < se_L4_ଅଙ୍କ_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < se_L4_ଅଙ୍କ_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    ଫେରନ୍ତୁ true;
}

/* Abstract wrapper around the ESR fault status value */

ସ୍ଥିର word_t getFaultStatus(word_t esr)
{
    ଫେରନ୍ତୁ (esr & ESR_EXCEPTION_CLASS_MASK) >> ESR_EXCEPTION_CLASS_OFF;
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
bool_t isDebugFault(word_t esr)
{
    word_t exception_class = getFaultStatus(esr);
    ଫେରନ୍ତୁ (exception_class == DEBUG_ENTRY_BREAKPOINT ||
            exception_class == DEBUG_ENTRY_SINGLE_STEP ||
            exception_class == DEBUG_ENTRY_WATCHPOINT ||
            exception_class == DEBUG_ENTRY_EXPLICIT_BKPT);
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
seL4_Fault_t handleUserLevelDebugException(word_t esr, word_t fault_vaddr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା active_bp;
    word_t bp_reason, bp_vaddr;
    word_t exception_class = getFaultStatus(esr);

#ifdef TRACK_KERNEL_ENTRIES
    ksKernelEntry.path = Entry_DebugFault;
    ksKernelEntry.word = exception_class;
#endif

    ବାଛ (exception_class) {
    ସ୍ଥିତି DEBUG_ENTRY_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        ଭାଙ୍ଗ;
    ସ୍ଥିତି DEBUG_ENTRY_WATCHPOINT:
        bp_reason = seL4_DataBreakpoint;
        bp_vaddr = getFAR();
        ଭାଙ୍ଗ;
    ସ୍ଥିତି DEBUG_ENTRY_SINGLE_STEP:
        bp_reason = seL4_SingleStep;
        bp_vaddr = fault_vaddr;
        active_bp = 0;
        ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* EXPLICIT_BKPT: BKPT instruction */
        ପ୍ରତିଜ୍ଞା(exception_class == DEBUG_ENTRY_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        active_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    ଯଦି (bp_reason != seL4_SoftwareBreakRequest && bp_reason != seL4_SingleStep) {
        active_bp = getAndResetActiveBreakpoint(bp_vaddr, bp_reason);
        active_bp = get_Bp_ଅଙ୍କ_From_Type(active_bp, bp_reason);
        ପ୍ରତିଜ୍ଞା(active_bp >= 0);
    }

    ଯଦି (bp_reason == seL4_SingleStep && !singleStepFaultCounterReady(ଗଣ୍ଠି_STATE(ksCurThread))) {
        ଫେରନ୍ତୁ seL4_Fault_NullFault_new();
    }

    ଫେରନ୍ତୁ seL4_Fault_DebugException_new(bp_vaddr, active_bp, bp_reason);
}

#endif /* CONFIG_HARDWARE_DEBUG_API */

#ifdef ARM_BASE_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
ଶୂନ୍ୟ aarch64_restore_user_debug_context(tcb_t *target_thread)
{
    ପ୍ରତିଜ୍ଞା(target_thread != ଖାଲି);

    /* Set/unset single stepping if applicable */
    word_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = getRegister(target_thread, SPSR_EL1);
    ଯଦି (target_thread->tcbArch.tcbContext.breakpointState.single_step_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } ଅନ୍ୟଥା {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    setRegister(target_thread, SPSR_EL1, spsr);
}

#endif /* ARM_BASE_CP14_SAVE_AND_RESTORE */
