/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <api/syscall.h>
#അന്തര്ഭാവ <api/invocation.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <object/untyped.h>
#അന്തര്ഭാവ <object/objecttype.h>
#അന്തര്ഭാവ <object/cnode.h>
#അന്തര്ഭാവ <kernel/cspace.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <util.h>

സ്ഥിതം word_t alignUp(word_t base_മൂല്യം, word_t alignment)
{
    തിരികെ (base_മൂല്യം + (BIT(alignment) - 1)) & ~MASK(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t length, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *buffer)
{
    word_t newType, userObjSize, കണ്ണി_Index;
    word_t കണ്ണി_Depth, കണ്ണി_Offset, കണ്ണി_Window;
    cte_t *വേര്_Slot;
    exception_t status;
    cap_t കണ്ണി_Cap;
    lookupSlot_ret_t lu_ret;
    word_t കണ്ണി_Size;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, objectSize, untypedFreeBytes;
    word_t freeIndex;
    bool_t deviceMemory;
    bool_t reset;

    /* Ensure operation is valid. */
    എങ്കിൽ (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    എങ്കിൽ (length < 6 || current_extra_caps.excaprefs[0] == ശൂന്യ) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, buffer);
    userObjSize = getSyscallArg(1, buffer);
    കണ്ണി_Index   = getSyscallArg(2, buffer);
    കണ്ണി_Depth   = getSyscallArg(3, buffer);
    കണ്ണി_Offset  = getSyscallArg(4, buffer);
    കണ്ണി_Window  = getSyscallArg(5, buffer);
    വേര്_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    എങ്കിൽ (newType >= se_L4_Object_Type_എണ്ണം) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 0;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    objectSize = getObjectSize(newType, userObjSize);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    എങ്കിൽ (userObjSize >= wordBits || objectSize > seL4_MaxUntypedBits) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = seL4_MaxUntypedBits;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    എങ്കിൽ (newType == seL4_CapTableObject && userObjSize == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    എങ്കിൽ (newType == seL4_UntypedObject && userObjSize < seL4_MinUntypedBits) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

#ifdef CONFIG_KERNEL_MCS
    എങ്കിൽ (newType == seL4_SchedContextObject && userObjSize < seL4_MinSchedContextBits) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
#endif

    /* Lookup the destination CNode (where our caps will be placed in). */
    എങ്കിൽ (കണ്ണി_Depth == 0) {
        കണ്ണി_Cap = വേര്_Slot->cap;
    } അല്ലെങ്കിൽ {
        cap_t വേര്_Cap = വേര്_Slot->cap;
        lu_ret = lookupTargetSlot(വേര്_Cap, കണ്ണി_Index, കണ്ണി_Depth);
        എങ്കിൽ (lu_ret.status != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            തിരികെ lu_ret.status;
        }
        കണ്ണി_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    എങ്കിൽ (cap_get_capType(കണ്ണി_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(കണ്ണി_Depth);
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    കണ്ണി_Size = 1ul << cap_cnode_cap_get_capCNodeRadix(കണ്ണി_Cap);
    എങ്കിൽ (കണ്ണി_Offset > കണ്ണി_Size - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (പൂർണ്ണസംഖ്യ)കണ്ണി_Offset);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = കണ്ണി_Size - 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (കണ്ണി_Window < 1 || കണ്ണി_Window > CONFIG_RETYPE_FAN_OUT_LIMIT) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (പൂർണ്ണസംഖ്യ)കണ്ണി_Window);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_OUT_LIMIT;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }
    എങ്കിൽ (കണ്ണി_Window > കണ്ണി_Size - കണ്ണി_Offset) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = കണ്ണി_Size - കണ്ണി_Offset;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(കണ്ണി_Cap));
    വേണ്ടി (i = കണ്ണി_Offset; i < കണ്ണി_Offset + കണ്ണി_Window; i++) {
        status = ensureEmptySlot(destCNode + i);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (പൂർണ്ണസംഖ്യ)i);
            തിരികെ status;
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
    എങ്കിൽ (status != EXCEPTION_NONE) {
        freeIndex = cap_untyped_cap_get_capFreeIndex(cap);
        reset = false;
    } അല്ലെങ്കിൽ {
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

    എങ്കിൽ ((untypedFreeBytes >> objectSize) < കണ്ണി_Window) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)കണ്ണി_Window,
                  (objectSize >= wordBits ? -1 : (1ul << objectSize)),
                  (word_t)(untypedFreeBytes));
        current_syscall_error.type = seL4_NotEnoughMemory;
        current_syscall_error.memory_ഇടത് = untypedFreeBytes;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    deviceMemory = cap_untyped_cap_get_capIsDevice(cap);
    എങ്കിൽ ((deviceMemory && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_സംഖ്യ = 1;
        തിരികെ EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = alignUp(freeRef, objectSize);

    /* Perform the retype. */
    setThreadState(കണ്ണി_STATE(ksCurThread), ThreadState_Restart);
    തിരികെ invokeUntyped_Retype(slot, reset,
                                (ശൂന്യം *)alignedFreeRef, newType, userObjSize,
                                destCNode, കണ്ണി_Offset, കണ്ണി_Window, deviceMemory);
}

സ്ഥിതം exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t prev_cap = srcSlot->cap;
    word_t block_size = cap_untyped_cap_get_capBlockSize(prev_cap);
    ശൂന്യം *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    പൂർണ്ണസംഖ്യ chunk = CONFIG_RESET_CHUNK_BITS;
    word_t offset = FREE_INDEX_TO_OFFSET(cap_untyped_cap_get_capFreeIndex(prev_cap));
    exception_t status;
    bool_t deviceMemory = cap_untyped_cap_get_capIsDevice(prev_cap);

    എങ്കിൽ (offset == 0) {
        തിരികെ EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    എങ്കിൽ (deviceMemory || block_size < chunk) {
        എങ്കിൽ (! deviceMemory) {
            clearMemory(regionBase, block_size);
        }
        srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, 0);
    } അല്ലെങ്കിൽ {
        വേണ്ടി (offset = ROUND_DOWN(offset - 1, chunk);
             offset != - BIT(chunk); offset -= BIT(chunk)) {
            clearMemory(GET_OFFSET_FREE_PTR(regionBase, offset), chunk);
            srcSlot->cap = cap_untyped_cap_set_capFreeIndex(prev_cap, OFFSET_TO_FREE_INDEX(offset));
            status = preemptionPoint();
            എങ്കിൽ (status != EXCEPTION_NONE) {
                തിരികെ status;
            }
        }
    }
    തിരികെ EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, ശൂന്യം *retypeBase,
                                 object_t newType, word_t userSize,
                                 cte_t *destCNode, word_t destOffset, word_t destLength,
                                 bool_t deviceMemory)
{
    word_t freeRef;
    word_t totalObjectSize;
    ശൂന്യം *regionBase = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t status;

    എങ്കിൽ (reset) {
        status = resetUntypedCap(srcSlot);
        എങ്കിൽ (status != EXCEPTION_NONE) {
            തിരികെ status;
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

    തിരികെ EXCEPTION_NONE;
}
