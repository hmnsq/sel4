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

bool_t Arch_handle_Fault_प्रत्युत्तरम्(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault:
        प्रत्यागम true;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_Fault_VGICMaintenance:
        प्रत्यागम true;
    पक्ष seL4_Fault_VCPUFault:
        प्रत्यागम true;
    पक्ष seL4_Fault_VPPIEvent:
        प्रत्यागम true;
#यदि_अन्तः
    स्वभाव:
        fail("Invalid fault");
    }
}

word_t Arch_समुच्चयः_MRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    विकल्प (faultType) {
    पक्ष seL4_Fault_VMFault: {
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_स्थानाङ्कः(sender->tcbFault));
        समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_Fault_VGICMaintenance:
        यदि (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } अन्यथा {
            प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    पक्ष seL4_Fault_VCPUFault:
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    पक्ष seL4_Fault_VPPIEvent:
        प्रत्यागम समुच्चयः_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#यदि_अन्तः

    स्वभाव:
        fail("Invalid fault");
    }
}
