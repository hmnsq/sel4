/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <api/syscall.h>
#شامل <api/invocation.h>
#شامل <machine/io.h>
#شامل <object/structures.h>
#شامل <object/untyped.h>
#شامل <object/objecttype.h>
#شامل <object/cnode.h>
#شامل <kernel/cspace.h>
#شامل <kernel/thread.h>
#شامل <util.h>

ساکن لفظ_t align_اوپر(لفظ_t بنیاد_قدر, لفظ_t alignment)
{
    واپس (بنیاد_قدر + (ثنائی_ہندسہ(alignment) - 1)) & ~نقاب(alignment);
}

exception_t decodeUntypedInvocation(لفظ_t invLabel, لفظ_t لمبائی, cte_t *خانہ,
                                    cap_t cap, bool_t call, لفظ_t *بفر)
{
    لفظ_t newType, user_Obj_سائز, گرہ_اشاریہ;
    لفظ_t گرہ_گہرائی, گرہ_انحراف, گرہ_کھڑکی;
    cte_t *جڑ_خانہ;
    exception_t حالت_2;
    cap_t گرہ_Cap;
    lookup_خانہ_ret_t lu_ret;
    لفظ_t گرہ_سائز;
    لفظ_t i;
    cte_t *destCNode;
    لفظ_t freeRef, alignedFreeRef, object_سائز, untyped_Free_بائٹیں;
    لفظ_t free_اشاریہ;
    bool_t device_حافظہ;
    bool_t reset;

    /* Ensure operation is valid. */
    اگر (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    اگر (لمبائی < 6 || current_extra_caps.excaprefs[0] == تہی) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, بفر);
    user_Obj_سائز = getSyscallArg(1, بفر);
    گرہ_اشاریہ   = getSyscallArg(2, بفر);
    گرہ_گہرائی   = getSyscallArg(3, بفر);
    گرہ_انحراف  = getSyscallArg(4, بفر);
    گرہ_کھڑکی  = getSyscallArg(5, بفر);
    جڑ_خانہ = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    اگر (newType >= se_L4_Object_Type_گنتی) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    object_سائز = get_Object_سائز(newType, user_Obj_سائز);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    اگر (user_Obj_سائز >= لفظ_بٹ || object_سائز > se_L4_Max_Untyped_بٹ) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_بٹ;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    اگر (newType == se_L4_Cap_جدول_Object && user_Obj_سائز == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    اگر (newType == seL4_UntypedObject && user_Obj_سائز < se_L4_Min_Untyped_بٹ) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شدہ CONFIG_KERNEL_MCS
    اگر (newType == seL4_SchedContextObject && user_Obj_سائز < se_L4_Min_Sched_Context_بٹ) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
#اگر_ختم

    /* Lookup the destination CNode (where our caps will be placed in). */
    اگر (گرہ_گہرائی == 0) {
        گرہ_Cap = جڑ_خانہ->cap;
    } ورنہ {
        cap_t جڑ_Cap = جڑ_خانہ->cap;
        lu_ret = lookup_ہدف_خانہ(جڑ_Cap, گرہ_اشاریہ, گرہ_گہرائی);
        اگر (lu_ret.حالت_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            واپس lu_ret.حالت_2;
        }
        گرہ_Cap = lu_ret.خانہ->cap;
    }

    /* Is the destination actually a CNode? */
    اگر (cap_get_capType(گرہ_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ماخذ = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(گرہ_گہرائی);
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    گرہ_سائز = 1ul << cap_cnode_cap_get_capCNodeRadix(گرہ_Cap);
    اگر (گرہ_انحراف > گرہ_سائز - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (صحیح)گرہ_انحراف);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = گرہ_سائز - 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (گرہ_کھڑکی < 1 || گرہ_کھڑکی > CONFIG_RETYPE_FAN_برآمد_حد_بندی) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (صحیح)گرہ_کھڑکی);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_برآمد_حد_بندی;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
    اگر (گرہ_کھڑکی > گرہ_سائز - گرہ_انحراف) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = گرہ_سائز - گرہ_انحراف;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گرہ_Cap));
    برائے (i = گرہ_انحراف; i < گرہ_انحراف + گرہ_کھڑکی; i++) {
        حالت_2 = ensure_خالی_خانے_خانہ(destCNode + i);
        اگر (حالت_2 != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (صحیح)i);
            واپس حالت_2;
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
    حالت_2 = ensureNoChildren(خانہ);
    اگر (حالت_2 != EXCEPTION_NONE) {
        free_اشاریہ = cap_untyped_cap_get_cap_Free_اشاریہ(cap);
        reset = false;
    } ورنہ {
        free_اشاریہ = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_اشاریہ);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_بائٹیں = ثنائی_ہندسہ(cap_untyped_cap_get_cap_بلاک_سائز(cap)) -
                       FREE_اشاریہ_مقصد_انحراف(free_اشاریہ);

    اگر ((untyped_Free_بائٹیں >> object_سائز) < گرہ_کھڑکی) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (لفظ_t)گرہ_کھڑکی,
                  (object_سائز >= لفظ_بٹ ? -1 : (1ul << object_سائز)),
                  (لفظ_t)(untyped_Free_بائٹیں));
        current_syscall_error.type = se_L4_Not_Enough_حافظہ;
        current_syscall_error.حافظہ_بایاں = untyped_Free_بائٹیں;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    device_حافظہ = cap_untyped_cap_get_capIsDevice(cap);
    اگر ((device_حافظہ && !Arch_is_چوکھٹا_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        واپس EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_اوپر(freeRef, object_سائز);

    /* Perform the retype. */
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    واپس invokeUntyped_Retype(خانہ, reset,
                                (خالی *)alignedFreeRef, newType, user_Obj_سائز,
                                destCNode, گرہ_انحراف, گرہ_کھڑکی, device_حافظہ);
}

ساکن exception_t resetUntypedCap(cte_t *src_خانہ)
{
    cap_t پچھلا_cap = src_خانہ->cap;
    لفظ_t بلاک_سائز = cap_untyped_cap_get_cap_بلاک_سائز(پچھلا_cap);
    خالی *region_بنیاد = لفظ_PTR(cap_untyped_cap_get_capPtr(پچھلا_cap));
    صحیح chunk = CONFIG_RESET_CHUNK_بٹ;
    لفظ_t انحراف = FREE_اشاریہ_مقصد_انحراف(cap_untyped_cap_get_cap_Free_اشاریہ(پچھلا_cap));
    exception_t حالت_2;
    bool_t device_حافظہ = cap_untyped_cap_get_capIsDevice(پچھلا_cap);

    اگر (انحراف == 0) {
        واپس EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    اگر (device_حافظہ || بلاک_سائز < chunk) {
        اگر (! device_حافظہ) {
            clear_حافظہ(region_بنیاد, بلاک_سائز);
        }
        src_خانہ->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(پچھلا_cap, 0);
    } ورنہ {
        برائے (انحراف = باری_نیچے_کو(انحراف - 1, chunk);
             انحراف != - ثنائی_ہندسہ(chunk); انحراف -= ثنائی_ہندسہ(chunk)) {
            clear_حافظہ(GET_انحراف_FREE_PTR(region_بنیاد, انحراف), chunk);
            src_خانہ->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(پچھلا_cap, انحراف_مقصد_FREE_اشاریہ(انحراف));
            حالت_2 = preemption_نقطہ();
            اگر (حالت_2 != EXCEPTION_NONE) {
                واپس حالت_2;
            }
        }
    }
    واپس EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_خانہ,
                                 bool_t reset, خالی *retype_بنیاد,
                                 object_t newType, لفظ_t user_سائز,
                                 cte_t *destCNode, لفظ_t dest_انحراف, لفظ_t dest_لمبائی,
                                 bool_t device_حافظہ)
{
    لفظ_t freeRef;
    لفظ_t کل_Object_سائز;
    خالی *region_بنیاد = لفظ_PTR(cap_untyped_cap_get_capPtr(src_خانہ->cap));
    exception_t حالت_2;

    اگر (reset) {
        حالت_2 = resetUntypedCap(src_خانہ);
        اگر (حالت_2 != EXCEPTION_NONE) {
            واپس حالت_2;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    کل_Object_سائز = dest_لمبائی << get_Object_سائز(newType, user_سائز);
    freeRef = (لفظ_t)retype_بنیاد + کل_Object_سائز;
    src_خانہ->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(src_خانہ->cap,
                                                    GET_FREE_اشاریہ(region_بنیاد, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_خانہ, destCNode, dest_انحراف, dest_لمبائی,
                     retype_بنیاد, user_سائز, device_حافظہ);

    واپس EXCEPTION_NONE;
}
