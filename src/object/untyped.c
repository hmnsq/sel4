/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <api/syscall.h>
#অন্তর্ভাৱ <api/invocation.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <object/structures.h>
#অন্তর্ভাৱ <object/untyped.h>
#অন্তর্ভাৱ <object/objecttype.h>
#অন্তর্ভাৱ <object/cnode.h>
#অন্তর্ভাৱ <kernel/cspace.h>
#অন্তর্ভাৱ <kernel/thread.h>
#অন্তর্ভাৱ <util.h>

স্থির word_t alignUp(word_t base_মান, word_t alignment)
{
    ঘূৰাই (base_মান + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, গ্ৰন্থি_Index;
    word_t গ্ৰন্থি_Depth, গ্ৰন্থি_Offset, গ্ৰন্থি_Window;
    cte_t *মূল_Slot;
    exception_t status;
    cap_t গ্ৰন্থি_Cap;
    lookupSlot_ret_t lu_ret;
    word_t গ্ৰন্থি_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    যদি (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    যদি (length < 6 || current_extra_caps.excaprefs[0] == রিক্ত) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    গ্ৰন্থি_Index   = getSyscallArg(2, buffer);
    গ্ৰন্থি_Depth   = getSyscallArg(3, buffer);
    গ্ৰন্থি_Offset  = getSyscallArg(4, buffer);
    গ্ৰন্থি_Window  = getSyscallArg(5, buffer);
    মূল_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    যদি (newType >= se_L4_Object_Type_গুনতি) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    যদি (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    যদি (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    যদি (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    যদি (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    যদি (গ্ৰন্থি_Depth == 0) {
        গ্ৰন্থি_Cap = মূল_Slot->cap;
    } অন্যথা {
        cap_t মূল_Cap = মূল_Slot->cap;
        lu_ret = lookupTargetSlot(মূল_Cap, গ্ৰন্থি_Index, গ্ৰন্থি_Depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ঘূৰাই lu_ret.status;
        }
        গ্ৰন্থি_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    যদি (cap_get_capType(গ্ৰন্থি_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(গ্ৰন্থি_Depth);
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    গ্ৰন্থি_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(গ্ৰন্থি_Cap);
    যদি (গ্ৰন্থি_Offset > গ্ৰন্থি_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (পূর্ণাঙ্ক)গ্ৰন্থি_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = গ্ৰন্থি_Size - 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (গ্ৰন্থি_Window < 1 || গ্ৰন্থি_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (পূর্ণাঙ্ক)গ্ৰন্থি_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }
    যদি (গ্ৰন্থি_Window > গ্ৰন্থি_Size - গ্ৰন্থি_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = গ্ৰন্থি_Size - গ্ৰন্থি_Offset;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(গ্ৰন্থি_Cap));
    বাবে (i = গ্ৰন্থি_Offset; i < গ্ৰন্থি_Offset + গ্ৰন্থি_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        যদি (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (পূর্ণাঙ্ক)i);
            ঘূৰাই status;
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
    যদি (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } অন্যথা {
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

    যদি ((untypedFreeBytes >> objectSize) < গ্ৰন্থি_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)গ্ৰন্থি_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_বাম = untypedFreeBytes;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    যদি ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ঘূৰাই EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(গ্ৰন্থি_STATE(ksCurThread), ThreadState_Restart);
    ঘূৰাই invokeUntyped_Retype(slot, reset,
                                (শূন্য *)alignedFreeRef, newType, userObjSize,
                                destCNode, গ্ৰন্থি_Offset, গ্ৰন্থি_Window, deviceMemory);
}

স্থির exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    শূন্য *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    পূর্ণাঙ্ক chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    যদি (offset == 0) {
        ঘূৰাই EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    যদি (deviceMemory || block_size < chunk) {
        যদি (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } অন্যথা {
        বাবে (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            যদি (status != EXCEPTION_NONE) {
                ঘূৰাই status;
            }
        }
    }
    ঘূৰাই EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, শূন্য *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    শূন্য *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    যদি (reset) {
        status = resetUntypedCap(srcSlot);
        যদি (status != EXCEPTION_NONE) {
            ঘূৰাই status;
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

    ঘূৰাই EXCEPTION_NONE;
}
