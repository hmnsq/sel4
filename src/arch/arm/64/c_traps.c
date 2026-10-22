/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/fastpath/fastpath.h>
#అన్తర్భావ <arch/kernel/traps.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <machine/fpu.h>

#అన్తర్భావ <benchmark/benchmark_track.h>
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

/** DONT_TRANSLATE */
శూన్య VISIBLE NORETURN restore_user_context(శూన్య)
{
    c_exit_hook();

#నిర్వచితమైతే ARM_CP14_SAVE_AND_RESTORE_NATIVE_దారాలు
    restore_user_debug_context(కణుపు_STATE(ksCurThread));
#అయితే_ముగింపు

    కణుపు_UNLOCK_IF_HELD;

    asm అస్థిర(
        "mov     sp, %0                     \n"

        /* Restore thread's SPSR, LR, and SP */
        "ldp     x21, x22, [sp, %[SP_EL0]] \n"
        "ldr     x23, [sp, %[SPSR_EL1]]    \n"
        "msr     sp_el0, x21                \n"
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        "msr     elr_el2, x22               \n"
        "msr     spsr_el2, x23              \n"
#లేకపోతే
        "msr     elr_el1, x22               \n"
        "msr     spsr_el1, x23              \n"
#అయితే_ముగింపు
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
        : "r"(కణుపు_STATE(ksCurThread)->tcbArch.tcbContext.registers),
        [SP_EL0] "i"(PT_SP_EL0), [SPSR_EL1] "i"(PT_SPSR_EL1), [LR] "i"(PT_LR)
        : "memory"
    );
    UNREACHABLE();
}

#ఒకవేళ defined(CONFIG_DEBUG_BUILD)
/* See 'arm_vector_table' for details on 'vect_offset' */
వాడని స్థిర స్థిరరాశి అక్షరం *vect_స్థానభ్రంశం_గమ్యం_name(మాట_t vect_స్థానభ్రంశం)
{
    ఎంపిక (vect_స్థానభ్రంశం) {
    స్థితి 0x000:
        తిరిగి "Synchronous EL1t/EL2t";
    స్థితి 0x080:
        తిరిగి "IRQ EL1t/EL2t";
    స్థితి 0x100:
        తిరిగి "FIQ EL1t/EL2t";
    స్థితి 0x180:
        తిరిగి "SError EL1t/EL2t";
    స్థితి 0x200:
        తిరిగి "Synchronous Current EL";
    స్థితి 0x280:
        తిరిగి "IRQ Current EL";
    స్థితి 0x300:
        తిరిగి "FIQ Current EL";
    స్థితి 0x380:
        తిరిగి "SError Current EL";
    స్థితి 0x400:
        తిరిగి "Synchronous 64-bit EL0/EL1";
    స్థితి 0x480:
        తిరిగి "IRQ 64-bit EL0/EL1";
    స్థితి 0x500:
        తిరిగి "FIQ 64-bit EL0/EL1";
    స్థితి 0x580:
        తిరిగి "SError 64-bit EL0/EL1";
    స్థితి 0x600:
        తిరిగి "Synchronous 32-bit EL0/EL1";
    స్థితి 0x680:
        తిరిగి "IRQ 32-bit EL0/EL1";
    స్థితి 0x700:
        తిరిగి "FIQ 32-bit EL0/EL1";
    స్థితి 0x780:
        తిరిగి "SError 32-bit EL0/EL1";
    అప్రమేయ:
        తిరిగి "<Unknown>";
    }
}

శూన్య VISIBLE c_handle_invalid_సదిశ_నమోదు(మాట_t vect_స్థానభ్రంశం, మాట_t pc)
{
    రాయి("\n\nKERNEL INVALID VECTOR ENTRY!\n");
    రాయి("Vector: 0x%"SEL4_PRIx_మాట" (%s)\n", vect_స్థానభ్రంశం, vect_స్థానభ్రంశం_గమ్యం_name(vect_స్థానభ్రంశం));
    రాయి("Fault attributed to program counter: 0x%"SEL4_PRIx_మాట"\n", pc);
    రాయి("ESR: 0x%"SEL4_PRIx_మాట" FAR: 0x%"SEL4_PRIx_మాట"\n", getESR(), getFAR());
}
#అయితే_ముగింపు
