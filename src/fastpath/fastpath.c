/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <fastpath/fastpath.h>

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ضمن <benchmark/benchmark_track.h>
#جيڪڏهن_پورو
#ضمن <benchmark/benchmark_utilisation.h>

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#جيڪڏهن_پورو
فراغ NORETURN fastpath_call(لفظ_t cptr, لفظ_t نياپو_Info)
{
    se_L4_پيغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    لفظ_t ڊيگهائي;
    tcb_t *dest;
    لفظ_t badge;
    cap_t newVTable;
    vspace_پاڙ_t *cap_pd;
    pde_t stored_hw_asid;
    لفظ_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = پيغام_Info_From_لفظ_raw(نياپو_Info);
    ڊيگهائي = se_L4_پيغام_Info_get_ڊيگهائي(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ڳنڍ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    إذا (unlikely(fastpath_mi_check(نياپو_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    إذا (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#إذا defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    إذا (unlikely(dest->tcbArch.tcbContext.breakpointState.single_قدم_enabled)) {
        slowpath(SysCall);
    }
#جيڪڏهن_پورو

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_پاڙ_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    إذا (unlikely(! is_Valid_VTable_پاڙ_fp(newVTable))) {
        slowpath(SysCall);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_جڳهه];
#جيڪڏهن_پورو

#إذا defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_پاڙ_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.لفظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    إذا (unlikely(ڳوليو_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    إذا (unlikely(dest->tcb_ترجيح < ڳنڍ_STATE(ksCurThread->tcb_ترجيح) &&
                 !isHighestPrio(dom, dest->tcb_ترجيح))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    إذا (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_جواب(ep_cap))) {
        slowpath(SysCall);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    إذا (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#جيڪڏهن_پورو

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (unlikely(dest->tcbSchedContext != عدم)) {
        slowpath(SysCall);
    }

    جواب_t *جواب = thread_state_get_جواب_Object_np(dest->tcbState);
    إذا (unlikely(جواب == عدم)) {
        slowpath(SysCall);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(ڳنڍ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو

    /* Dequeue the destination. */
    endpoint_ptr_سيٽ_ep_قطار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    إذا (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = عدم;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        dest->tcbEPNext = عدم;
#جيڪڏهن_پورو
    } وإلا {
        endpoint_ptr_mset_ep_قطار_پڇ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سيٽ_ts_Type_np(&ڳنڍ_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_جواب);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    thread_state_ptr_سيٽ_جواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سيٽ_جواب_Object_np(&ڳنڍ_STATE(ksCurThread)->tcbState, جواب_REF(جواب));
    جواب->جواب_TCB = ڳنڍ_STATE(ksCurThread);

    sched_context_t *sc = ڳنڍ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ڳنڍ_STATE(ksCurThread)->tcbSchedContext = عدم;

    جواب_t *old_caller = sc->sc_جواب;
    جواب->جواب_Prev = call_ڍير_new(جواب_REF(sc->sc_جواب), false);
    إذا (unlikely(old_caller)) {
        old_caller->جواب_ايندڙ = call_ڍير_new(جواب_REF(جواب), false);
    }
    جواب->جواب_ايندڙ = call_ڍير_new(SC_REF(sc), true);
    sc->sc_جواب = جواب;
#وإلا
    /* Get sender reply slot */
    cte_t *جواب_جڳهه = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcb_جواب);

    /* Get dest caller slot */
    cte_t *caller_جڳهه = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    لفظ_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_جواب_cap_ptr_new_np(&caller_جڳهه->cap, جواب_Can_Grant, 0,
                             TCB_REF(ڳنڍ_STATE(ksCurThread)));
    mdb_ڳنڍ_ptr_سيٽ_mdb_Prev_np(&caller_جڳهه->cteMDBNode, CTE_REF(جواب_جڳهه));
    mdb_ڳنڍ_ptr_mset_mdb_ايندڙ_mdb_Revocable_mdb_پهريون_Badged(
        &جواب_جڳهه->cteMDBNode, CTE_REF(caller_جڳهه), 1, 1);
#جيڪڏهن_پورو

    fastpath_copy_mrs(ڊيگهائي, ڳنڍ_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_سيٽ_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_منزل_Thread_fp(dest, cap_pd, stored_hw_asid);

    نياپو_Info = لفظ_From_پيغام_Info(se_L4_پيغام_Info_سيٽ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, نياپو_Info, ڳنڍ_STATE(ksCurThread));
}

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
فراغ NORETURN fastpath_جواب_recv(لفظ_t cptr, لفظ_t نياپو_Info, لفظ_t جواب)
#وإلا
فراغ NORETURN fastpath_جواب_recv(لفظ_t cptr, لفظ_t نياپو_Info)
#جيڪڏهن_پورو
{
    se_L4_پيغام_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    لفظ_t ڊيگهائي;
    tcb_t *caller;
    لفظ_t badge;
    tcb_t *endpoint_پڇ;
    لفظ_t fault_type;

    cap_t newVTable;
    vspace_پاڙ_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = پيغام_Info_From_لفظ_raw(نياپو_Info);
    ڊيگهائي = se_L4_پيغام_Info_get_ڊيگهائي(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ڳنڍ_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    إذا (unlikely(fastpath_mi_check(نياپو_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    إذا (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_جواب_Recv);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t جواب_cap = lookup_fp(TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCTable)->cap, جواب);

    /* check it's a reply object */
    إذا (unlikely(!cap_capType_equals(جواب_cap, cap_جواب_cap))) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو

    /* Check there is nothing waiting on the notification */
    إذا (unlikely(ڳنڍ_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(ڳنڍ_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_سرگرم)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_جواب_Recv);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* Get the reply address */
    جواب_t *جواب_ptr = جواب_PTR(cap_جواب_cap_get_cap_جواب_Ptr(جواب_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    إذا (unlikely(جواب_ptr->جواب_TCB == عدم ||
                 call_ڍير_get_is_سر(جواب_ptr->جواب_ايندڙ) == 0 ||
                 SC_PTR(call_ڍير_get_call_ڍير_Ptr(جواب_ptr->جواب_ايندڙ)) != ڳنڍ_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_جواب_Recv);
    }

    /* Determine who the caller is. */
    caller = جواب_ptr->جواب_TCB;
#وإلا
    /* Only reply if the reply cap is valid. */
    cte_t *caller_جڳهه = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = caller_جڳهه->cap;
    إذا (unlikely(!fastpath_جواب_cap_check(callerCap))) {
        slowpath(Sys_جواب_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_جواب_cap_get_cap_TCBPtr(callerCap));
#جيڪڏهن_پورو

    /* ensure we are not single stepping the caller in ia32 */
#إذا defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    إذا (unlikely(caller->tcbArch.tcbContext.breakpointState.single_قدم_enabled)) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#جيڪڏهن_وصف_نه_ٿيل CONFIG_EXCEPTION_FASTPATH
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_جواب_Recv);
    }
#وإلا
    إذا (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_پاڙ_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    إذا (unlikely(! is_Valid_VTable_پاڙ_fp(newVTable))) {
        slowpath(Sys_جواب_Recv);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_جڳهه];
#جيڪڏهن_پورو

#إذا defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_پاڙ_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map)) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_جواب_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.لفظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(newVTable);
    إذا (unlikely(ڳوليو_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(Sys_جواب_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    إذا (unlikely(!isHighestPrio(dom, caller->tcb_ترجيح))) {
        slowpath(Sys_جواب_Recv);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    إذا (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_جواب_Recv);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (unlikely(caller->tcbSchedContext != عدم)) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(ڳنڍ_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_جواب_Recv);
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &ڳنڍ_STATE(ksCurThread)->tcbState, (لفظ_t)ep_ptr, ThreadState_BlockedOnReceive);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_سيٽ_جواب_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_سيٽ_جواب_Object_np(&ڳنڍ_STATE(ksCurThread)->tcbState, جواب_REF(جواب_ptr));
    جواب_ptr->جواب_TCB = ڳنڍ_STATE(ksCurThread);
#وإلا
    thread_state_ptr_سيٽ_blocking_IPCCan_Grant(&ڳنڍ_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#جيڪڏهن_پورو

    /* Place the thread in the endpoint queue */
    endpoint_پڇ = endpoint_ptr_get_ep_قطار_پڇ_fp(ep_ptr);
    إذا (likely(!endpoint_پڇ)) {
        ڳنڍ_STATE(ksCurThread)->tcbEPPrev = عدم;
        ڳنڍ_STATE(ksCurThread)->tcbEPNext = عدم;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_سيٽ_ep_قطار_سر_np(ep_ptr, TCB_REF(ڳنڍ_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_قطار_پڇ_state(ep_ptr, TCB_REF(ڳنڍ_STATE(ksCurThread)),
                                             EPState_Recv);
    } وإلا {
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_قطار_t قطار = tcbAppend(ڳنڍ_STATE(ksCurThread), ep_ptr_get_قطار(ep_ptr));
        endpoint_ptr_سيٽ_ep_قطار_سر_np(ep_ptr, TCB_REF(قطار.سر));
        endpoint_ptr_mset_ep_قطار_پڇ_state(ep_ptr, TCB_REF(قطار.پڇاڙي), EPState_Recv);
#وإلا
        /* Append current thread onto the queue. */
        endpoint_پڇ->tcbEPNext = ڳنڍ_STATE(ksCurThread);
        ڳنڍ_STATE(ksCurThread)->tcbEPPrev = endpoint_پڇ;
        ڳنڍ_STATE(ksCurThread)->tcbEPNext = عدم;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_قطار_پڇ_state(ep_ptr, TCB_REF(ڳنڍ_STATE(ksCurThread)),
                                             EPState_Recv);
#جيڪڏهن_پورو
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    /* update call stack */
    لفظ_t prev_ptr = call_ڍير_get_call_ڍير_Ptr(جواب_ptr->جواب_Prev);
    sched_context_t *sc = ڳنڍ_STATE(ksCurThread)->tcbSchedContext;
    ڳنڍ_STATE(ksCurThread)->tcbSchedContext = عدم;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_جواب = جواب_PTR(prev_ptr);
    إذا (unlikely(جواب_PTR(prev_ptr) != عدم)) {
        sc->sc_جواب->جواب_ايندڙ = جواب_ptr->جواب_ايندڙ;
    }

    /* TODO neccessary? */
    جواب_ptr->جواب_Prev.لفظ[0] = 0;
    جواب_ptr->جواب_ايندڙ.لفظ[0] = 0;
#وإلا
    /* Delete the reply cap. */
    mdb_ڳنڍ_ptr_mset_mdb_ايندڙ_mdb_Revocable_mdb_پهريون_Badged(
        &CTE_PTR(mdb_ڳنڍ_get_mdb_Prev(caller_جڳهه->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    caller_جڳهه->cap = cap_null_cap_new();
    caller_جڳهه->cteMDBNode = nullMDBNode;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_EXCEPTION_FASTPATH
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        لفظ_t pc = getRestartPC(caller);
        سيٽ_ايندڙ_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سيٽ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_منزل_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } وإلا {
#جيڪڏهن_پورو
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(ڊيگهائي, ڳنڍ_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_سيٽ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_منزل_Thread_fp(caller, cap_pd, stored_hw_asid);

        نياپو_Info = لفظ_From_پيغام_Info(se_L4_پيغام_Info_سيٽ_caps_Unwrapped(info, 0));

        fastpath_restore(badge, نياپو_Info, ڳنڍ_STATE(ksCurThread));

#جيڪڏهن_وصف_ٿيل CONFIG_EXCEPTION_FASTPATH
    }
#جيڪڏهن_پورو
}

#جيڪڏهن_وصف_ٿيل CONFIG_SIGNAL_FASTPATH
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_ARM
ساكن مضمن
FORCE_INLINE
#جيڪڏهن_پورو
فراغ NORETURN fastpath_signal(لفظ_t cptr, لفظ_t نياپو_Info)
{
    لفظ_t fault_type;
    sched_context_t *sc = عدم;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = عدم;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(ڳنڍ_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    إذا (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCTable)->cap, cptr);

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
    لفظ_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    بدل (ntfnState) {
    حالة Ntfn_State_سرگرم:
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو
        ntfn_سيٽ_سرگرم(ntfnPtr, badge | notification_ptr_get_ntfn_نياپو_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    حالة NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        إذا (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو
            ntfn_سيٽ_سرگرم(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        اكسر;
    حالة Ntfn_State_انتظار:
        dest = TCB_PTR(notification_ptr_get_ntfn_قطار_سر(ntfnPtr));
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
#جيڪڏهن_پورو
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    إذا (ڳنڍ_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_ترجيح < dest->tcb_ترجيح) {
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
#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو

    إذا (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } وإلا {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    سيٽ_رجسٽر(dest, badge_رجسٽر, badge);
    thread_state_ptr_سيٽ_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    إذا (sc_sporadic(dest->tcbSchedContext)) {
        أكد(dest->tcbSchedContext != ڳنڍ_STATE(ksCurSC));
        إذا (dest->tcbSchedContext != ڳنڍ_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    إذا (schedulable) {
        إذا (ڳنڍ_STATE(ksCurThread)->tcb_ترجيح > dest->tcb_ترجيح || crossnode) {
            SCHED_قطار_اندر_وجهو(dest);
        } وإلا {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_EXCEPTION_FASTPATH
ساكن مضمن
FORCE_INLINE
فراغ NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_پاڙ_t *cap_pd;
    لفظ_t badge;
    se_L4_پيغام_Info_t info;
    لفظ_t نياپو_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbFaultHandler)->cap;
#وإلا
    cptr_t handlerCPtr;
    handlerCPtr = ڳنڍ_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#جيڪڏهن_پورو

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    إذا (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_جواب(handler_cap))
#جيڪڏهن_پورو
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_قطار_سر(ep_ptr));

    /* Check that there's a thread waiting to receive */
    إذا (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_پاڙ_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    إذا (unlikely(! is_Valid_VTable_پاڙ_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    إذا (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.لفظ[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#وإلا
    stored_hw_asid.لفظ[0] = asid;
#جيڪڏهن_پورو
#جيڪڏهن_پورو

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    إذا (unlikely(dest->tcb_ترجيح < ڳنڍ_STATE(ksCurThread->tcb_ترجيح) &&
                 !isHighestPrio(dom, dest->tcb_ترجيح))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    إذا (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (unlikely(dest->tcbSchedContext != عدم)) {
        vm_fault_slowpath(type);
    }

    جواب_t *جواب = thread_state_get_جواب_Object_np(dest->tcbState);
    إذا (unlikely(جواب == عدم)) {
        vm_fault_slowpath(type);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    إذا (unlikely(ڳنڍ_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_سيٽ_tcbfault_vm_fault(type);

#جيڪڏهن_وصف_ٿيل CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_داخلا.is_fastpath = true;
#جيڪڏهن_پورو

    /* Dequeue the destination. */
    endpoint_ptr_سيٽ_ep_قطار_سر_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    إذا (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = عدم;
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        dest->tcbEPNext = عدم;
#جيڪڏهن_پورو
    } وإلا {
        endpoint_ptr_mset_ep_قطار_پڇ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_سيٽ_ts_Type_np(&ڳنڍ_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_جواب);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS

    thread_state_ptr_سيٽ_جواب_Object_np(&dest->tcbState, 0);
    thread_state_ptr_سيٽ_جواب_Object_np(&ڳنڍ_STATE(ksCurThread)->tcbState, جواب_REF(جواب));
    جواب->جواب_TCB = ڳنڍ_STATE(ksCurThread);

    sched_context_t *sc = ڳنڍ_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ڳنڍ_STATE(ksCurThread)->tcbSchedContext = عدم;

    جواب_t *old_caller = sc->sc_جواب;
    جواب->جواب_Prev = call_ڍير_new(جواب_REF(sc->sc_جواب), false);
    إذا (unlikely(old_caller)) {
        old_caller->جواب_ايندڙ = call_ڍير_new(جواب_REF(جواب), false);
    }
    جواب->جواب_ايندڙ = call_ڍير_new(SC_REF(sc), true);
    sc->sc_جواب = جواب;
#وإلا
    /* Get sender reply slot */
    cte_t *جواب_جڳهه = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcb_جواب);

    /* Get dest caller slot */
    cte_t *caller_جڳهه = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    لفظ_t جواب_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_جواب_cap_ptr_new_np(&caller_جڳهه->cap, جواب_Can_Grant, 0, TCB_REF(ڳنڍ_STATE(ksCurThread)));
    mdb_ڳنڍ_ptr_سيٽ_mdb_Prev_np(&caller_جڳهه->cteMDBNode, CTE_REF(جواب_جڳهه));
    mdb_ڳنڍ_ptr_mset_mdb_ايندڙ_mdb_Revocable_mdb_پهريون_Badged(&جواب_جڳهه->cteMDBNode, CTE_REF(caller_جڳهه), 1, 1);
#جيڪڏهن_پورو
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_سيٽ_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_پيغام_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_ڊيگهائي);

    /* Set the fault handler to running */
    thread_state_ptr_سيٽ_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_منزل_Thread_fp(dest, cap_pd, stored_hw_asid);
    نياپو_Info = لفظ_From_پيغام_Info(se_L4_پيغام_Info_سيٽ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, نياپو_Info, ڳنڍ_STATE(ksCurThread));
}
#جيڪڏهن_پورو
