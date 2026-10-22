/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <fastpath/fastpath.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_track.h>
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <benchmark/benchmark_utilisation.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_ARM
ಸ್ಥಿರ ಸಾಲೊಳಗೆ
FORCE_INLINE
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ NORETURN fastpath_call(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
{
    se_L4_ಸಂದೇಶ_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    ಶಬ್ದ_t ಉದ್ದಳತೆ;
    tcb_t *dest;
    ಶಬ್ದ_t badge;
    cap_t newVTable;
    vspace_ಬೇರು_t *cap_pd;
    pde_t stored_hw_asid;
    ಶಬ್ದ_t fault_type;
    dom_t dom;

    /* Get message info, length, and fault type. */
    info = ಸಂದೇಶ_Info_From_ಶಬ್ದ_raw(ಸಂದೇಶ_Info);
    ಉದ್ದಳತೆ = se_L4_ಸಂದೇಶ_Info_get_ಉದ್ದಳತೆ(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ಗಂಟು_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    ಆದರೆ (unlikely(fastpath_mi_check(ಸಂದೇಶ_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(SysCall);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's an endpoint */
    ಆದರೆ (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanSend(ep_cap))) {
        slowpath(SysCall);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Get the destination thread, which is only going to be valid
     * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_ಸರತಿ_ತಲೆ(ep_ptr));

    /* Check that there's a thread waiting to receive */
    ಆದರೆ (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        slowpath(SysCall);
    }

    /* ensure we are not single stepping the destination in ia32 */
#ಆದರೆ defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    ಆದರೆ (unlikely(dest->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled)) {
        slowpath(SysCall);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ಬೇರು_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    ಆದರೆ (unlikely(! is_Valid_VTable_ಬೇರು_fp(newVTable))) {
        slowpath(SysCall);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    /* Get HW ASID */
    stored_hw_asid = cap_pd[PD_ASID_ಸ್ಥಾನ];
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_ಬೇರು_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map)) != cap_pd)) {
        slowpath(SysCall);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ಆದರೆ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(SysCall);
    }
    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ಪದಗಳು[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ಇಲ್ಲದಿದ್ದರೆ
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(newVTable);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(SysCall);
    }
    /* Get HW ASID */
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    ಆದರೆ (unlikely(dest->tcb_ಆದ್ಯತೆ < ಗಂಟು_STATE(ksCurThread->tcb_ಆದ್ಯತೆ) &&
                 !isHighestPrio(dom, dest->tcb_ಆದ್ಯತೆ))) {
        slowpath(SysCall);
    }

    /* Ensure that the endpoint has has grant or grant-reply rights so that we can
     * create the reply cap */
    ಆದರೆ (unlikely(!cap_endpoint_cap_get_capCanGrant(ep_cap) &&
                 !cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(ep_cap))) {
        slowpath(SysCall);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    ಆದರೆ (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(SysCall);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ಆದರೆ (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(SysCall);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (unlikely(dest->tcbSchedContext != ಖಾಲಿ)) {
        slowpath(SysCall);
    }

    ಉತ್ತರ_t *ಉತ್ತರ = thread_state_get_ಉತ್ತರ_Object_np(dest->tcbState);
    ಆದರೆ (unlikely(ಉತ್ತರ == ಖಾಲಿ)) {
        slowpath(SysCall);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        slowpath(SysCall);
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ

    /* Dequeue the destination. */
    endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    ಆದರೆ (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        dest->tcbEPNext = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        endpoint_ptr_mset_ep_ಸರತಿ_ಬಾಲ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(ep_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_ಗಣ_ts_Type_np(&ಗಂಟು_STATE(ksCurThread)->tcbState,
                                   Thread_State_Blocked_On_ಉತ್ತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&dest->tcbState, 0);
    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&ಗಂಟು_STATE(ksCurThread)->tcbState, ಉತ್ತರ_REF(ಉತ್ತರ));
    ಉತ್ತರ->ಉತ್ತರ_TCB = ಗಂಟು_STATE(ksCurThread);

    sched_context_t *sc = ಗಂಟು_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ಗಂಟು_STATE(ksCurThread)->tcbSchedContext = ಖಾಲಿ;

    ಉತ್ತರ_t *old_caller = sc->sc_ಉತ್ತರ;
    ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(ಉತ್ತರ_REF(sc->sc_ಉತ್ತರ), false);
    ಆದರೆ (unlikely(old_caller)) {
        old_caller->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(ಉತ್ತರ_REF(ಉತ್ತರ), false);
    }
    ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(SC_REF(sc), true);
    sc->sc_ಉತ್ತರ = ಉತ್ತರ;
#ಇಲ್ಲದಿದ್ದರೆ
    /* Get sender reply slot */
    cte_t *ಉತ್ತರ_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcb_ಉತ್ತರ);

    /* Get dest caller slot */
    cte_t *caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    ಶಬ್ದ_t ಉತ್ತರ_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_ಉತ್ತರ_cap_ptr_new_np(&caller_ಸ್ಥಾನ->cap, ಉತ್ತರ_Can_Grant, 0,
                             TCB_REF(ಗಂಟು_STATE(ksCurThread)));
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev_np(&caller_ಸ್ಥಾನ->cteMDBNode, CTE_REF(ಉತ್ತರ_ಸ್ಥಾನ));
    mdb_ಗಂಟು_ptr_mset_mdb_ಮುಂದಿನ_mdb_Revocable_mdb_ಮೊದಲ_Badged(
        &ಉತ್ತರ_ಸ್ಥಾನ->cteMDBNode, CTE_REF(caller_ಸ್ಥಾನ), 1, 1);
#ಆದರೆ_ಅಂತ್ಯ

    fastpath_copy_mrs(ಉದ್ದಳತೆ, ಗಂಟು_STATE(ksCurThread), dest);

    /* Dest thread is set Running, but not queued. */
    thread_state_ptr_ಗಣ_ts_Type_np(&dest->tcbState,
                                   ThreadState_Running);
    switch_ಗಮ್ಯ_Thread_fp(dest, cap_pd, stored_hw_asid);

    ಸಂದೇಶ_Info = ಶಬ್ದ_From_ಸಂದೇಶ_Info(se_L4_ಸಂದೇಶ_Info_ಗಣ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, ಸಂದೇಶ_Info, ಗಂಟು_STATE(ksCurThread));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_ARM
ಸ್ಥಿರ ಸಾಲೊಳಗೆ
FORCE_INLINE
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ NORETURN fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info, ಶಬ್ದ_t ಉತ್ತರ)
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ NORETURN fastpath_ಉತ್ತರ_recv(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
#ಆದರೆ_ಅಂತ್ಯ
{
    se_L4_ಸಂದೇಶ_Info_t info;
    cap_t ep_cap;
    endpoint_t *ep_ptr;
    ಶಬ್ದ_t ಉದ್ದಳತೆ;
    tcb_t *caller;
    ಶಬ್ದ_t badge;
    tcb_t *endpoint_ಬಾಲ;
    ಶಬ್ದ_t fault_type;

    cap_t newVTable;
    vspace_ಬೇರು_t *cap_pd;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get message info and length */
    info = ಸಂದೇಶ_Info_From_ಶಬ್ದ_raw(ಸಂದೇಶ_Info);
    ಉದ್ದಳತೆ = se_L4_ಸಂದೇಶ_Info_get_ಉದ್ದಳತೆ(info);
    fault_type = seL4_Fault_get_seL4_FaultType(ಗಂಟು_STATE(ksCurThread)->tcbFault);

    /* Check there's no extra caps, the length is ok and there's no
     * saved fault. */
    ಆದರೆ (unlikely(fastpath_mi_check(ಸಂದೇಶ_Info) ||
                 fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

    /* Lookup the cap */
    ep_cap = lookup_fp(TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCTable)->cap,
                       cptr);

    /* Check it's an endpoint */
    ಆದರೆ (unlikely(!cap_capType_equals(ep_cap, cap_endpoint_cap) ||
                 !cap_endpoint_cap_get_capCanReceive(ep_cap))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* lookup the reply object */
    cap_t ಉತ್ತರ_cap = lookup_fp(TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCTable)->cap, ಉತ್ತರ);

    /* check it's a reply object */
    ಆದರೆ (unlikely(!cap_capType_equals(ಉತ್ತರ_cap, cap_ಉತ್ತರ_cap))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Check there is nothing waiting on the notification */
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification &&
                 notification_ptr_get_state(ಗಂಟು_STATE(ksCurThread)->tcbBoundNotification) == Ntfn_State_ಸಕ್ರಿಯ)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(ep_cap));

    /* Check that there's not a thread waiting to send */
    ಆದರೆ (unlikely(endpoint_ptr_get_state(ep_ptr) == EPState_Send)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* Get the reply address */
    ಉತ್ತರ_t *ಉತ್ತರ_ptr = ಉತ್ತರ_PTR(cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Ptr(ಉತ್ತರ_cap));
    /* check that its valid and at the head of the call chain
       and that the current thread's SC is going to be donated. */
    ಆದರೆ (unlikely(ಉತ್ತರ_ptr->ಉತ್ತರ_TCB == ಖಾಲಿ ||
                 call_ರಾಶಿ_get_is_ತಲೆ(ಉತ್ತರ_ptr->ಉತ್ತರ_ಮುಂದಿನ) == 0 ||
                 SC_PTR(call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ_ptr->ಉತ್ತರ_ಮುಂದಿನ)) != ಗಂಟು_STATE(ksCurThread)->tcbSchedContext)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

    /* Determine who the caller is. */
    caller = ಉತ್ತರ_ptr->ಉತ್ತರ_TCB;
#ಇಲ್ಲದಿದ್ದರೆ
    /* Only reply if the reply cap is valid. */
    cte_t *caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCaller);
    cap_t callerCap = caller_ಸ್ಥಾನ->cap;
    ಆದರೆ (unlikely(!fastpath_ಉತ್ತರ_cap_check(callerCap))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

    /* Determine who the caller is. */
    caller = TCB_PTR(cap_ಉತ್ತರ_cap_get_cap_TCBPtr(callerCap));
#ಆದರೆ_ಅಂತ್ಯ

    /* ensure we are not single stepping the caller in ia32 */
#ಆದರೆ defined(CONFIG_HARDWARE_DEBUG_API) && defined(CONFIG_ARCH_IA32)
    ಆದರೆ (unlikely(caller->tcbArch.tcbContext.breakpointState.single_ಹೆಜ್ಜೆ_enabled)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Check that the caller has not faulted, in which case a fault
       reply is generated instead. */
    fault_type = seL4_Fault_get_seL4_FaultType(caller->tcbFault);

    /* Change this as more types of faults are supported */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_EXCEPTION_FASTPATH
    ಆದರೆ (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (unlikely(fault_type != seL4_Fault_NullFault && fault_type != seL4_Fault_VMFault)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(caller, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ಬೇರು_fp(newVTable);

    /* Ensure that the destination has a valid MMU. */
    ಆದರೆ (unlikely(! is_Valid_VTable_ಬೇರು_fp(newVTable))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    /* Get HWASID. */
    stored_hw_asid = cap_pd[PD_ASID_ಸ್ಥಾನ];
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CONFIG_ARCH_X86_64) || defined(CONFIG_ARCH_IA32)
    /* Need to test that the ASID is still valid */
    asid_t asid = vspace_ಬೇರು_get_Mapped_ASID_fp(newVTable);
    asid_map_t asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map) != cap_pd)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
    /* Borrow the stored_hw_asid for PCID on x86_64. On ia32 this value will be ignored, but
     * it is passed to a function below, so at least some value needs to be written. */
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map)) != cap_pd)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ಆದರೆ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ಪದಗಳು[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ಇಲ್ಲದಿದ್ದರೆ
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_RISCV
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(newVTable);
    ಆದರೆ (unlikely(ಕಂಡುಹಿಡಿ_VSpace_For_ASID_fp(asid) != cap_pd)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
    /* Get HW ASID */
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ

    /* Ensure the original caller can be scheduled directly. */
    dom = maxDom ? ksCurDomain : 0;
    ಆದರೆ (unlikely(!isHighestPrio(dom, caller->tcb_ಆದ್ಯತೆ))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH32
    /* Ensure the HWASID is valid. */
    ಆದರೆ (unlikely(!pde_pde_invalid_get_stored_asid_valid(stored_hw_asid))) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ಆದರೆ (unlikely(caller->tcbDomain != ksCurDomain && 0 < maxDom)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (unlikely(caller->tcbSchedContext != ಖಾಲಿ)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcbAffinity != caller->tcbAffinity)) {
        slowpath(Sys_ಉತ್ತರ_Recv);
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ

    /* Set thread state to BlockedOnReceive */
    thread_state_ptr_mset_blockingObject_tsType(
        &ಗಂಟು_STATE(ksCurThread)->tcbState, (ಶಬ್ದ_t)ep_ptr, ThreadState_BlockedOnReceive);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* unlink reply object from caller */
    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&caller->tcbState, 0);
    /* set the reply object */
    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&ಗಂಟು_STATE(ksCurThread)->tcbState, ಉತ್ತರ_REF(ಉತ್ತರ_ptr));
    ಉತ್ತರ_ptr->ಉತ್ತರ_TCB = ಗಂಟು_STATE(ksCurThread);
#ಇಲ್ಲದಿದ್ದರೆ
    thread_state_ptr_ಗಣ_blocking_IPCCan_Grant(&ಗಂಟು_STATE(ksCurThread)->tcbState,
                                             cap_endpoint_cap_get_capCanGrant(ep_cap));;
#ಆದರೆ_ಅಂತ್ಯ

    /* Place the thread in the endpoint queue */
    endpoint_ಬಾಲ = endpoint_ptr_get_ep_ಸರತಿ_ಬಾಲ_fp(ep_ptr);
    ಆದರೆ (likely(!endpoint_ಬಾಲ)) {
        ಗಂಟು_STATE(ksCurThread)->tcbEPPrev = ಖಾಲಿ;
        ಗಂಟು_STATE(ksCurThread)->tcbEPNext = ಖಾಲಿ;

        /* Set head/tail of queue and endpoint state. */
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ_np(ep_ptr, TCB_REF(ಗಂಟು_STATE(ksCurThread)));
        endpoint_ptr_mset_ep_ಸರತಿ_ಬಾಲ_state(ep_ptr, TCB_REF(ಗಂಟು_STATE(ksCurThread)),
                                             EPState_Recv);
    } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        /* Update queue. */
        tcb_ಸರತಿ_t ಸರತಿ = tcbAppend(ಗಂಟು_STATE(ksCurThread), ep_ptr_get_ಸರತಿ(ep_ptr));
        endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ_np(ep_ptr, TCB_REF(ಸರತಿ.ತಲೆ));
        endpoint_ptr_mset_ep_ಸರತಿ_ಬಾಲ_state(ep_ptr, TCB_REF(ಸರತಿ.ಕೊನೆ), EPState_Recv);
#ಇಲ್ಲದಿದ್ದರೆ
        /* Append current thread onto the queue. */
        endpoint_ಬಾಲ->tcbEPNext = ಗಂಟು_STATE(ksCurThread);
        ಗಂಟು_STATE(ksCurThread)->tcbEPPrev = endpoint_ಬಾಲ;
        ಗಂಟು_STATE(ksCurThread)->tcbEPNext = ಖಾಲಿ;

        /* Update tail of queue. */
        endpoint_ptr_mset_ep_ಸರತಿ_ಬಾಲ_state(ep_ptr, TCB_REF(ಗಂಟು_STATE(ksCurThread)),
                                             EPState_Recv);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    /* update call stack */
    ಶಬ್ದ_t prev_ptr = call_ರಾಶಿ_get_call_ರಾಶಿ_Ptr(ಉತ್ತರ_ptr->ಉತ್ತರ_Prev);
    sched_context_t *sc = ಗಂಟು_STATE(ksCurThread)->tcbSchedContext;
    ಗಂಟು_STATE(ksCurThread)->tcbSchedContext = ಖಾಲಿ;
    caller->tcbSchedContext = sc;
    sc->scTcb = caller;

    sc->sc_ಉತ್ತರ = ಉತ್ತರ_PTR(prev_ptr);
    ಆದರೆ (unlikely(ಉತ್ತರ_PTR(prev_ptr) != ಖಾಲಿ)) {
        sc->sc_ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = ಉತ್ತರ_ptr->ಉತ್ತರ_ಮುಂದಿನ;
    }

    /* TODO neccessary? */
    ಉತ್ತರ_ptr->ಉತ್ತರ_Prev.ಪದಗಳು[0] = 0;
    ಉತ್ತರ_ptr->ಉತ್ತರ_ಮುಂದಿನ.ಪದಗಳು[0] = 0;
#ಇಲ್ಲದಿದ್ದರೆ
    /* Delete the reply cap. */
    mdb_ಗಂಟು_ptr_mset_mdb_ಮುಂದಿನ_mdb_Revocable_mdb_ಮೊದಲ_Badged(
        &CTE_PTR(mdb_ಗಂಟು_get_mdb_Prev(caller_ಸ್ಥಾನ->cteMDBNode))->cteMDBNode,
        0, 1, 1);
    caller_ಸ್ಥಾನ->cap = cap_null_cap_new();
    caller_ಸ್ಥಾನ->cteMDBNode = nullMDBNode;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXCEPTION_FASTPATH
    ಆದರೆ (unlikely(fault_type != seL4_Fault_NullFault)) {
        /* Note - this works as is for VM faults but will need to be changed when other faults are added. VM faults always
         * restart the faulting thread upon reply but this is not always the case with other types of faults. This can either
         * be handled in the fastpath or redirected to the slowpath, but either way, this code must be changed so we do not
         * forcefully switch to a thread which is meant to stay inactive. */


        /* In the slowpath, the thread is set to ThreadState_Restart and its PC is set to its restartPC in activateThread().
         * In the fastpath, this step is bypassed and we directly complete the activateThread() steps that set the PC and make
         * the thread runnable. */
        ಶಬ್ದ_t pc = getRestartPC(caller);
        ಗಣ_ಮುಂದಿನ_PC(caller, pc);

        /* Clear the tcbFault variable to indicate that it has been handled. */
        caller->tcbFault = seL4_Fault_NullFault_new();

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_ಗಣ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_ಗಮ್ಯ_Thread_fp(caller, cap_pd, stored_hw_asid);

        /* The badge/msginfo do not need to be not sent - this is not necessary for exceptions */
        restore_user_context();
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ_ಅಂತ್ಯ
        /* There's no fault, so straight to the transfer. */

        /* Replies don't have a badge. */
        badge = 0;

        fastpath_copy_mrs(ಉದ್ದಳತೆ, ಗಂಟು_STATE(ksCurThread), caller);

        /* Dest thread is set Running, but not queued. */
        thread_state_ptr_ಗಣ_ts_Type_np(&caller->tcbState, ThreadState_Running);
        switch_ಗಮ್ಯ_Thread_fp(caller, cap_pd, stored_hw_asid);

        ಸಂದೇಶ_Info = ಶಬ್ದ_From_ಸಂದೇಶ_Info(se_L4_ಸಂದೇಶ_Info_ಗಣ_caps_Unwrapped(info, 0));

        fastpath_restore(badge, ಸಂದೇಶ_Info, ಗಂಟು_STATE(ksCurThread));

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXCEPTION_FASTPATH
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_SIGNAL_FASTPATH
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_ARM
ಸ್ಥಿರ ಸಾಲೊಳಗೆ
FORCE_INLINE
#ಆದರೆ_ಅಂತ್ಯ
ಶೂನ್ಯ NORETURN fastpath_signal(ಶಬ್ದ_t cptr, ಶಬ್ದ_t ಸಂದೇಶ_Info)
{
    ಶಬ್ದ_t fault_type;
    sched_context_t *sc = ಖಾಲಿ;
    bool_t schedulable = false;
    bool_t crossnode = false;
    bool_t idle = false;
    tcb_t *dest = ಖಾಲಿ;

    /* Get fault type. */
    fault_type = seL4_Fault_get_seL4_FaultType(ಗಂಟು_STATE(ksCurThread)->tcbFault);

    /* Check there's no saved fault. Can be removed if the current thread can't
     * have a fault while invoking the fastpath */
    ಆದರೆ (unlikely(fault_type != seL4_Fault_NullFault)) {
        slowpath(SysSend);
    }

    /* Lookup the cap */
    cap_t cap = lookup_fp(TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCTable)->cap, cptr);

    /* Check it's a notification */
    ಆದರೆ (unlikely(!cap_capType_equals(cap, cap_notification_cap))) {
        slowpath(SysSend);
    }

    /* Check that we are allowed to send to this cap */
    ಆದರೆ (unlikely(!cap_notification_cap_get_capNtfnCanSend(cap))) {
        slowpath(SysSend);
    }

    /* Check that the current domain hasn't expired */
    ಆದರೆ (unlikely(isCurDomainExpired())) {
        slowpath(SysSend);
    }

    /* Get the notification address */
    notification_t *ntfnPtr = NTFN_PTR(cap_notification_cap_get_capNtfnPtr(cap));

    /* Get the notification state */
    uint32_t ntfnState = notification_ptr_get_state(ntfnPtr);

    /* Get the notification badge */
    ಶಬ್ದ_t badge = cap_notification_cap_get_capNtfnBadge(cap);
    ಆಯ್ಕೆ (ntfnState) {
    ಸ್ಥಿತಿ Ntfn_State_ಸಕ್ರಿಯ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
        ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ
        ntfn_ಗಣ_ಸಕ್ರಿಯ(ntfnPtr, badge | notification_ptr_get_ntfn_ಸಂದೇಶ_Identifier(ntfnPtr));
        restore_user_context();
        UNREACHABLE();
    ಸ್ಥಿತಿ NtfnState_Idle:
        dest = (tcb_t *) notification_ptr_get_ntfnBoundTCB(ntfnPtr);

        ಆದರೆ (!dest || thread_state_ptr_get_tsType(&dest->tcbState) != ThreadState_BlockedOnReceive) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
            ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ
            ntfn_ಗಣ_ಸಕ್ರಿಯ(ntfnPtr, badge);
            restore_user_context();
            UNREACHABLE();
        }

        idle = true;
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ Ntfn_State_ಕಾಯುವಿಕೆ:
        dest = TCB_PTR(notification_ptr_get_ntfn_ಸರತಿ_ತಲೆ(ntfnPtr));
        ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid notification state");
    }

    /* Get the bound SC of the signalled thread */
    sc = dest->tcbSchedContext;

    /* If the signalled thread doesn't have a bound SC, check if one can be
     * donated from the notification. If not, go to the slowpath */
    ಆದರೆ (!sc) {
        sc = SC_PTR(notification_ptr_get_ntfnSchedContext(ntfnPtr));
        ಆದರೆ (sc == ಖಾಲಿ || sc->scTcb != ಖಾಲಿ) {
            slowpath(SysSend);
        }

        /* Slowpath the case where dest has its FPU context in the FPU of a core*/
#ಆದರೆ defined(ENABLE_SMP_SUPPORT) && defined(CONFIG_HAVE_FPU)
        ಆದರೆ (nativeThreadUsingFPU(dest)) {
            slowpath(SysSend);
        }
#ಆದರೆ_ಅಂತ್ಯ
    }

    /* Only fastpath signal to threads which will not become the new highest prio thread on the
     * core of their SC, even if the currently running thread on the core is the idle thread. */
    ಆದರೆ (ಗಂಟು_STATE_ON_CORE(ksCurThread, sc->scCore)->tcb_ಆದ್ಯತೆ < dest->tcb_ಆದ್ಯತೆ) {
        slowpath(SysSend);
    }

    /* Simplified schedContext_resume that does not change state and reverts to the
     * slowpath in cases where the SC does not have sufficient budget, as this case
     * adds extra scheduler logic. Normally, this is done after donation of SC
     * but after tweaking it, I don't see anything executed in schedContext_donate
     * that will affect the conditions of this check */
    ಆದರೆ (sc->scRefillMax > 0) {
        ಆದರೆ (!(refill_ready(sc) && refill_sufficient(sc, 0))) {
            slowpath(SysSend);
        }
        schedulable = true;
    }

    /* Check if signal is cross-core or cross-domain */
    ಆದರೆ (ksCurDomain != dest->tcbDomain SMP_COND_STATEMENT( || sc->scCore != getCurrentCPUIndex())) {
        crossnode = true;
    }

    /*  Point of no return */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (idle) {
        /* Cancel the IPC that the signalled thread is waiting on */
        cancelIPC_fp(dest);
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Dequeue dest from the notification queue */
        tcbNTFNDequeue(dest, ntfnPtr);
    }

    /* Wake up the signalled thread and transfer badge */
    ಗಣ_ರಿಜಿಸ್ಟರ್(dest, badge_ರಿಜಿಸ್ಟರ್, badge);
    thread_state_ptr_ಗಣ_ts_Type_np(&dest->tcbState, ThreadState_Running);

    /* Donate SC if necessary. The checks for this were already done before
     * the point of no return */
    maybeDonateSchedContext_fp(dest, sc);

    /* Left this in the same form as the slowpath. Not sure if optimal */
    ಆದರೆ (sc_sporadic(dest->tcbSchedContext)) {
        ಪ್ರತಿಜ್ಞಾ(dest->tcbSchedContext != ಗಂಟು_STATE(ksCurSC));
        ಆದರೆ (dest->tcbSchedContext != ಗಂಟು_STATE(ksCurSC)) {
            refill_unblock_check(dest->tcbSchedContext);
        }
    }

    /* If dest was already not schedulable prior to the budget check
     * the slowpath doesn't seem to do anything special besides just not
     * not scheduling the dest thread. */
    ಆದರೆ (schedulable) {
        ಆದರೆ (ಗಂಟು_STATE(ksCurThread)->tcb_ಆದ್ಯತೆ > dest->tcb_ಆದ್ಯತೆ || crossnode) {
            SCHED_ಸರತಿಗೆ_ಸೇರಿಸು(dest);
        } ಇಲ್ಲದಿದ್ದರೆ {
            SCHED_APPEND(dest);
        }
    }

    restore_user_context();
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_EXCEPTION_FASTPATH
ಸ್ಥಿರ ಸಾಲೊಳಗೆ
FORCE_INLINE
ಶೂನ್ಯ NORETURN fastpath_vm_fault(vm_fault_type_t type)
{
    cap_t handler_cap;
    endpoint_t *ep_ptr;
    tcb_t *dest;
    cap_t newVTable;
    vspace_ಬೇರು_t *cap_pd;
    ಶಬ್ದ_t badge;
    se_L4_ಸಂದೇಶ_Info_t info;
    ಶಬ್ದ_t ಸಂದೇಶ_Info;
    pde_t stored_hw_asid;
    dom_t dom;

    /* Get the fault handler endpoint */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    handler_cap = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbFaultHandler)->cap;
#ಇಲ್ಲದಿದ್ದರೆ
    cptr_t handlerCPtr;
    handlerCPtr = ಗಂಟು_STATE(ksCurThread)->tcbFaultHandler;
    handler_cap = lookup_fp(TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCTable)->cap, handlerCPtr);
#ಆದರೆ_ಅಂತ್ಯ

    /* Check that the cap is an endpoint cap and on non-mcs, that you can send to it and create the reply cap */
    ಆದರೆ (unlikely(!cap_capType_equals(handler_cap, cap_endpoint_cap)
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
                 || !cap_endpoint_cap_get_capCanSend(handler_cap) || (!cap_endpoint_cap_get_capCanGrant(handler_cap) &&
                                                                      !cap_endpoint_cap_get_cap_Can_Grant_ಉತ್ತರ(handler_cap))
#ಆದರೆ_ಅಂತ್ಯ
                )) {
        vm_fault_slowpath(type);
    }

    /* Get the endpoint address */
    ep_ptr = EP_PTR(cap_endpoint_cap_get_capEPPtr(handler_cap));

    /* Get the destination thread, which is only going to be valid
    * if the endpoint is valid. */
    dest = TCB_PTR(endpoint_ptr_get_ep_ಸರತಿ_ತಲೆ(ep_ptr));

    /* Check that there's a thread waiting to receive */
    ಆದರೆ (unlikely(endpoint_ptr_get_state(ep_ptr) != EPState_Recv)) {
        vm_fault_slowpath(type);
    }

    /* Get destination thread.*/
    newVTable = TCB_PTR_CTE_PTR(dest, tcbVTable)->cap;

    /* Get vspace root. */
    cap_pd = cap_vtable_cap_get_vspace_ಬೇರು_fp(newVTable);

    /* Ensure that the destination has a valid VTable. */
    ಆದರೆ (unlikely(! is_Valid_VTable_ಬೇರು_fp(newVTable))) {
        vm_fault_slowpath(type);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
    /* Need to test that the ASID is still valid */
    asid_t asid = cap_vspace_cap_get_capVSMappedASID(newVTable);
    asid_map_t asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (unlikely(asid_map_get_type(asid_map) != asid_map_asid_map_vspace ||
                 VSPACE_PTR(asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map)) != cap_pd)) {
        vm_fault_slowpath(type);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Ensure the vmid is valid. */
    ಆದರೆ (unlikely(!asid_map_asid_map_vspace_get_stored_vmid_valid(asid_map))) {
        vm_fault_slowpath(type);
    }

    /* vmids are the tags used instead of hw_asids in hyp mode */
    stored_hw_asid.ಪದಗಳು[0] = asid_map_asid_map_vspace_get_stored_hw_vmid(asid_map);
#ಇಲ್ಲದಿದ್ದರೆ
    stored_hw_asid.ಪದಗಳು[0] = asid;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ_ಅಂತ್ಯ

    /* let gcc optimise this out for 1 domain */
    dom = maxDom ? ksCurDomain : 0;
    /* ensure only the idle thread or lower prio threads are present in the scheduler */
    ಆದರೆ (unlikely(dest->tcb_ಆದ್ಯತೆ < ಗಂಟು_STATE(ksCurThread->tcb_ಆದ್ಯತೆ) &&
                 !isHighestPrio(dom, dest->tcb_ಆದ್ಯತೆ))) {

        vm_fault_slowpath(type);
    }

    /* Ensure the original caller is in the current domain and can be scheduled directly. */
    ಆದರೆ (unlikely(dest->tcbDomain != ksCurDomain && 0 < maxDom)) {
        vm_fault_slowpath(type);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (unlikely(dest->tcbSchedContext != ಖಾಲಿ)) {
        vm_fault_slowpath(type);
    }

    ಉತ್ತರ_t *ಉತ್ತರ = thread_state_get_ಉತ್ತರ_Object_np(dest->tcbState);
    ಆದರೆ (unlikely(ಉತ್ತರ == ಖಾಲಿ)) {
        vm_fault_slowpath(type);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    /* Ensure both threads have the same affinity */
    ಆದರೆ (unlikely(ಗಂಟು_STATE(ksCurThread)->tcbAffinity != dest->tcbAffinity)) {
        vm_fault_slowpath(type);
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

    /*
     * --- POINT OF NO RETURN ---
     *
     * At this stage, we have committed to performing the IPC.
     */

    /* Sets the tcb fault based on the vm fault information. Has one slowpath transition
    but only for a debug fault on AARCH32 */

    fastpath_ಗಣ_tcbfault_vm_fault(type);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_BENCHMARK_TRACK_KERNEL_ENTRIES
    ks_Kernel_ನಮೂದು.is_fastpath = true;
#ಆದರೆ_ಅಂತ್ಯ

    /* Dequeue the destination. */
    endpoint_ptr_ಗಣ_ep_ಸರತಿ_ತಲೆ_np(ep_ptr, TCB_REF(dest->tcbEPNext));
    ಆದರೆ (unlikely(dest->tcbEPNext)) {
        dest->tcbEPNext->tcbEPPrev = ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        dest->tcbEPNext = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
    } ಇಲ್ಲದಿದ್ದರೆ {
        endpoint_ptr_mset_ep_ಸರತಿ_ಬಾಲ_state(ep_ptr, 0, EPState_Idle);
    }

    badge = cap_endpoint_cap_get_capEPBadge(handler_cap);

    /* Unlink dest <-> reply, link src (cur thread) <-> reply */
    thread_state_ptr_ಗಣ_ts_Type_np(&ಗಂಟು_STATE(ksCurThread)->tcbState, Thread_State_Blocked_On_ಉತ್ತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS

    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&dest->tcbState, 0);
    thread_state_ptr_ಗಣ_ಉತ್ತರ_Object_np(&ಗಂಟು_STATE(ksCurThread)->tcbState, ಉತ್ತರ_REF(ಉತ್ತರ));
    ಉತ್ತರ->ಉತ್ತರ_TCB = ಗಂಟು_STATE(ksCurThread);

    sched_context_t *sc = ಗಂಟು_STATE(ksCurThread)->tcbSchedContext;
    sc->scTcb = dest;
    dest->tcbSchedContext = sc;
    ಗಂಟು_STATE(ksCurThread)->tcbSchedContext = ಖಾಲಿ;

    ಉತ್ತರ_t *old_caller = sc->sc_ಉತ್ತರ;
    ಉತ್ತರ->ಉತ್ತರ_Prev = call_ರಾಶಿ_new(ಉತ್ತರ_REF(sc->sc_ಉತ್ತರ), false);
    ಆದರೆ (unlikely(old_caller)) {
        old_caller->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(ಉತ್ತರ_REF(ಉತ್ತರ), false);
    }
    ಉತ್ತರ->ಉತ್ತರ_ಮುಂದಿನ = call_ರಾಶಿ_new(SC_REF(sc), true);
    sc->sc_ಉತ್ತರ = ಉತ್ತರ;
#ಇಲ್ಲದಿದ್ದರೆ
    /* Get sender reply slot */
    cte_t *ಉತ್ತರ_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcb_ಉತ್ತರ);

    /* Get dest caller slot */
    cte_t *caller_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(dest, tcbCaller);

    /* Insert reply cap */
    ಶಬ್ದ_t ಉತ್ತರ_Can_Grant = thread_state_ptr_get_blockingIPCCanGrant(&dest->tcbState);;
    cap_ಉತ್ತರ_cap_ptr_new_np(&caller_ಸ್ಥಾನ->cap, ಉತ್ತರ_Can_Grant, 0, TCB_REF(ಗಂಟು_STATE(ksCurThread)));
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev_np(&caller_ಸ್ಥಾನ->cteMDBNode, CTE_REF(ಉತ್ತರ_ಸ್ಥಾನ));
    mdb_ಗಂಟು_ptr_mset_mdb_ಮುಂದಿನ_mdb_Revocable_mdb_ಮೊದಲ_Badged(&ಉತ್ತರ_ಸ್ಥಾನ->cteMDBNode, CTE_REF(caller_ಸ್ಥಾನ), 1, 1);
#ಆದರೆ_ಅಂತ್ಯ
    /* Set the message registers for the vm fault*/
    fastpath_vm_fault_ಗಣ_mrs(dest);

    /* Generate the msginfo */
    info = se_L4_ಸಂದೇಶ_Info_new(seL4_Fault_VMFault, 0, 0, se_L4_VMFault_ಉದ್ದಳತೆ);

    /* Set the fault handler to running */
    thread_state_ptr_ಗಣ_ts_Type_np(&dest->tcbState, ThreadState_Running);
    switch_ಗಮ್ಯ_Thread_fp(dest, cap_pd, stored_hw_asid);
    ಸಂದೇಶ_Info = ಶಬ್ದ_From_ಸಂದೇಶ_Info(se_L4_ಸಂದೇಶ_Info_ಗಣ_caps_Unwrapped(info, 0));

    fastpath_restore(badge, ಸಂದೇಶ_Info, ಗಂಟು_STATE(ksCurThread));
}
#ಆದರೆ_ಅಂತ್ಯ
