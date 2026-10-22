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

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        إرجاع true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        إرجاع true;
    حالة seL4_Fault_VCPUFault:
        إرجاع true;
    حالة seL4_Fault_VPPIEvent:
        إرجاع true;
#endif
    افتراضي:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        إرجاع setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_Fault_VGICMaintenance:
        إذا (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            إرجاع setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } وإلا {
            إرجاع setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالة seL4_Fault_VCPUFault:
        إرجاع setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالة seL4_Fault_VPPIEvent:
        إرجاع setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    افتراضي:
        fail("Invalid fault");
    }
}
