/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <fastpath/fastpath.h>

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#د_که_پای
#ضمن <benchmark/benchmark_utilisation.h>

#که_تعریف_شوی CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#د_که_پای
فراغ NORETURN fastpath_call(کلمه_t cptr, کلمه_t پیغام_Info)
{
    se_L4_پیغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    کلمه_t اوږدتیا;
    tcb_t *dest;
    کلمه_t badge;
    cap_t newVTable;
    vspace_ریښه_t *cap_pd;
    pde_t stored_hw_asid;
    کلمه_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = پیغام_Info_From_کلمه_raw(پیغام_Info);
    اوږدتیا = se_L4_پیغام_Info_get_اوږدتیا(info);
    fault_type = seL4_Fault_get_seL4_FaultType(غوټه_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    إذا (unlikely(fastpath_mi_check(پیغام_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    إذا (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_کتار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#إذا defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    إذا (unlikely(dest->tcbArch.tcbContext.breakpointState.single_ګام_enabled)) {
        slowpath(SysCall);
    }
#د_که_پای

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ریښه_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    إذا (unlikely(! is_Valid_VTable_ریښه_fp(newVTable))) {
        slowpath(SysCall);
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_خونه];
#د_که_پای

#إذا defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_ریښه_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ریښه(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ټکي[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای
#د_که_پای

#که_تعریف_شوی CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    إذا (unlikely(پیدا_کړه_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    إذا (unlikely(dest->tcb_لومړیتوب < غوټه_STATE(ksCurThread->tcb_لومړیتوب) &&
                 !isHighestPrio(dom, dest->tcb_لومړیتوب))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    إذا (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_ځواب(ep_cap))) {
        slowpath(SysCall);
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH32
    إذا (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#د_که_پای

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (unlikely(dest->tcbSchedContext != عدم)) {
        slowpath(SysCall);
    }

    ځواب_t *ځواب = thread_state_get_ځواب_Object_np(dest->tcbState);
    إذا (unlikely(ځواب == عدم)) {
        slowpath(SysCall);
    }
#د_که_پای

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(غوټه_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای

    /* Dequeue the destination. */
    endpoint_ptr_سېټ_ep_کتار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    إذا (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = عدم;
#که_تعریف_شوی CONFIG_KERNEL_MCS
        dest->tcbEPNext = عدم;
#د_که_پای
    } وإلا {
        endpoint_ptr_mset_ep_کتار_لکۍ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سېټ_ts_Type_np(&غوټه_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_ځواب);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    thread_state_ptr_سېټ_ځواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سېټ_ځواب_Object_np(&غوټه_STATE(ksCurThread)->tcbState, ځواب_REF(ځواب));
    ځواب->ځواب_TCB = غوټه_STATE(ksCurThread);

    sched_context_t *sc = غوټه_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    غوټه_STATE(ksCurThread)->tcbSchedContext = عدم;

    ځواب_t *old_caller = sc->sc_ځواب;
    ځواب->ځواب_Prev = call_ډېری_new(ځواب_REF(sc->sc_ځواب), false);
    إذا (unlikely(old_caller)) {
        old_caller->ځواب_بل = call_ډېری_new(ځواب_REF(ځواب), false);
    }
    ځواب->ځواب_بل = call_ډېری_new(SC_REF(sc), true);
    sc->sc_ځواب = ځواب;
#وإلا
    /* Get sender reply slot */
    cte_t *ځواب_خونه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcb_ځواب);

    /* Get dest caller slot */
    cte_t *caller_خونه = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    کلمه_t ځواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_ځواب_cap_ptr_new_np(&caller_خونه->cap, ځواب_Can_Grant, 0,
                             TCB_REF(غوټه_STATE(ksCurThread)));
    mdb_غوټه_ptr_سېټ_mdb_Prev_np(&caller_خونه->cteMDBNode, CTE_REF(ځواب_خونه));
    mdb_غوټه_ptr_mset_mdb_بل_mdb_Revocable_mdb_لومړی_Badged(
        &ځواب_خونه->cteMDBNode, CTE_REF(caller_خونه), 1, 1);
#د_که_پای

    fastpath_copy_mrs(اوږدتیا, غوټه_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_سېټ_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_منزل_Thread_fp(dest, cap_pd, stored_hw_asid);

    پیغام_Info = کلمه_From_پیغام_Info(se_L4_پیغام_Info_سېټ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, پیغام_Info, غوټه_STATE(ksCurThread));
}

#که_تعریف_شوی CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#د_که_پای
#که_تعریف_شوی CONFIG_KERNEL_MCS
فراغ NORETURN fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info, کلمه_t ځواب)
#وإلا
فراغ NORETURN fastpath_ځواب_recv(کلمه_t cptr, کلمه_t پیغام_Info)
#د_که_پای
{
    se_L4_پیغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    کلمه_t اوږدتیا;
    tcb_t *caller;
    کلمه_t badge;
    tcb_t *endpoint_لکۍ;
    کلمه_t fault_type;

    cap_t newVTable;
    vspace_ریښه_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = پیغام_Info_From_کلمه_raw(پیغام_Info);
    اوږدتیا = se_L4_پیغام_Info_get_اوږدتیا(info);
    fault_type = seL4_Fault_get_seL4_FaultType(غوټه_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    إذا (unlikely(fastpath_mi_check(پیغام_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_ځواب_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    إذا (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_ځواب_Recv);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t ځواب_cap = lookup_fp(TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCTable)->cap, ځواب);

    /* check it's a reply object */
    إذا (unlikely(!cap_capType_equals(ځواب_cap, cap_ځواب_cap))) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای

    /* Check there is nothing waiting on the notification */
    إذا (unlikely(غوټه_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(غوټه_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_فعال)) {
        slowpath(Sys_ځواب_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_ځواب_Recv);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* Get the reply address */
    ځواب_t *ځواب_ptr = ځواب_PTR(cap_ځواب_cap_get_cap_ځواب_Ptr(ځواب_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    إذا (unlikely(ځواب_ptr->ځواب_TCB == عدم ||
                 call_ډېری_get_is_سر(ځواب_ptr->ځواب_بل) == 0 ||
                 SC_PTR(call_ډېری_get_call_ډېری_Ptr(ځواب_ptr->ځواب_بل)) != غوټه_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_ځواب_Recv);
    }

    /* Determine who the caller is. */
    caller = ځواب_ptr->ځواب_TCB;
#وإلا
    /* Only reply if the reply cap is valid. */
    cte_t *caller_خونه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = caller_خونه->cap;
    إذا (unlikely(!fastpath_ځواب_cap_check(callerCap))) {
        slowpath(Sys_ځواب_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_ځواب_cap_get_cap_TCBPtr(callerCap));
#د_که_پای

    /* ensure we are not single stepping the caller in ia32 */
#إذا defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    إذا (unlikely(caller->tcbArch.tcbContext.breakpointState.single_ګام_enabled)) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#که_نه_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_ځواب_Recv);
    }
#وإلا
    إذا (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ریښه_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    إذا (unlikely(! is_Valid_VTable_ریښه_fp(newVTable))) {
        slowpath(Sys_ځواب_Recv);
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_خونه];
#د_که_پای

#إذا defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_ریښه_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map) != cap_pd)) {
        slowpath(Sys_ځواب_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ریښه(asid_map)) != cap_pd)) {
        slowpath(Sys_ځواب_Recv);
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_ځواب_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ټکي[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای
#د_که_پای

#که_تعریف_شوی CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    إذا (unlikely(پیدا_کړه_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(Sys_ځواب_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    إذا (unlikely(!isHighestPrio(dom, caller->tcb_لومړیتوب))) {
        slowpath(Sys_ځواب_Recv);
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    إذا (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_ځواب_Recv);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (unlikely(caller->tcbSchedContext != عدم)) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(غوټه_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_ځواب_Recv);
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &غوټه_STATE(ksCurThread)->tcbState, (کلمه_t)ep_ptr, ThreadState_BlockedOnReceive);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_سېټ_ځواب_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_سېټ_ځواب_Object_np(&غوټه_STATE(ksCurThread)->tcbState, ځواب_REF(ځواب_ptr));
    ځواب_ptr->ځواب_TCB = غوټه_STATE(ksCurThread);
#وإلا
    thread_state_ptr_سېټ_blocking_IPCCan_Grant(&غوټه_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#د_که_پای

    /* Place the thread in the endpoint queue */
    endpoint_لکۍ = endpoint_ptr_get_ep_کتار_لکۍ_fp(ep_ptr);
    إذا (likely(!endpoint_لکۍ)) {
        غوټه_STATE(ksCurThread)->tcbEPPrev = عدم;
        غوټه_STATE(ksCurThread)->tcbEPNext = عدم;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_سېټ_ep_کتار_سر_np(ep_ptr, TCB_REF(غوټه_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_کتار_لکۍ_state(ep_ptr, TCB_REF(غوټه_STATE(ksCurThread)),
                                             EPState_Recv);
    } وإلا {
#که_تعریف_شوی CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_کتار_t کتار = tcbAppend(غوټه_STATE(ksCurThread), ep_ptr_get_کتار(ep_ptr));
        endpoint_ptr_سېټ_ep_کتار_سر_np(ep_ptr, TCB_REF(کتار.سر));
        endpoint_ptr_mset_ep_کتار_لکۍ_state(ep_ptr, TCB_REF(کتار.پای_ټکی), EPState_Recv);
#وإلا
        /* Append current thread onto the queue. */
        endpoint_لکۍ->tcbEPNext = غوټه_STATE(ksCurThread);
        غوټه_STATE(ksCurThread)->tcbEPPrev = endpoint_لکۍ;
        غوټه_STATE(ksCurThread)->tcbEPNext = عدم;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_کتار_لکۍ_state(ep_ptr, TCB_REF(غوټه_STATE(ksCurThread)),
                                             EPState_Recv);
#د_که_پای
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    /* update call stack */
    کلمه_t prev_ptr = call_ډېری_get_call_ډېری_Ptr(ځواب_ptr->ځواب_Prev);
    sched_context_t *sc = غوټه_STATE(ksCurThread)->tcbSchedContext;
    غوټه_STATE(ksCurThread)->tcbSchedContext = عدم;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_ځواب = ځواب_PTR(prev_ptr);
    إذا (unlikely(ځواب_PTR(prev_ptr) != عدم)) {
        sc->sc_ځواب->ځواب_بل = ځواب_ptr->ځواب_بل;
    }

    /* TODO neccessary? */
    ځواب_ptr->ځواب_Prev.ټکي[0] = 0;
    ځواب_ptr->ځواب_بل.ټکي[0] = 0;
#وإلا
    /* Delete the reply cap. */
    mdb_غوټه_ptr_mset_mdb_بل_mdb_Revocable_mdb_لومړی_Badged(
        &CTE_PTR(mdb_غوټه_get_mdb_Prev(caller_خونه->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    caller_خونه->cap = cap_null_cap_new();
    caller_خونه->cteMDBNode = nullMDBNode;
#د_که_پای

#که_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        کلمه_t pc = getRestartPC(caller);
        سېټ_بل_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سېټ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_منزل_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } وإلا {
#د_که_پای
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(اوږدتیا, غوټه_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سېټ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_منزل_Thread_fp(caller, cap_pd, stored_hw_asid);

        پیغام_Info = کلمه_From_پیغام_Info(se_L4_پیغام_Info_سېټ_caps_Unwrapped(info, 0));

        fastpath_restore(badge, پیغام_Info, غوټه_STATE(ksCurThread));

#که_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
    }
#د_که_پای
}

#که_تعریف_شوی CONFIG_SIGNAL_FASTPATH
#که_تعریف_شوی CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#د_که_پای
فراغ NORETURN fastpath_signal(کلمه_t cptr, کلمه_t پیغام_Info)
{
    کلمه_t fault_type;
    sched_context_t *sc = عدم;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = عدم;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(غوټه_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    إذا (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    إذا (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    إذا (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    کلمه_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    بدل (ntfnState) {
    حالة Ntfn_State_فعال:
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای
        ntfn_سېټ_فعال(ntfnPtr, badge | notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    حالة NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        إذا (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای
            ntfn_سېټ_فعال(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        اكسر;
    حالة Ntfn_State_انتظار:
        dest = TCB_PTR(notification_ptr_get_ntfn_کتار_سر(ntfnPtr));
        اكسر;
    افتراضي:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    إذا (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        إذا (sc == عدم || sc->scTcb != عدم) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#إذا defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        إذا (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#د_که_پای
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    إذا (غوټه_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_لومړیتوب < dest->tcb_لومړیتوب) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    إذا (sc->scRefillMax > 0) {
        إذا (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    إذا (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای

    إذا (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } وإلا {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    سېټ_ثبتوونکی(dest, badge_ثبتوونکی, badge);
    thread_state_ptr_سېټ_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    إذا (sc_sporadic(dest->tcbSchedContext)) {
        أكد(dest->tcbSchedContext != غوټه_STATE(ksCurSC));
        إذا (dest->tcbSchedContext != غوټه_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    إذا (schedulable) {
        إذا (غوټه_STATE(ksCurThread)->tcb_لومړیتوب > dest->tcb_لومړیتوب || crossnode) {
            SCHED_کتار_ته_ورزیاتول(dest);
        } وإلا {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#د_که_پای

#که_تعریف_شوی CONFIG_EXCEPTION_FASTPATH
ساكن مضمن
FORCE_INLINE
فراغ NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_ریښه_t *cap_pd;
    کلمه_t badge;
    se_L4_پیغام_Info_t info;
    کلمه_t پیغام_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#که_تعریف_شوی CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbFaultHandler)->cap;
#وإلا
    cptr_t handlerCPtr;
    handlerCPtr = غوټه_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#د_که_پای

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    إذا (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_ځواب(handler_cap))
#د_که_پای
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_کتار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ریښه_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    إذا (unlikely(! is_Valid_VTable_ریښه_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#که_تعریف_شوی CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ریښه(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ټکي[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.ټکي[0] = asid;
#د_که_پای
#د_که_پای

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    إذا (unlikely(dest->tcb_لومړیتوب < غوټه_STATE(ksCurThread->tcb_لومړیتوب) &&
                 !isHighestPrio(dom, dest->tcb_لومړیتوب))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (unlikely(dest->tcbSchedContext != عدم)) {
        vm_fault_slowpath(type);
    }

    ځواب_t *ځواب = thread_state_get_ځواب_Object_np(dest->tcbState);
    إذا (unlikely(ځواب == عدم)) {
        vm_fault_slowpath(type);
    }
#د_که_پای

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(غوټه_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_سېټ_tcbfault_vm_fault(type);

#که_تعریف_شوی CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ثبت.is_fastpath = true;
#د_که_پای

    /* Dequeue the destination. */
    endpoint_ptr_سېټ_ep_کتار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    إذا (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = عدم;
#که_تعریف_شوی CONFIG_KERNEL_MCS
        dest->tcbEPNext = عدم;
#د_که_پای
    } وإلا {
        endpoint_ptr_mset_ep_کتار_لکۍ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سېټ_ts_Type_np(&غوټه_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_ځواب);
#که_تعریف_شوی CONFIG_KERNEL_MCS

    thread_state_ptr_سېټ_ځواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سېټ_ځواب_Object_np(&غوټه_STATE(ksCurThread)->tcbState, ځواب_REF(ځواب));
    ځواب->ځواب_TCB = غوټه_STATE(ksCurThread);

    sched_context_t *sc = غوټه_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    غوټه_STATE(ksCurThread)->tcbSchedContext = عدم;

    ځواب_t *old_caller = sc->sc_ځواب;
    ځواب->ځواب_Prev = call_ډېری_new(ځواب_REF(sc->sc_ځواب), false);
    إذا (unlikely(old_caller)) {
        old_caller->ځواب_بل = call_ډېری_new(ځواب_REF(ځواب), false);
    }
    ځواب->ځواب_بل = call_ډېری_new(SC_REF(sc), true);
    sc->sc_ځواب = ځواب;
#وإلا
    /* Get sender reply slot */
    cte_t *ځواب_خونه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcb_ځواب);

    /* Get dest caller slot */
    cte_t *caller_خونه = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    کلمه_t ځواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_ځواب_cap_ptr_new_np(&caller_خونه->cap, ځواب_Can_Grant, 0, TCB_REF(غوټه_STATE(ksCurThread)));
    mdb_غوټه_ptr_سېټ_mdb_Prev_np(&caller_خونه->cteMDBNode, CTE_REF(ځواب_خونه));
    mdb_غوټه_ptr_mset_mdb_بل_mdb_Revocable_mdb_لومړی_Badged(&ځواب_خونه->cteMDBNode, CTE_REF(caller_خونه), 1, 1);
#د_که_پای
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_سېټ_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_پیغام_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_اوږدتیا);

    /* Set the fault handler to running */
    thread_state_ptr_سېټ_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_منزل_Thread_fp(dest, cap_pd, stored_hw_asid);
    پیغام_Info = کلمه_From_پیغام_Info(se_L4_پیغام_Info_سېټ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, پیغام_Info, غوټه_STATE(ksCurThread));
}
#د_که_پای
