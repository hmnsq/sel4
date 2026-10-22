/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <fastpath/fastpath.h>

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#شامل <benchmark/benchmark_track.h>
#اگر_ختم
#شامل <benchmark/benchmark_utilisation.h>

#اگر_تعریف_شدہ CONFIG_ARCH_ARM
ساکن اندرونی
FORCE_INLINE
#اگر_ختم
خالی NORETURN fastpath_call(لفظ_t cptr, لفظ_t پیغام_Info)
{
    se_L4_پیغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    لفظ_t لمبائی;
    tcb_t *dest;
    لفظ_t badge;
    cap_t newVTable;
    vspace_جڑ_t *cap_pd;
    pde_t stored_hw_asid;
    لفظ_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = پیغام_Info_From_لفظ_raw(پیغام_Info);
    لمبائی = se_L4_پیغام_Info_get_لمبائی(info);
    fault_type = seL4_Fault_get_seL4_FaultType(گرہ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    اگر (unlikely(fastpath_mi_check(پیغام_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    اگر (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    اگر (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#اگر defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    اگر (unlikely(dest->tcbArch.tcbContext.breakpointState.single_قدم_enabled)) {
        slowpath(SysCall);
    }
#اگر_ختم

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_جڑ_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    اگر (unlikely(! is_Valid_VTable_جڑ_fp(newVTable))) {
        slowpath(SysCall);
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_خانہ];
#اگر_ختم

#اگر defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_جڑ_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_جڑ(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    اگر (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.الفاظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ورنہ
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    اگر (unlikely(ڈھونڈو_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    اگر (unlikely(dest->tcb_ترجیح < گرہ_STATE(ksCurThread->tcb_ترجیح) &&
                 !isHighestPrio(dom, dest->tcb_ترجیح))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    اگر (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_جواب(ep_cap))) {
        slowpath(SysCall);
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    اگر (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#اگر_ختم

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    اگر (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (unlikely(dest->tcbSchedContext != تہی)) {
        slowpath(SysCall);
    }

    جواب_t *جواب = thread_state_get_جواب_Object_np(dest->tcbState);
    اگر (unlikely(جواب == تہی)) {
        slowpath(SysCall);
    }
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    اگر (unlikely(گرہ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم

    /* Dequeue the destination. */
    endpoint_ptr_سیٹ_ep_قطار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    اگر (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = تہی;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        dest->tcbEPNext = تہی;
#اگر_ختم
    } ورنہ {
        endpoint_ptr_mset_ep_قطار_دم_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سیٹ_ts_Type_np(&گرہ_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_جواب);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    thread_state_ptr_سیٹ_جواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سیٹ_جواب_Object_np(&گرہ_STATE(ksCurThread)->tcbState, جواب_REF(جواب));
    جواب->جواب_TCB = گرہ_STATE(ksCurThread);

    sched_context_t *sc = گرہ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    گرہ_STATE(ksCurThread)->tcbSchedContext = تہی;

    جواب_t *old_caller = sc->sc_جواب;
    جواب->جواب_پچھلا = call_ڈھیر_new(جواب_REF(sc->sc_جواب), false);
    اگر (unlikely(old_caller)) {
        old_caller->جواب_اگلا = call_ڈھیر_new(جواب_REF(جواب), false);
    }
    جواب->جواب_اگلا = call_ڈھیر_new(SC_REF(sc), true);
    sc->sc_جواب = جواب;
#ورنہ
    /* Get sender reply slot */
    cte_t *جواب_خانہ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcb_جواب);

    /* Get dest caller slot */
    cte_t *caller_خانہ = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    لفظ_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_جواب_cap_ptr_new_np(&caller_خانہ->cap, جواب_Can_Grant, 0,
                             TCB_REF(گرہ_STATE(ksCurThread)));
    mdb_گرہ_ptr_سیٹ_mdb_پچھلا_np(&caller_خانہ->cteMDBNode, CTE_REF(جواب_خانہ));
    mdb_گرہ_ptr_mset_mdb_اگلا_mdb_Revocable_mdb_پہلا_Badged(
        &جواب_خانہ->cteMDBNode, CTE_REF(caller_خانہ), 1, 1);
#اگر_ختم

    fastpath_copy_mrs(لمبائی, گرہ_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_سیٹ_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_مقصد_Thread_fp(dest, cap_pd, stored_hw_asid);

    پیغام_Info = لفظ_From_پیغام_Info(se_L4_پیغام_Info_سیٹ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, پیغام_Info, گرہ_STATE(ksCurThread));
}

#اگر_تعریف_شدہ CONFIG_ARCH_ARM
ساکن اندرونی
FORCE_INLINE
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی NORETURN fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info, لفظ_t جواب)
#ورنہ
خالی NORETURN fastpath_جواب_recv(لفظ_t cptr, لفظ_t پیغام_Info)
#اگر_ختم
{
    se_L4_پیغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    لفظ_t لمبائی;
    tcb_t *caller;
    لفظ_t badge;
    tcb_t *endpoint_دم;
    لفظ_t fault_type;

    cap_t newVTable;
    vspace_جڑ_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = پیغام_Info_From_لفظ_raw(پیغام_Info);
    لمبائی = se_L4_پیغام_Info_get_لمبائی(info);
    fault_type = seL4_Fault_get_seL4_FaultType(گرہ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    اگر (unlikely(fastpath_mi_check(پیغام_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    اگر (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_جواب_Recv);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t جواب_cap = lookup_fp(TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCTable)->cap, جواب);

    /* check it's a reply object */
    اگر (unlikely(!cap_capType_equals(جواب_cap, cap_جواب_cap))) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم

    /* Check there is nothing waiting on the notification */
    اگر (unlikely(گرہ_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(گرہ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_فعال)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    اگر (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_جواب_Recv);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* Get the reply address */
    جواب_t *جواب_ptr = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(جواب_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    اگر (unlikely(جواب_ptr->جواب_TCB == تہی ||
                 call_ڈھیر_get_is_سر(جواب_ptr->جواب_اگلا) == 0 ||
                 SC_PTR(call_ڈھیر_get_call_ڈھیر_Ptr(جواب_ptr->جواب_اگلا)) != گرہ_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Determine who the caller is. */
    caller = جواب_ptr->جواب_TCB;
#ورنہ
    /* Only reply if the reply cap is valid. */
    cte_t *caller_خانہ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = caller_خانہ->cap;
    اگر (unlikely(!fastpath_جواب_cap_check(callerCap))) {
        slowpath(Sys_جواب_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(callerCap));
#اگر_ختم

    /* ensure we are not single stepping the caller in ia32 */
#اگر defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    اگر (unlikely(caller->tcbArch.tcbContext.breakpointState.single_قدم_enabled)) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#اگر_غیر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
    اگر (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_جواب_Recv);
    }
#ورنہ
    اگر (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_جڑ_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    اگر (unlikely(! is_Valid_VTable_جڑ_fp(newVTable))) {
        slowpath(Sys_جواب_Recv);
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_خانہ];
#اگر_ختم

#اگر defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_جڑ_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_جڑ(asid_map)) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    اگر (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_جواب_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.الفاظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ورنہ
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    اگر (unlikely(ڈھونڈو_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    اگر (unlikely(!isHighestPrio(dom, caller->tcb_ترجیح))) {
        slowpath(Sys_جواب_Recv);
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    اگر (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    اگر (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_جواب_Recv);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (unlikely(caller->tcbSchedContext != تہی)) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    اگر (unlikely(گرہ_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_جواب_Recv);
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &گرہ_STATE(ksCurThread)->tcbState, (لفظ_t)ep_ptr, ThreadState_BlockedOnReceive);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_سیٹ_جواب_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_سیٹ_جواب_Object_np(&گرہ_STATE(ksCurThread)->tcbState, جواب_REF(جواب_ptr));
    جواب_ptr->جواب_TCB = گرہ_STATE(ksCurThread);
#ورنہ
    thread_state_ptr_سیٹ_blocking_IPCCan_Grant(&گرہ_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#اگر_ختم

    /* Place the thread in the endpoint queue */
    endpoint_دم = endpoint_ptr_get_ep_قطار_دم_fp(ep_ptr);
    اگر (likely(!endpoint_دم)) {
        گرہ_STATE(ksCurThread)->tcbEPPrev = تہی;
        گرہ_STATE(ksCurThread)->tcbEPNext = تہی;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_سیٹ_ep_قطار_سر_np(ep_ptr, TCB_REF(گرہ_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_قطار_دم_state(ep_ptr, TCB_REF(گرہ_STATE(ksCurThread)),
                                             EPState_Recv);
    } ورنہ {
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_قطار_t قطار = tcbAppend(گرہ_STATE(ksCurThread), ep_ptr_get_قطار(ep_ptr));
        endpoint_ptr_سیٹ_ep_قطار_سر_np(ep_ptr, TCB_REF(قطار.سر));
        endpoint_ptr_mset_ep_قطار_دم_state(ep_ptr, TCB_REF(قطار.انتہا), EPState_Recv);
#ورنہ
        /* Append current thread onto the queue. */
        endpoint_دم->tcbEPNext = گرہ_STATE(ksCurThread);
        گرہ_STATE(ksCurThread)->tcbEPPrev = endpoint_دم;
        گرہ_STATE(ksCurThread)->tcbEPNext = تہی;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_قطار_دم_state(ep_ptr, TCB_REF(گرہ_STATE(ksCurThread)),
                                             EPState_Recv);
#اگر_ختم
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    /* update call stack */
    لفظ_t پچھلا_ptr = call_ڈھیر_get_call_ڈھیر_Ptr(جواب_ptr->جواب_پچھلا);
    sched_context_t *sc = گرہ_STATE(ksCurThread)->tcbSchedContext;
    گرہ_STATE(ksCurThread)->tcbSchedContext = تہی;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_جواب = جواب_PTR(پچھلا_ptr);
    اگر (unlikely(جواب_PTR(پچھلا_ptr) != تہی)) {
        sc->sc_جواب->جواب_اگلا = جواب_ptr->جواب_اگلا;
    }

    /* TODO neccessary? */
    جواب_ptr->جواب_پچھلا.الفاظ[0] = 0;
    جواب_ptr->جواب_اگلا.الفاظ[0] = 0;
#ورنہ
    /* Delete the reply cap. */
    mdb_گرہ_ptr_mset_mdb_اگلا_mdb_Revocable_mdb_پہلا_Badged(
        &CTE_PTR(mdb_گرہ_get_mdb_پچھلا(caller_خانہ->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    caller_خانہ->cap = cap_null_cap_new();
    caller_خانہ->cteMDBNode = nullMDBNode;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
    اگر (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        لفظ_t pc = getRestartPC(caller);
        سیٹ_اگلا_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سیٹ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_مقصد_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } ورنہ {
#اگر_ختم
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(لمبائی, گرہ_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سیٹ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_مقصد_Thread_fp(caller, cap_pd, stored_hw_asid);

        پیغام_Info = لفظ_From_پیغام_Info(se_L4_پیغام_Info_سیٹ_caps_Unwrapped(info, 0));

        fastpath_restore(badge, پیغام_Info, گرہ_STATE(ksCurThread));

#اگر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
    }
#اگر_ختم
}

#اگر_تعریف_شدہ CONFIG_SIGNAL_FASTPATH
#اگر_تعریف_شدہ CONFIG_ARCH_ARM
ساکن اندرونی
FORCE_INLINE
#اگر_ختم
خالی NORETURN fastpath_signal(لفظ_t cptr, لفظ_t پیغام_Info)
{
    لفظ_t fault_type;
    sched_context_t *sc = تہی;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = تہی;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(گرہ_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    اگر (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    اگر (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    اگر (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    اگر (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    لفظ_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    انتخاب (ntfnState) {
    حالت Ntfn_State_فعال:
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم
        ntfn_سیٹ_فعال(ntfnPtr, badge | notification_ptr_get_ntfn_پیغام_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    حالت NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        اگر (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم
            ntfn_سیٹ_فعال(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        توڑو;
    حالت Ntfn_State_انتظار:
        dest = TCB_PTR(notification_ptr_get_ntfn_قطار_سر(ntfnPtr));
        توڑو;
    طےشدہ:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    اگر (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        اگر (sc == تہی || sc->scTcb != تہی) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#اگر defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        اگر (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#اگر_ختم
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    اگر (گرہ_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_ترجیح < dest->tcb_ترجیح) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    اگر (sc->scRefillMax > 0) {
        اگر (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    اگر (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم

    اگر (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } ورنہ {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    سیٹ_رجسٹر(dest, badge_رجسٹر, badge);
    thread_state_ptr_سیٹ_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    اگر (sc_sporadic(dest->tcbSchedContext)) {
        أكد(dest->tcbSchedContext != گرہ_STATE(ksCurSC));
        اگر (dest->tcbSchedContext != گرہ_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    اگر (schedulable) {
        اگر (گرہ_STATE(ksCurThread)->tcb_ترجیح > dest->tcb_ترجیح || crossnode) {
            SCHED_قطار_میں_ڈالو(dest);
        } ورنہ {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_EXCEPTION_FASTPATH
ساکن اندرونی
FORCE_INLINE
خالی NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_جڑ_t *cap_pd;
    لفظ_t badge;
    se_L4_پیغام_Info_t info;
    لفظ_t پیغام_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbFaultHandler)->cap;
#ورنہ
    cptr_t handlerCPtr;
    handlerCPtr = گرہ_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#اگر_ختم

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    اگر (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_جواب(handler_cap))
#اگر_ختم
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    اگر (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_جڑ_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    اگر (unlikely(! is_Valid_VTable_جڑ_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڈھونڈو_Map_For_ASID(asid);
    اگر (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_جڑ(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    اگر (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.الفاظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ورنہ
    stored_hw_asid.الفاظ[0] = asid;
#اگر_ختم
#اگر_ختم

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    اگر (unlikely(dest->tcb_ترجیح < گرہ_STATE(ksCurThread->tcb_ترجیح) &&
                 !isHighestPrio(dom, dest->tcb_ترجیح))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    اگر (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (unlikely(dest->tcbSchedContext != تہی)) {
        vm_fault_slowpath(type);
    }

    جواب_t *جواب = thread_state_get_جواب_Object_np(dest->tcbState);
    اگر (unlikely(جواب == تہی)) {
        vm_fault_slowpath(type);
    }
#اگر_ختم

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    اگر (unlikely(گرہ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#اگر_ختم /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_سیٹ_tcbfault_vm_fault(type);

#اگر_تعریف_شدہ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_اندراج.is_fastpath = true;
#اگر_ختم

    /* Dequeue the destination. */
    endpoint_ptr_سیٹ_ep_قطار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    اگر (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = تہی;
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
        dest->tcbEPNext = تہی;
#اگر_ختم
    } ورنہ {
        endpoint_ptr_mset_ep_قطار_دم_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سیٹ_ts_Type_np(&گرہ_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_جواب);
#اگر_تعریف_شدہ CONFIG_KERNEL_MCS

    thread_state_ptr_سیٹ_جواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سیٹ_جواب_Object_np(&گرہ_STATE(ksCurThread)->tcbState, جواب_REF(جواب));
    جواب->جواب_TCB = گرہ_STATE(ksCurThread);

    sched_context_t *sc = گرہ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    گرہ_STATE(ksCurThread)->tcbSchedContext = تہی;

    جواب_t *old_caller = sc->sc_جواب;
    جواب->جواب_پچھلا = call_ڈھیر_new(جواب_REF(sc->sc_جواب), false);
    اگر (unlikely(old_caller)) {
        old_caller->جواب_اگلا = call_ڈھیر_new(جواب_REF(جواب), false);
    }
    جواب->جواب_اگلا = call_ڈھیر_new(SC_REF(sc), true);
    sc->sc_جواب = جواب;
#ورنہ
    /* Get sender reply slot */
    cte_t *جواب_خانہ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcb_جواب);

    /* Get dest caller slot */
    cte_t *caller_خانہ = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    لفظ_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_جواب_cap_ptr_new_np(&caller_خانہ->cap, جواب_Can_Grant, 0, TCB_REF(گرہ_STATE(ksCurThread)));
    mdb_گرہ_ptr_سیٹ_mdb_پچھلا_np(&caller_خانہ->cteMDBNode, CTE_REF(جواب_خانہ));
    mdb_گرہ_ptr_mset_mdb_اگلا_mdb_Revocable_mdb_پہلا_Badged(&جواب_خانہ->cteMDBNode, CTE_REF(caller_خانہ), 1, 1);
#اگر_ختم
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_سیٹ_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_پیغام_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_لمبائی);

    /* Set the fault handler to running */
    thread_state_ptr_سیٹ_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_مقصد_Thread_fp(dest, cap_pd, stored_hw_asid);
    پیغام_Info = لفظ_From_پیغام_Info(se_L4_پیغام_Info_سیٹ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, پیغام_Info, گرہ_STATE(ksCurThread));
}
#اگر_ختم
