/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <model/statedata.h>
#समावेश <arch/fastpath/fastpath.h>
#समावेश <arch/kernel/traps.h>
#समावेश <api/syscall.h>
#समावेश <linker.h>
#समावेश <machine/fpu.h>

#समावेश <benchmark/benchmark_track.h>
#समावेश <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
व्योम VISIBLE NORETURN restore_user_context(व्योम)
{
    c_exit_hook();

#अगर_घोषित ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(गाँठ_STATE(ksCurThread));
#पूर्ण_अगर

    गाँठ_UNLOCK_IF_HELD;

    यंत्र अस्थिर(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#अन्यथा
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#पूर्ण_अगर
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
        : "r"(गाँठ_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#अगर defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED अटल स्थिर अक्षर *vect_offset_to_name(word_t vect_offset)
{
    चयन (vect_offset) {
    हाल 0x000:
        वापस "Synchronous EL1t/EL2t";
    हाल 0x080:
        वापस "IRQ EL1t/EL2t";
    हाल 0x100:
        वापस "FIQ EL1t/EL2t";
    हाल 0x180:
        वापस "SError EL1t/EL2t";
    हाल 0x200:
        वापस "Synchronous Current EL";
    हाल 0x280:
        वापस "IRQ Current EL";
    हाल 0x300:
        वापस "FIQ Current EL";
    हाल 0x380:
        वापस "SError Current EL";
    हाल 0x400:
        वापस "Synchronous 64-bit EL0/EL1";
    हाल 0x480:
        वापस "IRQ 64-bit EL0/EL1";
    हाल 0x500:
        वापस "FIQ 64-bit EL0/EL1";
    हाल 0x580:
        वापस "SError 64-bit EL0/EL1";
    हाल 0x600:
        वापस "Synchronous 32-bit EL0/EL1";
    हाल 0x680:
        वापस "IRQ 32-bit EL0/EL1";
    हाल 0x700:
        वापस "FIQ 32-bit EL0/EL1";
    हाल 0x780:
        वापस "SError 32-bit EL0/EL1";
    शेष:
        वापस "<Unknown>";
    }
}

व्योम VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    म_लिखो("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    म_लिखो("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    म_लिखो("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#पूर्ण_अगर
