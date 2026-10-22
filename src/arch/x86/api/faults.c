/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    निवड (faultType) {
    स्थिती seL4_Fault_VMFault:
        परत true;

    पूर्वनिर्धारित:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    निवड (faultType) {
    स्थिती seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        परत setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    पूर्वनिर्धारित:
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
    जर (vector == int_gp_fault && ARCH_गाठ_STATE(x86KSGPExceptReturnTo) != 0) {
        word_t ret = ARCH_गाठ_STATE(x86KSGPExceptReturnTo);
        ARCH_गाठ_STATE(x86KSGPExceptReturnTo) = 0;
        परत ret;
    }
    म_लिखो("\n========== KERNEL EXCEPTION ==========\n");
    म_लिखो("Vector:  0x%lx\n", vector);
    म_लिखो("ErrCode: 0x%lx\n", errcode);
    म_लिखो("IP:      0x%lx\n", ip);
    म_लिखो("SP:      0x%lx\n", sp);
    म_लिखो("FLAGS:   0x%lx\n", flags);
    म_लिखो("CR0:     0x%lx\n", cr0);
    म_लिखो("CR2:     0x%lx (page-fault address)\n", cr2);
    म_लिखो("CR3:     0x%lx (page-directory physical address)\n", cr3);
    म_लिखो("CR4:     0x%lx\n", cr4);
    म_लिखो("\nStack Dump:\n");
    साठी (i = 0; i < 20; i++) {
        word_t UNUSED चळत = sp + i * आकार(word_t);
        म_लिखो("*0x%lx == 0x%lx\n", चळत, *(word_t *)चळत);
    }
    म_लिखो("\nHalting...\n");
    halt();
    UNREACHABLE();
}
