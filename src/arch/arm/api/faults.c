/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <object.h>
#شامل <kernel/vspace.h>
#شامل <api/faults.h>
#شامل <api/syscall.h>

bool_t Arch_handle_Fault_جواب(tcb_t *receiver, tcb_t *sender, لفظ_t faultType)
{
    انتخاب (faultType) {
    حالت seL4_Fault_VMFault:
        واپس true;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_Fault_VGICMaintenance:
        واپس true;
    حالت seL4_Fault_VCPUFault:
        واپس true;
    حالت seL4_Fault_VPPIEvent:
        واپس true;
#اگر_ختم
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

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_Fault_VGICMaintenance:
        اگر (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            واپس سیٹ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } ورنہ {
            واپس سیٹ_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالت seL4_Fault_VCPUFault:
        واپس سیٹ_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالت seL4_Fault_VPPIEvent:
        واپس سیٹ_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#اگر_ختم

    طےشدہ:
        fail("Invalid fault");
    }
}
