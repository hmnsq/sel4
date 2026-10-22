/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API

#شامل <string.h>
#شامل <util.h>
#شامل <arch/model/statedata.h>
#شامل <arch/machine/debug.h>
#شامل <arch/machine/debug_conf.h>
#شامل <arch/kernel/vspace.h>
#شامل <arch/machine/registerset.h>
#شامل <armv/debug.h>
#شامل <mode/machine/debug.h>
#شامل <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#تعریف_کرو DBGDSCR_MDBGEN                (ثنائی_ہندسہ(15))
#تعریف_کرو DBGDSCR_HDBGEN                (ثنائی_ہندسہ(14))
#تعریف_کرو DBGDSCR_USER_ACCESS_DISABLE   (ثنائی_ہندسہ(12))

/* This bit is always RAO */
#تعریف_کرو DBGLSR_تالا_IMPLEMENTED       (ثنائی_ہندسہ(0))
#تعریف_کرو DBGLSR_تالا_ENABLED           (ثنائی_ہندسہ(1))
#تعریف_کرو DBGLAR_UNLOCK_قدر           (0xC5ACCE55u)

#تعریف_کرو DBGOSLAR_تالا_قدر           (0xC5ACCE55u)

#تعریف_کرو DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#تعریف_کرو DBGOSLSR_تالا_MODEL_NO_OSLOCK (0u)
#تعریف_کرو DBGOSLSR_تالا_MODEL_OSLOCK_AND_OSSR (1u)
#تعریف_کرو DBGOSLSR_تالا_MODEL_OSLOCK_ONLY (2u)

#تعریف_کرو DBGPRSR_OSLOCK                    (ثنائی_ہندسہ(5))
#تعریف_کرو DBGPRSR_OS_DLOCK                  (ثنائی_ہندسہ(6))

#تعریف_کرو DBGOSDLR_تالا_ENABLE              (ثنائی_ہندسہ(0))

#تعریف_کرو DBGAUTHSTATUS_NSI_IMPLEMENTED (ثنائی_ہندسہ(1))
#تعریف_کرو DBGAUTHSTATUS_NSI_ENABLED     (ثنائی_ہندسہ(0))
#تعریف_کرو DBGAUTHSTATUS_SI_IMPLEMENTED (ثنائی_ہندسہ(5))
#تعریف_کرو DBGAUTHSTATUS_SI_ENABLED     (ثنائی_ہندسہ(4))

#تعریف_کرو DBGDRAR_VALID                (نقاب(2))
#تعریف_کرو DBGDSAR_VALID                (نقاب(2))

#تعریف_کرو DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (ثنائی_ہندسہ(0))

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
قسمتعریف ساخت debug_state {
    bool_t is_دستیاب, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_اندر_secure_منوال, single_قدم_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_نسخہ, oem_variant, oem_revision;
} debug_state_t;
ساکن debug_state_t dbg;

bool_t byte8WatchpointsSupported(خالی)
{
    واپس dbg.watchpoint_8b_supported;
}

#تعریف_کرو SCR "p15, 0, %0, c1, c1, 0"
#تعریف_کرو DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#تعریف_کرو DBGDRCR ""
#تعریف_کرو DBGVCR "p15, 0, %0, c0, c7, 0"

#تعریف_کرو DBGDRAR_32 "p14,0,%0,c1,c0,0"
#تعریف_کرو DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#تعریف_کرو DBGDSAR_32 "p14,0,%0,c2,c0,0"
#تعریف_کرو DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#تعریف_کرو DBGPRSR "p14, 0, %0, c1, c5, 4"

#تعریف_کرو DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#تعریف_کرو DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#تعریف_کرو DBGOSDLR "p14, 0, %0, c1, c3, 4"

#تعریف_کرو DBGDEVID2 "p14,0,%0,c7,c0,7"
#تعریف_کرو DBGDEVID1 "p14,0,%0,c7,c1,7"
#تعریف_کرو DBGDEVID "p14,0,%0,c7,c2,7"
#تعریف_کرو DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#تعریف_کرو DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
ساکن اندرونی لفظ_t get_طریقہ_کا_اندراج(خالی)
{
    dbg_dscr_t dscr;

    dscr.الفاظ[0] = readDscrCp();
    واپس dbg_dscr_get_طریقہ_کا_اندراج(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_عدد,
                               لفظ_t n_instr,
                               bool_t is_جواب)
{

    اگر (is_جواب) {
        bp_عدد = t->tcbArch.tcbContext.breakpointState.single_قدم_hw_bp_عدد;
    } ورنہ {
        bp_عدد = convert_Bp_عدد_مقصد_Arch(bp_عدد);
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

    bcr.الفاظ[0] = readBcrContext(t, bp_عدد);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    اگر (n_instr > 0) {
        bcr = dbg_bcr_سیٹ_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = true;
    } ورنہ {
        bcr = dbg_bcr_سیٹ_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = false;
    }

    bcr = dbg_bcr_سیٹ_lbn(bcr, 0);
    bcr = dbg_bcr_سیٹ_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_سیٹ_hmc(bcr, 0);
    bcr = dbg_bcr_سیٹ_ssc(bcr, 0);
    bcr = dbg_bcr_سیٹ_bas(bcr, convert_سائز_مقصد_Arch(1));
    bcr = Arch_تیاری_Bcr(bcr, false);

    writeBvrContext(t, bp_عدد, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_عدد, bcr.الفاظ[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_قدم_hw_bp_عدد = bp_عدد;
    واپس true;
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
BOOT_رمز ساکن خالی init_نسخہ_Info(خالی)
{
    dbg_didr_t didr;

    didr.الفاظ[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_نسخہ = dbg_didr_get_نسخہ(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_قدم_supported = true;

    انتخاب (dbg.didr_نسخہ) {
    حالت 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_قدم_supported = false;
        توڑو;
    حالت 0x2:
        dbg.debug_armv = 0x61;
        توڑو;
    حالت 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        توڑو;
    حالت 0x4:
        dbg.debug_armv = 0x70;
        توڑو;
    حالت 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        توڑو;
    حالت 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        توڑو;
    طےشدہ:
        dbg.is_دستیاب = false;
        dbg.debug_armv = 0;
        واپس;
    }

    dbg.is_دستیاب = true;
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
BOOT_رمز bool_t Arch_initHardwareBreakpoints(خالی)
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

    init_نسخہ_Info();
    اگر (dbg.is_دستیاب == false) {
        لکھو("Debug architecture not implemented.\n");
        واپس false;
    }

    لکھو("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_نسخہ, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    اگر (dbg.debug_armv > 0x61) {
        اگر (dbg.coprocessor_is_baseline_only) {
            لکھو("ARMDBG: No reliable access to DBG regs.\n");
            واپس dbg.is_دستیاب = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_اندر_secure_منوال = !(readDscrCp() & DBGDSCR_SECURE_منوال_DISABLED);
        اگر (dbg.cpu_is_اندر_secure_منوال) {
            لفظ_t sder;

            لکھو("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        اگر (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_تالا_ENABLE);
        } ورنہ اگر (dbg.debug_armv == 0x70) {
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
        اگر (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_تالا_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_تالا_قدر);
        }

        disableAllBpsAndWps();
        اگر (!enable_Monitor_منوال()) {
            واپس dbg.is_دستیاب = false;
        }
    } ورنہ {
        /* On v6 you have to enable monitor mode first. */
        اگر (!enable_Monitor_منوال()) {
            واپس dbg.is_دستیاب = false;
        }
        disableAllBpsAndWps();
    }

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

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    واپس true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
قسمتعریف ساخت fault_حالت {
    uint8_t حالت_2;
    bool_t is_long_desc_format;
} fault_حالت_t;

ساکن fault_حالت_t get_Fault_حالت(لفظ_t hsr_or_fsr)
{
    fault_حالت_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.حالت_2 = hsr_or_fsr & 0x3F;
#ورنہ
    /* Non-hyp uses IFSR/DFSR */
    اگر (hsr_or_fsr & ثنائی_ہندسہ(FSR_LPAE_سرکاؤ)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.حالت_2 = hsr_or_fsr & 0x3F;
    } ورنہ {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.حالت_2 = (hsr_or_fsr & ثنائی_ہندسہ(FSR_حالت_BIT4_سرکاؤ)) >> FSR_حالت_BIT4_سرکاؤ;
        ret.حالت_2 <<= 4;
        ret.حالت_2 = hsr_or_fsr & 0xF;
    }
#اگر_ختم

    واپس ret;
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
    اگر (fs.is_long_desc_format) {
        اگر (fs.حالت_2 == FSR_LONGDESC_حالت_DEBUG_EVENT) {
            واپس true;
        }
    } ورنہ {
        اگر (fs.حالت_2 == FSR_SHORTDESC_حالت_DEBUG_EVENT) {
            واپس true;
        }
    }

    اگر (get_طریقہ_کا_اندراج() == DEBUG_اندراج_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        واپس true;
    }
    واپس false;
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
#اگر_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.راستہ = اندراج_Debug_Fault;
    ks_Kernel_اندراج.لفظ = fault_vaddr;
#اگر_ختم

    لفظ_t طریقہ_کا_اندراج = get_طریقہ_کا_اندراج();
    صحیح فعال_bp;
    seL4_Fault_t ret;
    لفظ_t bp_reason, bp_vaddr;

    انتخاب (طریقہ_کا_اندراج) {
    حالت DEBUG_اندراج_BREAKPOINT:
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
        tcb_t *curr_thread = گرہ_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        اگر (context->breakpointState.single_قدم_enabled) {
            لفظ_t bvr;
            لفظ_t bp_عدد = context->breakpointState.single_قدم_hw_bp_عدد;

            bvr = readBvrCp(bp_عدد);
            اگر (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_قدم;
                فعال_bp = bp_عدد;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, فعال_bp, context->registers[FaultIP]);
            }
        }
        توڑو;
    حالت DEBUG_اندراج_SYNC_WATCHPOINT:
        bp_reason = se_L4_ڈیٹا_Breakpoint;
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#ورنہ
        bp_vaddr = getFAR();
#اگر_ختم
        توڑو;

    حالت DEBUG_اندراج_ASYNC_WATCHPOINT:
        bp_reason = se_L4_ڈیٹا_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        توڑو;

    طےشدہ: /* EXPLICIT_BKPT: BKPT instruction */
        أكد(طریقہ_کا_اندراج == DEBUG_اندراج_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        فعال_bp = 0;
    }

    اگر (طریقہ_کا_اندراج != DEBUG_اندراج_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_قدم) {
        فعال_bp = get_And_Reset_فعال_Breakpoint(bp_vaddr,
                                                bp_reason);
        أكد(فعال_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    اگر (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        فعال_bp = get_Bp_عدد_From_Type(فعال_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, فعال_bp, bp_reason);
    واپس ret;
}

#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */
