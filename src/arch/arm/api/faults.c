/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <object.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <api/faults.h>
#அந்தர்பாவ <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    தேர்வு (faultType) {
    நிலை seL4_Fault_VMFault:
        திரும்பு true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_Fault_VGICMaintenance:
        திரும்பு true;
    நிலை seL4_Fault_VCPUFault:
        திரும்பு true;
    நிலை seL4_Fault_VPPIEvent:
        திரும்பு true;
#endif
    இயல்பு:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    தேர்வு (faultType) {
    நிலை seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_Fault_VGICMaintenance:
        எனில் (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } இல்லையெனில் {
            திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    நிலை seL4_Fault_VCPUFault:
        திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    நிலை seL4_Fault_VPPIEvent:
        திரும்பு setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    இயல்பு:
        fail("Invalid fault");
    }
}
