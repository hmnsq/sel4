/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <object.h>
#ضمن <kernel/vspace.h>
#ضمن <api/faults.h>
#ضمن <api/syscall.h>

#ضمن <types.h>
#ضمن <machine/io.h>
#ضمن <api/faults.h>
#ضمن <api/syscall.h>
#ضمن <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

    افتراضي:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}
