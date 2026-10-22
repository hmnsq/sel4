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

بنية finalise_جایگاه_ret {
    exception_t وضعیت;
    bool_t success;
    cap_t cleanupInfo;
};
تعريفنوع بنية finalise_جایگاه_ret finalise_جایگاه_ret_t;

ساكن finalise_جایگاه_ret_t finalise_جایگاه(cte_t *جایگاه, bool_t exposed);
ساكن فراغ خالی_جایگاه(cte_t *جایگاه, cap_t cleanupInfo);
ساكن exception_t reduceZombie(cte_t *جایگاه, bool_t exposed);

#اگر_تعریف_شده CONFIG_KERNEL_MCS
#تعریف_کن CNODE_آخرین_INVOCATION CNodeRotate
#وإلا
#تعریف_کن CNODE_آخرین_INVOCATION CNodeSaveCaller
#پایان_اگر

exception_t decodeCNodeInvocation(کلمه_t invLabel, کلمه_t درازی, cap_t cap,
                                  کلمه_t *میانگیر)
{
    lookup_جایگاه_ret_t lu_ret;
    cte_t *dest_جایگاه;
    کلمه_t شاخص, w_بیتها;
    exception_t وضعیت;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    أكد(cap_get_capType(cap) == cap_cnode_cap);

    إذا (invLabel < CNodeRevoke || invLabel > CNODE_آخرین_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    شاخص = getSyscallArg(0, میانگیر);
    w_بیتها = getSyscallArg(1, میانگیر);

    lu_ret = lookup_هدف_جایگاه(cap, شاخص, w_بیتها);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        أرجع lu_ret.وضعیت;
    }
    dest_جایگاه = lu_ret.جایگاه;

    إذا (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_جایگاه;
        کلمه_t src_شاخص, src_عمق, cap_داده;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_ریشه, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        إذا (درازی < 4 || current_extra_caps.excaprefs[0] == عدم) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        src_شاخص = getSyscallArg(2, میانگیر);
        src_عمق = getSyscallArg(3, میانگیر);

        src_ریشه = current_extra_caps.excaprefs[0]->cap;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            أرجع وضعیت;
        }

        lu_ret = lookup_منبع_جایگاه(src_ریشه, src_شاخص, src_عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            أرجع lu_ret.وضعیت;
        }
        src_جایگاه = lu_ret.جایگاه;

        إذا (cap_get_capType(src_جایگاه->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_عمق);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        بدل (invLabel) {
        حالة CNodeCopy:

            إذا (درازی < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_پیام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_کلمه(getSyscallArg(4, میانگیر));
            srcCap = ماسک_Cap_Rights(cap_rights, src_جایگاه->cap);
            dc_ret = deriveCap(src_جایگاه, srcCap);
            إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                أرجع dc_ret.وضعیت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMint:
            إذا (درازی < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_کلمه(getSyscallArg(4, میانگیر));
            cap_داده = getSyscallArg(5, میانگیر);
            srcCap = ماسک_Cap_Rights(cap_rights, src_جایگاه->cap);
            dc_ret = deriveCap(src_جایگاه,
                               update_Cap_داده(false, cap_داده, srcCap));
            إذا (dc_ret.وضعیت != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                أرجع dc_ret.وضعیت;
            }
            newCap = dc_ret.cap;
            isMove = false;

            اكسر;

        حالة CNodeMove:
            newCap = src_جایگاه->cap;
            isMove = true;

            اكسر;

        حالة CNodeMutate:
            إذا (درازی < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_پیام;
                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            cap_داده = getSyscallArg(4, میانگیر);
            newCap = update_Cap_داده(true, cap_داده, src_جایگاه->cap);
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

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        إذا (isMove) {
            أرجع invokeCNodeMove(newCap, src_جایگاه, dest_جایگاه);
        } وإلا {
            أرجع invoke_CNode_درج(newCap, src_جایگاه, dest_جایگاه);
        }
    }

    إذا (invLabel == CNodeRevoke) {
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRevoke(dest_جایگاه);
    }

    إذا (invLabel == CNodeDelete) {
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeDelete(dest_جایگاه);
    }

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
    إذا (invLabel == CNodeSaveCaller) {
        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeSaveCaller(dest_جایگاه);
    }
#پایان_اگر

    إذا (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_جایگاه->cap;

        إذا (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeCancelBadgedSends(destCap);
    }

    إذا (invLabel == CNodeRotate) {
        کلمه_t محور_New_داده, محور_شاخص, محور_عمق;
        کلمه_t src_New_داده, src_شاخص, src_عمق;
        cte_t *محور_جایگاه, *src_جایگاه;
        cap_t محور_ریشه, src_ریشه, newSrcCap, new_محور_Cap;

        إذا (درازی < 8 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        محور_New_داده = getSyscallArg(2, میانگیر);
        محور_شاخص   = getSyscallArg(3, میانگیر);
        محور_عمق   = getSyscallArg(4, میانگیر);
        src_New_داده   = getSyscallArg(5, میانگیر);
        src_شاخص     = getSyscallArg(6, میانگیر);
        src_عمق     = getSyscallArg(7, میانگیر);

        محور_ریشه = current_extra_caps.excaprefs[0]->cap;
        src_ریشه   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_منبع_جایگاه(src_ریشه, src_شاخص, src_عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع lu_ret.وضعیت;
        }
        src_جایگاه = lu_ret.جایگاه;

        lu_ret = lookup_محور_جایگاه(محور_ریشه, محور_شاخص, محور_عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع lu_ret.وضعیت;
        }
        محور_جایگاه = lu_ret.جایگاه;

        إذا (محور_جایگاه == src_جایگاه || محور_جایگاه == dest_جایگاه) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (src_جایگاه != dest_جایگاه) {
            وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
            إذا (وضعیت != EXCEPTION_NONE) {
                أرجع وضعیت;
            }
        }

        إذا (cap_get_capType(src_جایگاه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_عمق);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (cap_get_capType(محور_جایگاه->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(محور_عمق);
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_داده(true, src_New_داده, src_جایگاه->cap);
        new_محور_Cap = update_Cap_داده(true, محور_New_داده, محور_جایگاه->cap);

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

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع invokeCNodeRotate(newSrcCap, new_محور_Cap,
                                 src_جایگاه, محور_جایگاه, dest_جایگاه);
    }

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_جایگاه)
{
    أرجع cteRevoke(dest_جایگاه);
}

exception_t invokeCNodeDelete(cte_t *dest_جایگاه)
{
    أرجع cteDelete(dest_جایگاه, true);
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

exception_t invoke_CNode_درج(cap_t cap, cte_t *src_جایگاه, cte_t *dest_جایگاه)
{
    cte_درج(cap, src_جایگاه, dest_جایگاه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_جایگاه, cte_t *dest_جایگاه)
{
    cteMove(cap, src_جایگاه, dest_جایگاه);

    أرجع EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    إذا (slot1 == slot3) {
        cte_جابجا(cap1, slot1, cap2, slot2);
    } وإلا {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    أرجع EXCEPTION_NONE;
}

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_جایگاه)
{
    cap_t cap;
    cte_t *src_جایگاه;

    src_جایگاه = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbCaller);
    cap = src_جایگاه->cap;

    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        اكسر;

    حالة cap_پاسخ_cap:
        إذا (!cap_پاسخ_cap_get_cap_پاسخ_Master(cap)) {
            cteMove(cap, src_جایگاه, dest_جایگاه);
        }
        اكسر;

    افتراضي:
        fail("caller capability must be null or reply");
        اكسر;
    }

    أرجع EXCEPTION_NONE;
}
#پایان_اگر

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
ساكن فراغ مجموعه_Untyped_Cap_As_پر(cap_t srcCap, cap_t newCap, cte_t *src_جایگاه)
{
    إذا ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        إذا ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_بلوک_اندازه(newCap)
                == cap_untyped_cap_get_cap_بلوک_اندازه(srcCap))) {
            cap_untyped_cap_ptr_مجموعه_cap_Free_شاخص(&(src_جایگاه->cap),
                                                 MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلوک_اندازه(srcCap)));
        }
    }
}

فراغ cte_درج(cap_t newCap, cte_t *src_جایگاه, cte_t *dest_جایگاه)
{
    mdb_گره_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_جایگاه->cteMDBNode;
    srcCap = src_جایگاه->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_گره_مجموعه_mdb_Prev(srcMDB, CTE_REF(src_جایگاه));
    newMDB = mdb_گره_مجموعه_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_گره_مجموعه_mdb_اولی_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    أكد(cap_get_capType(dest_جایگاه->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    أكد((cte_t *)mdb_گره_get_mdb_بعدی(dest_جایگاه->cteMDBNode) == عدم &&
           (cte_t *)mdb_گره_get_mdb_Prev(dest_جایگاه->cteMDBNode) == عدم);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    مجموعه_Untyped_Cap_As_پر(srcCap, newCap, src_جایگاه);

    dest_جایگاه->cap = newCap;
    dest_جایگاه->cteMDBNode = newMDB;
    mdb_گره_ptr_مجموعه_mdb_بعدی(&src_جایگاه->cteMDBNode, CTE_REF(dest_جایگاه));
    إذا (mdb_گره_get_mdb_بعدی(newMDB)) {
        mdb_گره_ptr_مجموعه_mdb_Prev(
            &CTE_PTR(mdb_گره_get_mdb_بعدی(newMDB))->cteMDBNode,
            CTE_REF(dest_جایگاه));
    }
}

فراغ cteMove(cap_t newCap, cte_t *src_جایگاه, cte_t *dest_جایگاه)
{
    mdb_گره_t mdb;
    کلمه_t prev_ptr, بعدی_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    أكد(cap_get_capType(dest_جایگاه->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    أكد((cte_t *)mdb_گره_get_mdb_بعدی(dest_جایگاه->cteMDBNode) == عدم &&
           (cte_t *)mdb_گره_get_mdb_Prev(dest_جایگاه->cteMDBNode) == عدم);

    mdb = src_جایگاه->cteMDBNode;
    dest_جایگاه->cap = newCap;
    src_جایگاه->cap = cap_null_cap_new();
    dest_جایگاه->cteMDBNode = mdb;
    src_جایگاه->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb);
    إذا (prev_ptr)
        mdb_گره_ptr_مجموعه_mdb_بعدی(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_جایگاه));

    بعدی_ptr = mdb_گره_get_mdb_بعدی(mdb);
    إذا (بعدی_ptr)
        mdb_گره_ptr_مجموعه_mdb_Prev(
            &CTE_PTR(بعدی_ptr)->cteMDBNode,
            CTE_REF(dest_جایگاه));
}

فراغ cap_جابجا_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    إذا (slot1 == slot2) {
        أرجع;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_جابجا(cap1, slot1, cap2, slot2);
}

فراغ cte_جابجا(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_گره_t mdb1, mdb2;
    کلمه_t بعدی_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb1);
    إذا (prev_ptr)
        mdb_گره_ptr_مجموعه_mdb_بعدی(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    بعدی_ptr = mdb_گره_get_mdb_بعدی(mdb1);
    إذا (بعدی_ptr)
        mdb_گره_ptr_مجموعه_mdb_Prev(
            &CTE_PTR(بعدی_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_گره_get_mdb_Prev(mdb2);
    إذا (prev_ptr)
        mdb_گره_ptr_مجموعه_mdb_بعدی(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    بعدی_ptr = mdb_گره_get_mdb_بعدی(mdb2);
    إذا (بعدی_ptr)
        mdb_گره_ptr_مجموعه_mdb_Prev(
            &CTE_PTR(بعدی_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *جایگاه)
{
    cte_t *بعدی_Ptr;
    exception_t وضعیت;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    لكل (بعدی_Ptr = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode));
         بعدی_Ptr && is_MDBParent_از(جایگاه, بعدی_Ptr);
         بعدی_Ptr = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode))) {
        وضعیت = cteDelete(بعدی_Ptr, true);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        وضعیت = preemption_نقطه();
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *جایگاه, bool_t exposed)
{
    finalise_جایگاه_ret_t fs_ret;

    fs_ret = finalise_جایگاه(جایگاه, exposed);
    إذا (fs_ret.وضعیت != EXCEPTION_NONE) {
        أرجع fs_ret.وضعیت;
    }

    إذا (exposed || fs_ret.success) {
        خالی_جایگاه(جایگاه, fs_ret.cleanupInfo);
    }
    أرجع EXCEPTION_NONE;
}

ساكن فراغ خالی_جایگاه(cte_t *جایگاه, cap_t cleanupInfo)
{
    إذا (cap_get_capType(جایگاه->cap) != cap_null_cap) {
        mdb_گره_t mdb_گره;
        cte_t *prev, *بعدی;

        mdb_گره = جایگاه->cteMDBNode;
        prev = CTE_PTR(mdb_گره_get_mdb_Prev(mdb_گره));
        بعدی = CTE_PTR(mdb_گره_get_mdb_بعدی(mdb_گره));

        إذا (prev) {
            mdb_گره_ptr_مجموعه_mdb_بعدی(&prev->cteMDBNode, CTE_REF(بعدی));
        }
        إذا (بعدی) {
            mdb_گره_ptr_مجموعه_mdb_Prev(&بعدی->cteMDBNode, CTE_REF(prev));
        }
        إذا (بعدی)
            mdb_گره_ptr_مجموعه_mdb_اولی_Badged(&بعدی->cteMDBNode,
                                            mdb_گره_get_mdb_اولی_Badged(بعدی->cteMDBNode) ||
                                            mdb_گره_get_mdb_اولی_Badged(mdb_گره));
        جایگاه->cap = cap_null_cap_new();
        جایگاه->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

ساكن مضمن bool_t CONST capRemovable(cap_t cap, cte_t *جایگاه)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_null_cap:
        أرجع true;
    حالة cap_zombie_cap: {
        کلمه_t n = cap_zombie_cap_get_cap_Zombie_عدد(cap);
        cte_t *z_جایگاه = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        أرجع (n == 0 || (n == 1 && جایگاه == z_جایگاه));
    }
    افتراضي:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

ساكن مضمن bool_t CONST capCyclicZombie(cap_t cap, cte_t *جایگاه)
{
    أرجع cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == جایگاه;
}

ساكن finalise_جایگاه_ret_t finalise_جایگاه(cte_t *جایگاه, bool_t فوری)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t وضعیت;
    finalise_جایگاه_ret_t ret;

    طالما (cap_get_capType(جایگاه->cap) != cap_null_cap) {
        final = isFinalCapability(جایگاه);
        fc_ret = finaliseCap(جایگاه->cap, final, false);

        إذا (capRemovable(fc_ret.remainder, جایگاه)) {
            ret.وضعیت = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        جایگاه->cap = fc_ret.remainder;

        إذا (!فوری && capCyclicZombie(fc_ret.remainder, جایگاه)) {
            ret.وضعیت = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            أرجع ret;
        }

        وضعیت = reduceZombie(جایگاه, فوری);
        إذا (وضعیت != EXCEPTION_NONE) {
            ret.وضعیت = وضعیت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }

        وضعیت = preemption_نقطه();
        إذا (وضعیت != EXCEPTION_NONE) {
            ret.وضعیت = وضعیت;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            أرجع ret;
        }
    }
    ret.وضعیت = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    أرجع ret;
}

ساكن exception_t reduceZombie(cte_t *جایگاه, bool_t فوری)
{
    cte_t *ptr;
    کلمه_t n, type;
    exception_t وضعیت;

    أكد(cap_get_capType(جایگاه->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(جایگاه->cap);
    n = cap_zombie_cap_get_cap_Zombie_عدد(جایگاه->cap);
    type = cap_zombie_cap_get_capZombieType(جایگاه->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    أكد(n > 0);

    إذا (فوری) {
        cte_t *انتها_جایگاه = &ptr[n - 1];

        وضعیت = cteDelete(انتها_جایگاه, false);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        بدل (cap_get_capType(جایگاه->cap)) {
        حالة cap_null_cap:
            اكسر;

        حالة cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(جایگاه->cap);

            إذا (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_عدد(جایگاه->cap) == n &&
                cap_zombie_cap_get_capZombieType(جایگاه->cap) == type) {
                أكد(cap_get_capType(انتها_جایگاه->cap) == cap_null_cap);
                جایگاه->cap =
                    cap_zombie_cap_مجموعه_cap_Zombie_عدد(جایگاه->cap, n - 1);
            } وإلا {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                أكد(ptr2 == جایگاه && ptr != جایگاه);
            }
            اكسر;
        }

        افتراضي:
            fail("Expected recursion to result in Zombie.");
        }
    } وإلا {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        أكد(ptr != جایگاه);

        إذا (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            أكد(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_جابجا_For_Delete(ptr, جایگاه);
    }
    أرجع EXCEPTION_NONE;
}

فراغ cteDeleteOne(cte_t *جایگاه)
{
    کلمه_t cap_type = cap_get_capType(جایگاه->cap);
    إذا (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret استفادهنشده;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(جایگاه);
        fc_ret = finaliseCap(جایگاه->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        أكد(capRemovable(fc_ret.remainder, جایگاه) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        خالی_جایگاه(جایگاه, cap_null_cap_new());
    }
}

فراغ درج_New_Cap(cte_t *والد, cte_t *جایگاه, cap_t cap)
{
    cte_t *بعدی;

    بعدی = CTE_PTR(mdb_گره_get_mdb_بعدی(والد->cteMDBNode));
    جایگاه->cap = cap;
    جایگاه->cteMDBNode = mdb_گره_new(CTE_REF(بعدی), true, true, CTE_REF(والد));
    إذا (بعدی) {
        mdb_گره_ptr_مجموعه_mdb_Prev(&بعدی->cteMDBNode, CTE_REF(جایگاه));
    }
    mdb_گره_ptr_مجموعه_mdb_بعدی(&والد->cteMDBNode, CTE_REF(جایگاه));
}

#اگر_تعریف_نشده CONFIG_KERNEL_MCS
فراغ راهاندازی_پاسخ_Master(tcb_t *thread)
{
    cte_t *جایگاه;

    جایگاه = TCB_PTR_CTE_PTR(thread, tcb_پاسخ);
    إذا (cap_get_capType(جایگاه->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        جایگاه->cap = cap_پاسخ_cap_new(true, true, TCB_REF(thread));
        جایگاه->cteMDBNode = nullMDBNode;
        mdb_گره_ptr_مجموعه_mdb_Revocable(&جایگاه->cteMDBNode, true);
        mdb_گره_ptr_مجموعه_mdb_اولی_Badged(&جایگاه->cteMDBNode, true);
    }
}
#پایان_اگر

bool_t خالص is_MDBParent_از(cte_t *cte_a, cte_t *cte_b)
{
    إذا (!mdb_گره_get_mdb_Revocable(cte_a->cteMDBNode)) {
        أرجع false;
    }
    إذا (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        أرجع false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    إذا (!Arch_is_MDBParent_از(cte_a->cap, cte_b->cap,
                            mdb_گره_get_mdb_اولی_Badged(cte_b->cteMDBNode))) {
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
               !mdb_گره_get_mdb_اولی_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    حالة cap_notification_cap: {
        کلمه_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        إذا (badge == 0) {
            أرجع true;
        }
        أرجع (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_گره_get_mdb_اولی_Badged(cte_b->cteMDBNode);
        اكسر;
    }

    افتراضي:
        أرجع true;
        اكسر;
    }
}

exception_t ensureNoChildren(cte_t *جایگاه)
{
    إذا (mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode) != 0) {
        cte_t *بعدی;

        بعدی = CTE_PTR(mdb_گره_get_mdb_بعدی(جایگاه->cteMDBNode));
        إذا (is_MDBParent_از(جایگاه, بعدی)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    أرجع EXCEPTION_NONE;
}

exception_t ensure_خالی_جایگاه(cte_t *جایگاه)
{
    إذا (cap_get_capType(جایگاه->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_اولی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

bool_t خالص isFinalCapability(cte_t *cte)
{
    mdb_گره_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    إذا (mdb_گره_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } وإلا {
        cte_t *prev;

        prev = CTE_PTR(mdb_گره_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    إذا (prevIsSameObject) {
        أرجع false;
    } وإلا {
        إذا (mdb_گره_get_mdb_بعدی(mdb) == 0) {
            أرجع true;
        } وإلا {
            cte_t *بعدی;

            بعدی = CTE_PTR(mdb_گره_get_mdb_بعدی(mdb));
            أرجع !sameObjectAs(cte->cap, بعدی->cap);
        }
    }
}

bool_t خالص جایگاه_Cap_Long_Running_Delete(cte_t *جایگاه)
{
    إذا (cap_get_capType(جایگاه->cap) == cap_null_cap) {
        أرجع false;
    } وإلا إذا (! isFinalCapability(جایگاه)) {
        أرجع false;
    }
    بدل (cap_get_capType(جایگاه->cap)) {
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
cte_t *getReceiveSlots(tcb_t *thread, کلمه_t *میانگیر)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_جایگاه_ret_t lus_ret;
    cte_t *جایگاه;
    cap_t cnode;

    إذا (!میانگیر) {
        أرجع عدم;
    }

    ct = بارگیری_Cap_Transfer(میانگیر);
    cptr = ct.ct_Receive_ریشه;

    luc_ret = lookupCap(thread, cptr);
    إذا (luc_ret.وضعیت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_هدف_جایگاه(cnode, ct.ct_Receive_شاخص, ct.ct_Receive_عمق);
    إذا (lus_ret.وضعیت != EXCEPTION_NONE) {
        أرجع عدم;
    }
    جایگاه = lus_ret.جایگاه;

    إذا (cap_get_capType(جایگاه->cap) != cap_null_cap) {
        أرجع عدم;
    }

    أرجع جایگاه;
}

cap_transfer_t خالص بارگیری_Cap_Transfer(کلمه_t *میانگیر)
{
    ثابت صحيح جابه_جایی = se_L4_پیام_Max_درازی + se_L4_پیام_Max_Extra_Caps + 2;
    أرجع cap_Transfer_From_واژگان(میانگیر + جابه_جایی);
}
