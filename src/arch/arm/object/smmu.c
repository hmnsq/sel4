/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/smmu.h>

ਸਥਿਰ exception_t checkARMCBVspace(cap_t cap)
{
    ਸ਼ਬਦ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_ਥਾਂ = smmuStateCBNode + cb;
    ਜੇ (unlikely(!is_VTable_ਜੜ੍ਹ(cb_ਥਾਂ->cap))) {
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                          cte_t *src_ਥਾਂ, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{

    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ, sid;
    cte_t *dest_ਥਾਂ;
    cap_t cnodeCap;
    lookup_ਥਾਂ_ret_t lu_ret;
    exception_t ਸਥਿਤੀ;
    uint32_t fault_ਸਥਿਤੀ_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    ਜੇ (label == ARMSIDGetFault) {
        thread = ਗੰਢ_STATE(ksCurThread);
        smmu_read_fault_state(&fault_ਸਥਿਤੀ_2, &faultSyndrome_0, &faultSyndrome_1);
        ਜੇ (call) {
            ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, fault_ਸਥਿਤੀ_2);
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 1, faultSyndrome_0);
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 2, faultSyndrome_1);
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                            se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, 3)));
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, ਬਫਰ);
    ਸੂਚਕਾਂਕ = getSyscallArg(1, ਬਫਰ);
    ਡੂੰਘਾਈ = getSyscallArg(2, ਬਫਰ);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (ਪੂਰਨ)sid);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
        userError("Rejecting request for SID %u. Already active.", (ਪੂਰਨ)sid);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)sid);
        ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
    }
    dest_ਥਾਂ = lu_ret.ਥਾਂ;
    ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)sid);
        ਵਾਪਸ ਸਥਿਤੀ;
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_ਪਾਓ(cap_sid_cap_new(sid), src_ਥਾਂ, dest_ਥਾਂ);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                   cte_t *src_ਥਾਂ, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cap_t cbCap;
    cte_t *cb_Cap_ਥਾਂ;
    cte_t *cb_ਸੌਂਪੋ_ਥਾਂ;
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t sid;

    ਚੋਣ (label) {
    ਹਾਲਤ ARMSIDBindCB:
        ਜੇ (unlikely(current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_ਥਾਂ->cap;
        ਜੇ (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_ਸੌਂਪੋ_ਥਾਂ = smmuStateSIDNode + sid;
        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(cb_ਸੌਂਪੋ_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_ਪਾਓ(cbCap, cb_Cap_ਥਾਂ, cb_ਸੌਂਪੋ_ਥਾਂ);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_ਸੈੱਟ_cap_Bind_SID(&(cb_ਸੌਂਪੋ_ਥਾਂ->cap), sid);
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_ਸੌਂਪੋ_ਥਾਂ = smmuStateSIDNode + sid;
        ਜੇ (unlikely(cap_get_capType(cb_ਸੌਂਪੋ_ਥਾਂ->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸਥਿਤੀ = cteDelete(cb_ਸੌਂਪੋ_ਥਾਂ, true);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ EXCEPTION_NONE;
    ਮੂਲ:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    ਸ਼ਬਦ_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_ਸੌਂਪੋ_ਥਾਂ = smmuStateSIDNode + sid;
    exception_t ਸਥਿਤੀ = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    ਜੇ (unlikely(cap_get_capType(cb_ਸੌਂਪੋ_ਥਾਂ->cap) == cap_cb_cap)) {
        ਸਥਿਤੀ = cteDelete(cb_ਸੌਂਪੋ_ਥਾਂ, true);
    }
    smmuStateSIDTable[sid] = false;
    ਵਾਪਸ ਸਥਿਤੀ;
}

exception_t decodeARMCBControlInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                         cte_t *src_ਥਾਂ, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{

    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ, cb;
    cte_t *dest_ਥਾਂ;
    cap_t cnodeCap;
    lookup_ਥਾਂ_ret_t lu_ret;
    exception_t ਸਥਿਤੀ;

    ਜੇ (label == ARMCBTLBInvalidateAll) {
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        ਵਾਪਸ EXCEPTION_NONE;
    }

    ਜੇ (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, ਬਫਰ);
    ਸੂਚਕਾਂਕ = getSyscallArg(1, ਬਫਰ);
    ਡੂੰਘਾਈ = getSyscallArg(2, ਬਫਰ);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (ਪੂਰਨ)cb);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
        userError("Rejecting request for CB %u. Already active.", (ਪੂਰਨ)cb);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnodeCap, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)cb);
        ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
    }
    dest_ਥਾਂ = lu_ret.ਥਾਂ;
    ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(ਬਫਰ, 0), (ਪੂਰਨ)cb);
        ਵਾਪਸ ਸਥਿਤੀ;
    }

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_ਪਾਓ(cap_cb_cap_new(SID_INVALID, cb), src_ਥਾਂ, dest_ਥਾਂ);
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                  cte_t *src_ਥਾਂ, cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_ਥਾਂ;
    cte_t *cb_ਥਾਂ;
    exception_t ਸਥਿਤੀ;
    ਸ਼ਬਦ_t cb;
    uint32_t fault_ਸਥਿਤੀ_2;
    ਸ਼ਬਦ_t fault_ਪਤਾ;
    tcb_t *thread;

    ਚੋਣ (label) {
    ਹਾਲਤ ARMCBTLBInvalidate:
        ਜੇ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_ਥਾਂ = smmuStateCBNode + cb;
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_ਥਾਂ->cap));
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ ARMCBAssignVspace:
        ਜੇ (unlikely(current_extra_caps.excaprefs[0] == ਨਿਰਾ)) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ਥਾਂ->cap;

        ਜੇ (unlikely(!is_VTable_ਜੜ੍ਹ(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_ਥਾਂ = smmuStateCBNode + cb;
        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(cb_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_ਸੌਂਪੋ_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_ਪਾਓ(vspaceCap, vspace_Cap_ਥਾਂ, cb_ਥਾਂ);
        cap_vspace_cap_ptr_ਸੈੱਟ_cap_VSMapped_CB(&(cb_ਥਾਂ->cap), cb);
        /*set relationship between CB and ASID*/
        smmu_State_CBAsid_ਸਾਰਣੀ[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ ARMCBUnassignVspace:
        ਜੇ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_ਥਾਂ = smmuStateCBNode + cb;
        ਸਥਿਤੀ = cteDelete(cb_ਥਾਂ, true);
        ਜੇ (unlikely(ਸਥਿਤੀ != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ ARMCBGetFault:
        thread = ਗੰਢ_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_ਸਥਿਤੀ_2, &fault_ਪਤਾ);
        ਜੇ (call) {
            ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, fault_ਸਥਿਤੀ_2);
            ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 1, fault_ਪਤਾ);
            ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                            se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, 2)));
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
        ਵਾਪਸ EXCEPTION_NONE;

    ਹਾਲਤ ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ EXCEPTION_NONE;

    ਮੂਲ:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    ਸ਼ਬਦ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_ਥਾਂ;
    exception_t ਸਥਿਤੀ = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    ਜੇ (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_ਥਾਂ = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        ਸਥਿਤੀ = cteDelete(cb_ਥਾਂ, true);
    }
    smmuStateCBTable[cb] = false;
    ਵਾਪਸ ਸਥਿਤੀ;
}

ਖਾਲੀ smmu_cb_delete_vspace(ਸ਼ਬਦ_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_ਸਾਰਣੀ[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

ਖਾਲੀ invalidateSMMUTLBByASID(asid_t asid, ਸ਼ਬਦ_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    ਲਈ (ਪੂਰਨ cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ਜੇ (unlikely(smmu_State_CBAsid_ਸਾਰਣੀ[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

ਖਾਲੀ invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, ਸ਼ਬਦ_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    ਲਈ (ਪੂਰਨ cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ਜੇ (unlikely(smmu_State_CBAsid_ਸਾਰਣੀ[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#ਜੇ_ਅੰਤ

