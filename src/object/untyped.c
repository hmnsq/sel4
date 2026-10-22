/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <api/syscall.h>
#समावेश <api/invocation.h>
#समावेश <machine/io.h>
#समावेश <object/structures.h>
#समावेश <object/untyped.h>
#समावेश <object/objecttype.h>
#समावेश <object/cnode.h>
#समावेश <kernel/cspace.h>
#समावेश <kernel/thread.h>
#समावेश <util.h>

अटल word_t alignUp(word_t base_मूल्य, word_t alignment)
{
    वापस (base_मूल्य + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t लंबाई, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *बफर)
{
    word_t newType, user_Obj_आकार, गाँठ_सूचकांक;
    word_t गाँठ_Depth, गाँठ_Offset, गाँठ_Window;
    cte_t *मूल_Slot;
    exception_t status;
    cap_t गाँठ_Cap;
    lookupSlot_ret_t lu_ret;
    word_t गाँठ_आकार;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, object_आकार, untypedFreeBytes;
    word_t free_सूचकांक;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    अगर (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    अगर (लंबाई < 6 || current_extra_caps.excaprefs[0] == शून्य) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, बफर);
    user_Obj_आकार = getSyscallArg(1, बफर);
    गाँठ_सूचकांक   = getSyscallArg(2, बफर);
    गाँठ_Depth   = getSyscallArg(3, बफर);
    गाँठ_Offset  = getSyscallArg(4, बफर);
    गाँठ_Window  = getSyscallArg(5, बफर);
    मूल_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    अगर (newType >= se_L4_Object_Type_गिनती) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 0;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    object_आकार = get_Object_आकार(newType, user_Obj_आकार);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    अगर (user_Obj_आकार >= wordBits || object_आकार > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    अगर (newType == seL4_CapTableObject && user_Obj_आकार == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    अगर (newType == seL4_UntypedObject && user_Obj_आकार < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

#अगर_घोषित CONFIG_KERNEL_MCS
    अगर (newType == seL4_SchedContextObject && user_Obj_आकार < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
#पूर्ण_अगर

    /* Lookup the destination CNode (where our caps will be placed in). */
    अगर (गाँठ_Depth == 0) {
        गाँठ_Cap = मूल_Slot->cap;
    } अन्यथा {
        cap_t मूल_Cap = मूल_Slot->cap;
        lu_ret = lookupTargetSlot(मूल_Cap, गाँठ_सूचकांक, गाँठ_Depth);
        अगर (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            वापस lu_ret.status;
        }
        गाँठ_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    अगर (cap_get_capType(गाँठ_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(गाँठ_Depth);
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    गाँठ_आकार = 1ul << cap_cnode_cap_get_capCNodeRadix(गाँठ_Cap);
    अगर (गाँठ_Offset > गाँठ_आकार - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (पूर्णांक)गाँठ_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = गाँठ_आकार - 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (गाँठ_Window < 1 || गाँठ_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (पूर्णांक)गाँठ_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        वापस EXCEPTION_SYSCALL_ERROR;
    }
    अगर (गाँठ_Window > गाँठ_आकार - गाँठ_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = गाँठ_आकार - गाँठ_Offset;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(गाँठ_Cap));
    क्रम (i = गाँठ_Offset; i < गाँठ_Offset + गाँठ_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        अगर (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (पूर्णांक)i);
            वापस status;
        }
    }

    /*
     * Determine where in the Untyped region we should start allocating new
     * objects.
     *
     * If we have no children, we can start allocating from the beginning of
     * our untyped, regardless of what the "free" value in the cap states.
     * (This may happen if all of the objects beneath us got deleted).
     *
     * If we have children, we just keep allocating from the "free" value
     * recorded in the cap.
     */
    status = ensureNoChildren(slot);
    अगर (status != EXCEPTION_NONE) {
        free_सूचकांक = cap_untyped_cap_get_cap_Free_सूचकांक(cap);
        reset = false;
    } अन्यथा {
        free_सूचकांक = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_सूचकांक);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untypedFreeBytes = BIT(cap_untyped_cap_get_cap_Block_आकार(cap)) -
                       FREE_सूचकांक_TO_OFFSET(free_सूचकांक);

    अगर ((untypedFreeBytes >> object_आकार) < गाँठ_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)गाँठ_Window,
                  (object_आकार >= wordBits ? -1 : (1ul << object_आकार)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_बायाँ = untypedFreeBytes;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    अगर ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_संख्या = 1;
        वापस EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, object_आकार);

    /* Perform the retype. */
    setThreadState(गाँठ_STATE(ksCurThread), ThreadState_Restart);
    वापस invokeUntyped_Retype(slot, reset,
                                (व्योम *)alignedFreeRef, newType, user_Obj_आकार,
                                destCNode, गाँठ_Offset, गाँठ_Window, deviceMemory);
}

अटल exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t पूर्व_cap = srcSlot->cap;
    word_t block_आकार = cap_untyped_cap_get_cap_Block_आकार(पूर्व_cap);
    व्योम *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(पूर्व_cap));
    पूर्णांक chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_सूचकांक_TO_OFFSET(cap_untyped_cap_get_cap_Free_सूचकांक(पूर्व_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(पूर्व_cap);

    अगर (offset == 0) {
        वापस EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    अगर (deviceMemory || block_आकार < chunk) {
        अगर (! deviceMemory) {
            clearMemory(regionBase, block_आकार);
        }
        srcSlot->cap = cap_untyped_cap_set_cap_Free_सूचकांक(पूर्व_cap, 0);
    } अन्यथा {
        क्रम (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_cap_Free_सूचकांक(पूर्व_cap, OFFSET_TO_FREE_सूचकांक(offset));
            status = preemptionPoint();
            अगर (status != EXCEPTION_NONE) {
                वापस status;
            }
        }
    }
    वापस EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, व्योम *retypeBase,
                                 object_t newType, word_t user_आकार,
                                 cte_t *destCNode, word_t destOffset, word_t dest_लंबाई,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t योग_Object_आकार;
    व्योम *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    अगर (reset) {
        status = resetUntypedCap(srcSlot);
        अगर (status != EXCEPTION_NONE) {
            वापस status;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    योग_Object_आकार = dest_लंबाई << get_Object_आकार(newType, user_आकार);
    freeRef = (word_t)retypeBase + योग_Object_आकार;
    srcSlot->cap = cap_untyped_cap_set_cap_Free_सूचकांक(srcSlot->cap,
                                                    GET_FREE_सूचकांक(regionBase, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, srcSlot, destCNode, destOffset, dest_लंबाई,
                     retypeBase, user_आकार, deviceMemory);

    वापस EXCEPTION_NONE;
}
