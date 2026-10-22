/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_ARM_SMMU
#ضمن <arch/object/smmu.h>

ساكن exception_t checkARMCBVspace(cap_t cap)
{
    لفظ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_تھاں = smmuStateCBNode + cb;
    إذا (unlikely(!is_VTable_جڑ(cb_تھاں->cap))) {
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(لفظ_t label, لفظ_t لمائی, cptr_t cptr,
                                          cte_t *src_تھاں, cap_t cap, bool_t call, لفظ_t *بفر)
{

    لفظ_t اشاریہ, ڈونگھائی, sid;
    cte_t *dest_تھاں;
    cap_t cnodeCap;
    lookup_تھاں_ret_t lu_ret;
    exception_t حالت;
    uint32_t fault_حالت_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    إذا (label == ARMSIDGetFault) {
        thread = گرہ_STATE(ksCurThread);
        smmu_read_fault_state(&fault_حالت_2, &faultSyndrome_0, &faultSyndrome_1);
        إذا (call) {
            لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
            سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
            سیٹ_MR(thread, ipc_بفر, 0, fault_حالت_2);
            سیٹ_MR(thread, ipc_بفر, 1, faultSyndrome_0);
            سیٹ_MR(thread, ipc_بفر, 2, faultSyndrome_1);
            سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                            se_L4_سنیہا_Info_new(0, 0, 0, 3)));
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;
    }

    إذا (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (لمائی < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, بفر);
    اشاریہ = getSyscallArg(1, بفر);
    ڈونگھائی = getSyscallArg(2, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_پہلا;
        userError("Rejecting request for SID %u. Already active.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(بفر, 0), (صحيح)sid);
        أرجع lu_ret.حالت;
    }
    dest_تھاں = lu_ret.تھاں;
    حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(بفر, 0), (صحيح)sid);
        أرجع حالت;
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_داخل_کرو(cap_sid_cap_new(sid), src_تھاں, dest_تھاں);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(لفظ_t label, لفظ_t لمائی, cptr_t cptr,
                                   cte_t *src_تھاں, cap_t cap, bool_t call, لفظ_t *بفر)
{
    cap_t cbCap;
    cte_t *cb_Cap_تھاں;
    cte_t *cb_سونپو_تھاں;
    exception_t حالت;
    لفظ_t sid;

    بدل (label) {
    حالة ARMSIDBindCB:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_تھاں = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_تھاں->cap;
        إذا (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_سونپو_تھاں = smmuStateSIDNode + sid;
        حالت = ensure_خالی_تھاواں_تھاں(cb_سونپو_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            أرجع حالت;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_داخل_کرو(cbCap, cb_Cap_تھاں, cb_سونپو_تھاں);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_سیٹ_cap_Bind_SID(&(cb_سونپو_تھاں->cap), sid);
        أرجع EXCEPTION_NONE;

    حالة ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_سونپو_تھاں = smmuStateSIDNode + sid;
        إذا (unlikely(cap_get_capType(cb_سونپو_تھاں->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        حالت = cteDelete(cb_سونپو_تھاں, true);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            أرجع حالت;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    افتراضي:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    لفظ_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_سونپو_تھاں = smmuStateSIDNode + sid;
    exception_t حالت = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    إذا (unlikely(cap_get_capType(cb_سونپو_تھاں->cap) == cap_cb_cap)) {
        حالت = cteDelete(cb_سونپو_تھاں, true);
    }
    smmuStateSIDTable[sid] = false;
    أرجع حالت;
}

exception_t decodeARMCBControlInvocation(لفظ_t label, لفظ_t لمائی, cptr_t cptr,
                                         cte_t *src_تھاں, cap_t cap, bool_t call, لفظ_t *بفر)
{

    لفظ_t اشاریہ, ڈونگھائی, cb;
    cte_t *dest_تھاں;
    cap_t cnodeCap;
    lookup_تھاں_ret_t lu_ret;
    exception_t حالت;

    إذا (label == ARMCBTLBInvalidateAll) {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (لمائی < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, بفر);
    اشاریہ = getSyscallArg(1, بفر);
    ڈونگھائی = getSyscallArg(2, بفر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_پہلا;
        userError("Rejecting request for CB %u. Already active.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_نشانہ_تھاں(cnodeCap, اشاریہ, ڈونگھائی);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(بفر, 0), (صحيح)cb);
        أرجع lu_ret.حالت;
    }
    dest_تھاں = lu_ret.تھاں;
    حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
    إذا (حالت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(بفر, 0), (صحيح)cb);
        أرجع حالت;
    }

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_داخل_کرو(cap_cb_cap_new(SID_INVALID, cb), src_تھاں, dest_تھاں);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(لفظ_t label, لفظ_t لمائی, cptr_t cptr,
                                  cte_t *src_تھاں, cap_t cap, bool_t call, لفظ_t *بفر)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_تھاں;
    cte_t *cb_تھاں;
    exception_t حالت;
    لفظ_t cb;
    uint32_t fault_حالت_2;
    لفظ_t fault_پتہ;
    tcb_t *thread;

    بدل (label) {
    حالة ARMCBTLBInvalidate:
        إذا (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_تھاں = smmuStateCBNode + cb;
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_تھاں->cap));
        أرجع EXCEPTION_NONE;

    حالة ARMCBAssignVspace:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_تھاں = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_تھاں->cap;

        إذا (unlikely(!is_VTable_جڑ(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_تھاں = smmuStateCBNode + cb;
        حالت = ensure_خالی_تھاواں_تھاں(cb_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_سونپو_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_داخل_کرو(vspaceCap, vspace_Cap_تھاں, cb_تھاں);
        cap_vspace_cap_ptr_سیٹ_cap_VSMapped_CB(&(cb_تھاں->cap), cb);
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
        cb_تھاں = smmuStateCBNode + cb;
        حالت = cteDelete(cb_تھاں, true);
        إذا (unlikely(حالت != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            أرجع حالت;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;

    حالة ARMCBGetFault:
        thread = گرہ_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_حالت_2, &fault_پتہ);
        إذا (call) {
            لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
            سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
            سیٹ_MR(thread, ipc_بفر, 0, fault_حالت_2);
            سیٹ_MR(thread, ipc_بفر, 1, fault_پتہ);
            سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                            se_L4_سنیہا_Info_new(0, 0, 0, 2)));
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;

    حالة ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;

    افتراضي:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    لفظ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_تھاں;
    exception_t حالت = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    إذا (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_تھاں = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        حالت = cteDelete(cb_تھاں, true);
    }
    smmuStateCBTable[cb] = false;
    أرجع حالت;
}

فراغ smmu_cb_delete_vspace(لفظ_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_جدول[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

فراغ invalidateSMMUTLBByASID(asid_t asid, لفظ_t bind_cb)
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

فراغ invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, لفظ_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    لكل (صحيح cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        إذا (unlikely(smmu_State_CBAsid_جدول[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#جے_انت

