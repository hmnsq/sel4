/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/invocation.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <api/types.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/objecttype.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <object/interrupt.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <model/preemption.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <util.h>

నిర్మాణం finalise_స్థానం_ret {
    exception_t స్థితి_2;
    bool_t success;
    cap_t cleanupInfo;
};
రకనిర్వచనం నిర్మాణం finalise_స్థానం_ret finalise_స్థానం_ret_t;

స్థిర finalise_స్థానం_ret_t finalise_స్థానం(cte_t *స్థానం, bool_t exposed);
స్థిర శూన్య ఖాళీలు_స్థానం(cte_t *స్థానం, cap_t cleanupInfo);
స్థిర exception_t reduceZombie(cte_t *స్థానం, bool_t exposed);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
#నిర్వచించు CNODE_చివరి_INVOCATION CNodeRotate
#లేకపోతే
#నిర్వచించు CNODE_చివరి_INVOCATION CNodeSaveCaller
#అయితే_ముగింపు

exception_t decodeCNodeInvocation(మాట_t invLabel, మాట_t నిడివి, cap_t cap,
                                  మాట_t *బఫరు)
{
    lookup_స్థానం_ret_t lu_ret;
    cte_t *dest_స్థానం;
    మాట_t సూచీ, w_బిట్లు;
    exception_t స్థితి_2;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    ప్రతిజ్ఞా(cap_get_capType(cap) == cap_cnode_cap);

    ఒకవేళ (invLabel < CNodeRevoke || invLabel > CNODE_చివరి_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    సూచీ = getSyscallArg(0, బఫరు);
    w_బిట్లు = getSyscallArg(1, బఫరు);

    lu_ret = lookup_లక్ష్యం_స్థానం(cap, సూచీ, w_బిట్లు);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        తిరిగి lu_ret.స్థితి_2;
    }
    dest_స్థానం = lu_ret.స్థానం;

    ఒకవేళ (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *src_స్థానం;
        మాట_t src_సూచీ, src_లోతు, cap_దత్తాంశం;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_మూలం, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        ఒకవేళ (నిడివి < 4 || current_extra_caps.excaprefs[0] == ఖాళీ) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        src_సూచీ = getSyscallArg(2, బఫరు);
        src_లోతు = getSyscallArg(3, బఫరు);

        src_మూలం = current_extra_caps.excaprefs[0]->cap;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            తిరిగి స్థితి_2;
        }

        lu_ret = lookup_ఆధారం_స్థానం(src_మూలం, src_సూచీ, src_లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            తిరిగి lu_ret.స్థితి_2;
        }
        src_స్థానం = lu_ret.స్థానం;

        ఒకవేళ (cap_get_capType(src_స్థానం->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_లోతు);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఎంపిక (invLabel) {
        స్థితి CNodeCopy:

            ఒకవేళ (నిడివి < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_సందేశం;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_మాట(getSyscallArg(4, బఫరు));
            srcCap = ముసుగు_Cap_Rights(cap_rights, src_స్థానం->cap);
            dc_ret = deriveCap(src_స్థానం, srcCap);
            ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                తిరిగి dc_ret.స్థితి_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ఆపు;

        స్థితి CNodeMint:
            ఒకవేళ (నిడివి < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_సందేశం;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rights_From_మాట(getSyscallArg(4, బఫరు));
            cap_దత్తాంశం = getSyscallArg(5, బఫరు);
            srcCap = ముసుగు_Cap_Rights(cap_rights, src_స్థానం->cap);
            dc_ret = deriveCap(src_స్థానం,
                               update_Cap_దత్తాంశం(false, cap_దత్తాంశం, srcCap));
            ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                తిరిగి dc_ret.స్థితి_2;
            }
            newCap = dc_ret.cap;
            isMove = false;

            ఆపు;

        స్థితి CNodeMove:
            newCap = src_స్థానం->cap;
            isMove = true;

            ఆపు;

        స్థితి CNodeMutate:
            ఒకవేళ (నిడివి < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_సందేశం;
                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            cap_దత్తాంశం = getSyscallArg(4, బఫరు);
            newCap = update_Cap_దత్తాంశం(true, cap_దత్తాంశం, src_స్థానం->cap);
            isMove = true;

            ఆపు;

        అప్రమేయ:
            ప్రతిజ్ఞా(0);
            తిరిగి EXCEPTION_NONE;
        }

        ఒకవేళ (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        ఒకవేళ (isMove) {
            తిరిగి invokeCNodeMove(newCap, src_స్థానం, dest_స్థానం);
        } లేకపోతే {
            తిరిగి invoke_CNode_చొప్పించు(newCap, src_స్థానం, dest_స్థానం);
        }
    }

    ఒకవేళ (invLabel == CNodeRevoke) {
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeCNodeRevoke(dest_స్థానం);
    }

    ఒకవేళ (invLabel == CNodeDelete) {
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeCNodeDelete(dest_స్థానం);
    }

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    ఒకవేళ (invLabel == CNodeSaveCaller) {
        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeCNodeSaveCaller(dest_స్థానం);
    }
#అయితే_ముగింపు

    ఒకవేళ (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = dest_స్థానం->cap;

        ఒకవేళ (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeCNodeCancelBadgedSends(destCap);
    }

    ఒకవేళ (invLabel == CNodeRotate) {
        మాట_t ఇరుసు_New_దత్తాంశం, ఇరుసు_సూచీ, ఇరుసు_లోతు;
        మాట_t src_New_దత్తాంశం, src_సూచీ, src_లోతు;
        cte_t *ఇరుసు_స్థానం, *src_స్థానం;
        cap_t ఇరుసు_మూలం, src_మూలం, newSrcCap, new_ఇరుసు_Cap;

        ఒకవేళ (నిడివి < 8 || current_extra_caps.excaprefs[0] == ఖాళీ
            || current_extra_caps.excaprefs[1] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఇరుసు_New_దత్తాంశం = getSyscallArg(2, బఫరు);
        ఇరుసు_సూచీ   = getSyscallArg(3, బఫరు);
        ఇరుసు_లోతు   = getSyscallArg(4, బఫరు);
        src_New_దత్తాంశం   = getSyscallArg(5, బఫరు);
        src_సూచీ     = getSyscallArg(6, బఫరు);
        src_లోతు     = getSyscallArg(7, బఫరు);

        ఇరుసు_మూలం = current_extra_caps.excaprefs[0]->cap;
        src_మూలం   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookup_ఆధారం_స్థానం(src_మూలం, src_సూచీ, src_లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి lu_ret.స్థితి_2;
        }
        src_స్థానం = lu_ret.స్థానం;

        lu_ret = lookup_ఇరుసు_స్థానం(ఇరుసు_మూలం, ఇరుసు_సూచీ, ఇరుసు_లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి lu_ret.స్థితి_2;
        }
        ఇరుసు_స్థానం = lu_ret.స్థానం;

        ఒకవేళ (ఇరుసు_స్థానం == src_స్థానం || ఇరుసు_స్థానం == dest_స్థానం) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (src_స్థానం != dest_స్థానం) {
            స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
            ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
                తిరిగి స్థితి_2;
            }
        }

        ఒకవేళ (cap_get_capType(src_స్థానం->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_లోతు);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (cap_get_capType(ఇరుసు_స్థానం->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(ఇరుసు_లోతు);
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_దత్తాంశం(true, src_New_దత్తాంశం, src_స్థానం->cap);
        new_ఇరుసు_Cap = update_Cap_దత్తాంశం(true, ఇరుసు_New_దత్తాంశం, ఇరుసు_స్థానం->cap);

        ఒకవేళ (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (cap_get_capType(new_ఇరుసు_Cap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి invokeCNodeRotate(newSrcCap, new_ఇరుసు_Cap,
                                 src_స్థానం, ఇరుసు_స్థానం, dest_స్థానం);
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *dest_స్థానం)
{
    తిరిగి cteRevoke(dest_స్థానం);
}

exception_t invokeCNodeDelete(cte_t *dest_స్థానం)
{
    తిరిగి cteDelete(dest_స్థానం, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    మాట_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    ఒకవేళ (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    తిరిగి EXCEPTION_NONE;
}

exception_t invoke_CNode_చొప్పించు(cap_t cap, cte_t *src_స్థానం, cte_t *dest_స్థానం)
{
    cte_చొప్పించు(cap, src_స్థానం, dest_స్థానం);

    తిరిగి EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *src_స్థానం, cte_t *dest_స్థానం)
{
    cteMove(cap, src_స్థానం, dest_స్థానం);

    తిరిగి EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    ఒకవేళ (slot1 == slot3) {
        cte_మార్పిడి(cap1, slot1, cap2, slot2);
    } లేకపోతే {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    తిరిగి EXCEPTION_NONE;
}

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *dest_స్థానం)
{
    cap_t cap;
    cte_t *src_స్థానం;

    src_స్థానం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbCaller);
    cap = src_స్థానం->cap;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        ఆపు;

    స్థితి cap_జవాబు_cap:
        ఒకవేళ (!cap_జవాబు_cap_get_cap_జవాబు_Master(cap)) {
            cteMove(cap, src_స్థానం, dest_స్థానం);
        }
        ఆపు;

    అప్రమేయ:
        fail("caller capability must be null or reply");
        ఆపు;
    }

    తిరిగి EXCEPTION_NONE;
}
#అయితే_ముగింపు

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
స్థిర శూన్య సమితి_Untyped_Cap_As_నిండిన(cap_t srcCap, cap_t newCap, cte_t *src_స్థానం)
{
    ఒకవేళ ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        ఒకవేళ ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_ఖండం_కొలత(newCap)
                == cap_untyped_cap_get_cap_ఖండం_కొలత(srcCap))) {
            cap_untyped_cap_ptr_సమితి_cap_Free_సూచీ(&(src_స్థానం->cap),
                                                 MAX_FREE_సూచీ(cap_untyped_cap_get_cap_ఖండం_కొలత(srcCap)));
        }
    }
}

శూన్య cte_చొప్పించు(cap_t newCap, cte_t *src_స్థానం, cte_t *dest_స్థానం)
{
    mdb_కణుపు_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = src_స్థానం->cteMDBNode;
    srcCap = src_స్థానం->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_కణుపు_సమితి_mdb_Prev(srcMDB, CTE_REF(src_స్థానం));
    newMDB = mdb_కణుపు_సమితి_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_కణుపు_సమితి_mdb_మొదటి_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    ప్రతిజ్ఞా(cap_get_capType(dest_స్థానం->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    ప్రతిజ్ఞా((cte_t *)mdb_కణుపు_get_mdb_తదుపరి(dest_స్థానం->cteMDBNode) == ఖాళీ &&
           (cte_t *)mdb_కణుపు_get_mdb_Prev(dest_స్థానం->cteMDBNode) == ఖాళీ);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    సమితి_Untyped_Cap_As_నిండిన(srcCap, newCap, src_స్థానం);

    dest_స్థానం->cap = newCap;
    dest_స్థానం->cteMDBNode = newMDB;
    mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(&src_స్థానం->cteMDBNode, CTE_REF(dest_స్థానం));
    ఒకవేళ (mdb_కణుపు_get_mdb_తదుపరి(newMDB)) {
        mdb_కణుపు_ptr_సమితి_mdb_Prev(
            &CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(newMDB))->cteMDBNode,
            CTE_REF(dest_స్థానం));
    }
}

శూన్య cteMove(cap_t newCap, cte_t *src_స్థానం, cte_t *dest_స్థానం)
{
    mdb_కణుపు_t mdb;
    మాట_t prev_ptr, తదుపరి_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    ప్రతిజ్ఞా(cap_get_capType(dest_స్థానం->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    ప్రతిజ్ఞా((cte_t *)mdb_కణుపు_get_mdb_తదుపరి(dest_స్థానం->cteMDBNode) == ఖాళీ &&
           (cte_t *)mdb_కణుపు_get_mdb_Prev(dest_స్థానం->cteMDBNode) == ఖాళీ);

    mdb = src_స్థానం->cteMDBNode;
    dest_స్థానం->cap = newCap;
    src_స్థానం->cap = cap_null_cap_new();
    dest_స్థానం->cteMDBNode = mdb;
    src_స్థానం->cteMDBNode = nullMDBNode;

    prev_ptr = mdb_కణుపు_get_mdb_Prev(mdb);
    ఒకవేళ (prev_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(dest_స్థానం));

    తదుపరి_ptr = mdb_కణుపు_get_mdb_తదుపరి(mdb);
    ఒకవేళ (తదుపరి_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_Prev(
            &CTE_PTR(తదుపరి_ptr)->cteMDBNode,
            CTE_REF(dest_స్థానం));
}

శూన్య cap_మార్పిడి_For_Delete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    ఒకవేళ (slot1 == slot2) {
        తిరిగి;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cte_మార్పిడి(cap1, slot1, cap2, slot2);
}

శూన్య cte_మార్పిడి(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_కణుపు_t mdb1, mdb2;
    మాట_t తదుపరి_ptr, prev_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    prev_ptr = mdb_కణుపు_get_mdb_Prev(mdb1);
    ఒకవేళ (prev_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot2));

    తదుపరి_ptr = mdb_కణుపు_get_mdb_తదుపరి(mdb1);
    ఒకవేళ (తదుపరి_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_Prev(
            &CTE_PTR(తదుపరి_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    prev_ptr = mdb_కణుపు_get_mdb_Prev(mdb2);
    ఒకవేళ (prev_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(
            &CTE_PTR(prev_ptr)->cteMDBNode,
            CTE_REF(slot1));

    తదుపరి_ptr = mdb_కణుపు_get_mdb_తదుపరి(mdb2);
    ఒకవేళ (తదుపరి_ptr)
        mdb_కణుపు_ptr_సమితి_mdb_Prev(
            &CTE_PTR(తదుపరి_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *స్థానం)
{
    cte_t *తదుపరి_Ptr;
    exception_t స్థితి_2;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    కొరకు (తదుపరి_Ptr = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode));
         తదుపరి_Ptr && is_MDBParent_యొక్క(స్థానం, తదుపరి_Ptr);
         తదుపరి_Ptr = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode))) {
        స్థితి_2 = cteDelete(తదుపరి_Ptr, true);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        స్థితి_2 = preemption_బిందువు();
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *స్థానం, bool_t exposed)
{
    finalise_స్థానం_ret_t fs_ret;

    fs_ret = finalise_స్థానం(స్థానం, exposed);
    ఒకవేళ (fs_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి fs_ret.స్థితి_2;
    }

    ఒకవేళ (exposed || fs_ret.success) {
        ఖాళీలు_స్థానం(స్థానం, fs_ret.cleanupInfo);
    }
    తిరిగి EXCEPTION_NONE;
}

స్థిర శూన్య ఖాళీలు_స్థానం(cte_t *స్థానం, cap_t cleanupInfo)
{
    ఒకవేళ (cap_get_capType(స్థానం->cap) != cap_null_cap) {
        mdb_కణుపు_t mdb_కణుపు;
        cte_t *prev, *తదుపరి;

        mdb_కణుపు = స్థానం->cteMDBNode;
        prev = CTE_PTR(mdb_కణుపు_get_mdb_Prev(mdb_కణుపు));
        తదుపరి = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(mdb_కణుపు));

        ఒకవేళ (prev) {
            mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(&prev->cteMDBNode, CTE_REF(తదుపరి));
        }
        ఒకవేళ (తదుపరి) {
            mdb_కణుపు_ptr_సమితి_mdb_Prev(&తదుపరి->cteMDBNode, CTE_REF(prev));
        }
        ఒకవేళ (తదుపరి)
            mdb_కణుపు_ptr_సమితి_mdb_మొదటి_Badged(&తదుపరి->cteMDBNode,
                                            mdb_కణుపు_get_mdb_మొదటి_Badged(తదుపరి->cteMDBNode) ||
                                            mdb_కణుపు_get_mdb_మొదటి_Badged(mdb_కణుపు));
        స్థానం->cap = cap_null_cap_new();
        స్థానం->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

స్థిర అంతఃవరుస bool_t CONST capRemovable(cap_t cap, cte_t *స్థానం)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_null_cap:
        తిరిగి true;
    స్థితి cap_zombie_cap: {
        మాట_t n = cap_zombie_cap_get_cap_Zombie_సంఖ్య(cap);
        cte_t *z_స్థానం = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        తిరిగి (n == 0 || (n == 1 && స్థానం == z_స్థానం));
    }
    అప్రమేయ:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

స్థిర అంతఃవరుస bool_t CONST capCyclicZombie(cap_t cap, cte_t *స్థానం)
{
    తిరిగి cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == స్థానం;
}

స్థిర finalise_స్థానం_ret_t finalise_స్థానం(cte_t *స్థానం, bool_t తక్షణ)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t స్థితి_2;
    finalise_స్థానం_ret_t ret;

    అయితే (cap_get_capType(స్థానం->cap) != cap_null_cap) {
        final = isFinalCapability(స్థానం);
        fc_ret = finaliseCap(స్థానం->cap, final, false);

        ఒకవేళ (capRemovable(fc_ret.remainder, స్థానం)) {
            ret.స్థితి_2 = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            తిరిగి ret;
        }

        స్థానం->cap = fc_ret.remainder;

        ఒకవేళ (!తక్షణ && capCyclicZombie(fc_ret.remainder, స్థానం)) {
            ret.స్థితి_2 = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            తిరిగి ret;
        }

        స్థితి_2 = reduceZombie(స్థానం, తక్షణ);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            ret.స్థితి_2 = స్థితి_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            తిరిగి ret;
        }

        స్థితి_2 = preemption_బిందువు();
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            ret.స్థితి_2 = స్థితి_2;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            తిరిగి ret;
        }
    }
    ret.స్థితి_2 = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    తిరిగి ret;
}

స్థిర exception_t reduceZombie(cte_t *స్థానం, bool_t తక్షణ)
{
    cte_t *ptr;
    మాట_t n, type;
    exception_t స్థితి_2;

    ప్రతిజ్ఞా(cap_get_capType(స్థానం->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(స్థానం->cap);
    n = cap_zombie_cap_get_cap_Zombie_సంఖ్య(స్థానం->cap);
    type = cap_zombie_cap_get_capZombieType(స్థానం->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    ప్రతిజ్ఞా(n > 0);

    ఒకవేళ (తక్షణ) {
        cte_t *చివర_స్థానం = &ptr[n - 1];

        స్థితి_2 = cteDelete(చివర_స్థానం, false);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        ఎంపిక (cap_get_capType(స్థానం->cap)) {
        స్థితి cap_null_cap:
            ఆపు;

        స్థితి cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(స్థానం->cap);

            ఒకవేళ (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_సంఖ్య(స్థానం->cap) == n &&
                cap_zombie_cap_get_capZombieType(స్థానం->cap) == type) {
                ప్రతిజ్ఞా(cap_get_capType(చివర_స్థానం->cap) == cap_null_cap);
                స్థానం->cap =
                    cap_zombie_cap_సమితి_cap_Zombie_సంఖ్య(స్థానం->cap, n - 1);
            } లేకపోతే {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                ప్రతిజ్ఞా(ptr2 == స్థానం && ptr != స్థానం);
            }
            ఆపు;
        }

        అప్రమేయ:
            fail("Expected recursion to result in Zombie.");
        }
    } లేకపోతే {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        ప్రతిజ్ఞా(ptr != స్థానం);

        ఒకవేళ (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            ప్రతిజ్ఞా(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        cap_మార్పిడి_For_Delete(ptr, స్థానం);
    }
    తిరిగి EXCEPTION_NONE;
}

శూన్య cteDeleteOne(cte_t *స్థానం)
{
    మాట_t cap_type = cap_get_capType(స్థానం->cap);
    ఒకవేళ (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret వాడని;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(స్థానం);
        fc_ret = finaliseCap(స్థానం->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        ప్రతిజ్ఞా(capRemovable(fc_ret.remainder, స్థానం) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        ఖాళీలు_స్థానం(స్థానం, cap_null_cap_new());
    }
}

శూన్య చొప్పించు_New_Cap(cte_t *మాతృ, cte_t *స్థానం, cap_t cap)
{
    cte_t *తదుపరి;

    తదుపరి = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(మాతృ->cteMDBNode));
    స్థానం->cap = cap;
    స్థానం->cteMDBNode = mdb_కణుపు_new(CTE_REF(తదుపరి), true, true, CTE_REF(మాతృ));
    ఒకవేళ (తదుపరి) {
        mdb_కణుపు_ptr_సమితి_mdb_Prev(&తదుపరి->cteMDBNode, CTE_REF(స్థానం));
    }
    mdb_కణుపు_ptr_సమితి_mdb_తదుపరి(&మాతృ->cteMDBNode, CTE_REF(స్థానం));
}

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
శూన్య అమరిక_జవాబు_Master(tcb_t *thread)
{
    cte_t *స్థానం;

    స్థానం = TCB_PTR_CTE_PTR(thread, tcb_జవాబు);
    ఒకవేళ (cap_get_capType(స్థానం->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        స్థానం->cap = cap_జవాబు_cap_new(true, true, TCB_REF(thread));
        స్థానం->cteMDBNode = nullMDBNode;
        mdb_కణుపు_ptr_సమితి_mdb_Revocable(&స్థానం->cteMDBNode, true);
        mdb_కణుపు_ptr_సమితి_mdb_మొదటి_Badged(&స్థానం->cteMDBNode, true);
    }
}
#అయితే_ముగింపు

bool_t శుద్ధ is_MDBParent_యొక్క(cte_t *cte_a, cte_t *cte_b)
{
    ఒకవేళ (!mdb_కణుపు_get_mdb_Revocable(cte_a->cteMDBNode)) {
        తిరిగి false;
    }
    ఒకవేళ (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        తిరిగి false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    ఒకవేళ (!Arch_is_MDBParent_యొక్క(cte_a->cap, cte_b->cap,
                            mdb_కణుపు_get_mdb_మొదటి_Badged(cte_b->cteMDBNode))) {
        తిరిగి false;
    }
    ఎంపిక (cap_get_capType(cte_a->cap)) {
    స్థితి cap_endpoint_cap: {
        మాట_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        ఒకవేళ (badge == 0) {
            తిరిగి true;
        }
        తిరిగి (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_కణుపు_get_mdb_మొదటి_Badged(cte_b->cteMDBNode);
        ఆపు;
    }

    స్థితి cap_notification_cap: {
        మాట_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        ఒకవేళ (badge == 0) {
            తిరిగి true;
        }
        తిరిగి (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_కణుపు_get_mdb_మొదటి_Badged(cte_b->cteMDBNode);
        ఆపు;
    }

    అప్రమేయ:
        తిరిగి true;
        ఆపు;
    }
}

exception_t ensureNoChildren(cte_t *స్థానం)
{
    ఒకవేళ (mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode) != 0) {
        cte_t *తదుపరి;

        తదుపరి = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(స్థానం->cteMDBNode));
        ఒకవేళ (is_MDBParent_యొక్క(స్థానం, తదుపరి)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    }

    తిరిగి EXCEPTION_NONE;
}

exception_t ensure_ఖాళీలు_స్థానం(cte_t *స్థానం)
{
    ఒకవేళ (cap_get_capType(స్థానం->cap) != cap_null_cap) {
        current_syscall_error.type = se_L4_Delete_మొదటి;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

bool_t శుద్ధ isFinalCapability(cte_t *cte)
{
    mdb_కణుపు_t mdb;
    bool_t prevIsSameObject;

    mdb = cte->cteMDBNode;

    ఒకవేళ (mdb_కణుపు_get_mdb_Prev(mdb) == 0) {
        prevIsSameObject = false;
    } లేకపోతే {
        cte_t *prev;

        prev = CTE_PTR(mdb_కణుపు_get_mdb_Prev(mdb));
        prevIsSameObject = sameObjectAs(prev->cap, cte->cap);
    }

    ఒకవేళ (prevIsSameObject) {
        తిరిగి false;
    } లేకపోతే {
        ఒకవేళ (mdb_కణుపు_get_mdb_తదుపరి(mdb) == 0) {
            తిరిగి true;
        } లేకపోతే {
            cte_t *తదుపరి;

            తదుపరి = CTE_PTR(mdb_కణుపు_get_mdb_తదుపరి(mdb));
            తిరిగి !sameObjectAs(cte->cap, తదుపరి->cap);
        }
    }
}

bool_t శుద్ధ స్థానం_Cap_Long_Running_Delete(cte_t *స్థానం)
{
    ఒకవేళ (cap_get_capType(స్థానం->cap) == cap_null_cap) {
        తిరిగి false;
    } లేకపోతే ఒకవేళ (! isFinalCapability(స్థానం)) {
        తిరిగి false;
    }
    ఎంపిక (cap_get_capType(స్థానం->cap)) {
    స్థితి cap_thread_cap:
    స్థితి cap_zombie_cap:
    స్థితి cap_cnode_cap:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, మాట_t *బఫరు)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookup_స్థానం_ret_t lus_ret;
    cte_t *స్థానం;
    cap_t cnode;

    ఒకవేళ (!బఫరు) {
        తిరిగి ఖాళీ;
    }

    ct = ఎక్కించు_Cap_Transfer(బఫరు);
    cptr = ct.ct_Receive_మూలం;

    luc_ret = lookupCap(thread, cptr);
    ఒకవేళ (luc_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి ఖాళీ;
    }
    cnode = luc_ret.cap;

    lus_ret = lookup_లక్ష్యం_స్థానం(cnode, ct.ct_Receive_సూచీ, ct.ct_Receive_లోతు);
    ఒకవేళ (lus_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి ఖాళీ;
    }
    స్థానం = lus_ret.స్థానం;

    ఒకవేళ (cap_get_capType(స్థానం->cap) != cap_null_cap) {
        తిరిగి ఖాళీ;
    }

    తిరిగి స్థానం;
}

cap_transfer_t శుద్ధ ఎక్కించు_Cap_Transfer(మాట_t *బఫరు)
{
    స్థిరరాశి పూర్ణాంకం స్థానభ్రంశం = se_L4_సందేశం_Max_నిడివి + se_L4_సందేశం_Max_Extra_Caps + 2;
    తిరిగి cap_Transfer_From_పదాలు(బఫరు + స్థానభ్రంశం);
}
