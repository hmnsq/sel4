/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/fastpath/fastpath.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/traps.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>

#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ಶೂನ್ಯ VISIBLE NORETURN restore_user_context(ಶೂನ್ಯ)
{
    c_exit_hook();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ARM_CP14_SAVE_AND_RESTORE_NATIVE_ಎಳೆಗಳು
    restore_user_debug_context(ಗಂಟು_STATE(ksCurThread));
#ಆದರೆ_ಅಂತ್ಯ

    ಗಂಟು_UNLOCK_IF_HELD;

    asm ಅಸ್ಥಿರ(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#ಇಲ್ಲದಿದ್ದರೆ
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#ಆದರೆ_ಅಂತ್ಯ
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
        : "r"(ಗಂಟು_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#ಆದರೆ defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
ಬಳಸದ ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vect_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_name(ಶಬ್ದ_t vect_ಸ್ಥಾನಾಂತರ)
{
    ಆಯ್ಕೆ (vect_ಸ್ಥಾನಾಂತರ) {
    ಸ್ಥಿತಿ 0x000:
        ಹಿಂತಿರುಗಿ "Synchronous EL1t/EL2t";
    ಸ್ಥಿತಿ 0x080:
        ಹಿಂತಿರುಗಿ "IRQ EL1t/EL2t";
    ಸ್ಥಿತಿ 0x100:
        ಹಿಂತಿರುಗಿ "FIQ EL1t/EL2t";
    ಸ್ಥಿತಿ 0x180:
        ಹಿಂತಿರುಗಿ "SError EL1t/EL2t";
    ಸ್ಥಿತಿ 0x200:
        ಹಿಂತಿರುಗಿ "Synchronous Current EL";
    ಸ್ಥಿತಿ 0x280:
        ಹಿಂತಿರುಗಿ "IRQ Current EL";
    ಸ್ಥಿತಿ 0x300:
        ಹಿಂತಿರುಗಿ "FIQ Current EL";
    ಸ್ಥಿತಿ 0x380:
        ಹಿಂತಿರುಗಿ "SError Current EL";
    ಸ್ಥಿತಿ 0x400:
        ಹಿಂತಿರುಗಿ "Synchronous 64-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x480:
        ಹಿಂತಿರುಗಿ "IRQ 64-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x500:
        ಹಿಂತಿರುಗಿ "FIQ 64-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x580:
        ಹಿಂತಿರುಗಿ "SError 64-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x600:
        ಹಿಂತಿರುಗಿ "Synchronous 32-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x680:
        ಹಿಂತಿರುಗಿ "IRQ 32-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x700:
        ಹಿಂತಿರುಗಿ "FIQ 32-bit EL0/EL1";
    ಸ್ಥಿತಿ 0x780:
        ಹಿಂತಿರುಗಿ "SError 32-bit EL0/EL1";
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ "<Unknown>";
    }
}

ಶೂನ್ಯ VISIBLE c_handle_invalid_ಸದಿಶ_ನಮೂದು(ಶಬ್ದ_t vect_ಸ್ಥಾನಾಂತರ, ಶಬ್ದ_t pc)
{
    ಬರೆ("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    ಬರೆ("Vector: 0x%"SEL4_PRIx_ಶಬ್ದ" (%s)\n", vect_ಸ್ಥಾನಾಂತರ, vect_ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_name(vect_ಸ್ಥಾನಾಂತರ));
    ಬರೆ("Fault attributed to program counter: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", pc);
    ಬರೆ("ESR: 0x%"SEL4_PRIx_ಶಬ್ದ" FAR: 0x%"SEL4_PRIx_ಶಬ್ದ"\n", getESR(), getFAR());
}
#ಆದರೆ_ಅಂತ್ಯ
