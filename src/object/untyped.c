/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <api/invocation.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <object/untyped.h>
#అన్తర్భావ <object/objecttype.h>
#అన్తర్భావ <object/cnode.h>
#అన్తర్భావ <kernel/cspace.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <util.h>

స్థిర మాట_t align_పైకి(మాట_t పునాది_విలువ, మాట_t alignment)
{
    తిరిగి (పునాది_విలువ + (బిట్టు(alignment) - 1)) & ~ముసుగు(alignment);
}

exception_t decodeUntypedInvocation(మాట_t invLabel, మాట_t నిడివి, cte_t *స్థానం,
                                    cap_t cap, bool_t call, మాట_t *బఫరు)
{
    మాట_t newType, user_Obj_కొలత, కణుపు_సూచీ;
    మాట_t కణుపు_లోతు, కణుపు_స్థానభ్రంశం, కణుపు_కిటికీ;
    cte_t *మూలం_స్థానం;
    exception_t స్థితి_2;
    cap_t కణుపు_Cap;
    lookup_స్థానం_ret_t lu_ret;
    మాట_t కణుపు_కొలత;
    మాట_t i;
    cte_t *destCNode;
    మాట_t freeRef, alignedFreeRef, object_కొలత, untyped_Free_బైట్లు;
    మాట_t free_సూచీ;
    bool_t device_జ్ఞాపకం;
    bool_t reset;

    /* Ensure operation is valid. */
    ఒకవేళ (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    ఒకవేళ (నిడివి < 6 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, బఫరు);
    user_Obj_కొలత = getSyscallArg(1, బఫరు);
    కణుపు_సూచీ   = getSyscallArg(2, బఫరు);
    కణుపు_లోతు   = getSyscallArg(3, బఫరు);
    కణుపు_స్థానభ్రంశం  = getSyscallArg(4, బఫరు);
    కణుపు_కిటికీ  = getSyscallArg(5, బఫరు);
    మూలం_స్థానం = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    ఒకవేళ (newType >= se_L4_Object_Type_లెక్క) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    object_కొలత = get_Object_కొలత(newType, user_Obj_కొలత);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    ఒకవేళ (user_Obj_కొలత >= మాట_బిట్లు || object_కొలత > se_L4_Max_Untyped_బిట్లు) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_బిట్లు;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    ఒకవేళ (newType == se_L4_Cap_పట్టిక_Object && user_Obj_కొలత == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    ఒకవేళ (newType == seL4_UntypedObject && user_Obj_కొలత < se_L4_Min_Untyped_బిట్లు) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    ఒకవేళ (newType == seL4_SchedContextObject && user_Obj_కొలత < se_L4_Min_Sched_Context_బిట్లు) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
#అయితే_ముగింపు

    /* Lookup the destination CNode (where our caps will be placed in). */
    ఒకవేళ (కణుపు_లోతు == 0) {
        కణుపు_Cap = మూలం_స్థానం->cap;
    } లేకపోతే {
        cap_t మూలం_Cap = మూలం_స్థానం->cap;
        lu_ret = lookup_లక్ష్యం_స్థానం(మూలం_Cap, కణుపు_సూచీ, కణుపు_లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            తిరిగి lu_ret.స్థితి_2;
        }
        కణుపు_Cap = lu_ret.స్థానం->cap;
    }

    /* Is the destination actually a CNode? */
    ఒకవేళ (cap_get_capType(కణుపు_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(కణుపు_లోతు);
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    కణుపు_కొలత = 1ul << cap_cnode_cap_get_capCNodeRadix(కణుపు_Cap);
    ఒకవేళ (కణుపు_స్థానభ్రంశం > కణుపు_కొలత - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (పూర్ణాంకం)కణుపు_స్థానభ్రంశం);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = కణుపు_కొలత - 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (కణుపు_కిటికీ < 1 || కణుపు_కిటికీ > CONFIG_RETYPE_FAN_బహిర్గతం_హద్దు) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (పూర్ణాంకం)కణుపు_కిటికీ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_బహిర్గతం_హద్దు;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (కణుపు_కిటికీ > కణుపు_కొలత - కణుపు_స్థానభ్రంశం) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = కణుపు_కొలత - కణుపు_స్థానభ్రంశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(కణుపు_Cap));
    కొరకు (i = కణుపు_స్థానభ్రంశం; i < కణుపు_స్థానభ్రంశం + కణుపు_కిటికీ; i++) {
        స్థితి_2 = ensure_ఖాళీలు_స్థానం(destCNode + i);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (పూర్ణాంకం)i);
            తిరిగి స్థితి_2;
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
    స్థితి_2 = ensureNoChildren(స్థానం);
    ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
        free_సూచీ = cap_untyped_cap_get_cap_Free_సూచీ(cap);
        reset = false;
    } లేకపోతే {
        free_సూచీ = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_సూచీ);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_బైట్లు = బిట్టు(cap_untyped_cap_get_cap_ఖండం_కొలత(cap)) -
                       FREE_సూచీ_గమ్యం_స్థానభ్రంశం(free_సూచీ);

    ఒకవేళ ((untyped_Free_బైట్లు >> object_కొలత) < కణుపు_కిటికీ) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (మాట_t)కణుపు_కిటికీ,
                  (object_కొలత >= మాట_బిట్లు ? -1 : (1ul << object_కొలత)),
                  (మాట_t)(untyped_Free_బైట్లు));
        current_syscall_error.type = se_L4_Not_Enough_జ్ఞాపకం;
        current_syscall_error.జ్ఞాపకం_ఎడమ = untyped_Free_బైట్లు;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    device_జ్ఞాపకం = cap_untyped_cap_get_capIsDevice(cap);
    ఒకవేళ ((device_జ్ఞాపకం && !Arch_is_చట్రం_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 1;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_పైకి(freeRef, object_కొలత);

    /* Perform the retype. */
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి invokeUntyped_Retype(స్థానం, reset,
                                (శూన్య *)alignedFreeRef, newType, user_Obj_కొలత,
                                destCNode, కణుపు_స్థానభ్రంశం, కణుపు_కిటికీ, device_జ్ఞాపకం);
}

స్థిర exception_t resetUntypedCap(cte_t *src_స్థానం)
{
    cap_t prev_cap = src_స్థానం->cap;
    మాట_t ఖండం_కొలత = cap_untyped_cap_get_cap_ఖండం_కొలత(prev_cap);
    శూన్య *region_పునాది = మాట_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    పూర్ణాంకం chunk = CONFIG_RESET_CHUNK_బిట్లు;
    మాట_t స్థానభ్రంశం = FREE_సూచీ_గమ్యం_స్థానభ్రంశం(cap_untyped_cap_get_cap_Free_సూచీ(prev_cap));
    exception_t స్థితి_2;
    bool_t device_జ్ఞాపకం = cap_untyped_cap_get_capIsDevice(prev_cap);

    ఒకవేళ (స్థానభ్రంశం == 0) {
        తిరిగి EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    ఒకవేళ (device_జ్ఞాపకం || ఖండం_కొలత < chunk) {
        ఒకవేళ (! device_జ్ఞాపకం) {
            clear_జ్ఞాపకం(region_పునాది, ఖండం_కొలత);
        }
        src_స్థానం->cap = cap_untyped_cap_సమితి_cap_Free_సూచీ(prev_cap, 0);
    } లేకపోతే {
        కొరకు (స్థానభ్రంశం = విడత_కిందికి(స్థానభ్రంశం - 1, chunk);
             స్థానభ్రంశం != - బిట్టు(chunk); స్థానభ్రంశం -= బిట్టు(chunk)) {
            clear_జ్ఞాపకం(GET_స్థానభ్రంశం_FREE_PTR(region_పునాది, స్థానభ్రంశం), chunk);
            src_స్థానం->cap = cap_untyped_cap_సమితి_cap_Free_సూచీ(prev_cap, స్థానభ్రంశం_గమ్యం_FREE_సూచీ(స్థానభ్రంశం));
            స్థితి_2 = preemption_బిందువు();
            ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
                తిరిగి స్థితి_2;
            }
        }
    }
    తిరిగి EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_స్థానం,
                                 bool_t reset, శూన్య *retype_పునాది,
                                 object_t newType, మాట_t user_కొలత,
                                 cte_t *destCNode, మాట_t dest_స్థానభ్రంశం, మాట_t dest_నిడివి,
                                 bool_t device_జ్ఞాపకం)
{
    మాట_t freeRef;
    మాట_t మొత్తం_Object_కొలత;
    శూన్య *region_పునాది = మాట_PTR(cap_untyped_cap_get_capPtr(src_స్థానం->cap));
    exception_t స్థితి_2;

    ఒకవేళ (reset) {
        స్థితి_2 = resetUntypedCap(src_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    మొత్తం_Object_కొలత = dest_నిడివి << get_Object_కొలత(newType, user_కొలత);
    freeRef = (మాట_t)retype_పునాది + మొత్తం_Object_కొలత;
    src_స్థానం->cap = cap_untyped_cap_సమితి_cap_Free_సూచీ(src_స్థానం->cap,
                                                    GET_FREE_సూచీ(region_పునాది, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_స్థానం, destCNode, dest_స్థానభ్రంశం, dest_నిడివి,
                     retype_పునాది, user_కొలత, device_జ్ఞాపకం);

    తిరిగి EXCEPTION_NONE;
}
