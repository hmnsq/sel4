/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#अन्तर्भाव <config.h>

#यदि_परिभाषितम् CONFIG_ARM_SMMU
#अन्तर्भाव <arch/object/smmu.h>

स्थिर exception_t checkARMCBVspace(cap_t cap)
{
    word_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cbSlot = smmuStateCBNode + cb;
    यदि (unlikely(!is_VTable_मूलम्(cbSlot->cap))) {
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                          cte_t *srcSlot, cap_t cap, bool_t call, word_t *बफर)
{

    word_t सूचकांक, गभीरता, sid;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t स्थितिः;
    uint32_t fault_स्थितिः_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    यदि (label == ARMSIDGetFault) {
        thread = ग्रन्थिः_STATE(ksCurThread);
        smmu_read_fault_state(&fault_स्थितिः_2, &faultSyndrome_0, &faultSyndrome_1);
        यदि (call) {
            word_t *ipc_बफर = lookupIPCBuffer(true, thread);
            समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
            समुच्चयः_MR(thread, ipc_बफर, 0, fault_स्थितिः_2);
            समुच्चयः_MR(thread, ipc_बफर, 1, faultSyndrome_0);
            समुच्चयः_MR(thread, ipc_बफर, 2, faultSyndrome_1);
            समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                            se_L4_सन्देशः_Info_new(0, 0, 0, 3)));
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (दीर्घता < 3 || current_extra_caps.excaprefs[0] == रिक्त) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, बफर);
    सूचकांक = getSyscallArg(1, बफर);
    गभीरता = getSyscallArg(2, बफर);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    यदि (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (पूर्णाङ्क)sid);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (smmuStateSIDTable[sid]) {
        current_syscall_error.type = seL4_RevokeFirst;
        userError("Rejecting request for SID %u. Already active.", (पूर्णाङ्क)sid);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(बफर, 0), (पूर्णाङ्क)sid);
        प्रत्यागम lu_ret.स्थितिः;
    }
    destSlot = lu_ret.slot;
    स्थितिः = ensure_रिक्तानि_Slot(destSlot);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(बफर, 0), (पूर्णाङ्क)sid);
        प्रत्यागम स्थितिः;
    }
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_निवेशय(cap_sid_cap_new(sid), srcSlot, destSlot);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                   cte_t *srcSlot, cap_t cap, bool_t call, word_t *बफर)
{
    cap_t cbCap;
    cte_t *cbCapSlot;
    cte_t *cb_नियोजय_Slot;
    exception_t स्थितिः;
    word_t sid;

    विकल्प (label) {
    पक्ष ARMSIDBindCB:
        यदि (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        cbCapSlot = current_extra_caps.excaprefs[0];
        cbCap = cbCapSlot->cap;
        यदि (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        यदि (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_नियोजय_Slot = smmuStateSIDNode + sid;
        स्थितिः = ensure_रिक्तानि_Slot(cb_नियोजय_Slot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            प्रत्यागम स्थितिः;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_निवेशय(cbCap, cbCapSlot, cb_नियोजय_Slot);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_समुच्चयः_cap_Bind_SID(&(cb_नियोजय_Slot->cap), sid);
        प्रत्यागम EXCEPTION_NONE;

    पक्ष ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_नियोजय_Slot = smmuStateSIDNode + sid;
        यदि (unlikely(cap_get_capType(cb_नियोजय_Slot->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        स्थितिः = cteDelete(cb_नियोजय_Slot, true);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            प्रत्यागम स्थितिः;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम EXCEPTION_NONE;
    स्वभाव:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    word_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_नियोजय_Slot = smmuStateSIDNode + sid;
    exception_t स्थितिः = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    यदि (unlikely(cap_get_capType(cb_नियोजय_Slot->cap) == cap_cb_cap)) {
        स्थितिः = cteDelete(cb_नियोजय_Slot, true);
    }
    smmuStateSIDTable[sid] = false;
    प्रत्यागम स्थितिः;
}

exception_t decodeARMCBControlInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                         cte_t *srcSlot, cap_t cap, bool_t call, word_t *बफर)
{

    word_t सूचकांक, गभीरता, cb;
    cte_t *destSlot;
    cap_t cnodeCap;
    lookupSlot_ret_t lu_ret;
    exception_t स्थितिः;

    यदि (label == ARMCBTLBInvalidateAll) {
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        प्रत्यागम EXCEPTION_NONE;
    }

    यदि (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (दीर्घता < 3 || current_extra_caps.excaprefs[0] == रिक्त) {
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, बफर);
    सूचकांक = getSyscallArg(1, बफर);
    गभीरता = getSyscallArg(2, बफर);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    यदि (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (पूर्णाङ्क)cb);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (smmuStateCBTable[cb]) {
        current_syscall_error.type = seL4_RevokeFirst;
        userError("Rejecting request for CB %u. Already active.", (पूर्णाङ्क)cb);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookupTargetSlot(cnodeCap, सूचकांक, गभीरता);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(बफर, 0), (पूर्णाङ्क)cb);
        प्रत्यागम lu_ret.स्थितिः;
    }
    destSlot = lu_ret.slot;
    स्थितिः = ensure_रिक्तानि_Slot(destSlot);
    यदि (स्थितिः != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(बफर, 0), (पूर्णाङ्क)cb);
        प्रत्यागम स्थितिः;
    }

    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_निवेशय(cap_cb_cap_new(SID_INVALID, cb), srcSlot, destSlot);
    प्रत्यागम EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                  cte_t *srcSlot, cap_t cap, bool_t call, word_t *बफर)
{

    cap_t vspaceCap;
    cte_t *vspaceCapSlot;
    cte_t *cbSlot;
    exception_t स्थितिः;
    word_t cb;
    uint32_t fault_स्थितिः_2;
    word_t fault_स्थानाङ्कः;
    tcb_t *thread;

    विकल्प (label) {
    पक्ष ARMCBTLBInvalidate:
        यदि (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cbSlot->cap));
        प्रत्यागम EXCEPTION_NONE;

    पक्ष ARMCBAssignVspace:
        यदि (unlikely(current_extra_caps.excaprefs[0] == रिक्त)) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        यदि (unlikely(!is_VTable_मूलम्(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_सङ्ख्या = 1;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        स्थितिः = ensure_रिक्तानि_Slot(cbSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_नियोजय_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_निवेशय(vspaceCap, vspaceCapSlot, cbSlot);
        cap_vspace_cap_ptr_समुच्चयः_cap_VSMapped_CB(&(cbSlot->cap), cb);
        /*set relationship between CB and ASID*/
        smmu_State_CBAsid_सारणी[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        प्रत्यागम EXCEPTION_NONE;

    पक्ष ARMCBUnassignVspace:
        यदि (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cbSlot = smmuStateCBNode + cb;
        स्थितिः = cteDelete(cbSlot, true);
        यदि (unlikely(स्थितिः != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            प्रत्यागम स्थितिः;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम EXCEPTION_NONE;

    पक्ष ARMCBGetFault:
        thread = ग्रन्थिः_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_स्थितिः_2, &fault_स्थानाङ्कः);
        यदि (call) {
            word_t *ipc_बफर = lookupIPCBuffer(true, thread);
            समुच्चयः_पञ्जिकाकोशः(thread, badge_पञ्जिकाकोशः, 0);
            समुच्चयः_MR(thread, ipc_बफर, 0, fault_स्थितिः_2);
            समुच्चयः_MR(thread, ipc_बफर, 1, fault_स्थानाङ्कः);
            समुच्चयः_पञ्जिकाकोशः(thread, सन्देशः_Info_पञ्जिकाकोशः, word_From_सन्देशः_Info(
                            se_L4_सन्देशः_Info_new(0, 0, 0, 2)));
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Running);
        प्रत्यागम EXCEPTION_NONE;

    पक्ष ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम EXCEPTION_NONE;

    स्वभाव:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    word_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cbSlot;
    exception_t स्थितिः = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    यदि (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cbSlot = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        स्थितिः = cteDelete(cbSlot, true);
    }
    smmuStateCBTable[cb] = false;
    प्रत्यागम स्थितिः;
}

शून्य smmu_cb_delete_vspace(word_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_सारणी[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

शून्य invalidateSMMUTLBByASID(asid_t asid, word_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    प्रति (पूर्णाङ्क cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        यदि (unlikely(smmu_State_CBAsid_सारणी[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

शून्य invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, word_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    प्रति (पूर्णाङ्क cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        यदि (unlikely(smmu_State_CBAsid_सारणी[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#यदि_अन्तः

