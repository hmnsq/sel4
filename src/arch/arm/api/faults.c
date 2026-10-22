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

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    انتخاب (faultType) {
    حالت seL4_Fault_VMFault:
        بازگشت true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_Fault_VGICMaintenance:
        بازگشت true;
    حالت seL4_Fault_VCPUFault:
        بازگشت true;
    حالت seL4_Fault_VPPIEvent:
        بازگشت true;
#endif
    پیشفرض:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    انتخاب (faultType) {
    حالت seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        بازگشت setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_Fault_VGICMaintenance:
        اگر (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            بازگشت setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } وگرنه {
            بازگشت setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    حالت seL4_Fault_VCPUFault:
        بازگشت setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    حالت seL4_Fault_VPPIEvent:
        بازگشت setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    پیشفرض:
        fail("Invalid fault");
    }
}
