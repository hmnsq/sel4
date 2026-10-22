/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/fastpath/fastpath.h>
#ଅନ୍ତର୍ଭାଵ <arch/kernel/traps.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <linker.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>

#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_track.h>
#ଅନ୍ତର୍ଭାଵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ଶୂନ୍ୟ VISIBLE NORETURN restore_user_context(ଶୂନ୍ୟ)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(ଗଣ୍ଠି_STATE(ksCurThread));
#endif

    ଗଣ୍ଠି_UNLOCK_IF_HELD;

    asm ଅସ୍ଥିର(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#ଅନ୍ୟଥା
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
        : "r"(ଗଣ୍ଠି_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#ଯଦି defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *vect_offset_to_name(word_t vect_offset)
{
    ବାଛ (vect_offset) {
    ସ୍ଥିତି 0x000:
        ଫେରନ୍ତୁ "Synchronous EL1t/EL2t";
    ସ୍ଥିତି 0x080:
        ଫେରନ୍ତୁ "IRQ EL1t/EL2t";
    ସ୍ଥିତି 0x100:
        ଫେରନ୍ତୁ "FIQ EL1t/EL2t";
    ସ୍ଥିତି 0x180:
        ଫେରନ୍ତୁ "SError EL1t/EL2t";
    ସ୍ଥିତି 0x200:
        ଫେରନ୍ତୁ "Synchronous Current EL";
    ସ୍ଥିତି 0x280:
        ଫେରନ୍ତୁ "IRQ Current EL";
    ସ୍ଥିତି 0x300:
        ଫେରନ୍ତୁ "FIQ Current EL";
    ସ୍ଥିତି 0x380:
        ଫେରନ୍ତୁ "SError Current EL";
    ସ୍ଥିତି 0x400:
        ଫେରନ୍ତୁ "Synchronous 64-bit EL0/EL1";
    ସ୍ଥିତି 0x480:
        ଫେରନ୍ତୁ "IRQ 64-bit EL0/EL1";
    ସ୍ଥିତି 0x500:
        ଫେରନ୍ତୁ "FIQ 64-bit EL0/EL1";
    ସ୍ଥିତି 0x580:
        ଫେରନ୍ତୁ "SError 64-bit EL0/EL1";
    ସ୍ଥିତି 0x600:
        ଫେରନ୍ତୁ "Synchronous 32-bit EL0/EL1";
    ସ୍ଥିତି 0x680:
        ଫେରନ୍ତୁ "IRQ 32-bit EL0/EL1";
    ସ୍ଥିତି 0x700:
        ଫେରନ୍ତୁ "FIQ 32-bit EL0/EL1";
    ସ୍ଥିତି 0x780:
        ଫେରନ୍ତୁ "SError 32-bit EL0/EL1";
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ "<Unknown>";
    }
}

ଶୂନ୍ୟ VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
