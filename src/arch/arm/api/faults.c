/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <object.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <api/faults.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    ବାଛ (faultType) {
    ସ୍ଥିତି seL4_Fault_VMFault:
        ଫେରନ୍ତୁ true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_Fault_VGICMaintenance:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_Fault_VCPUFault:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_Fault_VPPIEvent:
        ଫେରନ୍ତୁ true;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    ବାଛ (faultType) {
    ସ୍ଥିତି seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_Fault_VGICMaintenance:
        ଯଦି (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    ସ୍ଥିତି seL4_Fault_VCPUFault:
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    ସ୍ଥିତି seL4_Fault_VPPIEvent:
        ଫେରନ୍ତୁ setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid fault");
    }
}
