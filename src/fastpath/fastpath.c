/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <fastpath/fastpath.h>

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#அந்தர்பாவ <benchmark/benchmark_track.h>
#endif
#அந்தர்பாவ <benchmark/benchmark_utilisation.h>

#ifdef CONFIG_ARCH_ARM
நிலையான உள்வரி
FORCE_INLINE
#endif
வெறுமை NORETURN fastpath_call(word_t cptr, word_t செய்தி_Info)
{
    seL4_MessageInfo_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t length;
    tcb_t *dest;
    word_t badge;
    cap_t newVTable;
    vspace_வேர்_t *cap_pd;
    pde_t stored_hw_asid;
    word_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = messageInfoFromWord_raw(செய்தி_Info);
    length = seL4_MessageInfo_get_length(info);
    fault_type = seL4_Fault_get_seL4_FaultType(முனை_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    எனில் (unlikely(fastpath_mi_check(செய்தி_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    எனில் (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_epQueue_head(ep_ptr));

    /* Check that there's a thread waiting to receive */
    எனில் (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#எனில் defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    எனில் (unlikely(dest->tcbArch.tcbContext.breakpointState.single_step_enabled)) {
        slowpath(SysCall);
    }
#endif

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_வேர்_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    எனில் (unlikely(! is_Valid_VTable_வேர்_fp(newVTable))) {
        slowpath(SysCall);
    }

#ifdef CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#endif

#எனில் defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_வேர்_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    எனில் (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_வேர்_t *)asid_map_asid_map_vspace_get_vspace_வேர்(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.சொற்கள்[0] = asid;
#endif

#ifdef CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    எனில் (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_வேர்(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    எனில் (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.சொற்கள்[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#இல்லையெனில்
    stored_hw_asid.சொற்கள்[0] = asid;
#endif
#endif

#ifdef CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(newVTable);
    எனில் (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.சொற்கள்[0] = asid;
#endif

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    எனில் (unlikely(dest->tcbPriority < முனை_STATE(ksCurThread->tcbPriority) &&
                 !isHighestPrio(dom, dest->tcbPriority))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    எனில் (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_capCanGrantReply(ep_cap))) {
        slowpath(SysCall);
    }

#ifdef CONFIG_ARCH_AARCH32
    எனில் (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#endif

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    எனில் (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#ifdef CONFIG_KERNEL_MCS
    எனில் (unlikely(dest->tcbSchedContext != வெற்று)) {
        slowpath(SysCall);
    }

    reply_t *reply = thread_state_get_replyObject_np(dest->tcbState);
    எனில் (unlikely(reply == வெற்று)) {
        slowpath(SysCall);
    }
#endif

#ifdef ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    எனில் (unlikely(முனை_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#endif /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#endif

    /* Dequeue the destination. */
    endpoint_ptr_set_epQueue_head_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    எனில் (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = வெற்று;
#ifdef CONFIG_KERNEL_MCS
        dest->tcbEPNext = வெற்று;
#endif
    } இல்லையெனில் {
        endpoint_ptr_mset_epQueue_tail_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_set_tsType_np(&முனை_STATE(ksCurThread)->tcbState,
                                   ThreadState_BlockedOnReply);
#ifdef CONFIG_KERNEL_MCS
    thread_state_ptr_set_replyObject_np(&dest->tcbState, 0);
    thread_state_ptr_set_replyObject_np(&முனை_STATE(ksCurThread)->tcbState, REPLY_REF(reply));
    reply->replyTCB = முனை_STATE(ksCurThread);

    sched_context_t *sc = முனை_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    முனை_STATE(ksCurThread)->tcbSchedContext = வெற்று;

    reply_t *old_caller = sc->scReply;
    reply->replyPrev = call_அடுக்கு_new(REPLY_REF(sc->scReply), false);
    எனில் (unlikely(old_caller)) {
        old_caller->replyNext = call_அடுக்கு_new(REPLY_REF(reply), false);
    }
    reply->replyNext = call_அடுக்கு_new(SC_REF(sc), true);
    sc->scReply = reply;
#இல்லையெனில்
    /* Get sender reply slot */
    cte_t *replySlot = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbReply);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_reply_cap_ptr_new_np(&callerSlot->cap, replyCanGrant, 0,
                             TCB_REF(முனை_STATE(ksCurThread)));
    mdb_முனை_ptr_set_mdb_Prev_np(&callerSlot->cteMDBNode, CTE_REF(replySlot));
    mdb_முனை_ptr_mset_mdb_Next_mdb_Revocable_mdb_First_Badged(
        &replySlot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#endif

    fastpath_copy_mrs(length, முனை_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_set_tsType_np(&dest->tcbState,
                                   ThreadState_Running);
    switchToThread_fp(dest, cap_pd, stored_hw_asid);

    செய்தி_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

    fastpath_restore(badge, செய்தி_Info, முனை_STATE(ksCurThread));
}

#ifdef CONFIG_ARCH_ARM
நிலையான உள்வரி
FORCE_INLINE
#endif
#ifdef CONFIG_KERNEL_MCS
வெறுமை NORETURN fastpath_reply_recv(word_t cptr, word_t செய்தி_Info, word_t reply)
#இல்லையெனில்
வெறுமை NORETURN fastpath_reply_recv(word_t cptr, word_t செய்தி_Info)
#endif
{
    seL4_MessageInfo_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t length;
    tcb_t *caller;
    word_t badge;
    tcb_t *endpointTail;
    word_t fault_type;

    cap_t newVTable;
    vspace_வேர்_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = messageInfoFromWord_raw(செய்தி_Info);
    length = seL4_MessageInfo_get_length(info);
    fault_type = seL4_Fault_get_seL4_FaultType(முனை_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    எனில் (unlikely(fastpath_mi_check(செய்தி_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysReplyRecv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    எனில் (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(SysReplyRecv);
    }

#ifdef CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t reply_cap = lookup_fp(TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCTable)->cap, reply);

    /* check it's a reply object */
    எனில் (unlikely(!cap_capType_equals(reply_cap, cap_reply_cap))) {
        slowpath(SysReplyRecv);
    }
#endif

    /* Check there is nothing waiting on the notification */
    எனில் (unlikely(முனை_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(முனை_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active)) {
        slowpath(SysReplyRecv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    எனில் (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(SysReplyRecv);
    }

#ifdef CONFIG_KERNEL_MCS
    /* Get the reply address */
    reply_t *reply_ptr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(reply_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    எனில் (unlikely(reply_ptr->replyTCB == வெற்று ||
                 call_அடுக்கு_get_is_Head(reply_ptr->replyNext) == 0 ||
                 SC_PTR(call_அடுக்கு_get_call_அடுக்கு_Ptr(reply_ptr->replyNext)) != முனை_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(SysReplyRecv);
    }

    /* Determine who the caller is. */
    caller = reply_ptr->replyTCB;
#இல்லையெனில்
    /* Only reply if the reply cap is valid. */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = callerSlot->cap;
    எனில் (unlikely(!fastpath_reply_cap_check(callerCap))) {
        slowpath(SysReplyRecv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
#endif

    /* ensure we are not single stepping the caller in ia32 */
#எனில் defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    எனில் (unlikely(caller->tcbArch.tcbContext.breakpointState.single_step_enabled)) {
        slowpath(SysReplyRecv);
    }
#endif

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#ifndef CONFIG_EXCEPTION_FASTPATH
    எனில் (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysReplyRecv);
    }
#இல்லையெனில்
    எனில் (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(SysReplyRecv);
    }
#endif

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_வேர்_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    எனில் (unlikely(! is_Valid_VTable_வேர்_fp(newVTable))) {
        slowpath(SysReplyRecv);
    }

#ifdef CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#endif

#எனில் defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_வேர்_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    எனில் (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_வேர்_t *)asid_map_asid_map_vspace_get_vspace_வேர்(asid_map) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.சொற்கள்[0] = asid;
#endif

#ifdef CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    எனில் (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_வேர்(asid_map)) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    எனில் (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysReplyRecv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.சொற்கள்[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#இல்லையெனில்
    stored_hw_asid.சொற்கள்[0] = asid;
#endif
#endif

#ifdef CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(newVTable);
    எனில் (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
    /* Get HW ASID */
    stored_hw_asid.சொற்கள்[0] = asid;
#endif

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    எனில் (unlikely(!isHighestPrio(dom, caller->tcbPriority))) {
        slowpath(SysReplyRecv);
    }

#ifdef CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    எனில் (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysReplyRecv);
    }
#endif

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    எனில் (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysReplyRecv);
    }

#ifdef CONFIG_KERNEL_MCS
    எனில் (unlikely(caller->tcbSchedContext != வெற்று)) {
        slowpath(SysReplyRecv);
    }
#endif

#ifdef ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    எனில் (unlikely(முனை_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(SysReplyRecv);
    }
#endif /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#endif

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &முனை_STATE(ksCurThread)->tcbState, (word_t)ep_ptr, ThreadState_BlockedOnReceive);
#ifdef CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_set_replyObject_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_set_replyObject_np(&முனை_STATE(ksCurThread)->tcbState, REPLY_REF(reply_ptr));
    reply_ptr->replyTCB = முனை_STATE(ksCurThread);
#இல்லையெனில்
    thread_state_ptr_set_blockingIPCCanGrant(&முனை_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#endif

    /* Place the thread in the endpoint queue */
    endpointTail = endpoint_ptr_get_epQueue_tail_fp(ep_ptr);
    எனில் (likely(!endpointTail)) {
        முனை_STATE(ksCurThread)->tcbEPPrev = வெற்று;
        முனை_STATE(ksCurThread)->tcbEPNext = வெற்று;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_set_epQueue_head_np(ep_ptr, TCB_REF(முனை_STATE(ksCurThread)));
        endpoint_ptr_mset_epQueue_tail_state(ep_ptr, TCB_REF(முனை_STATE(ksCurThread)),
                                             EPState_Recv);
    } இல்லையெனில் {
#ifdef CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_queue_t queue = tcbAppend(முனை_STATE(ksCurThread), ep_ptr_get_queue(ep_ptr));
        endpoint_ptr_set_epQueue_head_np(ep_ptr, TCB_REF(queue.head));
        endpoint_ptr_mset_epQueue_tail_state(ep_ptr, TCB_REF(queue.end), EPState_Recv);
#இல்லையெனில்
        /* Append current thread onto the queue. */
        endpointTail->tcbEPNext = முனை_STATE(ksCurThread);
        முனை_STATE(ksCurThread)->tcbEPPrev = endpointTail;
        முனை_STATE(ksCurThread)->tcbEPNext = வெற்று;

        /* Update tail of queue. */
        endpoint_ptr_mset_epQueue_tail_state(ep_ptr, TCB_REF(முனை_STATE(ksCurThread)),
                                             EPState_Recv);
#endif
    }

#ifdef CONFIG_KERNEL_MCS
    /* update call stack */
    word_t prev_ptr = call_அடுக்கு_get_call_அடுக்கு_Ptr(reply_ptr->replyPrev);
    sched_context_t *sc = முனை_STATE(ksCurThread)->tcbSchedContext;
    முனை_STATE(ksCurThread)->tcbSchedContext = வெற்று;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->scReply = REPLY_PTR(prev_ptr);
    எனில் (unlikely(REPLY_PTR(prev_ptr) != வெற்று)) {
        sc->scReply->replyNext = reply_ptr->replyNext;
    }

    /* TODO neccessary? */
    reply_ptr->replyPrev.சொற்கள்[0] = 0;
    reply_ptr->replyNext.சொற்கள்[0] = 0;
#இல்லையெனில்
    /* Delete the reply cap. */
    mdb_முனை_ptr_mset_mdb_Next_mdb_Revocable_mdb_First_Badged(
        &CTE_PTR(mdb_முனை_get_mdb_Prev(callerSlot->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    callerSlot->cap = cap_null_cap_new();
    callerSlot->cteMDBNode = nullMDBNode;
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
    எனில் (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        word_t pc = getRestartPC(caller);
        setNextPC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_set_tsType_np(&caller->tcbState, ThreadState_Running);
        switchToThread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } இல்லையெனில் {
#endif
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(length, முனை_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_set_tsType_np(&caller->tcbState, ThreadState_Running);
        switchToThread_fp(caller, cap_pd, stored_hw_asid);

        செய்தி_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

        fastpath_restore(badge, செய்தி_Info, முனை_STATE(ksCurThread));

#ifdef CONFIG_EXCEPTION_FASTPATH
    }
#endif
}

#ifdef CONFIG_SIGNAL_FASTPATH
#ifdef CONFIG_ARCH_ARM
நிலையான உள்வரி
FORCE_INLINE
#endif
வெறுமை NORETURN fastpath_signal(word_t cptr, word_t செய்தி_Info)
{
    word_t fault_type;
    sched_context_t *sc = வெற்று;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = வெற்று;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(முனை_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    எனில் (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    எனில் (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    எனில் (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    எனில் (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    word_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    தேர்வு (ntfnState) {
    நிலை NtfnState_Active:
#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = true;
#endif
        ntfn_set_active(ntfnPtr, badge | notification_ptr_get_ntfn_செய்தி_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    நிலை NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        எனில் (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ksKernelEntry.is_fastpath = true;
#endif
            ntfn_set_active(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        நிறுத்து;
    நிலை NtfnState_Waiting:
        dest = TCB_PTR(notification_ptr_get_ntfnQueue_head(ntfnPtr));
        நிறுத்து;
    இயல்பு:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    எனில் (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        எனில் (sc == வெற்று || sc->scTcb != வெற்று) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#எனில் defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        எனில் (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#endif
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    எனில் (முனை_STATE_ON_CORE(ksCurThread, sc->scCore)->tcbPriority < dest->tcbPriority) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    எனில் (sc->scRefillMax > 0) {
        எனில் (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    எனில் (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#endif

    எனில் (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } இல்லையெனில் {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    setRegister(dest, badgeRegister, badge);
    thread_state_ptr_set_tsType_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    எனில் (sc_sporadic(dest->tcbSchedContext)) {
        ப்ரதிஜ்ஞா(dest->tcbSchedContext != முனை_STATE(ksCurSC));
        எனில் (dest->tcbSchedContext != முனை_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    எனில் (schedulable) {
        எனில் (முனை_STATE(ksCurThread)->tcbPriority > dest->tcbPriority || crossnode) {
            SCHED_ENQUEUE(dest);
        } இல்லையெனில் {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#endif

#ifdef CONFIG_EXCEPTION_FASTPATH
நிலையான உள்வரி
FORCE_INLINE
வெறுமை NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_வேர்_t *cap_pd;
    word_t badge;
    seL4_MessageInfo_t info;
    word_t செய்தி_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#ifdef CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbFaultHandler)->cap;
#இல்லையெனில்
    cptr_t handlerCPtr;
    handlerCPtr = முனை_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#endif

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    எனில் (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#ifndef CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_capCanGrantReply(handler_cap))
#endif
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_epQueue_head(ep_ptr));

    /* Check that there's a thread waiting to receive */
    எனில் (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_வேர்_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    எனில் (unlikely(! is_Valid_VTable_வேர்_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#ifdef CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    எனில் (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_வேர்(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    எனில் (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.சொற்கள்[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#இல்லையெனில்
    stored_hw_asid.சொற்கள்[0] = asid;
#endif
#endif

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    எனில் (unlikely(dest->tcbPriority < முனை_STATE(ksCurThread->tcbPriority) &&
                 !isHighestPrio(dom, dest->tcbPriority))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    எனில் (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#ifdef CONFIG_KERNEL_MCS
    எனில் (unlikely(dest->tcbSchedContext != வெற்று)) {
        vm_fault_slowpath(type);
    }

    reply_t *reply = thread_state_get_replyObject_np(dest->tcbState);
    எனில் (unlikely(reply == வெற்று)) {
        vm_fault_slowpath(type);
    }
#endif

#ifdef ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    எனில் (unlikely(முனை_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#endif /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_set_tcbfault_vm_fault(type);

#ifdef CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#endif

    /* Dequeue the destination. */
    endpoint_ptr_set_epQueue_head_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    எனில் (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = வெற்று;
#ifdef CONFIG_KERNEL_MCS
        dest->tcbEPNext = வெற்று;
#endif
    } இல்லையெனில் {
        endpoint_ptr_mset_epQueue_tail_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_set_tsType_np(&முனை_STATE(ksCurThread)->tcbState, ThreadState_BlockedOnReply);
#ifdef CONFIG_KERNEL_MCS

    thread_state_ptr_set_replyObject_np(&dest->tcbState, 0);
    thread_state_ptr_set_replyObject_np(&முனை_STATE(ksCurThread)->tcbState, REPLY_REF(reply));
    reply->replyTCB = முனை_STATE(ksCurThread);

    sched_context_t *sc = முனை_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    முனை_STATE(ksCurThread)->tcbSchedContext = வெற்று;

    reply_t *old_caller = sc->scReply;
    reply->replyPrev = call_அடுக்கு_new(REPLY_REF(sc->scReply), false);
    எனில் (unlikely(old_caller)) {
        old_caller->replyNext = call_அடுக்கு_new(REPLY_REF(reply), false);
    }
    reply->replyNext = call_அடுக்கு_new(SC_REF(sc), true);
    sc->scReply = reply;
#இல்லையெனில்
    /* Get sender reply slot */
    cte_t *replySlot = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbReply);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_reply_cap_ptr_new_np(&callerSlot->cap, replyCanGrant, 0, TCB_REF(முனை_STATE(ksCurThread)));
    mdb_முனை_ptr_set_mdb_Prev_np(&callerSlot->cteMDBNode, CTE_REF(replySlot));
    mdb_முனை_ptr_mset_mdb_Next_mdb_Revocable_mdb_First_Badged(&replySlot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#endif
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_set_mrs(dest);

    /* Generate the msginfo */
    info = seL4_MessageInfo_new(seL4_Fault_VMFault, 0, 0, seL4_VMFault_Length);

    /* Set the fault handler to running */
    thread_state_ptr_set_tsType_np(&dest->tcbState, ThreadState_Running);
    switchToThread_fp(dest, cap_pd, stored_hw_asid);
    செய்தி_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

    fastpath_restore(badge, செய்தி_Info, முனை_STATE(ksCurThread));
}
#endif
