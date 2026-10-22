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

بنية finalise_جڳهه_ret {
    exception_t حالت;
    bool_t success;
    cap_t cleanupInfo;
};
تعريفنوع بنية finalise_جڳهه_ret finalise_جڳهه_ret_t;

ساكن finalise_جڳهه_ret_t finalise_جڳهه(cte_t *جڳهه, bool_t exposed);
ساكن فراغ خالي_جايون_جڳهه(cte_t *جڳهه, cap_t cleanupInfo);
ساكن exception_t reduceZombie(cte_t *جڳهه, bool_t exposed);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
#وصف_ڪريو CNODE_آخري_INVOCATION CNodeRotate
#وإلا
#وصف_ڪريو CNODE_آخري_INVOCATION CNodeSaveCaller
#جيڪڏهن_پورو

exception_t decodeCNodeInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cap_t cap,
                                  لفظ_t *بفر)
{
    lookup_جڳهه_ret_t lu_ret;
    cte_t *dest_جڳهه;
    لفظ_t اشاريو, w_بٽ;
    exception_t حالت;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    إذا (invLabel < CNodeRevoke || invLabel > CNODE_آخري_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    اشاريو = getSyscallArg(0, بفر);
    w_بٽ = getSyscallArg(1, بفر);

    lu_ret = lookup_هدف_جڳهه(cap, اشاريو, w_بٽ);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        أرجع lu_ret.حالت;
    }
    dest_جڳهه = lu_ret.جڳهه;

    إذا (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_جڳهه;
        لفظ_t src_اشاريو, src_اونهائي, cap_معلومات;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_پاڙ, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        إذا (ڊيگهائي < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        src_اشاريو = getSyscallArg(2, بفر);
        src_اونهائي = getSyscallArg(3, بفر);

        src_پاڙ = current_extra_caps.excaprefs[0]->cap;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            أرجع حالت;
        }

        lu_ret = lookup_ذريعو_جڳهه(src_پاڙ, src_اشاريو, src_اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            أرجع lu_ret.حالت;
        }
        src_جڳهه = lu_ret.جڳهه;

        إذا (cap_get_capType(src_جڳهه->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_اونهائي);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            إذا (ڊيگهائي < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_پيغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            srcCap = نقاب_Cap_Rights(cap_rights, src_جڳهه->cap);
            dc_ret = deriveCap(src_جڳهه, srcCap);
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            إذا (ڊيگهائي < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پيغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_لفظ(getSyscallArg(4, بفر));
            cap_معلومات = getSyscallArg(5, بفر);
            srcCap = نقاب_Cap_Rights(cap_rights, src_جڳهه->cap);
            dc_ret = deriveCap(src_جڳهه,
                               update_Cap_معلومات(false, cap_معلومات, srcCap));
            إذا (dc_ret.حالت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                أرجع dc_ret.حالت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = src_جڳهه->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            إذا (ڊيگهائي < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پيغام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_معلومات = getSyscallArg(4, بفر);
            newCap = update_Cap_معلومات(true, cap_معلومات, src_جڳهه->cap);
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

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        إذا (isMove) {
            أرجع invokeCNodeMove(newCap, src_جڳهه, dest_جڳهه);
        } وإلا {
            أرجع invoke_CNode_وجهو(newCap, src_جڳهه, dest_جڳهه);
        }
    }

    إذا (invLabel == CNodeRevoke) {
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRevoke(dest_جڳهه);
    }

    إذا (invLabel == CNodeDelete) {
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeDelete(dest_جڳهه);
    }

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    إذا (invLabel == CNodeSaveCaller) {
        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeSaveCaller(dest_جڳهه);
    }
#جيڪڏهن_پورو

    إذا (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_جڳهه->cap;

        إذا (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeCancelBadgedSends(destCap);
    }

    إذا (invLabel == CNodeRotate) {
        لفظ_t محور_New_معلومات, محور_اشاريو, محور_اونهائي;
        لفظ_t src_New_معلومات, src_اشاريو, src_اونهائي;
        cte_t *محور_جڳهه, *src_جڳهه;
        cap_t محور_پاڙ, src_پاڙ, newSrcCap, new_محور_Cap;

        إذا (ڊيگهائي < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        محور_New_معلومات = getSyscallArg(2, بفر);
        محور_اشاريو   = getSyscallArg(3, بفر);
        محور_اونهائي   = getSyscallArg(4, بفر);
        src_New_معلومات   = getSyscallArg(5, بفر);
        src_اشاريو     = getSyscallArg(6, بفر);
        src_اونهائي     = getSyscallArg(7, بفر);

        محور_پاڙ = current_extra_caps.excaprefs[0]->cap;
        src_پاڙ   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_ذريعو_جڳهه(src_پاڙ, src_اشاريو, src_اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        src_جڳهه = lu_ret.جڳهه;

        lu_ret = lookup_محور_جڳهه(محور_پاڙ, محور_اشاريو, محور_اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        محور_جڳهه = lu_ret.جڳهه;

        إذا (محور_جڳهه == src_جڳهه || محور_جڳهه == dest_جڳهه) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (src_جڳهه != dest_جڳهه) {
            حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }

        إذا (cap_get_capType(src_جڳهه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_اونهائي);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(محور_جڳهه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(محور_اونهائي);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_معلومات(true, src_New_معلومات, src_جڳهه->cap);
        new_محور_Cap = update_Cap_معلومات(true, محور_New_معلومات, محور_جڳهه->cap);

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

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRotate(newSrcCap, new_محور_Cap,
                                 src_جڳهه, محور_جڳهه, dest_جڳهه);
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_جڳهه)
{
    أرجع cteRevoke(dest_جڳهه);
}

exception_t invokeCNodeDelete(cte_t *dest_جڳهه)
{
    أرجع cteDelete(dest_جڳهه, true);
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

exception_t invoke_CNode_وجهو(cap_t cap, cte_t *src_جڳهه, cte_t *dest_جڳهه)
{
    cte_وجهو(cap, src_جڳهه, dest_جڳهه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_جڳهه, cte_t *dest_جڳهه)
{
    cteMove(cap, src_جڳهه, dest_جڳهه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    إذا (slot1 == slot3) {
        cte_مٽا_سٽا(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    أرجع EXCEPTION_NONE;
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_جڳهه)
{
    cap_t cap;
    cte_t *src_جڳهه;

    src_جڳهه = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbCaller);
    cap = src_جڳهه->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_جواب_cap:
        إذا (!cap_جواب_cap_get_cap_جواب_Master(cap)) {
            cteMove(cap, src_جڳهه, dest_جڳهه);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}
#جيڪڏهن_پورو

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ سيٽ_Untyped_Cap_As_ڀريل(cap_t srcCap, cap_t newCap, cte_t *src_جڳهه)
{
    إذا ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        إذا ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_بلاڪ_سائيز(newCap)
                == cap_untyped_cap_get_cap_بلاڪ_سائيز(srcCap))) {
            cap_untyped_cap_ptr_سيٽ_cap_Free_اشاريو(&(src_جڳهه->cap),
                                                 MAX_FREE_اشاريو(cap_untyped_cap_get_cap_بلاڪ_سائيز(srcCap)));
        }
    }
}

فراغ cte_وجهو(cap_t newCap, cte_t *src_جڳهه, cte_t *dest_جڳهه)
{
    mdb_ڳنڍ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_جڳهه->cteMDBNode;
    srcCap = src_جڳهه->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ڳنڍ_سيٽ_mdb_Prev(srcMDB, CTE_REF(src_جڳهه));
    newMDB = mdb_ڳنڍ_سيٽ_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ڳنڍ_سيٽ_mdb_پهريون_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(dest_جڳهه->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_ڳنڍ_get_mdb_ايندڙ(dest_جڳهه->cteMDBNode) == عدم &&
           (cte_t *)mdb_ڳنڍ_get_mdb_Prev(dest_جڳهه->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    سيٽ_Untyped_Cap_As_ڀريل(srcCap, newCap, src_جڳهه);

    dest_جڳهه->cap = newCap;
    dest_جڳهه->cteMDBNode = newMDB;
    mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(&src_جڳهه->cteMDBNode, CTE_REF(dest_جڳهه));
    إذا (mdb_ڳنڍ_get_mdb_ايندڙ(newMDB)) {
        mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(
            &CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(newMDB))->cteMDBNode,
            CTE_REF(dest_جڳهه));
    }
}

فراغ cteMove(cap_t newCap, cte_t *src_جڳهه, cte_t *dest_جڳهه)
{
    mdb_ڳنڍ_t mdb;
    لفظ_t prev_ptr, ايندڙ_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(dest_جڳهه->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_ڳنڍ_get_mdb_ايندڙ(dest_جڳهه->cteMDBNode) == عدم &&
           (cte_t *)mdb_ڳنڍ_get_mdb_Prev(dest_جڳهه->cteMDBNode) == عدم);

    mdb = src_جڳهه->cteMDBNode;
    dest_جڳهه->cap = newCap;
    src_جڳهه->cap = cap_null_cap_new();
    dest_جڳهه->cteMDBNode = mdb;
    src_جڳهه->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_ڳنڍ_get_mdb_Prev(mdb);
    إذا (prev_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_جڳهه));

    ايندڙ_ptr = mdb_ڳنڍ_get_mdb_ايندڙ(mdb);
    إذا (ايندڙ_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(
            &CTE_PTR(ايندڙ_ptr)->cteMDBNode,
            CTE_REF(dest_جڳهه));
}

فراغ cap_مٽا_سٽا_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    إذا (slot1 == slot2) {
        أرجع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_مٽا_سٽا(cap1, slot1, cap2, slot2);
}

فراغ cte_مٽا_سٽا(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ڳنڍ_t mdb1, mdb2;
    لفظ_t ايندڙ_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_ڳنڍ_get_mdb_Prev(mdb1);
    إذا (prev_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    ايندڙ_ptr = mdb_ڳنڍ_get_mdb_ايندڙ(mdb1);
    إذا (ايندڙ_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(
            &CTE_PTR(ايندڙ_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_ڳنڍ_get_mdb_Prev(mdb2);
    إذا (prev_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    ايندڙ_ptr = mdb_ڳنڍ_get_mdb_ايندڙ(mdb2);
    إذا (ايندڙ_ptr)
        mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(
            &CTE_PTR(ايندڙ_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *جڳهه)
{
    cte_t *ايندڙ_Ptr;
    exception_t حالت;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (ايندڙ_Ptr = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode));
         ايندڙ_Ptr && is_MDBParent_جو(جڳهه, ايندڙ_Ptr);
         ايندڙ_Ptr = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode))) {
        حالت = cteDelete(ايندڙ_Ptr, true);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        حالت = preemption_نقطو();
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *جڳهه, bool_t exposed)
{
    finalise_جڳهه_ret_t fs_ret;

    fs_ret = finalise_جڳهه(جڳهه, exposed);
    إذا (fs_ret.حالت != EXCEPTION_NONE) {
        أرجع fs_ret.حالت;
    }

    إذا (exposed || fs_ret.success) {
        خالي_جايون_جڳهه(جڳهه, fs_ret.cleanupInfo);
    }
    أرجع EXCEPTION_NONE;
}

ساكن فراغ خالي_جايون_جڳهه(cte_t *جڳهه, cap_t cleanupInfo)
{
    إذا (cap_get_capType(جڳهه->cap) != cap_null_cap) {
        mdb_ڳنڍ_t mdb_ڳنڍ;
        cte_t *prev, *ايندڙ;

        mdb_ڳنڍ = جڳهه->cteMDBNode;
        prev = CTE_PTR(mdb_ڳنڍ_get_mdb_Prev(mdb_ڳنڍ));
        ايندڙ = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(mdb_ڳنڍ));

        إذا (prev) {
            mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(&prev->cteMDBNode, CTE_REF(ايندڙ));
        }
        إذا (ايندڙ) {
            mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(&ايندڙ->cteMDBNode, CTE_REF(prev));
        }
        إذا (ايندڙ)
            mdb_ڳنڍ_ptr_سيٽ_mdb_پهريون_Badged(&ايندڙ->cteMDBNode,
                                            mdb_ڳنڍ_get_mdb_پهريون_Badged(ايندڙ->cteMDBNode) ||
                                            mdb_ڳنڍ_get_mdb_پهريون_Badged(mdb_ڳنڍ));
        جڳهه->cap = cap_null_cap_new();
        جڳهه->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *جڳهه)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        أرجع true;
    حالة cap_zombie_cap: {
        لفظ_t n = cap_zombie_cap_get_cap_Zombie_عدد(cap);
        cte_t *z_جڳهه = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        أرجع (n == 0 || (n == 1 && جڳهه == z_جڳهه));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *جڳهه)
{
    أرجع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == جڳهه;
}

ساكن finalise_جڳهه_ret_t finalise_جڳهه(cte_t *جڳهه, bool_t فوري)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t حالت;
    finalise_جڳهه_ret_t ret;

    طالما (cap_get_capType(جڳهه->cap) != cap_null_cap) {
        final = isFinalCapability(جڳهه);
        fc_ret = finaliseCap(جڳهه->cap, final, false);

        إذا (capRemovable(fc_ret.remainder, جڳهه)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        جڳهه->cap = fc_ret.remainder;

        إذا (!فوري && capCyclicZombie(fc_ret.remainder, جڳهه)) {
            ret.حالت = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        حالت = reduceZombie(جڳهه, فوري);
        إذا (حالت != EXCEPTION_NONE) {
            ret.حالت = حالت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }

        حالت = preemption_نقطو();
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

ساكن exception_t reduceZombie(cte_t *جڳهه, bool_t فوري)
{
    cte_t *ptr;
    لفظ_t n, type;
    exception_t حالت;

    أكد(cap_get_capType(جڳهه->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(جڳهه->cap);
    n = cap_zombie_cap_get_cap_Zombie_عدد(جڳهه->cap);
    type = cap_zombie_cap_get_capZombieType(جڳهه->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    إذا (فوري) {
        cte_t *پڇاڙي_جڳهه = &ptr[n - 1];

        حالت = cteDelete(پڇاڙي_جڳهه, false);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        بدل (cap_get_capType(جڳهه->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(جڳهه->cap);

            إذا (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_عدد(جڳهه->cap) == n &&
                cap_zombie_cap_get_capZombieType(جڳهه->cap) == type) {
                أكد(cap_get_capType(پڇاڙي_جڳهه->cap) == cap_null_cap);
                جڳهه->cap =
                    cap_zombie_cap_سيٽ_cap_Zombie_عدد(جڳهه->cap, n - 1);
            } وإلا {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == جڳهه && ptr != جڳهه);
            }
            اكسر;
        }

        افتراضي:
            fail("Expected recursion to result in Zombie.");
        }
    } وإلا {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != جڳهه);

        إذا (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_مٽا_سٽا_For_Delete(ptr, جڳهه);
    }
    أرجع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *جڳهه)
{
    لفظ_t cap_type = cap_get_capType(جڳهه->cap);
    إذا (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret اڻ_استعمال_ٿيل;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(جڳهه);
        fc_ret = finaliseCap(جڳهه->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, جڳهه) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        خالي_جايون_جڳهه(جڳهه, cap_null_cap_new());
    }
}

فراغ وجهو_New_Cap(cte_t *والد, cte_t *جڳهه, cap_t cap)
{
    cte_t *ايندڙ;

    ايندڙ = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(والد->cteMDBNode));
    جڳهه->cap = cap;
    جڳهه->cteMDBNode = mdb_ڳنڍ_new(CTE_REF(ايندڙ), true, true, CTE_REF(والد));
    إذا (ايندڙ) {
        mdb_ڳنڍ_ptr_سيٽ_mdb_Prev(&ايندڙ->cteMDBNode, CTE_REF(جڳهه));
    }
    mdb_ڳنڍ_ptr_سيٽ_mdb_ايندڙ(&والد->cteMDBNode, CTE_REF(جڳهه));
}

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
فراغ تياري_جواب_Master(tcb_t *thread)
{
    cte_t *جڳهه;

    جڳهه = TCB_PTR_CTE_PTR(thread, tcb_جواب);
    إذا (cap_get_capType(جڳهه->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        جڳهه->cap = cap_جواب_cap_new(true, true, TCB_REF(thread));
        جڳهه->cteMDBNode = nullMDBNode;
        mdb_ڳنڍ_ptr_سيٽ_mdb_Revocable(&جڳهه->cteMDBNode, true);
        mdb_ڳنڍ_ptr_سيٽ_mdb_پهريون_Badged(&جڳهه->cteMDBNode, true);
    }
}
#جيڪڏهن_پورو

bool_t خالص is_MDBParent_جو(cte_t *cte_a, cte_t *cte_b)
{
    إذا (!mdb_ڳنڍ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        أرجع false;
    }
    إذا (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        أرجع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    إذا (!Arch_is_MDBParent_جو(cte_a->cap, cte_b->cap,
                            mdb_ڳنڍ_get_mdb_پهريون_Badged(cte_b->cteMDBNode))) {
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
               !mdb_ڳنڍ_get_mdb_پهريون_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        لفظ_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ڳنڍ_get_mdb_پهريون_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        أرجع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *جڳهه)
{
    إذا (mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode) != 0) {
        cte_t *ايندڙ;

        ايندڙ = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(جڳهه->cteMDBNode));
        إذا (is_MDBParent_جو(جڳهه, ايندڙ)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t ensure_خالي_جايون_جڳهه(cte_t *جڳهه)
{
    إذا (cap_get_capType(جڳهه->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_پهريون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

bool_t خالص isFinalCapability(cte_t *cte)
{
    mdb_ڳنڍ_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    إذا (mdb_ڳنڍ_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_ڳنڍ_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    إذا (prevIsSameObject) {
        أرجع false;
    } وإلا {
        إذا (mdb_ڳنڍ_get_mdb_ايندڙ(mdb) == 0) {
            أرجع true;
        } وإلا {
            cte_t *ايندڙ;

            ايندڙ = CTE_PTR(mdb_ڳنڍ_get_mdb_ايندڙ(mdb));
            أرجع !sameObjectAs(cte->cap, ايندڙ->cap);
        }
    }
}

bool_t خالص جڳهه_Cap_Long_Running_Delete(cte_t *جڳهه)
{
    إذا (cap_get_capType(جڳهه->cap) == cap_null_cap) {
        أرجع false;
    } وإلا إذا (! isFinalCapability(جڳهه)) {
        أرجع false;
    }
    بدل (cap_get_capType(جڳهه->cap)) {
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
    lookup_جڳهه_ret_t lus_ret;
    cte_t *جڳهه;
    cap_t cnode;

    إذا (!بفر) {
        أرجع عدم;
    }

    ct = ڀريو_Cap_Transfer(بفر);
    cptr = ct.ct_Receive_پاڙ;

    luc_ret = lookupCap(thread, cptr);
    إذا (luc_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_هدف_جڳهه(cnode, ct.ct_Receive_اشاريو, ct.ct_Receive_اونهائي);
    إذا (lus_ret.حالت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    جڳهه = lus_ret.جڳهه;

    إذا (cap_get_capType(جڳهه->cap) != cap_null_cap) {
        أرجع عدم;
    }

    أرجع جڳهه;
}

cap_transfer_t خالص ڀريو_Cap_Transfer(لفظ_t *بفر)
{
    ثابت صحيح وٿي = se_L4_نياپو_Max_ڊيگهائي + se_L4_نياپو_Max_Extra_Caps + 2;
    أرجع cap_Transfer_From_لفظ(بفر + وٿي);
}
