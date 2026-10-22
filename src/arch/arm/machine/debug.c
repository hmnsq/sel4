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

شمار watchpoint_privilege /* WCR[2:1] */ {
    DBGWCR_PRIV_RESERVED = 0u,
    DBGWCR_PRIV_PRIVILEGED = 1u,
    DBGWCR_PRIV_USER = 2u,
    DBGWCR_PRIV_EITHER = 3u
};

شمار watchpoint_access /* WCR[4:3] */ {
    DBGWCR_ACCESS_RESERVED = 0u,
    DBGWCR_ACCESS_بھرو = 1u,
    DBGWCR_ACCESS_STORE = 2u,
    DBGWCR_ACCESS_EITHER = 3u
};

#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */

#اگر_تعریف_شدہ ARM_بنیاد_CP14_SAVE_AND_RESTORE

/* These next few functions (read*Context()/write*Context()) read from TCB
 * context and not from the hardware registers.
 */
لفظ_t readBcrContext(tcb_t *t, uint16_t اشاریہ)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Breakpoints);
    واپس t->tcbArch.tcbContext.breakpointState.breakpoint[اشاریہ].cr;
}

ساکن لفظ_t readBvrContext(tcb_t *t, uint16_t اشاریہ)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Breakpoints);
    واپس t->tcbArch.tcbContext.breakpointState.breakpoint[اشاریہ].vr;
}

ساکن لفظ_t readWcrContext(tcb_t *t, uint16_t اشاریہ)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Watchpoints);
    واپس t->tcbArch.tcbContext.breakpointState.watchpoint[اشاریہ].cr;
}

ساکن لفظ_t readWvrContext(tcb_t *t, uint16_t اشاریہ)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Watchpoints);
    واپس t->tcbArch.tcbContext.breakpointState.watchpoint[اشاریہ].vr;
}

خالی writeBcrContext(tcb_t *t, uint16_t اشاریہ, لفظ_t val)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[اشاریہ].cr = val;
}

خالی writeBvrContext(tcb_t *t, uint16_t اشاریہ, لفظ_t val)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Breakpoints);
    t->tcbArch.tcbContext.breakpointState.breakpoint[اشاریہ].vr = val;
}

ساکن خالی writeWcrContext(tcb_t *t, uint16_t اشاریہ, لفظ_t val)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[اشاریہ].cr = val;
}

ساکن خالی writeWvrContext(tcb_t *t, uint16_t اشاریہ, لفظ_t val)
{
    أكد(اشاریہ < se_L4_عدد_Exclusive_Watchpoints);
    t->tcbArch.tcbContext.breakpointState.watchpoint[اشاریہ].vr = val;
}

#اگر_ختم /* ARM_BASE_CP14_SAVE_AND_RESTORE */

#اگر_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API

/** For debugging: prints out the debug register pair values as returned by the
 * coprocessor.
 *
 * @param nBp Number of breakpoint reg pairs to print, starting at BP #0.
 * @param nBp Number of watchpoint reg pairs to print, starting at WP #0.
 */
غیر_مستعمل ساکن خالی dumpBpsAndWpsCp(صحیح nBp, صحیح nWp)
{
    صحیح i;

    برائے (i = 0; i < nBp; i++) {
        userError("CP BP %d: Bcr %lx, Bvr %lx", i, readBcrCp(i), readBvrCp(i));
    }

    برائے (i = 0; i < nWp; i++) {
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
غیر_مستعمل ساکن خالی dumpBpsAndWpsContext(tcb_t *t, صحیح nBp, صحیح nWp)
{
    صحیح i;

    برائے (i = 0; i < nBp; i++) {
        userError("Ctxt BP %d: Bcr %lx, Bvr %lx", i, readBcrContext(t, i), readBvrContext(t, i));
    }

    برائے (i = 0; i < nWp; i++) {
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
ساکن لفظ_t convert_Arch_مقصد_سائز(لفظ_t archsize)
{
    انتخاب (archsize) {
    حالت 0x1:
        واپس 1;
    حالت 0x3:
        واپس 2;
    حالت 0xFF:
        واپس 8;
    طےشدہ:
        أكد(archsize == 0xF);
        واپس 4;
    }
}

/** Convert an access perms API value (seL4_BreakOnRead, etc) into the register
 * encoding that matches it.
 */
ساکن لفظ_t convert_Access_مقصد_Arch(لفظ_t access)
{
    انتخاب (access) {
    حالت seL4_BreakOnRead:
        واپس DBGWCR_ACCESS_بھرو;
    حالت seL4_BreakOnWrite:
        واپس DBGWCR_ACCESS_STORE;
    طےشدہ:
        أكد(access == seL4_BreakOnReadWrite);
        واپس DBGWCR_ACCESS_EITHER;
    }
}

/** Convert an arch-specific register encoding back into an API access perms
 * value.
 */
ساکن لفظ_t convert_Arch_مقصد_Access(لفظ_t archaccess)
{
    انتخاب (archaccess) {
    حالت DBGWCR_ACCESS_بھرو:
        واپس seL4_BreakOnRead;
    حالت DBGWCR_ACCESS_STORE:
        واپس seL4_BreakOnWrite;
    طےشدہ:
        أكد(archaccess == DBGWCR_ACCESS_EITHER);
        واپس seL4_BreakOnReadWrite;
    }
}

uint16_t get_Bp_عدد_From_Type(uint16_t bp_عدد, لفظ_t type)
{
    أكد(type == seL4_InstructionBreakpoint || type == se_L4_ڈیٹا_Breakpoint
           || type == se_L4_Single_قدم);

    انتخاب (type) {
    حالت seL4_InstructionBreakpoint:
    حالت se_L4_Single_قدم:
        واپس bp_عدد;
    طےشدہ: /* seL4_DataBreakpoint: */
        أكد(type == se_L4_ڈیٹا_Breakpoint);
        واپس bp_عدد + se_L4_عدد_Exclusive_Breakpoints;
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
خالی سیٹ_Breakpoint(tcb_t *t,
                   uint16_t bp_عدد,
                   لفظ_t vaddr, لفظ_t type, لفظ_t سائز, لفظ_t rw)
{
    bp_عدد = convert_Bp_عدد_مقصد_Arch(bp_عدد);

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
    اگر (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        writeBvrContext(t, bp_عدد, vaddr);

        /* Preserve reserved bits. */
        bcr.الفاظ[0] = readBcrContext(t, bp_عدد);
        bcr = dbg_bcr_سیٹ_enabled(bcr, 1);
        bcr = dbg_bcr_سیٹ_lbn(bcr, 0);
        bcr = dbg_bcr_سیٹ_pmc(bcr, DBGBCR_PRIV_USER);
        bcr = dbg_bcr_سیٹ_hmc(bcr, 0);
        bcr = dbg_bcr_سیٹ_ssc(bcr, 0);
        bcr = dbg_bcr_سیٹ_bas(bcr, convert_سائز_مقصد_Arch(4));
        bcr = Arch_تیاری_Bcr(bcr, true);
        writeBcrContext(t, bp_عدد, bcr.الفاظ[0]);
    } ورنہ {
        dbg_wcr_t wcr;

        writeWvrContext(t, bp_عدد, vaddr);

        /* Preserve reserved bits */
        wcr.الفاظ[0] = readWcrContext(t, bp_عدد);
        wcr = dbg_wcr_سیٹ_enabled(wcr, 1);
        wcr = dbg_wcr_سیٹ_pac(wcr, DBGWCR_PRIV_USER);
        wcr = dbg_wcr_سیٹ_bas(wcr, convert_سائز_مقصد_Arch(سائز));
        wcr = dbg_wcr_سیٹ_lsc(wcr, convert_Access_مقصد_Arch(rw));
        wcr = dbg_wcr_سیٹ_watchpoint_Type(wcr, 0);
        wcr = dbg_wcr_سیٹ_lbn(wcr, 0);
        wcr = dbg_wcr_سیٹ_پتہ_نقاب(wcr, 0);
        wcr = dbg_wcr_سیٹ_hmc(wcr, 0);
        wcr = dbg_wcr_سیٹ_ssc(wcr, 0);
        writeWcrContext(t, bp_عدد, wcr.الفاظ[0]);
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
getBreakpoint_t getBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    getBreakpoint_t ret;

    ret.type = get_Type_From_Bp_عدد(bp_عدد);
    bp_عدد = convert_Bp_عدد_مقصد_Arch(bp_عدد);

    اگر (ret.type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.الفاظ[0] = readBcrContext(t, bp_عدد);
        اگر (Arch_breakpointIsSingleStepping(t, bp_عدد)) {
            ret.type = se_L4_Single_قدم;
        };
        ret.سائز = 0;
        ret.rw = seL4_BreakOnRead;
        ret.vaddr = readBvrContext(t, bp_عدد);
        ret.is_enabled = dbg_bcr_get_enabled(bcr);
    } ورنہ {
        dbg_wcr_t wcr;

        wcr.الفاظ[0] = readWcrContext(t, bp_عدد);
        ret.سائز = convert_Arch_مقصد_سائز(dbg_wcr_get_bas(wcr));
        ret.rw = convert_Arch_مقصد_Access(dbg_wcr_get_lsc(wcr));
        ret.vaddr = readWvrContext(t, bp_عدد);
        ret.is_enabled = dbg_wcr_get_enabled(wcr);
    }
    واپس ret;
}

/** Disables and clears the configuration of a hardware breakpoint.
 *
 * @param at arch_tcb_t holding the reg context for the target thread.
 * @param bp_num The hardware breakpoint you want to disable+clear.
 */
خالی unsetBreakpoint(tcb_t *t, uint16_t bp_عدد)
{
    لفظ_t type;

    type = get_Type_From_Bp_عدد(bp_عدد);
    bp_عدد = convert_Bp_عدد_مقصد_Arch(bp_عدد);

    اگر (type == seL4_InstructionBreakpoint) {
        dbg_bcr_t bcr;

        bcr.الفاظ[0] = readBcrContext(t, bp_عدد);
        bcr = dbg_bcr_سیٹ_enabled(bcr, 0);
        writeBcrContext(t, bp_عدد, bcr.الفاظ[0]);
        writeBvrContext(t, bp_عدد, 0);
    } ورنہ {
        dbg_wcr_t wcr;

        wcr.الفاظ[0] = readWcrContext(t, bp_عدد);
        wcr = dbg_wcr_سیٹ_enabled(wcr, 0);
        writeWcrContext(t, bp_عدد, wcr.الفاظ[0]);
        writeWvrContext(t, bp_عدد, 0);
    }
}

/** Load an initial, all-disabled setup state for the registers.
 */
BOOT_رمز خالی disableAllBpsAndWps(خالی)
{
    صحیح i;

    برائے (i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        writeBvrCp(i, 0);
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    برائے (i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
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
صحیح get_And_Reset_فعال_Breakpoint(لفظ_t vaddr, لفظ_t reason)
{
    لفظ_t align_نقاب;
    صحیح i, ret = -1;

    اگر (reason == seL4_InstructionBreakpoint) {
        برائے (i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
            dbg_bcr_t bcr;
            لفظ_t bvr = readBvrCp(i);

            bcr.الفاظ[0] = readBcrCp(i);
            /* The actual trigger address may be an unaligned sub-byte of the
             * range, which means it's not guaranteed to match the aligned value
             * that was programmed into the address register.
             */
            align_نقاب = convert_Arch_مقصد_سائز(dbg_bcr_get_bas(bcr));
            align_نقاب = ~(align_نقاب - 1);

            اگر (bvr != (vaddr & align_نقاب) || !dbg_bcr_get_enabled(bcr)) {
                جاری;
            }

            ret = i;
            واپس ret;
        }
    }

    اگر (reason == se_L4_ڈیٹا_Breakpoint) {
        برائے (i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
            dbg_wcr_t wcr;
            لفظ_t wvr = readWvrCp(i);

            wcr.الفاظ[0] = readWcrCp(i);
            align_نقاب = convert_Arch_مقصد_سائز(dbg_wcr_get_bas(wcr));
            align_نقاب = ~(align_نقاب - 1);

            اگر (wvr != (vaddr & align_نقاب) || !dbg_wcr_get_enabled(wcr)) {
                جاری;
            }

            ret = i;
            واپس ret;
        }
    }

    واپس ret;
}

#اگر_ختم /* CONFIG_HARDWARE_DEBUG_API */

#اگر_تعریف_شدہ ARM_بنیاد_CP14_SAVE_AND_RESTORE

/** Mirrors Arch_initFpuContext.
 *
 * Zeroes out the BVR thread context and preloads reserved bit values from the
 * control regs into the thread context so we can operate solely on the values
 * cached in RAM in API calls, rather than retrieving the values from the
 * coprocessor.
 */
خالی Arch_initBreakpointContext(user_context_t *uc)
{
    uc->breakpointState = armKSNullBreakpointState;
}

خالی بھرو_All_Disabled_Breakpoint_State(خالی)
{
    صحیح i;

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
    برائے (i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
    }
    برائے (i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
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
خالی saveAllBreakpointState(tcb_t *t)
{
    صحیح i;

    أكد(t != تہی);

    برائے (i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        writeBvrContext(t, i, readBvrCp(i));
        writeBcrContext(t, i, readBcrCp(i));
    }

    برائے (i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
        writeWvrContext(t, i, readWvrCp(i));
        writeWcrContext(t, i, readWcrCp(i));
    }
}

#اگر_تعریف_شدہ ARM_HYP_CP14_SAVE_AND_RESTORE_VCPU_دھاگے
خالی Arch_debugAssociateVCPUTCB(tcb_t *t)
{
    /* Don't attempt to shift beyond end of word. */
    أكد(se_L4_عدد_HWBreakpoints < حجم(لفظ_t) * 8);

    /* Set all the bits to 1, so loadBreakpointState() will
     * restore all the debug regs unconditionally.
     */
    t->tcbArch.tcbContext.breakpointState.مستعمل_breakpoints_bf = نقاب(se_L4_عدد_HWBreakpoints);
}

خالی Arch_debugDissociateVCPUTCB(tcb_t *t)
{
    t->tcbArch.tcbContext.breakpointState.مستعمل_breakpoints_bf = 0;
}
#اگر_ختم

ساکن خالی بھرو_Breakpoint_State(tcb_t *t)
{
    صحیح i;

    أكد(t != تہی);

    برائے (i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        اگر (t->tcbArch.tcbContext.breakpointState.مستعمل_breakpoints_bf & ثنائی_ہندسہ(i)) {
            writeBvrCp(i, readBvrContext(t, i));
            writeBcrCp(i, readBcrContext(t, i));
        } ورنہ {
            /* If the thread isn't using the BP, then just load
             * a default "disabled" state.
             */
            writeBcrCp(i, readBcrCp(i) & ~DBGBCR_ENABLE);
        }
    }

    برائے (i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
        اگر (t->tcbArch.tcbContext.breakpointState.مستعمل_breakpoints_bf &
            ثنائی_ہندسہ(i + se_L4_عدد_Exclusive_Breakpoints)) {
            writeWvrCp(i, readWvrContext(t, i));
            writeWcrCp(i, readWcrContext(t, i));
        } ورنہ {
            writeWcrCp(i, readWcrCp(i) & ~DBGWCR_ENABLE);
        }
    }
}

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
خالی restore_user_debug_context(tcb_t *ہدف_thread)
{
    أكد(ہدف_thread != تہی);

    اگر (ہدف_thread->tcbArch.tcbContext.breakpointState.مستعمل_breakpoints_bf == 0) {
        بھرو_All_Disabled_Breakpoint_State();
    } ورنہ {
        بھرو_Breakpoint_State(ہدف_thread);
    }

    /* ARMv7 manual, sec C3.7:
     * "Usually, an exception return sequence is a context change operation as
     * well as a context synchronization operation, in which case the context
     * change operation is guaranteed to take effect on the debug logic by the
     * end of that exception return sequence."
     *
     * So we don't need to execute ISB here because we're about to RFE.
     */

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    aarch64_restore_user_debug_context(ہدف_thread);
#اگر_ختم /* CONFIG_ARCH_ARCH64 */
}

#اگر_ختم /* ARM_BASE_CP14_SAVE_AND_RESTORE */
