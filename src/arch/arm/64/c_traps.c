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
    restore_user_debug_context(गाठ_STATE(ksCurThread));
#endif

    गाठ_UNLOCK_IF_HELD;

    यंत्र अस्थिर(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#अन्यथा
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
        : "r"(गाठ_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#जर defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED स्थिर स्थिरांक अक्षर *vect_offset_to_name(word_t vect_offset)
{
    निवड (vect_offset) {
    स्थिती 0x000:
        परत "Synchronous EL1t/EL2t";
    स्थिती 0x080:
        परत "IRQ EL1t/EL2t";
    स्थिती 0x100:
        परत "FIQ EL1t/EL2t";
    स्थिती 0x180:
        परत "SError EL1t/EL2t";
    स्थिती 0x200:
        परत "Synchronous Current EL";
    स्थिती 0x280:
        परत "IRQ Current EL";
    स्थिती 0x300:
        परत "FIQ Current EL";
    स्थिती 0x380:
        परत "SError Current EL";
    स्थिती 0x400:
        परत "Synchronous 64-bit EL0/EL1";
    स्थिती 0x480:
        परत "IRQ 64-bit EL0/EL1";
    स्थिती 0x500:
        परत "FIQ 64-bit EL0/EL1";
    स्थिती 0x580:
        परत "SError 64-bit EL0/EL1";
    स्थिती 0x600:
        परत "Synchronous 32-bit EL0/EL1";
    स्थिती 0x680:
        परत "IRQ 32-bit EL0/EL1";
    स्थिती 0x700:
        परत "FIQ 32-bit EL0/EL1";
    स्थिती 0x780:
        परत "SError 32-bit EL0/EL1";
    पूर्वनिर्धारित:
        परत "<Unknown>";
    }
}

शून्य VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    म_लिखो("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    म_लिखो("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    म_लिखो("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    म_लिखो("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
