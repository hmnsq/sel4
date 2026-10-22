/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <fastpath/fastpath.h>

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#समावेश <benchmark/benchmark_track.h>
#पूर्ण_अगर
#समावेश <benchmark/benchmark_utilisation.h>

#अगर_घोषित CONFIG_ARCH_ARM
अटल अंतरभूत
FORCE_INLINE
#पूर्ण_अगर
व्योम NORETURN fastpath_call(word_t cptr, word_t संदेश_Info)
{
    seL4_MessageInfo_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t लंबाई;
    tcb_t *dest;
    word_t badge;
    cap_t newVTable;
    vspace_मूल_t *cap_pd;
    pde_t stored_hw_asid;
    word_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = messageInfoFromWord_raw(संदेश_Info);
    लंबाई = se_L4_Message_Info_get_लंबाई(info);
    fault_type = seL4_Fault_get_seL4_FaultType(गाँठ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    अगर (unlikely(fastpath_mi_check(संदेश_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    अगर (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(ep_ptr));

    /* Check that there's a thread waiting to receive */
    अगर (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#अगर defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    अगर (unlikely(dest->tcbArch.tcbContext.breakpointState.single_step_enabled)) {
        slowpath(SysCall);
    }
#पूर्ण_अगर

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूल_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    अगर (unlikely(! is_Valid_VTable_मूल_fp(newVTable))) {
        slowpath(SysCall);
    }

#अगर_घोषित CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#पूर्ण_अगर

#अगर defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_मूल_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    अगर (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_मूल_t *)asid_map_asid_map_vspace_get_vspace_मूल(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    अगर (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूल(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    अगर (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दगण[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(newVTable);
    अगर (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    अगर (unlikely(dest->tcbPriority < गाँठ_STATE(ksCurThread->tcbPriority) &&
                 !isHighestPrio(dom, dest->tcbPriority))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    अगर (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_capCanGrantReply(ep_cap))) {
        slowpath(SysCall);
    }

#अगर_घोषित CONFIG_ARCH_AARCH32
    अगर (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#पूर्ण_अगर

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    अगर (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (unlikely(dest->tcbSchedContext != शून्य)) {
        slowpath(SysCall);
    }

    reply_t *reply = thread_state_get_replyObject_np(dest->tcbState);
    अगर (unlikely(reply == शून्य)) {
        slowpath(SysCall);
    }
#पूर्ण_अगर

#अगर_घोषित ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    अगर (unlikely(गाँठ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर

    /* Dequeue the destination. */
    endpoint_ptr_set_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    अगर (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = शून्य;
#अगर_घोषित CONFIG_KERNEL_MCS
        dest->tcbEPNext = शून्य;
#पूर्ण_अगर
    } अन्यथा {
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_set_tsType_np(&गाँठ_STATE(ksCurThread)->tcbState,
                                   ThreadState_BlockedOnReply);
#अगर_घोषित CONFIG_KERNEL_MCS
    thread_state_ptr_set_replyObject_np(&dest->tcbState, 0);
    thread_state_ptr_set_replyObject_np(&गाँठ_STATE(ksCurThread)->tcbState, REPLY_REF(reply));
    reply->replyTCB = गाँठ_STATE(ksCurThread);

    sched_context_t *sc = गाँठ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    गाँठ_STATE(ksCurThread)->tcbSchedContext = शून्य;

    reply_t *old_caller = sc->scReply;
    reply->reply_पूर्व = call_ढेर_new(REPLY_REF(sc->scReply), false);
    अगर (unlikely(old_caller)) {
        old_caller->reply_अग्र = call_ढेर_new(REPLY_REF(reply), false);
    }
    reply->reply_अग्र = call_ढेर_new(SC_REF(sc), true);
    sc->scReply = reply;
#अन्यथा
    /* Get sender reply slot */
    cte_t *replySlot = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbReply);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_reply_cap_ptr_new_np(&callerSlot->cap, replyCanGrant, 0,
                             TCB_REF(गाँठ_STATE(ksCurThread)));
    mdb_गाँठ_ptr_set_mdb_पूर्व_np(&callerSlot->cteMDBNode, CTE_REF(replySlot));
    mdb_गाँठ_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(
        &replySlot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#पूर्ण_अगर

    fastpath_copy_mrs(लंबाई, गाँठ_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_set_tsType_np(&dest->tcbState,
                                   ThreadState_Running);
    switchToThread_fp(dest, cap_pd, stored_hw_asid);

    संदेश_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

    fastpath_restore(badge, संदेश_Info, गाँठ_STATE(ksCurThread));
}

#अगर_घोषित CONFIG_ARCH_ARM
अटल अंतरभूत
FORCE_INLINE
#पूर्ण_अगर
#अगर_घोषित CONFIG_KERNEL_MCS
व्योम NORETURN fastpath_reply_recv(word_t cptr, word_t संदेश_Info, word_t reply)
#अन्यथा
व्योम NORETURN fastpath_reply_recv(word_t cptr, word_t संदेश_Info)
#पूर्ण_अगर
{
    seL4_MessageInfo_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t लंबाई;
    tcb_t *caller;
    word_t badge;
    tcb_t *endpoint_पुच्छ;
    word_t fault_type;

    cap_t newVTable;
    vspace_मूल_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = messageInfoFromWord_raw(संदेश_Info);
    लंबाई = se_L4_Message_Info_get_लंबाई(info);
    fault_type = seL4_Fault_get_seL4_FaultType(गाँठ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    अगर (unlikely(fastpath_mi_check(संदेश_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysReplyRecv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    अगर (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(SysReplyRecv);
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t reply_cap = lookup_fp(TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCTable)->cap, reply);

    /* check it's a reply object */
    अगर (unlikely(!cap_capType_equals(reply_cap, cap_reply_cap))) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर

    /* Check there is nothing waiting on the notification */
    अगर (unlikely(गाँठ_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(गाँठ_STATE(ksCurThread)->tcbBoundNotification) == NtfnState_Active)) {
        slowpath(SysReplyRecv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    अगर (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(SysReplyRecv);
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    /* Get the reply address */
    reply_t *reply_ptr = REPLY_PTR(cap_reply_cap_get_capReplyPtr(reply_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    अगर (unlikely(reply_ptr->replyTCB == शून्य ||
                 call_ढेर_get_is_शीर्ष(reply_ptr->reply_अग्र) == 0 ||
                 SC_PTR(call_ढेर_get_call_ढेर_Ptr(reply_ptr->reply_अग्र)) != गाँठ_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(SysReplyRecv);
    }

    /* Determine who the caller is. */
    caller = reply_ptr->replyTCB;
#अन्यथा
    /* Only reply if the reply cap is valid. */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = callerSlot->cap;
    अगर (unlikely(!fastpath_reply_cap_check(callerCap))) {
        slowpath(SysReplyRecv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_reply_cap_get_capTCBPtr(callerCap));
#पूर्ण_अगर

    /* ensure we are not single stepping the caller in ia32 */
#अगर defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    अगर (unlikely(caller->tcbArch.tcbContext.breakpointState.single_step_enabled)) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#अगर_अघोषित CONFIG_EXCEPTION_FASTPATH
    अगर (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysReplyRecv);
    }
#अन्यथा
    अगर (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूल_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    अगर (unlikely(! is_Valid_VTable_मूल_fp(newVTable))) {
        slowpath(SysReplyRecv);
    }

#अगर_घोषित CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#पूर्ण_अगर

#अगर defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_मूल_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    अगर (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_मूल_t *)asid_map_asid_map_vspace_get_vspace_मूल(asid_map) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    अगर (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूल(asid_map)) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    अगर (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysReplyRecv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दगण[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_table_cap_get_capPTMappedASID(newVTable);
    अगर (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(SysReplyRecv);
    }
    /* Get HW ASID */
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    अगर (unlikely(!isHighestPrio(dom, caller->tcbPriority))) {
        slowpath(SysReplyRecv);
    }

#अगर_घोषित CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    अगर (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    अगर (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysReplyRecv);
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (unlikely(caller->tcbSchedContext != शून्य)) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर

#अगर_घोषित ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    अगर (unlikely(गाँठ_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(SysReplyRecv);
    }
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &गाँठ_STATE(ksCurThread)->tcbState, (word_t)ep_ptr, ThreadState_BlockedOnReceive);
#अगर_घोषित CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_set_replyObject_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_set_replyObject_np(&गाँठ_STATE(ksCurThread)->tcbState, REPLY_REF(reply_ptr));
    reply_ptr->replyTCB = गाँठ_STATE(ksCurThread);
#अन्यथा
    thread_state_ptr_set_blockingIPCCanGrant(&गाँठ_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#पूर्ण_अगर

    /* Place the thread in the endpoint queue */
    endpoint_पुच्छ = endpoint_ptr_get_ep_Queue_पुच्छ_fp(ep_ptr);
    अगर (likely(!endpoint_पुच्छ)) {
        गाँठ_STATE(ksCurThread)->tcbEPPrev = शून्य;
        गाँठ_STATE(ksCurThread)->tcbEPNext = शून्य;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_set_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(गाँठ_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(गाँठ_STATE(ksCurThread)),
                                             EPState_Recv);
    } अन्यथा {
#अगर_घोषित CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_queue_t queue = tcbAppend(गाँठ_STATE(ksCurThread), ep_ptr_get_queue(ep_ptr));
        endpoint_ptr_set_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(queue.शीर्ष));
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(queue.end), EPState_Recv);
#अन्यथा
        /* Append current thread onto the queue. */
        endpoint_पुच्छ->tcbEPNext = गाँठ_STATE(ksCurThread);
        गाँठ_STATE(ksCurThread)->tcbEPPrev = endpoint_पुच्छ;
        गाँठ_STATE(ksCurThread)->tcbEPNext = शून्य;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(गाँठ_STATE(ksCurThread)),
                                             EPState_Recv);
#पूर्ण_अगर
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    /* update call stack */
    word_t पूर्व_ptr = call_ढेर_get_call_ढेर_Ptr(reply_ptr->reply_पूर्व);
    sched_context_t *sc = गाँठ_STATE(ksCurThread)->tcbSchedContext;
    गाँठ_STATE(ksCurThread)->tcbSchedContext = शून्य;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->scReply = REPLY_PTR(पूर्व_ptr);
    अगर (unlikely(REPLY_PTR(पूर्व_ptr) != शून्य)) {
        sc->scReply->reply_अग्र = reply_ptr->reply_अग्र;
    }

    /* TODO neccessary? */
    reply_ptr->reply_पूर्व.शब्दगण[0] = 0;
    reply_ptr->reply_अग्र.शब्दगण[0] = 0;
#अन्यथा
    /* Delete the reply cap. */
    mdb_गाँठ_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(
        &CTE_PTR(mdb_गाँठ_get_mdb_पूर्व(callerSlot->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    callerSlot->cap = cap_null_cap_new();
    callerSlot->cteMDBNode = nullMDBNode;
#पूर्ण_अगर

#अगर_घोषित CONFIG_EXCEPTION_FASTPATH
    अगर (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        word_t pc = getRestartPC(caller);
        set_अग्र_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_set_tsType_np(&caller->tcbState, ThreadState_Running);
        switchToThread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } अन्यथा {
#पूर्ण_अगर
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(लंबाई, गाँठ_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_set_tsType_np(&caller->tcbState, ThreadState_Running);
        switchToThread_fp(caller, cap_pd, stored_hw_asid);

        संदेश_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

        fastpath_restore(badge, संदेश_Info, गाँठ_STATE(ksCurThread));

#अगर_घोषित CONFIG_EXCEPTION_FASTPATH
    }
#पूर्ण_अगर
}

#अगर_घोषित CONFIG_SIGNAL_FASTPATH
#अगर_घोषित CONFIG_ARCH_ARM
अटल अंतरभूत
FORCE_INLINE
#पूर्ण_अगर
व्योम NORETURN fastpath_signal(word_t cptr, word_t संदेश_Info)
{
    word_t fault_type;
    sched_context_t *sc = शून्य;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = शून्य;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(गाँठ_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    अगर (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    अगर (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    अगर (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    अगर (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    word_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    चयन (ntfnState) {
    हाल NtfnState_Active:
#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर
        ntfn_set_active(ntfnPtr, badge | notification_ptr_get_ntfn_संदेश_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    हाल NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        अगर (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर
            ntfn_set_active(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        अवरोध;
    हाल NtfnState_Waiting:
        dest = TCB_PTR(notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr));
        अवरोध;
    शेष:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    अगर (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        अगर (sc == शून्य || sc->scTcb != शून्य) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#अगर defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        अगर (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#पूर्ण_अगर
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    अगर (गाँठ_STATE_ON_CORE(ksCurThread, sc->scCore)->tcbPriority < dest->tcbPriority) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    अगर (sc->scRefillMax > 0) {
        अगर (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    अगर (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर

    अगर (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } अन्यथा {
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
    अगर (sc_sporadic(dest->tcbSchedContext)) {
        निश्चित(dest->tcbSchedContext != गाँठ_STATE(ksCurSC));
        अगर (dest->tcbSchedContext != गाँठ_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    अगर (schedulable) {
        अगर (गाँठ_STATE(ksCurThread)->tcbPriority > dest->tcbPriority || crossnode) {
            SCHED_ENQUEUE(dest);
        } अन्यथा {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#पूर्ण_अगर

#अगर_घोषित CONFIG_EXCEPTION_FASTPATH
अटल अंतरभूत
FORCE_INLINE
व्योम NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_मूल_t *cap_pd;
    word_t badge;
    seL4_MessageInfo_t info;
    word_t संदेश_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#अगर_घोषित CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbFaultHandler)->cap;
#अन्यथा
    cptr_t handlerCPtr;
    handlerCPtr = गाँठ_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#पूर्ण_अगर

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    अगर (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#अगर_अघोषित CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_capCanGrantReply(handler_cap))
#पूर्ण_अगर
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(ep_ptr));

    /* Check that there's a thread waiting to receive */
    अगर (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूल_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    अगर (unlikely(! is_Valid_VTable_मूल_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#अगर_घोषित CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    अगर (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूल(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    अगर (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दगण[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दगण[0] = asid;
#पूर्ण_अगर
#पूर्ण_अगर

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    अगर (unlikely(dest->tcbPriority < गाँठ_STATE(ksCurThread->tcbPriority) &&
                 !isHighestPrio(dom, dest->tcbPriority))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    अगर (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (unlikely(dest->tcbSchedContext != शून्य)) {
        vm_fault_slowpath(type);
    }

    reply_t *reply = thread_state_get_replyObject_np(dest->tcbState);
    अगर (unlikely(reply == शून्य)) {
        vm_fault_slowpath(type);
    }
#पूर्ण_अगर

#अगर_घोषित ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    अगर (unlikely(गाँठ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_set_tcbfault_vm_fault(type);

#अगर_घोषित CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ksKernelEntry.is_fastpath = true;
#पूर्ण_अगर

    /* Dequeue the destination. */
    endpoint_ptr_set_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    अगर (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = शून्य;
#अगर_घोषित CONFIG_KERNEL_MCS
        dest->tcbEPNext = शून्य;
#पूर्ण_अगर
    } अन्यथा {
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_set_tsType_np(&गाँठ_STATE(ksCurThread)->tcbState, ThreadState_BlockedOnReply);
#अगर_घोषित CONFIG_KERNEL_MCS

    thread_state_ptr_set_replyObject_np(&dest->tcbState, 0);
    thread_state_ptr_set_replyObject_np(&गाँठ_STATE(ksCurThread)->tcbState, REPLY_REF(reply));
    reply->replyTCB = गाँठ_STATE(ksCurThread);

    sched_context_t *sc = गाँठ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    गाँठ_STATE(ksCurThread)->tcbSchedContext = शून्य;

    reply_t *old_caller = sc->scReply;
    reply->reply_पूर्व = call_ढेर_new(REPLY_REF(sc->scReply), false);
    अगर (unlikely(old_caller)) {
        old_caller->reply_अग्र = call_ढेर_new(REPLY_REF(reply), false);
    }
    reply->reply_अग्र = call_ढेर_new(SC_REF(sc), true);
    sc->scReply = reply;
#अन्यथा
    /* Get sender reply slot */
    cte_t *replySlot = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbReply);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t replyCanGrant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_reply_cap_ptr_new_np(&callerSlot->cap, replyCanGrant, 0, TCB_REF(गाँठ_STATE(ksCurThread)));
    mdb_गाँठ_ptr_set_mdb_पूर्व_np(&callerSlot->cteMDBNode, CTE_REF(replySlot));
    mdb_गाँठ_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(&replySlot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#पूर्ण_अगर
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_set_mrs(dest);

    /* Generate the msginfo */
    info = seL4_MessageInfo_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_लंबाई);

    /* Set the fault handler to running */
    thread_state_ptr_set_tsType_np(&dest->tcbState, ThreadState_Running);
    switchToThread_fp(dest, cap_pd, stored_hw_asid);
    संदेश_Info = wordFromMessageInfo(seL4_MessageInfo_set_capsUnwrapped(info, 0));

    fastpath_restore(badge, संदेश_Info, गाँठ_STATE(ksCurThread));
}
#पूर्ण_अगर
