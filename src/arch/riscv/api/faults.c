/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <api/syscall.h>

#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <api/syscall.h>
#અન્તર્ભાવ <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    પસંદગી (faultType) {
    સ્થિતિ seL4_Fault_VMFault:
        પાછા true;

    મૂળભૂત:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    પસંદગી (faultType) {
    સ્થિતિ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        પાછા setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    મૂળભૂત:
        fail("Invalid fault");
    }
}
