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

ساكن لفظ_t align_مٿي(لفظ_t بنياد_قدر, لفظ_t alignment)
{
    أرجع (بنياد_قدر + (ٻٽو_انگ(alignment) - 1)) & ~نقاب(alignment);
}

exception_t decodeUntypedInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cte_t *جڳهه,
                                    cap_t cap, bool_t call, لفظ_t *بفر)
{
    لفظ_t newType, user_Obj_سائيز, ڳنڍ_اشاريو;
    لفظ_t ڳنڍ_اونهائي, ڳنڍ_وٿي, ڳنڍ_دري;
    cte_t *پاڙ_جڳهه;
    exception_t حالت;
    cap_t ڳنڍ_Cap;
    lookup_جڳهه_ret_t lu_ret;
    لفظ_t ڳنڍ_سائيز;
    لفظ_t i;
    cte_t *destCNode;
    لفظ_t freeRef, alignedFreeRef, object_سائيز, untyped_Free_بائيٽون;
    لفظ_t free_اشاريو;
    bool_t device_يادگيري;
    bool_t reset;

    /* Ensure operation is valid. */
    إذا (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    إذا (ڊيگهائي < 6 || current_extra_caps.excaprefs[0] == عدم) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, بفر);
    user_Obj_سائيز = getSyscallArg(1, بفر);
    ڳنڍ_اشاريو   = getSyscallArg(2, بفر);
    ڳنڍ_اونهائي   = getSyscallArg(3, بفر);
    ڳنڍ_وٿي  = getSyscallArg(4, بفر);
    ڳنڍ_دري  = getSyscallArg(5, بفر);
    پاڙ_جڳهه = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    إذا (newType >= se_L4_Object_Type_ڳڻپ) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    object_سائيز = get_Object_سائيز(newType, user_Obj_سائيز);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    إذا (user_Obj_سائيز >= لفظ_بٽ || object_سائيز > se_L4_Max_Untyped_بٽ) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_بٽ;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    إذا (newType == se_L4_Cap_جدول_Object && user_Obj_سائيز == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    إذا (newType == seL4_UntypedObject && user_Obj_سائيز < se_L4_Min_Untyped_بٽ) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    إذا (newType == seL4_SchedContextObject && user_Obj_سائيز < se_L4_Min_Sched_Context_بٽ) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جيڪڏهن_پورو

    /* Lookup the destination CNode (where our caps will be placed in). */
    إذا (ڳنڍ_اونهائي == 0) {
        ڳنڍ_Cap = پاڙ_جڳهه->cap;
    } وإلا {
        cap_t پاڙ_Cap = پاڙ_جڳهه->cap;
        lu_ret = lookup_هدف_جڳهه(پاڙ_Cap, ڳنڍ_اشاريو, ڳنڍ_اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            أرجع lu_ret.حالت;
        }
        ڳنڍ_Cap = lu_ret.جڳهه->cap;
    }

    /* Is the destination actually a CNode? */
    إذا (cap_get_capType(ڳنڍ_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ڳنڍ_اونهائي);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ڳنڍ_سائيز = 1ul << cap_cnode_cap_get_capCNodeRadix(ڳنڍ_Cap);
    إذا (ڳنڍ_وٿي > ڳنڍ_سائيز - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (صحيح)ڳنڍ_وٿي);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ڳنڍ_سائيز - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ڳنڍ_دري < 1 || ڳنڍ_دري > CONFIG_RETYPE_FAN_ٻاهريون_حد_بندي) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (صحيح)ڳنڍ_دري);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_ٻاهريون_حد_بندي;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (ڳنڍ_دري > ڳنڍ_سائيز - ڳنڍ_وٿي) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ڳنڍ_سائيز - ڳنڍ_وٿي;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ڳنڍ_Cap));
    لكل (i = ڳنڍ_وٿي; i < ڳنڍ_وٿي + ڳنڍ_دري; i++) {
        حالت = ensure_خالي_جايون_جڳهه(destCNode + i);
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
    حالت = ensureNoChildren(جڳهه);
    إذا (حالت != EXCEPTION_NONE) {
        free_اشاريو = cap_untyped_cap_get_cap_Free_اشاريو(cap);
        reset = false;
    } وإلا {
        free_اشاريو = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_اشاريو);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_بائيٽون = ٻٽو_انگ(cap_untyped_cap_get_cap_بلاڪ_سائيز(cap)) -
                       FREE_اشاريو_منزل_وٿي(free_اشاريو);

    إذا ((untyped_Free_بائيٽون >> object_سائيز) < ڳنڍ_دري) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (لفظ_t)ڳنڍ_دري,
                  (object_سائيز >= لفظ_بٽ ? -1 : (1ul << object_سائيز)),
                  (لفظ_t)(untyped_Free_بائيٽون));
        current_syscall_error.type = se_L4_Not_Enough_يادگيري;
        current_syscall_error.يادگيري_کاٻو = untyped_Free_بائيٽون;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    device_يادگيري = cap_untyped_cap_get_capIsDevice(cap);
    إذا ((device_يادگيري && !Arch_is_چوکٺو_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_مٿي(freeRef, object_سائيز);

    /* Perform the retype. */
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUntyped_Retype(جڳهه, reset,
                                (فراغ *)alignedFreeRef, newType, user_Obj_سائيز,
                                destCNode, ڳنڍ_وٿي, ڳنڍ_دري, device_يادگيري);
}

ساكن exception_t resetUntypedCap(cte_t *src_جڳهه)
{
    cap_t prev_cap = src_جڳهه->cap;
    لفظ_t بلاڪ_سائيز = cap_untyped_cap_get_cap_بلاڪ_سائيز(prev_cap);
    فراغ *region_بنياد = لفظ_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    صحيح chunk = CONFIG_RESET_CHUNK_بٽ;
    لفظ_t وٿي = FREE_اشاريو_منزل_وٿي(cap_untyped_cap_get_cap_Free_اشاريو(prev_cap));
    exception_t حالت;
    bool_t device_يادگيري = cap_untyped_cap_get_capIsDevice(prev_cap);

    إذا (وٿي == 0) {
        أرجع EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    إذا (device_يادگيري || بلاڪ_سائيز < chunk) {
        إذا (! device_يادگيري) {
            clear_يادگيري(region_بنياد, بلاڪ_سائيز);
        }
        src_جڳهه->cap = cap_untyped_cap_سيٽ_cap_Free_اشاريو(prev_cap, 0);
    } وإلا {
        لكل (وٿي = ڦيرو_هيٺ_ڏانهن(وٿي - 1, chunk);
             وٿي != - ٻٽو_انگ(chunk); وٿي -= ٻٽو_انگ(chunk)) {
            clear_يادگيري(GET_وٿي_FREE_PTR(region_بنياد, وٿي), chunk);
            src_جڳهه->cap = cap_untyped_cap_سيٽ_cap_Free_اشاريو(prev_cap, وٿي_منزل_FREE_اشاريو(وٿي));
            حالت = preemption_نقطو();
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_جڳهه,
                                 bool_t reset, فراغ *retype_بنياد,
                                 object_t newType, لفظ_t user_سائيز,
                                 cte_t *destCNode, لفظ_t dest_وٿي, لفظ_t dest_ڊيگهائي,
                                 bool_t device_يادگيري)
{
    لفظ_t freeRef;
    لفظ_t ڪل_Object_سائيز;
    فراغ *region_بنياد = لفظ_PTR(cap_untyped_cap_get_capPtr(src_جڳهه->cap));
    exception_t حالت;

    إذا (reset) {
        حالت = resetUntypedCap(src_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    ڪل_Object_سائيز = dest_ڊيگهائي << get_Object_سائيز(newType, user_سائيز);
    freeRef = (لفظ_t)retype_بنياد + ڪل_Object_سائيز;
    src_جڳهه->cap = cap_untyped_cap_سيٽ_cap_Free_اشاريو(src_جڳهه->cap,
                                                    GET_FREE_اشاريو(region_بنياد, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_جڳهه, destCNode, dest_وٿي, dest_ڊيگهائي,
                     retype_بنياد, user_سائيز, device_يادگيري);

    أرجع EXCEPTION_NONE;
}
