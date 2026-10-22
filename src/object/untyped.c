/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <util.h>

स्थिर word_t alignUp(word_t base_मानम्, word_t alignment)
{
    पटिनिवत्त (base_मानम् + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, ग्रन्थिः_Index;
    word_t ग्रन्थिः_Depth, ग्रन्थिः_Offset, ग्रन्थिः_Window;
    cte_t *मूलम्_Slot;
    exception_t status;
    cap_t ग्रन्थिः_Cap;
    lookupSlot_ret_t lu_ret;
    word_t ग्रन्थिः_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    सचे (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    सचे (length < 6 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    ग्रन्थिः_Index   = getSyscallArg(2, buffer);
    ग्रन्थिः_Depth   = getSyscallArg(3, buffer);
    ग्रन्थिः_Offset  = getSyscallArg(4, buffer);
    ग्रन्थिः_Window  = getSyscallArg(5, buffer);
    मूलम्_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    सचे (newType >= se_L4_Object_Type_गणनम्) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 0;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    सचे (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    सचे (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    सचे (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    सचे (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    सचे (ग्रन्थिः_Depth == 0) {
        ग्रन्थिः_Cap = मूलम्_Slot->cap;
    } अञ्ञथा {
        cap_t मूलम्_Cap = मूलम्_Slot->cap;
        lu_ret = lookupTargetSlot(मूलम्_Cap, ग्रन्थिः_Index, ग्रन्थिः_Depth);
        सचे (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            पटिनिवत्त lu_ret.status;
        }
        ग्रन्थिः_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    सचे (cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ग्रन्थिः_Depth);
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ग्रन्थिः_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(ग्रन्थिः_Cap);
    सचे (ग्रन्थिः_Offset > ग्रन्थिः_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (पूर्णाङ्क)ग्रन्थिः_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ग्रन्थिः_Size - 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (ग्रन्थिः_Window < 1 || ग्रन्थिः_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (पूर्णाङ्क)ग्रन्थिः_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }
    सचे (ग्रन्थिः_Window > ग्रन्थिः_Size - ग्रन्थिः_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ग्रन्थिः_Size - ग्रन्थिः_Offset;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ग्रन्थिः_Cap));
    अत्थाय (i = ग्रन्थिः_Offset; i < ग्रन्थिः_Offset + ग्रन्थिः_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        सचे (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (पूर्णाङ्क)i);
            पटिनिवत्त status;
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
    सचे (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } अञ्ञथा {
        freeIndex = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), freeIndex);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untypedFreeBytes = BIT(cap_untyped_cap_get_capBlockSize(cap)) -
                       FREE_INDEX_TO_OFFSET(freeIndex);

    सचे ((untypedFreeBytes >> objectSize) < ग्रन्थिः_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)ग्रन्थिः_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_वामः = untypedFreeBytes;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    सचे ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_सङ्ख्या = 1;
        पटिनिवत्त EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    पटिनिवत्त invokeUntyped_Retype(slot, reset,
                                (शून्य *)alignedFreeRef, newType, userObjSize,
                                destCNode, ग्रन्थिः_Offset, ग्रन्थिः_Window, deviceMemory);
}

स्थिर exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    शून्य *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    पूर्णाङ्क chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    सचे (offset == 0) {
        पटिनिवत्त EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    सचे (deviceMemory || block_size < chunk) {
        सचे (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } अञ्ञथा {
        अत्थाय (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            सचे (status != EXCEPTION_NONE) {
                पटिनिवत्त status;
            }
        }
    }
    पटिनिवत्त EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, शून्य *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    शून्य *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    सचे (reset) {
        status = resetUntypedCap(srcSlot);
        सचे (status != EXCEPTION_NONE) {
            पटिनिवत्त status;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    totalObjectSize = destLength << getObjectSize(newType, userSize);
    freeRef = (word_t)retypeBase + totalObjectSize;
    srcSlot->cap = cap_untyped_cap_set_capFreeIndex(srcSlot->cap,
                                                    GET_FREE_INDEX(regionBase, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, srcSlot, destCNode, destOffset, destLength,
                     retypeBase, userSize, deviceMemory);

    पटिनिवत्त EXCEPTION_NONE;
}
