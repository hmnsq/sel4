/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>

bool_t Arch_handle_Fault_ਜਵਾਬ(tcb_t *receiver, tcb_t *sender, ਸ਼ਬਦ_t faultType)
{
    ਚੋਣ (faultType) {
    ਹਾਲਤ seL4_Fault_VMFault:
        ਵਾਪਸ true;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_Fault_VGICMaintenance:
        ਵਾਪਸ true;
    ਹਾਲਤ seL4_Fault_VCPUFault:
        ਵਾਪਸ true;
    ਹਾਲਤ seL4_Fault_VPPIEvent:
        ਵਾਪਸ true;
#ਜੇ_ਅੰਤ
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_Fault_VGICMaintenance:
        ਜੇ (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    ਹਾਲਤ seL4_Fault_VCPUFault:
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    ਹਾਲਤ seL4_Fault_VPPIEvent:
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        fail("Invalid fault");
    }
}
