/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ضمن <config.h>

#اگر_تعریف_شده CONFIG_ARM_SMMU
#ضمن <arch/object/smmu.h>

ساكن exception_t checkARMCBVspace(cap_t cap)
{
    کلمه_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_جایگاه = smmuStateCBNode + cb;
    إذا (unlikely(!is_VTable_ریشه(cb_جایگاه->cap))) {
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(کلمه_t label, کلمه_t درازی, cptr_t cptr,
                                          cte_t *src_جایگاه, cap_t cap, bool_t call, کلمه_t *میانگیر)
{

    کلمه_t شاخص, عمق, sid;
    cte_t *dest_جایگاه;
    cap_t cnodeCap;
    lookup_جایگاه_ret_t lu_ret;
    exception_t وضعیت;
    uint32_t fault_وضعیت_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    إذا (label == ARMSIDGetFault) {
        thread = گره_STATE(ksCurThread);
        smmu_read_fault_state(&fault_وضعیت_2, &faultSyndrome_0, &faultSyndrome_1);
        إذا (call) {
            کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
            مجموعه_ثبات(thread, badge_ثبات, 0);
            مجموعه_MR(thread, ipc_میانگیر, 0, fault_وضعیت_2);
            مجموعه_MR(thread, ipc_میانگیر, 1, faultSyndrome_0);
            مجموعه_MR(thread, ipc_میانگیر, 2, faultSyndrome_1);
            مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                            se_L4_پیام_Info_new(0, 0, 0, 3)));
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;
    }

    إذا (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (درازی < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, میانگیر);
    شاخص = getSyscallArg(1, میانگیر);
    عمق = getSyscallArg(2, میانگیر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_اولی;
        userError("Rejecting request for SID %u. Already active.", (صحيح)sid);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(میانگیر, 0), (صحيح)sid);
        أرجع lu_ret.وضعیت;
    }
    dest_جایگاه = lu_ret.جایگاه;
    وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(میانگیر, 0), (صحيح)sid);
        أرجع وضعیت;
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_درج(cap_sid_cap_new(sid), src_جایگاه, dest_جایگاه);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(کلمه_t label, کلمه_t درازی, cptr_t cptr,
                                   cte_t *src_جایگاه, cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    cap_t cbCap;
    cte_t *cb_Cap_جایگاه;
    cte_t *cb_تخصیص_بده_جایگاه;
    exception_t وضعیت;
    کلمه_t sid;

    بدل (label) {
    حالة ARMSIDBindCB:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_جایگاه = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_جایگاه->cap;
        إذا (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_تخصیص_بده_جایگاه = smmuStateSIDNode + sid;
        وضعیت = ensure_خالی_جایگاه(cb_تخصیص_بده_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            أرجع وضعیت;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_درج(cbCap, cb_Cap_جایگاه, cb_تخصیص_بده_جایگاه);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_مجموعه_cap_Bind_SID(&(cb_تخصیص_بده_جایگاه->cap), sid);
        أرجع EXCEPTION_NONE;

    حالة ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_تخصیص_بده_جایگاه = smmuStateSIDNode + sid;
        إذا (unlikely(cap_get_capType(cb_تخصیص_بده_جایگاه->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        وضعیت = cteDelete(cb_تخصیص_بده_جایگاه, true);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            أرجع وضعیت;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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
    cte_t *cb_تخصیص_بده_جایگاه = smmuStateSIDNode + sid;
    exception_t وضعیت = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    إذا (unlikely(cap_get_capType(cb_تخصیص_بده_جایگاه->cap) == cap_cb_cap)) {
        وضعیت = cteDelete(cb_تخصیص_بده_جایگاه, true);
    }
    smmuStateSIDTable[sid] = false;
    أرجع وضعیت;
}

exception_t decodeARMCBControlInvocation(کلمه_t label, کلمه_t درازی, cptr_t cptr,
                                         cte_t *src_جایگاه, cap_t cap, bool_t call, کلمه_t *میانگیر)
{

    کلمه_t شاخص, عمق, cb;
    cte_t *dest_جایگاه;
    cap_t cnodeCap;
    lookup_جایگاه_ret_t lu_ret;
    exception_t وضعیت;

    إذا (label == ARMCBTLBInvalidateAll) {
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        أرجع EXCEPTION_NONE;
    }

    إذا (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (درازی < 3 || current_extra_caps.excaprefs[0] == عدم) {
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, میانگیر);
    شاخص = getSyscallArg(1, میانگیر);
    عمق = getSyscallArg(2, میانگیر);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    إذا (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_اولی;
        userError("Rejecting request for CB %u. Already active.", (صحيح)cb);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_هدف_جایگاه(cnodeCap, شاخص, عمق);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(میانگیر, 0), (صحيح)cb);
        أرجع lu_ret.وضعیت;
    }
    dest_جایگاه = lu_ret.جایگاه;
    وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
    إذا (وضعیت != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(میانگیر, 0), (صحيح)cb);
        أرجع وضعیت;
    }

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_درج(cap_cb_cap_new(SID_INVALID, cb), src_جایگاه, dest_جایگاه);
    أرجع EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(کلمه_t label, کلمه_t درازی, cptr_t cptr,
                                  cte_t *src_جایگاه, cap_t cap, bool_t call, کلمه_t *میانگیر)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_جایگاه;
    cte_t *cb_جایگاه;
    exception_t وضعیت;
    کلمه_t cb;
    uint32_t fault_وضعیت_2;
    کلمه_t fault_آدرس;
    tcb_t *thread;

    بدل (label) {
    حالة ARMCBTLBInvalidate:
        إذا (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_جایگاه = smmuStateCBNode + cb;
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_جایگاه->cap));
        أرجع EXCEPTION_NONE;

    حالة ARMCBAssignVspace:
        إذا (unlikely(current_extra_caps.excaprefs[0] == عدم)) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جایگاه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جایگاه->cap;

        إذا (unlikely(!is_VTable_ریشه(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_جایگاه = smmuStateCBNode + cb;
        وضعیت = ensure_خالی_جایگاه(cb_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_تخصیص_بده_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_درج(vspaceCap, vspace_Cap_جایگاه, cb_جایگاه);
        cap_vspace_cap_ptr_مجموعه_cap_VSMapped_CB(&(cb_جایگاه->cap), cb);
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
        cb_جایگاه = smmuStateCBNode + cb;
        وضعیت = cteDelete(cb_جایگاه, true);
        إذا (unlikely(وضعیت != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            أرجع وضعیت;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع EXCEPTION_NONE;

    حالة ARMCBGetFault:
        thread = گره_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_وضعیت_2, &fault_آدرس);
        إذا (call) {
            کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
            مجموعه_ثبات(thread, badge_ثبات, 0);
            مجموعه_MR(thread, ipc_میانگیر, 0, fault_وضعیت_2);
            مجموعه_MR(thread, ipc_میانگیر, 1, fault_آدرس);
            مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                            se_L4_پیام_Info_new(0, 0, 0, 2)));
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
        أرجع EXCEPTION_NONE;

    حالة ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
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
    cte_t *cb_جایگاه;
    exception_t وضعیت = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    إذا (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_جایگاه = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        وضعیت = cteDelete(cb_جایگاه, true);
    }
    smmuStateCBTable[cb] = false;
    أرجع وضعیت;
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

#پایان_اگر

