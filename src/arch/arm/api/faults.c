/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <object.h>
#ضمن <kernel/vspace.h>
#ضمن <api/faults.h>
#ضمن <api/syscall.h>

bool_t Arch_handle_Fault_جواب(tcb_t *receiver, tcb_t *sender, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        أرجع true;
    حالة seL4_Fault_VCPUFault:
        أرجع true;
    حالة seL4_Fault_VPPIEvent:
        أرجع true;
#جيڪڏهن_پورو
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

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        إذا (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } وإلا {
            أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالة seL4_Fault_VCPUFault:
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالة seL4_Fault_VPPIEvent:
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#جيڪڏهن_پورو

    افتراضي:
        fail("Invalid fault");
    }
}
