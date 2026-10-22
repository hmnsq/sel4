/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <api/faults.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    ৱিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault:
        ঘূৰাই true;

    স্ৱভাৱ:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    ৱিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ঘূৰাই setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    স্ৱভাৱ:
        fail("Invalid fault");
    }
}

word_t handleKernelException(
    word_t vector,
    word_t errcode,
    word_t ip,
    word_t sp,
    word_t flags,
    word_t cr0,
    word_t cr2,
    word_t cr3,
    word_t cr4
);

VISIBLE
word_t handleKernelException(
    word_t vector,
    word_t errcode,
    word_t ip,
    word_t sp,
    word_t flags,
    word_t cr0,
    word_t cr2,
    word_t cr3,
    word_t cr4
)
{
    word_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    যদি (vector == int_gp_fault && ARCH_গ্ৰন্থি_STATE(x86KSGPExceptReturnTo) != 0) {
        word_t ret = ARCH_গ্ৰন্থি_STATE(x86KSGPExceptReturnTo);
        ARCH_গ্ৰন্থি_STATE(x86KSGPExceptReturnTo) = 0;
        ঘূৰাই ret;
    }
    printf("\n========== KERNEL EXCEPTION ==========\n");
    printf("Vector:  0x%lx\n", vector);
    printf("ErrCode: 0x%lx\n", errcode);
    printf("IP:      0x%lx\n", ip);
    printf("SP:      0x%lx\n", sp);
    printf("FLAGS:   0x%lx\n", flags);
    printf("CR0:     0x%lx\n", cr0);
    printf("CR2:     0x%lx (page-fault address)\n", cr2);
    printf("CR3:     0x%lx (page-directory physical address)\n", cr3);
    printf("CR4:     0x%lx\n", cr4);
    printf("\nStack Dump:\n");
    বাবে (i = 0; i < 20; i++) {
        word_t UNUSED স্তূপ = sp + i * পরিমাণ(word_t);
        printf("*0x%lx == 0x%lx\n", স্তূপ, *(word_t *)স্তূপ);
    }
    printf("\nHalting...\n");
    halt();
    UNREACHABLE();
}
