/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <object.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <api/faults.h>
#অন্তর্ভাব <api/syscall.h>

#অন্তর্ভাব <types.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <api/faults.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <util.h>

bool_t Arch_handleFaultReply(tcb_t *receiver, tcb_t *sender, word_t faultType)
{
    বিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault:
        প্রত্যাগম true;

    স্বভাব:
        fail("Invalid fault");
    }
}

word_t Arch_setMRs_fault(tcb_t *sender, tcb_t *receiver, word_t *receiveIPCBuffer, word_t faultType)
{
    বিকল্প (faultType) {
    পক্ষ seL4_Fault_VMFault: {
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_IP, getRestartPC(sender));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_Addr,
              seL4_Fault_VMFault_get_address(sender->tcbFault));
        setMR(receiver, receiveIPCBuffer, seL4_VMFault_PrefetchFault,
              seL4_Fault_VMFault_get_instructionFault(sender->tcbFault));
        প্রত্যাগম setMR(receiver, receiveIPCBuffer, seL4_VMFault_FSR,
                     seL4_Fault_VMFault_get_FSR(sender->tcbFault));
    }
    স্বভাব:
        fail("Invalid fault");
    }
}
