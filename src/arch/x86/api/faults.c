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

bool_t Arch_handle_Fault_प्रत्युत्तरम्(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault:
        प्रत्यागम true;

    स्वभाव:
        fail("Invalid fault");
    }
}

word_t Arch_समुच्चयः_MRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault: {
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_स्थानाङ्कः(sender->tcbFault));
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    स्वभाव:
        fail("Invalid fault");
    }
}

word_t handleKernelException(
    word_t सदिशम्,
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
    word_t सदिशम्,
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
    यदि (सदिशम् == int_gp_fault && ARCH_ग्रन्थिः_STATE(x86_KSGPExcept_Return_गन्तव्यम्) != 0) {
        word_t ret = ARCH_ग्रन्थिः_STATE(x86_KSGPExcept_Return_गन्तव्यम्);
        ARCH_ग्रन्थिः_STATE(x86_KSGPExcept_Return_गन्तव्यम्) = 0;
        प्रत्यागम ret;
    }
    प्रारूप_लिख("\n========== KERNEL EXCEPTION ==========\n");
    प्रारूप_लिख("Vector:  0x%lx\n", सदिशम्);
    प्रारूप_लिख("ErrCode: 0x%lx\n", errcode);
    प्रारूप_लिख("IP:      0x%lx\n", ip);
    प्रारूप_लिख("SP:      0x%lx\n", sp);
    प्रारूप_लिख("FLAGS:   0x%lx\n", flags);
    प्रारूप_लिख("CR0:     0x%lx\n", cr0);
    प्रारूप_लिख("CR2:     0x%lx (page-fault address)\n", cr2);
    प्रारूप_लिख("CR3:     0x%lx (page-directory physical address)\n", cr3);
    प्रारूप_लिख("CR4:     0x%lx\n", cr4);
    प्रारूप_लिख("\nStack Dump:\n");
    प्रति (i = 0; i < 20; i++) {
        word_t अप्रयुक्तम् राशिः = sp + i * परिमाण(word_t);
        प्रारूप_लिख("*0x%lx == 0x%lx\n", राशिः, *(word_t *)राशिः);
    }
    प्रारूप_लिख("\nHalting...\n");
    halt();
    UNREACHABLE();
}
