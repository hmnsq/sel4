/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <object.h>
#شامل <kernel/vspace.h>
#شامل <api/faults.h>
#شامل <api/syscall.h>

#شامل <types.h>
#شامل <machine/io.h>
#شامل <api/faults.h>
#شامل <api/syscall.h>
#شامل <util.h>

bool_t Arch_handle_Fault_جواب(tcb_t *receiver, tcb_t *sender, لفظ_t faultType)
{
    انتخاب (faultType) {
    حالت seL4_Fault_VMFault:
        واپس true;

    طےشدہ:
        fail("Invalid fault");
    }
}

لفظ_t Arch_سیٹ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer, لفظ_t faultType)
{
    انتخاب (faultType) {
    حالت seL4_Fault_VMFault: {
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پتہ(sender->tcbFault));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        واپس سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    طےشدہ:
        fail("Invalid fault");
    }
}
