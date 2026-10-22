/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/faulthandler.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ਨਿਰਾ);
    ਜੇ (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

ਖਾਲੀ handleTimeout(tcb_t *tptr)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    ਜੇ (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        ਪ੍ਰਤਿਜ੍ਞਾ(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        ਜੇ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ਵਾਪਸ true;
    } ਨਹੀਂਤਾਂ {
        ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(handlerCap) == cap_null_cap);
        ਵਾਪਸ false;
    }
}
#ਨਹੀਂਤਾਂ

ਖਾਲੀ handleFault(tcb_t *tptr)
{
    exception_t ਸਥਿਤੀ;
    seL4_Fault_t fault = current_fault;

    ਸਥਿਤੀ = sendFaultIPC(tptr);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
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
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        ਵਾਪਸ EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    ਜੇ (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_ਜਵਾਬ(handlerCap))) {
        tptr->tcbFault = current_fault;
        ਜੇ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ਵਾਪਸ EXCEPTION_NONE;
    } ਨਹੀਂਤਾਂ {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        ਵਾਪਸ EXCEPTION_FAULT;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
ਸਥਿਰ ਖਾਲੀ print_fault(seL4_Fault_t f)
{
    ਚੋਣ (seL4_Fault_get_seL4_FaultType(f)) {
    ਹਾਲਤ seL4_Fault_NullFault:
        ਲਿਖੋ("null fault");
        ਰੋਕੋ;
    ਹਾਲਤ seL4_Fault_CapFault:
        ਲਿਖੋ("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_ਵਿੱਚ_Receive_Phase(f) ? "receive" : "send",
               (ਖਾਲੀ *)se_L4_Fault_Cap_Fault_get_ਪਤਾ(f));
        ਰੋਕੋ;
    ਹਾਲਤ seL4_Fault_VMFault:
        ਲਿਖੋ("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (ਖਾਲੀ *)se_L4_Fault_VMFault_get_ਪਤਾ(f),
               (ਖਾਲੀ *)seL4_Fault_VMFault_get_FSR(f));
        ਰੋਕੋ;
    ਹਾਲਤ seL4_Fault_UnknownSyscall:
        ਲਿਖੋ("unknown syscall %p",
               (ਖਾਲੀ *)se_L4_Fault_Unknown_Syscall_get_syscall_ਸੰਖਿਆ(f));
        ਰੋਕੋ;
    ਹਾਲਤ seL4_Fault_UserException:
        ਲਿਖੋ("user exception %p code %p",
               (ਖਾਲੀ *)se_L4_Fault_User_Exception_get_ਸੰਖਿਆ(f),
               (ਖਾਲੀ *)se_L4_Fault_User_Exception_get_ਕੋਡ(f));
        ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਹਾਲਤ seL4_Fault_Timeout:
        ਲਿਖੋ("Timeout fault for 0x%x\n", (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ) seL4_Fault_Timeout_get_badge(f));
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਲਿਖੋ("unknown fault");
        ਰੋਕੋ;
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ handleNoFaultHandler(tcb_t *tptr)
#ਨਹੀਂਤਾਂ
/* The second fault, ex2, is stored in the global current_fault */
ਖਾਲੀ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#ਜੇ_ਅੰਤ
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਲਿਖੋ("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#ਨਹੀਂਤਾਂ
    seL4_Fault_t ex2 = current_fault;
    ਲਿਖੋ("Caught ");
    print_fault(ex2);
    ਲਿਖੋ("\nwhile trying to handle:\n");
    print_fault(ex1);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    ਲਿਖੋ("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */

    ਲਿਖੋ("at address %p\n", (ਖਾਲੀ *)getRestartPC(tptr));
    ਲਿਖੋ("With stack:\n");
    Arch_user_ਢੇਰ_Trace(tptr);
#ਜੇ_ਅੰਤ

    ਸੈੱਟ_Thread_State(tptr, ThreadState_Inactive);
}
