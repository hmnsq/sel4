/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <kernel/faulthandler.h>
#અન્તર્ભાવ <kernel/thread.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <arch/machine.h>

#ifdef CONFIG_KERNEL_MCS
શૂન્ય handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ખાલી);
    જો (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

શૂન્ય handleTimeout(tcb_t *tptr)
{
    પ્રતિજ્ઞા(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    જો (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        પ્રતિજ્ઞા(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        જો (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_capCanGrantReply(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        પાછા true;
    } નહીંતર {
        પ્રતિજ્ઞા(cap_get_capType(handlerCap) == cap_null_cap);
        પાછા false;
    }
}
#નહીંતર

શૂન્ય handleFault(tcb_t *tptr)
{
    exception_t status;
    seL4_Fault_t fault = current_fault;

    status = sendFaultIPC(tptr);
    જો (status != EXCEPTION_NONE) {
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
    જો (lu_ret.status != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        પાછા EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    જો (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_capCanGrantReply(handlerCap))) {
        tptr->tcbFault = current_fault;
        જો (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        પાછા EXCEPTION_NONE;
    } નહીંતર {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        પાછા EXCEPTION_FAULT;
    }
}
#endif

#ifdef CONFIG_PRINTING
સ્થિર શૂન્ય print_fault(seL4_Fault_t f)
{
    પસંદગી (seL4_Fault_get_seL4_FaultType(f)) {
    સ્થિતિ seL4_Fault_NullFault:
        printf("null fault");
        તોડો;
    સ્થિતિ seL4_Fault_CapFault:
        printf("cap fault in %s phase at address %p",
               seL4_Fault_CapFault_get_inReceivePhase(f) ? "receive" : "send",
               (શૂન્ય *)seL4_Fault_CapFault_get_address(f));
        તોડો;
    સ્થિતિ seL4_Fault_VMFault:
        printf("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (શૂન્ય *)seL4_Fault_VMFault_get_address(f),
               (શૂન્ય *)seL4_Fault_VMFault_get_FSR(f));
        તોડો;
    સ્થિતિ seL4_Fault_UnknownSyscall:
        printf("unknown syscall %p",
               (શૂન્ય *)se_L4_Fault_Unknown_Syscall_get_syscall_સંખ્યા(f));
        તોડો;
    સ્થિતિ seL4_Fault_UserException:
        printf("user exception %p code %p",
               (શૂન્ય *)se_L4_Fault_User_Exception_get_સંખ્યા(f),
               (શૂન્ય *)seL4_Fault_UserException_get_code(f));
        તોડો;
#ifdef CONFIG_KERNEL_MCS
    સ્થિતિ seL4_Fault_Timeout:
        printf("Timeout fault for 0x%x\n", (unsigned પૂર્ણાંક) seL4_Fault_Timeout_get_badge(f));
        તોડો;
#endif
    મૂળભૂત:
        printf("unknown fault");
        તોડો;
    }
}
#endif

#ifdef CONFIG_KERNEL_MCS
શૂન્ય handleNoFaultHandler(tcb_t *tptr)
#નહીંતર
/* The second fault, ex2, is stored in the global current_fault */
શૂન્ય handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#endif
{
#ifdef CONFIG_PRINTING
#ifdef CONFIG_KERNEL_MCS
    printf("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#નહીંતર
    seL4_Fault_t ex2 = current_fault;
    printf("Caught ");
    print_fault(ex2);
    printf("\nwhile trying to handle:\n");
    print_fault(ex1);
#endif
#ifdef CONFIG_DEBUG_BUILD
    printf("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#endif /* CONFIG_DEBUG_BUILD */

    printf("at address %p\n", (શૂન્ય *)getRestartPC(tptr));
    printf("With stack:\n");
    Arch_user_ઢગલો_Trace(tptr);
#endif

    setThreadState(tptr, ThreadState_Inactive);
}
