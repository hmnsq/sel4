/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/fastpath/fastpath.h>
#அந்தர்பாவ <arch/kernel/traps.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <linker.h>
#அந்தர்பாவ <machine/fpu.h>

#அந்தர்பாவ <benchmark/benchmark_track.h>
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
வெறுமை VISIBLE NORETURN restore_user_context(வெறுமை)
{
    c_exit_hook();

#ifdef ARM_CP14_SAVE_AND_RESTORE_NATIVE_THREADS
    restore_user_debug_context(முனை_STATE(ksCurThread));
#endif

    முனை_UNLOCK_IF_HELD;

    asm நிலையற்ற(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#இல்லையெனில்
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
        : "r"(முனை_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#எனில் defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
UNUSED நிலையான மாறிலி எழுத்து *vect_offset_to_name(word_t vect_offset)
{
    தேர்வு (vect_offset) {
    நிலை 0x000:
        திரும்பு "Synchronous EL1t/EL2t";
    நிலை 0x080:
        திரும்பு "IRQ EL1t/EL2t";
    நிலை 0x100:
        திரும்பு "FIQ EL1t/EL2t";
    நிலை 0x180:
        திரும்பு "SError EL1t/EL2t";
    நிலை 0x200:
        திரும்பு "Synchronous Current EL";
    நிலை 0x280:
        திரும்பு "IRQ Current EL";
    நிலை 0x300:
        திரும்பு "FIQ Current EL";
    நிலை 0x380:
        திரும்பு "SError Current EL";
    நிலை 0x400:
        திரும்பு "Synchronous 64-bit EL0/EL1";
    நிலை 0x480:
        திரும்பு "IRQ 64-bit EL0/EL1";
    நிலை 0x500:
        திரும்பு "FIQ 64-bit EL0/EL1";
    நிலை 0x580:
        திரும்பு "SError 64-bit EL0/EL1";
    நிலை 0x600:
        திரும்பு "Synchronous 32-bit EL0/EL1";
    நிலை 0x680:
        திரும்பு "IRQ 32-bit EL0/EL1";
    நிலை 0x700:
        திரும்பு "FIQ 32-bit EL0/EL1";
    நிலை 0x780:
        திரும்பு "SError 32-bit EL0/EL1";
    இயல்பு:
        திரும்பு "<Unknown>";
    }
}

வெறுமை VISIBLE c_handle_invalid_vector_entry(word_t vect_offset, word_t pc)
{
    printf("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    printf("Vector: 0x%"SEL4_PRIx_word" (%s)\n", vect_offset, vect_offset_to_name(vect_offset));
    printf("Fault attributed to program counter: 0x%"SEL4_PRIx_word"\n", pc);
    printf("ESR: 0x%"SEL4_PRIx_word" FAR: 0x%"SEL4_PRIx_word"\n", getESR(), getFAR());
}
#endif
