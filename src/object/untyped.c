/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <api/syscall.h>
#অন্তর্ভাব <api/invocation.h>
#অন্তর্ভাব <machine/io.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <object/untyped.h>
#অন্তর্ভাব <object/objecttype.h>
#অন্তর্ভাব <object/cnode.h>
#অন্তর্ভাব <kernel/cspace.h>
#অন্তর্ভাব <kernel/thread.h>
#অন্তর্ভাব <util.h>

স্থির word_t alignUp(word_t base_মান, word_t alignment)
{
    ফেরত (base_মান + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t দৈর্ঘ্য, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *বাফার)
{
    word_t newType, user_Obj_আকার, গ্রন্থি_সূচক;
    word_t গ্রন্থি_Depth, গ্রন্থি_Offset, গ্রন্থি_Window;
    cte_t *মূল_Slot;
    exception_t status;
    cap_t গ্রন্থি_Cap;
    lookupSlot_ret_t lu_ret;
    word_t গ্রন্থি_আকার;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, object_আকার, untypedFreeBytes;
    word_t free_সূচক;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    যদি (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    যদি (দৈর্ঘ্য < 6 || current_extra_caps.excaprefs[0] == ফাঁকা) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, বাফার);
    user_Obj_আকার = getSyscallArg(1, বাফার);
    গ্রন্থি_সূচক   = getSyscallArg(2, বাফার);
    গ্রন্থি_Depth   = getSyscallArg(3, বাফার);
    গ্রন্থি_Offset  = getSyscallArg(4, বাফার);
    গ্রন্থি_Window  = getSyscallArg(5, বাফার);
    মূল_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    যদি (newType >= se_L4_Object_Type_গুনতি) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 0;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    object_আকার = get_Object_আকার(newType, user_Obj_আকার);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    যদি (user_Obj_আকার >= wordBits || object_আকার > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    যদি (newType == seL4_CapTableObject && user_Obj_আকার == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    যদি (newType == seL4_UntypedObject && user_Obj_আকার < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    যদি (newType == seL4_SchedContextObject && user_Obj_আকার < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    যদি (গ্রন্থি_Depth == 0) {
        গ্রন্থি_Cap = মূল_Slot->cap;
    } অন্যথা {
        cap_t মূল_Cap = মূল_Slot->cap;
        lu_ret = lookupTargetSlot(মূল_Cap, গ্রন্থি_সূচক, গ্রন্থি_Depth);
        যদি (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ফেরত lu_ret.status;
        }
        গ্রন্থি_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    যদি (cap_get_capType(গ্রন্থি_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(গ্রন্থি_Depth);
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    গ্রন্থি_আকার = 1ul << cap_cnode_cap_get_capCNodeRadix(গ্রন্থি_Cap);
    যদি (গ্রন্থি_Offset > গ্রন্থি_আকার - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (পূর্ণসংখ্যা)গ্রন্থি_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = গ্রন্থি_আকার - 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (গ্রন্থি_Window < 1 || গ্রন্থি_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (পূর্ণসংখ্যা)গ্রন্থি_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }
    যদি (গ্রন্থি_Window > গ্রন্থি_আকার - গ্রন্থি_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = গ্রন্থি_আকার - গ্রন্থি_Offset;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(গ্রন্থি_Cap));
    জন্য (i = গ্রন্থি_Offset; i < গ্রন্থি_Offset + গ্রন্থি_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        যদি (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (পূর্ণসংখ্যা)i);
            ফেরত status;
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
        free_সূচক = cap_untyped_cap_get_cap_Free_সূচক(cap);
        reset = false;
    } অন্যথা {
        free_সূচক = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_সূচক);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untypedFreeBytes = BIT(cap_untyped_cap_get_cap_Block_আকার(cap)) -
                       FREE_সূচক_TO_OFFSET(free_সূচক);

    যদি ((untypedFreeBytes >> object_আকার) < গ্রন্থি_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)গ্রন্থি_Window,
                  (object_আকার >= wordBits ? -1 : (1ul << object_আকার)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_বাম = untypedFreeBytes;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    যদি ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_সংখ্যা = 1;
        ফেরত EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, object_আকার);

    /* Perform the retype. */
    setThreadState(গ্রন্থি_STATE(ksCurThread), ThreadState_Restart);
    ফেরত invokeUntyped_Retype(slot, reset,
                                (শূন্য *)alignedFreeRef, newType, user_Obj_আকার,
                                destCNode, গ্রন্থি_Offset, গ্রন্থি_Window, deviceMemory);
}

স্থির exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t পূর্ব_cap = srcSlot->cap;
    word_t block_আকার = cap_untyped_cap_get_cap_Block_আকার(পূর্ব_cap);
    শূন্য *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(পূর্ব_cap));
    পূর্ণসংখ্যা chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_সূচক_TO_OFFSET(cap_untyped_cap_get_cap_Free_সূচক(পূর্ব_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(পূর্ব_cap);

    যদি (offset == 0) {
        ফেরত EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    যদি (deviceMemory || block_আকার < chunk) {
        যদি (! deviceMemory) {
            clearMemory(regionBase, block_আকার);
        }
        srcSlot->cap = cap_untyped_cap_set_cap_Free_সূচক(পূর্ব_cap, 0);
    } অন্যথা {
        জন্য (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_cap_Free_সূচক(পূর্ব_cap, OFFSET_TO_FREE_সূচক(offset));
            status = preemptionPoint();
            যদি (status != EXCEPTION_NONE) {
                ফেরত status;
            }
        }
    }
    ফেরত EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, শূন্য *retypeBase,
                                 object_t newType, word_t user_আকার,
                                 cte_t *destCNode, word_t destOffset, word_t dest_দৈর্ঘ্য,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t যোগ_Object_আকার;
    শূন্য *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    যদি (reset) {
        status = resetUntypedCap(srcSlot);
        যদি (status != EXCEPTION_NONE) {
            ফেরত status;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    যোগ_Object_আকার = dest_দৈর্ঘ্য << get_Object_আকার(newType, user_আকার);
    freeRef = (word_t)retypeBase + যোগ_Object_আকার;
    srcSlot->cap = cap_untyped_cap_set_cap_Free_সূচক(srcSlot->cap,
                                                    GET_FREE_সূচক(regionBase, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, srcSlot, destCNode, destOffset, dest_দৈর্ঘ্য,
                     retypeBase, user_আকার, deviceMemory);

    ফেরত EXCEPTION_NONE;
}
