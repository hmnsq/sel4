/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HARDWARE_DEBUG_API

#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/debug_conf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <armv/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/machine/debug.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSCR_MDBGEN                (ਦੋਆਧਾਰੀ_ਅੰਕ(15))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSCR_HDBGEN                (ਦੋਆਧਾਰੀ_ਅੰਕ(14))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSCR_USER_ACCESS_DISABLE   (ਦੋਆਧਾਰੀ_ਅੰਕ(12))

/* This bit is always RAO */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGLSR_ਤਾਲਾ_IMPLEMENTED       (ਦੋਆਧਾਰੀ_ਅੰਕ(0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGLSR_ਤਾਲਾ_ENABLED           (ਦੋਆਧਾਰੀ_ਅੰਕ(1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGLAR_UNLOCK_ਮੁੱਲ           (0xC5ACCE55u)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLAR_ਤਾਲਾ_ਮੁੱਲ           (0xC5ACCE55u)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLSR_ਤਾਲਾ_MODEL_NO_OSLOCK (0u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLSR_ਤਾਲਾ_MODEL_OSLOCK_AND_OSSR (1u)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLSR_ਤਾਲਾ_MODEL_OSLOCK_ONLY (2u)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGPRSR_OSLOCK                    (ਦੋਆਧਾਰੀ_ਅੰਕ(5))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGPRSR_OS_DLOCK                  (ਦੋਆਧਾਰੀ_ਅੰਕ(6))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSDLR_ਤਾਲਾ_ENABLE              (ਦੋਆਧਾਰੀ_ਅੰਕ(0))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGAUTHSTATUS_NSI_IMPLEMENTED (ਦੋਆਧਾਰੀ_ਅੰਕ(1))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGAUTHSTATUS_NSI_ENABLED     (ਦੋਆਧਾਰੀ_ਅੰਕ(0))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGAUTHSTATUS_SI_IMPLEMENTED (ਦੋਆਧਾਰੀ_ਅੰਕ(5))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGAUTHSTATUS_SI_ENABLED     (ਦੋਆਧਾਰੀ_ਅੰਕ(4))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDRAR_VALID                (ਨਕਾਬ(2))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSAR_VALID                (ਨਕਾਬ(2))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (ਦੋਆਧਾਰੀ_ਅੰਕ(0))

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
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ debug_state {
    bool_t is_ਉਪਲਬਧ, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_ਵਿੱਚ_secure_ਬਹੁਲਕ, single_ਕਦਮ_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_ਸੰਸਕਰਣ, oem_variant, oem_revision;
} debug_state_t;
ਸਥਿਰ debug_state_t dbg;

bool_t byte8WatchpointsSupported(ਖਾਲੀ)
{
    ਵਾਪਸ dbg.watchpoint_8b_supported;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SCR "p15, 0, %0, c1, c1, 0"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDRCR ""
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGVCR "p15, 0, %0, c0, c7, 0"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDRAR_32 "p14,0,%0,c1,c0,0"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSAR_32 "p14,0,%0,c2,c0,0"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGPRSR "p14, 0, %0, c1, c5, 4"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGOSDLR "p14, 0, %0, c1, c3, 4"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDEVID2 "p14,0,%0,c7,c0,7"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDEVID1 "p14,0,%0,c7,c1,7"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDEVID "p14,0,%0,c7,c2,7"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t get_ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ(ਖਾਲੀ)
{
    dbg_dscr_t dscr;

    dscr.ਸ਼ਬਦ[0] = readDscrCp();
    ਵਾਪਸ dbg_dscr_get_ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_ਅੰਕ,
                               ਸ਼ਬਦ_t n_instr,
                               bool_t is_ਜਵਾਬ)
{

    ਜੇ (is_ਜਵਾਬ) {
        bp_ਅੰਕ = t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_hw_bp_ਅੰਕ;
    } ਨਹੀਂਤਾਂ {
        bp_ਅੰਕ = convert_Bp_ਅੰਕ_ਮੰਜ਼ਿਲ_Arch(bp_ਅੰਕ);
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

    bcr.ਸ਼ਬਦ[0] = readBcrContext(t, bp_ਅੰਕ);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    ਜੇ (n_instr > 0) {
        bcr = dbg_bcr_ਸੈੱਟ_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = true;
    } ਨਹੀਂਤਾਂ {
        bcr = dbg_bcr_ਸੈੱਟ_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_enabled = false;
    }

    bcr = dbg_bcr_ਸੈੱਟ_lbn(bcr, 0);
    bcr = dbg_bcr_ਸੈੱਟ_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_ਸੈੱਟ_hmc(bcr, 0);
    bcr = dbg_bcr_ਸੈੱਟ_ssc(bcr, 0);
    bcr = dbg_bcr_ਸੈੱਟ_bas(bcr, convert_ਮਾਪ_ਮੰਜ਼ਿਲ_Arch(1));
    bcr = Arch_ਤਿਆਰੀ_Bcr(bcr, false);

    writeBvrContext(t, bp_ਅੰਕ, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_ਅੰਕ, bcr.ਸ਼ਬਦ[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_ਕਦਮ_hw_bp_ਅੰਕ = bp_ਅੰਕ;
    ਵਾਪਸ true;
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
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_ਸੰਸਕਰਣ_Info(ਖਾਲੀ)
{
    dbg_didr_t didr;

    didr.ਸ਼ਬਦ[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_ਸੰਸਕਰਣ = dbg_didr_get_ਸੰਸਕਰਣ(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_ਕਦਮ_supported = true;

    ਚੋਣ (dbg.didr_ਸੰਸਕਰਣ) {
    ਹਾਲਤ 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_ਕਦਮ_supported = false;
        ਰੋਕੋ;
    ਹਾਲਤ 0x2:
        dbg.debug_armv = 0x61;
        ਰੋਕੋ;
    ਹਾਲਤ 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        ਰੋਕੋ;
    ਹਾਲਤ 0x4:
        dbg.debug_armv = 0x70;
        ਰੋਕੋ;
    ਹਾਲਤ 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        ਰੋਕੋ;
    ਹਾਲਤ 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        ਰੋਕੋ;
    ਮੂਲ:
        dbg.is_ਉਪਲਬਧ = false;
        dbg.debug_armv = 0;
        ਵਾਪਸ;
    }

    dbg.is_ਉਪਲਬਧ = true;
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
BOOT_ਕੋਡ bool_t Arch_initHardwareBreakpoints(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t dbgosdlr, dbgoslsr;

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
    memset(&dbg, 0, ਆਕਾਰ(dbg));

    init_ਸੰਸਕਰਣ_Info();
    ਜੇ (dbg.is_ਉਪਲਬਧ == false) {
        ਲਿਖੋ("Debug architecture not implemented.\n");
        ਵਾਪਸ false;
    }

    ਲਿਖੋ("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_ਸੰਸਕਰਣ, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    ਜੇ (dbg.debug_armv > 0x61) {
        ਜੇ (dbg.coprocessor_is_baseline_only) {
            ਲਿਖੋ("ARMDBG: No reliable access to DBG regs.\n");
            ਵਾਪਸ dbg.is_ਉਪਲਬਧ = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_ਵਿੱਚ_secure_ਬਹੁਲਕ = !(readDscrCp() & DBGDSCR_SECURE_ਬਹੁਲਕ_DISABLED);
        ਜੇ (dbg.cpu_is_ਵਿੱਚ_secure_ਬਹੁਲਕ) {
            ਸ਼ਬਦ_t sder;

            ਲਿਖੋ("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        ਜੇ (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_ਤਾਲਾ_ENABLE);
        } ਨਹੀਂਤਾਂ ਜੇ (dbg.debug_armv == 0x70) {
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
        ਜੇ (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_ਤਾਲਾ_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_ਤਾਲਾ_ਮੁੱਲ);
        }

        disableAllBpsAndWps();
        ਜੇ (!enable_Monitor_ਬਹੁਲਕ()) {
            ਵਾਪਸ dbg.is_ਉਪਲਬਧ = false;
        }
    } ਨਹੀਂਤਾਂ {
        /* On v6 you have to enable monitor mode first. */
        ਜੇ (!enable_Monitor_ਬਹੁਲਕ()) {
            ਵਾਪਸ dbg.is_ਉਪਲਬਧ = false;
        }
        disableAllBpsAndWps();
    }

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

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    ਵਾਪਸ true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ fault_ਸਥਿਤੀ {
    uint8_t ਸਥਿਤੀ;
    bool_t is_long_desc_format;
} fault_ਸਥਿਤੀ_t;

ਸਥਿਰ fault_ਸਥਿਤੀ_t get_Fault_ਸਥਿਤੀ(ਸ਼ਬਦ_t hsr_or_fsr)
{
    fault_ਸਥਿਤੀ_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.ਸਥਿਤੀ = hsr_or_fsr & 0x3F;
#ਨਹੀਂਤਾਂ
    /* Non-hyp uses IFSR/DFSR */
    ਜੇ (hsr_or_fsr & ਦੋਆਧਾਰੀ_ਅੰਕ(FSR_LPAE_ਖਿਸਕਾਓ)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.ਸਥਿਤੀ = hsr_or_fsr & 0x3F;
    } ਨਹੀਂਤਾਂ {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.ਸਥਿਤੀ = (hsr_or_fsr & ਦੋਆਧਾਰੀ_ਅੰਕ(FSR_ਸਥਿਤੀ_BIT4_ਖਿਸਕਾਓ)) >> FSR_ਸਥਿਤੀ_BIT4_ਖਿਸਕਾਓ;
        ret.ਸਥਿਤੀ <<= 4;
        ret.ਸਥਿਤੀ = hsr_or_fsr & 0xF;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ret;
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
bool_t isDebugFault(ਸ਼ਬਦ_t hsr_or_fsr)
{
    fault_ਸਥਿਤੀ_t fs;

    fs = get_Fault_ਸਥਿਤੀ(hsr_or_fsr);
    ਜੇ (fs.is_long_desc_format) {
        ਜੇ (fs.ਸਥਿਤੀ == FSR_LONGDESC_ਸਥਿਤੀ_DEBUG_EVENT) {
            ਵਾਪਸ true;
        }
    } ਨਹੀਂਤਾਂ {
        ਜੇ (fs.ਸਥਿਤੀ == FSR_SHORTDESC_ਸਥਿਤੀ_DEBUG_EVENT) {
            ਵਾਪਸ true;
        }
    }

    ਜੇ (get_ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ() == DEBUG_ਇੰਦਰਾਜ_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        ਵਾਪਸ true;
    }
    ਵਾਪਸ false;
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
seL4_Fault_t handleUserLevelDebugException(ਸ਼ਬਦ_t fault_vaddr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_KERNEL_ENTRIES
    ks_Kernel_ਇੰਦਰਾਜ.ਰਸਤਾ = ਇੰਦਰਾਜ_Debug_Fault;
    ks_Kernel_ਇੰਦਰਾਜ.ਸ਼ਬਦ_2 = fault_vaddr;
#ਜੇ_ਅੰਤ

    ਸ਼ਬਦ_t ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ = get_ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ();
    ਪੂਰਨ ਸਰਗਰਮ_bp;
    seL4_Fault_t ret;
    ਸ਼ਬਦ_t bp_reason, bp_vaddr;

    ਚੋਣ (ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ) {
    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_BREAKPOINT:
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
        tcb_t *curr_thread = ਗੰਢ_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        ਜੇ (context->breakpointState.single_ਕਦਮ_enabled) {
            ਸ਼ਬਦ_t bvr;
            ਸ਼ਬਦ_t bp_ਅੰਕ = context->breakpointState.single_ਕਦਮ_hw_bp_ਅੰਕ;

            bvr = readBvrCp(bp_ਅੰਕ);
            ਜੇ (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_ਕਦਮ;
                ਸਰਗਰਮ_bp = bp_ਅੰਕ;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, ਸਰਗਰਮ_bp, context->registers[FaultIP]);
            }
        }
        ਰੋਕੋ;
    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_SYNC_WATCHPOINT:
        bp_reason = se_L4_ਅੰਕੜੇ_Breakpoint;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#ਨਹੀਂਤਾਂ
        bp_vaddr = getFAR();
#ਜੇ_ਅੰਤ
        ਰੋਕੋ;

    ਹਾਲਤ DEBUG_ਇੰਦਰਾਜ_ASYNC_WATCHPOINT:
        bp_reason = se_L4_ਅੰਕੜੇ_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        ਰੋਕੋ;

    ਮੂਲ: /* EXPLICIT_BKPT: BKPT instruction */
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ == DEBUG_ਇੰਦਰਾਜ_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        ਸਰਗਰਮ_bp = 0;
    }

    ਜੇ (ਵਿਧੀ_ਦਾ_ਇੰਦਰਾਜ != DEBUG_ਇੰਦਰਾਜ_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_ਕਦਮ) {
        ਸਰਗਰਮ_bp = get_And_Reset_ਸਰਗਰਮ_Breakpoint(bp_vaddr,
                                                bp_reason);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਸਰਗਰਮ_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    ਜੇ (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        ਸਰਗਰਮ_bp = get_Bp_ਅੰਕ_From_Type(ਸਰਗਰਮ_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, ਸਰਗਰਮ_bp, bp_reason);
    ਵਾਪਸ ret;
}

#ਜੇ_ਅੰਤ /* CONFIG_HARDWARE_DEBUG_API */
