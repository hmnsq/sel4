/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <object.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <api/faults.h>
#অন্তর্ভাৱ <api/syscall.h>

#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <api/faults.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    ৱিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault:
        ঘূৰাই true;

    স্ৱভাৱ:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    ৱিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        ঘূৰাই setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    স্ৱভাৱ:
        fail("Invalid fault");
    }
}
