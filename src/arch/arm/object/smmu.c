/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#అన్తర్భావ <config.h>

#నిర్వచితమైతే CONFIG_ARM_SMMU
#అన్తర్భావ <arch/object/smmu.h>

స్థిర exception_t checkARMCBVspace(cap_t cap)
{
    మాట_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_స్థానం = smmuStateCBNode + cb;
    ఒకవేళ (unlikely(!is_VTable_మూలం(cb_స్థానం->cap))) {
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                          cte_t *src_స్థానం, cap_t cap, bool_t call, మాట_t *బఫరు)
{

    మాట_t సూచీ, లోతు, sid;
    cte_t *dest_స్థానం;
    cap_t cnodeCap;
    lookup_స్థానం_ret_t lu_ret;
    exception_t స్థితి_2;
    uint32_t fault_స్థితి_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    ఒకవేళ (label == ARMSIDGetFault) {
        thread = కణుపు_STATE(ksCurThread);
        smmu_read_fault_state(&fault_స్థితి_2, &faultSyndrome_0, &faultSyndrome_1);
        ఒకవేళ (call) {
            మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
            సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
            సమితి_MR(thread, ipc_బఫరు, 0, fault_స్థితి_2);
            సమితి_MR(thread, ipc_బఫరు, 1, faultSyndrome_0);
            సమితి_MR(thread, ipc_బఫరు, 2, faultSyndrome_1);
            సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                            se_L4_సందేశం_Info_new(0, 0, 0, 3)));
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, బఫరు);
    సూచీ = getSyscallArg(1, బఫరు);
    లోతు = getSyscallArg(2, బఫరు);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (పూర్ణాంకం)sid);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_మొదటి;
        userError("Rejecting request for SID %u. Already active.", (పూర్ణాంకం)sid);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)sid);
        తిరిగి lu_ret.స్థితి_2;
    }
    dest_స్థానం = lu_ret.స్థానం;
    స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)sid);
        తిరిగి స్థితి_2;
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_చొప్పించు(cap_sid_cap_new(sid), src_స్థానం, dest_స్థానం);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                   cte_t *src_స్థానం, cap_t cap, bool_t call, మాట_t *బఫరు)
{
    cap_t cbCap;
    cte_t *cb_Cap_స్థానం;
    cte_t *cb_కేటాయించు_స్థానం;
    exception_t స్థితి_2;
    మాట_t sid;

    ఎంపిక (label) {
    స్థితి ARMSIDBindCB:
        ఒకవేళ (unlikely(current_extra_caps.excaprefs[0] == ఖాళీ)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_స్థానం = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_స్థానం->cap;
        ఒకవేళ (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_కేటాయించు_స్థానం = smmuStateSIDNode + sid;
        స్థితి_2 = ensure_ఖాళీలు_స్థానం(cb_కేటాయించు_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            తిరిగి స్థితి_2;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_చొప్పించు(cbCap, cb_Cap_స్థానం, cb_కేటాయించు_స్థానం);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_సమితి_cap_Bind_SID(&(cb_కేటాయించు_స్థానం->cap), sid);
        తిరిగి EXCEPTION_NONE;

    స్థితి ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_కేటాయించు_స్థానం = smmuStateSIDNode + sid;
        ఒకవేళ (unlikely(cap_get_capType(cb_కేటాయించు_స్థానం->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        స్థితి_2 = cteDelete(cb_కేటాయించు_స్థానం, true);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            తిరిగి స్థితి_2;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి EXCEPTION_NONE;
    అప్రమేయ:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    మాట_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_కేటాయించు_స్థానం = smmuStateSIDNode + sid;
    exception_t స్థితి_2 = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    ఒకవేళ (unlikely(cap_get_capType(cb_కేటాయించు_స్థానం->cap) == cap_cb_cap)) {
        స్థితి_2 = cteDelete(cb_కేటాయించు_స్థానం, true);
    }
    smmuStateSIDTable[sid] = false;
    తిరిగి స్థితి_2;
}

exception_t decodeARMCBControlInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                         cte_t *src_స్థానం, cap_t cap, bool_t call, మాట_t *బఫరు)
{

    మాట_t సూచీ, లోతు, cb;
    cte_t *dest_స్థానం;
    cap_t cnodeCap;
    lookup_స్థానం_ret_t lu_ret;
    exception_t స్థితి_2;

    ఒకవేళ (label == ARMCBTLBInvalidateAll) {
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        తిరిగి EXCEPTION_NONE;
    }

    ఒకవేళ (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, బఫరు);
    సూచీ = getSyscallArg(1, బఫరు);
    లోతు = getSyscallArg(2, బఫరు);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (పూర్ణాంకం)cb);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_మొదటి;
        userError("Rejecting request for CB %u. Already active.", (పూర్ణాంకం)cb);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_లక్ష్యం_స్థానం(cnodeCap, సూచీ, లోతు);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)cb);
        తిరిగి lu_ret.స్థితి_2;
    }
    dest_స్థానం = lu_ret.స్థానం;
    స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(బఫరు, 0), (పూర్ణాంకం)cb);
        తిరిగి స్థితి_2;
    }

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_చొప్పించు(cap_cb_cap_new(SID_INVALID, cb), src_స్థానం, dest_స్థానం);
    తిరిగి EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                  cte_t *src_స్థానం, cap_t cap, bool_t call, మాట_t *బఫరు)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_స్థానం;
    cte_t *cb_స్థానం;
    exception_t స్థితి_2;
    మాట_t cb;
    uint32_t fault_స్థితి_2;
    మాట_t fault_చిరునామా;
    tcb_t *thread;

    ఎంపిక (label) {
    స్థితి ARMCBTLBInvalidate:
        ఒకవేళ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_స్థానం = smmuStateCBNode + cb;
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_స్థానం->cap));
        తిరిగి EXCEPTION_NONE;

    స్థితి ARMCBAssignVspace:
        ఒకవేళ (unlikely(current_extra_caps.excaprefs[0] == ఖాళీ)) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_స్థానం = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_స్థానం->cap;

        ఒకవేళ (unlikely(!is_VTable_మూలం(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_స్థానం = smmuStateCBNode + cb;
        స్థితి_2 = ensure_ఖాళీలు_స్థానం(cb_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_కేటాయించు_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_చొప్పించు(vspaceCap, vspace_Cap_స్థానం, cb_స్థానం);
        cap_vspace_cap_ptr_సమితి_cap_VSMapped_CB(&(cb_స్థానం->cap), cb);
        /*set relationship between CB and ASID*/
        smmu_State_CBAsid_పట్టిక[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        తిరిగి EXCEPTION_NONE;

    స్థితి ARMCBUnassignVspace:
        ఒకవేళ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_స్థానం = smmuStateCBNode + cb;
        స్థితి_2 = cteDelete(cb_స్థానం, true);
        ఒకవేళ (unlikely(స్థితి_2 != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            తిరిగి స్థితి_2;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి EXCEPTION_NONE;

    స్థితి ARMCBGetFault:
        thread = కణుపు_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_స్థితి_2, &fault_చిరునామా);
        ఒకవేళ (call) {
            మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
            సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
            సమితి_MR(thread, ipc_బఫరు, 0, fault_స్థితి_2);
            సమితి_MR(thread, ipc_బఫరు, 1, fault_చిరునామా);
            సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                            se_L4_సందేశం_Info_new(0, 0, 0, 2)));
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
        తిరిగి EXCEPTION_NONE;

    స్థితి ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి EXCEPTION_NONE;

    అప్రమేయ:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    మాట_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_స్థానం;
    exception_t స్థితి_2 = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    ఒకవేళ (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_స్థానం = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        స్థితి_2 = cteDelete(cb_స్థానం, true);
    }
    smmuStateCBTable[cb] = false;
    తిరిగి స్థితి_2;
}

శూన్య smmu_cb_delete_vspace(మాట_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_పట్టిక[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

శూన్య invalidateSMMUTLBByASID(asid_t asid, మాట_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    కొరకు (పూర్ణాంకం cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ఒకవేళ (unlikely(smmu_State_CBAsid_పట్టిక[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

శూన్య invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, మాట_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    కొరకు (పూర్ణాంకం cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ఒకవేళ (unlikely(smmu_State_CBAsid_పట్టిక[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#అయితే_ముగింపు

