/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <object.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <api/faults.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>

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
