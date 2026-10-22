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

bool_t Arch_handle_Fault_پاسخ(tcb_t *receiver, tcb_t *sender, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        أرجع true;
    حالة seL4_Fault_VCPUFault:
        أرجع true;
    حالة seL4_Fault_VPPIEvent:
        أرجع true;
#پایان_اگر
    افتراضي:
        fail("Invalid fault");
    }
}

کلمه_t Arch_مجموعه_MRs_fault(tcb_t *sender, tcb_t *receiver, کلمه_t *receiveIPCBuffer, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_آدرس(sender->tcbFault));
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        إذا (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } وإلا {
            أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالة seL4_Fault_VCPUFault:
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالة seL4_Fault_VPPIEvent:
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#پایان_اگر

    افتراضي:
        fail("Invalid fault");
    }
}
