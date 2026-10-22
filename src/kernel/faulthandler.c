/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <api/failures.h>
#شامل <kernel/cspace.h>
#شامل <kernel/faulthandler.h>
#شامل <kernel/thread.h>
#شامل <machine/io.h>
#شامل <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
تهی handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != پوچ);
    اگر (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

تهی handleTimeout(tcb_t *tptr)
{
    أكد(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    اگر (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        أكد(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        اگر (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        بازگشت true;
    } وگرنه {
        أكد(cap_get_capType(handlerCap) == cap_null_cap);
        بازگشت false;
    }
}
#وگرنه

تهی handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    اگر (status != EXCEPTION_NONE) {
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
    اگر (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        بازگشت EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    اگر (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        اگر (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        بازگشت EXCEPTION_NONE;
    } وگرنه {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        بازگشت EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
ایستا تهی print_fault(seL4_Fault_t f)
{
    انتخاب (seL4_Fault_get_seL4_FaultType(f)) {
    حالت seL4_Fault_NullFault:
        printf("null fault");
        بشکن;
    حالت seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (تهی *)seL4_Fault_CapFault_get_address(f));
        بشکن;
    حالت seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (تهی *)seL4_Fault_VMFault_get_address(f),
               (تهی *)seL4_Fault_VMFault_get_FSR(f));
        بشکن;
    حالت seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (تهی *)se_L4_Fault_Unknown_Syscall_get_syscall_عدد(f));
        بشکن;
    حالت seL4_Fault_UserException:
        printf("user exception %p code %p",
               (تهی *)se_L4_Fault_User_Exception_get_عدد(f),
               (تهی *)seL4_Fault_UserException_get_code(f));
        بشکن;
#ifdef CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned صحیح) seL4_Fault_Timeout_get_badge(f));
        بشکن;
#endif
    پیشفرض:
        printf("unknown fault");
        بشکن;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
تهی handleNoFaultHandler(tcb_t *tptr)
#وگرنه
/* The second fault, ex2, is stored in the global current_fault */
تهی handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#وگرنه
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (تهی *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_پشته_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
