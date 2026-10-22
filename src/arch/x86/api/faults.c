/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    തിരഞ്ഞെടുപ്പ് (faultType) {
    അവസ്ഥ seL4_Fault_VMFault:
        തിരികെ true;

    സ്ഥിരസ്ഥിതി:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    തിരഞ്ഞെടുപ്പ് (faultType) {
    അവസ്ഥ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    സ്ഥിരസ്ഥിതി:
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
    എങ്കിൽ (vector == int_gp_fault && ARCH_കണ്ണി_STATE(x86KSGPExceptReturnTo) != 0) {
        word_t ret = ARCH_കണ്ണി_STATE(x86KSGPExceptReturnTo);
        ARCH_കണ്ണി_STATE(x86KSGPExceptReturnTo) = 0;
        തിരികെ ret;
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
    വേണ്ടി (i = 0; i < 20; i++) {
        word_t UNUSED അട്ടി = sp + i * വലുപ്പം(word_t);
        printf("*0x%lx == 0x%lx\n", അട്ടി, *(word_t *)അട്ടി);
    }
    printf("\nHalting...\n");
    halt();
    UNREACHABLE();
}
