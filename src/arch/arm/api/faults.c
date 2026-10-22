/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <object.h>
#समावेश <kernel/vspace.h>
#समावेश <api/faults.h>
#समावेश <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    चयन (faultType) {
    हाल seL4_Fault_VMFault:
        वापस true;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_Fault_VGICMaintenance:
        वापस true;
    हाल seL4_Fault_VCPUFault:
        वापस true;
    हाल seL4_Fault_VPPIEvent:
        वापस true;
#पूर्ण_अगर
    शेष:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    चयन (faultType) {
    हाल seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        वापस setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_Fault_VGICMaintenance:
        अगर (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            वापस setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } अन्यथा {
            वापस setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    हाल seL4_Fault_VCPUFault:
        वापस setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    हाल seL4_Fault_VPPIEvent:
        वापस setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#पूर्ण_अगर

    शेष:
        fail("Invalid fault");
    }
}
