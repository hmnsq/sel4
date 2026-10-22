/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <object.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <api/faults.h>
#অন্তর্ভাব <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    নির্বাচন (faultType) {
    ক্ষেত্র seL4_Fault_VMFault:
        ফেরত true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_Fault_VGICMaintenance:
        ফেরত true;
    ক্ষেত্র seL4_Fault_VCPUFault:
        ফেরত true;
    ক্ষেত্র seL4_Fault_VPPIEvent:
        ফেরত true;
#endif
    পূর্বনির্ধারিত:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    নির্বাচন (faultType) {
    ক্ষেত্র seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ফেরত setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_Fault_VGICMaintenance:
        যদি (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            ফেরত setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } অন্যথা {
            ফেরত setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    ক্ষেত্র seL4_Fault_VCPUFault:
        ফেরত setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    ক্ষেত্র seL4_Fault_VPPIEvent:
        ফেরত setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    পূর্বনির্ধারিত:
        fail("Invalid fault");
    }
}
