/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API

#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/debug_conf.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <armv/debug.h>
#ಅನ್ತರ್ಭಾವ <mode/machine/debug.h>
#ಅನ್ತರ್ಭಾವ <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSCR_MDBGEN                (ಬಿಟ್(15))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSCR_HDBGEN                (ಬಿಟ್(14))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSCR_USER_ACCESS_DISABLE   (ಬಿಟ್(12))

/* This bit is always RAO */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGLSR_ಬೀಗ_IMPLEMENTED       (ಬಿಟ್(0))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGLSR_ಬೀಗ_ENABLED           (ಬಿಟ್(1))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGLAR_UNLOCK_ಮೌಲ್ಯ           (0xC5ACCE55u)

#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLAR_ಬೀಗ_ಮೌಲ್ಯ           (0xC5ACCE55u)

#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLSR_GET_OSLOCK_MODEL(v)  ((((v) >> 2u) & 0x2u) | ((v) & 0x1u))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLSR_ಬೀಗ_MODEL_NO_OSLOCK (0u)
#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLSR_ಬೀಗ_MODEL_OSLOCK_AND_OSSR (1u)
#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLSR_ಬೀಗ_MODEL_OSLOCK_ONLY (2u)

#ವ್ಯಾಖ್ಯಾನಿಸು DBGPRSR_OSLOCK                    (ಬಿಟ್(5))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGPRSR_OS_DLOCK                  (ಬಿಟ್(6))

#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSDLR_ಬೀಗ_ENABLE              (ಬಿಟ್(0))

#ವ್ಯಾಖ್ಯಾನಿಸು DBGAUTHSTATUS_NSI_IMPLEMENTED (ಬಿಟ್(1))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGAUTHSTATUS_NSI_ENABLED     (ಬಿಟ್(0))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGAUTHSTATUS_SI_IMPLEMENTED (ಬಿಟ್(5))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGAUTHSTATUS_SI_ENABLED     (ಬಿಟ್(4))

#ವ್ಯಾಖ್ಯಾನಿಸು DBGDRAR_VALID                (ಮುಖವಾಡ(2))
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSAR_VALID                (ಮುಖವಾಡ(2))

#ವ್ಯಾಖ್ಯಾನಿಸು DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG       (ಬಿಟ್(0))

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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ debug_state {
    bool_t is_ಲಭ್ಯ, coprocessor_is_baseline_only, watchpoint_8b_supported,
           non_secure_invasive_unavailable, secure_invasive_unavailable,
           cpu_is_ಒಳ_secure_ಬಹುಲಕ, single_ಹೆಜ್ಜೆ_supported, breakpoints_supported,
           watchpoints_supported;
    uint8_t debug_armv;
    uint8_t didr_ಆವೃತ್ತಿ, oem_variant, oem_revision;
} debug_state_t;
ಸ್ಥಿರ debug_state_t dbg;

bool_t byte8WatchpointsSupported(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ dbg.watchpoint_8b_supported;
}

#ವ್ಯಾಖ್ಯಾನಿಸು SCR "p15, 0, %0, c1, c1, 0"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDIDR "p14,0,%0,c0,c0,0"
/* Not guaranteed in v7, only v7.1+ */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDRCR ""
#ವ್ಯಾಖ್ಯಾನಿಸು DBGVCR "p15, 0, %0, c0, c7, 0"

#ವ್ಯಾಖ್ಯಾನಿಸು DBGDRAR_32 "p14,0,%0,c1,c0,0"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDRAR_64 "p14,0,%Q0,%R0,c1"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSAR_32 "p14,0,%0,c2,c0,0"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDSAR_64 "p14,0,%Q0,%R0,c2"

/* ARMv7 manual C11.11.41:
 * "This register is required in all implementations."
 * "In v7.1 DBGPRSR is not visible in the CP14 interface."
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGPRSR "p14, 0, %0, c1, c5, 4"

#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLAR "p14,0,%0,c1,c0,4"
/* ARMv7 manual: C11.11.32:
 * "In any implementation, software can read this register to detect whether
 * the OS Save and Restore mechanism is implemented. If it is not implemented
 * the read of DBGOSLSR.OSLM returns zero."
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSLSR "p14,0,%0,c1,c1,4"

/* ARMv7 manual: C11.11.30:
 * "This register is only visible in the CP14 interface."
 * "In v7 Debug, this register is not implemented."
 * "In v7.1 Debug, this register is required in all implementations."
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGOSDLR "p14, 0, %0, c1, c3, 4"

#ವ್ಯಾಖ್ಯಾನಿಸು DBGDEVID2 "p14,0,%0,c7,c0,7"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDEVID1 "p14,0,%0,c7,c1,7"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDEVID "p14,0,%0,c7,c2,7"
#ವ್ಯಾಖ್ಯಾನಿಸು DBGDEVTYPE ""

/* ARMv7 manual: C11.11.1: DBGAUTHSTATUS:
    * "This register is required in all implementations."
 * However, in v7, it is only visible in the memory mapped interface.
 * However, in the v6 manual, this register is not mentioned at all and doesn't
 * exist.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DBGAUTHSTATUS "p14,0,%0,c7,c14,6"

/** Extracts the "Method of Entry" bits from DBGDSCR.
 *
 * Used to determine what type of debug exception has occurred.
 */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t get_ವಿಧಾನ_ದ_ನಮೂದು(ಶೂನ್ಯ)
{
    dbg_dscr_t dscr;

    dscr.ಪದಗಳು[0] = readDscrCp();
    ಹಿಂತಿರುಗಿ dbg_dscr_get_ವಿಧಾನ_ದ_ನಮೂದು(dscr);
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t,
                               uint16_t bp_ಅಂಕೆ,
                               ಶಬ್ದ_t n_instr,
                               bool_t is_ಉತ್ತರ)
{

    ಆದರೆ (is_ಉತ್ತರ) {
        bp_ಅಂಕೆ = t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_hw_bp_ಅಂಕೆ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        bp_ಅಂಕೆ = convert_Bp_ಅಂಕೆ_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ);
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

    bcr.ಪದಗಳು[0] = readBcrContext(t, bp_ಅಂಕೆ);

    /* If the user calls us with n_instr == 0, allow them to configure, but
     * leave it disabled.
     */
    ಆದರೆ (n_instr > 0) {
        bcr = dbg_bcr_ಗಣ_enabled(bcr, 1);
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = true;
    } ಇಲ್ಲದಿದ್ದರೆ {
        bcr = dbg_bcr_ಗಣ_enabled(bcr, 0);
        t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled = false;
    }

    bcr = dbg_bcr_ಗಣ_lbn(bcr, 0);
    bcr = dbg_bcr_ಗಣ_pmc(bcr, DBGBCR_PRIV_USER);
    bcr = dbg_bcr_ಗಣ_hmc(bcr, 0);
    bcr = dbg_bcr_ಗಣ_ssc(bcr, 0);
    bcr = dbg_bcr_ಗಣ_bas(bcr, convert_ಗಾತ್ರ_ಗಮ್ಯ_Arch(1));
    bcr = Arch_ಸಿದ್ಧತೆ_Bcr(bcr, false);

    writeBvrContext(t, bp_ಅಂಕೆ, t->tcbArch.tcbContext.registers[FaultIP]);
    writeBcrContext(t, bp_ಅಂಕೆ, bcr.ಪದಗಳು[0]);

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    t->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_hw_bp_ಅಂಕೆ = bp_ಅಂಕೆ;
    ಹಿಂತಿರುಗಿ true;
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
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_ಆವೃತ್ತಿ_Info(ಶೂನ್ಯ)
{
    dbg_didr_t didr;

    didr.ಪದಗಳು[0] = getDIDR();
    dbg.oem_revision = dbg_didr_get_revision(didr);
    dbg.oem_variant = dbg_didr_get_variant(didr);
    dbg.didr_ಆವೃತ್ತಿ = dbg_didr_get_ಆವೃತ್ತಿ(didr);
    dbg.coprocessor_is_baseline_only = true;
    dbg.breakpoints_supported = dbg.watchpoints_supported =
                                    dbg.single_ಹೆಜ್ಜೆ_supported = true;

    ಆಯ್ಕೆ (dbg.didr_ಆವೃತ್ತಿ) {
    ಸ್ಥಿತಿ 0x1:
        dbg.debug_armv = 0x60;
        dbg.single_ಹೆಜ್ಜೆ_supported = false;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0x2:
        dbg.debug_armv = 0x61;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0x3:
        dbg.debug_armv = 0x70;
        dbg.coprocessor_is_baseline_only = false;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0x4:
        dbg.debug_armv = 0x70;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0x5:
        dbg.debug_armv = 0x71;
        dbg.coprocessor_is_baseline_only = false;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0x6:
        dbg.debug_armv = 0x80;
        dbg.coprocessor_is_baseline_only = false;
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        dbg.is_ಲಭ್ಯ = false;
        dbg.debug_armv = 0;
        ಹಿಂತಿರುಗಿ;
    }

    dbg.is_ಲಭ್ಯ = true;
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
BOOT_ಸಂಕೇತ bool_t Arch_initHardwareBreakpoints(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t dbgosdlr, dbgoslsr;

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
    memset(&dbg, 0, ಗಾತ್ರ(dbg));

    init_ಆವೃತ್ತಿ_Info();
    ಆದರೆ (dbg.is_ಲಭ್ಯ == false) {
        ಬರೆ("Debug architecture not implemented.\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಬರೆ("DIDRv: %x, armv %x, coproc baseline only? %s.\n",
           dbg.didr_ಆವೃತ್ತಿ, dbg.debug_armv,
           ((dbg.coprocessor_is_baseline_only) ? "yes" : "no"));

    ಆದರೆ (dbg.debug_armv > 0x61) {
        ಆದರೆ (dbg.coprocessor_is_baseline_only) {
            ಬರೆ("ARMDBG: No reliable access to DBG regs.\n");
            ಹಿಂತಿರುಗಿ dbg.is_ಲಭ್ಯ = false;
        }

        /* Interestingly, since the debug features have so many bits that
         * behave differently pending the state of secure-mode, ARM had to
         * expose a bit in the debug coprocessor that reveals whether or not the
         * CPU is in secure mode, or else it would be semi-impossible to program
         * this feature.
         */
        dbg.cpu_is_ಒಳ_secure_ಬಹುಲಕ = !(readDscrCp() & DBGDSCR_SECURE_ಬಹುಲಕ_DISABLED);
        ಆದರೆ (dbg.cpu_is_ಒಳ_secure_ಬಹುಲಕ) {
            ಶಬ್ದ_t sder;

            ಬರೆ("CPU is in secure mode. Enabling debugging in secure user mode.\n");
            MRC(DBGSDER, sder);
            MCR(DBGSDER, sder
                | DBGSDER_ENABLE_SECURE_USER_INVASIVE_DEBUG);
        }

        /* Deal with OS Double-lock: */
        ಆದರೆ (dbg.debug_armv == 0x71) {
            /* ARMv7 manuals, C11.11.30:
             * "In v7.1 Debug, this register is required in all implementations."
             */
            MRC(DBGOSDLR, dbgosdlr);
            MCR(DBGOSDLR, dbgosdlr & ~DBGOSDLR_ಬೀಗ_ENABLE);
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dbg.debug_armv == 0x70) {
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
        ಆದರೆ (DBGOSLSR_GET_OSLOCK_MODEL(dbgoslsr) != DBGOSLSR_ಬೀಗ_MODEL_NO_OSLOCK) {
            MCR(DBGOSLAR, ~DBGOSLAR_ಬೀಗ_ಮೌಲ್ಯ);
        }

        disableAllBpsAndWps();
        ಆದರೆ (!enable_Monitor_ಬಹುಲಕ()) {
            ಹಿಂತಿರುಗಿ dbg.is_ಲಭ್ಯ = false;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* On v6 you have to enable monitor mode first. */
        ಆದರೆ (!enable_Monitor_ಬಹುಲಕ()) {
            ಹಿಂತಿರುಗಿ dbg.is_ಲಭ್ಯ = false;
        }
        disableAllBpsAndWps();
    }

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

    dbg.watchpoint_8b_supported = watchpoint8bSupported();
    ಹಿಂತಿರುಗಿ true;
}

/** Abstract wrapper around the IFSR/DFSR fault status values.
 *
 * Format of the FSR bits is different for long and short descriptors, so
 * extract the FSR bits and accompany them with a boolean.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ fault_ಸ್ಥಿತಿ {
    uint8_t ಸ್ಥಿತಿ_2;
    bool_t is_long_desc_format;
} fault_ಸ್ಥಿತಿ_t;

ಸ್ಥಿರ fault_ಸ್ಥಿತಿ_t get_Fault_ಸ್ಥಿತಿ(ಶಬ್ದ_t hsr_or_fsr)
{
    fault_ಸ್ಥಿತಿ_t ret;

    /* Hyp mode uses the HSR, Hype syndrome register. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* the HSR only uses the long descriptor format. */
    ret.is_long_desc_format = true;
    /* FSR[5:0]. */
    ret.ಸ್ಥಿತಿ_2 = hsr_or_fsr & 0x3F;
#ಇಲ್ಲದಿದ್ದರೆ
    /* Non-hyp uses IFSR/DFSR */
    ಆದರೆ (hsr_or_fsr & ಬಿಟ್(FSR_LPAE_ಸರಿಸು)) {
        ret.is_long_desc_format = true;
        /* FSR[5:0] */
        ret.ಸ್ಥಿತಿ_2 = hsr_or_fsr & 0x3F;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ret.is_long_desc_format = false;
        /* FSR[10] | FSR[3:0]. */
        ret.ಸ್ಥಿತಿ_2 = (hsr_or_fsr & ಬಿಟ್(FSR_ಸ್ಥಿತಿ_BIT4_ಸರಿಸು)) >> FSR_ಸ್ಥಿತಿ_BIT4_ಸರಿಸು;
        ret.ಸ್ಥಿತಿ_2 <<= 4;
        ret.ಸ್ಥಿತಿ_2 = hsr_or_fsr & 0xF;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ret;
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
bool_t isDebugFault(ಶಬ್ದ_t hsr_or_fsr)
{
    fault_ಸ್ಥಿತಿ_t fs;

    fs = get_Fault_ಸ್ಥಿತಿ(hsr_or_fsr);
    ಆದರೆ (fs.is_long_desc_format) {
        ಆದರೆ (fs.ಸ್ಥಿತಿ_2 == FSR_LONGDESC_ಸ್ಥಿತಿ_DEBUG_EVENT) {
            ಹಿಂತಿರುಗಿ true;
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (fs.ಸ್ಥಿತಿ_2 == FSR_SHORTDESC_ಸ್ಥಿತಿ_DEBUG_EVENT) {
            ಹಿಂತಿರುಗಿ true;
        }
    }

    ಆದರೆ (get_ವಿಧಾನ_ದ_ನಮೂದು() == DEBUG_ನಮೂದು_ASYNC_WATCHPOINT) {
        userError("Debug: Watchpoint delivered as async abort.");
        ಹಿಂತಿರುಗಿ true;
    }
    ಹಿಂತಿರುಗಿ false;
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
seL4_Fault_t handleUserLevelDebugException(ಶಬ್ದ_t fault_vaddr)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.ಮಾರ್ಗ = ನಮೂದು_Debug_Fault;
    ks_Kernel_ನಮೂದು.ಶಬ್ದ = fault_vaddr;
#ಆದರೆ_ಅಂತ್ಯ

    ಶಬ್ದ_t ವಿಧಾನ_ದ_ನಮೂದು = get_ವಿಧಾನ_ದ_ನಮೂದು();
    ಪೂರ್ಣಾಂಕ ಸಕ್ರಿಯ_bp;
    seL4_Fault_t ret;
    ಶಬ್ದ_t bp_reason, bp_vaddr;

    ಆಯ್ಕೆ (ವಿಧಾನ_ದ_ನಮೂದು) {
    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_BREAKPOINT:
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
        tcb_t *curr_thread = ಗಂಟು_STATE(ksCurThread);
        user_context_t *context = &curr_thread->tcbArch.tcbContext;

        ಆದರೆ (context->breakpointState.single_ಹೆಜ್ಜೆ_enabled) {
            ಶಬ್ದ_t bvr;
            ಶಬ್ದ_t bp_ಅಂಕೆ = context->breakpointState.single_ಹೆಜ್ಜೆ_hw_bp_ಅಂಕೆ;

            bvr = readBvrCp(bp_ಅಂಕೆ);
            ಆದರೆ (bvr != context->registers[FaultIP]) {
                bp_reason = se_L4_Single_ಹೆಜ್ಜೆ;
                ಸಕ್ರಿಯ_bp = bp_ಅಂಕೆ;
                /* Update the BVR so it doesn't fault on the same address
                 * again (in the case of stepping through multiple instructions)
                 */
                writeBvrContext(curr_thread, ಸಕ್ರಿಯ_bp, context->registers[FaultIP]);
            }
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_SYNC_WATCHPOINT:
        bp_reason = se_L4_ದತ್ತಾಂಶ_Breakpoint;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /* Sync watchpoint sets the BP vaddr in HDFAR. */
        bp_vaddr = getHDFAR();
#ಇಲ್ಲದಿದ್ದರೆ
        bp_vaddr = getFAR();
#ಆದರೆ_ಅಂತ್ಯ
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ DEBUG_ನಮೂದು_ASYNC_WATCHPOINT:
        bp_reason = se_L4_ದತ್ತಾಂಶ_Breakpoint;
        /* Async WP sets the WP vaddr in DBGWFAR for both hyp and non-hyp. */
        bp_vaddr = getWFAR();
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ: /* EXPLICIT_BKPT: BKPT instruction */
        ಪ್ರತಿಜ್ಞಾ(ವಿಧಾನ_ದ_ನಮೂದು == DEBUG_ನಮೂದು_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        ಸಕ್ರಿಯ_bp = 0;
    }

    ಆದರೆ (ವಿಧಾನ_ದ_ನಮೂದು != DEBUG_ನಮೂದು_EXPLICIT_BKPT
        && bp_reason != se_L4_Single_ಹೆಜ್ಜೆ) {
        ಸಕ್ರಿಯ_bp = get_And_Reset_ಸಕ್ರಿಯ_Breakpoint(bp_vaddr,
                                                bp_reason);
        ಪ್ರತಿಜ್ಞಾ(ಸಕ್ರಿಯ_bp >= 0);
    }

    /* There is no hardware register associated with BKPT instruction
     * triggers.
     */
    ಆದರೆ (bp_reason != seL4_SoftwareBreakRequest) {
        /* Convert the hardware BP num back into an API-ID */
        ಸಕ್ರಿಯ_bp = get_Bp_ಅಂಕೆ_From_Type(ಸಕ್ರಿಯ_bp, bp_reason);
    }
    ret = seL4_Fault_DebugException_new(bp_vaddr, ಸಕ್ರಿಯ_bp, bp_reason);
    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */
