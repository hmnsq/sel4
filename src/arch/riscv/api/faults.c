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

bool_t Arch_handle_Fault_جواب(tcb_t *receiver, tcb_t *sender, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

    افتراضي:
        fail("Invalid fault");
    }
}

لفظ_t Arch_سيٽ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پتو(sender->tcbFault));
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}
