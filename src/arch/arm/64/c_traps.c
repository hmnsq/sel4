/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <arch/fastpath/fastpath.h>
#अन्तर्भाव <arch/kernel/traps.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <machine/fpu.h>

#अन्तर्भाव <benchmark/benchmark_track.h>
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
शून्य VISIBLE NORETURN restore_user_context(शून्य)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(ग्रन्थिः_STATE(ksCurThread));
#endif

    ग्रन्थिः_UNLOCK_IF_HELD;

    asm चञ्चल(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#अञ्ञथा
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
        : "r"(ग्रन्थिः_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#सचे defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED स्थिर ध्रुव वर्ण *vect_offset_to_name(word_t vect_offset)
{
    विकल्प (vect_offset) {
    पक्ष 0x000:
        पटिनिवत्त "Synchronous EL1t/EL2t";
    पक्ष 0x080:
        पटिनिवत्त "IRQ EL1t/EL2t";
    पक्ष 0x100:
        पटिनिवत्त "FIQ EL1t/EL2t";
    पक्ष 0x180:
        पटिनिवत्त "SError EL1t/EL2t";
    पक्ष 0x200:
        पटिनिवत्त "Synchronous Current EL";
    पक्ष 0x280:
        पटिनिवत्त "IRQ Current EL";
    पक्ष 0x300:
        पटिनिवत्त "FIQ Current EL";
    पक्ष 0x380:
        पटिनिवत्त "SError Current EL";
    पक्ष 0x400:
        पटिनिवत्त "Synchronous 64-bit EL0/EL1";
    पक्ष 0x480:
        पटिनिवत्त "IRQ 64-bit EL0/EL1";
    पक्ष 0x500:
        पटिनिवत्त "FIQ 64-bit EL0/EL1";
    पक्ष 0x580:
        पटिनिवत्त "SError 64-bit EL0/EL1";
    पक्ष 0x600:
        पटिनिवत्त "Synchronous 32-bit EL0/EL1";
    पक्ष 0x680:
        पटिनिवत्त "IRQ 32-bit EL0/EL1";
    पक्ष 0x700:
        पटिनिवत्त "FIQ 32-bit EL0/EL1";
    पक्ष 0x780:
        पटिनिवत्त "SError 32-bit EL0/EL1";
    स्वभाव:
        पटिनिवत्त "<Unknown>";
    }
}

शून्य VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
