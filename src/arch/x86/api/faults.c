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

لفظ_t Arch_سيٽ_MRs_fault(tcb_t *sender, tcb_t *receiver, لفظ_t *receiveIPCBuffer, لفظ_t faultType)
{
    بدل (faultType) {
    حالة seL4_Fault_VMFault: {
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_پتو(sender->tcbFault));
        سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        أرجع سيٽ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    افتراضي:
        fail("Invalid fault");
    }
}

لفظ_t handleKernelException(
    لفظ_t ويڪٽر,
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
    لفظ_t ويڪٽر,
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
    إذا (ويڪٽر == int_gp_fault && ARCH_ڳنڍ_STATE(x86_KSGPExcept_Return_منزل) != 0) {
        لفظ_t ret = ARCH_ڳنڍ_STATE(x86_KSGPExcept_Return_منزل);
        ARCH_ڳنڍ_STATE(x86_KSGPExcept_Return_منزل) = 0;
        أرجع ret;
    }
    لکو("\n========== KERNEL EXCEPTION ==========\n");
    لکو("Vector:  0x%lx\n", ويڪٽر);
    لکو("ErrCode: 0x%lx\n", errcode);
    لکو("IP:      0x%lx\n", ip);
    لکو("SP:      0x%lx\n", sp);
    لکو("FLAGS:   0x%lx\n", flags);
    لکو("CR0:     0x%lx\n", cr0);
    لکو("CR2:     0x%lx (page-fault address)\n", cr2);
    لکو("CR3:     0x%lx (page-directory physical address)\n", cr3);
    لکو("CR4:     0x%lx\n", cr4);
    لکو("\nStack Dump:\n");
    لكل (i = 0; i < 20; i++) {
        لفظ_t اڻ_استعمال_ٿيل ڍير = sp + i * حجم(لفظ_t);
        لکو("*0x%lx == 0x%lx\n", ڍير, *(لفظ_t *)ڍير);
    }
    لکو("\nHalting...\n");
    halt();
    UNREACHABLE();
}
