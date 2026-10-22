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

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ರಿಕ್ತ);
    ಯದಿ (!hasFaultHandler) {
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
    ಯದಿ (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        ಪ್ರತಿಜ್ಞಾ(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        ಯದಿ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ಪ್ರತ್ಯಾಗಮ true;
    } ಅನ್ಯಥಾ {
        ಪ್ರತಿಜ್ಞಾ(cap_get_capType(handlerCap) == cap_null_cap);
        ಪ್ರತ್ಯಾಗಮ false;
    }
}
#ಅನ್ಯಥಾ

ಶೂನ್ಯ handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    ಯದಿ (status != EXCEPTION_NONE) {
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
    ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    ಯದಿ (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        ಯದಿ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    } ಅನ್ಯಥಾ {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        ಪ್ರತ್ಯಾಗಮ EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
ಸ್ಥಿರ ಶೂನ್ಯ print_fault(seL4_Fault_t f)
{
    ವಿಕಲ್ಪ (seL4_Fault_get_seL4_FaultType(f)) {
    ಪಕ್ಷ seL4_Fault_NullFault:
        printf("null fault");
        ಭಙ್ಗ;
    ಪಕ್ಷ seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (ಶೂನ್ಯ *)seL4_Fault_CapFault_get_address(f));
        ಭಙ್ಗ;
    ಪಕ್ಷ seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (ಶೂನ್ಯ *)seL4_Fault_VMFault_get_address(f),
               (ಶೂನ್ಯ *)seL4_Fault_VMFault_get_FSR(f));
        ಭಙ್ಗ;
    ಪಕ್ಷ seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (ಶೂನ್ಯ *)se_L4_Fault_Unknown_Syscall_get_syscall_ಸಂಖ್ಯೆ(f));
        ಭಙ್ಗ;
    ಪಕ್ಷ seL4_Fault_UserException:
        printf("user exception %p code %p",
               (ಶೂನ್ಯ *)se_L4_Fault_User_Exception_get_ಸಂಖ್ಯೆ(f),
               (ಶೂನ್ಯ *)seL4_Fault_UserException_get_code(f));
        ಭಙ್ಗ;
#ifdef CONFIG_KERNEL_MCS
    ಪಕ್ಷ seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned ಪೂರ್ಣಾಙ್ಕ) seL4_Fault_Timeout_get_badge(f));
        ಭಙ್ಗ;
#endif
    ಸ್ವಭಾವ:
        printf("unknown fault");
        ಭಙ್ಗ;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
ಶೂನ್ಯ handleNoFaultHandler(tcb_t *tptr)
#ಅನ್ಯಥಾ
/* The second fault, ex2, is stored in the global current_fault */
ಶೂನ್ಯ handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#ಅನ್ಯಥಾ
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (ಶೂನ್ಯ *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_ರಾಶಿ_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
