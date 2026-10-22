/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/faulthandler.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
வெறுமை handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != வெற்று);
    எனில் (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

வெறுமை handleTimeout(tcb_t *tptr)
{
    ப்ரதிஜ்ஞா(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    எனில் (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        ப்ரதிஜ்ஞா(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        எனில் (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        திரும்பு true;
    } இல்லையெனில் {
        ப்ரதிஜ்ஞா(cap_get_capType(handlerCap) == cap_null_cap);
        திரும்பு false;
    }
}
#இல்லையெனில்

வெறுமை handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    எனில் (status != EXCEPTION_NONE) {
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
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        திரும்பு EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    எனில் (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        எனில் (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        திரும்பு EXCEPTION_NONE;
    } இல்லையெனில் {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        திரும்பு EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
நிலையான வெறுமை print_fault(seL4_Fault_t f)
{
    தேர்வு (seL4_Fault_get_seL4_FaultType(f)) {
    நிலை seL4_Fault_NullFault:
        printf("null fault");
        நிறுத்து;
    நிலை seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (வெறுமை *)seL4_Fault_CapFault_get_address(f));
        நிறுத்து;
    நிலை seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (வெறுமை *)seL4_Fault_VMFault_get_address(f),
               (வெறுமை *)seL4_Fault_VMFault_get_FSR(f));
        நிறுத்து;
    நிலை seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (வெறுமை *)se_L4_Fault_Unknown_Syscall_get_syscall_எண்(f));
        நிறுத்து;
    நிலை seL4_Fault_UserException:
        printf("user exception %p code %p",
               (வெறுமை *)se_L4_Fault_User_Exception_get_எண்(f),
               (வெறுமை *)seL4_Fault_UserException_get_code(f));
        நிறுத்து;
#ifdef CONFIG_KERNEL_MCS
    நிலை seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned முழு) seL4_Fault_Timeout_get_badge(f));
        நிறுத்து;
#endif
    இயல்பு:
        printf("unknown fault");
        நிறுத்து;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
வெறுமை handleNoFaultHandler(tcb_t *tptr)
#இல்லையெனில்
/* The second fault, ex2, is stored in the global current_fault */
வெறுமை handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#இல்லையெனில்
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (வெறுமை *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_அடுக்கு_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
