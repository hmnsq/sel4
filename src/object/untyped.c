/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <api/syscall.h>
#ضمن <api/invocation.h>
#ضمن <machine/io.h>
#ضمن <object/structures.h>
#ضمن <object/untyped.h>
#ضمن <object/objecttype.h>
#ضمن <object/cnode.h>
#ضمن <kernel/cspace.h>
#ضمن <kernel/thread.h>
#ضمن <util.h>

ساكن کلمه_t align_پورته(کلمه_t بنسټ_ارزښت, کلمه_t alignment)
{
    أرجع (بنسټ_ارزښت + (بټ(alignment) - 1)) & ~ماسک(alignment);
}

exception_t decodeUntypedInvocation(کلمه_t invLabel, کلمه_t اوږدتیا, cte_t *خونه,
                                    cap_t cap, bool_t call, کلمه_t *بفر)
{
    کلمه_t newType, user_Obj_اندازه, غوټه_شاخص;
    کلمه_t غوټه_ژوروالی, غوټه_دننه_واټن, غوټه_کړکۍ;
    cte_t *ریښه_خونه;
    exception_t حالت;
    cap_t غوټه_Cap;
    lookup_خونه_ret_t lu_ret;
    کلمه_t غوټه_اندازه;
    کلمه_t i;
    cte_t *destCNode;
    کلمه_t freeRef, alignedFreeRef, object_اندازه, untyped_Free_بایټونه;
    کلمه_t free_شاخص;
    bool_t device_حافظه;
    bool_t reset;

    /* Ensure operation is valid. */
    إذا (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    إذا (اوږدتیا < 6 || current_extra_caps.excaprefs[0] == عدم) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, بفر);
    user_Obj_اندازه = getSyscallArg(1, بفر);
    غوټه_شاخص   = getSyscallArg(2, بفر);
    غوټه_ژوروالی   = getSyscallArg(3, بفر);
    غوټه_دننه_واټن  = getSyscallArg(4, بفر);
    غوټه_کړکۍ  = getSyscallArg(5, بفر);
    ریښه_خونه = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    إذا (newType >= se_L4_Object_Type_شمېر) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    object_اندازه = get_Object_اندازه(newType, user_Obj_اندازه);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    إذا (user_Obj_اندازه >= کلمه_بټونه || object_اندازه > se_L4_Max_Untyped_بټونه) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_بټونه;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    إذا (newType == se_L4_Cap_جدول_Object && user_Obj_اندازه == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    إذا (newType == seL4_UntypedObject && user_Obj_اندازه < se_L4_Min_Untyped_بټونه) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#که_تعریف_شوی CONFIG_KERNEL_MCS
    إذا (newType == seL4_SchedContextObject && user_Obj_اندازه < se_L4_Min_Sched_Context_بټونه) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#د_که_پای

    /* Lookup the destination CNode (where our caps will be placed in). */
    إذا (غوټه_ژوروالی == 0) {
        غوټه_Cap = ریښه_خونه->cap;
    } وإلا {
        cap_t ریښه_Cap = ریښه_خونه->cap;
        lu_ret = lookup_موخه_خونه(ریښه_Cap, غوټه_شاخص, غوټه_ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            أرجع lu_ret.حالت;
        }
        غوټه_Cap = lu_ret.خونه->cap;
    }

    /* Is the destination actually a CNode? */
    إذا (cap_get_capType(غوټه_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(غوټه_ژوروالی);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    غوټه_اندازه = 1ul << cap_cnode_cap_get_capCNodeRadix(غوټه_Cap);
    إذا (غوټه_دننه_واټن > غوټه_اندازه - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (صحيح)غوټه_دننه_واټن);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = غوټه_اندازه - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (غوټه_کړکۍ < 1 || غوټه_کړکۍ > CONFIG_RETYPE_FAN_بهر_حد) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (صحيح)غوټه_کړکۍ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_بهر_حد;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (غوټه_کړکۍ > غوټه_اندازه - غوټه_دننه_واټن) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = غوټه_اندازه - غوټه_دننه_واټن;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(غوټه_Cap));
    لكل (i = غوټه_دننه_واټن; i < غوټه_دننه_واټن + غوټه_کړکۍ; i++) {
        حالت = ensure_تشه_خونه(destCNode + i);
        إذا (حالت != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (صحيح)i);
            أرجع حالت;
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
    حالت = ensureNoChildren(خونه);
    إذا (حالت != EXCEPTION_NONE) {
        free_شاخص = cap_untyped_cap_get_cap_Free_شاخص(cap);
        reset = false;
    } وإلا {
        free_شاخص = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_شاخص);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_بایټونه = بټ(cap_untyped_cap_get_cap_بلاک_اندازه(cap)) -
                       FREE_شاخص_منزل_دننه_واټن(free_شاخص);

    إذا ((untyped_Free_بایټونه >> object_اندازه) < غوټه_کړکۍ) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (کلمه_t)غوټه_کړکۍ,
                  (object_اندازه >= کلمه_بټونه ? -1 : (1ul << object_اندازه)),
                  (کلمه_t)(untyped_Free_بایټونه));
        current_syscall_error.type = se_L4_Not_Enough_حافظه;
        current_syscall_error.حافظه_کیڼ = untyped_Free_بایټونه;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    device_حافظه = cap_untyped_cap_get_capIsDevice(cap);
    إذا ((device_حافظه && !Arch_is_چوکاټ_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_پورته(freeRef, object_اندازه);

    /* Perform the retype. */
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUntyped_Retype(خونه, reset,
                                (فراغ *)alignedFreeRef, newType, user_Obj_اندازه,
                                destCNode, غوټه_دننه_واټن, غوټه_کړکۍ, device_حافظه);
}

ساكن exception_t resetUntypedCap(cte_t *src_خونه)
{
    cap_t prev_cap = src_خونه->cap;
    کلمه_t بلاک_اندازه = cap_untyped_cap_get_cap_بلاک_اندازه(prev_cap);
    فراغ *region_بنسټ = کلمه_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    صحيح chunk = CONFIG_RESET_CHUNK_بټونه;
    کلمه_t دننه_واټن = FREE_شاخص_منزل_دننه_واټن(cap_untyped_cap_get_cap_Free_شاخص(prev_cap));
    exception_t حالت;
    bool_t device_حافظه = cap_untyped_cap_get_capIsDevice(prev_cap);

    إذا (دننه_واټن == 0) {
        أرجع EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    إذا (device_حافظه || بلاک_اندازه < chunk) {
        إذا (! device_حافظه) {
            clear_حافظه(region_بنسټ, بلاک_اندازه);
        }
        src_خونه->cap = cap_untyped_cap_سېټ_cap_Free_شاخص(prev_cap, 0);
    } وإلا {
        لكل (دننه_واټن = پړاو_ښکته(دننه_واټن - 1, chunk);
             دننه_واټن != - بټ(chunk); دننه_واټن -= بټ(chunk)) {
            clear_حافظه(GET_دننه_واټن_FREE_PTR(region_بنسټ, دننه_واټن), chunk);
            src_خونه->cap = cap_untyped_cap_سېټ_cap_Free_شاخص(prev_cap, دننه_واټن_منزل_FREE_شاخص(دننه_واټن));
            حالت = preemption_ټکی();
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_خونه,
                                 bool_t reset, فراغ *retype_بنسټ,
                                 object_t newType, کلمه_t user_اندازه,
                                 cte_t *destCNode, کلمه_t dest_دننه_واټن, کلمه_t dest_اوږدتیا,
                                 bool_t device_حافظه)
{
    کلمه_t freeRef;
    کلمه_t ټول_Object_اندازه;
    فراغ *region_بنسټ = کلمه_PTR(cap_untyped_cap_get_capPtr(src_خونه->cap));
    exception_t حالت;

    إذا (reset) {
        حالت = resetUntypedCap(src_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    ټول_Object_اندازه = dest_اوږدتیا << get_Object_اندازه(newType, user_اندازه);
    freeRef = (کلمه_t)retype_بنسټ + ټول_Object_اندازه;
    src_خونه->cap = cap_untyped_cap_سېټ_cap_Free_شاخص(src_خونه->cap,
                                                    GET_FREE_شاخص(region_بنسټ, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_خونه, destCNode, dest_دننه_واټن, dest_اوږدتیا,
                     retype_بنسټ, user_اندازه, device_حافظه);

    أرجع EXCEPTION_NONE;
}
