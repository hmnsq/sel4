/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <api/syscall.h>

#அந்தர்பாவ <types.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    தேர்வு (faultType) {
    நிலை seL4_Fault_VMFault:
        திரும்பு true;

    இயல்பு:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    தேர்வு (faultType) {
    நிலை seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    இயல்பு:
        fail("Invalid fault");
    }
}
