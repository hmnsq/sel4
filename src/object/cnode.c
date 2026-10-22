/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/invocation.h>
#ضمن <api/syscall.h>
#ضمن <api/types.h>
#ضمن <machine/io.h>
#ضمن <object/structures.h>
#ضمن <object/objecttype.h>
#ضمن <object/cnode.h>
#ضمن <object/interrupt.h>
#ضمن <object/untyped.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <model/preemption.h>
#ضمن <model/statedata.h>
#ضمن <util.h>

بنية finalise_تھاں_ret {
    exception_t حالت;
    bool_t success;
    cap_t cleanupInfo;
};
تعريفنوع بنية finalise_تھاں_ret finalise_تھاں_ret_t;

ساكن finalise_تھاں_ret_t finalise_تھاں(cte_t *تھاں, bool_t exposed);
ساكن فراغ خالی_تھاواں_تھاں(cte_t *تھاں, cap_t cleanupInfo);
ساكن exception_t reduceZombie(cte_t *تھاں, bool_t exposed);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
#تعریف_کرو CNODE_آخری_INVOCATION CNodeRotate
#وإلا
#تعریف_کرو CNODE_آخری_INVOCATION CNodeSaveCaller
#جے_انت

exception_t decodeCNodeInvocation(لفظ_t invLabel, لفظ_t لمائی, cap_t cap,
                                  لفظ_t *بفر)
{
    lookup_تھاں_ret_t lu_ret;
    cte_t *dest_تھاں;
    لفظ_t اشاریہ, w_بٹ;
    exception_t حالت;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    إذا (invLabel < CNodeRevoke || invLabel > CNODE_آخری_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (لمائی < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاریہ = getSyscallArg(0, بفر);
    w_بٹ = getSyscallArg(1, بفر);

    lu_ret = lookup_نشانہ_تھاں(cap, اشاریہ, w_بٹ);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        أرجع lu_ret.حالت;
    }
    dest_تھاں = lu_ret.تھاں;

    إذا (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_تھاں;
        لفظ_t src_اشاریہ, src_ڈونگھائی, cap_انکڑے;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_جڑ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        إذا (لمائی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        src_اشاریہ = getSyscallArg(2, بفر);
        src_ڈونگھائی = getSyscallArg(3, بفر);

        src_جڑ = current_extra_caps.excaprefs[0]->cap;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            أرجع حالت;
        }

        lu_ret = lookup_سروت_تھاں(src_جڑ, src_اشاریہ, src_ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            أرجع lu_ret.حالت;
        }
        src_تھاں = lu_ret.تھاں;

        إذا (cap_get_capType(src_تھاں->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_ڈونگھائی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            إذا (لمائی < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_سنیہا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            srcCap = نقاب_Cap_Rights(cap_rights, src_تھاں->cap);
            dc_ret = deriveCap(src_تھاں, srcCap);
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            إذا (لمائی < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_سنیہا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            cap_انکڑے = getSyscallArg(5, بفر);
            srcCap = نقاب_Cap_Rights(cap_rights, src_تھاں->cap);
            dc_ret = deriveCap(src_تھاں,
                               update_Cap_انکڑے(false, cap_انکڑے, srcCap));
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = src_تھاں->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            إذا (لمائی < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_سنیہا;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_انکڑے = getSyscallArg(4, بفر);
            newCap = update_Cap_انکڑے(true, cap_انکڑے, src_تھاں->cap);
            isMove = true;

            اكسر;

        افتراضي:
            أكد(0);
            أرجع EXCEPTION_NONE;
        }

        إذا (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        إذا (isMove) {
            أرجع invokeCNodeMove(newCap, src_تھاں, dest_تھاں);
        } وإلا {
            أرجع invoke_CNode_داخل_کرو(newCap, src_تھاں, dest_تھاں);
        }
    }

    إذا (invLabel == CNodeRevoke) {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRevoke(dest_تھاں);
    }

    إذا (invLabel == CNodeDelete) {
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeDelete(dest_تھاں);
    }

#جے_بنا_تعریف CONFIG_KERNEL_MCS
    إذا (invLabel == CNodeSaveCaller) {
        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeSaveCaller(dest_تھاں);
    }
#جے_انت

    إذا (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_تھاں->cap;

        إذا (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeCancelBadgedSends(destCap);
    }

    إذا (invLabel == CNodeRotate) {
        لفظ_t دھرا_New_انکڑے, دھرا_اشاریہ, دھرا_ڈونگھائی;
        لفظ_t src_New_انکڑے, src_اشاریہ, src_ڈونگھائی;
        cte_t *دھرا_تھاں, *src_تھاں;
        cap_t دھرا_جڑ, src_جڑ, newSrcCap, new_دھرا_Cap;

        إذا (لمائی < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        دھرا_New_انکڑے = getSyscallArg(2, بفر);
        دھرا_اشاریہ   = getSyscallArg(3, بفر);
        دھرا_ڈونگھائی   = getSyscallArg(4, بفر);
        src_New_انکڑے   = getSyscallArg(5, بفر);
        src_اشاریہ     = getSyscallArg(6, بفر);
        src_ڈونگھائی     = getSyscallArg(7, بفر);

        دھرا_جڑ = current_extra_caps.excaprefs[0]->cap;
        src_جڑ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_سروت_تھاں(src_جڑ, src_اشاریہ, src_ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        src_تھاں = lu_ret.تھاں;

        lu_ret = lookup_دھرا_تھاں(دھرا_جڑ, دھرا_اشاریہ, دھرا_ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        دھرا_تھاں = lu_ret.تھاں;

        إذا (دھرا_تھاں == src_تھاں || دھرا_تھاں == dest_تھاں) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (src_تھاں != dest_تھاں) {
            حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }

        إذا (cap_get_capType(src_تھاں->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_ڈونگھائی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(دھرا_تھاں->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(دھرا_ڈونگھائی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_انکڑے(true, src_New_انکڑے, src_تھاں->cap);
        new_دھرا_Cap = update_Cap_انکڑے(true, دھرا_New_انکڑے, دھرا_تھاں->cap);

        إذا (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(new_دھرا_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRotate(newSrcCap, new_دھرا_Cap,
                                 src_تھاں, دھرا_تھاں, dest_تھاں);
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_تھاں)
{
    أرجع cteRevoke(dest_تھاں);
}

exception_t invokeCNodeDelete(cte_t *dest_تھاں)
{
    أرجع cteDelete(dest_تھاں, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    لفظ_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    إذا (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    أرجع EXCEPTION_NONE;
}

exception_t invoke_CNode_داخل_کرو(cap_t cap, cte_t *src_تھاں, cte_t *dest_تھاں)
{
    cte_داخل_کرو(cap, src_تھاں, dest_تھاں);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_تھاں, cte_t *dest_تھاں)
{
    cteMove(cap, src_تھاں, dest_تھاں);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    إذا (slot1 == slot3) {
        cte_وٹاندرا(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    أرجع EXCEPTION_NONE;
}

#جے_بنا_تعریف CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_تھاں)
{
    cap_t cap;
    cte_t *src_تھاں;

    src_تھاں = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbCaller);
    cap = src_تھاں->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_جواب_cap:
        إذا (!cap_جواب_cap_get_cap_جواب_Master(cap)) {
            cteMove(cap, src_تھاں, dest_تھاں);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}
#جے_انت

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ سیٹ_Untyped_Cap_As_بھریا(cap_t srcCap, cap_t newCap, cte_t *src_تھاں)
{
    إذا ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        إذا ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_بلاک_ناپ(newCap)
                == cap_untyped_cap_get_cap_بلاک_ناپ(srcCap))) {
            cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(src_تھاں->cap),
                                                 MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_ناپ(srcCap)));
        }
    }
}

فراغ cte_داخل_کرو(cap_t newCap, cte_t *src_تھاں, cte_t *dest_تھاں)
{
    mdb_گرہ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_تھاں->cteMDBNode;
    srcCap = src_تھاں->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_گرہ_سیٹ_mdb_Prev(srcMDB, CTE_REF(src_تھاں));
    newMDB = mdb_گرہ_سیٹ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_گرہ_سیٹ_mdb_پہلا_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(dest_تھاں->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_گرہ_get_mdb_اگلا(dest_تھاں->cteMDBNode) == عدم &&
           (cte_t *)mdb_گرہ_get_mdb_Prev(dest_تھاں->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    سیٹ_Untyped_Cap_As_بھریا(srcCap, newCap, src_تھاں);

    dest_تھاں->cap = newCap;
    dest_تھاں->cteMDBNode = newMDB;
    mdb_گرہ_ptr_سیٹ_mdb_اگلا(&src_تھاں->cteMDBNode, CTE_REF(dest_تھاں));
    إذا (mdb_گرہ_get_mdb_اگلا(newMDB)) {
        mdb_گرہ_ptr_سیٹ_mdb_Prev(
            &CTE_PTR(mdb_گرہ_get_mdb_اگلا(newMDB))->cteMDBNode,
            CTE_REF(dest_تھاں));
    }
}

فراغ cteMove(cap_t newCap, cte_t *src_تھاں, cte_t *dest_تھاں)
{
    mdb_گرہ_t mdb;
    لفظ_t prev_ptr, اگلا_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(dest_تھاں->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_گرہ_get_mdb_اگلا(dest_تھاں->cteMDBNode) == عدم &&
           (cte_t *)mdb_گرہ_get_mdb_Prev(dest_تھاں->cteMDBNode) == عدم);

    mdb = src_تھاں->cteMDBNode;
    dest_تھاں->cap = newCap;
    src_تھاں->cap = cap_null_cap_new();
    dest_تھاں->cteMDBNode = mdb;
    src_تھاں->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_گرہ_get_mdb_Prev(mdb);
    إذا (prev_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_تھاں));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb);
    إذا (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_Prev(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(dest_تھاں));
}

فراغ cap_وٹاندرا_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    إذا (slot1 == slot2) {
        أرجع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_وٹاندرا(cap1, slot1, cap2, slot2);
}

فراغ cte_وٹاندرا(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_گرہ_t mdb1, mdb2;
    لفظ_t اگلا_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_گرہ_get_mdb_Prev(mdb1);
    إذا (prev_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb1);
    إذا (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_Prev(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_گرہ_get_mdb_Prev(mdb2);
    إذا (prev_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_اگلا(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    اگلا_ptr = mdb_گرہ_get_mdb_اگلا(mdb2);
    إذا (اگلا_ptr)
        mdb_گرہ_ptr_سیٹ_mdb_Prev(
            &CTE_PTR(اگلا_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *تھاں)
{
    cte_t *اگلا_Ptr;
    exception_t حالت;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode));
         اگلا_Ptr && is_MDBParent_دا(تھاں, اگلا_Ptr);
         اگلا_Ptr = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode))) {
        حالت = cteDelete(اگلا_Ptr, true);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        حالت = preemption_نقطہ();
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *تھاں, bool_t exposed)
{
    finalise_تھاں_ret_t fs_ret;

    fs_ret = finalise_تھاں(تھاں, exposed);
    إذا (fs_ret.حالت != EXCEPTION_NONE) {
        أرجع fs_ret.حالت;
    }

    إذا (exposed || fs_ret.success) {
        خالی_تھاواں_تھاں(تھاں, fs_ret.cleanupInfo);
    }
    أرجع EXCEPTION_NONE;
}

ساكن فراغ خالی_تھاواں_تھاں(cte_t *تھاں, cap_t cleanupInfo)
{
    إذا (cap_get_capType(تھاں->cap) != cap_null_cap) {
        mdb_گرہ_t mdb_گرہ;
        cte_t *prev, *اگلا;

        mdb_گرہ = تھاں->cteMDBNode;
        prev = CTE_PTR(mdb_گرہ_get_mdb_Prev(mdb_گرہ));
        اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(mdb_گرہ));

        إذا (prev) {
            mdb_گرہ_ptr_سیٹ_mdb_اگلا(&prev->cteMDBNode, CTE_REF(اگلا));
        }
        إذا (اگلا) {
            mdb_گرہ_ptr_سیٹ_mdb_Prev(&اگلا->cteMDBNode, CTE_REF(prev));
        }
        إذا (اگلا)
            mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&اگلا->cteMDBNode,
                                            mdb_گرہ_get_mdb_پہلا_Badged(اگلا->cteMDBNode) ||
                                            mdb_گرہ_get_mdb_پہلا_Badged(mdb_گرہ));
        تھاں->cap = cap_null_cap_new();
        تھاں->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *تھاں)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        أرجع true;
    حالة cap_zombie_cap: {
        لفظ_t n = cap_zombie_cap_get_cap_Zombie_نمبر(cap);
        cte_t *z_تھاں = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        أرجع (n == 0 || (n == 1 && تھاں == z_تھاں));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *تھاں)
{
    أرجع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == تھاں;
}

ساكن finalise_تھاں_ret_t finalise_تھاں(cte_t *تھاں, bool_t فوری)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t حالت;
    finalise_تھاں_ret_t ret;

    طالما (cap_get_capType(تھاں->cap) != cap_null_cap) {
        final = isFinalCapability(تھاں);
        fc_ret = finaliseCap(تھاں->cap, final, false);

        إذا (capRemovable(fc_ret.remainder, تھاں)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        تھاں->cap = fc_ret.remainder;

        إذا (!فوری && capCyclicZombie(fc_ret.remainder, تھاں)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        حالت = reduceZombie(تھاں, فوری);
        إذا (حالت != EXCEPTION_NONE) {
            ret.حالت = حالت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }

        حالت = preemption_نقطہ();
        إذا (حالت != EXCEPTION_NONE) {
            ret.حالت = حالت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }
    }
    ret.حالت = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    أرجع ret;
}

ساكن exception_t reduceZombie(cte_t *تھاں, bool_t فوری)
{
    cte_t *ptr;
    لفظ_t n, type;
    exception_t حالت;

    أكد(cap_get_capType(تھاں->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(تھاں->cap);
    n = cap_zombie_cap_get_cap_Zombie_نمبر(تھاں->cap);
    type = cap_zombie_cap_get_capZombieType(تھاں->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    إذا (فوری) {
        cte_t *سرا_تھاں = &ptr[n - 1];

        حالت = cteDelete(سرا_تھاں, false);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        بدل (cap_get_capType(تھاں->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(تھاں->cap);

            إذا (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_نمبر(تھاں->cap) == n &&
                cap_zombie_cap_get_capZombieType(تھاں->cap) == type) {
                أكد(cap_get_capType(سرا_تھاں->cap) == cap_null_cap);
                تھاں->cap =
                    cap_zombie_cap_سیٹ_cap_Zombie_نمبر(تھاں->cap, n - 1);
            } وإلا {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == تھاں && ptr != تھاں);
            }
            اكسر;
        }

        افتراضي:
            fail("Expected recursion to result in Zombie.");
        }
    } وإلا {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != تھاں);

        إذا (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_وٹاندرا_For_Delete(ptr, تھاں);
    }
    أرجع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *تھاں)
{
    لفظ_t cap_type = cap_get_capType(تھاں->cap);
    إذا (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret بن_ورتیا;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(تھاں);
        fc_ret = finaliseCap(تھاں->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, تھاں) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        خالی_تھاواں_تھاں(تھاں, cap_null_cap_new());
    }
}

فراغ داخل_کرو_New_Cap(cte_t *ماپا, cte_t *تھاں, cap_t cap)
{
    cte_t *اگلا;

    اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(ماپا->cteMDBNode));
    تھاں->cap = cap;
    تھاں->cteMDBNode = mdb_گرہ_new(CTE_REF(اگلا), true, true, CTE_REF(ماپا));
    إذا (اگلا) {
        mdb_گرہ_ptr_سیٹ_mdb_Prev(&اگلا->cteMDBNode, CTE_REF(تھاں));
    }
    mdb_گرہ_ptr_سیٹ_mdb_اگلا(&ماپا->cteMDBNode, CTE_REF(تھاں));
}

#جے_بنا_تعریف CONFIG_KERNEL_MCS
فراغ تیاری_جواب_Master(tcb_t *thread)
{
    cte_t *تھاں;

    تھاں = TCB_PTR_CTE_PTR(thread, tcb_جواب);
    إذا (cap_get_capType(تھاں->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        تھاں->cap = cap_جواب_cap_new(true, true, TCB_REF(thread));
        تھاں->cteMDBNode = nullMDBNode;
        mdb_گرہ_ptr_سیٹ_mdb_Revocable(&تھاں->cteMDBNode, true);
        mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&تھاں->cteMDBNode, true);
    }
}
#جے_انت

bool_t خالص is_MDBParent_دا(cte_t *cte_a, cte_t *cte_b)
{
    إذا (!mdb_گرہ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        أرجع false;
    }
    إذا (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        أرجع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    إذا (!Arch_is_MDBParent_دا(cte_a->cap, cte_b->cap,
                            mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode))) {
        أرجع false;
    }
    بدل (cap_get_capType(cte_a->cap)) {
    حالة cap_endpoint_cap: {
        لفظ_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        لفظ_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_گرہ_get_mdb_پہلا_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        أرجع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *تھاں)
{
    إذا (mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode) != 0) {
        cte_t *اگلا;

        اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(تھاں->cteMDBNode));
        إذا (is_MDBParent_دا(تھاں, اگلا)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t ensure_خالی_تھاواں_تھاں(cte_t *تھاں)
{
    إذا (cap_get_capType(تھاں->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

bool_t خالص isFinalCapability(cte_t *cte)
{
    mdb_گرہ_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    إذا (mdb_گرہ_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_گرہ_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    إذا (prevIsSameObject) {
        أرجع false;
    } وإلا {
        إذا (mdb_گرہ_get_mdb_اگلا(mdb) == 0) {
            أرجع true;
        } وإلا {
            cte_t *اگلا;

            اگلا = CTE_PTR(mdb_گرہ_get_mdb_اگلا(mdb));
            أرجع !sameObjectAs(cte->cap, اگلا->cap);
        }
    }
}

bool_t خالص تھاں_Cap_Long_Running_Delete(cte_t *تھاں)
{
    إذا (cap_get_capType(تھاں->cap) == cap_null_cap) {
        أرجع false;
    } وإلا إذا (! isFinalCapability(تھاں)) {
        أرجع false;
    }
    بدل (cap_get_capType(تھاں->cap)) {
    حالة cap_thread_cap:
    حالة cap_zombie_cap:
    حالة cap_cnode_cap:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, لفظ_t *بفر)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_تھاں_ret_t lus_ret;
    cte_t *تھاں;
    cap_t cnode;

    إذا (!بفر) {
        أرجع عدم;
    }

    ct = لدو_Cap_Transfer(بفر);
    cptr = ct.ct_Receive_جڑ;

    luc_ret = lookupCap(thread, cptr);
    إذا (luc_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_نشانہ_تھاں(cnode, ct.ct_Receive_اشاریہ, ct.ct_Receive_ڈونگھائی);
    إذا (lus_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    تھاں = lus_ret.تھاں;

    إذا (cap_get_capType(تھاں->cap) != cap_null_cap) {
        أرجع عدم;
    }

    أرجع تھاں;
}

cap_transfer_t خالص لدو_Cap_Transfer(لفظ_t *بفر)
{
    ثابت صحيح وتھ = se_L4_پیغام_Max_لمائی + se_L4_پیغام_Max_Extra_Caps + 2;
    أرجع cap_Transfer_From_الفاظ(بفر + وتھ);
}
