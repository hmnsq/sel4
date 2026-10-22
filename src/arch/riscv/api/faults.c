/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/syscall.h>

#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <util.h>

bool_t Arch_handle_Fault_జవాబు(tcb_t *receiver, tcb_t *sender, మాట_t faultType)
{
    ఎంపిక (faultType) {
    స్థితి seL4_Fault_VMFault:
        తిరిగి true;

    అప్రమేయ:
        fail("Invalid fault");
    }
}

మాట_t Arch_సమితి_MRs_fault(tcb_t *sender, tcb_t *receiver, మాట_t *receiveIPCBuffer, మాట_t faultType)
{
    ఎంపిక (faultType) {
    స్థితి seL4_Fault_VMFault: {
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_చిరునామా(sender->tcbFault));
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    అప్రమేయ:
        fail("Invalid fault");
    }
}
