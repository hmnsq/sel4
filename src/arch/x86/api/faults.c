/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <machine/io.h>
#ضمن <api/faults.h>
#ضمن <api/syscall.h>
#ضمن <util.h>

bool_t Arch_handle_Fault_ځواب(tcb_t *receiver, tcb_t *sender, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

    افتراضي:
        fail("Invalid fault");
    }
}

کلمه_t Arch_سېټ_MRs_fault(tcb_t *sender, tcb_t *receiver, کلمه_t *receiveIPCBuffer, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        سېټ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سېټ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پته(sender->tcbFault));
        سېټ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع سېټ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}

کلمه_t handleKernelException(
    کلمه_t ویکټور,
    کلمه_t errcode,
    کلمه_t ip,
    کلمه_t sp,
    کلمه_t flags,
    کلمه_t cr0,
    کلمه_t cr2,
    کلمه_t cr3,
    کلمه_t cr4
);

VISIBLE
کلمه_t handleKernelException(
    کلمه_t ویکټور,
    کلمه_t errcode,
    کلمه_t ip,
    کلمه_t sp,
    کلمه_t flags,
    کلمه_t cr0,
    کلمه_t cr2,
    کلمه_t cr3,
    کلمه_t cr4
)
{
    کلمه_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    إذا (ویکټور == int_gp_fault && ARCH_غوټه_STATE(x86_KSGPExcept_Return_منزل) != 0) {
        کلمه_t ret = ARCH_غوټه_STATE(x86_KSGPExcept_Return_منزل);
        ARCH_غوټه_STATE(x86_KSGPExcept_Return_منزل) = 0;
        أرجع ret;
    }
    چاپ_کړه("\n========== KERNEL EXCEPTION ==========\n");
    چاپ_کړه("Vector:  0x%lx\n", ویکټور);
    چاپ_کړه("ErrCode: 0x%lx\n", errcode);
    چاپ_کړه("IP:      0x%lx\n", ip);
    چاپ_کړه("SP:      0x%lx\n", sp);
    چاپ_کړه("FLAGS:   0x%lx\n", flags);
    چاپ_کړه("CR0:     0x%lx\n", cr0);
    چاپ_کړه("CR2:     0x%lx (page-fault address)\n", cr2);
    چاپ_کړه("CR3:     0x%lx (page-directory physical address)\n", cr3);
    چاپ_کړه("CR4:     0x%lx\n", cr4);
    چاپ_کړه("\nStack Dump:\n");
    لكل (i = 0; i < 20; i++) {
        کلمه_t ناکارول_شوی ډېری = sp + i * حجم(کلمه_t);
        چاپ_کړه("*0x%lx == 0x%lx\n", ډېری, *(کلمه_t *)ډېری);
    }
    چاپ_کړه("\nHalting...\n");
    halt();
    UNREACHABLE();
}
