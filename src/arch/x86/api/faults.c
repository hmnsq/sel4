/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/faults.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

bool_t Arch_handle_Fault_ਜਵਾਬ(tcb_t *receiver, tcb_t *sender, ਸ਼ਬਦ_t faultType)
{
    ਚੋਣ (faultType) {
    ਹਾਲਤ seL4_Fault_VMFault:
        ਵਾਪਸ true;

    ਮੂਲ:
        fail("Invalid fault");
    }
}

ਸ਼ਬਦ_t Arch_ਸੈੱਟ_MRs_fault(tcb_t *sender, tcb_t *receiver, ਸ਼ਬਦ_t *receiveIPCBuffer, ਸ਼ਬਦ_t faultType)
{
    ਚੋਣ (faultType) {
    ਹਾਲਤ seL4_Fault_VMFault: {
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_ਪਤਾ(sender->tcbFault));
        ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ਵਾਪਸ ਸੈੱਟ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    ਮੂਲ:
        fail("Invalid fault");
    }
}

ਸ਼ਬਦ_t handleKernelException(
    ਸ਼ਬਦ_t ਸਦਿਸ਼,
    ਸ਼ਬਦ_t errcode,
    ਸ਼ਬਦ_t ip,
    ਸ਼ਬਦ_t sp,
    ਸ਼ਬਦ_t flags,
    ਸ਼ਬਦ_t cr0,
    ਸ਼ਬਦ_t cr2,
    ਸ਼ਬਦ_t cr3,
    ਸ਼ਬਦ_t cr4
);

VISIBLE
ਸ਼ਬਦ_t handleKernelException(
    ਸ਼ਬਦ_t ਸਦਿਸ਼,
    ਸ਼ਬਦ_t errcode,
    ਸ਼ਬਦ_t ip,
    ਸ਼ਬਦ_t sp,
    ਸ਼ਬਦ_t flags,
    ਸ਼ਬਦ_t cr0,
    ਸ਼ਬਦ_t cr2,
    ਸ਼ਬਦ_t cr3,
    ਸ਼ਬਦ_t cr4
)
{
    ਸ਼ਬਦ_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    ਜੇ (ਸਦਿਸ਼ == int_gp_fault && ARCH_ਗੰਢ_STATE(x86_KSGPExcept_Return_ਮੰਜ਼ਿਲ) != 0) {
        ਸ਼ਬਦ_t ret = ARCH_ਗੰਢ_STATE(x86_KSGPExcept_Return_ਮੰਜ਼ਿਲ);
        ARCH_ਗੰਢ_STATE(x86_KSGPExcept_Return_ਮੰਜ਼ਿਲ) = 0;
        ਵਾਪਸ ret;
    }
    ਲਿਖੋ("\n========== KERNEL EXCEPTION ==========\n");
    ਲਿਖੋ("Vector:  0x%lx\n", ਸਦਿਸ਼);
    ਲਿਖੋ("ErrCode: 0x%lx\n", errcode);
    ਲਿਖੋ("IP:      0x%lx\n", ip);
    ਲਿਖੋ("SP:      0x%lx\n", sp);
    ਲਿਖੋ("FLAGS:   0x%lx\n", flags);
    ਲਿਖੋ("CR0:     0x%lx\n", cr0);
    ਲਿਖੋ("CR2:     0x%lx (page-fault address)\n", cr2);
    ਲਿਖੋ("CR3:     0x%lx (page-directory physical address)\n", cr3);
    ਲਿਖੋ("CR4:     0x%lx\n", cr4);
    ਲਿਖੋ("\nStack Dump:\n");
    ਲਈ (i = 0; i < 20; i++) {
        ਸ਼ਬਦ_t ਅਣਵਰਤਿਆ ਢੇਰ = sp + i * ਆਕਾਰ(ਸ਼ਬਦ_t);
        ਲਿਖੋ("*0x%lx == 0x%lx\n", ਢੇਰ, *(ਸ਼ਬਦ_t *)ਢੇਰ);
    }
    ਲਿਖੋ("\nHalting...\n");
    halt();
    UNREACHABLE();
}
