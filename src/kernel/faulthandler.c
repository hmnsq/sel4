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

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
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
                cap_endpoint_cap_get_cap_Can_Grant_جواب(handlerCap),
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
    exception_t حالت;
    seL4_Fault_t fault = current_fault;

    حالت = sendFaultIPC(tptr);
    إذا (حالت != EXCEPTION_NONE) {
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
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        أرجع EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    إذا (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_جواب(handlerCap))) {
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
#جے_انت

#جے_تعریف_شدہ CONFIG_PRINTING
ساكن فراغ print_fault(seL4_Fault_t f)
{
    بدل (seL4_Fault_get_seL4_FaultType(f)) {
    حالة seL4_Fault_NullFault:
        لکھو("null fault");
        اكسر;
    حالة seL4_Fault_CapFault:
        لکھو("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_وچ_Receive_Phase(f) ? "receive" : "send",
               (فراغ *)se_L4_Fault_Cap_Fault_get_پتہ(f));
        اكسر;
    حالة seL4_Fault_VMFault:
        لکھو("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (فراغ *)se_L4_Fault_VMFault_get_پتہ(f),
               (فراغ *)seL4_Fault_VMFault_get_FSR(f));
        اكسر;
    حالة seL4_Fault_UnknownSyscall:
        لکھو("unknown syscall %p",
               (فراغ *)se_L4_Fault_Unknown_Syscall_get_syscall_نمبر(f));
        اكسر;
    حالة seL4_Fault_UserException:
        لکھو("user exception %p code %p",
               (فراغ *)se_L4_Fault_User_Exception_get_نمبر(f),
               (فراغ *)se_L4_Fault_User_Exception_get_کوڈ(f));
        اكسر;
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    حالة seL4_Fault_Timeout:
        لکھو("Timeout fault for 0x%x\n", (بنانشانی صحيح) seL4_Fault_Timeout_get_badge(f));
        اكسر;
#جے_انت
    افتراضي:
        لکھو("unknown fault");
        اكسر;
    }
}
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
فراغ handleNoFaultHandler(tcb_t *tptr)
#وإلا
/* The second fault, ex2, is stored in the global current_fault */
فراغ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#جے_انت
{
#جے_تعریف_شدہ CONFIG_PRINTING
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    لکھو("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#وإلا
    seL4_Fault_t ex2 = current_fault;
    لکھو("Caught ");
    print_fault(ex2);
    لکھو("\nwhile trying to handle:\n");
    print_fault(ex1);
#جے_انت
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    لکھو("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#جے_انت /* CONFIG_DEBUG_BUILD */

    لکھو("at address %p\n", (فراغ *)getRestartPC(tptr));
    لکھو("With stack:\n");
    Arch_user_ڈھیر_Trace(tptr);
#جے_انت

    سیٹ_Thread_State(tptr, ThreadState_Inactive);
}
