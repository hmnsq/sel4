/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#جيڪڏهن_وصف_ٿيل CONFIG_HARDWARE_DEBUG_API

#ضمن <string.h>
#ضمن <util.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/machine/debug.h>
#ضمن <arch/machine/debug_conf.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/machine/registerset.h>
#ضمن <armv/debug.h>
#ضمن <mode/machine/debug.h>
#ضمن <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#وصف_ڪريو DBGDSCR_MDBGEN                (ٻٽو_انگ(15))
#وصف_ڪريو DBGDSCR_HDBGEN                (ٻٽو_انگ(14))
#وصف_ڪريو DBGDSCR_USER_ACCESS_DISABLE   (ٻٽو_انگ(12))

/* This bit is always RAO */
#وصف_ڪريو DBGLSR_تالو_IMPLEMENTED       (ٻٽو_انگ(0))
#وصف_ڪريو DBGLSR_تالو_ENABLED           (ٻٽو_انگ(1))
#وصف_ڪريو DBGLAR_UNLOCK_قدر           (0xC5ACCE55u)

#وصف_ڪريو DBGOSLAR_تالو_قدر           (0xC5ACCE55u)

#وصف_ڪريو DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#وصف_ڪريو DBGOSLSR_تالو_MODEL_NO_OSLOCK (0u)
#وصف_ڪريو DBGOSLSR_تالو_MODEL_OSLOCK_AND_OSSR (1u)
#وصف_ڪريو DBGOSLSR_تالو_MODEL_OSLOCK_ONLY (2u)

#وصف_ڪريو DBGPRSR_OSLOCK                    (ٻٽو_انگ(5))
#وصف_ڪريو DBGPRSR_OS_DLOCK                  (ٻٽو_انگ(6))

#وصف_ڪريو DBGOSDLR_تالو_ENABLE              (ٻٽو_انگ(0))

#وصف_ڪريو DBGAUTHSTATUS_NSI_IMPLEMENTED (ٻٽو_انگ(1))
#وصف_ڪريو DBGAUTHSTATUS_NSI_ENABLED     (ٻٽو_انگ(0))
#وصف_ڪريو DBGAUTHSTATUS_SI_IMPLEMENTED (ٻٽو_انگ(5))
#وصف_ڪريو DBGAUTHSTATUS_SI_ENABLED     (ٻٽو_انگ(4))

#وصف_ڪريو DBGDRAR_VALID                (نقاب(2))
#وصف_ڪريو DBGDSAR_VALID                (نقاب(2))

#وصف_ڪريو DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (ٻٽو_انگ(0))

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
تعريفنوع بنية debug_state {
    bool_t is_موجود, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_اندريون_secure_گهڻائي, single_قدم_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_نسخو, oem_variant, oem_revision;
} debug_state_t;
ساكن debug_state_t dbg;

bool_t byte8WatchpointsSupported(فراغ)
{
    أرجع dbg.watchpoint_8b_supported;
}

#وصف_ڪريو SCR "p15, 0, %0, c1, c1, 0"
#وصف_ڪريو DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#وصف_ڪريو DBGDRCR ""
#وصف_ڪريو DBGVCR "p15, 0, %0, c0, c7, 0"

#وصف_ڪريو DBGDRAR_32 "p14,0,%0,c1,c0,0"
#وصف_ڪريو DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#وصف_ڪريو DBGDSAR_32 "p14,0,%0,c2,c0,0"
#وصف_ڪريو DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#وصف_ڪريو DBGPRSR "p14, 0, %0, c1, c5, 4"

#وصف_ڪريو DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#وصف_ڪريو DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#وصف_ڪريو DBGOSDLR "p14, 0, %0, c1, c3, 4"

#وصف_ڪريو DBGDEVID2 "p14,0,%0,c7,c0,7"
#وصف_ڪريو DBGDEVID1 "p14,0,%0,c7,c1,7"
#وصف_ڪريو DBGDEVID "p14,0,%0,c7,c2,7"
#وصف_ڪريو DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#وصف_ڪريو DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
ساكن مضمن لفظ_t get_طريقو_جو_داخلا(فراغ)
{
    dbg_dscr_t dscr;

    dscr.لفظ[0] = readDscrCp();
    أرجع dbg_dscr_get_طريقو_جو_داخلا(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_انگ,
                               لفظ_t n_instr,
                               bool_t is_جواب)
{

    إذا (is_جواب) {
        bp_انگ = t->tcbArch.tcbContext.breakpointState.single_قدم_hw_bp_انگ;
    } وإلا {
        bp_انگ = convert_Bp_انگ_منزل_Arch(bp_انگ);
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

    bcr.لفظ[0] = readBcrContext(t, bp_انگ);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    إذا (n_instr > 0) {
        bcr = dbg_bcr_سيٽ_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = true;
    } وإلا {
        bcr = dbg_bcr_سيٽ_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = false;
    }

    bcr = dbg_bcr_سيٽ_lbn(bcr, 0);
    bcr = dbg_bcr_سيٽ_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_سيٽ_hmc(bcr, 0);
    bcr = dbg_bcr_سيٽ_ssc(bcr, 0);
    bcr = dbg_bcr_سيٽ_bas(bcr, convert_سائيز_منزل_Arch(1));
    bcr = Arch_تياري_Bcr(bcr, false);

    writeBvrContext(t, bp_انگ, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_انگ, bcr.لفظ[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_قدم_hw_bp_انگ = bp_انگ;
    أرجع true;
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
BOOT_ڪوڊ ساكن فراغ init_نسخو_Info(فراغ)
{
    dbg_didr_t didr;

    didr.لفظ[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_نسخو = dbg_didr_get_نسخو(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_قدم_supported = true;

    بدل (dbg.didr_نسخو) {
    حالة 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_قدم_supported = false;
        اكسر;
    حالة 0x2:
        dbg.debug_armv = 0x61;
        اكسر;
    حالة 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        اكسر;
    حالة 0x4:
        dbg.debug_armv = 0x70;
        اكسر;
    حالة 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        اكسر;
    حالة 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        اكسر;
    افتراضي:
        dbg.is_موجود = false;
        dbg.debug_armv = 0;
        أرجع;
    }

    dbg.is_موجود = true;
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
BOOT_ڪوڊ bool_t Arch_initHardwareBreakpoints(فراغ)
{
    لفظ_t dbgosdlr, dbgoslsr;

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
    memset(&dbg, 0, حجم(dbg));

    init_نسخو_Info();
    إذا (dbg.is_موجود == false) {
        لکو("Debug architecture not implemented.\n");
        أرجع false;
    }

    لکو("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_نسخو, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    إذا (dbg.debug_armv > 0x61) {
        إذا (dbg.coprocessor_is_baseline_only) {
            لکو("ARMDBG: No reliable access to DBG regs.\n");
            أرجع dbg.is_موجود = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_اندريون_secure_گهڻائي = !(readDscrCp() & DBGDSCR_SECURE_گهڻائي_DISABLED);
        إذا (dbg.cpu_is_اندريون_secure_گهڻائي) {
            لفظ_t sder;

            لکو("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        إذا (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_تالو_ENABLE);
        } وإلا إذا (dbg.debug_armv == 0x70) {
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
        إذا (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_تالو_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_تالو_قدر);
        }

        disableAllBpsAndWps();
        إذا (!enable_Monitor_گهڻائي()) {
            أرجع dbg.is_موجود = false;
        }
    } وإلا {
        /* On v6 you have to enable monitor mode first. */
        إذا (!enable_Monitor_گهڻائي()) {
            أرجع dbg.is_موجود = false;
        }
        disableAllBpsAndWps();
    }

    /* Finally, also pre-load some initial register state that can be used
     * for all new threads so that their initial saved debug register state
     * is valid when it's first loaded onto the CPU.
     */
    لكل (صحيح i = 0; i < se_L4_انگ_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    لكل (صحيح i = 0; i < se_L4_انگ_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    أرجع true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
تعريفنوع بنية fault_حالت {
    uint8_t حالت;
    bool_t is_long_desc_format;
} fault_حالت_t;

ساكن fault_حالت_t get_Fault_حالت(لفظ_t hsr_or_fsr)
{
    fault_حالت_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.حالت = hsr_or_fsr & 0x3F;
#وإلا
    /* Non-hyp uses IFSR/DFSR */
    إذا (hsr_or_fsr & ٻٽو_انگ(FSR_LPAE_سرڪايو)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.حالت = hsr_or_fsr & 0x3F;
    } وإلا {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.حالت = (hsr_or_fsr & ٻٽو_انگ(FSR_حالت_BIT4_سرڪايو)) >> FSR_حالت_BIT4_سرڪايو;
        ret.حالت <<= 4;
        ret.حالت = hsr_or_fsr & 0xF;
    }
#جيڪڏهن_پورو

    أرجع ret;
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
bool_t isDebugFault(لفظ_t hsr_or_fsr)
{
    fault_حالت_t fs;

    fs = get_Fault_حالت(hsr_or_fsr);
    إذا (fs.is_long_desc_format) {
        إذا (fs.حالت == FSR_LONGDESC_حالت_DEBUG_EVENT) {
            أرجع true;
        }
    } وإلا {
        إذا (fs.حالت == FSR_SHORTDESC_حالت_DEBUG_EVENT) {
            أرجع true;
        }
    }

    إذا (get_طريقو_جو_داخلا() == DEBUG_داخلا_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        أرجع true;
    }
    أرجع false;
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
seL4_Fault_t handleUserLevelDebugException(لفظ_t fault_vaddr)
{
#جيڪڏهن_وصف_ٿيل TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.رستو = داخلا_Debug_Fault;
    ks_Kernel_داخلا.لفظ_2 = fault_vaddr;
#جيڪڏهن_پورو

    لفظ_t طريقو_جو_داخلا = get_طريقو_جو_داخلا();
    صحيح سرگرم_bp;
    seL4_Fault_t ret;
    لفظ_t bp_reason, bp_vaddr;

    بدل (طريقو_جو_داخلا) {
    حالة DEBUG_داخلا_BREAKPOINT:
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
        tcb_t *curr_thread = ڳنڍ_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        إذا (context->breakpointState.single_قدم_enabled) {
            لفظ_t bvr;
            لفظ_t bp_انگ = context->breakpointState.single_قدم_hw_bp_انگ;

            bvr = readBvrCp(bp_انگ);
            إذا (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_قدم;
                سرگرم_bp = bp_انگ;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, سرگرم_bp, context->registers[FaultIP]);
            }
        }
        اكسر;
    حالة DEBUG_داخلا_SYNC_WATCHPOINT:
        bp_reason = se_L4_معلومات_Breakpoint;
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#وإلا
        bp_vaddr = getFAR();
#جيڪڏهن_پورو
        اكسر;

    حالة DEBUG_داخلا_ASYNC_WATCHPOINT:
        bp_reason = se_L4_معلومات_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        اكسر;

    افتراضي: /* EXPLICIT_BKPT: BKPT instruction */
        أكد(طريقو_جو_داخلا == DEBUG_داخلا_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        سرگرم_bp = 0;
    }

    إذا (طريقو_جو_داخلا != DEBUG_داخلا_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_قدم) {
        سرگرم_bp = get_And_Reset_سرگرم_Breakpoint(bp_vaddr,
                                                bp_reason);
        أكد(سرگرم_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    إذا (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        سرگرم_bp = get_Bp_انگ_From_Type(سرگرم_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, سرگرم_bp, bp_reason);
    أرجع ret;
}

#جيڪڏهن_پورو /* CONFIG_HARDWARE_DEBUG_API */
