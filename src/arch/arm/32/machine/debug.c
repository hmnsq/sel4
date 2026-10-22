/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API

#అన్తర్భావ <string.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/machine/debug.h>
#అన్తర్భావ <arch/machine/debug_conf.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/machine/registerset.h>
#అన్తర్భావ <armv/debug.h>
#అన్తర్భావ <mode/machine/debug.h>
#అన్తర్భావ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#నిర్వచించు DBGDSCR_MDBGEN                (బిట్టు(15))
#నిర్వచించు DBGDSCR_HDBGEN                (బిట్టు(14))
#నిర్వచించు DBGDSCR_USER_ACCESS_DISABLE   (బిట్టు(12))

/* This bit is always RAO */
#నిర్వచించు DBGLSR_తాళం_IMPLEMENTED       (బిట్టు(0))
#నిర్వచించు DBGLSR_తాళం_ENABLED           (బిట్టు(1))
#నిర్వచించు DBGLAR_UNLOCK_విలువ           (0xC5ACCE55u)

#నిర్వచించు DBGOSLAR_తాళం_విలువ           (0xC5ACCE55u)

#నిర్వచించు DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#నిర్వచించు DBGOSLSR_తాళం_MODEL_NO_OSLOCK (0u)
#నిర్వచించు DBGOSLSR_తాళం_MODEL_OSLOCK_AND_OSSR (1u)
#నిర్వచించు DBGOSLSR_తాళం_MODEL_OSLOCK_ONLY (2u)

#నిర్వచించు DBGPRSR_OSLOCK                    (బిట్టు(5))
#నిర్వచించు DBGPRSR_OS_DLOCK                  (బిట్టు(6))

#నిర్వచించు DBGOSDLR_తాళం_ENABLE              (బిట్టు(0))

#నిర్వచించు DBGAUTHSTATUS_NSI_IMPLEMENTED (బిట్టు(1))
#నిర్వచించు DBGAUTHSTATUS_NSI_ENABLED     (బిట్టు(0))
#నిర్వచించు DBGAUTHSTATUS_SI_IMPLEMENTED (బిట్టు(5))
#నిర్వచించు DBGAUTHSTATUS_SI_ENABLED     (బిట్టు(4))

#నిర్వచించు DBGDRAR_VALID                (ముసుగు(2))
#నిర్వచించు DBGDSAR_VALID                (ముసుగు(2))

#నిర్వచించు DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (బిట్టు(0))

/* ARMv7 Manuals, c3.3.1:
 *  "Breakpoint debug events are synchronous. That is, the debug event acts
 *  like an exception that cancels the breakpointed instruction."
 *
 * ARMv7 Manuals, c3.4.1:
 *  "Watchpoint debug events are precise and can be synchronous or asynchronous:
 *  a synchronous Watchpoint debug event acts like a synchronous abort
 *  exception on the memory access instruction itself. An asynchronous
 *  Watchpoint debug event acts like a precise asynchronous abort exception that
 *  cancels a later instruction."
 */


/** Describes the availability and level of support for the debug features on
 * a particular CPU. Currently a static local singleton instance, but for
 * multiprocessor adaptation, just make it per-CPU.
 *
 * The majority of the writing to the debug coprocessor is done when a thread
 * is being context-switched to, so the code in this file always executes on
 * the target CPU. MP adaptation should come with few surprises.
 */
రకనిర్వచనం నిర్మాణం debug_state {
    bool_t is_అందుబాటు, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_లోపల_secure_బహుళకం, single_అడుగు_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_సంస్కరణ, oem_variant, oem_revision;
} debug_state_t;
స్థిర debug_state_t dbg;

bool_t byte8WatchpointsSupported(శూన్య)
{
    తిరిగి dbg.watchpoint_8b_supported;
}

#నిర్వచించు SCR "p15, 0, %0, c1, c1, 0"
#నిర్వచించు DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#నిర్వచించు DBGDRCR ""
#నిర్వచించు DBGVCR "p15, 0, %0, c0, c7, 0"

#నిర్వచించు DBGDRAR_32 "p14,0,%0,c1,c0,0"
#నిర్వచించు DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#నిర్వచించు DBGDSAR_32 "p14,0,%0,c2,c0,0"
#నిర్వచించు DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#నిర్వచించు DBGPRSR "p14, 0, %0, c1, c5, 4"

#నిర్వచించు DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#నిర్వచించు DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#నిర్వచించు DBGOSDLR "p14, 0, %0, c1, c3, 4"

#నిర్వచించు DBGDEVID2 "p14,0,%0,c7,c0,7"
#నిర్వచించు DBGDEVID1 "p14,0,%0,c7,c1,7"
#నిర్వచించు DBGDEVID "p14,0,%0,c7,c2,7"
#నిర్వచించు DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#నిర్వచించు DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
స్థిర అంతఃవరుస మాట_t get_పద్ధతి_యొక్క_నమోదు(శూన్య)
{
    dbg_dscr_t dscr;

    dscr.పదాలు[0] = readDscrCp();
    తిరిగి dbg_dscr_get_పద్ధతి_యొక్క_నమోదు(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_అంకె,
                               మాట_t n_instr,
                               bool_t is_జవాబు)
{

    ఒకవేళ (is_జవాబు) {
        bp_అంకె = t->tcbArch.tcbContext.breakpointState.single_అడుగు_hw_bp_అంకె;
    } లేకపోతే {
        bp_అంకె = convert_Bp_అంకె_గమ్యం_Arch(bp_అంకె);
    }

    /* On ARM single-stepping is emulated using breakpoint mismatches. So you
     * would basically set the breakpoint to mismatch everything, and this will
     * cause an exception to be triggered on every instruction.
     *
     * We use NULL as the mismatch address since no code should be trying to
     * execute NULL, so it's a perfect address to use as the mismatch
     * criterion. An alternative might be to use an address in the kernel's
     * high vaddrspace, since that's an address that it's impossible for
     * userspace to be executing at.
     */
    dbg_bcr_t bcr;

    bcr.పదాలు[0] = readBcrContext(t, bp_అంకె);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    ఒకవేళ (n_instr > 0) {
        bcr = dbg_bcr_సమితి_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = true;
    } లేకపోతే {
        bcr = dbg_bcr_సమితి_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled = false;
    }

    bcr = dbg_bcr_సమితి_lbn(bcr, 0);
    bcr = dbg_bcr_సమితి_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_సమితి_hmc(bcr, 0);
    bcr = dbg_bcr_సమితి_ssc(bcr, 0);
    bcr = dbg_bcr_సమితి_bas(bcr, convert_కొలత_గమ్యం_Arch(1));
    bcr = Arch_అమరిక_Bcr(bcr, false);

    writeBvrContext(t, bp_అంకె, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_అంకె, bcr.పదాలు[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_అడుగు_hw_bp_అంకె = bp_అంకె;
    తిరిగి true;
}

/** Using the DBGDIDR register, detects the debug architecture version, and
 * does a preliminary check for the level of support for our debug API.
 *
 * Reads DBGDIDR, which is guaranteed to be read safely. Then
 * determine whether or not we can or should proceed.
 *
 * The majority of the debug setup is concerned with trying to tell which
 * registers are safe to access on this CPU. The debug architecture is wildly
 * different across different CPUs and platforms, so genericity is fairly
 * challenging.
 */
BOOT_సంకేతం స్థిర శూన్య init_సంస్కరణ_Info(శూన్య)
{
    dbg_didr_t didr;

    didr.పదాలు[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_సంస్కరణ = dbg_didr_get_సంస్కరణ(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_అడుగు_supported = true;

    ఎంపిక (dbg.didr_సంస్కరణ) {
    స్థితి 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_అడుగు_supported = false;
        ఆపు;
    స్థితి 0x2:
        dbg.debug_armv = 0x61;
        ఆపు;
    స్థితి 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        ఆపు;
    స్థితి 0x4:
        dbg.debug_armv = 0x70;
        ఆపు;
    స్థితి 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        ఆపు;
    స్థితి 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        ఆపు;
    అప్రమేయ:
        dbg.is_అందుబాటు = false;
        dbg.debug_armv = 0;
        తిరిగి;
    }

    dbg.is_అందుబాటు = true;
}

/** Guides the debug hardware initialization sequence.
 *
 * In short, there is a small set of registers, the "baseline" registers, which
 * are guaranteed to be available on all ARM debug architecture implementations.
 * Aside from those, the rest are a *COMPLETE* toss-up, and detection is
 * difficult, because if you access any particular register which is
 * unavailable on an implementation, you trigger an #UNDEFINED exception. And
 * there is little uniformity or consistency.
 *
 * In addition, there are as many as 3 lock registers, all of which have
 * effects on which registers you can access...and only one of them is
 * consistently implemented. The others may or may not be implemented, and well,
 * you have to grope in the dark to determine whether or not they are...but
 * if they are implemented, their effect on software is still upheld, of course.
 *
 * Much of this sequence is catering for the different versions and determining
 * which registers and locks are implemented, and creating a common register
 * environment for the rest of the API code.
 *
 * There are several conditions which will cause the code to exit and give up.
 * For the most part, most implementations give you the baseline registers and
 * some others. When an implementation only supports the baseline registers and
 * nothing more, you're told so, and that basically means you can't do anything
 * with it because you have no reliable access to the debug registers.
 */
BOOT_సంకేతం bool_t Arch_initHardwareBreakpoints(శూన్య)
{
    మాట_t dbgosdlr, dbgoslsr;

    /* The functioning of breakpoints on ARM requires that certain external
     * pin signals be enabled. If these are not enabled, there is nothing
     * that can be done from software. If these are enabled, we can then
     * select the debug-mode we want by programming the CP14 interface.
     *
     * Of the four modes available, we want monitor mode, because only monitor
     * mode delivers breakpoint and watchpoint events to the kernel as
     * exceptions. The other modes cause a break into "debug mode" or ignore
     * debug events.
     */
    memset(&dbg, 0, పరిమాణం(dbg));

    init_సంస్కరణ_Info();
    ఒకవేళ (dbg.is_అందుబాటు == false) {
        రాయి("Debug architecture not implemented.\n");
        తిరిగి false;
    }

    రాయి("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_సంస్కరణ, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    ఒకవేళ (dbg.debug_armv > 0x61) {
        ఒకవేళ (dbg.coprocessor_is_baseline_only) {
            రాయి("ARMDBG: No reliable access to DBG regs.\n");
            తిరిగి dbg.is_అందుబాటు = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_లోపల_secure_బహుళకం = !(readDscrCp() & DBGDSCR_SECURE_బహుళకం_DISABLED);
        ఒకవేళ (dbg.cpu_is_లోపల_secure_బహుళకం) {
            మాట_t sder;

            రాయి("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        ఒకవేళ (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_తాళం_ENABLE);
        } లేకపోతే ఒకవేళ (dbg.debug_armv == 0x70) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7 Debug, this register is not implemented."
             *
             * So no need to do anything for debug v7.0.
             */
        }

        /* Now deal with OS lock: ARMv7 manual, C11.11.32:
         *  "In any implementation, software can read this register to detect
         *   whether the OS Save and Restore mechanism is implemented. If it is
         *   not implemented the read of DBGOSLSR.OSLM returns zero."
         */
        MRC(DBGOSLSR, dbgoslsr);
        ఒకవేళ (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_తాళం_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_తాళం_విలువ);
        }

        disableAllBpsAndWps();
        ఒకవేళ (!enable_Monitor_బహుళకం()) {
            తిరిగి dbg.is_అందుబాటు = false;
        }
    } లేకపోతే {
        /* On v6 you have to enable monitor mode first. */
        ఒకవేళ (!enable_Monitor_బహుళకం()) {
            తిరిగి dbg.is_అందుబాటు = false;
        }
        disableAllBpsAndWps();
    }

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

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    తిరిగి true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
రకనిర్వచనం నిర్మాణం fault_స్థితి {
    uint8_t స్థితి_2;
    bool_t is_long_desc_format;
} fault_స్థితి_t;

స్థిర fault_స్థితి_t get_Fault_స్థితి(మాట_t hsr_or_fsr)
{
    fault_స్థితి_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.స్థితి_2 = hsr_or_fsr & 0x3F;
#లేకపోతే
    /* Non-hyp uses IFSR/DFSR */
    ఒకవేళ (hsr_or_fsr & బిట్టు(FSR_LPAE_జరుపు)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.స్థితి_2 = hsr_or_fsr & 0x3F;
    } లేకపోతే {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.స్థితి_2 = (hsr_or_fsr & బిట్టు(FSR_స్థితి_BIT4_జరుపు)) >> FSR_స్థితి_BIT4_జరుపు;
        ret.స్థితి_2 <<= 4;
        ret.స్థితి_2 = hsr_or_fsr & 0xF;
    }
#అయితే_ముగింపు

    తిరిగి ret;
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
bool_t isDebugFault(మాట_t hsr_or_fsr)
{
    fault_స్థితి_t fs;

    fs = get_Fault_స్థితి(hsr_or_fsr);
    ఒకవేళ (fs.is_long_desc_format) {
        ఒకవేళ (fs.స్థితి_2 == FSR_LONGDESC_స్థితి_DEBUG_EVENT) {
            తిరిగి true;
        }
    } లేకపోతే {
        ఒకవేళ (fs.స్థితి_2 == FSR_SHORTDESC_స్థితి_DEBUG_EVENT) {
            తిరిగి true;
        }
    }

    ఒకవేళ (get_పద్ధతి_యొక్క_నమోదు() == DEBUG_నమోదు_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        తిరిగి true;
    }
    తిరిగి false;
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
seL4_Fault_t handleUserLevelDebugException(మాట_t fault_vaddr)
{
#నిర్వచితమైతే TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.మార్గం = నమోదు_Debug_Fault;
    ks_Kernel_నమోదు.మాట = fault_vaddr;
#అయితే_ముగింపు

    మాట_t పద్ధతి_యొక్క_నమోదు = get_పద్ధతి_యొక్క_నమోదు();
    పూర్ణాంకం చురుకైన_bp;
    seL4_Fault_t ret;
    మాట_t bp_reason, bp_vaddr;

    ఎంపిక (పద్ధతి_యొక్క_నమోదు) {
    స్థితి DEBUG_నమోదు_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;

        /* Could have been triggered by:
         *  1. An actual breakpoint.
         *  2. A breakpoint configured in mismatch mode to emulate
         *  single-stepping.
         *
         * We assume that the exception was triggered by a normal breakpoint
         * unless the thread currently has single stepping enabled and the
         * breakpoint value register used for this is mismatched with the
         * current faultIP
         */
        tcb_t *curr_thread = కణుపు_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        ఒకవేళ (context->breakpointState.single_అడుగు_enabled) {
            మాట_t bvr;
            మాట_t bp_అంకె = context->breakpointState.single_అడుగు_hw_bp_అంకె;

            bvr = readBvrCp(bp_అంకె);
            ఒకవేళ (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_అడుగు;
                చురుకైన_bp = bp_అంకె;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, చురుకైన_bp, context->registers[FaultIP]);
            }
        }
        ఆపు;
    స్థితి DEBUG_నమోదు_SYNC_WATCHPOINT:
        bp_reason = se_L4_దత్తాంశం_Breakpoint;
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#లేకపోతే
        bp_vaddr = getFAR();
#అయితే_ముగింపు
        ఆపు;

    స్థితి DEBUG_నమోదు_ASYNC_WATCHPOINT:
        bp_reason = se_L4_దత్తాంశం_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        ఆపు;

    అప్రమేయ: /* EXPLICIT_BKPT: BKPT instruction */
        ప్రతిజ్ఞా(పద్ధతి_యొక్క_నమోదు == DEBUG_నమోదు_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        చురుకైన_bp = 0;
    }

    ఒకవేళ (పద్ధతి_యొక్క_నమోదు != DEBUG_నమోదు_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_అడుగు) {
        చురుకైన_bp = get_And_Reset_చురుకైన_Breakpoint(bp_vaddr,
                                                bp_reason);
        ప్రతిజ్ఞా(చురుకైన_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    ఒకవేళ (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        చురుకైన_bp = get_Bp_అంకె_From_Type(చురుకైన_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, చురుకైన_bp, bp_reason);
    తిరిగి ret;
}

#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */
