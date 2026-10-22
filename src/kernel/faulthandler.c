/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/faulthandler.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <arch/machine.h>

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య handleFault(tcb_t *tptr)
{
    cap_t faultHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbFaultHandler)->cap;
    bool_t hasFaultHandler = sendFaultIPC(tptr, faultHandlerCap, tptr->tcbSchedContext != ఖాళీ);
    ఒకవేళ (!hasFaultHandler) {
        handleNoFaultHandler(tptr);
    }
}

శూన్య handleTimeout(tcb_t *tptr)
{
    ప్రతిజ్ఞా(validTimeoutHandler(tptr));
    cap_t timeoutHandlerCap = TCB_PTR_CTE_PTR(tptr, tcbTimeoutHandler)->cap;
    sendFaultIPC(tptr, timeoutHandlerCap, false);
}

bool_t sendFaultIPC(tcb_t *tptr, cap_t handlerCap, bool_t can_donate)
{
    ఒకవేళ (cap_get_capType(handlerCap) == cap_endpoint_cap) {
        ప్రతిజ్ఞా(validFaultHandler(handlerCap));

        tptr->tcbFault = current_fault;
        ఒకవేళ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = current_lookup_fault;
        }
        sendIPC(true, false,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap),
                cap_endpoint_cap_get_cap_Can_Grant_జవాబు(handlerCap),
                can_donate, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        తిరిగి true;
    } లేకపోతే {
        ప్రతిజ్ఞా(cap_get_capType(handlerCap) == cap_null_cap);
        తిరిగి false;
    }
}
#లేకపోతే

శూన్య handleFault(tcb_t *tptr)
{
    exception_t స్థితి_2;
    seL4_Fault_t fault = current_fault;

    స్థితి_2 = sendFaultIPC(tptr);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
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
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        తిరిగి EXCEPTION_FAULT;
    }
    handlerCap = lu_ret.cap;

    ఒకవేళ (cap_get_capType(handlerCap) == cap_endpoint_cap &&
        cap_endpoint_cap_get_capCanSend(handlerCap) &&
        (cap_endpoint_cap_get_capCanGrant(handlerCap) ||
         cap_endpoint_cap_get_cap_Can_Grant_జవాబు(handlerCap))) {
        tptr->tcbFault = current_fault;
        ఒకవేళ (seL4_Fault_get_seL4_FaultType(current_fault) == seL4_Fault_CapFault) {
            tptr->tcbLookupFailure = original_lookup_fault;
        }
        sendIPC(true, true,
                cap_endpoint_cap_get_capEPBadge(handlerCap),
                cap_endpoint_cap_get_capCanGrant(handlerCap), true, tptr,
                EP_PTR(cap_endpoint_cap_get_capEPPtr(handlerCap)));

        తిరిగి EXCEPTION_NONE;
    } లేకపోతే {
        current_fault = seL4_Fault_CapFault_new(handlerCPtr, false);
        current_lookup_fault = lookup_fault_missing_capability_new(0);

        తిరిగి EXCEPTION_FAULT;
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_PRINTING
స్థిర శూన్య print_fault(seL4_Fault_t f)
{
    ఎంపిక (seL4_Fault_get_seL4_FaultType(f)) {
    స్థితి seL4_Fault_NullFault:
        రాయి("null fault");
        ఆపు;
    స్థితి seL4_Fault_CapFault:
        రాయి("cap fault in %s phase at address %p",
               se_L4_Fault_Cap_Fault_get_లోపల_Receive_Phase(f) ? "receive" : "send",
               (శూన్య *)se_L4_Fault_Cap_Fault_get_చిరునామా(f));
        ఆపు;
    స్థితి seL4_Fault_VMFault:
        రాయి("vm fault on %s at address %p with status %p",
               seL4_Fault_VMFault_get_instructionFault(f) ? "code" : "data",
               (శూన్య *)se_L4_Fault_VMFault_get_చిరునామా(f),
               (శూన్య *)seL4_Fault_VMFault_get_FSR(f));
        ఆపు;
    స్థితి seL4_Fault_UnknownSyscall:
        రాయి("unknown syscall %p",
               (శూన్య *)se_L4_Fault_Unknown_Syscall_get_syscall_సంఖ్య(f));
        ఆపు;
    స్థితి seL4_Fault_UserException:
        రాయి("user exception %p code %p",
               (శూన్య *)se_L4_Fault_User_Exception_get_సంఖ్య(f),
               (శూన్య *)se_L4_Fault_User_Exception_get_సంకేతం(f));
        ఆపు;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    స్థితి seL4_Fault_Timeout:
        రాయి("Timeout fault for 0x%x\n", (గుర్తులేని పూర్ణాంకం) seL4_Fault_Timeout_get_badge(f));
        ఆపు;
#అయితే_ముగింపు
    అప్రమేయ:
        రాయి("unknown fault");
        ఆపు;
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య handleNoFaultHandler(tcb_t *tptr)
#లేకపోతే
/* The second fault, ex2, is stored in the global current_fault */
శూన్య handleDoubleFault(tcb_t *tptr, seL4_Fault_t ex1)
#అయితే_ముగింపు
{
#నిర్వచితమైతే CONFIG_PRINTING
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    రాయి("Found thread has no fault handler while trying to handle:\n");
    print_fault(current_fault);
#లేకపోతే
    seL4_Fault_t ex2 = current_fault;
    రాయి("Caught ");
    print_fault(ex2);
    రాయి("\nwhile trying to handle:\n");
    print_fault(ex1);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    రాయి("\nin thread %p \"%s\" ", tptr, TCB_PTR_DEBUG_PTR(tptr)->tcbName);
#అయితే_ముగింపు /* CONFIG_DEBUG_BUILD */

    రాయి("at address %p\n", (శూన్య *)getRestartPC(tptr));
    రాయి("With stack:\n");
    Arch_user_దొంతర_Trace(tptr);
#అయితే_ముగింపు

    సమితి_Thread_State(tptr, ThreadState_Inactive);
}
