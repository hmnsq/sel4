/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <api/failures.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/faulthandler.h>
#ضمن <kernel/thread.h>
#ضمن <machine/io.h>
#ضمن <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
فراغ handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != عدم);
    إذا (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

فراغ handleTimeout(tcb_t *tptr)
{
    أكد(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    إذا (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        أكد(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        إذا (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        أرجع true;
    } وإلا {
        أكد(cap_get_capType(handlerCap) == cap_null_cap);
        أرجع false;
    }
}
#وإلا

فراغ handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    إذا (status != EXCEPTION_NONE) {
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
    إذا (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        أرجع EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    إذا (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        إذا (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        أرجع EXCEPTION_NONE;
    } وإلا {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        أرجع EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
ساكن فراغ اطبع_fault(seL4_Fault_t f)
{
    بدل (seL4_Fault_get_seL4_FaultType(f)) {
    حالة seL4_Fault_NullFault:
        printf("null fault");
        اكسر;
    حالة seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (فراغ *)seL4_Fault_CapFault_get_address(f));
        اكسر;
    حالة seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (فراغ *)seL4_Fault_VMFault_get_address(f),
               (فراغ *)seL4_Fault_VMFault_get_FSR(f));
        اكسر;
    حالة seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (فراغ *)se_L4_Fault_Unknown_Syscall_get_syscall_رقم(f));
        اكسر;
    حالة seL4_Fault_UserException:
        printf("user exception %p code %p",
               (فراغ *)se_L4_Fault_User_Exception_get_رقم(f),
               (فراغ *)seL4_Fault_UserException_get_code(f));
        اكسر;
#ifdef CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned صحيح) seL4_Fault_Timeout_get_badge(f));
        اكسر;
#endif
    افتراضي:
        printf("unknown fault");
        اكسر;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
فراغ handleNoFaultHandler(tcb_t *tptr)
#وإلا
/* The second fault, ex2, is stored in the global current_fault */
فراغ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    اطبع_fault(current_fault);
#وإلا
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    اطبع_fault(ex2);
    printf("\nwhile trying to handle:\n");
    اطبع_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (فراغ *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_مكدس_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
