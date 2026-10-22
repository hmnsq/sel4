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

ಸ್ಥಿರ ಶಬ್ದ_t align_ಮೇಲೆ(ಶಬ್ದ_t ತಳ_ಮೌಲ್ಯ, ಶಬ್ದ_t alignment)
{
    ಹಿಂತಿರುಗಿ (ತಳ_ಮೌಲ್ಯ + (ಬಿಟ್(alignment) - 1)) & ~ಮುಖವಾಡ(alignment);
}

exception_t decodeUntypedInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cte_t *ಸ್ಥಾನ,
                                    cap_t cap, bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    ಶಬ್ದ_t newType, user_Obj_ಗಾತ್ರ, ಗಂಟು_ಸೂಚ್ಯಂಕ;
    ಶಬ್ದ_t ಗಂಟು_ಆಳ, ಗಂಟು_ಸ್ಥಾನಾಂತರ, ಗಂಟು_ಕಿಟಕಿ;
    cte_t *ಬೇರು_ಸ್ಥಾನ;
    exception_t ಸ್ಥಿತಿ_2;
    cap_t ಗಂಟು_Cap;
    lookup_ಸ್ಥಾನ_ret_t lu_ret;
    ಶಬ್ದ_t ಗಂಟು_ಗಾತ್ರ;
    ಶಬ್ದ_t i;
    cte_t *destCNode;
    ಶಬ್ದ_t freeRef, alignedFreeRef, object_ಗಾತ್ರ, untyped_Free_ಬೈಟುಗಳು;
    ಶಬ್ದ_t free_ಸೂಚ್ಯಂಕ;
    bool_t device_ಸ್ಮರಣೆ;
    bool_t reset;

    /* Ensure operation is valid. */
    ಆದರೆ (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    ಆದರೆ (ಉದ್ದಳತೆ < 6 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, ಬಫರ್);
    user_Obj_ಗಾತ್ರ = getSyscallArg(1, ಬಫರ್);
    ಗಂಟು_ಸೂಚ್ಯಂಕ   = getSyscallArg(2, ಬಫರ್);
    ಗಂಟು_ಆಳ   = getSyscallArg(3, ಬಫರ್);
    ಗಂಟು_ಸ್ಥಾನಾಂತರ  = getSyscallArg(4, ಬಫರ್);
    ಗಂಟು_ಕಿಟಕಿ  = getSyscallArg(5, ಬಫರ್);
    ಬೇರು_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    ಆದರೆ (newType >= se_L4_Object_Type_ಎಣಿಕೆ) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    object_ಗಾತ್ರ = get_Object_ಗಾತ್ರ(newType, user_Obj_ಗಾತ್ರ);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    ಆದರೆ (user_Obj_ಗಾತ್ರ >= ಶಬ್ದ_ಬಿಟ್ಟುಗಳು || object_ಗಾತ್ರ > se_L4_Max_Untyped_ಬಿಟ್ಟುಗಳು) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_ಬಿಟ್ಟುಗಳು;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    ಆದರೆ (newType == se_L4_Cap_ಕೋಷ್ಟಕ_Object && user_Obj_ಗಾತ್ರ == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    ಆದರೆ (newType == seL4_UntypedObject && user_Obj_ಗಾತ್ರ < se_L4_Min_Untyped_ಬಿಟ್ಟುಗಳು) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಆದರೆ (newType == seL4_SchedContextObject && user_Obj_ಗಾತ್ರ < se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
#ಆದರೆ_ಅಂತ್ಯ

    /* Lookup the destination CNode (where our caps will be placed in). */
    ಆದರೆ (ಗಂಟು_ಆಳ == 0) {
        ಗಂಟು_Cap = ಬೇರು_ಸ್ಥಾನ->cap;
    } ಇಲ್ಲದಿದ್ದರೆ {
        cap_t ಬೇರು_Cap = ಬೇರು_ಸ್ಥಾನ->cap;
        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(ಬೇರು_Cap, ಗಂಟು_ಸೂಚ್ಯಂಕ, ಗಂಟು_ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        ಗಂಟು_Cap = lu_ret.ಸ್ಥಾನ->cap;
    }

    /* Is the destination actually a CNode? */
    ಆದರೆ (cap_get_capType(ಗಂಟು_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ಗಂಟು_ಆಳ);
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ಗಂಟು_ಗಾತ್ರ = 1ul << cap_cnode_cap_get_capCNodeRadix(ಗಂಟು_Cap);
    ಆದರೆ (ಗಂಟು_ಸ್ಥಾನಾಂತರ > ಗಂಟು_ಗಾತ್ರ - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (ಪೂರ್ಣಾಂಕ)ಗಂಟು_ಸ್ಥಾನಾಂತರ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ಗಂಟು_ಗಾತ್ರ - 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಗಂಟು_ಕಿಟಕಿ < 1 || ಗಂಟು_ಕಿಟಕಿ > CONFIG_RETYPE_FAN_ಹೊರಹೋಗು_ಎಲ್ಲೆ) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (ಪೂರ್ಣಾಂಕ)ಗಂಟು_ಕಿಟಕಿ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_ಹೊರಹೋಗು_ಎಲ್ಲೆ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ಗಂಟು_ಕಿಟಕಿ > ಗಂಟು_ಗಾತ್ರ - ಗಂಟು_ಸ್ಥಾನಾಂತರ) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ಗಂಟು_ಗಾತ್ರ - ಗಂಟು_ಸ್ಥಾನಾಂತರ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ಗಂಟು_Cap));
    ಗಾಗಿ (i = ಗಂಟು_ಸ್ಥಾನಾಂತರ; i < ಗಂಟು_ಸ್ಥಾನಾಂತರ + ಗಂಟು_ಕಿಟಕಿ; i++) {
        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(destCNode + i);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (ಪೂರ್ಣಾಂಕ)i);
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
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
    ಸ್ಥಿತಿ_2 = ensureNoChildren(ಸ್ಥಾನ);
    ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        free_ಸೂಚ್ಯಂಕ = cap_untyped_cap_get_cap_Free_ಸೂಚ್ಯಂಕ(cap);
        reset = false;
    } ಇಲ್ಲದಿದ್ದರೆ {
        free_ಸೂಚ್ಯಂಕ = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_ಸೂಚ್ಯಂಕ);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_ಬೈಟುಗಳು = ಬಿಟ್(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(cap)) -
                       FREE_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಸ್ಥಾನಾಂತರ(free_ಸೂಚ್ಯಂಕ);

    ಆದರೆ ((untyped_Free_ಬೈಟುಗಳು >> object_ಗಾತ್ರ) < ಗಂಟು_ಕಿಟಕಿ) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (ಶಬ್ದ_t)ಗಂಟು_ಕಿಟಕಿ,
                  (object_ಗಾತ್ರ >= ಶಬ್ದ_ಬಿಟ್ಟುಗಳು ? -1 : (1ul << object_ಗಾತ್ರ)),
                  (ಶಬ್ದ_t)(untyped_Free_ಬೈಟುಗಳು));
        current_syscall_error.type = se_L4_Not_Enough_ಸ್ಮರಣೆ;
        current_syscall_error.ಸ್ಮರಣೆ_ಎಡ = untyped_Free_ಬೈಟುಗಳು;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    device_ಸ್ಮರಣೆ = cap_untyped_cap_get_capIsDevice(cap);
    ಆದರೆ ((device_ಸ್ಮರಣೆ && !Arch_is_ಚೌಕಟ್ಟು_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_ಮೇಲೆ(freeRef, object_ಗಾತ್ರ);

    /* Perform the retype. */
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ invokeUntyped_Retype(ಸ್ಥಾನ, reset,
                                (ಶೂನ್ಯ *)alignedFreeRef, newType, user_Obj_ಗಾತ್ರ,
                                destCNode, ಗಂಟು_ಸ್ಥಾನಾಂತರ, ಗಂಟು_ಕಿಟಕಿ, device_ಸ್ಮರಣೆ);
}

ಸ್ಥಿರ exception_t resetUntypedCap(cte_t *src_ಸ್ಥಾನ)
{
    cap_t prev_cap = src_ಸ್ಥಾನ->cap;
    ಶಬ್ದ_t ಖಂಡ_ಗಾತ್ರ = cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(prev_cap);
    ಶೂನ್ಯ *region_ತಳ = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    ಪೂರ್ಣಾಂಕ chunk = CONFIG_RESET_CHUNK_ಬಿಟ್ಟುಗಳು;
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ = FREE_ಸೂಚ್ಯಂಕ_ಗಮ್ಯ_ಸ್ಥಾನಾಂತರ(cap_untyped_cap_get_cap_Free_ಸೂಚ್ಯಂಕ(prev_cap));
    exception_t ಸ್ಥಿತಿ_2;
    bool_t device_ಸ್ಮರಣೆ = cap_untyped_cap_get_capIsDevice(prev_cap);

    ಆದರೆ (ಸ್ಥಾನಾಂತರ == 0) {
        ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    ಆದರೆ (device_ಸ್ಮರಣೆ || ಖಂಡ_ಗಾತ್ರ < chunk) {
        ಆದರೆ (! device_ಸ್ಮರಣೆ) {
            clear_ಸ್ಮರಣೆ(region_ತಳ, ಖಂಡ_ಗಾತ್ರ);
        }
        src_ಸ್ಥಾನ->cap = cap_untyped_cap_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(prev_cap, 0);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(ಸ್ಥಾನಾಂತರ - 1, chunk);
             ಸ್ಥಾನಾಂತರ != - ಬಿಟ್(chunk); ಸ್ಥಾನಾಂತರ -= ಬಿಟ್(chunk)) {
            clear_ಸ್ಮರಣೆ(GET_ಸ್ಥಾನಾಂತರ_FREE_PTR(region_ತಳ, ಸ್ಥಾನಾಂತರ), chunk);
            src_ಸ್ಥಾನ->cap = cap_untyped_cap_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(prev_cap, ಸ್ಥಾನಾಂತರ_ಗಮ್ಯ_FREE_ಸೂಚ್ಯಂಕ(ಸ್ಥಾನಾಂತರ));
            ಸ್ಥಿತಿ_2 = preemption_ಬಿಂದು();
            ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
            }
        }
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_ಸ್ಥಾನ,
                                 bool_t reset, ಶೂನ್ಯ *retype_ತಳ,
                                 object_t newType, ಶಬ್ದ_t user_ಗಾತ್ರ,
                                 cte_t *destCNode, ಶಬ್ದ_t dest_ಸ್ಥಾನಾಂತರ, ಶಬ್ದ_t dest_ಉದ್ದಳತೆ,
                                 bool_t device_ಸ್ಮರಣೆ)
{
    ಶಬ್ದ_t freeRef;
    ಶಬ್ದ_t ಒಟ್ಟು_Object_ಗಾತ್ರ;
    ಶೂನ್ಯ *region_ತಳ = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(src_ಸ್ಥಾನ->cap));
    exception_t ಸ್ಥಿತಿ_2;

    ಆದರೆ (reset) {
        ಸ್ಥಿತಿ_2 = resetUntypedCap(src_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    ಒಟ್ಟು_Object_ಗಾತ್ರ = dest_ಉದ್ದಳತೆ << get_Object_ಗಾತ್ರ(newType, user_ಗಾತ್ರ);
    freeRef = (ಶಬ್ದ_t)retype_ತಳ + ಒಟ್ಟು_Object_ಗಾತ್ರ;
    src_ಸ್ಥಾನ->cap = cap_untyped_cap_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(src_ಸ್ಥಾನ->cap,
                                                    GET_FREE_ಸೂಚ್ಯಂಕ(region_ತಳ, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_ಸ್ಥಾನ, destCNode, dest_ಸ್ಥಾನಾಂತರ, dest_ಉದ್ದಳತೆ,
                     retype_ತಳ, user_ಗಾತ್ರ, device_ಸ್ಮರಣೆ);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}
