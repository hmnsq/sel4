/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/faulthandler.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
শূন্য handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ফাঁকা);
    যদি (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

শূন্য handleTimeout(tcb_t *tptr)
{
    প্রতিজ্ঞা(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    যদি (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        প্রতিজ্ঞা(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        যদি (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ফেরত true;
    } অন্যথা {
        প্রতিজ্ঞা(cap_get_capType(handlerCap) == cap_null_cap);
        ফেরত false;
    }
}
#অন্যথা

শূন্য handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    যদি (status != EXCEPTION_NONE) {
        handleDoubleFault(tptr, fault);
    }
}

exception_t sendFaultIPC(tcb_t *tptr)
{
    cptr_t handlerCPtr;
    cap_t  handlerCap;
    lookupCap_ret_t lu_ret;
    lookup_fault_t original_lookup_fault;

    original_lookup_fault = current_lookup_fault;

    handlerCPtr = tptr->tcbFaultHandler;
    lu_ret = lookupCap(tptr, handlerCPtr);
    যদি (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        ফেরত EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    যদি (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        যদি (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ফেরত EXCEPTION_NONE;
    } অন্যথা {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        ফেরত EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
স্থির শূন্য print_fault(seL4_Fault_t f)
{
    নির্বাচন (seL4_Fault_get_seL4_FaultType(f)) {
    ক্ষেত্র seL4_Fault_NullFault:
        printf("null fault");
        বিরতি;
    ক্ষেত্র seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (শূন্য *)seL4_Fault_CapFault_get_address(f));
        বিরতি;
    ক্ষেত্র seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (শূন্য *)seL4_Fault_VMFault_get_address(f),
               (শূন্য *)seL4_Fault_VMFault_get_FSR(f));
        বিরতি;
    ক্ষেত্র seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (শূন্য *)se_L4_Fault_Unknown_Syscall_get_syscall_সংখ্যা(f));
        বিরতি;
    ক্ষেত্র seL4_Fault_UserException:
        printf("user exception %p code %p",
               (শূন্য *)se_L4_Fault_User_Exception_get_সংখ্যা(f),
               (শূন্য *)seL4_Fault_UserException_get_code(f));
        বিরতি;
#ifdef CONFIG_KERNEL_MCS
    ক্ষেত্র seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned পূর্ণসংখ্যা) seL4_Fault_Timeout_get_badge(f));
        বিরতি;
#endif
    পূর্বনির্ধারিত:
        printf("unknown fault");
        বিরতি;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
শূন্য handleNoFaultHandler(tcb_t *tptr)
#অন্যথা
/* The second fault, ex2, is stored in the global current_fault */
শূন্য handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#অন্যথা
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (শূন্য *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_স্তূপ_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
