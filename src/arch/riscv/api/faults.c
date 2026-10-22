/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

bool_t Arch_handle_Fault_ਜਵਾਬ(tcb_t *receiver, tcb_t *sender, ਸ਼ਬਦ_t faultType)
{
    ਚੋਣ (faultType) {
    ਹਾਲਤ seL4_Fault_VMFault:
        ਵਾਪਸ true;

    ਮੂਲ:
        fail("Invalid fault");
    }
}

ਸ਼ਬਦ_t Arch_ਸੈੱਟ_MRs_fault(tcb_t *sender, tcb_t *receiver, ਸ਼ਬਦ_t *receiveIPCBuffer, ਸ਼ਬਦ_t faultType)
{
    ਚੋਣ (faultType) {
    ਹਾਲਤ seL4_Fault_VMFault: {
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_ਪਤਾ(sender->tcbFault));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    ਮੂਲ:
        fail("Invalid fault");
    }
}
