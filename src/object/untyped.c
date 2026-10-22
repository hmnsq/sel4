/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/untyped.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/cnode.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

ਸਥਿਰ ਸ਼ਬਦ_t align_ਉੱਪਰ(ਸ਼ਬਦ_t ਅਧਾਰ_ਮੁੱਲ, ਸ਼ਬਦ_t alignment)
{
    ਵਾਪਸ (ਅਧਾਰ_ਮੁੱਲ + (ਦੋਆਧਾਰੀ_ਅੰਕ(alignment) - 1)) & ~ਨਕਾਬ(alignment);
}

exception_t decodeUntypedInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cte_t *ਥਾਂ,
                                    cap_t cap, bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    ਸ਼ਬਦ_t newType, user_Obj_ਮਾਪ, ਗੰਢ_ਸੂਚਕਾਂਕ;
    ਸ਼ਬਦ_t ਗੰਢ_ਡੂੰਘਾਈ, ਗੰਢ_ਵਿੱਥ, ਗੰਢ_ਖਿੜਕੀ;
    cte_t *ਜੜ੍ਹ_ਥਾਂ;
    exception_t ਸਥਿਤੀ;
    cap_t ਗੰਢ_Cap;
    lookup_ਥਾਂ_ret_t lu_ret;
    ਸ਼ਬਦ_t ਗੰਢ_ਮਾਪ;
    ਸ਼ਬਦ_t i;
    cte_t *destCNode;
    ਸ਼ਬਦ_t freeRef, alignedFreeRef, object_ਮਾਪ, untyped_Free_ਬਾਈਟਾਂ;
    ਸ਼ਬਦ_t free_ਸੂਚਕਾਂਕ;
    bool_t device_ਸਮ੍ਰਿਤੀ;
    bool_t reset;

    /* Ensure operation is valid. */
    ਜੇ (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    ਜੇ (ਲੰਮਾਈ < 6 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, ਬਫਰ);
    user_Obj_ਮਾਪ = getSyscallArg(1, ਬਫਰ);
    ਗੰਢ_ਸੂਚਕਾਂਕ   = getSyscallArg(2, ਬਫਰ);
    ਗੰਢ_ਡੂੰਘਾਈ   = getSyscallArg(3, ਬਫਰ);
    ਗੰਢ_ਵਿੱਥ  = getSyscallArg(4, ਬਫਰ);
    ਗੰਢ_ਖਿੜਕੀ  = getSyscallArg(5, ਬਫਰ);
    ਜੜ੍ਹ_ਥਾਂ = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    ਜੇ (newType >= se_L4_Object_Type_ਗਿਣਤੀ) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    object_ਮਾਪ = get_Object_ਮਾਪ(newType, user_Obj_ਮਾਪ);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    ਜੇ (user_Obj_ਮਾਪ >= ਸ਼ਬਦ_ਬਿੱਟ || object_ਮਾਪ > se_L4_Max_Untyped_ਬਿੱਟ) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_ਬਿੱਟ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    ਜੇ (newType == se_L4_Cap_ਸਾਰਣੀ_Object && user_Obj_ਮਾਪ == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    ਜੇ (newType == seL4_UntypedObject && user_Obj_ਮਾਪ < se_L4_Min_Untyped_ਬਿੱਟ) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਜੇ (newType == seL4_SchedContextObject && user_Obj_ਮਾਪ < se_L4_Min_Sched_Context_ਬਿੱਟ) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
#ਜੇ_ਅੰਤ

    /* Lookup the destination CNode (where our caps will be placed in). */
    ਜੇ (ਗੰਢ_ਡੂੰਘਾਈ == 0) {
        ਗੰਢ_Cap = ਜੜ੍ਹ_ਥਾਂ->cap;
    } ਨਹੀਂਤਾਂ {
        cap_t ਜੜ੍ਹ_Cap = ਜੜ੍ਹ_ਥਾਂ->cap;
        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(ਜੜ੍ਹ_Cap, ਗੰਢ_ਸੂਚਕਾਂਕ, ਗੰਢ_ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        ਗੰਢ_Cap = lu_ret.ਥਾਂ->cap;
    }

    /* Is the destination actually a CNode? */
    ਜੇ (cap_get_capType(ਗੰਢ_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ਗੰਢ_ਡੂੰਘਾਈ);
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ਗੰਢ_ਮਾਪ = 1ul << cap_cnode_cap_get_capCNodeRadix(ਗੰਢ_Cap);
    ਜੇ (ਗੰਢ_ਵਿੱਥ > ਗੰਢ_ਮਾਪ - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (ਪੂਰਨ)ਗੰਢ_ਵਿੱਥ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ਗੰਢ_ਮਾਪ - 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਗੰਢ_ਖਿੜਕੀ < 1 || ਗੰਢ_ਖਿੜਕੀ > CONFIG_RETYPE_FAN_ਬਾਹਰਲਾ_ਹੱਦਬੰਦੀ) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (ਪੂਰਨ)ਗੰਢ_ਖਿੜਕੀ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_ਬਾਹਰਲਾ_ਹੱਦਬੰਦੀ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (ਗੰਢ_ਖਿੜਕੀ > ਗੰਢ_ਮਾਪ - ਗੰਢ_ਵਿੱਥ) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ਗੰਢ_ਮਾਪ - ਗੰਢ_ਵਿੱਥ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ਗੰਢ_Cap));
    ਲਈ (i = ਗੰਢ_ਵਿੱਥ; i < ਗੰਢ_ਵਿੱਥ + ਗੰਢ_ਖਿੜਕੀ; i++) {
        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(destCNode + i);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (ਪੂਰਨ)i);
            ਵਾਪਸ ਸਥਿਤੀ;
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
    ਸਥਿਤੀ = ensureNoChildren(ਥਾਂ);
    ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
        free_ਸੂਚਕਾਂਕ = cap_untyped_cap_get_cap_Free_ਸੂਚਕਾਂਕ(cap);
        reset = false;
    } ਨਹੀਂਤਾਂ {
        free_ਸੂਚਕਾਂਕ = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_ਸੂਚਕਾਂਕ);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_ਬਾਈਟਾਂ = ਦੋਆਧਾਰੀ_ਅੰਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(cap)) -
                       FREE_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਵਿੱਥ(free_ਸੂਚਕਾਂਕ);

    ਜੇ ((untyped_Free_ਬਾਈਟਾਂ >> object_ਮਾਪ) < ਗੰਢ_ਖਿੜਕੀ) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (ਸ਼ਬਦ_t)ਗੰਢ_ਖਿੜਕੀ,
                  (object_ਮਾਪ >= ਸ਼ਬਦ_ਬਿੱਟ ? -1 : (1ul << object_ਮਾਪ)),
                  (ਸ਼ਬਦ_t)(untyped_Free_ਬਾਈਟਾਂ));
        current_syscall_error.type = se_L4_Not_Enough_ਸਮ੍ਰਿਤੀ;
        current_syscall_error.ਸਮ੍ਰਿਤੀ_ਖੱਬਾ = untyped_Free_ਬਾਈਟਾਂ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    device_ਸਮ੍ਰਿਤੀ = cap_untyped_cap_get_capIsDevice(cap);
    ਜੇ ((device_ਸਮ੍ਰਿਤੀ && !Arch_is_ਚੌਖਟਾ_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 1;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_ਉੱਪਰ(freeRef, object_ਮਾਪ);

    /* Perform the retype. */
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ invokeUntyped_Retype(ਥਾਂ, reset,
                                (ਖਾਲੀ *)alignedFreeRef, newType, user_Obj_ਮਾਪ,
                                destCNode, ਗੰਢ_ਵਿੱਥ, ਗੰਢ_ਖਿੜਕੀ, device_ਸਮ੍ਰਿਤੀ);
}

ਸਥਿਰ exception_t resetUntypedCap(cte_t *src_ਥਾਂ)
{
    cap_t prev_cap = src_ਥਾਂ->cap;
    ਸ਼ਬਦ_t ਖੰਡ_ਮਾਪ = cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(prev_cap);
    ਖਾਲੀ *region_ਅਧਾਰ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    ਪੂਰਨ chunk = CONFIG_RESET_CHUNK_ਬਿੱਟ;
    ਸ਼ਬਦ_t ਵਿੱਥ = FREE_ਸੂਚਕਾਂਕ_ਮੰਜ਼ਿਲ_ਵਿੱਥ(cap_untyped_cap_get_cap_Free_ਸੂਚਕਾਂਕ(prev_cap));
    exception_t ਸਥਿਤੀ;
    bool_t device_ਸਮ੍ਰਿਤੀ = cap_untyped_cap_get_capIsDevice(prev_cap);

    ਜੇ (ਵਿੱਥ == 0) {
        ਵਾਪਸ EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    ਜੇ (device_ਸਮ੍ਰਿਤੀ || ਖੰਡ_ਮਾਪ < chunk) {
        ਜੇ (! device_ਸਮ੍ਰਿਤੀ) {
            clear_ਸਮ੍ਰਿਤੀ(region_ਅਧਾਰ, ਖੰਡ_ਮਾਪ);
        }
        src_ਥਾਂ->cap = cap_untyped_cap_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(prev_cap, 0);
    } ਨਹੀਂਤਾਂ {
        ਲਈ (ਵਿੱਥ = ਗੇੜਾ_ਥੱਲੇ(ਵਿੱਥ - 1, chunk);
             ਵਿੱਥ != - ਦੋਆਧਾਰੀ_ਅੰਕ(chunk); ਵਿੱਥ -= ਦੋਆਧਾਰੀ_ਅੰਕ(chunk)) {
            clear_ਸਮ੍ਰਿਤੀ(GET_ਵਿੱਥ_FREE_PTR(region_ਅਧਾਰ, ਵਿੱਥ), chunk);
            src_ਥਾਂ->cap = cap_untyped_cap_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(prev_cap, ਵਿੱਥ_ਮੰਜ਼ਿਲ_FREE_ਸੂਚਕਾਂਕ(ਵਿੱਥ));
            ਸਥਿਤੀ = preemption_ਬਿੰਦੂ();
            ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
                ਵਾਪਸ ਸਥਿਤੀ;
            }
        }
    }
    ਵਾਪਸ EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_ਥਾਂ,
                                 bool_t reset, ਖਾਲੀ *retype_ਅਧਾਰ,
                                 object_t newType, ਸ਼ਬਦ_t user_ਮਾਪ,
                                 cte_t *destCNode, ਸ਼ਬਦ_t dest_ਵਿੱਥ, ਸ਼ਬਦ_t dest_ਲੰਮਾਈ,
                                 bool_t device_ਸਮ੍ਰਿਤੀ)
{
    ਸ਼ਬਦ_t freeRef;
    ਸ਼ਬਦ_t ਕੁੱਲ_Object_ਮਾਪ;
    ਖਾਲੀ *region_ਅਧਾਰ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(src_ਥਾਂ->cap));
    exception_t ਸਥਿਤੀ;

    ਜੇ (reset) {
        ਸਥਿਤੀ = resetUntypedCap(src_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    ਕੁੱਲ_Object_ਮਾਪ = dest_ਲੰਮਾਈ << get_Object_ਮਾਪ(newType, user_ਮਾਪ);
    freeRef = (ਸ਼ਬਦ_t)retype_ਅਧਾਰ + ਕੁੱਲ_Object_ਮਾਪ;
    src_ਥਾਂ->cap = cap_untyped_cap_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(src_ਥਾਂ->cap,
                                                    GET_FREE_ਸੂਚਕਾਂਕ(region_ਅਧਾਰ, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_ਥਾਂ, destCNode, dest_ਵਿੱਥ, dest_ਲੰਮਾਈ,
                     retype_ਅਧਾਰ, user_ਮਾਪ, device_ਸਮ੍ਰਿਤੀ);

    ਵਾਪਸ EXCEPTION_NONE;
}
