/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <assert.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <api/invocation.h>
#समावेश <api/syscall.h>
#समावेश <api/types.h>
#समावेश <machine/io.h>
#समावेश <object/structures.h>
#समावेश <object/objecttype.h>
#समावेश <object/cnode.h>
#समावेश <object/interrupt.h>
#समावेश <object/untyped.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <model/preemption.h>
#समावेश <model/statedata.h>
#समावेश <util.h>

काष्ठा finaliseSlot_ret {
    exception_t status;
    bool_t success;
    cap_t cleanupInfo;
};
प्रकार काष्ठा finaliseSlot_ret finaliseSlot_ret_t;

अटल finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t exposed);
अटल व्योम emptySlot(cte_t *slot, cap_t cleanupInfo);
अटल exception_t reduceZombie(cte_t *slot, bool_t exposed);

#अगर_घोषित CONFIG_KERNEL_MCS
#घोषणा CNODE_LAST_INVOCATION CNodeRotate
#अन्यथा
#घोषणा CNODE_LAST_INVOCATION CNodeSaveCaller
#पूर्ण_अगर

exception_t decodeCNodeInvocation(word_t invLabel, word_t लंबाई, cap_t cap,
                                  word_t *बफर)
{
    lookupSlot_ret_t lu_ret;
    cte_t *destSlot;
    word_t सूचकांक, w_bits;
    exception_t status;

    /* Haskell error: "decodeCNodeInvocation: invalid cap" */
    निश्चित(cap_get_capType(cap) == cap_cnode_cap);

    अगर (invLabel < CNodeRevoke || invLabel > CNODE_LAST_INVOCATION) {
        userError("CNodeCap: Illegal Operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    अगर (लंबाई < 2) {
        userError("CNode operation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    सूचकांक = getSyscallArg(0, बफर);
    w_bits = getSyscallArg(1, बफर);

    lu_ret = lookupTargetSlot(cap, सूचकांक, w_bits);
    अगर (lu_ret.status != EXCEPTION_NONE) {
        userError("CNode operation: Target slot invalid.");
        वापस lu_ret.status;
    }
    destSlot = lu_ret.slot;

    अगर (invLabel >= CNodeCopy && invLabel <= CNodeMutate) {
        cte_t *srcSlot;
        word_t src_सूचकांक, srcDepth, cap_तथ्य;
        bool_t isMove;
        seL4_CapRights_t cap_rights;
        cap_t src_मूल, newCap;
        deriveCap_ret_t dc_ret;
        cap_t srcCap;

        अगर (लंबाई < 4 || current_extra_caps.excaprefs[0] == शून्य) {
            userError("CNode Copy/Mint/Move/Mutate: Truncated message.");
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        src_सूचकांक = getSyscallArg(2, बफर);
        srcDepth = getSyscallArg(3, बफर);

        src_मूल = current_extra_caps.excaprefs[0]->cap;

        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Destination not empty.");
            वापस status;
        }

        lu_ret = lookupSourceSlot(src_मूल, src_सूचकांक, srcDepth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("CNode Copy/Mint/Move/Mutate: Invalid source slot.");
            वापस lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        अगर (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Source slot invalid or empty.");
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault =
                lookup_fault_missing_capability_new(srcDepth);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        चयन (invLabel) {
        हाल CNodeCopy:

            अगर (लंबाई < 5) {
                userError("Truncated message for CNode Copy operation.");
                current_syscall_error.type = seL4_TruncatedMessage;
                वापस EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, बफर));
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot, srcCap);
            अगर (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Copy operation.");
                वापस dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            अवरोध;

        हाल CNodeMint:
            अगर (लंबाई < 6) {
                userError("CNode Mint: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                वापस EXCEPTION_SYSCALL_ERROR;
            }

            cap_rights = rightsFromWord(getSyscallArg(4, बफर));
            cap_तथ्य = getSyscallArg(5, बफर);
            srcCap = maskCapRights(cap_rights, srcSlot->cap);
            dc_ret = deriveCap(srcSlot,
                               update_Cap_तथ्य(false, cap_तथ्य, srcCap));
            अगर (dc_ret.status != EXCEPTION_NONE) {
                userError("Error deriving cap for CNode Mint operation.");
                वापस dc_ret.status;
            }
            newCap = dc_ret.cap;
            isMove = false;

            अवरोध;

        हाल CNodeMove:
            newCap = srcSlot->cap;
            isMove = true;

            अवरोध;

        हाल CNodeMutate:
            अगर (लंबाई < 5) {
                userError("CNode Mutate: Truncated message.");
                current_syscall_error.type = seL4_TruncatedMessage;
                वापस EXCEPTION_SYSCALL_ERROR;
            }

            cap_तथ्य = getSyscallArg(4, बफर);
            newCap = update_Cap_तथ्य(true, cap_तथ्य, srcSlot->cap);
            isMove = true;

            अवरोध;

        शेष:
            निश्चित(0);
            वापस EXCEPTION_NONE;
        }

        अगर (cap_get_capType(newCap) == cap_null_cap) {
            userError("CNode Copy/Mint/Move/Mutate: Mutated cap would be invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        अगर (isMove) {
            वापस invokeCNodeMove(newCap, srcSlot, destSlot);
        } अन्यथा {
            वापस invoke_CNode_जोड़ो(newCap, srcSlot, destSlot);
        }
    }

    अगर (invLabel == CNodeRevoke) {
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeCNodeRevoke(destSlot);
    }

    अगर (invLabel == CNodeDelete) {
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeCNodeDelete(destSlot);
    }

#अगर_अघोषित CONFIG_KERNEL_MCS
    अगर (invLabel == CNodeSaveCaller) {
        status = ensureEmptySlot(destSlot);
        अगर (status != EXCEPTION_NONE) {
            userError("CNode SaveCaller: Destination slot not empty.");
            वापस status;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeCNodeSaveCaller(destSlot);
    }
#पूर्ण_अगर

    अगर (invLabel == CNodeCancelBadgedSends) {
        cap_t destCap;

        destCap = destSlot->cap;

        अगर (!hasCancelSendRights(destCap)) {
            userError("CNode CancelBadgedSends: Target cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeCNodeCancelBadgedSends(destCap);
    }

    अगर (invLabel == CNodeRotate) {
        word_t pivot_New_तथ्य, pivot_सूचकांक, pivotDepth;
        word_t src_New_तथ्य, src_सूचकांक, srcDepth;
        cte_t *pivotSlot, *srcSlot;
        cap_t pivot_मूल, src_मूल, newSrcCap, newPivotCap;

        अगर (लंबाई < 8 || current_extra_caps.excaprefs[0] == शून्य
            || current_extra_caps.excaprefs[1] == शून्य) {
            current_syscall_error.type = seL4_TruncatedMessage;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
        pivot_New_तथ्य = getSyscallArg(2, बफर);
        pivot_सूचकांक   = getSyscallArg(3, बफर);
        pivotDepth   = getSyscallArg(4, बफर);
        src_New_तथ्य   = getSyscallArg(5, बफर);
        src_सूचकांक     = getSyscallArg(6, बफर);
        srcDepth     = getSyscallArg(7, बफर);

        pivot_मूल = current_extra_caps.excaprefs[0]->cap;
        src_मूल   = current_extra_caps.excaprefs[1]->cap;

        lu_ret = lookupSourceSlot(src_मूल, src_सूचकांक, srcDepth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            वापस lu_ret.status;
        }
        srcSlot = lu_ret.slot;

        lu_ret = lookupPivotSlot(pivot_मूल, pivot_सूचकांक, pivotDepth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            वापस lu_ret.status;
        }
        pivotSlot = lu_ret.slot;

        अगर (pivotSlot == srcSlot || pivotSlot == destSlot) {
            userError("CNode Rotate: Pivot slot the same as source or dest slot.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (srcSlot != destSlot) {
            status = ensureEmptySlot(destSlot);
            अगर (status != EXCEPTION_NONE) {
                वापस status;
            }
        }

        अगर (cap_get_capType(srcSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 1;
            current_lookup_fault = lookup_fault_missing_capability_new(srcDepth);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (cap_get_capType(pivotSlot->cap) == cap_null_cap) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = 0;
            current_lookup_fault = lookup_fault_missing_capability_new(pivotDepth);
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        newSrcCap = update_Cap_तथ्य(true, src_New_तथ्य, srcSlot->cap);
        newPivotCap = update_Cap_तथ्य(true, pivot_New_तथ्य, pivotSlot->cap);

        अगर (cap_get_capType(newSrcCap) == cap_null_cap) {
            userError("CNode Rotate: Source cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        अगर (cap_get_capType(newPivotCap) == cap_null_cap) {
            userError("CNode Rotate: Pivot cap invalid.");
            current_syscall_error.type = seL4_IllegalOperation;
            वापस EXCEPTION_SYSCALL_ERROR;
        }

        setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
        वापस invokeCNodeRotate(newSrcCap, newPivotCap,
                                 srcSlot, pivotSlot, destSlot);
    }

    वापस EXCEPTION_NONE;
}

exception_t invokeCNodeRevoke(cte_t *destSlot)
{
    वापस cteRevoke(destSlot);
}

exception_t invokeCNodeDelete(cte_t *destSlot)
{
    वापस cteDelete(destSlot, true);
}

exception_t invokeCNodeCancelBadgedSends(cap_t cap)
{
    word_t badge = cap_endpoint_cap_get_capEPBadge(cap);
    अगर (badge) {
        endpoint_t *ep = (endpoint_t *)
                         cap_endpoint_cap_get_capEPPtr(cap);
        cancelBadgedSends(ep, badge);
    }
    वापस EXCEPTION_NONE;
}

exception_t invoke_CNode_जोड़ो(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cte_जोड़ो(cap, srcSlot, destSlot);

    वापस EXCEPTION_NONE;
}

exception_t invokeCNodeMove(cap_t cap, cte_t *srcSlot, cte_t *destSlot)
{
    cteMove(cap, srcSlot, destSlot);

    वापस EXCEPTION_NONE;
}

exception_t invokeCNodeRotate(cap_t cap1, cap_t cap2, cte_t *slot1,
                              cte_t *slot2, cte_t *slot3)
{
    अगर (slot1 == slot3) {
        cteSwap(cap1, slot1, cap2, slot2);
    } अन्यथा {
        cteMove(cap2, slot2, slot3);
        cteMove(cap1, slot1, slot2);
    }

    वापस EXCEPTION_NONE;
}

#अगर_अघोषित CONFIG_KERNEL_MCS
exception_t invokeCNodeSaveCaller(cte_t *destSlot)
{
    cap_t cap;
    cte_t *srcSlot;

    srcSlot = TCB_PTR_CTE_PTR(गाँठ_STATE(ksCurThread), tcbCaller);
    cap = srcSlot->cap;

    चयन (cap_get_capType(cap)) {
    हाल cap_null_cap:
        userError("CNode SaveCaller: Reply cap not present.");
        अवरोध;

    हाल cap_reply_cap:
        अगर (!cap_reply_cap_get_capReplyMaster(cap)) {
            cteMove(cap, srcSlot, destSlot);
        }
        अवरोध;

    शेष:
        fail("caller capability must be null or reply");
        अवरोध;
    }

    वापस EXCEPTION_NONE;
}
#पूर्ण_अगर

/*
 * If creating a child UntypedCap, don't allow new objects to be created in the
 * parent.
 */
अटल व्योम setUntypedCapAsFull(cap_t srcCap, cap_t newCap, cte_t *srcSlot)
{
    अगर ((cap_get_capType(srcCap) == cap_untyped_cap)
        && (cap_get_capType(newCap) == cap_untyped_cap)) {
        अगर ((cap_untyped_cap_get_capPtr(srcCap)
             == cap_untyped_cap_get_capPtr(newCap))
            && (cap_untyped_cap_get_cap_Block_आकार(newCap)
                == cap_untyped_cap_get_cap_Block_आकार(srcCap))) {
            cap_untyped_cap_ptr_set_cap_Free_सूचकांक(&(srcSlot->cap),
                                                 MAX_FREE_सूचकांक(cap_untyped_cap_get_cap_Block_आकार(srcCap)));
        }
    }
}

व्योम cte_जोड़ो(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाँठ_t srcMDB, newMDB;
    cap_t srcCap;
    bool_t newCapIsRevocable;

    srcMDB = srcSlot->cteMDBNode;
    srcCap = srcSlot->cap;

    newCapIsRevocable = isCapRevocable(newCap, srcCap);

    newMDB = mdb_गाँठ_set_mdb_पूर्व(srcMDB, CTE_REF(srcSlot));
    newMDB = mdb_गाँठ_set_mdb_Revocable(newMDB, newCapIsRevocable);
    newMDB = mdb_गाँठ_set_mdb_First_Badged(newMDB, newCapIsRevocable);

    /* Haskell error: "cteInsert to non-empty destination" */
    निश्चित(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteInsert: mdb entry must be empty" */
    निश्चित((cte_t *)mdb_गाँठ_get_mdb_अग्र(destSlot->cteMDBNode) == शून्य &&
           (cte_t *)mdb_गाँठ_get_mdb_पूर्व(destSlot->cteMDBNode) == शून्य);

    /* Prevent parent untyped cap from being used again if creating a child
     * untyped from it. */
    setUntypedCapAsFull(srcCap, newCap, srcSlot);

    destSlot->cap = newCap;
    destSlot->cteMDBNode = newMDB;
    mdb_गाँठ_ptr_set_mdb_अग्र(&srcSlot->cteMDBNode, CTE_REF(destSlot));
    अगर (mdb_गाँठ_get_mdb_अग्र(newMDB)) {
        mdb_गाँठ_ptr_set_mdb_पूर्व(
            &CTE_PTR(mdb_गाँठ_get_mdb_अग्र(newMDB))->cteMDBNode,
            CTE_REF(destSlot));
    }
}

व्योम cteMove(cap_t newCap, cte_t *srcSlot, cte_t *destSlot)
{
    mdb_गाँठ_t mdb;
    word_t पूर्व_ptr, अग्र_ptr;

    /* Haskell error: "cteMove to non-empty destination" */
    निश्चित(cap_get_capType(destSlot->cap) == cap_null_cap);
    /* Haskell error: "cteMove: mdb entry must be empty" */
    निश्चित((cte_t *)mdb_गाँठ_get_mdb_अग्र(destSlot->cteMDBNode) == शून्य &&
           (cte_t *)mdb_गाँठ_get_mdb_पूर्व(destSlot->cteMDBNode) == शून्य);

    mdb = srcSlot->cteMDBNode;
    destSlot->cap = newCap;
    srcSlot->cap = cap_null_cap_new();
    destSlot->cteMDBNode = mdb;
    srcSlot->cteMDBNode = nullMDBNode;

    पूर्व_ptr = mdb_गाँठ_get_mdb_पूर्व(mdb);
    अगर (पूर्व_ptr)
        mdb_गाँठ_ptr_set_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(destSlot));

    अग्र_ptr = mdb_गाँठ_get_mdb_अग्र(mdb);
    अगर (अग्र_ptr)
        mdb_गाँठ_ptr_set_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(destSlot));
}

व्योम capSwapForDelete(cte_t *slot1, cte_t *slot2)
{
    cap_t cap1, cap2;

    अगर (slot1 == slot2) {
        वापस;
    }

    cap1 = slot1->cap;
    cap2 = slot2->cap;

    cteSwap(cap1, slot1, cap2, slot2);
}

व्योम cteSwap(cap_t cap1, cte_t *slot1, cap_t cap2, cte_t *slot2)
{
    mdb_गाँठ_t mdb1, mdb2;
    word_t अग्र_ptr, पूर्व_ptr;

    slot1->cap = cap2;
    slot2->cap = cap1;

    mdb1 = slot1->cteMDBNode;

    पूर्व_ptr = mdb_गाँठ_get_mdb_पूर्व(mdb1);
    अगर (पूर्व_ptr)
        mdb_गाँठ_ptr_set_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(slot2));

    अग्र_ptr = mdb_गाँठ_get_mdb_अग्र(mdb1);
    अगर (अग्र_ptr)
        mdb_गाँठ_ptr_set_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(slot2));

    mdb2 = slot2->cteMDBNode;
    slot1->cteMDBNode = mdb2;
    slot2->cteMDBNode = mdb1;

    पूर्व_ptr = mdb_गाँठ_get_mdb_पूर्व(mdb2);
    अगर (पूर्व_ptr)
        mdb_गाँठ_ptr_set_mdb_अग्र(
            &CTE_PTR(पूर्व_ptr)->cteMDBNode,
            CTE_REF(slot1));

    अग्र_ptr = mdb_गाँठ_get_mdb_अग्र(mdb2);
    अगर (अग्र_ptr)
        mdb_गाँठ_ptr_set_mdb_पूर्व(
            &CTE_PTR(अग्र_ptr)->cteMDBNode,
            CTE_REF(slot1));
}

exception_t cteRevoke(cte_t *slot)
{
    cte_t *अग्र_Ptr;
    exception_t status;

    /* there is no need to check for a NullCap as NullCaps are
       always accompanied by null mdb pointers */
    क्रम (अग्र_Ptr = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode));
         अग्र_Ptr && isMDBParentOf(slot, अग्र_Ptr);
         अग्र_Ptr = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode))) {
        status = cteDelete(अग्र_Ptr, true);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        status = preemptionPoint();
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }
    }

    वापस EXCEPTION_NONE;
}

exception_t cteDelete(cte_t *slot, bool_t exposed)
{
    finaliseSlot_ret_t fs_ret;

    fs_ret = finaliseSlot(slot, exposed);
    अगर (fs_ret.status != EXCEPTION_NONE) {
        वापस fs_ret.status;
    }

    अगर (exposed || fs_ret.success) {
        emptySlot(slot, fs_ret.cleanupInfo);
    }
    वापस EXCEPTION_NONE;
}

अटल व्योम emptySlot(cte_t *slot, cap_t cleanupInfo)
{
    अगर (cap_get_capType(slot->cap) != cap_null_cap) {
        mdb_गाँठ_t mdb_गाँठ;
        cte_t *पूर्व, *अग्र;

        mdb_गाँठ = slot->cteMDBNode;
        पूर्व = CTE_PTR(mdb_गाँठ_get_mdb_पूर्व(mdb_गाँठ));
        अग्र = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(mdb_गाँठ));

        अगर (पूर्व) {
            mdb_गाँठ_ptr_set_mdb_अग्र(&पूर्व->cteMDBNode, CTE_REF(अग्र));
        }
        अगर (अग्र) {
            mdb_गाँठ_ptr_set_mdb_पूर्व(&अग्र->cteMDBNode, CTE_REF(पूर्व));
        }
        अगर (अग्र)
            mdb_गाँठ_ptr_set_mdb_First_Badged(&अग्र->cteMDBNode,
                                            mdb_गाँठ_get_mdb_First_Badged(अग्र->cteMDBNode) ||
                                            mdb_गाँठ_get_mdb_First_Badged(mdb_गाँठ));
        slot->cap = cap_null_cap_new();
        slot->cteMDBNode = nullMDBNode;

        postCapDeletion(cleanupInfo);
    }
}

अटल अंतरभूत bool_t CONST capRemovable(cap_t cap, cte_t *slot)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_null_cap:
        वापस true;
    हाल cap_zombie_cap: {
        word_t n = cap_zombie_cap_get_cap_Zombie_संख्या(cap);
        cte_t *z_slot = (cte_t *)cap_zombie_cap_get_capZombiePtr(cap);
        वापस (n == 0 || (n == 1 && slot == z_slot));
    }
    शेष:
        fail("finaliseCap should only return Zombie or NullCap");
    }
}

अटल अंतरभूत bool_t CONST capCyclicZombie(cap_t cap, cte_t *slot)
{
    वापस cap_get_capType(cap) == cap_zombie_cap &&
           CTE_PTR(cap_zombie_cap_get_capZombiePtr(cap)) == slot;
}

अटल finaliseSlot_ret_t finaliseSlot(cte_t *slot, bool_t immediate)
{
    bool_t final;
    finaliseCap_ret_t fc_ret;
    exception_t status;
    finaliseSlot_ret_t ret;

    जबतक (cap_get_capType(slot->cap) != cap_null_cap) {
        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, false);

        अगर (capRemovable(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = true;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            वापस ret;
        }

        slot->cap = fc_ret.remainder;

        अगर (!immediate && capCyclicZombie(fc_ret.remainder, slot)) {
            ret.status = EXCEPTION_NONE;
            ret.success = false;
            ret.cleanupInfo = fc_ret.cleanupInfo;
            वापस ret;
        }

        status = reduceZombie(slot, immediate);
        अगर (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            वापस ret;
        }

        status = preemptionPoint();
        अगर (status != EXCEPTION_NONE) {
            ret.status = status;
            ret.success = false;
            ret.cleanupInfo = cap_null_cap_new();
            वापस ret;
        }
    }
    ret.status = EXCEPTION_NONE;
    ret.success = true;
    ret.cleanupInfo = cap_null_cap_new();
    वापस ret;
}

अटल exception_t reduceZombie(cte_t *slot, bool_t immediate)
{
    cte_t *ptr;
    word_t n, type;
    exception_t status;

    निश्चित(cap_get_capType(slot->cap) == cap_zombie_cap);
    ptr = (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);
    n = cap_zombie_cap_get_cap_Zombie_संख्या(slot->cap);
    type = cap_zombie_cap_get_capZombieType(slot->cap);

    /* Haskell error: "reduceZombie: expected unremovable zombie" */
    निश्चित(n > 0);

    अगर (immediate) {
        cte_t *endSlot = &ptr[n - 1];

        status = cteDelete(endSlot, false);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }

        चयन (cap_get_capType(slot->cap)) {
        हाल cap_null_cap:
            अवरोध;

        हाल cap_zombie_cap: {
            cte_t *ptr2 =
                (cte_t *)cap_zombie_cap_get_capZombiePtr(slot->cap);

            अगर (ptr == ptr2 &&
                cap_zombie_cap_get_cap_Zombie_संख्या(slot->cap) == n &&
                cap_zombie_cap_get_capZombieType(slot->cap) == type) {
                निश्चित(cap_get_capType(endSlot->cap) == cap_null_cap);
                slot->cap =
                    cap_zombie_cap_set_cap_Zombie_संख्या(slot->cap, n - 1);
            } अन्यथा {
                /* Haskell error:
                 * "Expected new Zombie to be self-referential."
                 */
                निश्चित(ptr2 == slot && ptr != slot);
            }
            अवरोध;
        }

        शेष:
            fail("Expected recursion to result in Zombie.");
        }
    } अन्यथा {
        /* Haskell error: "Cyclic zombie passed to unexposed reduceZombie" */
        निश्चित(ptr != slot);

        अगर (cap_get_capType(ptr->cap) == cap_zombie_cap) {
            /* Haskell error: "Moving self-referential Zombie aside." */
            निश्चित(ptr != CTE_PTR(cap_zombie_cap_get_capZombiePtr(ptr->cap)));
        }

        capSwapForDelete(ptr, slot);
    }
    वापस EXCEPTION_NONE;
}

व्योम cteDeleteOne(cte_t *slot)
{
    word_t cap_type = cap_get_capType(slot->cap);
    अगर (cap_type != cap_null_cap) {
        bool_t final;
        finaliseCap_ret_t fc_ret UNUSED;

        /** GHOSTUPD: "(gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = (-1)
            \<or> gs_get_assn cteDeleteOne_'proc \<acute>ghost'state = \<acute>cap_type, id)" */

        final = isFinalCapability(slot);
        fc_ret = finaliseCap(slot->cap, final, true);
        /* Haskell error: "cteDeleteOne: cap should be removable" */
        निश्चित(capRemovable(fc_ret.remainder, slot) &&
               cap_get_capType(fc_ret.cleanupInfo) == cap_null_cap);
        emptySlot(slot, cap_null_cap_new());
    }
}

व्योम जोड़ो_New_Cap(cte_t *parent, cte_t *slot, cap_t cap)
{
    cte_t *अग्र;

    अग्र = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(parent->cteMDBNode));
    slot->cap = cap;
    slot->cteMDBNode = mdb_गाँठ_new(CTE_REF(अग्र), true, true, CTE_REF(parent));
    अगर (अग्र) {
        mdb_गाँठ_ptr_set_mdb_पूर्व(&अग्र->cteMDBNode, CTE_REF(slot));
    }
    mdb_गाँठ_ptr_set_mdb_अग्र(&parent->cteMDBNode, CTE_REF(slot));
}

#अगर_अघोषित CONFIG_KERNEL_MCS
व्योम setupReplyMaster(tcb_t *thread)
{
    cte_t *slot;

    slot = TCB_PTR_CTE_PTR(thread, tcbReply);
    अगर (cap_get_capType(slot->cap) == cap_null_cap) {
        /* Haskell asserts that no reply caps exist for this thread here. This
         * cannot be translated. */
        slot->cap = cap_reply_cap_new(true, true, TCB_REF(thread));
        slot->cteMDBNode = nullMDBNode;
        mdb_गाँठ_ptr_set_mdb_Revocable(&slot->cteMDBNode, true);
        mdb_गाँठ_ptr_set_mdb_First_Badged(&slot->cteMDBNode, true);
    }
}
#पूर्ण_अगर

bool_t PURE isMDBParentOf(cte_t *cte_a, cte_t *cte_b)
{
    अगर (!mdb_गाँठ_get_mdb_Revocable(cte_a->cteMDBNode)) {
        वापस false;
    }
    अगर (!sameRegionAs(cte_a->cap, cte_b->cap)) {
        वापस false;
    }
    /* We want to take care of arch caps first, because that is easier in the proofs.
       We expect Arch_isMDBParentOf to return true if both caps are not architecture
       specific. */
    अगर (!Arch_isMDBParentOf(cte_a->cap, cte_b->cap,
                            mdb_गाँठ_get_mdb_First_Badged(cte_b->cteMDBNode))) {
        वापस false;
    }
    चयन (cap_get_capType(cte_a->cap)) {
    हाल cap_endpoint_cap: {
        word_t badge;

        badge = cap_endpoint_cap_get_capEPBadge(cte_a->cap);
        अगर (badge == 0) {
            वापस true;
        }
        वापस (badge == cap_endpoint_cap_get_capEPBadge(cte_b->cap)) &&
               !mdb_गाँठ_get_mdb_First_Badged(cte_b->cteMDBNode);
        अवरोध;
    }

    हाल cap_notification_cap: {
        word_t badge;

        badge = cap_notification_cap_get_capNtfnBadge(cte_a->cap);
        अगर (badge == 0) {
            वापस true;
        }
        वापस (badge == cap_notification_cap_get_capNtfnBadge(cte_b->cap)) &&
               !mdb_गाँठ_get_mdb_First_Badged(cte_b->cteMDBNode);
        अवरोध;
    }

    शेष:
        वापस true;
        अवरोध;
    }
}

exception_t ensureNoChildren(cte_t *slot)
{
    अगर (mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode) != 0) {
        cte_t *अग्र;

        अग्र = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(slot->cteMDBNode));
        अगर (isMDBParentOf(slot, अग्र)) {
            current_syscall_error.type = seL4_RevokeFirst;
            वापस EXCEPTION_SYSCALL_ERROR;
        }
    }

    वापस EXCEPTION_NONE;
}

exception_t ensureEmptySlot(cte_t *slot)
{
    अगर (cap_get_capType(slot->cap) != cap_null_cap) {
        current_syscall_error.type = seL4_DeleteFirst;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    वापस EXCEPTION_NONE;
}

bool_t PURE isFinalCapability(cte_t *cte)
{
    mdb_गाँठ_t mdb;
    bool_t पूर्व_Is_Same_Object;

    mdb = cte->cteMDBNode;

    अगर (mdb_गाँठ_get_mdb_पूर्व(mdb) == 0) {
        पूर्व_Is_Same_Object = false;
    } अन्यथा {
        cte_t *पूर्व;

        पूर्व = CTE_PTR(mdb_गाँठ_get_mdb_पूर्व(mdb));
        पूर्व_Is_Same_Object = sameObjectAs(पूर्व->cap, cte->cap);
    }

    अगर (पूर्व_Is_Same_Object) {
        वापस false;
    } अन्यथा {
        अगर (mdb_गाँठ_get_mdb_अग्र(mdb) == 0) {
            वापस true;
        } अन्यथा {
            cte_t *अग्र;

            अग्र = CTE_PTR(mdb_गाँठ_get_mdb_अग्र(mdb));
            वापस !sameObjectAs(cte->cap, अग्र->cap);
        }
    }
}

bool_t PURE slotCapLongRunningDelete(cte_t *slot)
{
    अगर (cap_get_capType(slot->cap) == cap_null_cap) {
        वापस false;
    } अन्यथा अगर (! isFinalCapability(slot)) {
        वापस false;
    }
    चयन (cap_get_capType(slot->cap)) {
    हाल cap_thread_cap:
    हाल cap_zombie_cap:
    हाल cap_cnode_cap:
        वापस true;
    शेष:
        वापस false;
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

    अगर (!बफर) {
        वापस शून्य;
    }

    ct = loadCapTransfer(बफर);
    cptr = ct.ct_Receive_मूल;

    luc_ret = lookupCap(thread, cptr);
    अगर (luc_ret.status != EXCEPTION_NONE) {
        वापस शून्य;
    }
    cnode = luc_ret.cap;

    lus_ret = lookupTargetSlot(cnode, ct.ct_Receive_सूचकांक, ct.ctReceiveDepth);
    अगर (lus_ret.status != EXCEPTION_NONE) {
        वापस शून्य;
    }
    slot = lus_ret.slot;

    अगर (cap_get_capType(slot->cap) != cap_null_cap) {
        वापस शून्य;
    }

    वापस slot;
}

cap_transfer_t PURE loadCapTransfer(word_t *बफर)
{
    स्थिर पूर्णांक offset = se_L4_संदेश_Max_लंबाई + se_L4_संदेश_Max_Extra_Caps + 2;
    वापस cap_Transfer_From_शब्दगण(बफर + offset);
}
