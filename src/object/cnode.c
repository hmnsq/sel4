/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <assert.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/invocation.h>
#شامل <api/syscall.h>
#شامل <api/types.h>
#شامل <machine/io.h>
#شامل <object/structures.h>
#شامل <object/objecttype.h>
#شامل <object/cnode.h>
#شامل <object/interrupt.h>
#شامل <object/untyped.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <model/preemption.h>
#شامل <model/statedata.h>
#شامل <util.h>

ساخت finalise_خانہ_ret {
    exception_t حالت_2;
    bool_t success;
    cap_t cleanupInfo;
};
قسمتعریف ساخت finalise_خانہ_ret finalise_خانہ_ret_t;

ساکن finalise_خانہ_ret_t finalise_خانہ(cte_t *خانہ, bool_t exposed);
ساکن خالی خالی_خانے_خانہ(cte_t *خانہ, cap_t cleanupInfo);
ساکن exception_t reduceZombie(cte_t *خانہ, bool_t exposed);

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو CNODE_آخری_INVOCATION CNodeRotate
#ورنہ
#تعریف_کرو CNODE_آخری_INVOCATION CNodeSaveCaller
#اگر_ختم

exception_t decodeCNodeInvocation(لفظ_t invLabel, لفظ_t لمبائی, cap_t cap,
                                  لفظ_t *بفر)
{
    lookup_خانہ_ret_t lu_ret;
    cte_t *dest_خانہ;
    لفظ_t اشاریہ, w_بٹ;
    exception_t حالت_2;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    اگر (invLabel < CNodeRevoke || invLabel > CNODE_آخری_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    اگر (لمبائی < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    w_بٹ = getSyscallArg(1, بفر);

    lu_ret = lookup_ہدف_خانہ(cap, اشاریہ, w_بٹ);
    اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        واپس lu_ret.حالت_2;
    }
    dest_خانہ = lu_ret.خانہ;

    اگر (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_خانہ;
        لفظ_t src_اشاریہ, src_گہرائی, cap_ڈیٹا;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_جڑ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        اگر (لمبائی < 4 || current_extra_caps.excaprefs[0] == تہی) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        src_اشاریہ = getSyscallArg(2, بفر);
        src_گہرائی = getSyscallArg(3, بفر);

        src_جڑ = current_extra_caps.excaprefs[0]->cap;

        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            واپس حالت_2;
        }

        lu_ret = lookup_ماخذ_خانہ(src_جڑ, src_اشاریہ, src_گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            واپس lu_ret.حالت_2;
        }
        src_خانہ = lu_ret.خانہ;

        اگر (cap_get_capType(src_خانہ->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_گہرائی);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        انتخاب (invLabel) {
        حالت CNodeCopy:

            اگر (لمبائی < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                واپس EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            srcCap = نقاب_Cap_Rights(cap_rights, src_خانہ->cap);
            dc_ret = deriveCap(src_خانہ, srcCap);
            اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                واپس dc_ret.حالت_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            توڑو;

        حالت CNodeMint:
            اگر (لمبائی < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                واپس EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            cap_ڈیٹا = getSyscallArg(5, بفر);
            srcCap = نقاب_Cap_Rights(cap_rights, src_خانہ->cap);
            dc_ret = deriveCap(src_خانہ,
                               update_Cap_ڈیٹا(false, cap_ڈیٹا, srcCap));
            اگر (dc_ret.حالت_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                واپس dc_ret.حالت_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            توڑو;

        حالت CNodeMove:
            newCap = src_خانہ->cap;
            isMove = true;

            توڑو;

        حالت CNodeMutate:
            اگر (لمبائی < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                واپس EXCEPTION_SYSCALL_ERROR;
            }

            cap_ڈیٹا = getSyscallArg(4, بفر);
            newCap = update_Cap_ڈیٹا(true, cap_ڈیٹا, src_خانہ->cap);
            isMove = true;

            توڑو;

        طےشدہ:
            أكد(0);
            واپس EXCEPTION_NONE;
        }

        اگر (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        اگر (isMove) {
            واپس invokeCNodeMove(newCap, src_خانہ, dest_خانہ);
        } ورنہ {
            واپس invoke_CNode_داخل_کرو(newCap, src_خانہ, dest_خانہ);
        }
    }

    اگر (invLabel == CNodeRevoke) {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeCNodeRevoke(dest_خانہ);
    }

    اگر (invLabel == CNodeDelete) {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeCNodeDelete(dest_خانہ);
    }

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (invLabel == CNodeSaveCaller) {
        حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            واپس حالت_2;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeCNodeSaveCaller(dest_خانہ);
    }
#اگر_ختم

    اگر (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_خانہ->cap;

        اگر (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeCNodeCancelBadgedSends(destCap);
    }

    اگر (invLabel == CNodeRotate) {
        لفظ_t محور_New_ڈیٹا, محور_اشاریہ, محور_گہرائی;
        لفظ_t src_New_ڈیٹا, src_اشاریہ, src_گہرائی;
        cte_t *محور_خانہ, *src_خانہ;
        cap_t محور_جڑ, src_جڑ, newSrcCap, new_محور_Cap;

        اگر (لمبائی < 8 || current_extra_caps.excaprefs[0] == تہی
            || current_extra_caps.excaprefs[1] == تہی) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
        محور_New_ڈیٹا = getSyscallArg(2, بفر);
        محور_اشاریہ   = getSyscallArg(3, بفر);
        محور_گہرائی   = getSyscallArg(4, بفر);
        src_New_ڈیٹا   = getSyscallArg(5, بفر);
        src_اشاریہ     = getSyscallArg(6, بفر);
        src_گہرائی     = getSyscallArg(7, بفر);

        محور_جڑ = current_extra_caps.excaprefs[0]->cap;
        src_جڑ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_ماخذ_خانہ(src_جڑ, src_اشاریہ, src_گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس lu_ret.حالت_2;
        }
        src_خانہ = lu_ret.خانہ;

        lu_ret = lookup_محور_خانہ(محور_جڑ, محور_اشاریہ, محور_گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            واپس lu_ret.حالت_2;
        }
        محور_خانہ = lu_ret.خانہ;

        اگر (محور_خانہ == src_خانہ || محور_خانہ == dest_خانہ) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (src_خانہ != dest_خانہ) {
            حالت_2 = ensure_خالی_خانے_خانہ(dest_خانہ);
            اگر (حالت_2 != EXCEPTION_NONE) {
                واپس حالت_2;
            }
        }

        اگر (cap_get_capType(src_خانہ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_گہرائی);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (cap_get_capType(محور_خانہ->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ماخذ = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(محور_گہرائی);
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_ڈیٹا(true, src_New_ڈیٹا, src_خانہ->cap);
        new_محور_Cap = update_Cap_ڈیٹا(true, محور_New_ڈیٹا, محور_خانہ->cap);

        اگر (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        اگر (cap_get_capType(new_محور_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            واپس EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        واپس invokeCNodeRotate(newSrcCap, new_محور_Cap,
                                 src_خانہ, محور_خانہ, dest_خانہ);
    }

    واپس EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_خانہ)
{
    واپس cteRevoke(dest_خانہ);
}

exception_t invokeCNodeDelete(cte_t *dest_خانہ)
{
    واپس cteDelete(dest_خانہ, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    لفظ_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    اگر (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    واپس EXCEPTION_NONE;
}

exception_t invoke_CNode_داخل_کرو(cap_t cap, cte_t *src_خانہ, cte_t *dest_خانہ)
{
    cte_داخل_کرو(cap, src_خانہ, dest_خانہ);

    واپس EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_خانہ, cte_t *dest_خانہ)
{
    cteMove(cap, src_خانہ, dest_خانہ);

    واپس EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    اگر (slot1 == slot3) {
        cte_تبادلہ(cap1, slot1, cap2, slot2);
    } ورنہ {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    واپس EXCEPTION_NONE;
}

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_خانہ)
{
    cap_t cap;
    cte_t *src_خانہ;

    src_خانہ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCaller);
    cap = src_خانہ->cap;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        توڑو;

    حالت cap_جواب_cap:
        اگر (!cap_جواب_cap_get_cap_جواب_Master(cap)) {
            cteMove(cap, src_خانہ, dest_خانہ);
        }
        توڑو;

    طےشدہ:
        fail("caller capability must be null or reply");
        توڑو;
    }

    واپس EXCEPTION_NONE;
}
#اگر_ختم

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساکن خالی سیٹ_Untyped_Cap_As_بھرا(cap_t srcCap, cap_t newCap, cte_t *src_خانہ)
{
    اگر ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        اگر ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_بلاک_سائز(newCap)
                == cap_untyped_cap_get_cap_بلاک_سائز(srcCap))) {
            cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(src_خانہ->cap),
                                                 MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_سائز(srcCap)));
        }
    }
}

خالی cte_داخل_کرو(cap_t newCap, cte_t *src_خانہ, cte_t *dest_خانہ)
{
    mdb_گرہ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_خانہ->cteMDBNode;
    srcCap = src_خانہ->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_گرہ_سیٹ_mdb_پچھلا(srcMDB, CTE_REF(src_خانہ));
    newMDB = mdb_گرہ_سیٹ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_گرہ_سیٹ_mdb_پہلا_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(dest_خانہ->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_گرہ_get_mdb_اگلا(dest_خانہ->cteMDBNode) == تہی &&
           (cte_t *)mdb_گرہ_get_mdb_پچھلا(dest_خانہ->cteMDBNode) == تہی);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    سیٹ_Untyped_Cap_As_بھرا(srcCap, newCap, src_خانہ);

    dest_خانہ->cap = newCap;
    dest_خانہ->cteMDBNode = newMDB;
    mdb_گرہ_ptr_سیٹ_mdb_اگلا(&src_خانہ->cteMDBNode, CTE_REF(dest_خانہ));
    اگر (mdb_گرہ_get_mdb_اگلا(newMDB)) {
        mdb_گرہ_ptr_سیٹ_mdb_پچھلا(
            &CTE_PTR(mdb_گرہ_get_mdb_اگلا(newMDB))->cteMDBNode,
            CTE_REF(dest_خانہ));
    }
}

خالی cteMove(cap_t newCap, cte_t *src_خانہ, cte_t *dest_خانہ)
{
    mdb_گرہ_t mdb;
    لفظ_t پچھلا_ptr, اگلا_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(dest_خانہ->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_گرہ_get_mdb_اگلا(dest_خانہ->cteMDBNode) == تہی &&
           (cte_t *)mdb_گرہ_get_mdb_پچھلا(dest_خانہ->cteMDBNode) == تہی);

    mdb = src_خانہ->cteMDBNode;
    dest_خانہ->cap = newCap;
    src_خانہ->cap = cap_null_cap_new();
    dest_خانہ->cteMDBNode = mdb;
    src_خانہ->cteMDBNode = nullMDBNode;

    پچھلا_ptr = mdb_گرہ_get_mdb_پچھلا(mdb);
    اگر (پچھلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(پچھلا_ptr)->cteMDBNode,
            CTE_REF(dest_خانہ));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb);
    اگر (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_پچھلا(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(dest_خانہ));
}

خالی cap_تبادلہ_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    اگر (slot1 == slot2) {
        واپس;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_تبادلہ(cap1, slot1, cap2, slot2);
}

خالی cte_تبادلہ(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_گرہ_t mdb1, mdb2;
    لفظ_t اگلا_ptr, پچھلا_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    پچھلا_ptr = mdb_گرہ_get_mdb_پچھلا(mdb1);
    اگر (پچھلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(پچھلا_ptr)->cteMDBNode,
            CTE_REF(slot2));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb1);
    اگر (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_پچھلا(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    پچھلا_ptr = mdb_گرہ_get_mdb_پچھلا(mdb2);
    اگر (پچھلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(پچھلا_ptr)->cteMDBNode,
            CTE_REF(slot1));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb2);
    اگر (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_پچھلا(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *خانہ)
{
    cte_t *اگلا_Ptr;
    exception_t حالت_2;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    برائے (اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode));
         اگلا_Ptr && is_MDBParent_کا(خانہ, اگلا_Ptr);
         اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode))) {
        حالت_2 = cteDelete(اگلا_Ptr, true);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        حالت_2 = preemption_نقطہ();
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }
    }

    واپس EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *خانہ, bool_t exposed)
{
    finalise_خانہ_ret_t fs_ret;

    fs_ret = finalise_خانہ(خانہ, exposed);
    اگر (fs_ret.حالت_2 != EXCEPTION_NONE) {
        واپس fs_ret.حالت_2;
    }

    اگر (exposed || fs_ret.success) {
        خالی_خانے_خانہ(خانہ, fs_ret.cleanupInfo);
    }
    واپس EXCEPTION_NONE;
}

ساکن خالی خالی_خانے_خانہ(cte_t *خانہ, cap_t cleanupInfo)
{
    اگر (cap_get_capType(خانہ->cap) != cap_null_cap) {
        mdb_گرہ_t mdb_گرہ;
        cte_t *پچھلا, *اگلا;

        mdb_گرہ = خانہ->cteMDBNode;
        پچھلا = CTE_PTR(mdb_گرہ_get_mdb_پچھلا(mdb_گرہ));
        اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(mdb_گرہ));

        اگر (پچھلا) {
            mdb_گرہ_ptr_سیٹ_mdb_اگلا(&پچھلا->cteMDBNode, CTE_REF(اگلا));
        }
        اگر (اگلا) {
            mdb_گرہ_ptr_سیٹ_mdb_پچھلا(&اگلا->cteMDBNode, CTE_REF(پچھلا));
        }
        اگر (اگلا)
            mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&اگلا->cteMDBNode,
                                            mdb_گرہ_get_mdb_پہلا_Badged(اگلا->cteMDBNode) ||
                                            mdb_گرہ_get_mdb_پہلا_Badged(mdb_گرہ));
        خانہ->cap = cap_null_cap_new();
        خانہ->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساکن اندرونی bool_t CONST capRemovable(cap_t cap, cte_t *خانہ)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_null_cap:
        واپس true;
    حالت cap_zombie_cap: {
        لفظ_t n = cap_zombie_cap_get_cap_Zombie_نمبر(cap);
        cte_t *z_خانہ = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        واپس (n == 0 || (n == 1 && خانہ == z_خانہ));
    }
    طےشدہ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساکن اندرونی bool_t CONST capCyclicZombie(cap_t cap, cte_t *خانہ)
{
    واپس cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == خانہ;
}

ساکن finalise_خانہ_ret_t finalise_خانہ(cte_t *خانہ, bool_t فوری)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t حالت_2;
    finalise_خانہ_ret_t ret;

    جب (cap_get_capType(خانہ->cap) != cap_null_cap) {
        final = isFinalCapability(خانہ);
        fc_ret = finaliseCap(خانہ->cap, final, false);

        اگر (capRemovable(fc_ret.remainder, خانہ)) {
            ret.حالت_2 = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            واپس ret;
        }

        خانہ->cap = fc_ret.remainder;

        اگر (!فوری && capCyclicZombie(fc_ret.remainder, خانہ)) {
            ret.حالت_2 = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            واپس ret;
        }

        حالت_2 = reduceZombie(خانہ, فوری);
        اگر (حالت_2 != EXCEPTION_NONE) {
            ret.حالت_2 = حالت_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            واپس ret;
        }

        حالت_2 = preemption_نقطہ();
        اگر (حالت_2 != EXCEPTION_NONE) {
            ret.حالت_2 = حالت_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            واپس ret;
        }
    }
    ret.حالت_2 = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    واپس ret;
}

ساکن exception_t reduceZombie(cte_t *خانہ, bool_t فوری)
{
    cte_t *ptr;
    لفظ_t n, type;
    exception_t حالت_2;

    أكد(cap_get_capType(خانہ->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(خانہ->cap);
    n = cap_zombie_cap_get_cap_Zombie_نمبر(خانہ->cap);
    type = cap_zombie_cap_get_capZombieType(خانہ->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    اگر (فوری) {
        cte_t *انتہا_خانہ = &ptr[n - 1];

        حالت_2 = cteDelete(انتہا_خانہ, false);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }

        انتخاب (cap_get_capType(خانہ->cap)) {
        حالت cap_null_cap:
            توڑو;

        حالت cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(خانہ->cap);

            اگر (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_نمبر(خانہ->cap) == n &&
                cap_zombie_cap_get_capZombieType(خانہ->cap) == type) {
                أكد(cap_get_capType(انتہا_خانہ->cap) == cap_null_cap);
                خانہ->cap =
                    cap_zombie_cap_سیٹ_cap_Zombie_نمبر(خانہ->cap, n - 1);
            } ورنہ {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == خانہ && ptr != خانہ);
            }
            توڑو;
        }

        طےشدہ:
            fail("Expected recursion to result in Zombie.");
        }
    } ورنہ {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != خانہ);

        اگر (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_تبادلہ_For_Delete(ptr, خانہ);
    }
    واپس EXCEPTION_NONE;
}

خالی cteDeleteOne(cte_t *خانہ)
{
    لفظ_t cap_type = cap_get_capType(خانہ->cap);
    اگر (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret غیر_مستعمل;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(خانہ);
        fc_ret = finaliseCap(خانہ->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, خانہ) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        خالی_خانے_خانہ(خانہ, cap_null_cap_new());
    }
}

خالی داخل_کرو_New_Cap(cte_t *والد, cte_t *خانہ, cap_t cap)
{
    cte_t *اگلا;

    اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(والد->cteMDBNode));
    خانہ->cap = cap;
    خانہ->cteMDBNode = mdb_گرہ_new(CTE_REF(اگلا), true, true, CTE_REF(والد));
    اگر (اگلا) {
        mdb_گرہ_ptr_سیٹ_mdb_پچھلا(&اگلا->cteMDBNode, CTE_REF(خانہ));
    }
    mdb_گرہ_ptr_سیٹ_mdb_اگلا(&والد->cteMDBNode, CTE_REF(خانہ));
}

#اگر_غیر_تعریف_شدہ CONFIG_KERNEL_MCS
خالی تیاری_جواب_Master(tcb_t *thread)
{
    cte_t *خانہ;

    خانہ = TCB_PTR_CTE_PTR(thread, tcb_جواب);
    اگر (cap_get_capType(خانہ->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        خانہ->cap = cap_جواب_cap_new(true, true, TCB_REF(thread));
        خانہ->cteMDBNode = nullMDBNode;
        mdb_گرہ_ptr_سیٹ_mdb_Revocable(&خانہ->cteMDBNode, true);
        mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&خانہ->cteMDBNode, true);
    }
}
#اگر_ختم

bool_t خالص is_MDBParent_کا(cte_t *cte_a, cte_t *cte_b)
{
    اگر (!mdb_گرہ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        واپس false;
    }
    اگر (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        واپس false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    اگر (!Arch_is_MDBParent_کا(cte_a->cap, cte_b->cap,
                            mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode))) {
        واپس false;
    }
    انتخاب (cap_get_capType(cte_a->cap)) {
    حالت cap_endpoint_cap: {
        لفظ_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        اگر (badge == 0) {
            واپس true;
        }
        واپس (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode);
        توڑو;
    }

    حالت cap_notification_cap: {
        لفظ_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        اگر (badge == 0) {
            واپس true;
        }
        واپس (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode);
        توڑو;
    }

    طےشدہ:
        واپس true;
        توڑو;
    }
}

exception_t ensureNoChildren(cte_t *خانہ)
{
    اگر (mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode) != 0) {
        cte_t *اگلا;

        اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(خانہ->cteMDBNode));
        اگر (is_MDBParent_کا(خانہ, اگلا)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            واپس EXCEPTION_SYSCALL_ERROR;
        }
    }

    واپس EXCEPTION_NONE;
}

exception_t ensure_خالی_خانے_خانہ(cte_t *خانہ)
{
    اگر (cap_get_capType(خانہ->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    واپس EXCEPTION_NONE;
}

bool_t خالص isFinalCapability(cte_t *cte)
{
    mdb_گرہ_t mdb;
    bool_t پچھلا_Is_Same_Object;

    mdb = cte->cteMDBNode;

    اگر (mdb_گرہ_get_mdb_پچھلا(mdb) == 0) {
        پچھلا_Is_Same_Object = false;
    } ورنہ {
        cte_t *پچھلا;

        پچھلا = CTE_PTR(mdb_گرہ_get_mdb_پچھلا(mdb));
        پچھلا_Is_Same_Object = sameObjectAs(پچھلا->cap, cte->cap);
    }

    اگر (پچھلا_Is_Same_Object) {
        واپس false;
    } ورنہ {
        اگر (mdb_گرہ_get_mdb_اگلا(mdb) == 0) {
            واپس true;
        } ورنہ {
            cte_t *اگلا;

            اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(mdb));
            واپس !sameObjectAs(cte->cap, اگلا->cap);
        }
    }
}

bool_t خالص خانہ_Cap_Long_Running_Delete(cte_t *خانہ)
{
    اگر (cap_get_capType(خانہ->cap) == cap_null_cap) {
        واپس false;
    } ورنہ اگر (! isFinalCapability(خانہ)) {
        واپس false;
    }
    انتخاب (cap_get_capType(خانہ->cap)) {
    حالت cap_thread_cap:
    حالت cap_zombie_cap:
    حالت cap_cnode_cap:
        واپس true;
    طےشدہ:
        واپس false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, لفظ_t *بفر)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_خانہ_ret_t lus_ret;
    cte_t *خانہ;
    cap_t cnode;

    اگر (!بفر) {
        واپس تہی;
    }

    ct = بھرو_Cap_Transfer(بفر);
    cptr = ct.ct_Receive_جڑ;

    luc_ret = lookupCap(thread, cptr);
    اگر (luc_ret.حالت_2 != EXCEPTION_NONE) {
        واپس تہی;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_ہدف_خانہ(cnode, ct.ct_Receive_اشاریہ, ct.ct_Receive_گہرائی);
    اگر (lus_ret.حالت_2 != EXCEPTION_NONE) {
        واپس تہی;
    }
    خانہ = lus_ret.خانہ;

    اگر (cap_get_capType(خانہ->cap) != cap_null_cap) {
        واپس تہی;
    }

    واپس خانہ;
}

cap_transfer_t خالص بھرو_Cap_Transfer(لفظ_t *بفر)
{
    ثابت صحیح انحراف = se_L4_پیغام_Max_لمبائی + se_L4_پیغام_Max_Extra_Caps + 2;
    واپس cap_Transfer_From_الفاظ(بفر + انحراف);
}
