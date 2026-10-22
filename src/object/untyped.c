/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <api/invocation.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <object/untyped.h>
#ಅನ್ತರ್ಭಾವ <object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <object/cnode.h>
#ಅನ್ತರ್ಭಾವ <kernel/cspace.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <util.h>

ಸ್ಥಿರ word_t alignUp(word_t base_ಮೌಲ್ಯ, word_t alignment)
{
    ಪ್ರತ್ಯಾಗಮ (base_ಮೌಲ್ಯ + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, ಗಂಟು_Index;
    word_t ಗಂಟು_Depth, ಗಂಟು_Offset, ಗಂಟು_Window;
    cte_t *ಬೇರು_Slot;
    exception_t status;
    cap_t ಗಂಟು_Cap;
    lookupSlot_ret_t lu_ret;
    word_t ಗಂಟು_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    ಯದಿ (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    ಯದಿ (length < 6 || current_extra_caps.excaprefs[0] == ರಿಕ್ತ) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    ಗಂಟು_Index   = getSyscallArg(2, buffer);
    ಗಂಟು_Depth   = getSyscallArg(3, buffer);
    ಗಂಟು_Offset  = getSyscallArg(4, buffer);
    ಗಂಟು_Window  = getSyscallArg(5, buffer);
    ಬೇರು_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    ಯದಿ (newType >= se_L4_Object_Type_ಎಣಿಕೆ) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 0;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    ಯದಿ (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    ಯದಿ (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    ಯದಿ (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    ಯದಿ (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    ಯದಿ (ಗಂಟು_Depth == 0) {
        ಗಂಟು_Cap = ಬೇರು_Slot->cap;
    } ಅನ್ಯಥಾ {
        cap_t ಬೇರು_Cap = ಬೇರು_Slot->cap;
        lu_ret = lookupTargetSlot(ಬೇರು_Cap, ಗಂಟು_Index, ಗಂಟು_Depth);
        ಯದಿ (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ಪ್ರತ್ಯಾಗಮ lu_ret.status;
        }
        ಗಂಟು_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    ಯದಿ (cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ಗಂಟು_Depth);
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ಗಂಟು_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(ಗಂಟು_Cap);
    ಯದಿ (ಗಂಟು_Offset > ಗಂಟು_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (ಪೂರ್ಣಾಙ್ಕ)ಗಂಟು_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ಗಂಟು_Size - 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (ಗಂಟು_Window < 1 || ಗಂಟು_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (ಪೂರ್ಣಾಙ್ಕ)ಗಂಟು_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }
    ಯದಿ (ಗಂಟು_Window > ಗಂಟು_Size - ಗಂಟು_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ಗಂಟು_Size - ಗಂಟು_Offset;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ಗಂಟು_Cap));
    ಪ್ರತಿ (i = ಗಂಟು_Offset; i < ಗಂಟು_Offset + ಗಂಟು_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        ಯದಿ (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (ಪೂರ್ಣಾಙ್ಕ)i);
            ಪ್ರತ್ಯಾಗಮ status;
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
    ಯದಿ (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } ಅನ್ಯಥಾ {
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

    ಯದಿ ((untypedFreeBytes >> objectSize) < ಗಂಟು_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)ಗಂಟು_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_ಎಡ = untypedFreeBytes;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    ಯದಿ ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ಸಂಖ್ಯೆ = 1;
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಪ್ರತ್ಯಾಗಮ invokeUntyped_Retype(slot, reset,
                                (ಶೂನ್ಯ *)alignedFreeRef, newType, userObjSize,
                                destCNode, ಗಂಟು_Offset, ಗಂಟು_Window, deviceMemory);
}

ಸ್ಥಿರ exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    ಶೂನ್ಯ *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    ಪೂರ್ಣಾಙ್ಕ chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    ಯದಿ (offset == 0) {
        ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    ಯದಿ (deviceMemory || block_size < chunk) {
        ಯದಿ (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } ಅನ್ಯಥಾ {
        ಪ್ರತಿ (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            ಯದಿ (status != EXCEPTION_NONE) {
                ಪ್ರತ್ಯಾಗಮ status;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, ಶೂನ್ಯ *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    ಶೂನ್ಯ *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    ಯದಿ (reset) {
        status = resetUntypedCap(srcSlot);
        ಯದಿ (status != EXCEPTION_NONE) {
            ಪ್ರತ್ಯಾಗಮ status;
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

    ಪ್ರತ್ಯಾಗಮ EXCEPTION_NONE;
}
