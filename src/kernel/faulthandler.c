/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <api/failures.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/faulthandler.h>
#समावेश <kernel/thread.h>
#समावेश <machine/io.h>
#समावेश <arch/machine.h>

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != शून्य);
    अगर (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

व्योम handleTimeout(tcb_t *tptr)
{
    निश्चित(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    अगर (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        निश्चित(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        अगर (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        वापस true;
    } अन्यथा {
        निश्चित(cap_get_capType(handlerCap) == cap_null_cap);
        वापस false;
    }
}
#अन्यथा

व्योम handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    अगर (status != EXCEPTION_NONE) {
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
    अगर (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        वापस EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    अगर (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        अगर (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        वापस EXCEPTION_NONE;
    } अन्यथा {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        वापस EXCEPTION_FAULT;
    }
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_PRINTING
अटल व्योम print_fault(seL4_Fault_t f)
{
    चयन (seL4_Fault_get_seL4_FaultType(f)) {
    हाल seL4_Fault_NullFault:
        म_लिखो("null fault");
        अवरोध;
    हाल seL4_Fault_CapFault:
        म_लिखो("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (व्योम *)seL4_Fault_CapFault_get_address(f));
        अवरोध;
    हाल seL4_Fault_VMFault:
        म_लिखो("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (व्योम *)seL4_Fault_VMFault_get_address(f),
               (व्योम *)seL4_Fault_VMFault_get_FSR(f));
        अवरोध;
    हाल seL4_Fault_UnknownSyscall:
        म_लिखो("unknown syscall %p",
               (व्योम *)se_L4_Fault_Unknown_Syscall_get_syscall_संख्या(f));
        अवरोध;
    हाल seL4_Fault_UserException:
        म_लिखो("user exception %p code %p",
               (व्योम *)se_L4_Fault_User_Exception_get_संख्या(f),
               (व्योम *)seL4_Fault_UserException_get_code(f));
        अवरोध;
#अगर_घोषित CONFIG_KERNEL_MCS
    हाल seL4_Fault_Timeout:
        म_लिखो("Timeout fault for 0x%x\n", (अचिन्हित पूर्णांक) seL4_Fault_Timeout_get_badge(f));
        अवरोध;
#पूर्ण_अगर
    शेष:
        म_लिखो("unknown fault");
        अवरोध;
    }
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_KERNEL_MCS
व्योम handleNoFaultHandler(tcb_t *tptr)
#अन्यथा
/* The second fault, ex2, is stored in the global current_fault */
व्योम handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#पूर्ण_अगर
{
#अगर_घोषित CONFIG_PRINTING
#अगर_घोषित CONFIG_KERNEL_MCS
    म_लिखो("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#अन्यथा
    seL4_Fault_t ex2 = current_fault;
    म_लिखो("Caught ");
    print_fault(ex2);
    म_लिखो("\nwhile trying to handle:\n");
    print_fault(ex1);
#पूर्ण_अगर
#अगर_घोषित CONFIG_DEBUG_BUILD
    म_लिखो("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#पूर्ण_अगर /* CONFIG_DEBUG_BUILD */

    म_लिखो("at address %p\n", (व्योम *)getRestartPC(tptr));
    म_लिखो("With stack:\n");
    Arch_user_ढेर_Trace(tptr);
#पूर्ण_अगर

    setThreadState(tptr, ThreadState_Inactive);
}
