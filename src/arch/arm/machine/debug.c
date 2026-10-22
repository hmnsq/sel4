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

ಎಣಿಕೆ watchpoint_privilege /* WCR[2:1] */ {
    DBGWCR_PRIV_RESERVED = 0u,
    DBGWCR_PRIV_PRIVILEGED = 1u,
    DBGWCR_PRIV_USER = 2u,
    DBGWCR_PRIV_EITHER = 3u
};

ಎಣಿಕೆ watchpoint_access /* WCR[4:3] */ {
    DBGWCR_ACCESS_RESERVED = 0u,
    DBGWCR_ACCESS_ತುಂಬು = 1u,
    DBGWCR_ACCESS_STORE = 2u,
    DBGWCR_ACCESS_EITHER = 3u
};

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_ತಳ_CP14_SAVE_AND_RESTORE

/* These next few functions (read*Context()/write*Context()) read from TCB
 * context and not from the hardware registers.
 */
ಶಬ್ದ_t readBcrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Breakpoints);
    ಹಿಂತಿರುಗಿ t->tcbArch.tcbContext.breakpointState.breakpoint[ಸೂಚ್ಯಂಕ].cr;
}

ಸ್ಥಿರ ಶಬ್ದ_t readBvrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Breakpoints);
    ಹಿಂತಿರುಗಿ t->tcbArch.tcbContext.breakpointState.breakpoint[ಸೂಚ್ಯಂಕ].vr;
}

ಸ್ಥಿರ ಶಬ್ದ_t readWcrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Watchpoints);
    ಹಿಂತಿರುಗಿ t->tcbArch.tcbContext.breakpointState.watchpoint[ಸೂಚ್ಯಂಕ].cr;
}

ಸ್ಥಿರ ಶಬ್ದ_t readWvrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Watchpoints);
    ಹಿಂತಿರುಗಿ t->tcbArch.tcbContext.breakpointState.watchpoint[ಸೂಚ್ಯಂಕ].vr;
}

ಶೂನ್ಯ writeBcrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ, ಶಬ್ದ_t val)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[ಸೂಚ್ಯಂಕ].cr = val;
}

ಶೂನ್ಯ writeBvrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ, ಶಬ್ದ_t val)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[ಸೂಚ್ಯಂಕ].vr = val;
}

ಸ್ಥಿರ ಶೂನ್ಯ writeWcrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ, ಶಬ್ದ_t val)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[ಸೂಚ್ಯಂಕ].cr = val;
}

ಸ್ಥಿರ ಶೂನ್ಯ writeWvrContext(tcb_t *t, uint16_t ಸೂಚ್ಯಂಕ, ಶಬ್ದ_t val)
{
    ಪ್ರತಿಜ್ಞಾ(ಸೂಚ್ಯಂಕ < se_L4_ಅಂಕೆ_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[ಸೂಚ್ಯಂಕ].vr = val;
}

#ಆದರೆ_ಅಂತ್ಯ /* ARM_BASE_CP14_SAVE_AND_RESTORE */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HARDWARE_DEBUG_API

/** For debugging: prints out the debug register pair values as returned by the
 * coprocessor.
 *
 * @param nBp Number of breakpoint reg pairs to print, starting at BP #0.
 * @param nBp Number of watchpoint reg pairs to print, starting at WP #0.
 */
ಬಳಸದ ಸ್ಥಿರ ಶೂನ್ಯ dumpBpsAndWpsCp(ಪೂರ್ಣಾಂಕ nBp, ಪೂರ್ಣಾಂಕ nWp)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < nBp; i++) {
        userError("CP BP %d: Bcr %lx, Bvr %lx", i, readBcrCp(i), readBvrCp(i));
    }

    ಗಾಗಿ (i = 0; i < nWp; i++) {
        userError("CP WP %d: Wcr %lx, Wvr %lx", i, readWcrCp(i), readWvrCp(i));
    }
}

/** Print a thread's saved debug context. For debugging. This differs from
 * dumpBpsAndWpsCp in that it reads from a thread's saved register context, and
 * not from the hardware coprocessor registers.
 *
 * @param at arch_tcb_t where the thread's reg context is stored.
 * @param nBp Number of BP regs to print, beginning at BP #0.
 * @param mWp Number of WP regs to print, beginning at WP #0.
 */
ಬಳಸದ ಸ್ಥಿರ ಶೂನ್ಯ dumpBpsAndWpsContext(tcb_t *t, ಪೂರ್ಣಾಂಕ nBp, ಪೂರ್ಣಾಂಕ nWp)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < nBp; i++) {
        userError("Ctxt BP %d: Bcr %lx, Bvr %lx", i, readBcrContext(t, i), readBvrContext(t, i));
    }

    ಗಾಗಿ (i = 0; i < nWp; i++) {
        userError("Ctxt WP %d: Wcr %lx, Wvr %lx", i, readWcrContext(t, i), readWvrContext(t, i));
    }
}

/* ARM allows watchpoint trigger on load, load-exclusive, and "swap" accesses.
 * store, store-exclusive and "swap" accesses. All accesses.
 *
 * The mask defines which bits are EXCLUDED from the comparison.
 * Always program the DBGDWVR with a WORD aligned address, and use the BAS to
 * state which bits form part of the match.
 *
 * It seems the BAS works as a bitmask of bytes to select in the range.
 *
 * To detect support for the 8-bit BAS field:
 *  * If the 8-bit BAS is unsupported, then BAS[7:4] is RAZ/WI.
 *
 * When using an 8-byte watchpoint that is not dword aligned, the result is
 * undefined. You should program it as the aligned base of the range, and select
 * only the relevant bytes then.
 *
 * You cannot do sparse byte selection: you either select a single byte in the
 * BAS or you select a contiguous range. ARM has deprecated sparse byte
 * selection.
 */

/** Convert an arch specific encoded watchpoint size back into a simple integer
 * representation.
 */
ಸ್ಥಿರ ಶಬ್ದ_t convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(ಶಬ್ದ_t archsize)
{
    ಆಯ್ಕೆ (archsize) {
    ಸ್ಥಿತಿ 0x1:
        ಹಿಂತಿರುಗಿ 1;
    ಸ್ಥಿತಿ 0x3:
        ಹಿಂತಿರುಗಿ 2;
    ಸ್ಥಿತಿ 0xFF:
        ಹಿಂತಿರುಗಿ 8;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(archsize == 0xF);
        ಹಿಂತಿರುಗಿ 4;
    }
}

/** Convert an access perms API value (seL4_BreakOnRead, etc) into the register
 * encoding that matches it.
 */
ಸ್ಥಿರ ಶಬ್ದ_t convert_Access_ಗಮ್ಯ_Arch(ಶಬ್ದ_t access)
{
    ಆಯ್ಕೆ (access) {
    ಸ್ಥಿತಿ seL4_BreakOnRead:
        ಹಿಂತಿರುಗಿ DBGWCR_ACCESS_ತುಂಬು;
    ಸ್ಥಿತಿ seL4_BreakOnWrite:
        ಹಿಂತಿರುಗಿ DBGWCR_ACCESS_STORE;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(access == seL4_BreakOnReadWrite);
        ಹಿಂತಿರುಗಿ DBGWCR_ACCESS_EITHER;
    }
}

/** Convert an arch-specific register encoding back into an API access perms
 * value.
 */
ಸ್ಥಿರ ಶಬ್ದ_t convert_Arch_ಗಮ್ಯ_Access(ಶಬ್ದ_t archaccess)
{
    ಆಯ್ಕೆ (archaccess) {
    ಸ್ಥಿತಿ DBGWCR_ACCESS_ತುಂಬು:
        ಹಿಂತಿರುಗಿ seL4_BreakOnRead;
    ಸ್ಥಿತಿ DBGWCR_ACCESS_STORE:
        ಹಿಂತಿರುಗಿ seL4_BreakOnWrite;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಪ್ರತಿಜ್ಞಾ(archaccess == DBGWCR_ACCESS_EITHER);
        ಹಿಂತಿರುಗಿ seL4_BreakOnReadWrite;
    }
}

uint16_t get_Bp_ಅಂಕೆ_From_Type(uint16_t bp_ಅಂಕೆ, ಶಬ್ದ_t type)
{
    ಪ್ರತಿಜ್ಞಾ(type == seL4_InstructionBreakpoint || type == se_L4_ದತ್ತಾಂಶ_Breakpoint
           || type == se_L4_Single_ಹೆಜ್ಜೆ);

    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ seL4_InstructionBreakpoint:
    ಸ್ಥಿತಿ se_L4_Single_ಹೆಜ್ಜೆ:
        ಹಿಂತಿರುಗಿ bp_ಅಂಕೆ;
    ಪೂರ್ವನಿಯೋಜಿತ: /* seL4_DataBreakpoint: */
        ಪ್ರತಿಜ್ಞಾ(type == se_L4_ದತ್ತಾಂಶ_Breakpoint);
        ಹಿಂತಿರುಗಿ bp_ಅಂಕೆ + se_L4_ಅಂಕೆ_Exclusive_Breakpoints;
    }
}

/** Sets up the requested hardware breakpoint register.
 *
 * Acts as the backend for seL4_TCB_SetBreakpoint. Doesn't actually operate
 * on the hardware coprocessor, but just modifies the thread's debug register
 * context. The thread will pop off the updated register context when it is
 * popping its context the next time it runs.
 *
 * On ARM the hardware breakpoints are consumed by all operations, including
 * single-stepping, unlike x86, where single-stepping doesn't require the use
 * of an actual hardware breakpoint register (just uses the EFLAGS.TF bit).
 *
 * @param at arch_tcb_t that points to the register context of the thread we
 *           want to modify.
 * @param bp_num The hardware register we want to set up.
 * @params vaddr, type, size, rw: seL4 API values for seL4_TCB_SetBreakpoint.
 *         All documented in the seL4 API Manuals.
 */
ಶೂನ್ಯ ಗಣ_Breakpoint(tcb_t *t,
                   uint16_t bp_ಅಂಕೆ,
                   ಶಬ್ದ_t vaddr, ಶಬ್ದ_t type, ಶಬ್ದ_t ಗಾತ್ರ_2, ಶಬ್ದ_t rw)
{
    bp_ಅಂಕೆ = convert_Bp_ಅಂಕೆ_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ);

    /* C3.3.4: "A debugger can use either byte address selection or address range
     *  masking, if it is implemented. However, it must not attempt to use both at
     * the same time"
     *
     * "v7 Debug and v7.1 Debug deprecate any use of the DBGBCR.MASK field."
     * ^ So prefer to use DBGBCR.BAS instead. When using masking, you must set
     * BAS to all 1s, and when using BAS you must set the MASK field to all 0s.
     *
     * To detect support for BPAddrMask:
     *  * When it's unsupported: DBGBCR.MASK is always RAZ/WI, and EITHER:
     *      * DBGIDR.DEVID_tmp is RAZ
     *      * OR DBGIDR.DEVID_tmp is RAO and DBGDEVID.{CIDMask, BPAddrMask} are RAZ.
     *  * OR:
     *      * DBGDEVID.BPAddrMask indicates whether addr masking is supported.
     *      * DBGBCR.MASK is UNK/SBZP.
     *
     * Setting BAS to 0b0000 makes the cpu break on every instruction.
     * Be aware that the processor checks the MASK before the BAS.
     * You must set BAS to 0b1111 for all context match comparisons.
     */
    ಆದರೆ (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        writeBvrContext(t, bp_ಅಂಕೆ, vaddr);

        /* Preserve reserved bits. */
        bcr.ಪದಗಳು[0] = readBcrContext(t, bp_ಅಂಕೆ);
        bcr = dbg_bcr_ಗಣ_enabled(bcr, 1);
        bcr = dbg_bcr_ಗಣ_lbn(bcr, 0);
        bcr = dbg_bcr_ಗಣ_pmc(bcr, DBGBCR_PRIV_USER);
        bcr = dbg_bcr_ಗಣ_hmc(bcr, 0);
        bcr = dbg_bcr_ಗಣ_ssc(bcr, 0);
        bcr = dbg_bcr_ಗಣ_bas(bcr, convert_ಗಾತ್ರ_ಗಮ್ಯ_Arch(4));
        bcr = Arch_ಸಿದ್ಧತೆ_Bcr(bcr, true);
        writeBcrContext(t, bp_ಅಂಕೆ, bcr.ಪದಗಳು[0]);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dbg_wcr_t wcr;

        writeWvrContext(t, bp_ಅಂಕೆ, vaddr);

        /* Preserve reserved bits */
        wcr.ಪದಗಳು[0] = readWcrContext(t, bp_ಅಂಕೆ);
        wcr = dbg_wcr_ಗಣ_enabled(wcr, 1);
        wcr = dbg_wcr_ಗಣ_pac(wcr, DBGWCR_PRIV_USER);
        wcr = dbg_wcr_ಗಣ_bas(wcr, convert_ಗಾತ್ರ_ಗಮ್ಯ_Arch(ಗಾತ್ರ_2));
        wcr = dbg_wcr_ಗಣ_lsc(wcr, convert_Access_ಗಮ್ಯ_Arch(rw));
        wcr = dbg_wcr_ಗಣ_watchpoint_Type(wcr, 0);
        wcr = dbg_wcr_ಗಣ_lbn(wcr, 0);
        wcr = dbg_wcr_ಗಣ_ವಿಳಾಸ_ಮುಖವಾಡ(wcr, 0);
        wcr = dbg_wcr_ಗಣ_hmc(wcr, 0);
        wcr = dbg_wcr_ಗಣ_ssc(wcr, 0);
        writeWcrContext(t, bp_ಅಂಕೆ, wcr.ಪದಗಳು[0]);
    }
}

/** Retrieves the current configuration of a hardware breakpoint for a given
 * thread.
 *
 * Doesn't modify the configuration of that thread's breakpoints.
 *
 * @param at arch_tcb_t that holds the register context for the thread you wish
 *           to query.
 * @param bp_num Hardware breakpoint ID.
 * @return A struct describing the current configuration of the requested
 *         breakpoint.
 */
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    getBreakpoint_t ret;

    ret.type = get_Type_From_Bp_ಅಂಕೆ(bp_ಅಂಕೆ);
    bp_ಅಂಕೆ = convert_Bp_ಅಂಕೆ_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ);

    ಆದರೆ (ret.type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.ಪದಗಳು[0] = readBcrContext(t, bp_ಅಂಕೆ);
        ಆದರೆ (Arch_breakpointIsSingleStepping(t, bp_ಅಂಕೆ)) {
            ret.type = se_L4_Single_ಹೆಜ್ಜೆ;
        };
        ret.ಗಾತ್ರ_2 = 0;
        ret.rw = seL4_BreakOnRead;
        ret.vaddr = readBvrContext(t, bp_ಅಂಕೆ);
        ret.is_enabled = dbg_bcr_get_enabled(bcr);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dbg_wcr_t wcr;

        wcr.ಪದಗಳು[0] = readWcrContext(t, bp_ಅಂಕೆ);
        ret.ಗಾತ್ರ_2 = convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(dbg_wcr_get_bas(wcr));
        ret.rw = convert_Arch_ಗಮ್ಯ_Access(dbg_wcr_get_lsc(wcr));
        ret.vaddr = readWvrContext(t, bp_ಅಂಕೆ);
        ret.is_enabled = dbg_wcr_get_enabled(wcr);
    }
    ಹಿಂತಿರುಗಿ ret;
}

/** Disables and clears the configuration of a hardware breakpoint.
 *
 * @param at arch_tcb_t holding the reg context for the target thread.
 * @param bp_num The hardware breakpoint you want to disable+clear.
 */
ಶೂನ್ಯ unsetBreakpoint(tcb_t *t, uint16_t bp_ಅಂಕೆ)
{
    ಶಬ್ದ_t type;

    type = get_Type_From_Bp_ಅಂಕೆ(bp_ಅಂಕೆ);
    bp_ಅಂಕೆ = convert_Bp_ಅಂಕೆ_ಗಮ್ಯ_Arch(bp_ಅಂಕೆ);

    ಆದರೆ (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.ಪದಗಳು[0] = readBcrContext(t, bp_ಅಂಕೆ);
        bcr = dbg_bcr_ಗಣ_enabled(bcr, 0);
        writeBcrContext(t, bp_ಅಂಕೆ, bcr.ಪದಗಳು[0]);
        writeBvrContext(t, bp_ಅಂಕೆ, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        dbg_wcr_t wcr;

        wcr.ಪದಗಳು[0] = readWcrContext(t, bp_ಅಂಕೆ);
        wcr = dbg_wcr_ಗಣ_enabled(wcr, 0);
        writeWcrContext(t, bp_ಅಂಕೆ, wcr.ಪದಗಳು[0]);
        writeWvrContext(t, bp_ಅಂಕೆ, 0);
    }
}

/** Load an initial, all-disabled setup state for the registers.
 */
BOOT_ಸಂಕೇತ ಶೂನ್ಯ disableAllBpsAndWps(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
        writeBvrCp(i, 0);
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
        writeWvrCp(i, 0);
        writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
    }

    isb();
}

/** Determines which breakpoint or watchpoint register caused the debug
 * exception to be triggered.
 *
 * Checks to see which hardware breakpoint was triggered, and saves
 * the ID of that breakpoint.
 * There is no short way to do this on ARM. On x86 there is a status
 * register that tells you which watchpoint has been triggered. On ARM
 * there is no such register, so you have to manually check each to see which
 * one was triggered.
 *
 * The arguments also work a bit differently from x86 as well. On x86 the
 * 2 arguments are dummy values, while on ARM, they contain useful information.
 *
 * @param vaddr The virtual address stored in the IFSR/DFSR register, which
 *              is either the watchpoint address or breakpoint address.
 * @param reason The presumed reason for the exception, which is based on
 *               whether it was a prefetch or data abort.
 * @return Struct with a member "bp_num", which is a positive integer if we
 *         successfully detected which debug register triggered the exception.
 *         "Bp_num" will be negative otherwise.
 */
ಪೂರ್ಣಾಂಕ get_And_Reset_ಸಕ್ರಿಯ_Breakpoint(ಶಬ್ದ_t vaddr, ಶಬ್ದ_t reason)
{
    ಶಬ್ದ_t align_ಮುಖವಾಡ;
    ಪೂರ್ಣಾಂಕ i, ret = -1;

    ಆದರೆ (reason == seL4_InstructionBreakpoint) {
        ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
            dbg_bcr_t bcr;
            ಶಬ್ದ_t bvr = readBvrCp(i);

            bcr.ಪದಗಳು[0] = readBcrCp(i);
            /* The actual trigger address may be an unaligned sub-byte of the
             * range, which means it's not guaranteed to match the aligned value
             * that was programmed into the address register.
             */
            align_ಮುಖವಾಡ = convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(dbg_bcr_get_bas(bcr));
            align_ಮುಖವಾಡ = ~(align_ಮುಖವಾಡ - 1);

            ಆದರೆ (bvr != (vaddr & align_ಮುಖವಾಡ) || !dbg_bcr_get_enabled(bcr)) {
                ಮುಂದುವರಿ;
            }

            ret = i;
            ಹಿಂತಿರುಗಿ ret;
        }
    }

    ಆದರೆ (reason == se_L4_ದತ್ತಾಂಶ_Breakpoint) {
        ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
            dbg_wcr_t wcr;
            ಶಬ್ದ_t wvr = readWvrCp(i);

            wcr.ಪದಗಳು[0] = readWcrCp(i);
            align_ಮುಖವಾಡ = convert_Arch_ಗಮ್ಯ_ಗಾತ್ರ(dbg_wcr_get_bas(wcr));
            align_ಮುಖವಾಡ = ~(align_ಮುಖವಾಡ - 1);

            ಆದರೆ (wvr != (vaddr & align_ಮುಖವಾಡ) || !dbg_wcr_get_enabled(wcr)) {
                ಮುಂದುವರಿ;
            }

            ret = i;
            ಹಿಂತಿರುಗಿ ret;
        }
    }

    ಹಿಂತಿರುಗಿ ret;
}

#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_HARDWARE_DEBUG_API */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_ತಳ_CP14_SAVE_AND_RESTORE

/** Mirrors Arch_initFpuContext.
 *
 * Zeroes out the BVR thread context and preloads reserved bit values from the
 * control regs into the thread context so we can operate solely on the values
 * cached in RAM in API calls, rather than retrieving the values from the
 * coprocessor.
 */
ಶೂನ್ಯ Arch_initBreakpointContext(user_context_t *uc)
{
    uc->breakpointState = armKSNullBreakpointState;
}

ಶೂನ್ಯ ತುಂಬು_All_Disabled_Breakpoint_State(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;

    /* We basically just want to read-modify-write each reg to ensure its
     * "ENABLE" bit is clear. We did preload the register context with the
     * reserved values from the control registers, so we can read our
     * initial values from either the coprocessor or the thread's register
     * context.
     *
     * Both are perfectly fine, and the only discriminant factor is performance.
     * I suspect that reading from RAM is faster than reading from the
     * coprocessor, but I can't be sure.
     */
    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
        writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
    }
}

/* We only need to save the breakpoint state in the hypervisor
 * build, and only for threads that have an associated VCPU.
 *
 * When the normal kernel is running with the debug API, all
 * changes to the debug regs are done through the debug API.
 * In the hypervisor build, the guest VM has full access to the
 * debug regs in PL1, so we need to save its values on vmexit.
 *
 * When saving the debug regs we will always save all of them.
 * When restoring, we will restore only those that have been used
 * for native threads; and we will restore all of them
 * unconditionally for VCPUs (because we don't know which of
 * them have been changed by the guest).
 *
 * To ensure that all the debug regs are restored unconditionally,
 * we just set the "used_breakpoints_bf" bitfield to all 1s in
 * associateVcpu.
 */
ಶೂನ್ಯ saveAllBreakpointState(tcb_t *t)
{
    ಪೂರ್ಣಾಂಕ i;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);

    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
        writeBvrContext(t, i, readBvrCp(i));
        writeBcrContext(t, i, readBcrCp(i));
    }

    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
        writeWvrContext(t, i, readWvrCp(i));
        writeWcrContext(t, i, readWcrCp(i));
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_ಎಳೆಗಳು
ಶೂನ್ಯ Arch_debugAssociateVCPUTCB(tcb_t *t)
{
    /* Don't attempt to shift beyond end of word. */
    ಪ್ರತಿಜ್ಞಾ(se_L4_ಅಂಕೆ_HWBreakpoints < ಗಾತ್ರ(ಶಬ್ದ_t) * 8);

    /* Set all the bits to 1, so loadBreakpointState() will
     * restore all the debug regs unconditionally.
     */
    t->tcbArch.tcbContext.breakpointState.ಬಳಸಿದ_breakpoints_bf = ಮುಖವಾಡ(se_L4_ಅಂಕೆ_HWBreakpoints);
}

ಶೂನ್ಯ Arch_debugDissociateVCPUTCB(tcb_t *t)
{
    t->tcbArch.tcbContext.breakpointState.ಬಳಸಿದ_breakpoints_bf = 0;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ ತುಂಬು_Breakpoint_State(tcb_t *t)
{
    ಪೂರ್ಣಾಂಕ i;

    ಪ್ರತಿಜ್ಞಾ(t != ಖಾಲಿ);

    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Breakpoints; i++) {
        ಆದರೆ (t->tcbArch.tcbContext.breakpointState.ಬಳಸಿದ_breakpoints_bf & ಬಿಟ್(i)) {
            writeBvrCp(i, readBvrContext(t, i));
            writeBcrCp(i, readBcrContext(t, i));
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* If the thread isn't using the BP, then just load
             * a default "disabled" state.
             */
            writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
        }
    }

    ಗಾಗಿ (i = 0; i < se_L4_ಅಂಕೆ_Exclusive_Watchpoints; i++) {
        ಆದರೆ (t->tcbArch.tcbContext.breakpointState.ಬಳಸಿದ_breakpoints_bf &
            ಬಿಟ್(i + se_L4_ಅಂಕೆ_Exclusive_Breakpoints)) {
            writeWvrCp(i, readWvrContext(t, i));
            writeWcrCp(i, readWcrContext(t, i));
        } ಇಲ್ಲದಿದ್ದರೆ {
            writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
        }
    }
}

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
ಶೂನ್ಯ restore_user_debug_context(tcb_t *ಗುರಿ_thread)
{
    ಪ್ರತಿಜ್ಞಾ(ಗುರಿ_thread != ಖಾಲಿ);

    ಆದರೆ (ಗುರಿ_thread->tcbArch.tcbContext.breakpointState.ಬಳಸಿದ_breakpoints_bf == 0) {
        ತುಂಬು_All_Disabled_Breakpoint_State();
    } ಇಲ್ಲದಿದ್ದರೆ {
        ತುಂಬು_Breakpoint_State(ಗುರಿ_thread);
    }

    /* ARMv7 manual, sec C3.7:
     * "Usually, an exception return sequence is a context change operation as
     * well as a context synchronization operation, in which case the context
     * change operation is guaranteed to take effect on the debug logic by the
     * end of that exception return sequence."
     *
     * So we don't need to execute ISB here because we're about to RFE.
     */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    aarch64_restore_user_debug_context(ಗುರಿ_thread);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_ARCH64 */
}

#ಆದರೆ_ಅಂತ್ಯ /* ARM_BASE_CP14_SAVE_AND_RESTORE */
