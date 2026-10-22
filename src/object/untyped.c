/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <api/invocation.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <object/untyped.h>
#அந்தர்பாவ <object/objecttype.h>
#அந்தர்பாவ <object/cnode.h>
#அந்தர்பாவ <kernel/cspace.h>
#அந்தர்பாவ <kernel/thread.h>
#அந்தர்பாவ <util.h>

நிலையான word_t alignUp(word_t base_மதிப்பு, word_t alignment)
{
    திரும்பு (base_மதிப்பு + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, முனை_Index;
    word_t முனை_Depth, முனை_Offset, முனை_Window;
    cte_t *வேர்_Slot;
    exception_t status;
    cap_t முனை_Cap;
    lookupSlot_ret_t lu_ret;
    word_t முனை_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    எனில் (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    எனில் (length < 6 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    முனை_Index   = getSyscallArg(2, buffer);
    முனை_Depth   = getSyscallArg(3, buffer);
    முனை_Offset  = getSyscallArg(4, buffer);
    முனை_Window  = getSyscallArg(5, buffer);
    வேர்_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    எனில் (newType >= se_L4_Object_Type_மொத்தம்) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 0;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    எனில் (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    எனில் (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    எனில் (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    எனில் (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    எனில் (முனை_Depth == 0) {
        முனை_Cap = வேர்_Slot->cap;
    } இல்லையெனில் {
        cap_t வேர்_Cap = வேர்_Slot->cap;
        lu_ret = lookupTargetSlot(வேர்_Cap, முனை_Index, முனை_Depth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            திரும்பு lu_ret.status;
        }
        முனை_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    எனில் (cap_get_capType(முனை_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(முனை_Depth);
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    முனை_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(முனை_Cap);
    எனில் (முனை_Offset > முனை_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (முழு)முனை_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = முனை_Size - 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (முனை_Window < 1 || முனை_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (முழு)முனை_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (முனை_Window > முனை_Size - முனை_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = முனை_Size - முனை_Offset;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(முனை_Cap));
    க்கு (i = முனை_Offset; i < முனை_Offset + முனை_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        எனில் (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (முழு)i);
            திரும்பு status;
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
    எனில் (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } இல்லையெனில் {
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

    எனில் ((untypedFreeBytes >> objectSize) < முனை_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)முனை_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_இடது = untypedFreeBytes;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    எனில் ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 1;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு invokeUntyped_Retype(slot, reset,
                                (வெறுமை *)alignedFreeRef, newType, userObjSize,
                                destCNode, முனை_Offset, முனை_Window, deviceMemory);
}

நிலையான exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    வெறுமை *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    முழு chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    எனில் (offset == 0) {
        திரும்பு EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    எனில் (deviceMemory || block_size < chunk) {
        எனில் (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } இல்லையெனில் {
        க்கு (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            எனில் (status != EXCEPTION_NONE) {
                திரும்பு status;
            }
        }
    }
    திரும்பு EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, வெறுமை *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    வெறுமை *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    எனில் (reset) {
        status = resetUntypedCap(srcSlot);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
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

    திரும்பு EXCEPTION_NONE;
}
