/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <fastpath/fastpath.h>

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#అన్తర్భావ <benchmark/benchmark_track.h>
#అయితే_ముగింపు
#అన్తర్భావ <benchmark/benchmark_utilisation.h>

#నిర్వచితమైతే CONFIG_ARCH_ARM
స్థిర అంతఃవరుస
FORCE_INLINE
#అయితే_ముగింపు
శూన్య NORETURN fastpath_call(మాట_t cptr, మాట_t సందేశం_Info)
{
    se_L4_సందేశం_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    మాట_t నిడివి;
    tcb_t *dest;
    మాట_t badge;
    cap_t newVTable;
    vspace_మూలం_t *cap_pd;
    pde_t stored_hw_asid;
    మాట_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = సందేశం_Info_From_మాట_raw(సందేశం_Info);
    నిడివి = se_L4_సందేశం_Info_get_నిడివి(info);
    fault_type = seL4_Fault_get_seL4_FaultType(కణుపు_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    ఒకవేళ (unlikely(fastpath_mi_check(సందేశం_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    ఒకవేళ (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_వరుస_తల(ep_ptr));

    /* Check that there's a thread waiting to receive */
    ఒకవేళ (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#ఒకవేళ defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    ఒకవేళ (unlikely(dest->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled)) {
        slowpath(SysCall);
    }
#అయితే_ముగింపు

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_మూలం_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    ఒకవేళ (unlikely(! is_Valid_VTable_మూలం_fp(newVTable))) {
        slowpath(SysCall);
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_స్థానం];
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_మూలం_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_మూలం(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ఒకవేళ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.పదాలు[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#లేకపోతే
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(newVTable);
    ఒకవేళ (unlikely(కనుగొను_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    ఒకవేళ (unlikely(dest->tcb_ప్రాధాన్యత < కణుపు_STATE(ksCurThread->tcb_ప్రాధాన్యత) &&
                 !isHighestPrio(dom, dest->tcb_ప్రాధాన్యత))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    ఒకవేళ (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_జవాబు(ep_cap))) {
        slowpath(SysCall);
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    ఒకవేళ (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#అయితే_ముగింపు

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ఒకవేళ (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (unlikely(dest->tcbSchedContext != ఖాళీ)) {
        slowpath(SysCall);
    }

    జవాబు_t *జవాబు = thread_state_get_జవాబు_Object_np(dest->tcbState);
    ఒకవేళ (unlikely(జవాబు == ఖాళీ)) {
        slowpath(SysCall);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు

    /* Dequeue the destination. */
    endpoint_ptr_సమితి_ep_వరుస_తల_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    ఒకవేళ (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = ఖాళీ;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        dest->tcbEPNext = ఖాళీ;
#అయితే_ముగింపు
    } లేకపోతే {
        endpoint_ptr_mset_ep_వరుస_తోక_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_సమితి_ts_Type_np(&కణుపు_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_జవాబు);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    thread_state_ptr_సమితి_జవాబు_Object_np(&dest->tcbState, 0);
    thread_state_ptr_సమితి_జవాబు_Object_np(&కణుపు_STATE(ksCurThread)->tcbState, జవాబు_REF(జవాబు));
    జవాబు->జవాబు_TCB = కణుపు_STATE(ksCurThread);

    sched_context_t *sc = కణుపు_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    కణుపు_STATE(ksCurThread)->tcbSchedContext = ఖాళీ;

    జవాబు_t *old_caller = sc->sc_జవాబు;
    జవాబు->జవాబు_Prev = call_దొంతర_new(జవాబు_REF(sc->sc_జవాబు), false);
    ఒకవేళ (unlikely(old_caller)) {
        old_caller->జవాబు_తదుపరి = call_దొంతర_new(జవాబు_REF(జవాబు), false);
    }
    జవాబు->జవాబు_తదుపరి = call_దొంతర_new(SC_REF(sc), true);
    sc->sc_జవాబు = జవాబు;
#లేకపోతే
    /* Get sender reply slot */
    cte_t *జవాబు_స్థానం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcb_జవాబు);

    /* Get dest caller slot */
    cte_t *caller_స్థానం = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    మాట_t జవాబు_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_జవాబు_cap_ptr_new_np(&caller_స్థానం->cap, జవాబు_Can_Grant, 0,
                             TCB_REF(కణుపు_STATE(ksCurThread)));
    mdb_కణుపు_ptr_సమితి_mdb_Prev_np(&caller_స్థానం->cteMDBNode, CTE_REF(జవాబు_స్థానం));
    mdb_కణుపు_ptr_mset_mdb_తదుపరి_mdb_Revocable_mdb_మొదటి_Badged(
        &జవాబు_స్థానం->cteMDBNode, CTE_REF(caller_స్థానం), 1, 1);
#అయితే_ముగింపు

    fastpath_copy_mrs(నిడివి, కణుపు_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_సమితి_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_గమ్యం_Thread_fp(dest, cap_pd, stored_hw_asid);

    సందేశం_Info = మాట_From_సందేశం_Info(se_L4_సందేశం_Info_సమితి_caps_Unwrapped(info, 0));

    fastpath_restore(badge, సందేశం_Info, కణుపు_STATE(ksCurThread));
}

#నిర్వచితమైతే CONFIG_ARCH_ARM
స్థిర అంతఃవరుస
FORCE_INLINE
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_KERNEL_MCS
శూన్య NORETURN fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info, మాట_t జవాబు)
#లేకపోతే
శూన్య NORETURN fastpath_జవాబు_recv(మాట_t cptr, మాట_t సందేశం_Info)
#అయితే_ముగింపు
{
    se_L4_సందేశం_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    మాట_t నిడివి;
    tcb_t *caller;
    మాట_t badge;
    tcb_t *endpoint_తోక;
    మాట_t fault_type;

    cap_t newVTable;
    vspace_మూలం_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = సందేశం_Info_From_మాట_raw(సందేశం_Info);
    నిడివి = se_L4_సందేశం_Info_get_నిడివి(info);
    fault_type = seL4_Fault_get_seL4_FaultType(కణుపు_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    ఒకవేళ (unlikely(fastpath_mi_check(సందేశం_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_జవాబు_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    ఒకవేళ (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_జవాబు_Recv);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t జవాబు_cap = lookup_fp(TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCTable)->cap, జవాబు);

    /* check it's a reply object */
    ఒకవేళ (unlikely(!cap_capType_equals(జవాబు_cap, cap_జవాబు_cap))) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు

    /* Check there is nothing waiting on the notification */
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(కణుపు_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_చురుకైన)) {
        slowpath(Sys_జవాబు_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    ఒకవేళ (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_జవాబు_Recv);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* Get the reply address */
    జవాబు_t *జవాబు_ptr = జవాబు_PTR(cap_జవాబు_cap_get_cap_జవాబు_Ptr(జవాబు_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    ఒకవేళ (unlikely(జవాబు_ptr->జవాబు_TCB == ఖాళీ ||
                 call_దొంతర_get_is_తల(జవాబు_ptr->జవాబు_తదుపరి) == 0 ||
                 SC_PTR(call_దొంతర_get_call_దొంతర_Ptr(జవాబు_ptr->జవాబు_తదుపరి)) != కణుపు_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_జవాబు_Recv);
    }

    /* Determine who the caller is. */
    caller = జవాబు_ptr->జవాబు_TCB;
#లేకపోతే
    /* Only reply if the reply cap is valid. */
    cte_t *caller_స్థానం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = caller_స్థానం->cap;
    ఒకవేళ (unlikely(!fastpath_జవాబు_cap_check(callerCap))) {
        slowpath(Sys_జవాబు_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_జవాబు_cap_get_cap_TCBPtr(callerCap));
#అయితే_ముగింపు

    /* ensure we are not single stepping the caller in ia32 */
#ఒకవేళ defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    ఒకవేళ (unlikely(caller->tcbArch.tcbContext.breakpointState.single_అడుగు_enabled)) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#నిర్వచితం_కాకపోతే CONFIG_EXCEPTION_FASTPATH
    ఒకవేళ (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_జవాబు_Recv);
    }
#లేకపోతే
    ఒకవేళ (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_మూలం_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    ఒకవేళ (unlikely(! is_Valid_VTable_మూలం_fp(newVTable))) {
        slowpath(Sys_జవాబు_Recv);
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_స్థానం];
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_మూలం_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map) != cap_pd)) {
        slowpath(Sys_జవాబు_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_మూలం(asid_map)) != cap_pd)) {
        slowpath(Sys_జవాబు_Recv);
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ఒకవేళ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_జవాబు_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.పదాలు[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#లేకపోతే
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(newVTable);
    ఒకవేళ (unlikely(కనుగొను_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(Sys_జవాబు_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    ఒకవేళ (unlikely(!isHighestPrio(dom, caller->tcb_ప్రాధాన్యత))) {
        slowpath(Sys_జవాబు_Recv);
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    ఒకవేళ (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ఒకవేళ (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_జవాబు_Recv);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (unlikely(caller->tcbSchedContext != ఖాళీ)) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_జవాబు_Recv);
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &కణుపు_STATE(ksCurThread)->tcbState, (మాట_t)ep_ptr, ThreadState_BlockedOnReceive);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_సమితి_జవాబు_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_సమితి_జవాబు_Object_np(&కణుపు_STATE(ksCurThread)->tcbState, జవాబు_REF(జవాబు_ptr));
    జవాబు_ptr->జవాబు_TCB = కణుపు_STATE(ksCurThread);
#లేకపోతే
    thread_state_ptr_సమితి_blocking_IPCCan_Grant(&కణుపు_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#అయితే_ముగింపు

    /* Place the thread in the endpoint queue */
    endpoint_తోక = endpoint_ptr_get_ep_వరుస_తోక_fp(ep_ptr);
    ఒకవేళ (likely(!endpoint_తోక)) {
        కణుపు_STATE(ksCurThread)->tcbEPPrev = ఖాళీ;
        కణుపు_STATE(ksCurThread)->tcbEPNext = ఖాళీ;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_సమితి_ep_వరుస_తల_np(ep_ptr, TCB_REF(కణుపు_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_వరుస_తోక_state(ep_ptr, TCB_REF(కణుపు_STATE(ksCurThread)),
                                             EPState_Recv);
    } లేకపోతే {
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_వరుస_t వరుస = tcbAppend(కణుపు_STATE(ksCurThread), ep_ptr_get_వరుస(ep_ptr));
        endpoint_ptr_సమితి_ep_వరుస_తల_np(ep_ptr, TCB_REF(వరుస.తల));
        endpoint_ptr_mset_ep_వరుస_తోక_state(ep_ptr, TCB_REF(వరుస.చివర), EPState_Recv);
#లేకపోతే
        /* Append current thread onto the queue. */
        endpoint_తోక->tcbEPNext = కణుపు_STATE(ksCurThread);
        కణుపు_STATE(ksCurThread)->tcbEPPrev = endpoint_తోక;
        కణుపు_STATE(ksCurThread)->tcbEPNext = ఖాళీ;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_వరుస_తోక_state(ep_ptr, TCB_REF(కణుపు_STATE(ksCurThread)),
                                             EPState_Recv);
#అయితే_ముగింపు
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    /* update call stack */
    మాట_t prev_ptr = call_దొంతర_get_call_దొంతర_Ptr(జవాబు_ptr->జవాబు_Prev);
    sched_context_t *sc = కణుపు_STATE(ksCurThread)->tcbSchedContext;
    కణుపు_STATE(ksCurThread)->tcbSchedContext = ఖాళీ;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_జవాబు = జవాబు_PTR(prev_ptr);
    ఒకవేళ (unlikely(జవాబు_PTR(prev_ptr) != ఖాళీ)) {
        sc->sc_జవాబు->జవాబు_తదుపరి = జవాబు_ptr->జవాబు_తదుపరి;
    }

    /* TODO neccessary? */
    జవాబు_ptr->జవాబు_Prev.పదాలు[0] = 0;
    జవాబు_ptr->జవాబు_తదుపరి.పదాలు[0] = 0;
#లేకపోతే
    /* Delete the reply cap. */
    mdb_కణుపు_ptr_mset_mdb_తదుపరి_mdb_Revocable_mdb_మొదటి_Badged(
        &CTE_PTR(mdb_కణుపు_get_mdb_Prev(caller_స్థానం->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    caller_స్థానం->cap = cap_null_cap_new();
    caller_స్థానం->cteMDBNode = nullMDBNode;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_EXCEPTION_FASTPATH
    ఒకవేళ (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        మాట_t pc = getRestartPC(caller);
        సమితి_తదుపరి_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_సమితి_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_గమ్యం_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } లేకపోతే {
#అయితే_ముగింపు
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(నిడివి, కణుపు_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_సమితి_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_గమ్యం_Thread_fp(caller, cap_pd, stored_hw_asid);

        సందేశం_Info = మాట_From_సందేశం_Info(se_L4_సందేశం_Info_సమితి_caps_Unwrapped(info, 0));

        fastpath_restore(badge, సందేశం_Info, కణుపు_STATE(ksCurThread));

#నిర్వచితమైతే CONFIG_EXCEPTION_FASTPATH
    }
#అయితే_ముగింపు
}

#నిర్వచితమైతే CONFIG_SIGNAL_FASTPATH
#నిర్వచితమైతే CONFIG_ARCH_ARM
స్థిర అంతఃవరుస
FORCE_INLINE
#అయితే_ముగింపు
శూన్య NORETURN fastpath_signal(మాట_t cptr, మాట_t సందేశం_Info)
{
    మాట_t fault_type;
    sched_context_t *sc = ఖాళీ;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = ఖాళీ;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(కణుపు_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    ఒకవేళ (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    ఒకవేళ (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    ఒకవేళ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    ఒకవేళ (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    మాట_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    ఎంపిక (ntfnState) {
    స్థితి Ntfn_State_చురుకైన:
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు
        ntfn_సమితి_చురుకైన(ntfnPtr, badge | notification_ptr_get_ntfn_సందేశం_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    స్థితి NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        ఒకవేళ (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు
            ntfn_సమితి_చురుకైన(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        ఆపు;
    స్థితి Ntfn_State_నిరీక్షణ:
        dest = TCB_PTR(notification_ptr_get_ntfn_వరుస_తల(ntfnPtr));
        ఆపు;
    అప్రమేయ:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    ఒకవేళ (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        ఒకవేళ (sc == ఖాళీ || sc->scTcb != ఖాళీ) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#ఒకవేళ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        ఒకవేళ (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#అయితే_ముగింపు
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    ఒకవేళ (కణుపు_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_ప్రాధాన్యత < dest->tcb_ప్రాధాన్యత) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    ఒకవేళ (sc->scRefillMax > 0) {
        ఒకవేళ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    ఒకవేళ (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు

    ఒకవేళ (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } లేకపోతే {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    సమితి_రిజిస్టరు(dest, badge_రిజిస్టరు, badge);
    thread_state_ptr_సమితి_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    ఒకవేళ (sc_sporadic(dest->tcbSchedContext)) {
        ప్రతిజ్ఞా(dest->tcbSchedContext != కణుపు_STATE(ksCurSC));
        ఒకవేళ (dest->tcbSchedContext != కణుపు_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    ఒకవేళ (schedulable) {
        ఒకవేళ (కణుపు_STATE(ksCurThread)->tcb_ప్రాధాన్యత > dest->tcb_ప్రాధాన్యత || crossnode) {
            SCHED_వరుసలో_చేర్చు(dest);
        } లేకపోతే {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_EXCEPTION_FASTPATH
స్థిర అంతఃవరుస
FORCE_INLINE
శూన్య NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_మూలం_t *cap_pd;
    మాట_t badge;
    se_L4_సందేశం_Info_t info;
    మాట_t సందేశం_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbFaultHandler)->cap;
#లేకపోతే
    cptr_t handlerCPtr;
    handlerCPtr = కణుపు_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#అయితే_ముగింపు

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    ఒకవేళ (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_జవాబు(handler_cap))
#అయితే_ముగింపు
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_వరుస_తల(ep_ptr));

    /* Check that there's a thread waiting to receive */
    ఒకవేళ (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_మూలం_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    ఒకవేళ (unlikely(! is_Valid_VTable_మూలం_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_మూలం(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ఒకవేళ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.పదాలు[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#లేకపోతే
    stored_hw_asid.పదాలు[0] = asid;
#అయితే_ముగింపు
#అయితే_ముగింపు

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    ఒకవేళ (unlikely(dest->tcb_ప్రాధాన్యత < కణుపు_STATE(ksCurThread->tcb_ప్రాధాన్యత) &&
                 !isHighestPrio(dom, dest->tcb_ప్రాధాన్యత))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ఒకవేళ (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (unlikely(dest->tcbSchedContext != ఖాళీ)) {
        vm_fault_slowpath(type);
    }

    జవాబు_t *జవాబు = thread_state_get_జవాబు_Object_np(dest->tcbState);
    ఒకవేళ (unlikely(జవాబు == ఖాళీ)) {
        vm_fault_slowpath(type);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ఒకవేళ (unlikely(కణుపు_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_సమితి_tcbfault_vm_fault(type);

#నిర్వచితమైతే CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_నమోదు.is_fastpath = true;
#అయితే_ముగింపు

    /* Dequeue the destination. */
    endpoint_ptr_సమితి_ep_వరుస_తల_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    ఒకవేళ (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = ఖాళీ;
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        dest->tcbEPNext = ఖాళీ;
#అయితే_ముగింపు
    } లేకపోతే {
        endpoint_ptr_mset_ep_వరుస_తోక_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_సమితి_ts_Type_np(&కణుపు_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_జవాబు);
#నిర్వచితమైతే CONFIG_KERNEL_MCS

    thread_state_ptr_సమితి_జవాబు_Object_np(&dest->tcbState, 0);
    thread_state_ptr_సమితి_జవాబు_Object_np(&కణుపు_STATE(ksCurThread)->tcbState, జవాబు_REF(జవాబు));
    జవాబు->జవాబు_TCB = కణుపు_STATE(ksCurThread);

    sched_context_t *sc = కణుపు_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    కణుపు_STATE(ksCurThread)->tcbSchedContext = ఖాళీ;

    జవాబు_t *old_caller = sc->sc_జవాబు;
    జవాబు->జవాబు_Prev = call_దొంతర_new(జవాబు_REF(sc->sc_జవాబు), false);
    ఒకవేళ (unlikely(old_caller)) {
        old_caller->జవాబు_తదుపరి = call_దొంతర_new(జవాబు_REF(జవాబు), false);
    }
    జవాబు->జవాబు_తదుపరి = call_దొంతర_new(SC_REF(sc), true);
    sc->sc_జవాబు = జవాబు;
#లేకపోతే
    /* Get sender reply slot */
    cte_t *జవాబు_స్థానం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcb_జవాబు);

    /* Get dest caller slot */
    cte_t *caller_స్థానం = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    మాట_t జవాబు_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_జవాబు_cap_ptr_new_np(&caller_స్థానం->cap, జవాబు_Can_Grant, 0, TCB_REF(కణుపు_STATE(ksCurThread)));
    mdb_కణుపు_ptr_సమితి_mdb_Prev_np(&caller_స్థానం->cteMDBNode, CTE_REF(జవాబు_స్థానం));
    mdb_కణుపు_ptr_mset_mdb_తదుపరి_mdb_Revocable_mdb_మొదటి_Badged(&జవాబు_స్థానం->cteMDBNode, CTE_REF(caller_స్థానం), 1, 1);
#అయితే_ముగింపు
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_సమితి_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_సందేశం_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_నిడివి);

    /* Set the fault handler to running */
    thread_state_ptr_సమితి_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_గమ్యం_Thread_fp(dest, cap_pd, stored_hw_asid);
    సందేశం_Info = మాట_From_సందేశం_Info(se_L4_సందేశం_Info_సమితి_caps_Unwrapped(info, 0));

    fastpath_restore(badge, సందేశం_Info, కణుపు_STATE(ksCurThread));
}
#అయితే_ముగింపు
