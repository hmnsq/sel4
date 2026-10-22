/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <api/faults.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <util.h>

bool_t Arch_handle_Fault_జవాబు(tcb_t *receiver, tcb_t *sender, మాట_t faultType)
{
    ఎంపిక (faultType) {
    స్థితి seL4_Fault_VMFault:
        తిరిగి true;

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
    అప్రమేయ:
        fail("Invalid fault");
    }
}

మాట_t handleKernelException(
    మాట_t సదిశ,
    మాట_t errcode,
    మాట_t ip,
    మాట_t sp,
    మాట_t flags,
    మాట_t cr0,
    మాట_t cr2,
    మాట_t cr3,
    మాట_t cr4
);

VISIBLE
మాట_t handleKernelException(
    మాట_t సదిశ,
    మాట_t errcode,
    మాట_t ip,
    మాట_t sp,
    మాట_t flags,
    మాట_t cr0,
    మాట_t cr2,
    మాట_t cr3,
    మాట_t cr4
)
{
    మాట_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    ఒకవేళ (సదిశ == int_gp_fault && ARCH_కణుపు_STATE(x86_KSGPExcept_Return_గమ్యం) != 0) {
        మాట_t ret = ARCH_కణుపు_STATE(x86_KSGPExcept_Return_గమ్యం);
        ARCH_కణుపు_STATE(x86_KSGPExcept_Return_గమ్యం) = 0;
        తిరిగి ret;
    }
    రాయి("\n========== KERNEL EXCEPTION ==========\n");
    రాయి("Vector:  0x%lx\n", సదిశ);
    రాయి("ErrCode: 0x%lx\n", errcode);
    రాయి("IP:      0x%lx\n", ip);
    రాయి("SP:      0x%lx\n", sp);
    రాయి("FLAGS:   0x%lx\n", flags);
    రాయి("CR0:     0x%lx\n", cr0);
    రాయి("CR2:     0x%lx (page-fault address)\n", cr2);
    రాయి("CR3:     0x%lx (page-directory physical address)\n", cr3);
    రాయి("CR4:     0x%lx\n", cr4);
    రాయి("\nStack Dump:\n");
    కొరకు (i = 0; i < 20; i++) {
        మాట_t వాడని దొంతర = sp + i * పరిమాణం(మాట_t);
        రాయి("*0x%lx == 0x%lx\n", దొంతర, *(మాట_t *)దొంతర);
    }
    రాయి("\nHalting...\n");
    halt();
    UNREACHABLE();
}
