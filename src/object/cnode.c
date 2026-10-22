/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/invocation.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <api/types.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <object/interrupt.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <model/preemption.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <util.h>

ರಚನೆ finalise_ಸ್ಥಾನ_ret {
    exception_t ಸ್ಥಿತಿ_2;
    bool_t success;
    cap_t cleanupInfo;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ finalise_ಸ್ಥಾನ_ret finalise_ಸ್ಥಾನ_ret_t;

ಸ್ಥಿರ finalise_ಸ್ಥಾನ_ret_t finalise_ಸ್ಥಾನ(cte_t *ಸ್ಥಾನ, bool_t exposed);
ಸ್ಥಿರ ಶೂನ್ಯ ಖಾಲಿಗಳು_ಸ್ಥಾನ(cte_t *ಸ್ಥಾನ, cap_t cleanupInfo);
ಸ್ಥಿರ exception_t reduceZombie(cte_t *ಸ್ಥಾನ, bool_t exposed);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
#ವ್ಯಾಖ್ಯಾನಿಸು CNODE_ಕೊನೆಯ_INVOCATION CNodeRotate
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು CNODE_ಕೊನೆಯ_INVOCATION CNodeSaveCaller
#ಆದರೆ_ಅಂತ್ಯ

exception_t decodeCNodeInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cap_t cap,
                                  ಶಬ್ದ_t *ಬಫರ್)
{
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    cte_t *dest_ಸ್ಥಾನ;
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ, w_ಬಿಟ್ಟುಗಳು;
    exception_t ಸ್ಥಿತಿ_2;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(cap) == cap_cnode_cap);

    ಆದರೆ (invLabel < CNodeRevoke || invLabel > CNODE_ಕೊನೆಯ_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
    w_ಬಿಟ್ಟುಗಳು = getSyscallArg(1, ಬಫರ್);

    lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cap, ಸೂಚ್ಯಂಕ, w_ಬಿಟ್ಟುಗಳು);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
    }
    dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

    ಆದರೆ (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_ಸ್ಥಾನ;
        ಶಬ್ದ_t src_ಸೂಚ್ಯಂಕ, src_ಆಳ, cap_ದತ್ತಾಂಶ;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ಬೇರು, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        ಆದರೆ (ಉದ್ದಳತೆ < 4 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        src_ಸೂಚ್ಯಂಕ = getSyscallArg(2, ಬಫರ್);
        src_ಆಳ = getSyscallArg(3, ಬಫರ್);

        src_ಬೇರು = current_extra_caps.excaprefs[0]->cap;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        lu_ret = lookup_ಮೂಲ_ಸ್ಥಾನ(src_ಬೇರು, src_ಸೂಚ್ಯಂಕ, src_ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        src_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಆದರೆ (cap_get_capType(src_ಸ್ಥಾನ->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_ಆಳ);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆಯ್ಕೆ (invLabel) {
        ಸ್ಥಿತಿ CNodeCopy:

            ಆದರೆ (ಉದ್ದಳತೆ < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_ಶಬ್ದ(getSyscallArg(4, ಬಫರ್));
            srcCap = ಮುಖವಾಡ_Cap_Rights(cap_rights, src_ಸ್ಥಾನ->cap);
            dc_ret = deriveCap(src_ಸ್ಥಾನ, srcCap);
            ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ CNodeMint:
            ಆದರೆ (ಉದ್ದಳತೆ < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_ಶಬ್ದ(getSyscallArg(4, ಬಫರ್));
            cap_ದತ್ತಾಂಶ = getSyscallArg(5, ಬಫರ್);
            srcCap = ಮುಖವಾಡ_Cap_Rights(cap_rights, src_ಸ್ಥಾನ->cap);
            dc_ret = deriveCap(src_ಸ್ಥಾನ,
                               update_Cap_ದತ್ತಾಂಶ(false, cap_ದತ್ತಾಂಶ, srcCap));
            ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                ಹಿಂತಿರುಗಿ dc_ret.ಸ್ಥಿತಿ_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ CNodeMove:
            newCap = src_ಸ್ಥಾನ->cap;
            isMove = true;

            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ CNodeMutate:
            ಆದರೆ (ಉದ್ದಳತೆ < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            cap_ದತ್ತಾಂಶ = getSyscallArg(4, ಬಫರ್);
            newCap = update_Cap_ದತ್ತಾಂಶ(true, cap_ದತ್ತಾಂಶ, src_ಸ್ಥಾನ->cap);
            isMove = true;

            ನಿಲ್ಲಿಸು;

        ಪೂರ್ವನಿಯೋಜಿತ:
            ಪ್ರತಿಜ್ಞಾ(0);
            ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
        }

        ಆದರೆ (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಆದರೆ (isMove) {
            ಹಿಂತಿರುಗಿ invokeCNodeMove(newCap, src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ invoke_CNode_ಸೇರಿಸು(newCap, src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
        }
    }

    ಆದರೆ (invLabel == CNodeRevoke) {
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeCNodeRevoke(dest_ಸ್ಥಾನ);
    }

    ಆದರೆ (invLabel == CNodeDelete) {
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeCNodeDelete(dest_ಸ್ಥಾನ);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (invLabel == CNodeSaveCaller) {
        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeCNodeSaveCaller(dest_ಸ್ಥಾನ);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_ಸ್ಥಾನ->cap;

        ಆದರೆ (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeCNodeCancelBadgedSends(destCap);
    }

    ಆದರೆ (invLabel == CNodeRotate) {
        ಶಬ್ದ_t ಅಚ್ಚು_New_ದತ್ತಾಂಶ, ಅಚ್ಚು_ಸೂಚ್ಯಂಕ, ಅಚ್ಚು_ಆಳ;
        ಶಬ್ದ_t src_New_ದತ್ತಾಂಶ, src_ಸೂಚ್ಯಂಕ, src_ಆಳ;
        cte_t *ಅಚ್ಚು_ಸ್ಥಾನ, *src_ಸ್ಥಾನ;
        cap_t ಅಚ್ಚು_ಬೇರು, src_ಬೇರು, newSrcCap, new_ಅಚ್ಚು_Cap;

        ಆದರೆ (ಉದ್ದಳತೆ < 8 || current_extra_caps.excaprefs[0] == ಖಾಲಿ
            || current_extra_caps.excaprefs[1] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಅಚ್ಚು_New_ದತ್ತಾಂಶ = getSyscallArg(2, ಬಫರ್);
        ಅಚ್ಚು_ಸೂಚ್ಯಂಕ   = getSyscallArg(3, ಬಫರ್);
        ಅಚ್ಚು_ಆಳ   = getSyscallArg(4, ಬಫರ್);
        src_New_ದತ್ತಾಂಶ   = getSyscallArg(5, ಬಫರ್);
        src_ಸೂಚ್ಯಂಕ     = getSyscallArg(6, ಬಫರ್);
        src_ಆಳ     = getSyscallArg(7, ಬಫರ್);

        ಅಚ್ಚು_ಬೇರು = current_extra_caps.excaprefs[0]->cap;
        src_ಬೇರು   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_ಮೂಲ_ಸ್ಥಾನ(src_ಬೇರು, src_ಸೂಚ್ಯಂಕ, src_ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        src_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        lu_ret = lookup_ಅಚ್ಚು_ಸ್ಥಾನ(ಅಚ್ಚು_ಬೇರು, ಅಚ್ಚು_ಸೂಚ್ಯಂಕ, ಅಚ್ಚು_ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        ಅಚ್ಚು_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಆದರೆ (ಅಚ್ಚು_ಸ್ಥಾನ == src_ಸ್ಥಾನ || ಅಚ್ಚು_ಸ್ಥಾನ == dest_ಸ್ಥಾನ) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (src_ಸ್ಥಾನ != dest_ಸ್ಥಾನ) {
            ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
            ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
            }
        }

        ಆದರೆ (cap_get_capType(src_ಸ್ಥಾನ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_ಆಳ);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (cap_get_capType(ಅಚ್ಚು_ಸ್ಥಾನ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(ಅಚ್ಚು_ಆಳ);
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_ದತ್ತಾಂಶ(true, src_New_ದತ್ತಾಂಶ, src_ಸ್ಥಾನ->cap);
        new_ಅಚ್ಚು_Cap = update_Cap_ದತ್ತಾಂಶ(true, ಅಚ್ಚು_New_ದತ್ತಾಂಶ, ಅಚ್ಚು_ಸ್ಥಾನ->cap);

        ಆದರೆ (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (cap_get_capType(new_ಅಚ್ಚು_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ invokeCNodeRotate(newSrcCap, new_ಅಚ್ಚು_Cap,
                                 src_ಸ್ಥಾನ, ಅಚ್ಚು_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_ಸ್ಥಾನ)
{
    ಹಿಂತಿರುಗಿ cteRevoke(dest_ಸ್ಥಾನ);
}

exception_t invokeCNodeDelete(cte_t *dest_ಸ್ಥಾನ)
{
    ಹಿಂತಿರುಗಿ cteDelete(dest_ಸ್ಥಾನ, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    ಶಬ್ದ_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    ಆದರೆ (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invoke_CNode_ಸೇರಿಸು(cap_t cap, cte_t *src_ಸ್ಥಾನ, cte_t *dest_ಸ್ಥಾನ)
{
    cte_ಸೇರಿಸು(cap, src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_ಸ್ಥಾನ, cte_t *dest_ಸ್ಥಾನ)
{
    cteMove(cap, src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    ಆದರೆ (slot1 == slot3) {
        cte_ಅದಲುಬದಲು(cap1, slot1, cap2, slot2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_ಸ್ಥಾನ)
{
    cap_t cap;
    cte_t *src_ಸ್ಥಾನ;

    src_ಸ್ಥಾನ = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbCaller);
    cap = src_ಸ್ಥಾನ->cap;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಉತ್ತರ_cap:
        ಆದರೆ (!cap_ಉತ್ತರ_cap_get_cap_ಉತ್ತರ_Master(cap)) {
            cteMove(cap, src_ಸ್ಥಾನ, dest_ಸ್ಥಾನ);
        }
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("caller capability must be null or reply");
        ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_Untyped_Cap_As_ತುಂಬಿದ(cap_t srcCap, cap_t newCap, cte_t *src_ಸ್ಥಾನ)
{
    ಆದರೆ ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        ಆದರೆ ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(newCap)
                == cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(srcCap))) {
            cap_untyped_cap_ptr_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(&(src_ಸ್ಥಾನ->cap),
                                                 MAX_FREE_ಸೂಚ್ಯಂಕ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(srcCap)));
        }
    }
}

ಶೂನ್ಯ cte_ಸೇರಿಸು(cap_t newCap, cte_t *src_ಸ್ಥಾನ, cte_t *dest_ಸ್ಥಾನ)
{
    mdb_ಗಂಟು_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_ಸ್ಥಾನ->cteMDBNode;
    srcCap = src_ಸ್ಥಾನ->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ಗಂಟು_ಗಣ_mdb_Prev(srcMDB, CTE_REF(src_ಸ್ಥಾನ));
    newMDB = mdb_ಗಂಟು_ಗಣ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ಗಂಟು_ಗಣ_mdb_ಮೊದಲ_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(dest_ಸ್ಥಾನ->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ಪ್ರತಿಜ್ಞಾ((cte_t *)mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(dest_ಸ್ಥಾನ->cteMDBNode) == ಖಾಲಿ &&
           (cte_t *)mdb_ಗಂಟು_get_mdb_Prev(dest_ಸ್ಥಾನ->cteMDBNode) == ಖಾಲಿ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    ಗಣ_Untyped_Cap_As_ತುಂಬಿದ(srcCap, newCap, src_ಸ್ಥಾನ);

    dest_ಸ್ಥಾನ->cap = newCap;
    dest_ಸ್ಥಾನ->cteMDBNode = newMDB;
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(&src_ಸ್ಥಾನ->cteMDBNode, CTE_REF(dest_ಸ್ಥಾನ));
    ಆದರೆ (mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(newMDB)) {
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(
            &CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(newMDB))->cteMDBNode,
            CTE_REF(dest_ಸ್ಥಾನ));
    }
}

ಶೂನ್ಯ cteMove(cap_t newCap, cte_t *src_ಸ್ಥಾನ, cte_t *dest_ಸ್ಥಾನ)
{
    mdb_ಗಂಟು_t mdb;
    ಶಬ್ದ_t prev_ptr, ಮುಂದಿನ_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(dest_ಸ್ಥಾನ->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ಪ್ರತಿಜ್ಞಾ((cte_t *)mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(dest_ಸ್ಥಾನ->cteMDBNode) == ಖಾಲಿ &&
           (cte_t *)mdb_ಗಂಟು_get_mdb_Prev(dest_ಸ್ಥಾನ->cteMDBNode) == ಖಾಲಿ);

    mdb = src_ಸ್ಥಾನ->cteMDBNode;
    dest_ಸ್ಥಾನ->cap = newCap;
    src_ಸ್ಥಾನ->cap = cap_null_cap_new();
    dest_ಸ್ಥಾನ->cteMDBNode = mdb;
    src_ಸ್ಥಾನ->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb);
    ಆದರೆ (prev_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_ಸ್ಥಾನ));

    ಮುಂದಿನ_ptr = mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb);
    ಆದರೆ (ಮುಂದಿನ_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(
            &CTE_PTR(ಮುಂದಿನ_ptr)->cteMDBNode,
            CTE_REF(dest_ಸ್ಥಾನ));
}

ಶೂನ್ಯ cap_ಅದಲುಬದಲು_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    ಆದರೆ (slot1 == slot2) {
        ಹಿಂತಿರುಗಿ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_ಅದಲುಬದಲು(cap1, slot1, cap2, slot2);
}

ಶೂನ್ಯ cte_ಅದಲುಬದಲು(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ಗಂಟು_t mdb1, mdb2;
    ಶಬ್ದ_t ಮುಂದಿನ_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb1);
    ಆದರೆ (prev_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    ಮುಂದಿನ_ptr = mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb1);
    ಆದರೆ (ಮುಂದಿನ_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(
            &CTE_PTR(ಮುಂದಿನ_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ಗಂಟು_get_mdb_Prev(mdb2);
    ಆದರೆ (prev_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    ಮುಂದಿನ_ptr = mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb2);
    ಆದರೆ (ಮುಂದಿನ_ptr)
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(
            &CTE_PTR(ಮುಂದಿನ_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *ಸ್ಥಾನ)
{
    cte_t *ಮುಂದಿನ_Ptr;
    exception_t ಸ್ಥಿತಿ_2;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    ಗಾಗಿ (ಮುಂದಿನ_Ptr = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode));
         ಮುಂದಿನ_Ptr && is_MDBParent_ದ(ಸ್ಥಾನ, ಮುಂದಿನ_Ptr);
         ಮುಂದಿನ_Ptr = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode))) {
        ಸ್ಥಿತಿ_2 = cteDelete(ಮುಂದಿನ_Ptr, true);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಸ್ಥಿತಿ_2 = preemption_ಬಿಂದು();
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *ಸ್ಥಾನ, bool_t exposed)
{
    finalise_ಸ್ಥಾನ_ret_t fs_ret;

    fs_ret = finalise_ಸ್ಥಾನ(ಸ್ಥಾನ, exposed);
    ಆದರೆ (fs_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ fs_ret.ಸ್ಥಿತಿ_2;
    }

    ಆದರೆ (exposed || fs_ret.success) {
        ಖಾಲಿಗಳು_ಸ್ಥಾನ(ಸ್ಥಾನ, fs_ret.cleanupInfo);
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ ಶೂನ್ಯ ಖಾಲಿಗಳು_ಸ್ಥಾನ(cte_t *ಸ್ಥಾನ, cap_t cleanupInfo)
{
    ಆದರೆ (cap_get_capType(ಸ್ಥಾನ->cap) != cap_null_cap) {
        mdb_ಗಂಟು_t mdb_ಗಂಟು;
        cte_t *prev, *ಮುಂದಿನ;

        mdb_ಗಂಟು = ಸ್ಥಾನ->cteMDBNode;
        prev = CTE_PTR(mdb_ಗಂಟು_get_mdb_Prev(mdb_ಗಂಟು));
        ಮುಂದಿನ = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb_ಗಂಟು));

        ಆದರೆ (prev) {
            mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(&prev->cteMDBNode, CTE_REF(ಮುಂದಿನ));
        }
        ಆದರೆ (ಮುಂದಿನ) {
            mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(&ಮುಂದಿನ->cteMDBNode, CTE_REF(prev));
        }
        ಆದರೆ (ಮುಂದಿನ)
            mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮೊದಲ_Badged(&ಮುಂದಿನ->cteMDBNode,
                                            mdb_ಗಂಟು_get_mdb_ಮೊದಲ_Badged(ಮುಂದಿನ->cteMDBNode) ||
                                            mdb_ಗಂಟು_get_mdb_ಮೊದಲ_Badged(mdb_ಗಂಟು));
        ಸ್ಥಾನ->cap = cap_null_cap_new();
        ಸ್ಥಾನ->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t CONST capRemovable(cap_t cap, cte_t *ಸ್ಥಾನ)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_null_cap:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ cap_zombie_cap: {
        ಶಬ್ದ_t n = cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(cap);
        cte_t *z_ಸ್ಥಾನ = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        ಹಿಂತಿರುಗಿ (n == 0 || (n == 1 && ಸ್ಥಾನ == z_ಸ್ಥಾನ));
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool_t CONST capCyclicZombie(cap_t cap, cte_t *ಸ್ಥಾನ)
{
    ಹಿಂತಿರುಗಿ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == ಸ್ಥಾನ;
}

ಸ್ಥಿರ finalise_ಸ್ಥಾನ_ret_t finalise_ಸ್ಥಾನ(cte_t *ಸ್ಥಾನ, bool_t ತಕ್ಷಣ)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t ಸ್ಥಿತಿ_2;
    finalise_ಸ್ಥಾನ_ret_t ret;

    ಆಗ (cap_get_capType(ಸ್ಥಾನ->cap) != cap_null_cap) {
        final = isFinalCapability(ಸ್ಥಾನ);
        fc_ret = finaliseCap(ಸ್ಥಾನ->cap, final, false);

        ಆದರೆ (capRemovable(fc_ret.remainder, ಸ್ಥಾನ)) {
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ಹಿಂತಿರುಗಿ ret;
        }

        ಸ್ಥಾನ->cap = fc_ret.remainder;

        ಆದರೆ (!ತಕ್ಷಣ && capCyclicZombie(fc_ret.remainder, ಸ್ಥಾನ)) {
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ಹಿಂತಿರುಗಿ ret;
        }

        ಸ್ಥಿತಿ_2 = reduceZombie(ಸ್ಥಾನ, ತಕ್ಷಣ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ret.ಸ್ಥಿತಿ_2 = ಸ್ಥಿತಿ_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ಹಿಂತಿರುಗಿ ret;
        }

        ಸ್ಥಿತಿ_2 = preemption_ಬಿಂದು();
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ret.ಸ್ಥಿತಿ_2 = ಸ್ಥಿತಿ_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ಹಿಂತಿರುಗಿ ret;
        }
    }
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ exception_t reduceZombie(cte_t *ಸ್ಥಾನ, bool_t ತಕ್ಷಣ)
{
    cte_t *ptr;
    ಶಬ್ದ_t n, type;
    exception_t ಸ್ಥಿತಿ_2;

    ಪ್ರತಿಜ್ಞಾ(cap_get_capType(ಸ್ಥಾನ->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(ಸ್ಥಾನ->cap);
    n = cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(ಸ್ಥಾನ->cap);
    type = cap_zombie_cap_get_capZombieType(ಸ್ಥಾನ->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ಪ್ರತಿಜ್ಞಾ(n > 0);

    ಆದರೆ (ತಕ್ಷಣ) {
        cte_t *ಕೊನೆ_ಸ್ಥಾನ = &ptr[n - 1];

        ಸ್ಥಿತಿ_2 = cteDelete(ಕೊನೆ_ಸ್ಥಾನ, false);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಆಯ್ಕೆ (cap_get_capType(ಸ್ಥಾನ->cap)) {
        ಸ್ಥಿತಿ cap_null_cap:
            ನಿಲ್ಲಿಸು;

        ಸ್ಥಿತಿ cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(ಸ್ಥಾನ->cap);

            ಆದರೆ (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_ಸಂಖ್ಯೆ(ಸ್ಥಾನ->cap) == n &&
                cap_zombie_cap_get_capZombieType(ಸ್ಥಾನ->cap) == type) {
                ಪ್ರತಿಜ್ಞಾ(cap_get_capType(ಕೊನೆ_ಸ್ಥಾನ->cap) == cap_null_cap);
                ಸ್ಥಾನ->cap =
                    cap_zombie_cap_ಗಣ_cap_Zombie_ಸಂಖ್ಯೆ(ಸ್ಥಾನ->cap, n - 1);
            } ಇಲ್ಲದಿದ್ದರೆ {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ಪ್ರತಿಜ್ಞಾ(ptr2 == ಸ್ಥಾನ && ptr != ಸ್ಥಾನ);
            }
            ನಿಲ್ಲಿಸು;
        }

        ಪೂರ್ವನಿಯೋಜಿತ:
            fail("Expected recursion to result in Zombie.");
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ಪ್ರತಿಜ್ಞಾ(ptr != ಸ್ಥಾನ);

        ಆದರೆ (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ಪ್ರತಿಜ್ಞಾ(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_ಅದಲುಬದಲು_For_Delete(ptr, ಸ್ಥಾನ);
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ cteDeleteOne(cte_t *ಸ್ಥಾನ)
{
    ಶಬ್ದ_t cap_type = cap_get_capType(ಸ್ಥಾನ->cap);
    ಆದರೆ (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret ಬಳಸದ;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(ಸ್ಥಾನ);
        fc_ret = finaliseCap(ಸ್ಥಾನ->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ಪ್ರತಿಜ್ಞಾ(capRemovable(fc_ret.remainder, ಸ್ಥಾನ) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        ಖಾಲಿಗಳು_ಸ್ಥಾನ(ಸ್ಥಾನ, cap_null_cap_new());
    }
}

ಶೂನ್ಯ ಸೇರಿಸು_New_Cap(cte_t *ಪೋಷಕ, cte_t *ಸ್ಥಾನ, cap_t cap)
{
    cte_t *ಮುಂದಿನ;

    ಮುಂದಿನ = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಪೋಷಕ->cteMDBNode));
    ಸ್ಥಾನ->cap = cap;
    ಸ್ಥಾನ->cteMDBNode = mdb_ಗಂಟು_new(CTE_REF(ಮುಂದಿನ), true, true, CTE_REF(ಪೋಷಕ));
    ಆದರೆ (ಮುಂದಿನ) {
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Prev(&ಮುಂದಿನ->cteMDBNode, CTE_REF(ಸ್ಥಾನ));
    }
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮುಂದಿನ(&ಪೋಷಕ->cteMDBNode, CTE_REF(ಸ್ಥಾನ));
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
ಶೂನ್ಯ ಸಿದ್ಧತೆ_ಉತ್ತರ_Master(tcb_t *thread)
{
    cte_t *ಸ್ಥಾನ;

    ಸ್ಥಾನ = TCB_PTR_CTE_PTR(thread, tcb_ಉತ್ತರ);
    ಆದರೆ (cap_get_capType(ಸ್ಥಾನ->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        ಸ್ಥಾನ->cap = cap_ಉತ್ತರ_cap_new(true, true, TCB_REF(thread));
        ಸ್ಥಾನ->cteMDBNode = nullMDBNode;
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_Revocable(&ಸ್ಥಾನ->cteMDBNode, true);
        mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮೊದಲ_Badged(&ಸ್ಥಾನ->cteMDBNode, true);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

bool_t ಶುದ್ಧ is_MDBParent_ದ(cte_t *cte_a, cte_t *cte_b)
{
    ಆದರೆ (!mdb_ಗಂಟು_get_mdb_Revocable(cte_a->cteMDBNode)) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        ಹಿಂತಿರುಗಿ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    ಆದರೆ (!Arch_is_MDBParent_ದ(cte_a->cap, cte_b->cap,
                            mdb_ಗಂಟು_get_mdb_ಮೊದಲ_Badged(cte_b->cteMDBNode))) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆಯ್ಕೆ (cap_get_capType(cte_a->cap)) {
    ಸ್ಥಿತಿ cap_endpoint_cap: {
        ಶಬ್ದ_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        ಆದರೆ (badge == 0) {
            ಹಿಂತಿರುಗಿ true;
        }
        ಹಿಂತಿರುಗಿ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ಗಂಟು_get_mdb_ಮೊದಲ_Badged(cte_b->cteMDBNode);
        ನಿಲ್ಲಿಸು;
    }

    ಸ್ಥಿತಿ cap_notification_cap: {
        ಶಬ್ದ_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        ಆದರೆ (badge == 0) {
            ಹಿಂತಿರುಗಿ true;
        }
        ಹಿಂತಿರುಗಿ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ಗಂಟು_get_mdb_ಮೊದಲ_Badged(cte_b->cteMDBNode);
        ನಿಲ್ಲಿಸು;
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ true;
        ನಿಲ್ಲಿಸು;
    }
}

exception_t ensureNoChildren(cte_t *ಸ್ಥಾನ)
{
    ಆದರೆ (mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode) != 0) {
        cte_t *ಮುಂದಿನ;

        ಮುಂದಿನ = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(ಸ್ಥಾನ->cteMDBNode));
        ಆದರೆ (is_MDBParent_ದ(ಸ್ಥಾನ, ಮುಂದಿನ)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(cte_t *ಸ್ಥಾನ)
{
    ಆದರೆ (cap_get_capType(ಸ್ಥಾನ->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

bool_t ಶುದ್ಧ isFinalCapability(cte_t *cte)
{
    mdb_ಗಂಟು_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    ಆದರೆ (mdb_ಗಂಟು_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } ಇಲ್ಲದಿದ್ದರೆ {
        cte_t *prev;

        prev = CTE_PTR(mdb_ಗಂಟು_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    ಆದರೆ (prevIsSameObject) {
        ಹಿಂತಿರುಗಿ false;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಆದರೆ (mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb) == 0) {
            ಹಿಂತಿರುಗಿ true;
        } ಇಲ್ಲದಿದ್ದರೆ {
            cte_t *ಮುಂದಿನ;

            ಮುಂದಿನ = CTE_PTR(mdb_ಗಂಟು_get_mdb_ಮುಂದಿನ(mdb));
            ಹಿಂತಿರುಗಿ !sameObjectAs(cte->cap, ಮುಂದಿನ->cap);
        }
    }
}

bool_t ಶುದ್ಧ ಸ್ಥಾನ_Cap_Long_Running_Delete(cte_t *ಸ್ಥಾನ)
{
    ಆದರೆ (cap_get_capType(ಸ್ಥಾನ->cap) == cap_null_cap) {
        ಹಿಂತಿರುಗಿ false;
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (! isFinalCapability(ಸ್ಥಾನ)) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆಯ್ಕೆ (cap_get_capType(ಸ್ಥಾನ->cap)) {
    ಸ್ಥಿತಿ cap_thread_cap:
    ಸ್ಥಿತಿ cap_zombie_cap:
    ಸ್ಥಿತಿ cap_cnode_cap:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, ಶಬ್ದ_t *ಬಫರ್)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_ಸ್ಥಾನ_ret_t lus_ret;
    cte_t *ಸ್ಥಾನ;
    cap_t cnode;

    ಆದರೆ (!ಬಫರ್) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ct = ತುಂಬು_Cap_Transfer(ಬಫರ್);
    cptr = ct.ct_Receive_ಬೇರು;

    luc_ret = lookupCap(thread, cptr);
    ಆದರೆ (luc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(cnode, ct.ct_Receive_ಸೂಚ್ಯಂಕ, ct.ct_Receive_ಆಳ);
    ಆದರೆ (lus_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಸ್ಥಾನ = lus_ret.ಸ್ಥಾನ;

    ಆದರೆ (cap_get_capType(ಸ್ಥಾನ->cap) != cap_null_cap) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ ಸ್ಥಾನ;
}

cap_transfer_t ಶುದ್ಧ ತುಂಬು_Cap_Transfer(ಶಬ್ದ_t *ಬಫರ್)
{
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = se_L4_ಸಂದೇಶ_Max_ಉದ್ದಳತೆ + se_L4_ಸಂದೇಶ_Max_Extra_Caps + 2;
    ಹಿಂತಿರುಗಿ cap_Transfer_From_ಪದಗಳು(ಬಫರ್ + ಸ್ಥಾನಾಂತರ);
}
