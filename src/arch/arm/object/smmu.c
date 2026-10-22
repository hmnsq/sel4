/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#که_تعریف_شوی CONFIG_ARM_SMMU
#ضمن <arch/object/smmu.h>

ساكن exception_t checkARMCBVspace(cap_t cap)
{
    کلمه_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_خونه = smmuStateCBNode + cb;
    إذا (unlikely(!is_VTable_ریښه(cb_خونه->cap))) {
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                          cte_t *src_خونه, cap_t cap, bool_t call, کلمه_t *بفر)
{

    کلمه_t شاخص, ژوروالی, sid;
    cte_t *dest_خونه;
    cap_t cnodeCap;
    lookup_خونه_ret_t lu_ret;
    exception_t حالت;
    uint32_t fault_حالت_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    إذا (label == ARMSIDGetFault) {
        thread = غوټه_STATE(ksCurThread);
        smmu_read_fault_state(&fault_حالت_2, &faultSyndrome_0, &faultSyndrome_1);
        إذا (call) {
            کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
            سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
            سېټ_MR(thread, ipc_بفر, 0, fault_حالت_2);
            سېټ_MR(thread, ipc_بفر, 1, faultSyndrome_0);
            سېټ_MR(thread, ipc_بفر, 2, faultSyndrome_1);
            سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                            se_L4_پیغام_Info_new(0, 0, 0, 3)));
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;
    }

    إذا (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, بفر);
    شاخص = getSyscallArg(1, بفر);
    ژوروالی = getSyscallArg(2, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_لومړی;
        userError("Rejecting request for SID %u. Already active.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(بفر, 0), (صحيح)sid);
        أرجع lu_ret.حالت;
    }
    dest_خونه = lu_ret.خونه;
    حالت = ensure_تشه_خونه(dest_خونه);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(بفر, 0), (صحيح)sid);
        أرجع حالت;
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_ورننباسه(cap_sid_cap_new(sid), src_خونه, dest_خونه);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                   cte_t *src_خونه, cap_t cap, bool_t call, کلمه_t *بفر)
{
    cap_t cbCap;
    cte_t *cb_Cap_خونه;
    cte_t *cb_ټاکل_خونه;
    exception_t حالت;
    کلمه_t sid;

    بدل (label) {
    حالة ARMSIDBindCB:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_خونه = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_خونه->cap;
        إذا (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_ټاکل_خونه = smmuStateSIDNode + sid;
        حالت = ensure_تشه_خونه(cb_ټاکل_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            أرجع حالت;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_ورننباسه(cbCap, cb_Cap_خونه, cb_ټاکل_خونه);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_سېټ_cap_Bind_SID(&(cb_ټاکل_خونه->cap), sid);
        أرجع EXCEPTION_NONE;

    حالة ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_ټاکل_خونه = smmuStateSIDNode + sid;
        إذا (unlikely(cap_get_capType(cb_ټاکل_خونه->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        حالت = cteDelete(cb_ټاکل_خونه, true);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            أرجع حالت;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    افتراضي:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    کلمه_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_ټاکل_خونه = smmuStateSIDNode + sid;
    exception_t حالت = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    إذا (unlikely(cap_get_capType(cb_ټاکل_خونه->cap) == cap_cb_cap)) {
        حالت = cteDelete(cb_ټاکل_خونه, true);
    }
    smmuStateSIDTable[sid] = false;
    أرجع حالت;
}

exception_t decodeARMCBControlInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                         cte_t *src_خونه, cap_t cap, bool_t call, کلمه_t *بفر)
{

    کلمه_t شاخص, ژوروالی, cb;
    cte_t *dest_خونه;
    cap_t cnodeCap;
    lookup_خونه_ret_t lu_ret;
    exception_t حالت;

    إذا (label == ARMCBTLBInvalidateAll) {
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, بفر);
    شاخص = getSyscallArg(1, بفر);
    ژوروالی = getSyscallArg(2, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_لومړی;
        userError("Rejecting request for CB %u. Already active.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_موخه_خونه(cnodeCap, شاخص, ژوروالی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(بفر, 0), (صحيح)cb);
        أرجع lu_ret.حالت;
    }
    dest_خونه = lu_ret.خونه;
    حالت = ensure_تشه_خونه(dest_خونه);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(بفر, 0), (صحيح)cb);
        أرجع حالت;
    }

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_ورننباسه(cap_cb_cap_new(SID_INVALID, cb), src_خونه, dest_خونه);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                  cte_t *src_خونه, cap_t cap, bool_t call, کلمه_t *بفر)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_خونه;
    cte_t *cb_خونه;
    exception_t حالت;
    کلمه_t cb;
    uint32_t fault_حالت_2;
    کلمه_t fault_پته;
    tcb_t *thread;

    بدل (label) {
    حالة ARMCBTLBInvalidate:
        إذا (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_خونه = smmuStateCBNode + cb;
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_خونه->cap));
        أرجع EXCEPTION_NONE;

    حالة ARMCBAssignVspace:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خونه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خونه->cap;

        إذا (unlikely(!is_VTable_ریښه(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_خونه = smmuStateCBNode + cb;
        حالت = ensure_تشه_خونه(cb_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_ټاکل_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_ورننباسه(vspaceCap, vspace_Cap_خونه, cb_خونه);
        cap_vspace_cap_ptr_سېټ_cap_VSMapped_CB(&(cb_خونه->cap), cb);
        /*set relationship between CB and ASID*/
        smmu_State_CBAsid_جدول[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        أرجع EXCEPTION_NONE;

    حالة ARMCBUnassignVspace:
        إذا (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_خونه = smmuStateCBNode + cb;
        حالت = cteDelete(cb_خونه, true);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            أرجع حالت;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;

    حالة ARMCBGetFault:
        thread = غوټه_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_حالت_2, &fault_پته);
        إذا (call) {
            کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
            سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
            سېټ_MR(thread, ipc_بفر, 0, fault_حالت_2);
            سېټ_MR(thread, ipc_بفر, 1, fault_پته);
            سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                            se_L4_پیغام_Info_new(0, 0, 0, 2)));
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;

    حالة ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;

    افتراضي:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    کلمه_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_خونه;
    exception_t حالت = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    إذا (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_خونه = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        حالت = cteDelete(cb_خونه, true);
    }
    smmuStateCBTable[cb] = false;
    أرجع حالت;
}

فراغ smmu_cb_delete_vspace(کلمه_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_جدول[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

فراغ invalidateSMMUTLBByASID(asid_t asid, کلمه_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    لكل (صحيح cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        إذا (unlikely(smmu_State_CBAsid_جدول[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

فراغ invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, کلمه_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    لكل (صحيح cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        إذا (unlikely(smmu_State_CBAsid_جدول[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#د_که_پای

