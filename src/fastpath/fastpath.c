/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <fastpath/fastpath.h>

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#अन्तर्भाव <benchmark/benchmark_track.h>
#यदि_अन्तः
#अन्तर्भाव <benchmark/benchmark_utilisation.h>

#यदि_परिभाषितम् CONFIG_ARCH_ARM
स्थिर अन्तःस्थ
FORCE_INLINE
#यदि_अन्तः
शून्य NORETURN fastpath_call(word_t cptr, word_t सन्देशः_Info)
{
    se_L4_सन्देशः_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t दीर्घता;
    tcb_t *dest;
    word_t badge;
    cap_t newVTable;
    vspace_मूलम्_t *cap_pd;
    pde_t stored_hw_asid;
    word_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = सन्देशः_Info_From_Word_raw(सन्देशः_Info);
    दीर्घता = se_L4_सन्देशः_Info_get_दीर्घता(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ग्रन्थिः_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    यदि (unlikely(fastpath_mi_check(सन्देशः_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    यदि (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(ep_ptr));

    /* Check that there's a thread waiting to receive */
    यदि (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#यदि defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    यदि (unlikely(dest->tcbArch.tcbContext.breakpointState.single_चरणः_enabled)) {
        slowpath(SysCall);
    }
#यदि_अन्तः

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूलम्_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    यदि (unlikely(! is_Valid_VTable_मूलम्_fp(newVTable))) {
        slowpath(SysCall);
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#यदि_अन्तः

#यदि defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_मूलम्_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    यदि (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_मूलम्_t *)asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    यदि (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    यदि (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दाः[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(newVTable);
    यदि (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    यदि (unlikely(dest->tcb_प्राथमिकता < ग्रन्थिः_STATE(ksCurThread->tcb_प्राथमिकता) &&
                 !isHighestPrio(dom, dest->tcb_प्राथमिकता))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    यदि (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(ep_cap))) {
        slowpath(SysCall);
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    यदि (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#यदि_अन्तः

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    यदि (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (unlikely(dest->tcbSchedContext != रिक्त)) {
        slowpath(SysCall);
    }

    प्रत्युत्तरम्_t *प्रत्युत्तरम् = thread_state_get_प्रत्युत्तरम्_Object_np(dest->tcbState);
    यदि (unlikely(प्रत्युत्तरम् == रिक्त)) {
        slowpath(SysCall);
    }
#यदि_अन्तः

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    यदि (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः

    /* Dequeue the destination. */
    endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    यदि (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = रिक्त;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        dest->tcbEPNext = रिक्त;
#यदि_अन्तः
    } अन्यथा {
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_समुच्चयः_ts_Type_np(&ग्रन्थिः_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_प्रत्युत्तरम्);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&dest->tcbState, 0);
    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&ग्रन्थिः_STATE(ksCurThread)->tcbState, प्रत्युत्तरम्_REF(प्रत्युत्तरम्));
    प्रत्युत्तरम्->प्रत्युत्तरम्_TCB = ग्रन्थिः_STATE(ksCurThread);

    sched_context_t *sc = ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext = रिक्त;

    प्रत्युत्तरम्_t *old_caller = sc->sc_प्रत्युत्तरम्;
    प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(प्रत्युत्तरम्_REF(sc->sc_प्रत्युत्तरम्), false);
    यदि (unlikely(old_caller)) {
        old_caller->प्रत्युत्तरम्_अग्र = call_राशिः_new(प्रत्युत्तरम्_REF(प्रत्युत्तरम्), false);
    }
    प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(SC_REF(sc), true);
    sc->sc_प्रत्युत्तरम् = प्रत्युत्तरम्;
#अन्यथा
    /* Get sender reply slot */
    cte_t *प्रत्युत्तरम्_Slot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcb_प्रत्युत्तरम्);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t प्रत्युत्तरम्_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_प्रत्युत्तरम्_cap_ptr_new_np(&callerSlot->cap, प्रत्युत्तरम्_Can_Grant, 0,
                             TCB_REF(ग्रन्थिः_STATE(ksCurThread)));
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व_np(&callerSlot->cteMDBNode, CTE_REF(प्रत्युत्तरम्_Slot));
    mdb_ग्रन्थिः_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(
        &प्रत्युत्तरम्_Slot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#यदि_अन्तः

    fastpath_copy_mrs(दीर्घता, ग्रन्थिः_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_समुच्चयः_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_गन्तव्यम्_Thread_fp(dest, cap_pd, stored_hw_asid);

    सन्देशः_Info = word_From_सन्देशः_Info(se_L4_सन्देशः_Info_समुच्चयः_caps_Unwrapped(info, 0));

    fastpath_restore(badge, सन्देशः_Info, ग्रन्थिः_STATE(ksCurThread));
}

#यदि_परिभाषितम् CONFIG_ARCH_ARM
स्थिर अन्तःस्थ
FORCE_INLINE
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
शून्य NORETURN fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info, word_t प्रत्युत्तरम्)
#अन्यथा
शून्य NORETURN fastpath_प्रत्युत्तरम्_recv(word_t cptr, word_t सन्देशः_Info)
#यदि_अन्तः
{
    se_L4_सन्देशः_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    word_t दीर्घता;
    tcb_t *caller;
    word_t badge;
    tcb_t *endpoint_पुच्छ;
    word_t fault_type;

    cap_t newVTable;
    vspace_मूलम्_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = सन्देशः_Info_From_Word_raw(सन्देशः_Info);
    दीर्घता = se_L4_सन्देशः_Info_get_दीर्घता(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ग्रन्थिः_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    यदि (unlikely(fastpath_mi_check(सन्देशः_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    यदि (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t प्रत्युत्तरम्_cap = lookup_fp(TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCTable)->cap, प्रत्युत्तरम्);

    /* check it's a reply object */
    यदि (unlikely(!cap_capType_equals(प्रत्युत्तरम्_cap, cap_प्रत्युत्तरम्_cap))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः

    /* Check there is nothing waiting on the notification */
    यदि (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(ग्रन्थिः_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_सक्रिय)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    यदि (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* Get the reply address */
    प्रत्युत्तरम्_t *प्रत्युत्तरम्_ptr = प्रत्युत्तरम्_PTR(cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Ptr(प्रत्युत्तरम्_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    यदि (unlikely(प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_TCB == रिक्त ||
                 call_राशिः_get_is_शीर्ष(प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_अग्र) == 0 ||
                 SC_PTR(call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_अग्र)) != ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

    /* Determine who the caller is. */
    caller = प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_TCB;
#अन्यथा
    /* Only reply if the reply cap is valid. */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = callerSlot->cap;
    यदि (unlikely(!fastpath_प्रत्युत्तरम्_cap_check(callerCap))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_प्रत्युत्तरम्_cap_get_cap_TCBPtr(callerCap));
#यदि_अन्तः

    /* ensure we are not single stepping the caller in ia32 */
#यदि defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    यदि (unlikely(caller->tcbArch.tcbContext.breakpointState.single_चरणः_enabled)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#यदि_अपरिभाषितम् CONFIG_EXCEPTION_FASTPATH
    यदि (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#अन्यथा
    यदि (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूलम्_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    यदि (unlikely(! is_Valid_VTable_मूलम्_fp(newVTable))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_SLOT];
#यदि_अन्तः

#यदि defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_मूलम्_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    यदि (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_मूलम्_t *)asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map) != cap_pd)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    यदि (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map)) != cap_pd)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    यदि (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दाः[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_page_सारणी_cap_get_cap_PTMapped_ASID(newVTable);
    यदि (unlikely(findVSpaceForASID_fp(asid) != cap_pd)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    यदि (unlikely(!isHighestPrio(dom, caller->tcb_प्राथमिकता))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    यदि (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    यदि (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (unlikely(caller->tcbSchedContext != रिक्त)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    यदि (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_प्रत्युत्तरम्_Recv);
    }
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &ग्रन्थिः_STATE(ksCurThread)->tcbState, (word_t)ep_ptr, ThreadState_BlockedOnReceive);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&ग्रन्थिः_STATE(ksCurThread)->tcbState, प्रत्युत्तरम्_REF(प्रत्युत्तरम्_ptr));
    प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_TCB = ग्रन्थिः_STATE(ksCurThread);
#अन्यथा
    thread_state_ptr_समुच्चयः_blocking_IPCCan_Grant(&ग्रन्थिः_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#यदि_अन्तः

    /* Place the thread in the endpoint queue */
    endpoint_पुच्छ = endpoint_ptr_get_ep_Queue_पुच्छ_fp(ep_ptr);
    यदि (likely(!endpoint_पुच्छ)) {
        ग्रन्थिः_STATE(ksCurThread)->tcbEPPrev = रिक्त;
        ग्रन्थिः_STATE(ksCurThread)->tcbEPNext = रिक्त;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(ग्रन्थिः_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(ग्रन्थिः_STATE(ksCurThread)),
                                             EPState_Recv);
    } अन्यथा {
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_queue_t queue = tcbAppend(ग्रन्थिः_STATE(ksCurThread), ep_ptr_get_queue(ep_ptr));
        endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(queue.शीर्ष));
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(queue.अन्तः_2), EPState_Recv);
#अन्यथा
        /* Append current thread onto the queue. */
        endpoint_पुच्छ->tcbEPNext = ग्रन्थिः_STATE(ksCurThread);
        ग्रन्थिः_STATE(ksCurThread)->tcbEPPrev = endpoint_पुच्छ;
        ग्रन्थिः_STATE(ksCurThread)->tcbEPNext = रिक्त;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, TCB_REF(ग्रन्थिः_STATE(ksCurThread)),
                                             EPState_Recv);
#यदि_अन्तः
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    /* update call stack */
    word_t पूर्व_ptr = call_राशिः_get_call_राशिः_Ptr(प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_पूर्व);
    sched_context_t *sc = ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext;
    ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext = रिक्त;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_प्रत्युत्तरम् = प्रत्युत्तरम्_PTR(पूर्व_ptr);
    यदि (unlikely(प्रत्युत्तरम्_PTR(पूर्व_ptr) != रिक्त)) {
        sc->sc_प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_अग्र;
    }

    /* TODO neccessary? */
    प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_पूर्व.शब्दाः[0] = 0;
    प्रत्युत्तरम्_ptr->प्रत्युत्तरम्_अग्र.शब्दाः[0] = 0;
#अन्यथा
    /* Delete the reply cap. */
    mdb_ग्रन्थिः_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(
        &CTE_PTR(mdb_ग्रन्थिः_get_mdb_पूर्व(callerSlot->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    callerSlot->cap = cap_null_cap_new();
    callerSlot->cteMDBNode = nullMDBNode;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_EXCEPTION_FASTPATH
    यदि (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        word_t pc = getRestartPC(caller);
        समुच्चयः_अग्र_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_समुच्चयः_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_गन्तव्यम्_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } अन्यथा {
#यदि_अन्तः
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(दीर्घता, ग्रन्थिः_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_समुच्चयः_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_गन्तव्यम्_Thread_fp(caller, cap_pd, stored_hw_asid);

        सन्देशः_Info = word_From_सन्देशः_Info(se_L4_सन्देशः_Info_समुच्चयः_caps_Unwrapped(info, 0));

        fastpath_restore(badge, सन्देशः_Info, ग्रन्थिः_STATE(ksCurThread));

#यदि_परिभाषितम् CONFIG_EXCEPTION_FASTPATH
    }
#यदि_अन्तः
}

#यदि_परिभाषितम् CONFIG_SIGNAL_FASTPATH
#यदि_परिभाषितम् CONFIG_ARCH_ARM
स्थिर अन्तःस्थ
FORCE_INLINE
#यदि_अन्तः
शून्य NORETURN fastpath_signal(word_t cptr, word_t सन्देशः_Info)
{
    word_t fault_type;
    sched_context_t *sc = रिक्त;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = रिक्त;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(ग्रन्थिः_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    यदि (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    यदि (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    यदि (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    यदि (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    word_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    विकल्प (ntfnState) {
    पक्ष Ntfn_State_सक्रिय:
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः
        ntfn_समुच्चयः_सक्रिय(ntfnPtr, badge | notification_ptr_get_ntfn_सन्देशः_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    पक्ष NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        यदि (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः
            ntfn_समुच्चयः_सक्रिय(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        भङ्ग;
    पक्ष Ntfn_State_प्रतीक्षा:
        dest = TCB_PTR(notification_ptr_get_ntfn_Queue_शीर्ष(ntfnPtr));
        भङ्ग;
    स्वभाव:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    यदि (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        यदि (sc == रिक्त || sc->scTcb != रिक्त) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#यदि defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        यदि (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#यदि_अन्तः
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    यदि (ग्रन्थिः_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_प्राथमिकता < dest->tcb_प्राथमिकता) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    यदि (sc->scRefillMax > 0) {
        यदि (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    यदि (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः

    यदि (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } अन्यथा {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    समुच्चयः_पञ्जिकाकोशः(dest, badge_पञ्जिकाकोशः, badge);
    thread_state_ptr_समुच्चयः_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    यदि (sc_sporadic(dest->tcbSchedContext)) {
        प्रतिज्ञा(dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC));
        यदि (dest->tcbSchedContext != ग्रन्थिः_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    यदि (schedulable) {
        यदि (ग्रन्थिः_STATE(ksCurThread)->tcb_प्राथमिकता > dest->tcb_प्राथमिकता || crossnode) {
            SCHED_पङ्क्तौ_निक्षिप(dest);
        } अन्यथा {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_EXCEPTION_FASTPATH
स्थिर अन्तःस्थ
FORCE_INLINE
शून्य NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_मूलम्_t *cap_pd;
    word_t badge;
    se_L4_सन्देशः_Info_t info;
    word_t सन्देशः_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbFaultHandler)->cap;
#अन्यथा
    cptr_t handlerCPtr;
    handlerCPtr = ग्रन्थिः_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#यदि_अन्तः

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    यदि (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_प्रत्युत्तरम्(handler_cap))
#यदि_अन्तः
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_Queue_शीर्ष(ep_ptr));

    /* Check that there's a thread waiting to receive */
    यदि (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_मूलम्_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    यदि (unlikely(! is_Valid_VTable_मूलम्_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = findMapForASID(asid);
    यदि (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_मूलम्(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    यदि (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.शब्दाः[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#अन्यथा
    stored_hw_asid.शब्दाः[0] = asid;
#यदि_अन्तः
#यदि_अन्तः

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    यदि (unlikely(dest->tcb_प्राथमिकता < ग्रन्थिः_STATE(ksCurThread->tcb_प्राथमिकता) &&
                 !isHighestPrio(dom, dest->tcb_प्राथमिकता))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    यदि (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (unlikely(dest->tcbSchedContext != रिक्त)) {
        vm_fault_slowpath(type);
    }

    प्रत्युत्तरम्_t *प्रत्युत्तरम् = thread_state_get_प्रत्युत्तरम्_Object_np(dest->tcbState);
    यदि (unlikely(प्रत्युत्तरम् == रिक्त)) {
        vm_fault_slowpath(type);
    }
#यदि_अन्तः

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    यदि (unlikely(ग्रन्थिः_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_समुच्चयः_tcbfault_vm_fault(type);

#यदि_परिभाषितम् CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_प्रविष्टिः.is_fastpath = true;
#यदि_अन्तः

    /* Dequeue the destination. */
    endpoint_ptr_समुच्चयः_ep_Queue_शीर्ष_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    यदि (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = रिक्त;
#यदि_परिभाषितम् CONFIG_KERNEL_MCS
        dest->tcbEPNext = रिक्त;
#यदि_अन्तः
    } अन्यथा {
        endpoint_ptr_mset_ep_Queue_पुच्छ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_समुच्चयः_ts_Type_np(&ग्रन्थिः_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_प्रत्युत्तरम्);
#यदि_परिभाषितम् CONFIG_KERNEL_MCS

    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&dest->tcbState, 0);
    thread_state_ptr_समुच्चयः_प्रत्युत्तरम्_Object_np(&ग्रन्थिः_STATE(ksCurThread)->tcbState, प्रत्युत्तरम्_REF(प्रत्युत्तरम्));
    प्रत्युत्तरम्->प्रत्युत्तरम्_TCB = ग्रन्थिः_STATE(ksCurThread);

    sched_context_t *sc = ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ग्रन्थिः_STATE(ksCurThread)->tcbSchedContext = रिक्त;

    प्रत्युत्तरम्_t *old_caller = sc->sc_प्रत्युत्तरम्;
    प्रत्युत्तरम्->प्रत्युत्तरम्_पूर्व = call_राशिः_new(प्रत्युत्तरम्_REF(sc->sc_प्रत्युत्तरम्), false);
    यदि (unlikely(old_caller)) {
        old_caller->प्रत्युत्तरम्_अग्र = call_राशिः_new(प्रत्युत्तरम्_REF(प्रत्युत्तरम्), false);
    }
    प्रत्युत्तरम्->प्रत्युत्तरम्_अग्र = call_राशिः_new(SC_REF(sc), true);
    sc->sc_प्रत्युत्तरम् = प्रत्युत्तरम्;
#अन्यथा
    /* Get sender reply slot */
    cte_t *प्रत्युत्तरम्_Slot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcb_प्रत्युत्तरम्);

    /* Get dest caller slot */
    cte_t *callerSlot = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    word_t प्रत्युत्तरम्_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_प्रत्युत्तरम्_cap_ptr_new_np(&callerSlot->cap, प्रत्युत्तरम्_Can_Grant, 0, TCB_REF(ग्रन्थिः_STATE(ksCurThread)));
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व_np(&callerSlot->cteMDBNode, CTE_REF(प्रत्युत्तरम्_Slot));
    mdb_ग्रन्थिः_ptr_mset_mdb_अग्र_mdb_Revocable_mdb_First_Badged(&प्रत्युत्तरम्_Slot->cteMDBNode, CTE_REF(callerSlot), 1, 1);
#यदि_अन्तः
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_समुच्चयः_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_सन्देशः_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_दीर्घता);

    /* Set the fault handler to running */
    thread_state_ptr_समुच्चयः_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_गन्तव्यम्_Thread_fp(dest, cap_pd, stored_hw_asid);
    सन्देशः_Info = word_From_सन्देशः_Info(se_L4_सन्देशः_Info_समुच्चयः_caps_Unwrapped(info, 0));

    fastpath_restore(badge, सन्देशः_Info, ग्रन्थिः_STATE(ksCurThread));
}
#यदि_अन्तः
