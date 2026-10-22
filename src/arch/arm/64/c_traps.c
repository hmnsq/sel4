/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <model/statedata.h>
#অন্তর্ভাৱ <arch/fastpath/fastpath.h>
#অন্তর্ভাৱ <arch/kernel/traps.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <machine/fpu.h>

#অন্তর্ভাৱ <benchmark/benchmark_track.h>
#অন্তর্ভাৱ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
শূন্য VISIBLE NORETURN restore_user_context(শূন্য)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(গ্ৰন্থি_STATE(ksCurThread));
#endif

    গ্ৰন্থি_UNLOCK_IF_HELD;

    asm চঞ্চল(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#অন্যথা
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
        : "r"(গ্ৰন্থি_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#যদি defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED স্থির ধ্রুৱ ৱর্ণ *vect_offset_to_name(word_t vect_offset)
{
    ৱিকল্প (vect_offset) {
    পক্ষ 0x000:
        ঘূৰাই "Synchronous EL1t/EL2t";
    পক্ষ 0x080:
        ঘূৰাই "IRQ EL1t/EL2t";
    পক্ষ 0x100:
        ঘূৰাই "FIQ EL1t/EL2t";
    পক্ষ 0x180:
        ঘূৰাই "SError EL1t/EL2t";
    পক্ষ 0x200:
        ঘূৰাই "Synchronous Current EL";
    পক্ষ 0x280:
        ঘূৰাই "IRQ Current EL";
    পক্ষ 0x300:
        ঘূৰাই "FIQ Current EL";
    পক্ষ 0x380:
        ঘূৰাই "SError Current EL";
    পক্ষ 0x400:
        ঘূৰাই "Synchronous 64-bit EL0/EL1";
    পক্ষ 0x480:
        ঘূৰাই "IRQ 64-bit EL0/EL1";
    পক্ষ 0x500:
        ঘূৰাই "FIQ 64-bit EL0/EL1";
    পক্ষ 0x580:
        ঘূৰাই "SError 64-bit EL0/EL1";
    পক্ষ 0x600:
        ঘূৰাই "Synchronous 32-bit EL0/EL1";
    পক্ষ 0x680:
        ঘূৰাই "IRQ 32-bit EL0/EL1";
    পক্ষ 0x700:
        ঘূৰাই "FIQ 32-bit EL0/EL1";
    পক্ষ 0x780:
        ঘূৰাই "SError 32-bit EL0/EL1";
    স্ৱভাৱ:
        ঘূৰাই "<Unknown>";
    }
}

শূন্য VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
