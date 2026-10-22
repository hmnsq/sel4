/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <object.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <api/faults.h>
#അന്തര്ഭാവ <api/syscall.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    തിരഞ്ഞെടുപ്പ് (faultType) {
    അവസ്ഥ seL4_Fault_VMFault:
        തിരികെ true;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_Fault_VGICMaintenance:
        തിരികെ true;
    അവസ്ഥ seL4_Fault_VCPUFault:
        തിരികെ true;
    അവസ്ഥ seL4_Fault_VPPIEvent:
        തിരികെ true;
#endif
    സ്ഥിരസ്ഥിതി:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    തിരഞ്ഞെടുപ്പ് (faultType) {
    അവസ്ഥ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_Fault_VGICMaintenance:
        എങ്കിൽ (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } അല്ലെങ്കിൽ {
            തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    അവസ്ഥ seL4_Fault_VCPUFault:
        തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    അവസ്ഥ seL4_Fault_VPPIEvent:
        തിരികെ setMR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#endif

    സ്ഥിരസ്ഥിതി:
        fail("Invalid fault");
    }
}
