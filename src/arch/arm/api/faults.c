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

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        أرجع true;
    حالة seL4_Fault_VCPUFault:
        أرجع true;
    حالة seL4_Fault_VPPIEvent:
        أرجع true;
#جے_انت
    افتراضي:
        fail("Invalid fault");
    }
}

لفظ_t Arch_سیٹ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پتہ(sender->tcbFault));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        إذا (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } وإلا {
            أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالة seL4_Fault_VCPUFault:
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالة seL4_Fault_VPPIEvent:
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#جے_انت

    افتراضي:
        fail("Invalid fault");
    }
}
