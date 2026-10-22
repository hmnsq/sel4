/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/faulthandler.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ಖಾಲಿ);
    ಆದರೆ (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

ಶೂನ್ಯ handleTimeout(tcb_t *tptr)
{
    ಪ್ರತಿಜ್ಞಾ(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    ಆದರೆ (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        ಪ್ರತಿಜ್ಞಾ(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        ಆದರೆ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ಹಿಂತಿರುಗಿ true;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪ್ರತಿಜ್ಞಾ(cap_get_capType(handlerCap) == cap_null_cap);
        ಹಿಂತಿರುಗಿ false;
    }
}
#ಇಲ್ಲದಿದ್ದರೆ

ಶೂನ್ಯ handleFault(tcb_t *tptr)
{
    exception_t ಸ್ಥಿತಿ_2;
    seL4_Fault_t fault = current_fault;

    ಸ್ಥಿತಿ_2 = sendFaultIPC(tptr);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
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
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    ಆದರೆ (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(handlerCap))) {
        tptr->tcbFault = current_fault;
        ಆದರೆ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    } ಇಲ್ಲದಿದ್ದರೆ {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
ಸ್ಥಿರ ಶೂನ್ಯ print_fault(seL4_Fault_t f)
{
    ಆಯ್ಕೆ (seL4_Fault_get_seL4_FaultType(f)) {
    ಸ್ಥಿತಿ seL4_Fault_NullFault:
        ಬರೆ("null fault");
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ seL4_Fault_CapFault:
        ಬರೆ("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_ಒಳ_Receive_Phase(f) ? "receive" : "send",
               (ಶೂನ್ಯ *)se_L4_Fault_Cap_Fault_get_ವಿಳಾಸ(f));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ seL4_Fault_VMFault:
        ಬರೆ("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (ಶೂನ್ಯ *)se_L4_Fault_VMFault_get_ವಿಳಾಸ(f),
               (ಶೂನ್ಯ *)seL4_Fault_VMFault_get_FSR(f));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ seL4_Fault_UnknownSyscall:
        ಬರೆ("unknown syscall %p",
               (ಶೂನ್ಯ *)se_L4_Fault_Unknown_Syscall_get_syscall_ಸಂಖ್ಯೆ(f));
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ seL4_Fault_UserException:
        ಬರೆ("user exception %p code %p",
               (ಶೂನ್ಯ *)se_L4_Fault_User_Exception_get_ಸಂಖ್ಯೆ(f),
               (ಶೂನ್ಯ *)se_L4_Fault_User_Exception_get_ಸಂಕೇತ(f));
        ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಸ್ಥಿತಿ seL4_Fault_Timeout:
        ಬರೆ("Timeout fault for 0x%x\n", (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ) seL4_Fault_Timeout_get_badge(f));
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಬರೆ("unknown fault");
        ನಿಲ್ಲಿಸು;
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ handleNoFaultHandler(tcb_t *tptr)
#ಇಲ್ಲದಿದ್ದರೆ
/* The second fault, ex2, is stored in the global current_fault */
ಶೂನ್ಯ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#ಆದರೆ_ಅಂತ್ಯ
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_PRINTING
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಬರೆ("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#ಇಲ್ಲದಿದ್ದರೆ
    seL4_Fault_t ex2 = current_fault;
    ಬರೆ("Caught ");
    print_fault(ex2);
    ಬರೆ("\nwhile trying to handle:\n");
    print_fault(ex1);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    ಬರೆ("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_DEBUG_BUILD */

    ಬರೆ("at address %p\n", (ಶೂನ್ಯ *)getRestartPC(tptr));
    ಬರೆ("With stack:\n");
    Arch_user_ರಾಶಿ_Trace(tptr);
#ಆದರೆ_ಅಂತ್ಯ

    ಗಣ_Thread_State(tptr, ThreadState_Inactive);
}
