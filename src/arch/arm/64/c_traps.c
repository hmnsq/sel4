/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <model/statedata.h>
#ضمن <arch/fastpath/fastpath.h>
#ضمن <arch/kernel/traps.h>
#ضمن <api/syscall.h>
#ضمن <linker.h>
#ضمن <machine/fpu.h>

#ضمن <benchmark/benchmark_track.h>
#ضمن <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
فراغ VISIBLE NORETURN restore_user_context(فراغ)
{
    c_exit_hook();

#اگر_تعریف_شده ARM_CP14_SAVE_AND_RESTORE_NATIVE_نخها
    restore_user_debug_context(گره_STATE(ksCurThread));
#پایان_اگر

    گره_UNLOCK_IF_HELD;

    asm متطاير(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#وإلا
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#پایان_اگر
        /* Restore remaining registers */
        "ldp     x0,  x1,  [sp, #16 * 0]    \n"
        "ldp     x2,  x3,  [sp, #16 * 1]    \n"
        "ldp     x4,  x5,  [sp, #16 * 2]    \n"
        "ldp     x6,  x7,  [sp, #16 * 3]    \n"
        "ldp     x8,  x9,  [sp, #16 * 4]    \n"
        "ldp     x10, x11, [sp, #16 * 5]    \n"
        "ldp     x12, x13, [sp, #16 * 6]    \n"
        "ldp     x14, x15, [sp, #16 * 7]    \n"
        "ldp     x16, x17, [sp, #16 * 8]    \n"
        "ldp     x18, x19, [sp, #16 * 9]    \n"
        "ldp     x20, x21, [sp, #16 * 10]   \n"
        "ldp     x22, x23, [sp, #16 * 11]   \n"
        "ldp     x24, x25, [sp, #16 * 12]   \n"
        "ldp     x26, x27, [sp, #16 * 13]   \n"
        "ldp     x28, x29, [sp, #16 * 14]   \n"
        "ldr     x30, [sp, %[LR]]          \n"
        "eret"
        :
        : "r"(گره_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#إذا defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
استفادهنشده ساكن ثابت حرف *vect_جابه_جایی_مقصد_name(کلمه_t vect_جابه_جایی)
{
    بدل (vect_جابه_جایی) {
    حالة 0x000:
        أرجع "Synchronous EL1t/EL2t";
    حالة 0x080:
        أرجع "IRQ EL1t/EL2t";
    حالة 0x100:
        أرجع "FIQ EL1t/EL2t";
    حالة 0x180:
        أرجع "SError EL1t/EL2t";
    حالة 0x200:
        أرجع "Synchronous Current EL";
    حالة 0x280:
        أرجع "IRQ Current EL";
    حالة 0x300:
        أرجع "FIQ Current EL";
    حالة 0x380:
        أرجع "SError Current EL";
    حالة 0x400:
        أرجع "Synchronous 64-bit EL0/EL1";
    حالة 0x480:
        أرجع "IRQ 64-bit EL0/EL1";
    حالة 0x500:
        أرجع "FIQ 64-bit EL0/EL1";
    حالة 0x580:
        أرجع "SError 64-bit EL0/EL1";
    حالة 0x600:
        أرجع "Synchronous 32-bit EL0/EL1";
    حالة 0x680:
        أرجع "IRQ 32-bit EL0/EL1";
    حالة 0x700:
        أرجع "FIQ 32-bit EL0/EL1";
    حالة 0x780:
        أرجع "SError 32-bit EL0/EL1";
    افتراضي:
        أرجع "<Unknown>";
    }
}

فراغ VISIBLE c_handle_invalid_وکتور_مدخل(کلمه_t vect_جابه_جایی, کلمه_t pc)
{
    چاپ_کن("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    چاپ_کن("Vector: 0x%"SEL4_PRIx_کلمه" (%s)\n", vect_جابه_جایی, vect_جابه_جایی_مقصد_name(vect_جابه_جایی));
    چاپ_کن("Fault attributed to program counter: 0x%"SEL4_PRIx_کلمه"\n", pc);
    چاپ_کن("ESR: 0x%"SEL4_PRIx_کلمه" FAR: 0x%"SEL4_PRIx_کلمه"\n", getESR(), getFAR());
}
#پایان_اگر
