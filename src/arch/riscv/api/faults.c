/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <api/syscall.h>

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
