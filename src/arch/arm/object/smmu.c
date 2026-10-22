/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */
#ಅನ್ತರ್ಭಾವ <config.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
#ಅನ್ತರ್ಭಾವ <arch/object/smmu.h>

ಸ್ಥಿರ exception_t checkARMCBVspace(cap_t cap)
{
    ಶಬ್ದ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_ಸ್ಥಾನ = smmuStateCBNode + cb;
    ಆದರೆ (unlikely(!is_VTable_ಬೇರು(cb_ಸ್ಥಾನ->cap))) {
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMSIDControlInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                          cte_t *src_ಸ್ಥಾನ, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{

    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ, sid;
    cte_t *dest_ಸ್ಥಾನ;
    cap_t cnodeCap;
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    exception_t ಸ್ಥಿತಿ_2;
    uint32_t fault_ಸ್ಥಿತಿ_2, faultSyndrome_0, faultSyndrome_1;
    tcb_t *thread;

    ಆದರೆ (label == ARMSIDGetFault) {
        thread = ಗಂಟು_STATE(ksCurThread);
        smmu_read_fault_state(&fault_ಸ್ಥಿತಿ_2, &faultSyndrome_0, &faultSyndrome_1);
        ಆದರೆ (call) {
            ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
            ಗಣ_MR(thread, ipc_ಬಫರ್, 0, fault_ಸ್ಥಿತಿ_2);
            ಗಣ_MR(thread, ipc_ಬಫರ್, 1, faultSyndrome_0);
            ಗಣ_MR(thread, ipc_ಬಫರ್, 2, faultSyndrome_1);
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                            se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, 3)));
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (label == ARMSIDClearFault) {
        smmu_clear_fault_state();
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (label != ARMSIDIssueSIDManager) {
        userError("SIDControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    sid = getSyscallArg(0, ಬಫರ್);
    ಸೂಚ್ಯಂಕ = getSyscallArg(1, ಬಫರ್);
    ಆಳ = getSyscallArg(2, ಬಫರ್);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (sid >= SMMU_MAX_SID) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_SID - 1;
        userError("Rejecting request for SID %u. SID is greater than or equal to SMMU_MAX_SID.", (ಪೂರ್ಣಾಂಕ)sid);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (smmuStateSIDTable[sid]) {
        current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
        userError("Rejecting request for SID %u. Already active.", (ಪೂರ್ಣಾಂಕ)sid);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap invalid: cap %lu, SID %u.",
                  getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)sid);
        ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
    }
    dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;
    ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new SID Handler cap not empty: cap %lu, SID %u.",
                  getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)sid);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    smmuStateSIDTable[sid] = true;
    cte_ಸೇರಿಸು(cap_sid_cap_new(sid), src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMSIDInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                   cte_t *src_ಸ್ಥಾನ, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    cap_t cbCap;
    cte_t *cb_Cap_ಸ್ಥಾನ;
    cte_t *cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ;
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t sid;

    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ ARMSIDBindCB:
        ಆದರೆ (unlikely(current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        cb_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        cbCap = cb_Cap_ಸ್ಥಾನ->cap;
        ಆದರೆ (unlikely(cap_get_capType(cbCap) != cap_cb_cap)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (unlikely(checkARMCBVspace(cbCap) != EXCEPTION_NONE)) {
            userError("ARMSIDBindCB: Invalid CB cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        sid = cap_sid_cap_get_capSID(cap);
        cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ = smmuStateSIDNode + sid;
        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("ARMSIDBindCB: The SID is already bound with a context bank.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        /*binding the sid to cb in SMMU*/
        smmu_sid_bind_cb(sid, cap_cb_cap_get_capCB(cbCap));
        /* Building the connection between SID and CB caps by placing a
         * copy of the given cb cap in sid's cnode*/
        cte_ಸೇರಿಸು(cbCap, cb_Cap_ಸ್ಥಾನ, cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ);
        /* Recording the SID number in the copied CB cap.
         * Deleting the copied CB cap will trigger unbinding
         * operations. As a CB can be used (bound)
         * by multiple SID caps, each copied CB caps resulted from
         * binding operations keeps track of its serving SID numbers.*/
        cap_cb_cap_ptr_ಗಣ_cap_Bind_SID(&(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ->cap), sid);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ ARMSIDUnbindCB:
        sid = cap_sid_cap_get_capSID(cap);
        cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ = smmuStateSIDNode + sid;
        ಆದರೆ (unlikely(cap_get_capType(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ->cap) != cap_cb_cap)) {
            userError("ARMSIDUnbindCB: The SID is not assigned with a context bank.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಸ್ಥಿತಿ_2 = cteDelete(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ, true);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ARMSIDUnbindCB: the Assigned context bank cannot be unassigned.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("ARMSID: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

exception_t smmu_delete_sid(cap_t cap)
{
    ಶಬ್ದ_t sid = cap_sid_cap_get_capSID(cap);
    cte_t *cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ = smmuStateSIDNode + sid;
    exception_t ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    /*deleting the assigned context bank cap if exsits*/
    ಆದರೆ (unlikely(cap_get_capType(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ->cap) == cap_cb_cap)) {
        ಸ್ಥಿತಿ_2 = cteDelete(cb_ನಿಯೋಜಿಸು_ಸ್ಥಾನ, true);
    }
    smmuStateSIDTable[sid] = false;
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

exception_t decodeARMCBControlInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                         cte_t *src_ಸ್ಥಾನ, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{

    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, ಆಳ, cb;
    cte_t *dest_ಸ್ಥಾನ;
    cap_t cnodeCap;
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    exception_t ಸ್ಥಿತಿ_2;

    ಆದರೆ (label == ARMCBTLBInvalidateAll) {
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_all();
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    ಆದರೆ (label != ARMCBIssueCBManager) {
        userError("ARMCBControl: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    cb = getSyscallArg(0, ಬಫರ್);
    ಸೂಚ್ಯಂಕ = getSyscallArg(1, ಬಫರ್);
    ಆಳ = getSyscallArg(2, ಬಫರ್);
    cnodeCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (cb >= SMMU_MAX_CB) {
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = SMMU_MAX_CB - 1;
        userError("Rejecting request for CB %u. CB is greater than or equal to SMMU_MAX_CB.", (ಪೂರ್ಣಾಂಕ)cb);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (smmuStateCBTable[cb]) {
        current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
        userError("Rejecting request for CB %u. Already active.", (ಪೂರ್ಣಾಂಕ)cb);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnodeCap, ಸೂಚ್ಯಂಕ, ಆಳ);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap invalid: cap %lu, CB %u.",
                  getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)cb);
        ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
    }
    dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;
    ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("Target slot for new CB Handler cap not empty: cap %lu, CB %u.",
                  getExtraCPtr(ಬಫರ್, 0), (ಪೂರ್ಣಾಂಕ)cb);
        ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    }

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    smmuStateCBTable[cb] = true;
    cte_ಸೇರಿಸು(cap_cb_cap_new(SID_INVALID, cb), src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t decodeARMCBInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                  cte_t *src_ಸ್ಥಾನ, cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{

    cap_t vspaceCap;
    cte_t *vspace_Cap_ಸ್ಥಾನ;
    cte_t *cb_ಸ್ಥಾನ;
    exception_t ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t cb;
    uint32_t fault_ಸ್ಥಿತಿ_2;
    ಶಬ್ದ_t fault_ವಿಳಾಸ;
    tcb_t *thread;

    ಆಯ್ಕೆ (label) {
    ಸ್ಥಿತಿ ARMCBTLBInvalidate:
        ಆದರೆ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBTLBInvalidate: the CB does not have a vspace root.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_ಸ್ಥಾನ = smmuStateCBNode + cb;
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        smmu_tlb_invalidate_cb(cb, cap_vspace_cap_get_capVSMappedASID(cb_ಸ್ಥಾನ->cap));
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ ARMCBAssignVspace:
        ಆದರೆ (unlikely(current_extra_caps.excaprefs[0] == ಖಾಲಿ)) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ಸ್ಥಾನ->cap;

        ಆದರೆ (unlikely(!is_VTable_ಬೇರು(vspaceCap) || !cap_vspace_cap_get_capVSIsMapped(vspaceCap))) {
            userError("ARMCBAssignVspace: the vspace is invalid");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /*the cb number must be valid as it is created via the ARMCBIssueCBManager*/
        cb = cap_cb_cap_get_capCB(cap);
        cb_ಸ್ಥಾನ = smmuStateCBNode + cb;
        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(cb_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("ARMCBAssignVspace: the CB already assigned with a vspace root.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        /*setting up vspace for the context bank in SMMU*/
        smmu_cb_ನಿಯೋಜಿಸು_vspace(cb, VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(vspaceCap)),
                              cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        /*Connecting vspace cap to context bank*/
        cte_ಸೇರಿಸು(vspaceCap, vspace_Cap_ಸ್ಥಾನ, cb_ಸ್ಥಾನ);
        cap_vspace_cap_ptr_ಗಣ_cap_VSMapped_CB(&(cb_ಸ್ಥಾನ->cap), cb);
        /*set relationship between CB and ASID*/
        smmu_State_CBAsid_ಕೋಷ್ಟಕ[cb] = cap_vspace_cap_get_capVSMappedASID(vspaceCap);
        increaseASIDBindCB(cap_vspace_cap_get_capVSMappedASID(vspaceCap));
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ ARMCBUnassignVspace:
        ಆದರೆ (unlikely(checkARMCBVspace(cap) != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the CB does not have an assigned VSpace.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        cb = cap_cb_cap_get_capCB(cap);
        cb_ಸ್ಥಾನ = smmuStateCBNode + cb;
        ಸ್ಥಿತಿ_2 = cteDelete(cb_ಸ್ಥಾನ, true);
        ಆದರೆ (unlikely(ಸ್ಥಿತಿ_2 != EXCEPTION_NONE)) {
            userError("ARMCBUnassignVspace: the Assigned VSpace cannot be deleted.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ ARMCBGetFault:
        thread = ಗಂಟು_STATE(ksCurThread);
        smmu_cb_read_fault_state(cap_cb_cap_get_capCB(cap), &fault_ಸ್ಥಿತಿ_2, &fault_ವಿಳಾಸ);
        ಆದರೆ (call) {
            ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
            ಗಣ_MR(thread, ipc_ಬಫರ್, 0, fault_ಸ್ಥಿತಿ_2);
            ಗಣ_MR(thread, ipc_ಬಫರ್, 1, fault_ವಿಳಾಸ);
            ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                            se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, 2)));
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಸ್ಥಿತಿ ARMCBClearFault:
        smmu_cb_clear_fault_state(cap_cb_cap_get_capCB(cap));
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;

    ಪೂರ್ವನಿಯೋಜಿತ:
        userError("ARMCBInvocation: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}


exception_t smmu_delete_cb(cap_t cap)
{
    ಶಬ್ದ_t cb = cap_cb_cap_get_capCB(cap);
    cte_t *cb_ಸ್ಥಾನ;
    exception_t ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    /*deleting assigned vspace root if exists*/
    ಆದರೆ (unlikely(checkARMCBVspace(cap) == EXCEPTION_NONE)) {
        cb_ಸ್ಥಾನ = smmuStateCBNode + cb;
        /*the relationship between CB and ASID is reset at the vspace deletion
        triggered by the cteDelete*/
        ಸ್ಥಿತಿ_2 = cteDelete(cb_ಸ್ಥಾನ, true);
    }
    smmuStateCBTable[cb] = false;
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

ಶೂನ್ಯ smmu_cb_delete_vspace(ಶಬ್ದ_t cb, asid_t asid)
{
    /* Deleting the vspace cap stored in context bank's CNode, causing:
     * -reset the relationship between context bank and vspace's ASID
     * -disable the context bank as its vspace no longer exists*/
    smmu_State_CBAsid_ಕೋಷ್ಟಕ[cb] = ASID_INVALID;
    decreaseASIDBindCB(asid);
    smmu_cb_disable(cb, asid);
}

ಶೂನ್ಯ invalidateSMMUTLBByASID(asid_t asid, ಶಬ್ದ_t bind_cb)
{
    /* Due to the requirement of one vspace (ASID) can be shared by
     * multiple threads and drivers, there is no obvious way to
     * directly locate all context banks associated with a given ASID without a
     * search. Another possible solution is representing all context banks in
     * bitmaps, which also requires a search. This operation can only be triggered
     * by ASID invalidation or similar operations, hence the performance is not a major issue.*/
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ಆದರೆ (unlikely(smmu_State_CBAsid_ಕೋಷ್ಟಕ[cb] == asid)) {
            smmu_tlb_invalidate_cb(cb, asid);
            bind_cb--;
        }
    }
}

ಶೂನ್ಯ invalidateSMMUTLBByASIDVA(asid_t asid, vptr_t vaddr, ಶಬ್ದ_t bind_cb)
{
    /* Implemented in the same way as invalidateSMMUTLBByASID */
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ cb = 0; cb < SMMU_MAX_CB && bind_cb; cb++) {
        ಆದರೆ (unlikely(smmu_State_CBAsid_ಕೋಷ್ಟಕ[cb] == asid)) {
            smmu_tlb_invalidate_cb_va(cb, asid, vaddr);
            bind_cb--;
        }
    }
}

#ಆದರೆ_ಅಂತ್ಯ

