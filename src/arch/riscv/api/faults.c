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

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault:
        पटिनिवत्त true;

    स्वभाव:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        पटिनिवत्त setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    स्वभाव:
        fail("Invalid fault");
    }
}
