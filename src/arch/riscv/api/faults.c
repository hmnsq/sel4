/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <types.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <api/syscall.h>

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
