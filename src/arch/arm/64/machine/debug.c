/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#جے_تعریف_شدہ CONFIG_HARDWARE_DEBUG_API

#ضمن <machine/debug.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/machine/debug.h>
#ضمن <arch/machine/debug_conf.h>
#ضمن <arch/machine/registerset.h>
#ضمن <arch/model/statedata.h>
#ضمن <armv/debug.h>
#ضمن <mode/machine/debug.h>
#ضمن <sel4/constants.h> /* seL4_NumExclusiveBreakpoints/Watchpoints */
#ضمن <string.h>
#ضمن <util.h>

#تعریف_کرو MDSCR_MDE (ثنائی_ہندسہ(15))
#تعریف_کرو MDSCR_SS  (ثنائی_ہندسہ(0))
#تعریف_کرو SPSR_SS   (ثنائی_ہندسہ(21))

#تعریف_کرو ESR_EXCEPTION_CLASS_نقاب 0xFC000000
#تعریف_کرو ESR_EXCEPTION_CLASS_OFF 26

#تعریف_کرو DEBUG_اندراج_BREAKPOINT      0x30
#تعریف_کرو DEBUG_اندراج_SINGLE_قدم     0x32
#تعریف_کرو DEBUG_اندراج_WATCHPOINT      0x34
#تعریف_کرو DEBUG_اندراج_EXPLICIT_BKPT   0x3C

#تعریف_کرو OSDLR_جندرا (ثنائی_ہندسہ(0))
#تعریف_کرو OSLAR_جندرا (ثنائی_ہندسہ(0))

bool_t byte8WatchpointsSupported(فراغ)
{
    أرجع true;
}

exception_t handleDebugFaultEvent(لفظ_t esr)
{
    MCS_DO_IF_BUDGET({
        current_fault = handleUserLevelDebugException(esr, getRestartPC(گرہ_STATE(ksCurThread)));
        إذا (seL4_Fault_get_seL4_FaultType(current_fault) != seL4_Fault_NullFault)
        {
            handleFault(گرہ_STATE(ksCurThread));
        }
    })
    ویلا_سارنی();
    activateThread();

    أرجع EXCEPTION_NONE;
}

/** Initiates or halts single-stepping on the target process.
 *
 * @param at arch_tcb_t for the target process to be configured.
 * @param bp_num The hardware ID of the breakpoint register to be used.
 * @param n_instr The number of instructions to step over.
 */
bool_t configureSingleStepping(tcb_t *t, uint16_t bp_عدد, لفظ_t n_instr,
                               bool_t is_جواب)
{
    إذا (n_instr > 0) {
        /* Enable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = true;
    } وإلا {
        /* Disable single stepping */
        t->tcbArch.tcbContext.breakpointState.single_قدم_enabled = false;
    }

    t->tcbArch.tcbContext.breakpointState.n_instructions = n_instr;
    أرجع true;
}

/* Guides the debug hardware initialization sequence. */
BOOT_کوڈ bool_t Arch_initHardwareBreakpoints(فراغ)
{
    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.2: The Enable controls for each debug exception are:
     *    ... MDSCR_EL1.MDE
     */

    لفظ_t mdscr = 0;
    MRS("MDSCR_EL1", mdscr);
    mdscr |= MDSCR_MDE;
    MSR("MDSCR_EL1", mdscr);

    /*
     * ARMv8 Architecture Reference Manual for A-profile Architecture
     * D2.4: A debug exception can be taken only if all the following are true:
     *    - The OS Lock is unlocked
     *    - DoubleLockStatus() = False
     *    - The debug exception is enabled from the current exception level
     *    - The debug exception is enabled from the current security state
     */

    /* Ensure that the OS double lock is unset */
    لفظ_t osdlr = 0;
    MRS("osdlr_el1", osdlr);
    osdlr &= ~OSDLR_جندرا;
    MSR("osdlr_el1", osdlr);

    /* Ensure that the OS lock is unset */
    لفظ_t oslar = 0;
    MSR("oslar_el1", oslar);

    /* Ensure that all the breakpoint and watchpoint registers are initially disabled */
    disableAllBpsAndWps();

    /* Ensure that single stepping is initially disabled */
    MRS("MDSCR_EL1", mdscr);
    mdscr &= ~MDSCR_SS;
    MSR("MDSCR_EL1", mdscr);

    /* Finally, also pre-load some initial register state that can be used
     * for all new threads so that their initial saved debug register state
     * is valid when it's first loaded onto the CPU.
     */
    لكل (صحيح i = 0; i < se_L4_عدد_Exclusive_Breakpoints; i++) {
        armKSNullBreakpointState.breakpoint[i].cr = readBcrCp(i) & ~DBGBCR_ENABLE;
    }
    لكل (صحيح i = 0; i < se_L4_عدد_Exclusive_Watchpoints; i++) {
        armKSNullBreakpointState.watchpoint[i].cr = readWcrCp(i) & ~DBGWCR_ENABLE;
    }

    أرجع true;
}

/* Abstract wrapper around the ESR fault status value */

ساكن لفظ_t get_Fault_حالت(لفظ_t esr)
{
    أرجع (esr & ESR_EXCEPTION_CLASS_نقاب) >> ESR_EXCEPTION_CLASS_OFF;
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
bool_t isDebugFault(لفظ_t esr)
{
    لفظ_t exception_class = get_Fault_حالت(esr);
    أرجع (exception_class == DEBUG_اندراج_BREAKPOINT ||
            exception_class == DEBUG_اندراج_SINGLE_قدم ||
            exception_class == DEBUG_اندراج_WATCHPOINT ||
            exception_class == DEBUG_اندراج_EXPLICIT_BKPT);
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
seL4_Fault_t handleUserLevelDebugException(لفظ_t esr, لفظ_t fault_vaddr)
{
    صحيح سرگرم_bp;
    لفظ_t bp_reason, bp_vaddr;
    لفظ_t exception_class = get_Fault_حالت(esr);

#جے_تعریف_شدہ TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.رستہ = اندراج_Debug_Fault;
    ks_Kernel_اندراج.لفظ = exception_class;
#جے_انت

    بدل (exception_class) {
    حالة DEBUG_اندراج_BREAKPOINT:
        bp_reason = seL4_InstructionBreakpoint;
        bp_vaddr = fault_vaddr;
        اكسر;
    حالة DEBUG_اندراج_WATCHPOINT:
        bp_reason = se_L4_انکڑے_Breakpoint;
        bp_vaddr = getFAR();
        اكسر;
    حالة DEBUG_اندراج_SINGLE_قدم:
        bp_reason = se_L4_Single_قدم;
        bp_vaddr = fault_vaddr;
        سرگرم_bp = 0;
        اكسر;
    افتراضي: /* EXPLICIT_BKPT: BKPT instruction */
        أكد(exception_class == DEBUG_اندراج_EXPLICIT_BKPT);
        bp_reason = seL4_SoftwareBreakRequest;
        bp_vaddr = fault_vaddr;
        سرگرم_bp = 0;
    }

    /* There is no hardware register associated with BKPT instruction
    * triggers or single stepping.
    */
    إذا (bp_reason != seL4_SoftwareBreakRequest && bp_reason != se_L4_Single_قدم) {
        سرگرم_bp = get_And_Reset_سرگرم_Breakpoint(bp_vaddr, bp_reason);
        سرگرم_bp = get_Bp_عدد_From_Type(سرگرم_bp, bp_reason);
        أكد(سرگرم_bp >= 0);
    }

    إذا (bp_reason == se_L4_Single_قدم && !single_قدم_Fault_Counter_Ready(گرہ_STATE(ksCurThread))) {
        أرجع seL4_Fault_NullFault_new();
    }

    أرجع seL4_Fault_DebugException_new(bp_vaddr, سرگرم_bp, bp_reason);
}

#جے_انت /* CONFIG_HARDWARE_DEBUG_API */

#جے_تعریف_شدہ ARM_ادھار_CP14_SAVE_AND_RESTORE

/** Pops debug register context for a thread into the CPU.
 *
 * Mirrors the idea of restore_user_context.
 */
فراغ aarch64_restore_user_debug_context(tcb_t *نشانہ_thread)
{
    أكد(نشانہ_thread != عدم);

    /* Set/unset single stepping if applicable */
    لفظ_t mdscr = 0, spsr = 0;
    MRS("MDSCR_EL1", mdscr);
    spsr = get_رجسٹر(نشانہ_thread, SPSR_EL1);
    إذا (نشانہ_thread->tcbArch.tcbContext.breakpointState.single_قدم_enabled) {
        /* Enable single stepping */
        mdscr |= MDSCR_SS;
        spsr |= SPSR_SS;
    } وإلا {
        /* Disable single stepping */
        mdscr &= ~MDSCR_SS;
        spsr &= ~SPSR_SS;
    }
    MSR("MDSCR_EL1", mdscr);
    سیٹ_رجسٹر(نشانہ_thread, SPSR_EL1, spsr);
}

#جے_انت /* ARM_BASE_CP14_SAVE_AND_RESTORE */
