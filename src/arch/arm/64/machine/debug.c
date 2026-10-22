/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API

#అన్తర్భావ <machine/debug.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <arch/machine/debug_conf.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <armv/debug.h>
#అన్తర్భావ <mode/machine/debug.h>
#అన్తర్భావ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#అన్తర్భావ <string.h>
#అన్తర్భావ <util.h>

#నిర్వచించు MDSCR_MDE (బిట్టు(15))
#నిర్వచించు MDSCR_SS  (బిట్టు(0))
#నిర్వచించు SPSR_SS   (బిట్టు(21))

#నిర్వచించు ESR_EXCEPTION_CLASS_ముసుగు 0xFC000000
#నిర్వచించు ESR_EXCEPTION_CLASS_OFF 26

#నిర్వచించు DEBUG_నమోదు_BREAKPOINT      0x30
#నిర్వచించు DEBUG_నమోదు_SINGLE_అడుగు     0x32
#నిర్వచించు DEBUG_నమోదు_WATCHPOINT      0x34
#నిర్వచించు DEBUG_నమోదు_EXPLICIT_BKPT   0x3C

#నిర్వచించు OSDLR_తాళం (బిట్టు(0))
#నిర్వచించు OSLAR_తాళం (బిట్టు(0))

bool_t byte8WatchpointsSupported(శూన్య)
{
    తిరిగి true;
}

exception_t handleDebugFaultEvent(మాట_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(కణుపు_STATE(ksCurThread)));
        ఒకవేళ (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(కణుపు_STATE(ksCurThread));
        }
    })
    కాలపట్టిక();
    activateThread();

    తిరిగి EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_అంకె, మాట_t n_instr,
                               bool_t is_జవాబు)
{
    ఒకవేళ (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = true;
    } లేకపోతే {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    తిరిగి true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_సంకేతం bool_t Arch_initHardwareBreakpoints(శూన్య)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    మాట_t mdscr = 0;
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
    మాట_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_తాళం;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    మాట_t oslar = 0;
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
    కొరకు (పూర్ణాంకం i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    కొరకు (పూర్ణాంకం i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    తిరిగి true;
}

/* Abstract wrapper around the ESR fault status value */

స్థిర మాట_t get_Fault_స్థితి(మాట_t esr)
{
    తిరిగి (esr & ESR_EXCEPTION_CLASS_ముసుగు) >> ESR_EXCEPTION_CLASS_OFF;
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
bool_t isDebugFault(మాట_t esr)
{
    మాట_t exception_class = get_Fault_స్థితి(esr);
    తిరిగి (exception_class == DEBUG_నమోదు_BREAKPOINT ||
            exception_class == DEBUG_నమోదు_SINGLE_అడుగు ||
            exception_class == DEBUG_నమోదు_WATCHPOINT ||
            exception_class == DEBUG_నమోదు_EXPLICIT_BKPT);
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
seL4_Fault_t handleUserLevelDebugException(మాట_t esr, మాట_t fault_vaddr)
{
    పూర్ణాంకం చురుకైన_bp;
    మాట_t bp_reason, bp_vaddr;
    మాట_t exception_class = get_Fault_స్థితి(esr);

#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_Debug_Fault;
    ks_Kernel_నమోదు.మాట = exception_class;
#అయితే_ముగింపు

    ఎంపిక (exception_class) {
    స్థితి DEBUG_నమోదు_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        ఆపు;
    స్థితి DEBUG_నమోదు_WATCHPOINT:
        bp_reason = se_L4_దత్తాంశం_Breakpoint;
        bp_vaddr = getFAR();
        ఆపు;
    స్థితి DEBUG_నమోదు_SINGLE_అడుగు:
        bp_reason = se_L4_Single_అడుగు;
        bp_vaddr = fault_vaddr;
        చురుకైన_bp = 0;
        ఆపు;
    అప్రమేయ: /* EXPLICIT_BKPT: BKPT instruction */
        ప్రతిజ్ఞా(exception_class == DEBUG_నమోదు_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        చురుకైన_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    ఒకవేళ (bp_reason != seL4_SoftwareBreakRequest && bp_reason != se_L4_Single_అడుగు) {
        చురుకైన_bp = get_And_Reset_చురుకైన_Breakpoint(bp_vaddr, bp_reason);
        చురుకైన_bp = get_Bp_అంకె_From_Type(చురుకైన_bp, bp_reason);
        ప్రతిజ్ఞా(చురుకైన_bp >= 0);
    }

    ఒకవేళ (bp_reason == se_L4_Single_అడుగు && !single_అడుగు_Fault_Counter_Ready(కణుపు_STATE(ksCurThread))) {
        తిరిగి seL4_Fault_NullFault_new();
    }

    తిరిగి seL4_Fault_DebugException_new(bp_vaddr, చురుకైన_bp, bp_reason);
}

#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */

#నిర్వచితమైతే ARM_పునాది_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
శూన్య aarch64_restore_user_debug_context(tcb_t *లక్ష్యం_thread)
{
    ప్రతిజ్ఞా(లక్ష్యం_thread != ఖాళీ);

    /* Set/unset single stepping if applicable */
    మాట_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = get_రిజిస్టరు(లక్ష్యం_thread, SPSR_EL1);
    ఒకవేళ (లక్ష్యం_thread->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } లేకపోతే {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    సమితి_రిజిస్టరు(లక్ష్యం_thread, SPSR_EL1, spsr);
}

#అయితే_ముగింపు /* ARM_BASE_CP14_SAVE_AND_RESTORE */
