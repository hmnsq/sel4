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

bool_t Arch_handle_Fault_پاسخ(tcb_t *receiver, tcb_t *sender, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

    افتراضي:
        fail("Invalid fault");
    }
}

کلمه_t Arch_مجموعه_MRs_fault(tcb_t *sender, tcb_t *receiver, کلمه_t *receiveIPCBuffer, کلمه_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_آدرس(sender->tcbFault));
        مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع مجموعه_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}

کلمه_t handleKernelException(
    کلمه_t وکتور,
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
    کلمه_t وکتور,
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
    إذا (وکتور == int_gp_fault && ARCH_گره_STATE(x86_KSGPExcept_Return_مقصد) != 0) {
        کلمه_t ret = ARCH_گره_STATE(x86_KSGPExcept_Return_مقصد);
        ARCH_گره_STATE(x86_KSGPExcept_Return_مقصد) = 0;
        أرجع ret;
    }
    چاپ_کن("\n========== KERNEL EXCEPTION ==========\n");
    چاپ_کن("Vector:  0x%lx\n", وکتور);
    چاپ_کن("ErrCode: 0x%lx\n", errcode);
    چاپ_کن("IP:      0x%lx\n", ip);
    چاپ_کن("SP:      0x%lx\n", sp);
    چاپ_کن("FLAGS:   0x%lx\n", flags);
    چاپ_کن("CR0:     0x%lx\n", cr0);
    چاپ_کن("CR2:     0x%lx (page-fault address)\n", cr2);
    چاپ_کن("CR3:     0x%lx (page-directory physical address)\n", cr3);
    چاپ_کن("CR4:     0x%lx\n", cr4);
    چاپ_کن("\nStack Dump:\n");
    لكل (i = 0; i < 20; i++) {
        کلمه_t استفادهنشده پشته = sp + i * حجم(کلمه_t);
        چاپ_کن("*0x%lx == 0x%lx\n", پشته, *(کلمه_t *)پشته);
    }
    چاپ_کن("\nHalting...\n");
    halt();
    UNREACHABLE();
}
