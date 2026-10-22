/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>

bool_t Arch_handle_Fault_ಉತ್ತರ(tcb_t *receiver, tcb_t *sender, ಶಬ್ದ_t faultType)
{
    ಆಯ್ಕೆ (faultType) {
    ಸ್ಥಿತಿ seL4_Fault_VMFault:
        ಹಿಂತಿರುಗಿ true;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_Fault_VGICMaintenance:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ seL4_Fault_VCPUFault:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ seL4_Fault_VPPIEvent:
        ಹಿಂತಿರುಗಿ true;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid fault");
    }
}

ಶಬ್ದ_t Arch_ಗಣ_MRs_fault(tcb_t *sender, tcb_t *receiver, ಶಬ್ದ_t *receiveIPCBuffer, ಶಬ್ದ_t faultType)
{
    ಆಯ್ಕೆ (faultType) {
    ಸ್ಥಿತಿ seL4_Fault_VMFault: {
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_ವಿಳಾಸ(sender->tcbFault));
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_Fault_VGICMaintenance:
        ಆದರೆ (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    ಸ್ಥಿತಿ seL4_Fault_VCPUFault:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    ಸ್ಥಿತಿ seL4_Fault_VPPIEvent:
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid fault");
    }
}
