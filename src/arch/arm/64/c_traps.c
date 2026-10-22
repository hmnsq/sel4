/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <model/statedata.h>
#شامل <arch/fastpath/fastpath.h>
#شامل <arch/kernel/traps.h>
#شامل <api/syscall.h>
#شامل <linker.h>
#شامل <machine/fpu.h>

#شامل <benchmark/benchmark_track.h>
#شامل <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
خالی VISIBLE NORETURN restore_user_context(خالی)
{
    c_exit_hook();

#اگر_تعریف_شدہ ARM_CP14_SAVE_AND_RESTORE_NATIVE_دھاگے
    restore_user_debug_context(گرہ_STATE(ksCurThread));
#اگر_ختم

    گرہ_UNLOCK_IF_HELD;

    asm متلون(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#ورنہ
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#اگر_ختم
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
        : "r"(گرہ_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#اگر defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
غیر_مستعمل ساکن ثابت حرف *vect_انحراف_مقصد_name(لفظ_t vect_انحراف)
{
    انتخاب (vect_انحراف) {
    حالت 0x000:
        واپس "Synchronous EL1t/EL2t";
    حالت 0x080:
        واپس "IRQ EL1t/EL2t";
    حالت 0x100:
        واپس "FIQ EL1t/EL2t";
    حالت 0x180:
        واپس "SError EL1t/EL2t";
    حالت 0x200:
        واپس "Synchronous Current EL";
    حالت 0x280:
        واپس "IRQ Current EL";
    حالت 0x300:
        واپس "FIQ Current EL";
    حالت 0x380:
        واپس "SError Current EL";
    حالت 0x400:
        واپس "Synchronous 64-bit EL0/EL1";
    حالت 0x480:
        واپس "IRQ 64-bit EL0/EL1";
    حالت 0x500:
        واپس "FIQ 64-bit EL0/EL1";
    حالت 0x580:
        واپس "SError 64-bit EL0/EL1";
    حالت 0x600:
        واپس "Synchronous 32-bit EL0/EL1";
    حالت 0x680:
        واپس "IRQ 32-bit EL0/EL1";
    حالت 0x700:
        واپس "FIQ 32-bit EL0/EL1";
    حالت 0x780:
        واپس "SError 32-bit EL0/EL1";
    طےشدہ:
        واپس "<Unknown>";
    }
}

خالی VISIBLE c_handle_invalid_سمتیہ_اندراج(لفظ_t vect_انحراف, لفظ_t pc)
{
    لکھو("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    لکھو("Vector: 0x%"SEL4_PRIx_لفظ" (%s)\n", vect_انحراف, vect_انحراف_مقصد_name(vect_انحراف));
    لکھو("Fault attributed to program counter: 0x%"SEL4_PRIx_لفظ"\n", pc);
    لکھو("ESR: 0x%"SEL4_PRIx_لفظ" FAR: 0x%"SEL4_PRIx_لفظ"\n", getESR(), getFAR());
}
#اگر_ختم
