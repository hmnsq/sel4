/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <object.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <api/faults.h>
#અન્તર્ભાવ <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    પસંદગી (faultType) {
    સ્થિતિ seL4_Fault_VMFault:
        પાછા true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ seL4_Fault_VGICMaintenance:
        પાછા true;
    સ્થિતિ seL4_Fault_VCPUFault:
        પાછા true;
    સ્થિતિ seL4_Fault_VPPIEvent:
        પાછા true;
#endif
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

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ seL4_Fault_VGICMaintenance:
        જો (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            પાછા setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } નહીંતર {
            પાછા setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    સ્થિતિ seL4_Fault_VCPUFault:
        પાછા setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    સ્થિતિ seL4_Fault_VPPIEvent:
        પાછા setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    મૂળભૂત:
        fail("Invalid fault");
    }
}
