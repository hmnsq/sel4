/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <assert.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <api/types.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <object/interrupt.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <model/preemption.h>
#अन्तर्भाव <model/statedata.h>
#अन्तर्भाव <util.h>

संरचना finaliseSlot_ret {
    exception_t स्थितिः;
    bool_t success;
    cap_t cleanupInfo;
};
प्रकारनिर्देश संरचना finaliseSlot_ret finaliseSlot_ret_t;

स्थिर finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
स्थिर शून्य रिक्तानि_Slot(cte_t *slot, cap_t cleanupInfo);
स्थिर exception_t reduceZombie(cte_t *slot, bool_t exposed);

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
#परिभाषय CNODE_चरमः_INVOCATION CNodeRotate
#अन्यथा
#परिभाषय CNODE_चरमः_INVOCATION CNodeSaveCaller
#यदि_अन्तः

exception_t decodeCNodeInvocation(word_t invLabel, word_t दीर्घता, cap_t cap,
                                  word_t *बफर)
{
    lookupSlot_ret_t lu_ret;
    cte_t *destSlot;
    word_t सूचकांक, w_द्वयङ्काः;
    exception_t स्थितिः;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    प्रतिज्ञा(cap_get_capType(cap) == cap_cnode_cap);

    यदि (invLabel < CNodeRevoke || invLabel > CNODE_चरमः_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    यदि (दीर्घता < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    w_द्वयङ्काः = getSyscallArg(1, बफर);

    lu_ret = lookupTargetSlot(cap, सूचकांक, w_द्वयङ्काः);
    यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        प्रत्यागम lu_ret.स्थितिः;
    }
    destSlot = lu_ret.slot;

    यदि (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t src_सूचकांक, src_गभीरता, cap_दत्त;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_मूलम्, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        यदि (दीर्घता < 4 || current_extra_caps.excaprefs[0] == रिक्त) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        src_सूचकांक = getSyscallArg(2, बफर);
        src_गभीरता = getSyscallArg(3, बफर);

        src_मूलम् = current_extra_caps.excaprefs[0]->cap;

        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            प्रत्यागम स्थितिः;
        }

        lu_ret = lookupSourceSlot(src_मूलम्, src_सूचकांक, src_गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            प्रत्यागम lu_ret.स्थितिः;
        }
        srcSlot = lu_ret.slot;

        यदि (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(src_गभीरता);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        विकल्प (invLabel) {
        पक्ष CNodeCopy:

            यदि (दीर्घता < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = se_L4_Truncated_सन्देशः;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, बफर));
            srcCap = आवरणम्_Cap_Rights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                प्रत्यागम dc_ret.स्थितिः;
            }
            newCap = dc_ret.cap;
            isMove = false;

            भङ्ग;

        पक्ष CNodeMint:
            यदि (दीर्घता < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_सन्देशः;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, बफर));
            cap_दत्त = getSyscallArg(5, बफर);
            srcCap = आवरणम्_Cap_Rights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               update_Cap_दत्त(false, cap_दत्त, srcCap));
            यदि (dc_ret.स्थितिः != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                प्रत्यागम dc_ret.स्थितिः;
            }
            newCap = dc_ret.cap;
            isMove = false;

            भङ्ग;

        पक्ष CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            भङ्ग;

        पक्ष CNodeMutate:
            यदि (दीर्घता < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = se_L4_Truncated_सन्देशः;
                प्रत्यागम EXCEPTION_SYSCALL_ERROR;
            }

            cap_दत्त = getSyscallArg(4, बफर);
            newCap = update_Cap_दत्त(true, cap_दत्त, srcSlot->cap);
            isMove = true;

            भङ्ग;

        स्वभाव:
            प्रतिज्ञा(0);
            प्रत्यागम EXCEPTION_NONE;
        }

        यदि (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        यदि (isMove) {
            प्रत्यागम invokeCNodeMove(newCap, srcSlot, destSlot);
        } अन्यथा {
            प्रत्यागम invoke_CNode_निवेशय(newCap, srcSlot, destSlot);
        }
    }

    यदि (invLabel == CNodeRevoke) {
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeCNodeRevoke(destSlot);
    }

    यदि (invLabel == CNodeDelete) {
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeCNodeDelete(destSlot);
    }

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
    यदि (invLabel == CNodeSaveCaller) {
        स्थितिः = ensure_रिक्तानि_Slot(destSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            प्रत्यागम स्थितिः;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeCNodeSaveCaller(destSlot);
    }
#यदि_अन्तः

    यदि (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        यदि (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeCNodeCancelBadgedSends(destCap);
    }

    यदि (invLabel == CNodeRotate) {
        word_t pivot_New_दत्त, pivot_सूचकांक, pivot_गभीरता;
        word_t src_New_दत्त, src_सूचकांक, src_गभीरता;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_मूलम्, src_मूलम्, newSrcCap, newPivotCap;

        यदि (दीर्घता < 8 || current_extra_caps.excaprefs[0] == रिक्त
            || current_extra_caps.excaprefs[1] == रिक्त) {
            current_syscall_error.type = se_L4_Truncated_सन्देशः;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
        pivot_New_दत्त = getSyscallArg(2, बफर);
        pivot_सूचकांक   = getSyscallArg(3, बफर);
        pivot_गभीरता   = getSyscallArg(4, बफर);
        src_New_दत्त   = getSyscallArg(5, बफर);
        src_सूचकांक     = getSyscallArg(6, बफर);
        src_गभीरता     = getSyscallArg(7, बफर);

        pivot_मूलम् = current_extra_caps.excaprefs[0]->cap;
        src_मूलम्   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_मूलम्, src_सूचकांक, src_गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम lu_ret.स्थितिः;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_मूलम्, pivot_सूचकांक, pivot_गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम lu_ret.स्थितिः;
        }
        pivotSlot = lu_ret.slot;

        यदि (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (srcSlot != destSlot) {
            स्थितिः = ensure_रिक्तानि_Slot(destSlot);
            यदि (स्थितिः != EXCEPTION_NONE) {
                प्रत्यागम स्थितिः;
            }
        }

        यदि (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(src_गभीरता);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivot_गभीरता);
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_दत्त(true, src_New_दत्त, srcSlot->cap);
        newPivotCap = update_Cap_दत्त(true, pivot_New_दत्त, pivotSlot->cap);

        यदि (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        यदि (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }

        समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
        प्रत्यागम invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    प्रत्यागम cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    प्रत्यागम cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    यदि (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    प्रत्यागम EXCEPTION_NONE;
}

exception_t invoke_CNode_निवेशय(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_निवेशय(cap, srcSlot, destSlot);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    यदि (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } अन्यथा {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    प्रत्यागम EXCEPTION_NONE;
}

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(ग्रन्थिः_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        भङ्ग;

    पक्ष cap_प्रत्युत्तरम्_cap:
        यदि (!cap_प्रत्युत्तरम्_cap_get_cap_प्रत्युत्तरम्_Master(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        भङ्ग;

    स्वभाव:
        fail("caller capability must be null or reply");
        भङ्ग;
    }

    प्रत्यागम EXCEPTION_NONE;
}
#यदि_अन्तः

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
स्थिर शून्य समुच्चयः_Untyped_Cap_As_पूर्णम्(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    यदि ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        यदि ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_खण्डम्_आकार(newCap)
                == cap_untyped_cap_get_cap_खण्डम्_आकार(srcCap))) {
            cap_untyped_cap_ptr_समुच्चयः_cap_Free_सूचकांक(&(srcSlot->cap),
                                                 MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_खण्डम्_आकार(srcCap)));
        }
    }
}

शून्य cte_निवेशय(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ग्रन्थिः_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_ग्रन्थिः_समुच्चयः_mdb_पूर्व(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_ग्रन्थिः_समुच्चयः_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_ग्रन्थिः_समुच्चयः_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_ग्रन्थिः_get_mdb_अग्र(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_ग्रन्थिः_get_mdb_पूर्व(destSlot->cteMDBNode) == रिक्त);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    समुच्चयः_Untyped_Cap_As_पूर्णम्(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    यदि (mdb_ग्रन्थिः_get_mdb_अग्र(newMDB)) {
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(
            &CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

शून्य cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_ग्रन्थिः_t mdb;
    word_t पूर्व_ptr, अग्र_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    प्रतिज्ञा(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    प्रतिज्ञा((cte_t *)mdb_ग्रन्थिः_get_mdb_अग्र(destSlot->cteMDBNode) == रिक्त &&
           (cte_t *)mdb_ग्रन्थिः_get_mdb_पूर्व(destSlot->cteMDBNode) == रिक्त);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    पूर्व_ptr = mdb_ग्रन्थिः_get_mdb_पूर्व(mdb);
    यदि (पूर्व_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    अग्र_ptr = mdb_ग्रन्थिः_get_mdb_अग्र(mdb);
    यदि (अग्र_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

शून्य capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    यदि (slot1 == slot2) {
        प्रत्यागम;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

शून्य cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_ग्रन्थिः_t mdb1, mdb2;
    word_t अग्र_ptr, पूर्व_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    पूर्व_ptr = mdb_ग्रन्थिः_get_mdb_पूर्व(mdb1);
    यदि (पूर्व_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(slot2));

    अग्र_ptr = mdb_ग्रन्थिः_get_mdb_अग्र(mdb1);
    यदि (अग्र_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    पूर्व_ptr = mdb_ग्रन्थिः_get_mdb_पूर्व(mdb2);
    यदि (पूर्व_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(slot1));

    अग्र_ptr = mdb_ग्रन्थिः_get_mdb_अग्र(mdb2);
    यदि (अग्र_ptr)
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *अग्र_Ptr;
    exception_t स्थितिः;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    प्रति (अग्र_Ptr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode));
         अग्र_Ptr && is_MDBParent_सम्बन्धि(slot, अग्र_Ptr);
         अग्र_Ptr = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode))) {
        स्थितिः = cteDelete(अग्र_Ptr, true);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        स्थितिः = preemptionPoint();
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    यदि (fs_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम fs_ret.स्थितिः;
    }

    यदि (exposed || fs_ret.success) {
        रिक्तानि_Slot(slot, fs_ret.cleanupInfo);
    }
    प्रत्यागम EXCEPTION_NONE;
}

स्थिर शून्य रिक्तानि_Slot(cte_t *slot, cap_t cleanupInfo)
{
    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_ग्रन्थिः_t mdb_ग्रन्थिः;
        cte_t *पूर्व, *अग्र;

        mdb_ग्रन्थिः = slot->cteMDBNode;
        पूर्व = CTE_PTR(mdb_ग्रन्थिः_get_mdb_पूर्व(mdb_ग्रन्थिः));
        अग्र = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(mdb_ग्रन्थिः));

        यदि (पूर्व) {
            mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(&पूर्व->cteMDBNode, CTE_REF(अग्र));
        }
        यदि (अग्र) {
            mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(&अग्र->cteMDBNode, CTE_REF(पूर्व));
        }
        यदि (अग्र)
            mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_First_Badged(&अग्र->cteMDBNode,
                                            mdb_ग्रन्थिः_get_mdb_First_Badged(अग्र->cteMDBNode) ||
                                            mdb_ग्रन्थिः_get_mdb_First_Badged(mdb_ग्रन्थिः));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

स्थिर अन्तःस्थ bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_null_cap:
        प्रत्यागम true;
    पक्ष cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_सङ्ख्या(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        प्रत्यागम (n == 0 || (n == 1 && slot == z_slot));
    }
    स्वभाव:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

स्थिर अन्तःस्थ bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    प्रत्यागम cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

स्थिर finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t तात्कालिकम्)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t स्थितिः;
    finaliseSlot_ret_t ret;

    यावत् (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        यदि (capRemovable(fc_ret.remainder, slot)) {
            ret.स्थितिः = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            प्रत्यागम ret;
        }

        slot->cap = fc_ret.remainder;

        यदि (!तात्कालिकम् && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.स्थितिः = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            प्रत्यागम ret;
        }

        स्थितिः = reduceZombie(slot, तात्कालिकम्);
        यदि (स्थितिः != EXCEPTION_NONE) {
            ret.स्थितिः = स्थितिः;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            प्रत्यागम ret;
        }

        स्थितिः = preemptionPoint();
        यदि (स्थितिः != EXCEPTION_NONE) {
            ret.स्थितिः = स्थितिः;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            प्रत्यागम ret;
        }
    }
    ret.स्थितिः = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम ret;
}

स्थिर exception_t reduceZombie(cte_t *slot, bool_t तात्कालिकम्)
{
    cte_t *ptr;
    word_t n, type;
    exception_t स्थितिः;

    प्रतिज्ञा(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_सङ्ख्या(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    प्रतिज्ञा(n > 0);

    यदि (तात्कालिकम्) {
        cte_t *अन्तः_Slot = &ptr[n - 1];

        स्थितिः = cteDelete(अन्तः_Slot, false);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }

        विकल्प (cap_get_capType(slot->cap)) {
        पक्ष cap_null_cap:
            भङ्ग;

        पक्ष cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            यदि (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_सङ्ख्या(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                प्रतिज्ञा(cap_get_capType(अन्तः_Slot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_समुच्चयः_cap_Zombie_सङ्ख्या(slot->cap, n - 1);
            } अन्यथा {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                प्रतिज्ञा(ptr2 == slot && ptr != slot);
            }
            भङ्ग;
        }

        स्वभाव:
            fail("Expected recursion to result in Zombie.");
        }
    } अन्यथा {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        प्रतिज्ञा(ptr != slot);

        यदि (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            प्रतिज्ञा(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    प्रत्यागम EXCEPTION_NONE;
}

शून्य cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    यदि (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret अप्रयुक्तम्;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        प्रतिज्ञा(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        रिक्तानि_Slot(slot, cap_null_cap_new());
    }
}

शून्य निवेशय_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *अग्र;

    अग्र = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_ग्रन्थिः_new(CTE_REF(अग्र), true, true, CTE_REF(parent));
    यदि (अग्र) {
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_पूर्व(&अग्र->cteMDBNode, CTE_REF(slot));
    }
    mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_अग्र(&parent->cteMDBNode, CTE_REF(slot));
}

#यदि_अपरिभाषितम् CONFIG_KERNEL_MCS
शून्य सज्जता_प्रत्युत्तरम्_Master(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcb_प्रत्युत्तरम्);
    यदि (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_प्रत्युत्तरम्_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_ग्रन्थिः_ptr_समुच्चयः_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#यदि_अन्तः

bool_t विशुद्धम् is_MDBParent_सम्बन्धि(cte_t *cte_a, cte_t *cte_b)
{
    यदि (!mdb_ग्रन्थिः_get_mdb_Revocable(cte_a->cteMDBNode)) {
        प्रत्यागम false;
    }
    यदि (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        प्रत्यागम false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    यदि (!Arch_is_MDBParent_सम्बन्धि(cte_a->cap, cte_b->cap,
                            mdb_ग्रन्थिः_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        प्रत्यागम false;
    }
    विकल्प (cap_get_capType(cte_a->cap)) {
    पक्ष cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        यदि (badge == 0) {
            प्रत्यागम true;
        }
        प्रत्यागम (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_ग्रन्थिः_get_mdb_First_Badged(cte_b->cteMDBNode);
        भङ्ग;
    }

    पक्ष cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        यदि (badge == 0) {
            प्रत्यागम true;
        }
        प्रत्यागम (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_ग्रन्थिः_get_mdb_First_Badged(cte_b->cteMDBNode);
        भङ्ग;
    }

    स्वभाव:
        प्रत्यागम true;
        भङ्ग;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    यदि (mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode) != 0) {
        cte_t *अग्र;

        अग्र = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(slot->cteMDBNode));
        यदि (is_MDBParent_सम्बन्धि(slot, अग्र)) {
            current_syscall_error.type = seL4_RevokeFirst;
            प्रत्यागम EXCEPTION_SYSCALL_ERROR;
        }
    }

    प्रत्यागम EXCEPTION_NONE;
}

exception_t ensure_रिक्तानि_Slot(cte_t *slot)
{
    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    प्रत्यागम EXCEPTION_NONE;
}

bool_t विशुद्धम् isFinalCapability(cte_t *cte)
{
    mdb_ग्रन्थिः_t mdb;
    bool_t पूर्व_Is_Same_Object;

    mdb = cte->cteMDBNode;

    यदि (mdb_ग्रन्थिः_get_mdb_पूर्व(mdb) == 0) {
        पूर्व_Is_Same_Object = false;
    } अन्यथा {
        cte_t *पूर्व;

        पूर्व = CTE_PTR(mdb_ग्रन्थिः_get_mdb_पूर्व(mdb));
        पूर्व_Is_Same_Object = sameObjectAs(पूर्व->cap, cte->cap);
    }

    यदि (पूर्व_Is_Same_Object) {
        प्रत्यागम false;
    } अन्यथा {
        यदि (mdb_ग्रन्थिः_get_mdb_अग्र(mdb) == 0) {
            प्रत्यागम true;
        } अन्यथा {
            cte_t *अग्र;

            अग्र = CTE_PTR(mdb_ग्रन्थिः_get_mdb_अग्र(mdb));
            प्रत्यागम !sameObjectAs(cte->cap, अग्र->cap);
        }
    }
}

bool_t विशुद्धम् slotCapLongRunningDelete(cte_t *slot)
{
    यदि (cap_get_capType(slot->cap) == cap_null_cap) {
        प्रत्यागम false;
    } अन्यथा यदि (! isFinalCapability(slot)) {
        प्रत्यागम false;
    }
    विकल्प (cap_get_capType(slot->cap)) {
    पक्ष cap_thread_cap:
    पक्ष cap_zombie_cap:
    पक्ष cap_cnode_cap:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

/* This implementation is specialised to the (current) limit
 * of one cap receive slot. */
cte_t *getReceiveSlots(tcb_t *thread, word_t *बफर)
{
    cap_transfer_t ct;
    cptr_t cptr;
    lookupCap_ret_t luc_ret;
    lookupSlot_ret_t lus_ret;
    cte_t *slot;
    cap_t cnode;

    यदि (!बफर) {
        प्रत्यागम रिक्त;
    }

    ct = आरोपय_Cap_Transfer(बफर);
    cptr = ct.ct_Receive_मूलम्;

    luc_ret = lookupCap(thread, cptr);
    यदि (luc_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम रिक्त;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ct_Receive_सूचकांक, ct.ct_Receive_गभीरता);
    यदि (lus_ret.स्थितिः != EXCEPTION_NONE) {
        प्रत्यागम रिक्त;
    }
    slot = lus_ret.slot;

    यदि (cap_get_capType(slot->cap) != cap_null_cap) {
        प्रत्यागम रिक्त;
    }

    प्रत्यागम slot;
}

cap_transfer_t विशुद्धम् आरोपय_Cap_Transfer(word_t *बफर)
{
    ध्रुव पूर्णाङ्क विस्थापनम् = se_L4_सन्देशः_Max_दीर्घता + se_L4_सन्देशः_Max_Extra_Caps + 2;
    प्रत्यागम cap_Transfer_From_शब्दाः(बफर + विस्थापनम्);
}
