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

గణన watchpoint_privilege /* WCR[2:1] */ {
    DBGWCR_PRIV_RESERVED = 0u,
    DBGWCR_PRIV_PRIVILEGED = 1u,
    DBGWCR_PRIV_USER = 2u,
    DBGWCR_PRIV_EITHER = 3u
};

గణన watchpoint_access /* WCR[4:3] */ {
    DBGWCR_ACCESS_RESERVED = 0u,
    DBGWCR_ACCESS_ఎక్కించు = 1u,
    DBGWCR_ACCESS_STORE = 2u,
    DBGWCR_ACCESS_EITHER = 3u
};

#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */

#నిర్వచితమైతే ARM_పునాది_CP14_SAVE_AND_RESTORE

/* These next few functions (read*Context()/write*Context()) read from TCB
 * context and not from the hardware registers.
 */
మాట_t readBcrContext(tcb_t *t, uint16_t సూచీ)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Breakpoints);
    తిరిగి t->tcbArch.tcbContext.breakpointState.breakpoint[సూచీ].cr;
}

స్థిర మాట_t readBvrContext(tcb_t *t, uint16_t సూచీ)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Breakpoints);
    తిరిగి t->tcbArch.tcbContext.breakpointState.breakpoint[సూచీ].vr;
}

స్థిర మాట_t readWcrContext(tcb_t *t, uint16_t సూచీ)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Watchpoints);
    తిరిగి t->tcbArch.tcbContext.breakpointState.watchpoint[సూచీ].cr;
}

స్థిర మాట_t readWvrContext(tcb_t *t, uint16_t సూచీ)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Watchpoints);
    తిరిగి t->tcbArch.tcbContext.breakpointState.watchpoint[సూచీ].vr;
}

శూన్య writeBcrContext(tcb_t *t, uint16_t సూచీ, మాట_t val)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[సూచీ].cr = val;
}

శూన్య writeBvrContext(tcb_t *t, uint16_t సూచీ, మాట_t val)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[సూచీ].vr = val;
}

స్థిర శూన్య writeWcrContext(tcb_t *t, uint16_t సూచీ, మాట_t val)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[సూచీ].cr = val;
}

స్థిర శూన్య writeWvrContext(tcb_t *t, uint16_t సూచీ, మాట_t val)
{
    ప్రతిజ్ఞా(సూచీ < se_L4_అంకె_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[సూచీ].vr = val;
}

#అయితే_ముగింపు /* ARM_BASE_CP14_SAVE_AND_RESTORE */

#నిర్వచితమైతే CONFIG_HARDWARE_DEBUG_API

/** For debugging: prints out the debug register pair values as returned by the
 * coprocessor.
 *
 * @param nBp Number of breakpoint reg pairs to print, starting at BP #0.
 * @param nBp Number of watchpoint reg pairs to print, starting at WP #0.
 */
వాడని స్థిర శూన్య dumpBpsAndWpsCp(పూర్ణాంకం nBp, పూర్ణాంకం nWp)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < nBp; i++) {
        userError("CP BP %d: Bcr %lx, Bvr %lx", i, readBcrCp(i), readBvrCp(i));
    }

    కొరకు (i = 0; i < nWp; i++) {
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
వాడని స్థిర శూన్య dumpBpsAndWpsContext(tcb_t *t, పూర్ణాంకం nBp, పూర్ణాంకం nWp)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < nBp; i++) {
        userError("Ctxt BP %d: Bcr %lx, Bvr %lx", i, readBcrContext(t, i), readBvrContext(t, i));
    }

    కొరకు (i = 0; i < nWp; i++) {
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
స్థిర మాట_t convert_Arch_గమ్యం_కొలత(మాట_t archsize)
{
    ఎంపిక (archsize) {
    స్థితి 0x1:
        తిరిగి 1;
    స్థితి 0x3:
        తిరిగి 2;
    స్థితి 0xFF:
        తిరిగి 8;
    అప్రమేయ:
        ప్రతిజ్ఞా(archsize == 0xF);
        తిరిగి 4;
    }
}

/** Convert an access perms API value (seL4_BreakOnRead, etc) into the register
 * encoding that matches it.
 */
స్థిర మాట_t convert_Access_గమ్యం_Arch(మాట_t access)
{
    ఎంపిక (access) {
    స్థితి seL4_BreakOnRead:
        తిరిగి DBGWCR_ACCESS_ఎక్కించు;
    స్థితి seL4_BreakOnWrite:
        తిరిగి DBGWCR_ACCESS_STORE;
    అప్రమేయ:
        ప్రతిజ్ఞా(access == seL4_BreakOnReadWrite);
        తిరిగి DBGWCR_ACCESS_EITHER;
    }
}

/** Convert an arch-specific register encoding back into an API access perms
 * value.
 */
స్థిర మాట_t convert_Arch_గమ్యం_Access(మాట_t archaccess)
{
    ఎంపిక (archaccess) {
    స్థితి DBGWCR_ACCESS_ఎక్కించు:
        తిరిగి seL4_BreakOnRead;
    స్థితి DBGWCR_ACCESS_STORE:
        తిరిగి seL4_BreakOnWrite;
    అప్రమేయ:
        ప్రతిజ్ఞా(archaccess == DBGWCR_ACCESS_EITHER);
        తిరిగి seL4_BreakOnReadWrite;
    }
}

uint16_t get_Bp_అంకె_From_Type(uint16_t bp_అంకె, మాట_t type)
{
    ప్రతిజ్ఞా(type == seL4_InstructionBreakpoint || type == se_L4_దత్తాంశం_Breakpoint
           || type == se_L4_Single_అడుగు);

    ఎంపిక (type) {
    స్థితి seL4_InstructionBreakpoint:
    స్థితి se_L4_Single_అడుగు:
        తిరిగి bp_అంకె;
    అప్రమేయ: /* seL4_DataBreakpoint: */
        ప్రతిజ్ఞా(type == se_L4_దత్తాంశం_Breakpoint);
        తిరిగి bp_అంకె + se_L4_అంకె_Exclusive_Breakpoints;
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
శూన్య సమితి_Breakpoint(tcb_t *t,
                   uint16_t bp_అంకె,
                   మాట_t vaddr, మాట_t type, మాట_t కొలత, మాట_t rw)
{
    bp_అంకె = convert_Bp_అంకె_గమ్యం_Arch(bp_అంకె);

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
    ఒకవేళ (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        writeBvrContext(t, bp_అంకె, vaddr);

        /* Preserve reserved bits. */
        bcr.పదాలు[0] = readBcrContext(t, bp_అంకె);
        bcr = dbg_bcr_సమితి_enabled(bcr, 1);
        bcr = dbg_bcr_సమితి_lbn(bcr, 0);
        bcr = dbg_bcr_సమితి_pmc(bcr, DBGBCR_PRIV_USER);
        bcr = dbg_bcr_సమితి_hmc(bcr, 0);
        bcr = dbg_bcr_సమితి_ssc(bcr, 0);
        bcr = dbg_bcr_సమితి_bas(bcr, convert_కొలత_గమ్యం_Arch(4));
        bcr = Arch_అమరిక_Bcr(bcr, true);
        writeBcrContext(t, bp_అంకె, bcr.పదాలు[0]);
    } లేకపోతే {
        dbg_wcr_t wcr;

        writeWvrContext(t, bp_అంకె, vaddr);

        /* Preserve reserved bits */
        wcr.పదాలు[0] = readWcrContext(t, bp_అంకె);
        wcr = dbg_wcr_సమితి_enabled(wcr, 1);
        wcr = dbg_wcr_సమితి_pac(wcr, DBGWCR_PRIV_USER);
        wcr = dbg_wcr_సమితి_bas(wcr, convert_కొలత_గమ్యం_Arch(కొలత));
        wcr = dbg_wcr_సమితి_lsc(wcr, convert_Access_గమ్యం_Arch(rw));
        wcr = dbg_wcr_సమితి_watchpoint_Type(wcr, 0);
        wcr = dbg_wcr_సమితి_lbn(wcr, 0);
        wcr = dbg_wcr_సమితి_చిరునామా_ముసుగు(wcr, 0);
        wcr = dbg_wcr_సమితి_hmc(wcr, 0);
        wcr = dbg_wcr_సమితి_ssc(wcr, 0);
        writeWcrContext(t, bp_అంకె, wcr.పదాలు[0]);
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    getBreakpoint_t ret;

    ret.type = get_Type_From_Bp_అంకె(bp_అంకె);
    bp_అంకె = convert_Bp_అంకె_గమ్యం_Arch(bp_అంకె);

    ఒకవేళ (ret.type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.పదాలు[0] = readBcrContext(t, bp_అంకె);
        ఒకవేళ (Arch_breakpointIsSingleStepping(t, bp_అంకె)) {
            ret.type = se_L4_Single_అడుగు;
        };
        ret.కొలత = 0;
        ret.rw = seL4_BreakOnRead;
        ret.vaddr = readBvrContext(t, bp_అంకె);
        ret.is_enabled = dbg_bcr_get_enabled(bcr);
    } లేకపోతే {
        dbg_wcr_t wcr;

        wcr.పదాలు[0] = readWcrContext(t, bp_అంకె);
        ret.కొలత = convert_Arch_గమ్యం_కొలత(dbg_wcr_get_bas(wcr));
        ret.rw = convert_Arch_గమ్యం_Access(dbg_wcr_get_lsc(wcr));
        ret.vaddr = readWvrContext(t, bp_అంకె);
        ret.is_enabled = dbg_wcr_get_enabled(wcr);
    }
    తిరిగి ret;
}

/** Disables and clears the configuration of a hardware breakpoint.
 *
 * @param at arch_tcb_t holding the reg context for the target thread.
 * @param bp_num The hardware breakpoint you want to disable+clear.
 */
శూన్య unsetBreakpoint(tcb_t *t, uint16_t bp_అంకె)
{
    మాట_t type;

    type = get_Type_From_Bp_అంకె(bp_అంకె);
    bp_అంకె = convert_Bp_అంకె_గమ్యం_Arch(bp_అంకె);

    ఒకవేళ (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.పదాలు[0] = readBcrContext(t, bp_అంకె);
        bcr = dbg_bcr_సమితి_enabled(bcr, 0);
        writeBcrContext(t, bp_అంకె, bcr.పదాలు[0]);
        writeBvrContext(t, bp_అంకె, 0);
    } లేకపోతే {
        dbg_wcr_t wcr;

        wcr.పదాలు[0] = readWcrContext(t, bp_అంకె);
        wcr = dbg_wcr_సమితి_enabled(wcr, 0);
        writeWcrContext(t, bp_అంకె, wcr.పదాలు[0]);
        writeWvrContext(t, bp_అంకె, 0);
    }
}

/** Load an initial, all-disabled setup state for the registers.
 */
BOOT_సంకేతం శూన్య disableAllBpsAndWps(శూన్య)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
        writeBvrCp(i, 0);
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
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
పూర్ణాంకం get_And_Reset_చురుకైన_Breakpoint(మాట_t vaddr, మాట_t reason)
{
    మాట_t align_ముసుగు;
    పూర్ణాంకం i, ret = -1;

    ఒకవేళ (reason == seL4_InstructionBreakpoint) {
        కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
            dbg_bcr_t bcr;
            మాట_t bvr = readBvrCp(i);

            bcr.పదాలు[0] = readBcrCp(i);
            /* The actual trigger address may be an unaligned sub-byte of the
             * range, which means it's not guaranteed to match the aligned value
             * that was programmed into the address register.
             */
            align_ముసుగు = convert_Arch_గమ్యం_కొలత(dbg_bcr_get_bas(bcr));
            align_ముసుగు = ~(align_ముసుగు - 1);

            ఒకవేళ (bvr != (vaddr & align_ముసుగు) || !dbg_bcr_get_enabled(bcr)) {
                కొనసాగు;
            }

            ret = i;
            తిరిగి ret;
        }
    }

    ఒకవేళ (reason == se_L4_దత్తాంశం_Breakpoint) {
        కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
            dbg_wcr_t wcr;
            మాట_t wvr = readWvrCp(i);

            wcr.పదాలు[0] = readWcrCp(i);
            align_ముసుగు = convert_Arch_గమ్యం_కొలత(dbg_wcr_get_bas(wcr));
            align_ముసుగు = ~(align_ముసుగు - 1);

            ఒకవేళ (wvr != (vaddr & align_ముసుగు) || !dbg_wcr_get_enabled(wcr)) {
                కొనసాగు;
            }

            ret = i;
            తిరిగి ret;
        }
    }

    తిరిగి ret;
}

#అయితే_ముగింపు /* CONFIG_HARDWARE_DEBUG_API */

#నిర్వచితమైతే ARM_పునాది_CP14_SAVE_AND_RESTORE

/** Mirrors Arch_initFpuContext.
 *
 * Zeroes out the BVR thread context and preloads reserved bit values from the
 * control regs into the thread context so we can operate solely on the values
 * cached in RAM in API calls, rather than retrieving the values from the
 * coprocessor.
 */
శూన్య Arch_initBreakpointContext(user_context_t *uc)
{
    uc->breakpointState = armKSNullBreakpointState;
}

శూన్య ఎక్కించు_All_Disabled_Breakpoint_State(శూన్య)
{
    పూర్ణాంకం i;

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
    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
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
శూన్య saveAllBreakpointState(tcb_t *t)
{
    పూర్ణాంకం i;

    ప్రతిజ్ఞా(t != ఖాళీ);

    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
        writeBvrContext(t, i, readBvrCp(i));
        writeBcrContext(t, i, readBcrCp(i));
    }

    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
        writeWvrContext(t, i, readWvrCp(i));
        writeWcrContext(t, i, readWcrCp(i));
    }
}

#నిర్వచితమైతే ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_దారాలు
శూన్య Arch_debugAssociateVCPUTCB(tcb_t *t)
{
    /* Don't attempt to shift beyond end of word. */
    ప్రతిజ్ఞా(se_L4_అంకె_HWBreakpoints < పరిమాణం(మాట_t) * 8);

    /* Set all the bits to 1, so loadBreakpointState() will
     * restore all the debug regs unconditionally.
     */
    t->tcbArch.tcbContext.breakpointState.వాడిన_breakpoints_bf = ముసుగు(se_L4_అంకె_HWBreakpoints);
}

శూన్య Arch_debugDissociateVCPUTCB(tcb_t *t)
{
    t->tcbArch.tcbContext.breakpointState.వాడిన_breakpoints_bf = 0;
}
#అయితే_ముగింపు

స్థిర శూన్య ఎక్కించు_Breakpoint_State(tcb_t *t)
{
    పూర్ణాంకం i;

    ప్రతిజ్ఞా(t != ఖాళీ);

    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Breakpoints; i++) {
        ఒకవేళ (t->tcbArch.tcbContext.breakpointState.వాడిన_breakpoints_bf & బిట్టు(i)) {
            writeBvrCp(i, readBvrContext(t, i));
            writeBcrCp(i, readBcrContext(t, i));
        } లేకపోతే {
            /* If the thread isn't using the BP, then just load
             * a default "disabled" state.
             */
            writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
        }
    }

    కొరకు (i = 0; i < se_L4_అంకె_Exclusive_Watchpoints; i++) {
        ఒకవేళ (t->tcbArch.tcbContext.breakpointState.వాడిన_breakpoints_bf &
            బిట్టు(i + se_L4_అంకె_Exclusive_Breakpoints)) {
            writeWvrCp(i, readWvrContext(t, i));
            writeWcrCp(i, readWcrContext(t, i));
        } లేకపోతే {
            writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
        }
    }
}

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
శూన్య restore_user_debug_context(tcb_t *లక్ష్యం_thread)
{
    ప్రతిజ్ఞా(లక్ష్యం_thread != ఖాళీ);

    ఒకవేళ (లక్ష్యం_thread->tcbArch.tcbContext.breakpointState.వాడిన_breakpoints_bf == 0) {
        ఎక్కించు_All_Disabled_Breakpoint_State();
    } లేకపోతే {
        ఎక్కించు_Breakpoint_State(లక్ష్యం_thread);
    }

    /* ARMv7 manual, sec C3.7:
     * "Usually, an exception return sequence is a context change operation as
     * well as a context synchronization operation, in which case the context
     * change operation is guaranteed to take effect on the debug logic by the
     * end of that exception return sequence."
     *
     * So we don't need to execute ISB here because we're about to RFE.
     */

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    aarch64_restore_user_debug_context(లక్ష్యం_thread);
#అయితే_ముగింపు /* CONFIG_ARCH_ARCH64 */
}

#అయితే_ముగింపు /* ARM_BASE_CP14_SAVE_AND_RESTORE */
