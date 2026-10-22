/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#अगर_घोषित CONFIG_HARDWARE_DEBUG_API

#समावेश <string.h>
#समावेश <util.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/machine/debug.h>
#समावेश <arch/machine/debug_conf.h>
#समावेश <arch/kernel/vspace.h>
#समावेश <arch/machine/registerset.h>
#समावेश <armv/debug.h>
#समावेश <mode/machine/debug.h>
#समावेश <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */

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

क्रमागत watchpoint_privilege /* WCR[2:1] */ {
    DBGWCR_PRIV_RESERVED = 0u,
    DBGWCR_PRIV_PRIVILEGED = 1u,
    DBGWCR_PRIV_USER = 2u,
    DBGWCR_PRIV_EITHER = 3u
};

क्रमागत watchpoint_access /* WCR[4:3] */ {
    DBGWCR_ACCESS_RESERVED = 0u,
    DBGWCR_ACCESS_LOAD = 1u,
    DBGWCR_ACCESS_STORE = 2u,
    DBGWCR_ACCESS_EITHER = 3u
};

#पूर्ण_अगर /* CONFIG_HARDWARE_DEBUG_API */

#अगर_घोषित ARM_BASE_CP14_SAVE_AND_RESTORE

/* These next few functions (read*Context()/write*Context()) read from TCB
 * context and not from the hardware registers.
 */
word_t readBcrContext(tcb_t *t, uint16_t सूचकांक)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Breakpoints);
    वापस t->tcbArch.tcbContext.breakpointState.breakpoint[सूचकांक].cr;
}

अटल word_t readBvrContext(tcb_t *t, uint16_t सूचकांक)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Breakpoints);
    वापस t->tcbArch.tcbContext.breakpointState.breakpoint[सूचकांक].vr;
}

अटल word_t readWcrContext(tcb_t *t, uint16_t सूचकांक)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Watchpoints);
    वापस t->tcbArch.tcbContext.breakpointState.watchpoint[सूचकांक].cr;
}

अटल word_t readWvrContext(tcb_t *t, uint16_t सूचकांक)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Watchpoints);
    वापस t->tcbArch.tcbContext.breakpointState.watchpoint[सूचकांक].vr;
}

व्योम writeBcrContext(tcb_t *t, uint16_t सूचकांक, word_t val)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[सूचकांक].cr = val;
}

व्योम writeBvrContext(tcb_t *t, uint16_t सूचकांक, word_t val)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[सूचकांक].vr = val;
}

अटल व्योम writeWcrContext(tcb_t *t, uint16_t सूचकांक, word_t val)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[सूचकांक].cr = val;
}

अटल व्योम writeWvrContext(tcb_t *t, uint16_t सूचकांक, word_t val)
{
    निश्चित(सूचकांक < se_L4_अंक_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[सूचकांक].vr = val;
}

#पूर्ण_अगर /* ARM_BASE_CP14_SAVE_AND_RESTORE */

#अगर_घोषित CONFIG_HARDWARE_DEBUG_API

/** For debugging: prints out the debug register pair values as returned by the
 * coprocessor.
 *
 * @param nBp Number of breakpoint reg pairs to print, starting at BP #0.
 * @param nBp Number of watchpoint reg pairs to print, starting at WP #0.
 */
UNUSED अटल व्योम dumpBpsAndWpsCp(पूर्णांक nBp, पूर्णांक nWp)
{
    पूर्णांक i;

    क्रम (i = 0; i < nBp; i++) {
        userError("CP BP %d: Bcr %lx, Bvr %lx", i, readBcrCp(i), readBvrCp(i));
    }

    क्रम (i = 0; i < nWp; i++) {
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
UNUSED अटल व्योम dumpBpsAndWpsContext(tcb_t *t, पूर्णांक nBp, पूर्णांक nWp)
{
    पूर्णांक i;

    क्रम (i = 0; i < nBp; i++) {
        userError("Ctxt BP %d: Bcr %lx, Bvr %lx", i, readBcrContext(t, i), readBvrContext(t, i));
    }

    क्रम (i = 0; i < nWp; i++) {
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
अटल word_t convert_Arch_To_आकार(word_t archsize)
{
    चयन (archsize) {
    हाल 0x1:
        वापस 1;
    हाल 0x3:
        वापस 2;
    हाल 0xFF:
        वापस 8;
    शेष:
        निश्चित(archsize == 0xF);
        वापस 4;
    }
}

/** Convert an access perms API value (seL4_BreakOnRead, etc) into the register
 * encoding that matches it.
 */
अटल word_t convertAccessToArch(word_t access)
{
    चयन (access) {
    हाल seL4_BreakOnRead:
        वापस DBGWCR_ACCESS_LOAD;
    हाल seL4_BreakOnWrite:
        वापस DBGWCR_ACCESS_STORE;
    शेष:
        निश्चित(access == seL4_BreakOnReadWrite);
        वापस DBGWCR_ACCESS_EITHER;
    }
}

/** Convert an arch-specific register encoding back into an API access perms
 * value.
 */
अटल word_t convertArchToAccess(word_t archaccess)
{
    चयन (archaccess) {
    हाल DBGWCR_ACCESS_LOAD:
        वापस seL4_BreakOnRead;
    हाल DBGWCR_ACCESS_STORE:
        वापस seL4_BreakOnWrite;
    शेष:
        निश्चित(archaccess == DBGWCR_ACCESS_EITHER);
        वापस seL4_BreakOnReadWrite;
    }
}

uint16_t get_Bp_अंक_From_Type(uint16_t bp_अंक, word_t type)
{
    निश्चित(type == seL4_InstructionBreakpoint || type == se_L4_तथ्य_Breakpoint
           || type == seL4_SingleStep);

    चयन (type) {
    हाल seL4_InstructionBreakpoint:
    हाल seL4_SingleStep:
        वापस bp_अंक;
    शेष: /* seL4_DataBreakpoint: */
        निश्चित(type == se_L4_तथ्य_Breakpoint);
        वापस bp_अंक + se_L4_अंक_Exclusive_Breakpoints;
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
व्योम setBreakpoint(tcb_t *t,
                   uint16_t bp_अंक,
                   word_t vaddr, word_t type, word_t आकार, word_t rw)
{
    bp_अंक = convert_Bp_अंक_To_Arch(bp_अंक);

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
    अगर (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        writeBvrContext(t, bp_अंक, vaddr);

        /* Preserve reserved bits. */
        bcr.शब्दगण[0] = readBcrContext(t, bp_अंक);
        bcr = dbg_bcr_set_enabled(bcr, 1);
        bcr = dbg_bcr_set_lbn(bcr, 0);
        bcr = dbg_bcr_set_pmc(bcr, DBGBCR_PRIV_USER);
        bcr = dbg_bcr_set_hmc(bcr, 0);
        bcr = dbg_bcr_set_ssc(bcr, 0);
        bcr = dbg_bcr_set_bas(bcr, convert_आकार_To_Arch(4));
        bcr = Arch_setupBcr(bcr, true);
        writeBcrContext(t, bp_अंक, bcr.शब्दगण[0]);
    } अन्यथा {
        dbg_wcr_t wcr;

        writeWvrContext(t, bp_अंक, vaddr);

        /* Preserve reserved bits */
        wcr.शब्दगण[0] = readWcrContext(t, bp_अंक);
        wcr = dbg_wcr_set_enabled(wcr, 1);
        wcr = dbg_wcr_set_pac(wcr, DBGWCR_PRIV_USER);
        wcr = dbg_wcr_set_bas(wcr, convert_आकार_To_Arch(आकार));
        wcr = dbg_wcr_set_lsc(wcr, convertAccessToArch(rw));
        wcr = dbg_wcr_set_watchpointType(wcr, 0);
        wcr = dbg_wcr_set_lbn(wcr, 0);
        wcr = dbg_wcr_set_addressMask(wcr, 0);
        wcr = dbg_wcr_set_hmc(wcr, 0);
        wcr = dbg_wcr_set_ssc(wcr, 0);
        writeWcrContext(t, bp_अंक, wcr.शब्दगण[0]);
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    getBreakpoint_t ret;

    ret.type = get_Type_From_Bp_अंक(bp_अंक);
    bp_अंक = convert_Bp_अंक_To_Arch(bp_अंक);

    अगर (ret.type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.शब्दगण[0] = readBcrContext(t, bp_अंक);
        अगर (Arch_breakpointIsSingleStepping(t, bp_अंक)) {
            ret.type = seL4_SingleStep;
        };
        ret.आकार = 0;
        ret.rw = seL4_BreakOnRead;
        ret.vaddr = readBvrContext(t, bp_अंक);
        ret.is_enabled = dbg_bcr_get_enabled(bcr);
    } अन्यथा {
        dbg_wcr_t wcr;

        wcr.शब्दगण[0] = readWcrContext(t, bp_अंक);
        ret.आकार = convert_Arch_To_आकार(dbg_wcr_get_bas(wcr));
        ret.rw = convertArchToAccess(dbg_wcr_get_lsc(wcr));
        ret.vaddr = readWvrContext(t, bp_अंक);
        ret.is_enabled = dbg_wcr_get_enabled(wcr);
    }
    वापस ret;
}

/** Disables and clears the configuration of a hardware breakpoint.
 *
 * @param at arch_tcb_t holding the reg context for the target thread.
 * @param bp_num The hardware breakpoint you want to disable+clear.
 */
व्योम unsetBreakpoint(tcb_t *t, uint16_t bp_अंक)
{
    word_t type;

    type = get_Type_From_Bp_अंक(bp_अंक);
    bp_अंक = convert_Bp_अंक_To_Arch(bp_अंक);

    अगर (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.शब्दगण[0] = readBcrContext(t, bp_अंक);
        bcr = dbg_bcr_set_enabled(bcr, 0);
        writeBcrContext(t, bp_अंक, bcr.शब्दगण[0]);
        writeBvrContext(t, bp_अंक, 0);
    } अन्यथा {
        dbg_wcr_t wcr;

        wcr.शब्दगण[0] = readWcrContext(t, bp_अंक);
        wcr = dbg_wcr_set_enabled(wcr, 0);
        writeWcrContext(t, bp_अंक, wcr.शब्दगण[0]);
        writeWvrContext(t, bp_अंक, 0);
    }
}

/** Load an initial, all-disabled setup state for the registers.
 */
BOOT_CODE व्योम disableAllBpsAndWps(व्योम)
{
    पूर्णांक i;

    क्रम (i = 0; i < se_L4_अंक_Exclusive_Breakpoints; i++) {
        writeBvrCp(i, 0);
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    क्रम (i = 0; i < se_L4_अंक_Exclusive_Watchpoints; i++) {
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
पूर्णांक getAndResetActiveBreakpoint(word_t vaddr, word_t reason)
{
    word_t align_mask;
    पूर्णांक i, ret = -1;

    अगर (reason == seL4_InstructionBreakpoint) {
        क्रम (i = 0; i < se_L4_अंक_Exclusive_Breakpoints; i++) {
            dbg_bcr_t bcr;
            word_t bvr = readBvrCp(i);

            bcr.शब्दगण[0] = readBcrCp(i);
            /* The actual trigger address may be an unaligned sub-byte of the
             * range, which means it's not guaranteed to match the aligned value
             * that was programmed into the address register.
             */
            align_mask = convert_Arch_To_आकार(dbg_bcr_get_bas(bcr));
            align_mask = ~(align_mask - 1);

            अगर (bvr != (vaddr & align_mask) || !dbg_bcr_get_enabled(bcr)) {
                जारी;
            }

            ret = i;
            वापस ret;
        }
    }

    अगर (reason == se_L4_तथ्य_Breakpoint) {
        क्रम (i = 0; i < se_L4_अंक_Exclusive_Watchpoints; i++) {
            dbg_wcr_t wcr;
            word_t wvr = readWvrCp(i);

            wcr.शब्दगण[0] = readWcrCp(i);
            align_mask = convert_Arch_To_आकार(dbg_wcr_get_bas(wcr));
            align_mask = ~(align_mask - 1);

            अगर (wvr != (vaddr & align_mask) || !dbg_wcr_get_enabled(wcr)) {
                जारी;
            }

            ret = i;
            वापस ret;
        }
    }

    वापस ret;
}

#पूर्ण_अगर /* CONFIG_HARDWARE_DEBUG_API */

#अगर_घोषित ARM_BASE_CP14_SAVE_AND_RESTORE

/** Mirrors Arch_initFpuContext.
 *
 * Zeroes out the BVR thread context and preloads reserved bit values from the
 * control regs into the thread context so we can operate solely on the values
 * cached in RAM in API calls, rather than retrieving the values from the
 * coprocessor.
 */
व्योम Arch_initBreakpointContext(user_context_t *uc)
{
    uc->breakpointState = armKSNullBreakpointState;
}

व्योम loadAllDisabledBreakpointState(व्योम)
{
    पूर्णांक i;

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
    क्रम (i = 0; i < se_L4_अंक_Exclusive_Breakpoints; i++) {
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    क्रम (i = 0; i < se_L4_अंक_Exclusive_Watchpoints; i++) {
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
व्योम saveAllBreakpointState(tcb_t *t)
{
    पूर्णांक i;

    निश्चित(t != शून्य);

    क्रम (i = 0; i < se_L4_अंक_Exclusive_Breakpoints; i++) {
        writeBvrContext(t, i, readBvrCp(i));
        writeBcrContext(t, i, readBcrCp(i));
    }

    क्रम (i = 0; i < se_L4_अंक_Exclusive_Watchpoints; i++) {
        writeWvrContext(t, i, readWvrCp(i));
        writeWcrContext(t, i, readWcrCp(i));
    }
}

#अगर_घोषित ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_THREADS
व्योम Arch_debugAssociateVCPUTCB(tcb_t *t)
{
    /* Don't attempt to shift beyond end of word. */
    निश्चित(se_L4_अंक_HWBreakpoints < माप(word_t) * 8);

    /* Set all the bits to 1, so loadBreakpointState() will
     * restore all the debug regs unconditionally.
     */
    t->tcbArch.tcbContext.breakpointState.used_breakpoints_bf = MASK(se_L4_अंक_HWBreakpoints);
}

व्योम Arch_debugDissociateVCPUTCB(tcb_t *t)
{
    t->tcbArch.tcbContext.breakpointState.used_breakpoints_bf = 0;
}
#पूर्ण_अगर

अटल व्योम loadBreakpointState(tcb_t *t)
{
    पूर्णांक i;

    निश्चित(t != शून्य);

    क्रम (i = 0; i < se_L4_अंक_Exclusive_Breakpoints; i++) {
        अगर (t->tcbArch.tcbContext.breakpointState.used_breakpoints_bf & BIT(i)) {
            writeBvrCp(i, readBvrContext(t, i));
            writeBcrCp(i, readBcrContext(t, i));
        } अन्यथा {
            /* If the thread isn't using the BP, then just load
             * a default "disabled" state.
             */
            writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
        }
    }

    क्रम (i = 0; i < se_L4_अंक_Exclusive_Watchpoints; i++) {
        अगर (t->tcbArch.tcbContext.breakpointState.used_breakpoints_bf &
            BIT(i + se_L4_अंक_Exclusive_Breakpoints)) {
            writeWvrCp(i, readWvrContext(t, i));
            writeWcrCp(i, readWcrContext(t, i));
        } अन्यथा {
            writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
        }
    }
}

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
व्योम restore_user_debug_context(tcb_t *target_thread)
{
    निश्चित(target_thread != शून्य);

    अगर (target_thread->tcbArch.tcbContext.breakpointState.used_breakpoints_bf == 0) {
        loadAllDisabledBreakpointState();
    } अन्यथा {
        loadBreakpointState(target_thread);
    }

    /* ARMv7 manual, sec C3.7:
     * "Usually, an exception return sequence is a context change operation as
     * well as a context synchronization operation, in which case the context
     * change operation is guaranteed to take effect on the debug logic by the
     * end of that exception return sequence."
     *
     * So we don't need to execute ISB here because we're about to RFE.
     */

#अगर_घोषित CONFIG_ARCH_AARCH64
    aarch64_restore_user_debug_context(target_thread);
#पूर्ण_अगर /* CONFIG_ARCH_ARCH64 */
}

#पूर्ण_अगर /* ARM_BASE_CP14_SAVE_AND_RESTORE */
