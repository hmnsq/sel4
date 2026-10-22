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
تهی VISIBLE NORETURN restore_user_context(تهی)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(گره_STATE(ksCurThread));
#endif

    گره_UNLOCK_IF_HELD;

    asm فرار(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#وگرنه
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#endif
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

#اگر defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED ایستا ثابت نویسه *vect_offset_to_name(word_t vect_offset)
{
    انتخاب (vect_offset) {
    حالت 0x000:
        بازگشت "Synchronous EL1t/EL2t";
    حالت 0x080:
        بازگشت "IRQ EL1t/EL2t";
    حالت 0x100:
        بازگشت "FIQ EL1t/EL2t";
    حالت 0x180:
        بازگشت "SError EL1t/EL2t";
    حالت 0x200:
        بازگشت "Synchronous Current EL";
    حالت 0x280:
        بازگشت "IRQ Current EL";
    حالت 0x300:
        بازگشت "FIQ Current EL";
    حالت 0x380:
        بازگشت "SError Current EL";
    حالت 0x400:
        بازگشت "Synchronous 64-bit EL0/EL1";
    حالت 0x480:
        بازگشت "IRQ 64-bit EL0/EL1";
    حالت 0x500:
        بازگشت "FIQ 64-bit EL0/EL1";
    حالت 0x580:
        بازگشت "SError 64-bit EL0/EL1";
    حالت 0x600:
        بازگشت "Synchronous 32-bit EL0/EL1";
    حالت 0x680:
        بازگشت "IRQ 32-bit EL0/EL1";
    حالت 0x700:
        بازگشت "FIQ 32-bit EL0/EL1";
    حالت 0x780:
        بازگشت "SError 32-bit EL0/EL1";
    پیشفرض:
        بازگشت "<Unknown>";
    }
}

تهی VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
