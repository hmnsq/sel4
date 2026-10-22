/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/faulthandler.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
शून्य handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != रिक्त);
    यदि (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

शून्य handleTimeout(tcb_t *tptr)
{
    प्रतिज्ञा(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    यदि (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        प्रतिज्ञा(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        यदि (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        प्रत्यागम true;
    } अन्यथा {
        प्रतिज्ञा(cap_get_capType(handlerCap) == cap_null_cap);
        प्रत्यागम false;
    }
}
#अन्यथा

शून्य handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    यदि (status != EXCEPTION_NONE) {
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
    यदि (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        प्रत्यागम EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    यदि (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        यदि (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        प्रत्यागम EXCEPTION_NONE;
    } अन्यथा {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        प्रत्यागम EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
स्थिर शून्य print_fault(seL4_Fault_t f)
{
    विकल्प (seL4_Fault_get_seL4_FaultType(f)) {
    पक्ष seL4_Fault_NullFault:
        printf("null fault");
        भङ्ग;
    पक्ष seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (शून्य *)seL4_Fault_CapFault_get_address(f));
        भङ्ग;
    पक्ष seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (शून्य *)seL4_Fault_VMFault_get_address(f),
               (शून्य *)seL4_Fault_VMFault_get_FSR(f));
        भङ्ग;
    पक्ष seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (शून्य *)se_L4_Fault_Unknown_Syscall_get_syscall_संख्या(f));
        भङ्ग;
    पक्ष seL4_Fault_UserException:
        printf("user exception %p code %p",
               (शून्य *)se_L4_Fault_User_Exception_get_संख्या(f),
               (शून्य *)seL4_Fault_UserException_get_code(f));
        भङ्ग;
#ifdef CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned पूर्णाङ्क) seL4_Fault_Timeout_get_badge(f));
        भङ्ग;
#endif
    स्वभाव:
        printf("unknown fault");
        भङ्ग;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
शून्य handleNoFaultHandler(tcb_t *tptr)
#अन्यथा
/* The second fault, ex2, is stored in the global current_fault */
शून्य handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#अन्यथा
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (शून्य *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_ढेर_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
