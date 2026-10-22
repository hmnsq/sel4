/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#شامل <config.h>

#ifdef CONFIG_ARM_SMMU
#شامل <arch/object/smmu.h>

ایستا exception_t checkARMCBVspace(cap_t cap)
{
    word_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cbSlot = smmuStateCBNode + cb;
    اگر (unlikely(!is_VTable_ریشه(cbSlot->cap))) {
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    بازگشت EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(word_t label, word_t length, cptr_t cptr,
                                          cte_t *srcSlot, cap_t cap, bool_t call, word_t *buffer)
{

    word_t index, depth, sid;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;
    uint32_t faultStatus, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    اگر (label == ARMSIDGetFault) {
        thread = گره_STATE(ksCurThread);
        smmu_read_fault_state(&faultStatus, &faultSyndrome_0, &faultSyndrome_1);
        اگر (call) {
            word_t *ipcBuffer = lookupIPCBuffer(true, thread);
            setRegister(thread, badgeRegister, 0);
            setMR(thread, ipcBuffer, 0, faultStatus);
            setMR(thread, ipcBuffer, 1, faultSyndrome_0);
            setMR(thread, ipcBuffer, 2, faultSyndrome_1);
            setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                            seL4_MessageInfo_new(0, 0, 0, 3)));
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
        بازگشت EXCEPTION_NONE;
    }

    اگر (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت EXCEPTION_NONE;
    }

    اگر (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (length < 3 || current_extra_caps.excaprefs[0] == پوچ) {
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, buffer);
    index = getSyscallArg(1, buffer);
    depth = getSyscallArg(2, buffer);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    اگر (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (صحیح)sid);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (smmuStateSIDTable[sid]) {
        current_syscall_error.type = seL4_RevokeFirst;
        userError("Rejecting request for SID %u. Already active.", (صحیح)sid);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    اگر (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(buffer, 0), (صحیح)sid);
        بازگشت lu_ret.status;
    }
    destSlot = lu_ret.slot;
    status = ensureEmptySlot(destSlot);
    اگر (status != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(buffer, 0), (صحیح)sid);
        بازگشت status;
    }
    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_درج(cap_sid_cap_new(sid), srcSlot, destSlot);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(word_t label, word_t length, cptr_t cptr,
                                   cte_t *srcSlot, cap_t cap, bool_t call, word_t *buffer)
{
    cap_t cbCap;
    cte_t *cbCapSlot;
    cte_t *cbAssignSlot;
    exception_t status;
    word_t sid;

    انتخاب (label) {
    حالت ARMSIDBindCB:
        اگر (unlikely(current_extra_caps.excaprefs[0] == پوچ)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        cbCapSlot = current_extra_caps.excaprefs[0];
        cbCap = cbCapSlot->cap;
        اگر (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        اگر (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cbAssignSlot = smmuStateSIDNode + sid;
        status = ensureEmptySlot(cbAssignSlot);
        اگر (status != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            بازگشت status;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_درج(cbCap, cbCapSlot, cbAssignSlot);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_set_capBindSID(&(cbAssignSlot->cap), sid);
        بازگشت EXCEPTION_NONE;

    حالت ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cbAssignSlot = smmuStateSIDNode + sid;
        اگر (unlikely(cap_get_capType(cbAssignSlot->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        status = cteDelete(cbAssignSlot, true);
        اگر (unlikely(status != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            بازگشت status;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت EXCEPTION_NONE;
    پیشفرض:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    word_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cbAssignSlot = smmuStateSIDNode + sid;
    exception_t status = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    اگر (unlikely(cap_get_capType(cbAssignSlot->cap) == cap_cb_cap)) {
        status = cteDelete(cbAssignSlot, true);
    }
    smmuStateSIDTable[sid] = false;
    بازگشت status;
}

exception_t decodeARMCBControlInvocation(word_t label, word_t length, cptr_t cptr,
                                         cte_t *srcSlot, cap_t cap, bool_t call, word_t *buffer)
{

    word_t index, depth, cb;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t status;

    اگر (label == ARMCBTLBInvalidateAll) {
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        بازگشت EXCEPTION_NONE;
    }

    اگر (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (length < 3 || current_extra_caps.excaprefs[0] == پوچ) {
        current_syscall_error.type = seL4_TruncatedMessage;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, buffer);
    index = getSyscallArg(1, buffer);
    depth = getSyscallArg(2, buffer);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    اگر (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (صحیح)cb);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
    اگر (smmuStateCBTable[cb]) {
        current_syscall_error.type = seL4_RevokeFirst;
        userError("Rejecting request for CB %u. Already active.", (صحیح)cb);
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, index, depth);
    اگر (lu_ret.status != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(buffer, 0), (صحیح)cb);
        بازگشت lu_ret.status;
    }
    destSlot = lu_ret.slot;
    status = ensureEmptySlot(destSlot);
    اگر (status != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(buffer, 0), (صحیح)cb);
        بازگشت status;
    }

    setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_درج(cap_cb_cap_new(SID_INVALID, cb), srcSlot, destSlot);
    بازگشت EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(word_t label, word_t length, cptr_t cptr,
                                  cte_t *srcSlot, cap_t cap, bool_t call, word_t *buffer)
{

    cap_t vspaceCap;
    cte_t *vspaceCapSlot;
    cte_t *cbSlot;
    exception_t status;
    word_t cb;
    uint32_t faultStatus;
    word_t faultAddress;
    tcb_t *thread;

    انتخاب (label) {
    حالت ARMCBTLBInvalidate:
        اگر (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cbSlot->cap));
        بازگشت EXCEPTION_NONE;

    حالت ARMCBAssignVspace:
        اگر (unlikely(current_extra_caps.excaprefs[0] == پوچ)) {
            current_syscall_error.type = seL4_TruncatedMessage;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        اگر (unlikely(!is_VTable_ریشه(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        status = ensureEmptySlot(cbSlot);
        اگر (status != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            بازگشت status;
        }

        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_assign_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_درج(vspaceCap, vspaceCapSlot, cbSlot);
        cap_vspace_cap_ptr_set_capVSMappedCB(&(cbSlot->cap), cb);
        /*set relationship between CB and ASID*/
        smmuStateCBAsidTable[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        بازگشت EXCEPTION_NONE;

    حالت ARMCBUnassignVspace:
        اگر (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            بازگشت EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        status = cteDelete(cbSlot, true);
        اگر (unlikely(status != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            بازگشت status;
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت EXCEPTION_NONE;

    حالت ARMCBGetFault:
        thread = گره_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &faultStatus, &faultAddress);
        اگر (call) {
            word_t *ipcBuffer = lookupIPCBuffer(true, thread);
            setRegister(thread, badgeRegister, 0);
            setMR(thread, ipcBuffer, 0, faultStatus);
            setMR(thread, ipcBuffer, 1, faultAddress);
            setRegister(thread, پیام_Info_Register, wordFromMessageInfo(
                            seL4_MessageInfo_new(0, 0, 0, 2)));
        }
        setThreadState(گره_STATE(ksCurThread), ThreadState_Running);
        بازگشت EXCEPTION_NONE;

    حالت ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        setThreadState(گره_STATE(ksCurThread), ThreadState_Restart);
        بازگشت EXCEPTION_NONE;

    پیشفرض:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        بازگشت EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    word_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cbSlot;
    exception_t status = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    اگر (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cbSlot = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        status = cteDelete(cbSlot, true);
    }
    smmuStateCBTable[cb] = false;
    بازگشت status;
}

تهی smmu_cb_delete_vspace(word_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmuStateCBAsidTable[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

تهی invalidateSMMUTLBByASID(asid_t asid, word_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    برای (صحیح cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        اگر (unlikely(smmuStateCBAsidTable[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

تهی invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, word_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    برای (صحیح cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        اگر (unlikely(smmuStateCBAsidTable[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#endif

