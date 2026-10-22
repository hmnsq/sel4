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

#اگر_تعریف_شده CONFIG_KERNEL_MCS
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
                cap_endpoint_cap_get_cap_Can_Grant_پاسخ(handlerCap),
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
    exception_t وضعیت;
    seL4_Fault_t fault = current_fault;

    وضعیت = sendFaultIPC(tptr);
    إذا (وضعیت != EXCEPTION_NONE) {
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
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        أرجع EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    إذا (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_پاسخ(handlerCap))) {
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
#پایان_اگر

#اگر_تعریف_شده CONFIG_PRINTING
ساكن فراغ print_fault(seL4_Fault_t f)
{
    بدل (seL4_Fault_get_seL4_FaultType(f)) {
    حالة seL4_Fault_NullFault:
        چاپ_کن("null fault");
        اكسر;
    حالة seL4_Fault_CapFault:
        چاپ_کن("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_درون_Receive_Phase(f) ? "receive" : "send",
               (فراغ *)se_L4_Fault_Cap_Fault_get_آدرس(f));
        اكسر;
    حالة seL4_Fault_VMFault:
        چاپ_کن("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (فراغ *)se_L4_Fault_VMFault_get_آدرس(f),
               (فراغ *)seL4_Fault_VMFault_get_FSR(f));
        اكسر;
    حالة seL4_Fault_UnknownSyscall:
        چاپ_کن("unknown syscall %p",
               (فراغ *)se_L4_Fault_Unknown_Syscall_get_syscall_عدد(f));
        اكسر;
    حالة seL4_Fault_UserException:
        چاپ_کن("user exception %p code %p",
               (فراغ *)se_L4_Fault_User_Exception_get_عدد(f),
               (فراغ *)se_L4_Fault_User_Exception_get_کد(f));
        اكسر;
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        چاپ_کن("Timeout fault for 0x%x\n", (بیعلامت صحيح) seL4_Fault_Timeout_get_badge(f));
        اكسر;
#پایان_اگر
    افتراضي:
        چاپ_کن("unknown fault");
        اكسر;
    }
}
#پایان_اگر

#اگر_تعریف_شده CONFIG_KERNEL_MCS
فراغ handleNoFaultHandler(tcb_t *tptr)
#وإلا
/* The second fault, ex2, is stored in the global current_fault */
فراغ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#پایان_اگر
{
#اگر_تعریف_شده CONFIG_PRINTING
#اگر_تعریف_شده CONFIG_KERNEL_MCS
    چاپ_کن("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#وإلا
    seL4_Fault_t ex2 = current_fault;
    چاپ_کن("Caught ");
    print_fault(ex2);
    چاپ_کن("\nwhile trying to handle:\n");
    print_fault(ex1);
#پایان_اگر
#اگر_تعریف_شده CONFIG_DEBUG_BUILD
    چاپ_کن("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#پایان_اگر /* CONFIG_DEBUG_BUILD */

    چاپ_کن("at address %p\n", (فراغ *)getRestartPC(tptr));
    چاپ_کن("With stack:\n");
    Arch_user_پشته_Trace(tptr);
#پایان_اگر

    مجموعه_Thread_State(tptr, ThreadState_Inactive);
}
