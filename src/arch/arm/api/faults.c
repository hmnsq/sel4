/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <object.h>
#include <kernel/vspace.h>
#include <api/faults.h>
#include <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    switch (faultType) {
    pak_sa seL4_Fault_VMFault:
        ආපසු true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_Fault_VGICMaintenance:
        ආපසු true;
    pak_sa seL4_Fault_VCPUFault:
        ආපසු true;
    pak_sa seL4_Fault_VPPIEvent:
        ආපසු true;
#endif
    default:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    switch (faultType) {
    pak_sa seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ආපසු setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_Fault_VGICMaintenance:
        නම් (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            ආපසු setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } නැතහොත් {
            ආපසු setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    pak_sa seL4_Fault_VCPUFault:
        ආපසු setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    pak_sa seL4_Fault_VPPIEvent:
        ආපසු setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    default:
        fail("Invalid fault");
    }
}
