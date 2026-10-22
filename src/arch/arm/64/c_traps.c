/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/fastpath/fastpath.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/traps.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_track.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
ਖਾਲੀ VISIBLE NORETURN restore_user_context(ਖਾਲੀ)
{
    c_exit_hook();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ARM_CP14_SAVE_AND_RESTORE_NATIVE_ਧਾਗੇ
    restore_user_debug_context(ਗੰਢ_STATE(ksCurThread));
#ਜੇ_ਅੰਤ

    ਗੰਢ_UNLOCK_IF_HELD;

    asm ਅਸਥਿਰ(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#ਨਹੀਂਤਾਂ
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#ਜੇ_ਅੰਤ
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
        : "r"(ਗੰਢ_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#ਜੇ defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
ਅਣਵਰਤਿਆ ਸਥਿਰ ਅਚਲ ਅੱਖਰ *vect_ਵਿੱਥ_ਮੰਜ਼ਿਲ_name(ਸ਼ਬਦ_t vect_ਵਿੱਥ)
{
    ਚੋਣ (vect_ਵਿੱਥ) {
    ਹਾਲਤ 0x000:
        ਵਾਪਸ "Synchronous EL1t/EL2t";
    ਹਾਲਤ 0x080:
        ਵਾਪਸ "IRQ EL1t/EL2t";
    ਹਾਲਤ 0x100:
        ਵਾਪਸ "FIQ EL1t/EL2t";
    ਹਾਲਤ 0x180:
        ਵਾਪਸ "SError EL1t/EL2t";
    ਹਾਲਤ 0x200:
        ਵਾਪਸ "Synchronous Current EL";
    ਹਾਲਤ 0x280:
        ਵਾਪਸ "IRQ Current EL";
    ਹਾਲਤ 0x300:
        ਵਾਪਸ "FIQ Current EL";
    ਹਾਲਤ 0x380:
        ਵਾਪਸ "SError Current EL";
    ਹਾਲਤ 0x400:
        ਵਾਪਸ "Synchronous 64-bit EL0/EL1";
    ਹਾਲਤ 0x480:
        ਵਾਪਸ "IRQ 64-bit EL0/EL1";
    ਹਾਲਤ 0x500:
        ਵਾਪਸ "FIQ 64-bit EL0/EL1";
    ਹਾਲਤ 0x580:
        ਵਾਪਸ "SError 64-bit EL0/EL1";
    ਹਾਲਤ 0x600:
        ਵਾਪਸ "Synchronous 32-bit EL0/EL1";
    ਹਾਲਤ 0x680:
        ਵਾਪਸ "IRQ 32-bit EL0/EL1";
    ਹਾਲਤ 0x700:
        ਵਾਪਸ "FIQ 32-bit EL0/EL1";
    ਹਾਲਤ 0x780:
        ਵਾਪਸ "SError 32-bit EL0/EL1";
    ਮੂਲ:
        ਵਾਪਸ "<Unknown>";
    }
}

ਖਾਲੀ VISIBLE c_handle_invalid_ਸਦਿਸ਼_ਇੰਦਰਾਜ(ਸ਼ਬਦ_t vect_ਵਿੱਥ, ਸ਼ਬਦ_t pc)
{
    ਲਿਖੋ("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    ਲਿਖੋ("Vector: 0x%"SEL4_PRIx_ਸ਼ਬਦ" (%s)\n", vect_ਵਿੱਥ, vect_ਵਿੱਥ_ਮੰਜ਼ਿਲ_name(vect_ਵਿੱਥ));
    ਲਿਖੋ("Fault attributed to program counter: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", pc);
    ਲਿਖੋ("ESR: 0x%"SEL4_PRIx_ਸ਼ਬਦ" FAR: 0x%"SEL4_PRIx_ਸ਼ਬਦ"\n", getESR(), getFAR());
}
#ਜੇ_ਅੰਤ
