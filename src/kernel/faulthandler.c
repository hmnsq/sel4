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

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != تہی);
    اگر (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

خالی handleTimeout(tcb_t *tptr)
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
                cap_endpoint_cap_get_cap_Can_Grant_جواب(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        واپس true;
    } ورنہ {
        أكد(cap_get_capType(handlerCap) == cap_null_cap);
        واپس false;
    }
}
#ورنہ

خالی handleFault(tcb_t *tptr)
{
    exception_t حالت_2;
    seL4_Fault_t fault = current_fault;

    حالت_2 = sendFaultIPC(tptr);
    اگر (حالت_2 != EXCEPTION_NONE) {
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
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        واپس EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    اگر (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_جواب(handlerCap))) {
        tptr->tcbFault = current_fault;
        اگر (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        واپس EXCEPTION_NONE;
    } ورنہ {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        واپس EXCEPTION_FAULT;
    }
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_PRINTING
ساکن خالی print_fault(seL4_Fault_t f)
{
    انتخاب (seL4_Fault_get_seL4_FaultType(f)) {
    حالت seL4_Fault_NullFault:
        لکھو("null fault");
        توڑو;
    حالت seL4_Fault_CapFault:
        لکھو("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_اندر_Receive_Phase(f) ? "receive" : "send",
               (خالی *)se_L4_Fault_Cap_Fault_get_پتہ(f));
        توڑو;
    حالت seL4_Fault_VMFault:
        لکھو("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (خالی *)se_L4_Fault_VMFault_get_پتہ(f),
               (خالی *)seL4_Fault_VMFault_get_FSR(f));
        توڑو;
    حالت seL4_Fault_UnknownSyscall:
        لکھو("unknown syscall %p",
               (خالی *)se_L4_Fault_Unknown_Syscall_get_syscall_نمبر(f));
        توڑو;
    حالت seL4_Fault_UserException:
        لکھو("user exception %p code %p",
               (خالی *)se_L4_Fault_User_Exception_get_نمبر(f),
               (خالی *)se_L4_Fault_User_Exception_get_رمز(f));
        توڑو;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    حالت seL4_Fault_Timeout:
        لکھو("Timeout fault for 0x%x\n", (غیرعلامتی صحیح) seL4_Fault_Timeout_get_badge(f));
        توڑو;
#اگر_ختم
    طےشدہ:
        لکھو("unknown fault");
        توڑو;
    }
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی handleNoFaultHandler(tcb_t *tptr)
#ورنہ
/* The second fault, ex2, is stored in the global current_fault */
خالی handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#اگر_ختم
{
#اگر_تعریف_شدہ CONFIG_PRINTING
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    لکھو("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#ورنہ
    seL4_Fault_t ex2 = current_fault;
    لکھو("Caught ");
    print_fault(ex2);
    لکھو("\nwhile trying to handle:\n");
    print_fault(ex1);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD
    لکھو("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#اگر_ختم /* CONFIG_DEBUG_BUILD */

    لکھو("at address %p\n", (خالی *)getRestartPC(tptr));
    لکھو("With stack:\n");
    Arch_user_ڈھیر_Trace(tptr);
#اگر_ختم

    سیٹ_Thread_State(tptr, ThreadState_Inactive);
}
