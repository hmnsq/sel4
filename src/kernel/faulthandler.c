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

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
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
                cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(handlerCap),
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
    exception_t स्थितिः;
    seL4_Fault_t fault = current_fault;

    स्थितिः = sendFaultIPC(tptr);
    यदि (स्थितिः != EXCEPTION_NONE) {
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
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        प्रत्यागम EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    यदि (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(handlerCap))) {
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
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_PRINTING
स्थिर शून्य print_fault(seL4_Fault_t f)
{
    विकल्प (seL4_Fault_get_seL4_FaultType(f)) {
    पक्ष seL4_Fault_NullFault:
        प्रारूप_लिख("null fault");
        भङ्ग;
    पक्ष seL4_Fault_CapFault:
        प्रारूप_लिख("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_अन्तर्_Receive_Phase(f) ? "receive" : "send",
               (शून्य *)se_L4_Fault_Cap_Fault_get_स्थानाङ्कः(f));
        भङ्ग;
    पक्ष seL4_Fault_VMFault:
        प्रारूप_लिख("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (शून्य *)se_L4_Fault_VMFault_get_स्थानाङ्कः(f),
               (शून्य *)seL4_Fault_VMFault_get_FSR(f));
        भङ्ग;
    पक्ष seL4_Fault_UnknownSyscall:
        प्रारूप_लिख("unknown syscall %p",
               (शून्य *)se_L4_Fault_Unknown_Syscall_get_syscall_सङ्ख्या(f));
        भङ्ग;
    पक्ष seL4_Fault_UserException:
        प्रारूप_लिख("user exception %p code %p",
               (शून्य *)se_L4_Fault_User_Exception_get_सङ्ख्या(f),
               (शून्य *)se_L4_Fault_User_Exception_get_संहिता(f));
        भङ्ग;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    पक्ष seL4_Fault_Timeout:
        प्रारूप_लिख("Timeout fault for 0x%x\n", (अचिह्नित पूर्णाङ्क) seL4_Fault_Timeout_get_badge(f));
        भङ्ग;
#यदि_अन्तः
    स्वभाव:
        प्रारूप_लिख("unknown fault");
        भङ्ग;
    }
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य handleNoFaultHandler(tcb_t *tptr)
#अन्यथा
/* The second fault, ex2, is stored in the global current_fault */
शून्य handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#यदि_अन्तः
{
#यदि_परिभाषितम् CONFIG_PRINTING
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    प्रारूप_लिख("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#अन्यथा
    seL4_Fault_t ex2 = current_fault;
    प्रारूप_लिख("Caught ");
    print_fault(ex2);
    प्रारूप_लिख("\nwhile trying to handle:\n");
    print_fault(ex1);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_DEBUG_BUILD
    प्रारूप_लिख("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#यदि_अन्तः /* CONFIG_DEBUG_BUILD */

    प्रारूप_लिख("at address %p\n", (शून्य *)getRestartPC(tptr));
    प्रारूप_लिख("With stack:\n");
    Arch_user_राशिः_Trace(tptr);
#यदि_अन्तः

    समुच्चयः_Thread_State(tptr, ThreadState_Inactive);
}
