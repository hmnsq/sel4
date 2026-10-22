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

بنية finalise_خونه_ret {
    exception_t حالت;
    bool_t success;
    cap_t cleanupInfo;
};
تعريفنوع بنية finalise_خونه_ret finalise_خونه_ret_t;

ساكن finalise_خونه_ret_t finalise_خونه(cte_t *خونه, bool_t exposed);
ساكن فراغ تشه_خونه(cte_t *خونه, cap_t cleanupInfo);
ساكن exception_t reduceZombie(cte_t *خونه, bool_t exposed);

#که_تعریف_شوی CONFIG_KERNEL_MCS
#تعریف_کړه CNODE_وروستی_INVOCATION CNodeRotate
#وإلا
#تعریف_کړه CNODE_وروستی_INVOCATION CNodeSaveCaller
#د_که_پای

exception_t decodeCNodeInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cap_t cap,
                                  کلمه_t *بفر)
{
    lookup_خونه_ret_t lu_ret;
    cte_t *dest_خونه;
    کلمه_t شاخص, w_بټونه;
    exception_t حالت;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    إذا (invLabel < CNodeRevoke || invLabel > CNODE_وروستی_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, بفر);
    w_بټونه = getSyscallArg(1, بفر);

    lu_ret = lookup_موخه_خونه(cap, شاخص, w_بټونه);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        أرجع lu_ret.حالت;
    }
    dest_خونه = lu_ret.خونه;

    إذا (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_خونه;
        کلمه_t src_شاخص, src_ژوروالی, cap_معلومات;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ریښه, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        إذا (اوږدتیا < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        src_شاخص = getSyscallArg(2, بفر);
        src_ژوروالی = getSyscallArg(3, بفر);

        src_ریښه = current_extra_caps.excaprefs[0]->cap;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            أرجع حالت;
        }

        lu_ret = lookup_سرچینه_خونه(src_ریښه, src_شاخص, src_ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            أرجع lu_ret.حالت;
        }
        src_خونه = lu_ret.خونه;

        إذا (cap_get_capType(src_خونه->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_ژوروالی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            إذا (اوږدتیا < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_کلمه(getSyscallArg(4, بفر));
            srcCap = ماسک_Cap_Rights(cap_rights, src_خونه->cap);
            dc_ret = deriveCap(src_خونه, srcCap);
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            إذا (اوږدتیا < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_کلمه(getSyscallArg(4, بفر));
            cap_معلومات = getSyscallArg(5, بفر);
            srcCap = ماسک_Cap_Rights(cap_rights, src_خونه->cap);
            dc_ret = deriveCap(src_خونه,
                               update_Cap_معلومات(false, cap_معلومات, srcCap));
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = src_خونه->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            إذا (اوږدتیا < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_معلومات = getSyscallArg(4, بفر);
            newCap = update_Cap_معلومات(true, cap_معلومات, src_خونه->cap);
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

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        إذا (isMove) {
            أرجع invokeCNodeMove(newCap, src_خونه, dest_خونه);
        } وإلا {
            أرجع invoke_CNode_ورننباسه(newCap, src_خونه, dest_خونه);
        }
    }

    إذا (invLabel == CNodeRevoke) {
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRevoke(dest_خونه);
    }

    إذا (invLabel == CNodeDelete) {
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeDelete(dest_خونه);
    }

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (invLabel == CNodeSaveCaller) {
        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeSaveCaller(dest_خونه);
    }
#د_که_پای

    إذا (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_خونه->cap;

        إذا (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeCancelBadgedSends(destCap);
    }

    إذا (invLabel == CNodeRotate) {
        کلمه_t محور_New_معلومات, محور_شاخص, محور_ژوروالی;
        کلمه_t src_New_معلومات, src_شاخص, src_ژوروالی;
        cte_t *محور_خونه, *src_خونه;
        cap_t محور_ریښه, src_ریښه, newSrcCap, new_محور_Cap;

        إذا (اوږدتیا < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        محور_New_معلومات = getSyscallArg(2, بفر);
        محور_شاخص   = getSyscallArg(3, بفر);
        محور_ژوروالی   = getSyscallArg(4, بفر);
        src_New_معلومات   = getSyscallArg(5, بفر);
        src_شاخص     = getSyscallArg(6, بفر);
        src_ژوروالی     = getSyscallArg(7, بفر);

        محور_ریښه = current_extra_caps.excaprefs[0]->cap;
        src_ریښه   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_سرچینه_خونه(src_ریښه, src_شاخص, src_ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        src_خونه = lu_ret.خونه;

        lu_ret = lookup_محور_خونه(محور_ریښه, محور_شاخص, محور_ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        محور_خونه = lu_ret.خونه;

        إذا (محور_خونه == src_خونه || محور_خونه == dest_خونه) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (src_خونه != dest_خونه) {
            حالت = ensure_تشه_خونه(dest_خونه);
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }

        إذا (cap_get_capType(src_خونه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_ژوروالی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(محور_خونه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(محور_ژوروالی);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_معلومات(true, src_New_معلومات, src_خونه->cap);
        new_محور_Cap = update_Cap_معلومات(true, محور_New_معلومات, محور_خونه->cap);

        إذا (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(new_محور_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRotate(newSrcCap, new_محور_Cap,
                                 src_خونه, محور_خونه, dest_خونه);
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_خونه)
{
    أرجع cteRevoke(dest_خونه);
}

exception_t invokeCNodeDelete(cte_t *dest_خونه)
{
    أرجع cteDelete(dest_خونه, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    کلمه_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    إذا (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    أرجع EXCEPTION_NONE;
}

exception_t invoke_CNode_ورننباسه(cap_t cap, cte_t *src_خونه, cte_t *dest_خونه)
{
    cte_ورننباسه(cap, src_خونه, dest_خونه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_خونه, cte_t *dest_خونه)
{
    cteMove(cap, src_خونه, dest_خونه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    إذا (slot1 == slot3) {
        cte_بدلون(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    أرجع EXCEPTION_NONE;
}

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_خونه)
{
    cap_t cap;
    cte_t *src_خونه;

    src_خونه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbCaller);
    cap = src_خونه->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_ځواب_cap:
        إذا (!cap_ځواب_cap_get_cap_ځواب_Master(cap)) {
            cteMove(cap, src_خونه, dest_خونه);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}
#د_که_پای

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ سېټ_Untyped_Cap_As_ډک(cap_t srcCap, cap_t newCap, cte_t *src_خونه)
{
    إذا ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        إذا ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_بلاک_اندازه(newCap)
                == cap_untyped_cap_get_cap_بلاک_اندازه(srcCap))) {
            cap_untyped_cap_ptr_سېټ_cap_Free_شاخص(&(src_خونه->cap),
                                                 MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلاک_اندازه(srcCap)));
        }
    }
}

فراغ cte_ورننباسه(cap_t newCap, cte_t *src_خونه, cte_t *dest_خونه)
{
    mdb_غوټه_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_خونه->cteMDBNode;
    srcCap = src_خونه->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_غوټه_سېټ_mdb_Prev(srcMDB, CTE_REF(src_خونه));
    newMDB = mdb_غوټه_سېټ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_غوټه_سېټ_mdb_لومړی_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(dest_خونه->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_غوټه_get_mdb_بل(dest_خونه->cteMDBNode) == عدم &&
           (cte_t *)mdb_غوټه_get_mdb_Prev(dest_خونه->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    سېټ_Untyped_Cap_As_ډک(srcCap, newCap, src_خونه);

    dest_خونه->cap = newCap;
    dest_خونه->cteMDBNode = newMDB;
    mdb_غوټه_ptr_سېټ_mdb_بل(&src_خونه->cteMDBNode, CTE_REF(dest_خونه));
    إذا (mdb_غوټه_get_mdb_بل(newMDB)) {
        mdb_غوټه_ptr_سېټ_mdb_Prev(
            &CTE_PTR(mdb_غوټه_get_mdb_بل(newMDB))->cteMDBNode,
            CTE_REF(dest_خونه));
    }
}

فراغ cteMove(cap_t newCap, cte_t *src_خونه, cte_t *dest_خونه)
{
    mdb_غوټه_t mdb;
    کلمه_t prev_ptr, بل_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(dest_خونه->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_غوټه_get_mdb_بل(dest_خونه->cteMDBNode) == عدم &&
           (cte_t *)mdb_غوټه_get_mdb_Prev(dest_خونه->cteMDBNode) == عدم);

    mdb = src_خونه->cteMDBNode;
    dest_خونه->cap = newCap;
    src_خونه->cap = cap_null_cap_new();
    dest_خونه->cteMDBNode = mdb;
    src_خونه->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_غوټه_get_mdb_Prev(mdb);
    إذا (prev_ptr)
        mdb_غوټه_ptr_سېټ_mdb_بل(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_خونه));

    بل_ptr = mdb_غوټه_get_mdb_بل(mdb);
    إذا (بل_ptr)
        mdb_غوټه_ptr_سېټ_mdb_Prev(
            &CTE_PTR(بل_ptr)->cteMDBNode,
            CTE_REF(dest_خونه));
}

فراغ cap_بدلون_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    إذا (slot1 == slot2) {
        أرجع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_بدلون(cap1, slot1, cap2, slot2);
}

فراغ cte_بدلون(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_غوټه_t mdb1, mdb2;
    کلمه_t بل_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_غوټه_get_mdb_Prev(mdb1);
    إذا (prev_ptr)
        mdb_غوټه_ptr_سېټ_mdb_بل(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    بل_ptr = mdb_غوټه_get_mdb_بل(mdb1);
    إذا (بل_ptr)
        mdb_غوټه_ptr_سېټ_mdb_Prev(
            &CTE_PTR(بل_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_غوټه_get_mdb_Prev(mdb2);
    إذا (prev_ptr)
        mdb_غوټه_ptr_سېټ_mdb_بل(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    بل_ptr = mdb_غوټه_get_mdb_بل(mdb2);
    إذا (بل_ptr)
        mdb_غوټه_ptr_سېټ_mdb_Prev(
            &CTE_PTR(بل_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *خونه)
{
    cte_t *بل_Ptr;
    exception_t حالت;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (بل_Ptr = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode));
         بل_Ptr && is_MDBParent_د(خونه, بل_Ptr);
         بل_Ptr = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode))) {
        حالت = cteDelete(بل_Ptr, true);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        حالت = preemption_ټکی();
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *خونه, bool_t exposed)
{
    finalise_خونه_ret_t fs_ret;

    fs_ret = finalise_خونه(خونه, exposed);
    إذا (fs_ret.حالت != EXCEPTION_NONE) {
        أرجع fs_ret.حالت;
    }

    إذا (exposed || fs_ret.success) {
        تشه_خونه(خونه, fs_ret.cleanupInfo);
    }
    أرجع EXCEPTION_NONE;
}

ساكن فراغ تشه_خونه(cte_t *خونه, cap_t cleanupInfo)
{
    إذا (cap_get_capType(خونه->cap) != cap_null_cap) {
        mdb_غوټه_t mdb_غوټه;
        cte_t *prev, *بل;

        mdb_غوټه = خونه->cteMDBNode;
        prev = CTE_PTR(mdb_غوټه_get_mdb_Prev(mdb_غوټه));
        بل = CTE_PTR(mdb_غوټه_get_mdb_بل(mdb_غوټه));

        إذا (prev) {
            mdb_غوټه_ptr_سېټ_mdb_بل(&prev->cteMDBNode, CTE_REF(بل));
        }
        إذا (بل) {
            mdb_غوټه_ptr_سېټ_mdb_Prev(&بل->cteMDBNode, CTE_REF(prev));
        }
        إذا (بل)
            mdb_غوټه_ptr_سېټ_mdb_لومړی_Badged(&بل->cteMDBNode,
                                            mdb_غوټه_get_mdb_لومړی_Badged(بل->cteMDBNode) ||
                                            mdb_غوټه_get_mdb_لومړی_Badged(mdb_غوټه));
        خونه->cap = cap_null_cap_new();
        خونه->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *خونه)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        أرجع true;
    حالة cap_zombie_cap: {
        کلمه_t n = cap_zombie_cap_get_cap_Zombie_شمېره(cap);
        cte_t *z_خونه = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        أرجع (n == 0 || (n == 1 && خونه == z_خونه));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *خونه)
{
    أرجع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == خونه;
}

ساكن finalise_خونه_ret_t finalise_خونه(cte_t *خونه, bool_t سمدستي)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t حالت;
    finalise_خونه_ret_t ret;

    طالما (cap_get_capType(خونه->cap) != cap_null_cap) {
        final = isFinalCapability(خونه);
        fc_ret = finaliseCap(خونه->cap, final, false);

        إذا (capRemovable(fc_ret.remainder, خونه)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        خونه->cap = fc_ret.remainder;

        إذا (!سمدستي && capCyclicZombie(fc_ret.remainder, خونه)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        حالت = reduceZombie(خونه, سمدستي);
        إذا (حالت != EXCEPTION_NONE) {
            ret.حالت = حالت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }

        حالت = preemption_ټکی();
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

ساكن exception_t reduceZombie(cte_t *خونه, bool_t سمدستي)
{
    cte_t *ptr;
    کلمه_t n, type;
    exception_t حالت;

    أكد(cap_get_capType(خونه->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(خونه->cap);
    n = cap_zombie_cap_get_cap_Zombie_شمېره(خونه->cap);
    type = cap_zombie_cap_get_capZombieType(خونه->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    إذا (سمدستي) {
        cte_t *پای_ټکی_خونه = &ptr[n - 1];

        حالت = cteDelete(پای_ټکی_خونه, false);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        بدل (cap_get_capType(خونه->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(خونه->cap);

            إذا (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_شمېره(خونه->cap) == n &&
                cap_zombie_cap_get_capZombieType(خونه->cap) == type) {
                أكد(cap_get_capType(پای_ټکی_خونه->cap) == cap_null_cap);
                خونه->cap =
                    cap_zombie_cap_سېټ_cap_Zombie_شمېره(خونه->cap, n - 1);
            } وإلا {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == خونه && ptr != خونه);
            }
            اكسر;
        }

        افتراضي:
            fail("Expected recursion to result in Zombie.");
        }
    } وإلا {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != خونه);

        إذا (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_بدلون_For_Delete(ptr, خونه);
    }
    أرجع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *خونه)
{
    کلمه_t cap_type = cap_get_capType(خونه->cap);
    إذا (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret ناکارول_شوی;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(خونه);
        fc_ret = finaliseCap(خونه->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, خونه) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        تشه_خونه(خونه, cap_null_cap_new());
    }
}

فراغ ورننباسه_New_Cap(cte_t *پلار, cte_t *خونه, cap_t cap)
{
    cte_t *بل;

    بل = CTE_PTR(mdb_غوټه_get_mdb_بل(پلار->cteMDBNode));
    خونه->cap = cap;
    خونه->cteMDBNode = mdb_غوټه_new(CTE_REF(بل), true, true, CTE_REF(پلار));
    إذا (بل) {
        mdb_غوټه_ptr_سېټ_mdb_Prev(&بل->cteMDBNode, CTE_REF(خونه));
    }
    mdb_غوټه_ptr_سېټ_mdb_بل(&پلار->cteMDBNode, CTE_REF(خونه));
}

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
فراغ تیاری_ځواب_Master(tcb_t *thread)
{
    cte_t *خونه;

    خونه = TCB_PTR_CTE_PTR(thread, tcb_ځواب);
    إذا (cap_get_capType(خونه->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        خونه->cap = cap_ځواب_cap_new(true, true, TCB_REF(thread));
        خونه->cteMDBNode = nullMDBNode;
        mdb_غوټه_ptr_سېټ_mdb_Revocable(&خونه->cteMDBNode, true);
        mdb_غوټه_ptr_سېټ_mdb_لومړی_Badged(&خونه->cteMDBNode, true);
    }
}
#د_که_پای

bool_t خالص is_MDBParent_د(cte_t *cte_a, cte_t *cte_b)
{
    إذا (!mdb_غوټه_get_mdb_Revocable(cte_a->cteMDBNode)) {
        أرجع false;
    }
    إذا (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        أرجع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    إذا (!Arch_is_MDBParent_د(cte_a->cap, cte_b->cap,
                            mdb_غوټه_get_mdb_لومړی_Badged(cte_b->cteMDBNode))) {
        أرجع false;
    }
    بدل (cap_get_capType(cte_a->cap)) {
    حالة cap_endpoint_cap: {
        کلمه_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_غوټه_get_mdb_لومړی_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        کلمه_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_غوټه_get_mdb_لومړی_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        أرجع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *خونه)
{
    إذا (mdb_غوټه_get_mdb_بل(خونه->cteMDBNode) != 0) {
        cte_t *بل;

        بل = CTE_PTR(mdb_غوټه_get_mdb_بل(خونه->cteMDBNode));
        إذا (is_MDBParent_د(خونه, بل)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t ensure_تشه_خونه(cte_t *خونه)
{
    إذا (cap_get_capType(خونه->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_لومړی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

bool_t خالص isFinalCapability(cte_t *cte)
{
    mdb_غوټه_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    إذا (mdb_غوټه_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_غوټه_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    إذا (prevIsSameObject) {
        أرجع false;
    } وإلا {
        إذا (mdb_غوټه_get_mdb_بل(mdb) == 0) {
            أرجع true;
        } وإلا {
            cte_t *بل;

            بل = CTE_PTR(mdb_غوټه_get_mdb_بل(mdb));
            أرجع !sameObjectAs(cte->cap, بل->cap);
        }
    }
}

bool_t خالص خونه_Cap_Long_Running_Delete(cte_t *خونه)
{
    إذا (cap_get_capType(خونه->cap) == cap_null_cap) {
        أرجع false;
    } وإلا إذا (! isFinalCapability(خونه)) {
        أرجع false;
    }
    بدل (cap_get_capType(خونه->cap)) {
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
cte_t *getReceiveSlots(tcb_t *thread, کلمه_t *بفر)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_خونه_ret_t lus_ret;
    cte_t *خونه;
    cap_t cnode;

    إذا (!بفر) {
        أرجع عدم;
    }

    ct = بارول_Cap_Transfer(بفر);
    cptr = ct.ct_Receive_ریښه;

    luc_ret = lookupCap(thread, cptr);
    إذا (luc_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_موخه_خونه(cnode, ct.ct_Receive_شاخص, ct.ct_Receive_ژوروالی);
    إذا (lus_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    خونه = lus_ret.خونه;

    إذا (cap_get_capType(خونه->cap) != cap_null_cap) {
        أرجع عدم;
    }

    أرجع خونه;
}

cap_transfer_t خالص بارول_Cap_Transfer(کلمه_t *بفر)
{
    ثابت صحيح دننه_واټن = se_L4_پیغام_Max_اوږدتیا + se_L4_پیغام_Max_Extra_Caps + 2;
    أرجع cap_Transfer_From_ټکي(بفر + دننه_واټن);
}
