/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <arch/fastpath/fastpath.h>
#അന്തര്ഭാവ <arch/kernel/traps.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <machine/fpu.h>

#അന്തര്ഭാവ <benchmark/benchmark_track.h>
#അന്തര്ഭാവ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ശൂന്യം VISIBLE NORETURN restore_user_context(ശൂന്യം)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(കണ്ണി_STATE(ksCurThread));
#endif

    കണ്ണി_UNLOCK_IF_HELD;

    asm അസ്ഥിര(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#അല്ലെങ്കിൽ
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
        : "r"(കണ്ണി_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#എങ്കിൽ defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED സ്ഥിതം സ്ഥിരം അക്ഷരം *vect_offset_to_name(word_t vect_offset)
{
    തിരഞ്ഞെടുപ്പ് (vect_offset) {
    അവസ്ഥ 0x000:
        തിരികെ "Synchronous EL1t/EL2t";
    അവസ്ഥ 0x080:
        തിരികെ "IRQ EL1t/EL2t";
    അവസ്ഥ 0x100:
        തിരികെ "FIQ EL1t/EL2t";
    അവസ്ഥ 0x180:
        തിരികെ "SError EL1t/EL2t";
    അവസ്ഥ 0x200:
        തിരികെ "Synchronous Current EL";
    അവസ്ഥ 0x280:
        തിരികെ "IRQ Current EL";
    അവസ്ഥ 0x300:
        തിരികെ "FIQ Current EL";
    അവസ്ഥ 0x380:
        തിരികെ "SError Current EL";
    അവസ്ഥ 0x400:
        തിരികെ "Synchronous 64-bit EL0/EL1";
    അവസ്ഥ 0x480:
        തിരികെ "IRQ 64-bit EL0/EL1";
    അവസ്ഥ 0x500:
        തിരികെ "FIQ 64-bit EL0/EL1";
    അവസ്ഥ 0x580:
        തിരികെ "SError 64-bit EL0/EL1";
    അവസ്ഥ 0x600:
        തിരികെ "Synchronous 32-bit EL0/EL1";
    അവസ്ഥ 0x680:
        തിരികെ "IRQ 32-bit EL0/EL1";
    അവസ്ഥ 0x700:
        തിരികെ "FIQ 32-bit EL0/EL1";
    അവസ്ഥ 0x780:
        തിരികെ "SError 32-bit EL0/EL1";
    സ്ഥിരസ്ഥിതി:
        തിരികെ "<Unknown>";
    }
}

ശൂന്യം VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
