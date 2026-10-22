/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <util.h>

bool_t Arch_handle_Fault_ಉತ್ತರ(tcb_t *receiver, tcb_t *sender, ಶಬ್ದ_t faultType)
{
    ಆಯ್ಕೆ (faultType) {
    ಸ್ಥಿತಿ seL4_Fault_VMFault:
        ಹಿಂತಿರುಗಿ true;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid fault");
    }
}

ಶಬ್ದ_t Arch_ಗಣ_MRs_fault(tcb_t *sender, tcb_t *receiver, ಶಬ್ದ_t *receiveIPCBuffer, ಶಬ್ದ_t faultType)
{
    ಆಯ್ಕೆ (faultType) {
    ಸ್ಥಿತಿ seL4_Fault_VMFault: {
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              se_L4_Fault_VMFault_get_ವಿಳಾಸ(sender->tcbFault));
        ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ಹಿಂತಿರುಗಿ ಗಣ_MR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid fault");
    }
}

ಶಬ್ದ_t handleKernelException(
    ಶಬ್ದ_t ಸದಿಶ,
    ಶಬ್ದ_t errcode,
    ಶಬ್ದ_t ip,
    ಶಬ್ದ_t sp,
    ಶಬ್ದ_t flags,
    ಶಬ್ದ_t cr0,
    ಶಬ್ದ_t cr2,
    ಶಬ್ದ_t cr3,
    ಶಬ್ದ_t cr4
);

VISIBLE
ಶಬ್ದ_t handleKernelException(
    ಶಬ್ದ_t ಸದಿಶ,
    ಶಬ್ದ_t errcode,
    ಶಬ್ದ_t ip,
    ಶಬ್ದ_t sp,
    ಶಬ್ದ_t flags,
    ಶಬ್ದ_t cr0,
    ಶಬ್ದ_t cr2,
    ಶಬ್ದ_t cr3,
    ಶಬ್ದ_t cr4
)
{
    ಶಬ್ದ_t i;

    /* Check if we are in a state where we expect a GP fault, if so record it and return */
    ಆದರೆ (ಸದಿಶ == int_gp_fault && ARCH_ಗಂಟು_STATE(x86_KSGPExcept_Return_ಗಮ್ಯ) != 0) {
        ಶಬ್ದ_t ret = ARCH_ಗಂಟು_STATE(x86_KSGPExcept_Return_ಗಮ್ಯ);
        ARCH_ಗಂಟು_STATE(x86_KSGPExcept_Return_ಗಮ್ಯ) = 0;
        ಹಿಂತಿರುಗಿ ret;
    }
    ಬರೆ("\n========== KERNEL EXCEPTION ==========\n");
    ಬರೆ("Vector:  0x%lx\n", ಸದಿಶ);
    ಬರೆ("ErrCode: 0x%lx\n", errcode);
    ಬರೆ("IP:      0x%lx\n", ip);
    ಬರೆ("SP:      0x%lx\n", sp);
    ಬರೆ("FLAGS:   0x%lx\n", flags);
    ಬರೆ("CR0:     0x%lx\n", cr0);
    ಬರೆ("CR2:     0x%lx (page-fault address)\n", cr2);
    ಬರೆ("CR3:     0x%lx (page-directory physical address)\n", cr3);
    ಬರೆ("CR4:     0x%lx\n", cr4);
    ಬರೆ("\nStack Dump:\n");
    ಗಾಗಿ (i = 0; i < 20; i++) {
        ಶಬ್ದ_t ಬಳಸದ ರಾಶಿ = sp + i * ಗಾತ್ರ(ಶಬ್ದ_t);
        ಬರೆ("*0x%lx == 0x%lx\n", ರಾಶಿ, *(ಶಬ್ದ_t *)ರಾಶಿ);
    }
    ಬರೆ("\nHalting...\n");
    halt();
    UNREACHABLE();
}
