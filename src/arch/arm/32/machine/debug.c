/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#اگر_تعریف_شده CONFIG_HARDWARE_DEBUG_API

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

#تعریف_کن DBGDSCR_MDBGEN                (بیت(15))
#تعریف_کن DBGDSCR_HDBGEN                (بیت(14))
#تعریف_کن DBGDSCR_USER_ACCESS_DISABLE   (بیت(12))

/* This bit is always RAO */
#تعریف_کن DBGLSR_قفل_IMPLEMENTED       (بیت(0))
#تعریف_کن DBGLSR_قفل_ENABLED           (بیت(1))
#تعریف_کن DBGLAR_UNLOCK_مقدار           (0xC5ACCE55u)

#تعریف_کن DBGOSLAR_قفل_مقدار           (0xC5ACCE55u)

#تعریف_کن DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#تعریف_کن DBGOSLSR_قفل_MODEL_NO_OSLOCK (0u)
#تعریف_کن DBGOSLSR_قفل_MODEL_OSLOCK_AND_OSSR (1u)
#تعریف_کن DBGOSLSR_قفل_MODEL_OSLOCK_ONLY (2u)

#تعریف_کن DBGPRSR_OSLOCK                    (بیت(5))
#تعریف_کن DBGPRSR_OS_DLOCK                  (بیت(6))

#تعریف_کن DBGOSDLR_قفل_ENABLE              (بیت(0))

#تعریف_کن DBGAUTHSTATUS_NSI_IMPLEMENTED (بیت(1))
#تعریف_کن DBGAUTHSTATUS_NSI_ENABLED     (بیت(0))
#تعریف_کن DBGAUTHSTATUS_SI_IMPLEMENTED (بیت(5))
#تعریف_کن DBGAUTHSTATUS_SI_ENABLED     (بیت(4))

#تعریف_کن DBGDRAR_VALID                (ماسک(2))
#تعریف_کن DBGDSAR_VALID                (ماسک(2))

#تعریف_کن DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (بیت(0))

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
           cpu_is_درون_secure_نما, single_گام_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_نسخه, oem_variant, oem_revision;
} debug_state_t;
ساكن debug_state_t dbg;

bool_t byte8WatchpointsSupported(فراغ)
{
    أرجع dbg.watchpoint_8b_supported;
}

#تعریف_کن SCR "p15, 0, %0, c1, c1, 0"
#تعریف_کن DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#تعریف_کن DBGDRCR ""
#تعریف_کن DBGVCR "p15, 0, %0, c0, c7, 0"

#تعریف_کن DBGDRAR_32 "p14,0,%0,c1,c0,0"
#تعریف_کن DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#تعریف_کن DBGDSAR_32 "p14,0,%0,c2,c0,0"
#تعریف_کن DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#تعریف_کن DBGPRSR "p14, 0, %0, c1, c5, 4"

#تعریف_کن DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#تعریف_کن DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#تعریف_کن DBGOSDLR "p14, 0, %0, c1, c3, 4"

#تعریف_کن DBGDEVID2 "p14,0,%0,c7,c0,7"
#تعریف_کن DBGDEVID1 "p14,0,%0,c7,c1,7"
#تعریف_کن DBGDEVID "p14,0,%0,c7,c2,7"
#تعریف_کن DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#تعریف_کن DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
ساكن مضمن کلمه_t get_روش_از_مدخل(فراغ)
{
    dbg_dscr_t dscr;

    dscr.واژگان[0] = readDscrCp();
    أرجع dbg_dscr_get_روش_از_مدخل(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_رقم,
                               کلمه_t n_instr,
                               bool_t is_پاسخ)
{

    إذا (is_پاسخ) {
        bp_رقم = t->tcbArch.tcbContext.breakpointState.single_گام_hw_bp_رقم;
    } وإلا {
        bp_رقم = convert_Bp_رقم_مقصد_Arch(bp_رقم);
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

    bcr.واژگان[0] = readBcrContext(t, bp_رقم);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    إذا (n_instr > 0) {
        bcr = dbg_bcr_مجموعه_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_گام_enabled = true;
    } وإلا {
        bcr = dbg_bcr_مجموعه_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_گام_enabled = false;
    }

    bcr = dbg_bcr_مجموعه_lbn(bcr, 0);
    bcr = dbg_bcr_مجموعه_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_مجموعه_hmc(bcr, 0);
    bcr = dbg_bcr_مجموعه_ssc(bcr, 0);
    bcr = dbg_bcr_مجموعه_bas(bcr, convert_اندازه_مقصد_Arch(1));
    bcr = Arch_راهاندازی_Bcr(bcr, false);

    writeBvrContext(t, bp_رقم, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_رقم, bcr.واژگان[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_گام_hw_bp_رقم = bp_رقم;
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
BOOT_کد ساكن فراغ init_نسخه_Info(فراغ)
{
    dbg_didr_t didr;

    didr.واژگان[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_نسخه = dbg_didr_get_نسخه(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_گام_supported = true;

    بدل (dbg.didr_نسخه) {
    حالة 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_گام_supported = false;
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
BOOT_کد bool_t Arch_initHardwareBreakpoints(فراغ)
{
    کلمه_t dbgosdlr, dbgoslsr;

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

    init_نسخه_Info();
    إذا (dbg.is_موجود == false) {
        چاپ_کن("Debug architecture not implemented.\n");
        أرجع false;
    }

    چاپ_کن("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_نسخه, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    إذا (dbg.debug_armv > 0x61) {
        إذا (dbg.coprocessor_is_baseline_only) {
            چاپ_کن("ARMDBG: No reliable access to DBG regs.\n");
            أرجع dbg.is_موجود = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_درون_secure_نما = !(readDscrCp() & DBGDSCR_SECURE_نما_DISABLED);
        إذا (dbg.cpu_is_درون_secure_نما) {
            کلمه_t sder;

            چاپ_کن("CPU is in secure mode. Enabling debugging in secure user mode.\n");
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
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_قفل_ENABLE);
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
        إذا (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_قفل_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_قفل_مقدار);
        }

        disableAllBpsAndWps();
        إذا (!enable_Monitor_نما()) {
            أرجع dbg.is_موجود = false;
        }
    } وإلا {
        /* On v6 you have to enable monitor mode first. */
        إذا (!enable_Monitor_نما()) {
            أرجع dbg.is_موجود = false;
        }
        disableAllBpsAndWps();
    }

    /* Finally, also pre-load some initial register state that can be used
     * for all new threads so that their initial saved debug register state
     * is valid when it's first loaded onto the CPU.
     */
    لكل (صحيح i = 0; i < se_L4_رقم_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    لكل (صحيح i = 0; i < se_L4_رقم_Exclusive_Watchpoints; i++) {
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
تعريفنوع بنية fault_وضعیت {
    uint8_t وضعیت;
    bool_t is_long_desc_format;
} fault_وضعیت_t;

ساكن fault_وضعیت_t get_Fault_وضعیت(کلمه_t hsr_or_fsr)
{
    fault_وضعیت_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.وضعیت = hsr_or_fsr & 0x3F;
#وإلا
    /* Non-hyp uses IFSR/DFSR */
    إذا (hsr_or_fsr & بیت(FSR_LPAE_لغزش)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.وضعیت = hsr_or_fsr & 0x3F;
    } وإلا {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.وضعیت = (hsr_or_fsr & بیت(FSR_وضعیت_BIT4_لغزش)) >> FSR_وضعیت_BIT4_لغزش;
        ret.وضعیت <<= 4;
        ret.وضعیت = hsr_or_fsr & 0xF;
    }
#پایان_اگر

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
bool_t isDebugFault(کلمه_t hsr_or_fsr)
{
    fault_وضعیت_t fs;

    fs = get_Fault_وضعیت(hsr_or_fsr);
    إذا (fs.is_long_desc_format) {
        إذا (fs.وضعیت == FSR_LONGDESC_وضعیت_DEBUG_EVENT) {
            أرجع true;
        }
    } وإلا {
        إذا (fs.وضعیت == FSR_SHORTDESC_وضعیت_DEBUG_EVENT) {
            أرجع true;
        }
    }

    إذا (get_روش_از_مدخل() == DEBUG_مدخل_ASYNC_WATCHPOINT) {
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
seL4_Fault_t handleUserLevelDebugException(کلمه_t fault_vaddr)
{
#اگر_تعریف_شده TRACK_KERNEL_ENTRIES
    ks_Kernel_مدخل.مسیر = مدخل_Debug_Fault;
    ks_Kernel_مدخل.کلمه = fault_vaddr;
#پایان_اگر

    کلمه_t روش_از_مدخل = get_روش_از_مدخل();
    صحيح فعال_bp;
    seL4_Fault_t ret;
    کلمه_t bp_reason, bp_vaddr;

    بدل (روش_از_مدخل) {
    حالة DEBUG_مدخل_BREAKPOINT:
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
        tcb_t *curr_thread = گره_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        إذا (context->breakpointState.single_گام_enabled) {
            کلمه_t bvr;
            کلمه_t bp_رقم = context->breakpointState.single_گام_hw_bp_رقم;

            bvr = readBvrCp(bp_رقم);
            إذا (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_گام;
                فعال_bp = bp_رقم;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, فعال_bp, context->registers[FaultIP]);
            }
        }
        اكسر;
    حالة DEBUG_مدخل_SYNC_WATCHPOINT:
        bp_reason = se_L4_داده_Breakpoint;
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#وإلا
        bp_vaddr = getFAR();
#پایان_اگر
        اكسر;

    حالة DEBUG_مدخل_ASYNC_WATCHPOINT:
        bp_reason = se_L4_داده_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        اكسر;

    افتراضي: /* EXPLICIT_BKPT: BKPT instruction */
        أكد(روش_از_مدخل == DEBUG_مدخل_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        فعال_bp = 0;
    }

    إذا (روش_از_مدخل != DEBUG_مدخل_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_گام) {
        فعال_bp = get_And_Reset_فعال_Breakpoint(bp_vaddr,
                                                bp_reason);
        أكد(فعال_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    إذا (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        فعال_bp = get_Bp_رقم_From_Type(فعال_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, فعال_bp, bp_reason);
    أرجع ret;
}

#پایان_اگر /* CONFIG_HARDWARE_DEBUG_API */
