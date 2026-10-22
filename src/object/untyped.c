/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <api/syscall.h>
#ଅନ୍ତର୍ଭାଵ <api/invocation.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <object/untyped.h>
#ଅନ୍ତର୍ଭାଵ <object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <object/cnode.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <kernel/thread.h>
#ଅନ୍ତର୍ଭାଵ <util.h>

ସ୍ଥିର word_t alignUp(word_t base_ମୂଲ୍ୟ, word_t alignment)
{
    ଫେରନ୍ତୁ (base_ମୂଲ୍ୟ + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, ଗଣ୍ଠି_Index;
    word_t ଗଣ୍ଠି_Depth, ଗଣ୍ଠି_Offset, ଗଣ୍ଠି_Window;
    cte_t *ମୂଳ_Slot;
    exception_t status;
    cap_t ଗଣ୍ଠି_Cap;
    lookupSlot_ret_t lu_ret;
    word_t ଗଣ୍ଠି_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    ଯଦି (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    ଯଦି (length < 6 || current_extra_caps.excaprefs[0] == ଖାଲି) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    ଗଣ୍ଠି_Index   = getSyscallArg(2, buffer);
    ଗଣ୍ଠି_Depth   = getSyscallArg(3, buffer);
    ଗଣ୍ଠି_Offset  = getSyscallArg(4, buffer);
    ଗଣ୍ଠି_Window  = getSyscallArg(5, buffer);
    ମୂଳ_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    ଯଦି (newType >= se_L4_Object_Type_ଗଣନା) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 0;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    ଯଦି (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    ଯଦି (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    ଯଦି (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    ଯଦି (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    ଯଦି (ଗଣ୍ଠି_Depth == 0) {
        ଗଣ୍ଠି_Cap = ମୂଳ_Slot->cap;
    } ଅନ୍ୟଥା {
        cap_t ମୂଳ_Cap = ମୂଳ_Slot->cap;
        lu_ret = lookupTargetSlot(ମୂଳ_Cap, ଗଣ୍ଠି_Index, ଗଣ୍ଠି_Depth);
        ଯଦି (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ଫେରନ୍ତୁ lu_ret.status;
        }
        ଗଣ୍ଠି_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    ଯଦି (cap_get_capType(ଗଣ୍ଠି_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ଗଣ୍ଠି_Depth);
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ଗଣ୍ଠି_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(ଗଣ୍ଠି_Cap);
    ଯଦି (ଗଣ୍ଠି_Offset > ଗଣ୍ଠି_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଗଣ୍ଠି_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ଗଣ୍ଠି_Size - 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (ଗଣ୍ଠି_Window < 1 || ଗଣ୍ଠି_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଗଣ୍ଠି_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }
    ଯଦି (ଗଣ୍ଠି_Window > ଗଣ୍ଠି_Size - ଗଣ୍ଠି_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ଗଣ୍ଠି_Size - ଗଣ୍ଠି_Offset;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ଗଣ୍ଠି_Cap));
    ପାଇଁ (i = ଗଣ୍ଠି_Offset; i < ଗଣ୍ଠି_Offset + ଗଣ୍ଠି_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        ଯଦି (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)i);
            ଫେରନ୍ତୁ status;
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
    ଯଦି (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } ଅନ୍ୟଥା {
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

    ଯଦି ((untypedFreeBytes >> objectSize) < ଗଣ୍ଠି_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)ଗଣ୍ଠି_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_ବାମ = untypedFreeBytes;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    ଯଦି ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_ସଂଖ୍ୟା = 1;
        ଫେରନ୍ତୁ EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(ଗଣ୍ଠି_STATE(ksCurThread), ThreadState_Restart);
    ଫେରନ୍ତୁ invokeUntyped_Retype(slot, reset,
                                (ଶୂନ୍ୟ *)alignedFreeRef, newType, userObjSize,
                                destCNode, ଗଣ୍ଠି_Offset, ଗଣ୍ଠି_Window, deviceMemory);
}

ସ୍ଥିର exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    ଶୂନ୍ୟ *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    ଯଦି (offset == 0) {
        ଫେରନ୍ତୁ EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    ଯଦି (deviceMemory || block_size < chunk) {
        ଯଦି (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } ଅନ୍ୟଥା {
        ପାଇଁ (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            ଯଦି (status != EXCEPTION_NONE) {
                ଫେରନ୍ତୁ status;
            }
        }
    }
    ଫେରନ୍ତୁ EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, ଶୂନ୍ୟ *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    ଶୂନ୍ୟ *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    ଯଦି (reset) {
        status = resetUntypedCap(srcSlot);
        ଯଦି (status != EXCEPTION_NONE) {
            ଫେରନ୍ତୁ status;
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

    ଫେରନ୍ତୁ EXCEPTION_NONE;
}
