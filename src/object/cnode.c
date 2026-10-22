/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/interrupt.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/preemption.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

ਬਣਤਰ finalise_ਥਾਂ_ret {
    exception_t ਸਥਿਤੀ;
    bool_t success;
    cap_t cleanupInfo;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ finalise_ਥਾਂ_ret finalise_ਥਾਂ_ret_t;

ਸਥਿਰ finalise_ਥਾਂ_ret_t finalise_ਥਾਂ(cte_t *ਥਾਂ, bool_t exposed);
ਸਥਿਰ ਖਾਲੀ ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(cte_t *ਥਾਂ, cap_t cleanupInfo);
ਸਥਿਰ exception_t reduceZombie(cte_t *ਥਾਂ, bool_t exposed);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNODE_ਆਖਰੀ_INVOCATION CNodeRotate
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CNODE_ਆਖਰੀ_INVOCATION CNodeSaveCaller
#ਜੇ_ਅੰਤ

exception_t decodeCNodeInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cap_t cap,
                                  ਸ਼ਬਦ_t *ਬਫਰ)
{
    lookup_ਥਾਂ_ret_t lu_ret;
    cte_t *dest_ਥਾਂ;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ, w_ਬਿੱਟ;
    exception_t ਸਥਿਤੀ;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(cap) == cap_cnode_cap);

    ਜੇ (invLabel < CNodeRevoke || invLabel > CNODE_ਆਖਰੀ_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
    w_ਬਿੱਟ = getSyscallArg(1, ਬਫਰ);

    lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cap, ਸੂਚਕਾਂਕ, w_ਬਿੱਟ);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
    }
    dest_ਥਾਂ = lu_ret.ਥਾਂ;

    ਜੇ (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_ਥਾਂ;
        ਸ਼ਬਦ_t src_ਸੂਚਕਾਂਕ, src_ਡੂੰਘਾਈ, cap_ਅੰਕੜੇ;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ਜੜ੍ਹ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        ਜੇ (ਲੰਮਾਈ < 4 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        src_ਸੂਚਕਾਂਕ = getSyscallArg(2, ਬਫਰ);
        src_ਡੂੰਘਾਈ = getSyscallArg(3, ਬਫਰ);

        src_ਜੜ੍ਹ = current_extra_caps.excaprefs[0]->cap;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        lu_ret = lookup_ਸਰੋਤ_ਥਾਂ(src_ਜੜ੍ਹ, src_ਸੂਚਕਾਂਕ, src_ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        src_ਥਾਂ = lu_ret.ਥਾਂ;

        ਜੇ (cap_get_capType(src_ਥਾਂ->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_ਡੂੰਘਾਈ);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਚੋਣ (invLabel) {
        ਹਾਲਤ CNodeCopy:

            ਜੇ (ਲੰਮਾਈ < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_ਸ਼ਬਦ(getSyscallArg(4, ਬਫਰ));
            srcCap = ਨਕਾਬ_Cap_Rights(cap_rights, src_ਥਾਂ->cap);
            dc_ret = deriveCap(src_ਥਾਂ, srcCap);
            ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ਰੋਕੋ;

        ਹਾਲਤ CNodeMint:
            ਜੇ (ਲੰਮਾਈ < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_ਸ਼ਬਦ(getSyscallArg(4, ਬਫਰ));
            cap_ਅੰਕੜੇ = getSyscallArg(5, ਬਫਰ);
            srcCap = ਨਕਾਬ_Cap_Rights(cap_rights, src_ਥਾਂ->cap);
            dc_ret = deriveCap(src_ਥਾਂ,
                               update_Cap_ਅੰਕੜੇ(false, cap_ਅੰਕੜੇ, srcCap));
            ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                ਵਾਪਸ dc_ret.ਸਥਿਤੀ;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ਰੋਕੋ;

        ਹਾਲਤ CNodeMove:
            newCap = src_ਥਾਂ->cap;
            isMove = true;

            ਰੋਕੋ;

        ਹਾਲਤ CNodeMutate:
            ਜੇ (ਲੰਮਾਈ < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            cap_ਅੰਕੜੇ = getSyscallArg(4, ਬਫਰ);
            newCap = update_Cap_ਅੰਕੜੇ(true, cap_ਅੰਕੜੇ, src_ਥਾਂ->cap);
            isMove = true;

            ਰੋਕੋ;

        ਮੂਲ:
            ਪ੍ਰਤਿਜ੍ਞਾ(0);
            ਵਾਪਸ EXCEPTION_NONE;
        }

        ਜੇ (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਜੇ (isMove) {
            ਵਾਪਸ invokeCNodeMove(newCap, src_ਥਾਂ, dest_ਥਾਂ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ invoke_CNode_ਪਾਓ(newCap, src_ਥਾਂ, dest_ਥਾਂ);
        }
    }

    ਜੇ (invLabel == CNodeRevoke) {
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeCNodeRevoke(dest_ਥਾਂ);
    }

    ਜੇ (invLabel == CNodeDelete) {
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeCNodeDelete(dest_ਥਾਂ);
    }

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (invLabel == CNodeSaveCaller) {
        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeCNodeSaveCaller(dest_ਥਾਂ);
    }
#ਜੇ_ਅੰਤ

    ਜੇ (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_ਥਾਂ->cap;

        ਜੇ (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeCNodeCancelBadgedSends(destCap);
    }

    ਜੇ (invLabel == CNodeRotate) {
        ਸ਼ਬਦ_t ਧੁਰਾ_New_ਅੰਕੜੇ, ਧੁਰਾ_ਸੂਚਕਾਂਕ, ਧੁਰਾ_ਡੂੰਘਾਈ;
        ਸ਼ਬਦ_t src_New_ਅੰਕੜੇ, src_ਸੂਚਕਾਂਕ, src_ਡੂੰਘਾਈ;
        cte_t *ਧੁਰਾ_ਥਾਂ, *src_ਥਾਂ;
        cap_t ਧੁਰਾ_ਜੜ੍ਹ, src_ਜੜ੍ਹ, newSrcCap, new_ਧੁਰਾ_Cap;

        ਜੇ (ਲੰਮਾਈ < 8 || current_extra_caps.excaprefs[0] == ਨਿਰਾ
            || current_extra_caps.excaprefs[1] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਧੁਰਾ_New_ਅੰਕੜੇ = getSyscallArg(2, ਬਫਰ);
        ਧੁਰਾ_ਸੂਚਕਾਂਕ   = getSyscallArg(3, ਬਫਰ);
        ਧੁਰਾ_ਡੂੰਘਾਈ   = getSyscallArg(4, ਬਫਰ);
        src_New_ਅੰਕੜੇ   = getSyscallArg(5, ਬਫਰ);
        src_ਸੂਚਕਾਂਕ     = getSyscallArg(6, ਬਫਰ);
        src_ਡੂੰਘਾਈ     = getSyscallArg(7, ਬਫਰ);

        ਧੁਰਾ_ਜੜ੍ਹ = current_extra_caps.excaprefs[0]->cap;
        src_ਜੜ੍ਹ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_ਸਰੋਤ_ਥਾਂ(src_ਜੜ੍ਹ, src_ਸੂਚਕਾਂਕ, src_ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        src_ਥਾਂ = lu_ret.ਥਾਂ;

        lu_ret = lookup_ਧੁਰਾ_ਥਾਂ(ਧੁਰਾ_ਜੜ੍ਹ, ਧੁਰਾ_ਸੂਚਕਾਂਕ, ਧੁਰਾ_ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        ਧੁਰਾ_ਥਾਂ = lu_ret.ਥਾਂ;

        ਜੇ (ਧੁਰਾ_ਥਾਂ == src_ਥਾਂ || ਧੁਰਾ_ਥਾਂ == dest_ਥਾਂ) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (src_ਥਾਂ != dest_ਥਾਂ) {
            ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
            ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
                ਵਾਪਸ ਸਥਿਤੀ;
            }
        }

        ਜੇ (cap_get_capType(src_ਥਾਂ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_ਡੂੰਘਾਈ);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (cap_get_capType(ਧੁਰਾ_ਥਾਂ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(ਧੁਰਾ_ਡੂੰਘਾਈ);
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_ਅੰਕੜੇ(true, src_New_ਅੰਕੜੇ, src_ਥਾਂ->cap);
        new_ਧੁਰਾ_Cap = update_Cap_ਅੰਕੜੇ(true, ਧੁਰਾ_New_ਅੰਕੜੇ, ਧੁਰਾ_ਥਾਂ->cap);

        ਜੇ (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (cap_get_capType(new_ਧੁਰਾ_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ invokeCNodeRotate(newSrcCap, new_ਧੁਰਾ_Cap,
                                 src_ਥਾਂ, ਧੁਰਾ_ਥਾਂ, dest_ਥਾਂ);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_ਥਾਂ)
{
    ਵਾਪਸ cteRevoke(dest_ਥਾਂ);
}

exception_t invokeCNodeDelete(cte_t *dest_ਥਾਂ)
{
    ਵਾਪਸ cteDelete(dest_ਥਾਂ, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    ਸ਼ਬਦ_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    ਜੇ (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invoke_CNode_ਪਾਓ(cap_t cap, cte_t *src_ਥਾਂ, cte_t *dest_ਥਾਂ)
{
    cte_ਪਾਓ(cap, src_ਥਾਂ, dest_ਥਾਂ);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_ਥਾਂ, cte_t *dest_ਥਾਂ)
{
    cteMove(cap, src_ਥਾਂ, dest_ਥਾਂ);

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    ਜੇ (slot1 == slot3) {
        cte_ਅਦਲਾ_ਬਦਲੀ(cap1, slot1, cap2, slot2);
    } ਨਹੀਂਤਾਂ {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_ਥਾਂ)
{
    cap_t cap;
    cte_t *src_ਥਾਂ;

    src_ਥਾਂ = TCB_PTR_CTE_PTR(ਗੰਢ_STATE(ksCurThread), tcbCaller);
    cap = src_ਥਾਂ->cap;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਜਵਾਬ_cap:
        ਜੇ (!cap_ਜਵਾਬ_cap_get_cap_ਜਵਾਬ_Master(cap)) {
            cteMove(cap, src_ਥਾਂ, dest_ਥਾਂ);
        }
        ਰੋਕੋ;

    ਮੂਲ:
        fail("caller capability must be null or reply");
        ਰੋਕੋ;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}
#ਜੇ_ਅੰਤ

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_Untyped_Cap_As_ਭਰਿਆ(cap_t srcCap, cap_t newCap, cte_t *src_ਥਾਂ)
{
    ਜੇ ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        ਜੇ ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(newCap)
                == cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(srcCap))) {
            cap_untyped_cap_ptr_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(&(src_ਥਾਂ->cap),
                                                 MAX_FREE_ਸੂਚਕਾਂਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(srcCap)));
        }
    }
}

ਖਾਲੀ cte_ਪਾਓ(cap_t newCap, cte_t *src_ਥਾਂ, cte_t *dest_ਥਾਂ)
{
    mdb_ਗੰਢ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_ਥਾਂ->cteMDBNode;
    srcCap = src_ਥਾਂ->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ਗੰਢ_ਸੈੱਟ_mdb_Prev(srcMDB, CTE_REF(src_ਥਾਂ));
    newMDB = mdb_ਗੰਢ_ਸੈੱਟ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ਗੰਢ_ਸੈੱਟ_mdb_ਪਹਿਲਾ_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(dest_ਥਾਂ->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ਪ੍ਰਤਿਜ੍ਞਾ((cte_t *)mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(dest_ਥਾਂ->cteMDBNode) == ਨਿਰਾ &&
           (cte_t *)mdb_ਗੰਢ_get_mdb_Prev(dest_ਥਾਂ->cteMDBNode) == ਨਿਰਾ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    ਸੈੱਟ_Untyped_Cap_As_ਭਰਿਆ(srcCap, newCap, src_ਥਾਂ);

    dest_ਥਾਂ->cap = newCap;
    dest_ਥਾਂ->cteMDBNode = newMDB;
    mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(&src_ਥਾਂ->cteMDBNode, CTE_REF(dest_ਥਾਂ));
    ਜੇ (mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(newMDB)) {
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(
            &CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(newMDB))->cteMDBNode,
            CTE_REF(dest_ਥਾਂ));
    }
}

ਖਾਲੀ cteMove(cap_t newCap, cte_t *src_ਥਾਂ, cte_t *dest_ਥਾਂ)
{
    mdb_ਗੰਢ_t mdb;
    ਸ਼ਬਦ_t prev_ptr, ਅਗਲਾ_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(dest_ਥਾਂ->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ਪ੍ਰਤਿਜ੍ਞਾ((cte_t *)mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(dest_ਥਾਂ->cteMDBNode) == ਨਿਰਾ &&
           (cte_t *)mdb_ਗੰਢ_get_mdb_Prev(dest_ਥਾਂ->cteMDBNode) == ਨਿਰਾ);

    mdb = src_ਥਾਂ->cteMDBNode;
    dest_ਥਾਂ->cap = newCap;
    src_ਥਾਂ->cap = cap_null_cap_new();
    dest_ਥਾਂ->cteMDBNode = mdb;
    src_ਥਾਂ->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ਗੰਢ_get_mdb_Prev(mdb);
    ਜੇ (prev_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_ਥਾਂ));

    ਅਗਲਾ_ptr = mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb);
    ਜੇ (ਅਗਲਾ_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(
            &CTE_PTR(ਅਗਲਾ_ptr)->cteMDBNode,
            CTE_REF(dest_ਥਾਂ));
}

ਖਾਲੀ cap_ਅਦਲਾ_ਬਦਲੀ_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    ਜੇ (slot1 == slot2) {
        ਵਾਪਸ;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_ਅਦਲਾ_ਬਦਲੀ(cap1, slot1, cap2, slot2);
}

ਖਾਲੀ cte_ਅਦਲਾ_ਬਦਲੀ(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ਗੰਢ_t mdb1, mdb2;
    ਸ਼ਬਦ_t ਅਗਲਾ_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ਗੰਢ_get_mdb_Prev(mdb1);
    ਜੇ (prev_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    ਅਗਲਾ_ptr = mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb1);
    ਜੇ (ਅਗਲਾ_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(
            &CTE_PTR(ਅਗਲਾ_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ਗੰਢ_get_mdb_Prev(mdb2);
    ਜੇ (prev_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    ਅਗਲਾ_ptr = mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb2);
    ਜੇ (ਅਗਲਾ_ptr)
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(
            &CTE_PTR(ਅਗਲਾ_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *ਥਾਂ)
{
    cte_t *ਅਗਲਾ_Ptr;
    exception_t ਸਥਿਤੀ;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    ਲਈ (ਅਗਲਾ_Ptr = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode));
         ਅਗਲਾ_Ptr && is_MDBParent_ਦਾ(ਥਾਂ, ਅਗਲਾ_Ptr);
         ਅਗਲਾ_Ptr = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode))) {
        ਸਥਿਤੀ = cteDelete(ਅਗਲਾ_Ptr, true);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸਥਿਤੀ = preemption_ਬਿੰਦੂ();
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *ਥਾਂ, bool_t exposed)
{
    finalise_ਥਾਂ_ret_t fs_ret;

    fs_ret = finalise_ਥਾਂ(ਥਾਂ, exposed);
    ਜੇ (fs_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ fs_ret.ਸਥਿਤੀ;
    }

    ਜੇ (exposed || fs_ret.success) {
        ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(ਥਾਂ, fs_ret.cleanupInfo);
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ ਖਾਲੀ ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(cte_t *ਥਾਂ, cap_t cleanupInfo)
{
    ਜੇ (cap_get_capType(ਥਾਂ->cap) != cap_null_cap) {
        mdb_ਗੰਢ_t mdb_ਗੰਢ;
        cte_t *prev, *ਅਗਲਾ;

        mdb_ਗੰਢ = ਥਾਂ->cteMDBNode;
        prev = CTE_PTR(mdb_ਗੰਢ_get_mdb_Prev(mdb_ਗੰਢ));
        ਅਗਲਾ = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb_ਗੰਢ));

        ਜੇ (prev) {
            mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(&prev->cteMDBNode, CTE_REF(ਅਗਲਾ));
        }
        ਜੇ (ਅਗਲਾ) {
            mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(&ਅਗਲਾ->cteMDBNode, CTE_REF(prev));
        }
        ਜੇ (ਅਗਲਾ)
            mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਪਹਿਲਾ_Badged(&ਅਗਲਾ->cteMDBNode,
                                            mdb_ਗੰਢ_get_mdb_ਪਹਿਲਾ_Badged(ਅਗਲਾ->cteMDBNode) ||
                                            mdb_ਗੰਢ_get_mdb_ਪਹਿਲਾ_Badged(mdb_ਗੰਢ));
        ਥਾਂ->cap = cap_null_cap_new();
        ਥਾਂ->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t CONST capRemovable(cap_t cap, cte_t *ਥਾਂ)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_null_cap:
        ਵਾਪਸ true;
    ਹਾਲਤ cap_zombie_cap: {
        ਸ਼ਬਦ_t n = cap_zombie_cap_get_cap_Zombie_ਸੰਖਿਆ(cap);
        cte_t *z_ਥਾਂ = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        ਵਾਪਸ (n == 0 || (n == 1 && ਥਾਂ == z_ਥਾਂ));
    }
    ਮੂਲ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool_t CONST capCyclicZombie(cap_t cap, cte_t *ਥਾਂ)
{
    ਵਾਪਸ cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == ਥਾਂ;
}

ਸਥਿਰ finalise_ਥਾਂ_ret_t finalise_ਥਾਂ(cte_t *ਥਾਂ, bool_t ਤੁਰੰਤ)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t ਸਥਿਤੀ;
    finalise_ਥਾਂ_ret_t ret;

    ਜਦੋਂ (cap_get_capType(ਥਾਂ->cap) != cap_null_cap) {
        final = isFinalCapability(ਥਾਂ);
        fc_ret = finaliseCap(ਥਾਂ->cap, final, false);

        ਜੇ (capRemovable(fc_ret.remainder, ਥਾਂ)) {
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ਵਾਪਸ ret;
        }

        ਥਾਂ->cap = fc_ret.remainder;

        ਜੇ (!ਤੁਰੰਤ && capCyclicZombie(fc_ret.remainder, ਥਾਂ)) {
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            ਵਾਪਸ ret;
        }

        ਸਥਿਤੀ = reduceZombie(ਥਾਂ, ਤੁਰੰਤ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ret.ਸਥਿਤੀ = ਸਥਿਤੀ;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ਵਾਪਸ ret;
        }

        ਸਥਿਤੀ = preemption_ਬਿੰਦੂ();
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ret.ਸਥਿਤੀ = ਸਥਿਤੀ;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            ਵਾਪਸ ret;
        }
    }
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ ret;
}

ਸਥਿਰ exception_t reduceZombie(cte_t *ਥਾਂ, bool_t ਤੁਰੰਤ)
{
    cte_t *ptr;
    ਸ਼ਬਦ_t n, type;
    exception_t ਸਥਿਤੀ;

    ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(ਥਾਂ->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(ਥਾਂ->cap);
    n = cap_zombie_cap_get_cap_Zombie_ਸੰਖਿਆ(ਥਾਂ->cap);
    type = cap_zombie_cap_get_capZombieType(ਥਾਂ->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ਪ੍ਰਤਿਜ੍ਞਾ(n > 0);

    ਜੇ (ਤੁਰੰਤ) {
        cte_t *ਸਿਰਾ_ਥਾਂ = &ptr[n - 1];

        ਸਥਿਤੀ = cteDelete(ਸਿਰਾ_ਥਾਂ, false);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਚੋਣ (cap_get_capType(ਥਾਂ->cap)) {
        ਹਾਲਤ cap_null_cap:
            ਰੋਕੋ;

        ਹਾਲਤ cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(ਥਾਂ->cap);

            ਜੇ (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_ਸੰਖਿਆ(ਥਾਂ->cap) == n &&
                cap_zombie_cap_get_capZombieType(ਥਾਂ->cap) == type) {
                ਪ੍ਰਤਿਜ੍ਞਾ(cap_get_capType(ਸਿਰਾ_ਥਾਂ->cap) == cap_null_cap);
                ਥਾਂ->cap =
                    cap_zombie_cap_ਸੈੱਟ_cap_Zombie_ਸੰਖਿਆ(ਥਾਂ->cap, n - 1);
            } ਨਹੀਂਤਾਂ {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ਪ੍ਰਤਿਜ੍ਞਾ(ptr2 == ਥਾਂ && ptr != ਥਾਂ);
            }
            ਰੋਕੋ;
        }

        ਮੂਲ:
            fail("Expected recursion to result in Zombie.");
        }
    } ਨਹੀਂਤਾਂ {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ਪ੍ਰਤਿਜ੍ਞਾ(ptr != ਥਾਂ);

        ਜੇ (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ਪ੍ਰਤਿਜ੍ਞਾ(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_ਅਦਲਾ_ਬਦਲੀ_For_Delete(ptr, ਥਾਂ);
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ cteDeleteOne(cte_t *ਥਾਂ)
{
    ਸ਼ਬਦ_t cap_type = cap_get_capType(ਥਾਂ->cap);
    ਜੇ (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret ਅਣਵਰਤਿਆ;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(ਥਾਂ);
        fc_ret = finaliseCap(ਥਾਂ->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ਪ੍ਰਤਿਜ੍ਞਾ(capRemovable(fc_ret.remainder, ਥਾਂ) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(ਥਾਂ, cap_null_cap_new());
    }
}

ਖਾਲੀ ਪਾਓ_New_Cap(cte_t *ਮਾਪਾ, cte_t *ਥਾਂ, cap_t cap)
{
    cte_t *ਅਗਲਾ;

    ਅਗਲਾ = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਮਾਪਾ->cteMDBNode));
    ਥਾਂ->cap = cap;
    ਥਾਂ->cteMDBNode = mdb_ਗੰਢ_new(CTE_REF(ਅਗਲਾ), true, true, CTE_REF(ਮਾਪਾ));
    ਜੇ (ਅਗਲਾ) {
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Prev(&ਅਗਲਾ->cteMDBNode, CTE_REF(ਥਾਂ));
    }
    mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਅਗਲਾ(&ਮਾਪਾ->cteMDBNode, CTE_REF(ਥਾਂ));
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
ਖਾਲੀ ਤਿਆਰੀ_ਜਵਾਬ_Master(tcb_t *thread)
{
    cte_t *ਥਾਂ;

    ਥਾਂ = TCB_PTR_CTE_PTR(thread, tcb_ਜਵਾਬ);
    ਜੇ (cap_get_capType(ਥਾਂ->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        ਥਾਂ->cap = cap_ਜਵਾਬ_cap_new(true, true, TCB_REF(thread));
        ਥਾਂ->cteMDBNode = nullMDBNode;
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Revocable(&ਥਾਂ->cteMDBNode, true);
        mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਪਹਿਲਾ_Badged(&ਥਾਂ->cteMDBNode, true);
    }
}
#ਜੇ_ਅੰਤ

bool_t ਸ਼ੁੱਧ is_MDBParent_ਦਾ(cte_t *cte_a, cte_t *cte_b)
{
    ਜੇ (!mdb_ਗੰਢ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        ਵਾਪਸ false;
    }
    ਜੇ (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        ਵਾਪਸ false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    ਜੇ (!Arch_is_MDBParent_ਦਾ(cte_a->cap, cte_b->cap,
                            mdb_ਗੰਢ_get_mdb_ਪਹਿਲਾ_Badged(cte_b->cteMDBNode))) {
        ਵਾਪਸ false;
    }
    ਚੋਣ (cap_get_capType(cte_a->cap)) {
    ਹਾਲਤ cap_endpoint_cap: {
        ਸ਼ਬਦ_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        ਜੇ (badge == 0) {
            ਵਾਪਸ true;
        }
        ਵਾਪਸ (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ਗੰਢ_get_mdb_ਪਹਿਲਾ_Badged(cte_b->cteMDBNode);
        ਰੋਕੋ;
    }

    ਹਾਲਤ cap_notification_cap: {
        ਸ਼ਬਦ_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        ਜੇ (badge == 0) {
            ਵਾਪਸ true;
        }
        ਵਾਪਸ (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ਗੰਢ_get_mdb_ਪਹਿਲਾ_Badged(cte_b->cteMDBNode);
        ਰੋਕੋ;
    }

    ਮੂਲ:
        ਵਾਪਸ true;
        ਰੋਕੋ;
    }
}

exception_t ensureNoChildren(cte_t *ਥਾਂ)
{
    ਜੇ (mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode) != 0) {
        cte_t *ਅਗਲਾ;

        ਅਗਲਾ = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(ਥਾਂ->cteMDBNode));
        ਜੇ (is_MDBParent_ਦਾ(ਥਾਂ, ਅਗਲਾ)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(cte_t *ਥਾਂ)
{
    ਜੇ (cap_get_capType(ਥਾਂ->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

bool_t ਸ਼ੁੱਧ isFinalCapability(cte_t *cte)
{
    mdb_ਗੰਢ_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    ਜੇ (mdb_ਗੰਢ_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } ਨਹੀਂਤਾਂ {
        cte_t *prev;

        prev = CTE_PTR(mdb_ਗੰਢ_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    ਜੇ (prevIsSameObject) {
        ਵਾਪਸ false;
    } ਨਹੀਂਤਾਂ {
        ਜੇ (mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb) == 0) {
            ਵਾਪਸ true;
        } ਨਹੀਂਤਾਂ {
            cte_t *ਅਗਲਾ;

            ਅਗਲਾ = CTE_PTR(mdb_ਗੰਢ_get_mdb_ਅਗਲਾ(mdb));
            ਵਾਪਸ !sameObjectAs(cte->cap, ਅਗਲਾ->cap);
        }
    }
}

bool_t ਸ਼ੁੱਧ ਥਾਂ_Cap_Long_Running_Delete(cte_t *ਥਾਂ)
{
    ਜੇ (cap_get_capType(ਥਾਂ->cap) == cap_null_cap) {
        ਵਾਪਸ false;
    } ਨਹੀਂਤਾਂ ਜੇ (! isFinalCapability(ਥਾਂ)) {
        ਵਾਪਸ false;
    }
    ਚੋਣ (cap_get_capType(ਥਾਂ->cap)) {
    ਹਾਲਤ cap_thread_cap:
    ਹਾਲਤ cap_zombie_cap:
    ਹਾਲਤ cap_cnode_cap:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, ਸ਼ਬਦ_t *ਬਫਰ)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_ਥਾਂ_ret_t lus_ret;
    cte_t *ਥਾਂ;
    cap_t cnode;

    ਜੇ (!ਬਫਰ) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ct = ਲੱਦੋ_Cap_Transfer(ਬਫਰ);
    cptr = ct.ct_Receive_ਜੜ੍ਹ;

    luc_ret = lookupCap(thread, cptr);
    ਜੇ (luc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(cnode, ct.ct_Receive_ਸੂਚਕਾਂਕ, ct.ct_Receive_ਡੂੰਘਾਈ);
    ਜੇ (lus_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਥਾਂ = lus_ret.ਥਾਂ;

    ਜੇ (cap_get_capType(ਥਾਂ->cap) != cap_null_cap) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ ਥਾਂ;
}

cap_transfer_t ਸ਼ੁੱਧ ਲੱਦੋ_Cap_Transfer(ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਅਚਲ ਪੂਰਨ ਵਿੱਥ = se_L4_ਸੁਨੇਹਾ_Max_ਲੰਮਾਈ + se_L4_ਸੁਨੇਹਾ_Max_Extra_Caps + 2;
    ਵਾਪਸ cap_Transfer_From_ਸ਼ਬਦ(ਬਫਰ + ਵਿੱਥ);
}
