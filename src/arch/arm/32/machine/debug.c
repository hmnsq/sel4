/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#यदि_परिभाषितम् CONFIG_HARDWARE_DEBUG_API

#अन्तर्भाव <string.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/machine/debug.h>
#अन्तर्भाव <arch/machine/debug_conf.h>
#अन्तर्भाव <arch/kernel/vspace.h>
#अन्तर्भाव <arch/machine/registerset.h>
#अन्तर्भाव <armv/debug.h>
#अन्तर्भाव <mode/machine/debug.h>
#अन्तर्भाव <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#परिभाषय DBGDSCR_MDBGEN                (BIT(15))
#परिभाषय DBGDSCR_HDBGEN                (BIT(14))
#परिभाषय DBGDSCR_USER_ACCESS_DISABLE   (BIT(12))

/* This bit is always RAO */
#परिभाषय DBGLSR_तालकम्_IMPLEMENTED       (BIT(0))
#परिभाषय DBGLSR_तालकम्_ENABLED           (BIT(1))
#परिभाषय DBGLAR_UNLOCK_मानम्           (0xC5ACCE55u)

#परिभाषय DBGOSLAR_तालकम्_मानम्           (0xC5ACCE55u)

#परिभाषय DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#परिभाषय DBGOSLSR_तालकम्_MODEL_NO_OSLOCK (0u)
#परिभाषय DBGOSLSR_तालकम्_MODEL_OSLOCK_AND_OSSR (1u)
#परिभाषय DBGOSLSR_तालकम्_MODEL_OSLOCK_ONLY (2u)

#परिभाषय DBGPRSR_OSLOCK                    (BIT(5))
#परिभाषय DBGPRSR_OS_DLOCK                  (BIT(6))

#परिभाषय DBGOSDLR_तालकम्_ENABLE              (BIT(0))

#परिभाषय DBGAUTHSTATUS_NSI_IMPLEMENTED (BIT(1))
#परिभाषय DBGAUTHSTATUS_NSI_ENABLED     (BIT(0))
#परिभाषय DBGAUTHSTATUS_SI_IMPLEMENTED (BIT(5))
#परिभाषय DBGAUTHSTATUS_SI_ENABLED     (BIT(4))

#परिभाषय DBGDRAR_VALID                (आवरणम्(2))
#परिभाषय DBGDSAR_VALID                (आवरणम्(2))

#परिभाषय DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (BIT(0))

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
प्रकारनिर्देश संरचना debug_state {
    bool_t is_उपलब्धम्, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_अन्तर्_secure_बहुलकम्, single_चरणः_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_संस्करणम्, oem_variant, oem_revision;
} debug_state_t;
स्थिर debug_state_t dbg;

bool_t byte8WatchpointsSupported(शून्य)
{
    प्रत्यागम dbg.watchpoint_8b_supported;
}

#परिभाषय SCR "p15, 0, %0, c1, c1, 0"
#परिभाषय DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#परिभाषय DBGDRCR ""
#परिभाषय DBGVCR "p15, 0, %0, c0, c7, 0"

#परिभाषय DBGDRAR_32 "p14,0,%0,c1,c0,0"
#परिभाषय DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#परिभाषय DBGDSAR_32 "p14,0,%0,c2,c0,0"
#परिभाषय DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#परिभाषय DBGPRSR "p14, 0, %0, c1, c5, 4"

#परिभाषय DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#परिभाषय DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#परिभाषय DBGOSDLR "p14, 0, %0, c1, c3, 4"

#परिभाषय DBGDEVID2 "p14,0,%0,c7,c0,7"
#परिभाषय DBGDEVID1 "p14,0,%0,c7,c1,7"
#परिभाषय DBGDEVID "p14,0,%0,c7,c2,7"
#परिभाषय DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#परिभाषय DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
स्थिर अन्तःस्थ word_t get_विधानम्_सम्बन्धि_प्रविष्टिः(शून्य)
{
    dbg_dscr_t dscr;

    dscr.शब्दाः[0] = readDscrCp();
    प्रत्यागम dbg_dscr_get_विधानम्_सम्बन्धि_प्रविष्टिः(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_अङ्कः,
                               word_t n_instr,
                               bool_t is_प्रत्युत्तरम्)
{

    यदि (is_प्रत्युत्तरम्) {
        bp_अङ्कः = t->tcbArch.tcbContext.breakpointState.single_चरणः_hw_bp_अङ्कः;
    } अन्यथा {
        bp_अङ्कः = convert_Bp_अङ्कः_गन्तव्यम्_Arch(bp_अङ्कः);
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

    bcr.शब्दाः[0] = readBcrContext(t, bp_अङ्कः);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    यदि (n_instr > 0) {
        bcr = dbg_bcr_समुच्चयः_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_चरणः_enabled = true;
    } अन्यथा {
        bcr = dbg_bcr_समुच्चयः_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_चरणः_enabled = false;
    }

    bcr = dbg_bcr_समुच्चयः_lbn(bcr, 0);
    bcr = dbg_bcr_समुच्चयः_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_समुच्चयः_hmc(bcr, 0);
    bcr = dbg_bcr_समुच्चयः_ssc(bcr, 0);
    bcr = dbg_bcr_समुच्चयः_bas(bcr, convert_आकार_गन्तव्यम्_Arch(1));
    bcr = Arch_सज्जता_Bcr(bcr, false);

    writeBvrContext(t, bp_अङ्कः, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_अङ्कः, bcr.शब्दाः[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_चरणः_hw_bp_अङ्कः = bp_अङ्कः;
    प्रत्यागम true;
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
BOOT_संहिता स्थिर शून्य init_संस्करणम्_Info(शून्य)
{
    dbg_didr_t didr;

    didr.शब्दाः[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_संस्करणम् = dbg_didr_get_संस्करणम्(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_चरणः_supported = true;

    विकल्प (dbg.didr_संस्करणम्) {
    पक्ष 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_चरणः_supported = false;
        भङ्ग;
    पक्ष 0x2:
        dbg.debug_armv = 0x61;
        भङ्ग;
    पक्ष 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        भङ्ग;
    पक्ष 0x4:
        dbg.debug_armv = 0x70;
        भङ्ग;
    पक्ष 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        भङ्ग;
    पक्ष 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        भङ्ग;
    स्वभाव:
        dbg.is_उपलब्धम् = false;
        dbg.debug_armv = 0;
        प्रत्यागम;
    }

    dbg.is_उपलब्धम् = true;
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
BOOT_संहिता bool_t Arch_initHardwareBreakpoints(शून्य)
{
    word_t dbgosdlr, dbgoslsr;

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
    स्मृतिं_पूरय(&dbg, 0, परिमाण(dbg));

    init_संस्करणम्_Info();
    यदि (dbg.is_उपलब्धम् == false) {
        प्रारूप_लिख("Debug architecture not implemented.\n");
        प्रत्यागम false;
    }

    प्रारूप_लिख("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_संस्करणम्, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    यदि (dbg.debug_armv > 0x61) {
        यदि (dbg.coprocessor_is_baseline_only) {
            प्रारूप_लिख("ARMDBG: No reliable access to DBG regs.\n");
            प्रत्यागम dbg.is_उपलब्धम् = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_अन्तर्_secure_बहुलकम् = !(readDscrCp() & DBGDSCR_SECURE_बहुलकम्_DISABLED);
        यदि (dbg.cpu_is_अन्तर्_secure_बहुलकम्) {
            word_t sder;

            प्रारूप_लिख("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        यदि (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_तालकम्_ENABLE);
        } अन्यथा यदि (dbg.debug_armv == 0x70) {
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
        यदि (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_तालकम्_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_तालकम्_मानम्);
        }

        disableAllBpsAndWps();
        यदि (!enable_Monitor_बहुलकम्()) {
            प्रत्यागम dbg.is_उपलब्धम् = false;
        }
    } अन्यथा {
        /* On v6 you have to enable monitor mode first. */
        यदि (!enable_Monitor_बहुलकम्()) {
            प्रत्यागम dbg.is_उपलब्धम् = false;
        }
        disableAllBpsAndWps();
    }

    /* Finally, also pre-load some initial register state that can be used
     * for all new threads so that their initial saved debug register state
     * is valid when it's first loaded onto the CPU.
     */
    प्रति (पूर्णाङ्क i = 0; i < se_L4_अङ्कः_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    प्रति (पूर्णाङ्क i = 0; i < se_L4_अङ्कः_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    प्रत्यागम true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
प्रकारनिर्देश संरचना fault_स्थितिः {
    uint8_t स्थितिः;
    bool_t is_long_desc_format;
} fault_स्थितिः_t;

स्थिर fault_स्थितिः_t get_Fault_स्थितिः(word_t hsr_or_fsr)
{
    fault_स्थितिः_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.स्थितिः = hsr_or_fsr & 0x3F;
#अन्यथा
    /* Non-hyp uses IFSR/DFSR */
    यदि (hsr_or_fsr & BIT(FSR_LPAE_सरणम्)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.स्थितिः = hsr_or_fsr & 0x3F;
    } अन्यथा {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.स्थितिः = (hsr_or_fsr & BIT(FSR_स्थितिः_BIT4_सरणम्)) >> FSR_स्थितिः_BIT4_सरणम्;
        ret.स्थितिः <<= 4;
        ret.स्थितिः = hsr_or_fsr & 0xF;
    }
#यदि_अन्तः

    प्रत्यागम ret;
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
bool_t isDebugFault(word_t hsr_or_fsr)
{
    fault_स्थितिः_t fs;

    fs = get_Fault_स्थितिः(hsr_or_fsr);
    यदि (fs.is_long_desc_format) {
        यदि (fs.स्थितिः == FSR_LONGDESC_स्थितिः_DEBUG_EVENT) {
            प्रत्यागम true;
        }
    } अन्यथा {
        यदि (fs.स्थितिः == FSR_SHORTDESC_स्थितिः_DEBUG_EVENT) {
            प्रत्यागम true;
        }
    }

    यदि (get_विधानम्_सम्बन्धि_प्रविष्टिः() == DEBUG_प्रविष्टिः_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        प्रत्यागम true;
    }
    प्रत्यागम false;
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
seL4_Fault_t handleUserLevelDebugException(word_t fault_vaddr)
{
#यदि_परिभाषितम् TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.मार्गः = प्रविष्टिः_Debug_Fault;
    ks_Kernel_प्रविष्टिः.word = fault_vaddr;
#यदि_अन्तः

    word_t विधानम्_सम्बन्धि_प्रविष्टिः = get_विधानम्_सम्बन्धि_प्रविष्टिः();
    पूर्णाङ्क सक्रिय_bp;
    seL4_Fault_t ret;
    word_t bp_reason, bp_vaddr;

    विकल्प (विधानम्_सम्बन्धि_प्रविष्टिः) {
    पक्ष DEBUG_प्रविष्टिः_BREAKPOINT:
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
        tcb_t *curr_thread = ग्रन्थिः_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        यदि (context->breakpointState.single_चरणः_enabled) {
            word_t bvr;
            word_t bp_अङ्कः = context->breakpointState.single_चरणः_hw_bp_अङ्कः;

            bvr = readBvrCp(bp_अङ्कः);
            यदि (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_चरणः;
                सक्रिय_bp = bp_अङ्कः;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, सक्रिय_bp, context->registers[FaultIP]);
            }
        }
        भङ्ग;
    पक्ष DEBUG_प्रविष्टिः_SYNC_WATCHPOINT:
        bp_reason = se_L4_दत्त_Breakpoint;
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#अन्यथा
        bp_vaddr = getFAR();
#यदि_अन्तः
        भङ्ग;

    पक्ष DEBUG_प्रविष्टिः_ASYNC_WATCHPOINT:
        bp_reason = se_L4_दत्त_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        भङ्ग;

    स्वभाव: /* EXPLICIT_BKPT: BKPT instruction */
        प्रतिज्ञा(विधानम्_सम्बन्धि_प्रविष्टिः == DEBUG_प्रविष्टिः_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        सक्रिय_bp = 0;
    }

    यदि (विधानम्_सम्बन्धि_प्रविष्टिः != DEBUG_प्रविष्टिः_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_चरणः) {
        सक्रिय_bp = get_And_Reset_सक्रिय_Breakpoint(bp_vaddr,
                                                bp_reason);
        प्रतिज्ञा(सक्रिय_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    यदि (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        सक्रिय_bp = get_Bp_अङ्कः_From_Type(सक्रिय_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, सक्रिय_bp, bp_reason);
    प्रत्यागम ret;
}

#यदि_अन्तः /* CONFIG_HARDWARE_DEBUG_API */
