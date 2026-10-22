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

bool_t Arch_handle_Fault_جواب(tcb_t *receiver, tcb_t *sender, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault:
        أرجع true;

    افتراضي:
        fail("Invalid fault");
    }
}

لفظ_t Arch_سیٹ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پتہ(sender->tcbFault));
        سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع سیٹ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}

لفظ_t handleKernelException(
    لفظ_t سمتیہ,
    لفظ_t errcode,
    لفظ_t ip,
    لفظ_t sp,
    لفظ_t flags,
    لفظ_t cr0,
    لفظ_t cr2,
    لفظ_t cr3,
    لفظ_t cr4
);

VISIBLE
لفظ_t handleKernelException(
    لفظ_t سمتیہ,
    لفظ_t errcode,
    لفظ_t ip,
    لفظ_t sp,
    لفظ_t flags,
    لفظ_t cr0,
    لفظ_t cr2,
    لفظ_t cr3,
    لفظ_t cr4
)
{
    لفظ_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    إذا (سمتیہ == int_gp_fault && ARCH_گرہ_STATE(x86_KSGPExcept_Return_منزل) != 0) {
        لفظ_t ret = ARCH_گرہ_STATE(x86_KSGPExcept_Return_منزل);
        ARCH_گرہ_STATE(x86_KSGPExcept_Return_منزل) = 0;
        أرجع ret;
    }
    لکھو("\n========== KERNEL EXCEPTION ==========\n");
    لکھو("Vector:  0x%lx\n", سمتیہ);
    لکھو("ErrCode: 0x%lx\n", errcode);
    لکھو("IP:      0x%lx\n", ip);
    لکھو("SP:      0x%lx\n", sp);
    لکھو("FLAGS:   0x%lx\n", flags);
    لکھو("CR0:     0x%lx\n", cr0);
    لکھو("CR2:     0x%lx (page-fault address)\n", cr2);
    لکھو("CR3:     0x%lx (page-directory physical address)\n", cr3);
    لکھو("CR4:     0x%lx\n", cr4);
    لکھو("\nStack Dump:\n");
    لكل (i = 0; i < 20; i++) {
        لفظ_t بن_ورتیا ڈھیر = sp + i * حجم(لفظ_t);
        لکھو("*0x%lx == 0x%lx\n", ڈھیر, *(لفظ_t *)ڈھیر);
    }
    لکھو("\nHalting...\n");
    halt();
    UNREACHABLE();
}
