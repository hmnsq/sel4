/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <object.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <api/faults.h>
#अन्तर्भाव <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault:
        प्रत्यागम true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_Fault_VGICMaintenance:
        प्रत्यागम true;
    पक्ष seL4_Fault_VCPUFault:
        प्रत्यागम true;
    पक्ष seL4_Fault_VPPIEvent:
        प्रत्यागम true;
#endif
    स्वभाव:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_Fault_VGICMaintenance:
        यदि (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } अन्यथा {
            प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    पक्ष seL4_Fault_VCPUFault:
        प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    पक्ष seL4_Fault_VPPIEvent:
        प्रत्यागम setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    स्वभाव:
        fail("Invalid fault");
    }
}
