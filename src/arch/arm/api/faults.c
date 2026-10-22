/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <object.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/syscall.h>

bool_t Arch_handle_Fault_జవాబు(tcb_t *receiver, tcb_t *sender, మాట_t faultType)
{
    ఎంపిక (faultType) {
    స్థితి seL4_Fault_VMFault:
        తిరిగి true;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_Fault_VGICMaintenance:
        తిరిగి true;
    స్థితి seL4_Fault_VCPUFault:
        తిరిగి true;
    స్థితి seL4_Fault_VPPIEvent:
        తిరిగి true;
#అయితే_ముగింపు
    అప్రమేయ:
        fail("Invalid fault");
    }
}

మాట_t Arch_సమితి_MRs_fault(tcb_t *sender, tcb_t *receiver, మాట_t *receiveIPCBuffer, మాట_t faultType)
{
    ఎంపిక (faultType) {
    స్థితి seL4_Fault_VMFault: {
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_చిరునామా(sender->tcbFault));
        సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_Fault_VGICMaintenance:
        ఒకవేళ (seL4_Fault_VGICMaintenance_get_idxValid(sender->tcbFault)) {
            తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX,
                         seL4_Fault_VGICMaintenance_get_idx(sender->tcbFault));
        } లేకపోతే {
            తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VGICMaintenance_IDX, -1);
        }
    స్థితి seL4_Fault_VCPUFault:
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VCPUFault_HSR, seL4_Fault_VCPUFault_get_hsr(sender->tcbFault));
    స్థితి seL4_Fault_VPPIEvent:
        తిరిగి సమితి_MR(receiver, receiveIPCBuffer, seL4_VPPIEvent_IRQ, seL4_Fault_VPPIEvent_get_irq_w(sender->tcbFault));
#అయితే_ముగింపు

    అప్రమేయ:
        fail("Invalid fault");
    }
}
