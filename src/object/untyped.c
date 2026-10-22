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

ساكن لفظ_t align_اتے(لفظ_t ادھار_قدر, لفظ_t alignment)
{
    أرجع (ادھار_قدر + (ثنائی_ہندسہ(alignment) - 1)) & ~نقاب(alignment);
}

exception_t decodeUntypedInvocation(لفظ_t invLabel, لفظ_t لمائی, cte_t *تھاں,
                                    cap_t cap, bool_t call, لفظ_t *بفر)
{
    لفظ_t newType, user_Obj_ناپ, گرہ_اشاریہ;
    لفظ_t گرہ_ڈونگھائی, گرہ_وتھ, گرہ_کھڑکی;
    cte_t *جڑ_تھاں;
    exception_t حالت;
    cap_t گرہ_Cap;
    lookup_تھاں_ret_t lu_ret;
    لفظ_t گرہ_ناپ;
    لفظ_t i;
    cte_t *destCNode;
    لفظ_t freeRef, alignedFreeRef, object_ناپ, untyped_Free_بائٹاں;
    لفظ_t free_اشاریہ;
    bool_t device_حافظہ;
    bool_t reset;

    /* Ensure operation is valid. */
    إذا (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    إذا (لمائی < 6 || current_extra_caps.excaprefs[0] == عدم) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, بفر);
    user_Obj_ناپ = getSyscallArg(1, بفر);
    گرہ_اشاریہ   = getSyscallArg(2, بفر);
    گرہ_ڈونگھائی   = getSyscallArg(3, بفر);
    گرہ_وتھ  = getSyscallArg(4, بفر);
    گرہ_کھڑکی  = getSyscallArg(5, بفر);
    جڑ_تھاں = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    إذا (newType >= se_L4_Object_Type_گنتی) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    object_ناپ = get_Object_ناپ(newType, user_Obj_ناپ);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    إذا (user_Obj_ناپ >= لفظ_بٹ || object_ناپ > se_L4_Max_Untyped_بٹ) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_بٹ;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    إذا (newType == se_L4_Cap_جدول_Object && user_Obj_ناپ == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    إذا (newType == seL4_UntypedObject && user_Obj_ناپ < se_L4_Min_Untyped_بٹ) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    إذا (newType == seL4_SchedContextObject && user_Obj_ناپ < se_L4_Min_Sched_Context_بٹ) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#جے_انت

    /* Lookup the destination CNode (where our caps will be placed in). */
    إذا (گرہ_ڈونگھائی == 0) {
        گرہ_Cap = جڑ_تھاں->cap;
    } وإلا {
        cap_t جڑ_Cap = جڑ_تھاں->cap;
        lu_ret = lookup_نشانہ_تھاں(جڑ_Cap, گرہ_اشاریہ, گرہ_ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            أرجع lu_ret.حالت;
        }
        گرہ_Cap = lu_ret.تھاں->cap;
    }

    /* Is the destination actually a CNode? */
    إذا (cap_get_capType(گرہ_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(گرہ_ڈونگھائی);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    گرہ_ناپ = 1ul << cap_cnode_cap_get_capCNodeRadix(گرہ_Cap);
    إذا (گرہ_وتھ > گرہ_ناپ - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (صحيح)گرہ_وتھ);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = گرہ_ناپ - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گرہ_کھڑکی < 1 || گرہ_کھڑکی > CONFIG_RETYPE_FAN_باہرلا_حد_بندی) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (صحيح)گرہ_کھڑکی);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_باہرلا_حد_بندی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گرہ_کھڑکی > گرہ_ناپ - گرہ_وتھ) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = گرہ_ناپ - گرہ_وتھ;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گرہ_Cap));
    لكل (i = گرہ_وتھ; i < گرہ_وتھ + گرہ_کھڑکی; i++) {
        حالت = ensure_خالی_تھاواں_تھاں(destCNode + i);
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
    حالت = ensureNoChildren(تھاں);
    إذا (حالت != EXCEPTION_NONE) {
        free_اشاریہ = cap_untyped_cap_get_cap_Free_اشاریہ(cap);
        reset = false;
    } وإلا {
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
    untyped_Free_بائٹاں = ثنائی_ہندسہ(cap_untyped_cap_get_cap_بلاک_ناپ(cap)) -
                       FREE_اشاریہ_منزل_وتھ(free_اشاریہ);

    إذا ((untyped_Free_بائٹاں >> object_ناپ) < گرہ_کھڑکی) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (لفظ_t)گرہ_کھڑکی,
                  (object_ناپ >= لفظ_بٹ ? -1 : (1ul << object_ناپ)),
                  (لفظ_t)(untyped_Free_بائٹاں));
        current_syscall_error.type = se_L4_Not_Enough_حافظہ;
        current_syscall_error.حافظہ_بایاں = untyped_Free_بائٹاں;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    device_حافظہ = cap_untyped_cap_get_capIsDevice(cap);
    إذا ((device_حافظہ && !Arch_is_چوکھٹا_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_اتے(freeRef, object_ناپ);

    /* Perform the retype. */
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUntyped_Retype(تھاں, reset,
                                (فراغ *)alignedFreeRef, newType, user_Obj_ناپ,
                                destCNode, گرہ_وتھ, گرہ_کھڑکی, device_حافظہ);
}

ساكن exception_t resetUntypedCap(cte_t *src_تھاں)
{
    cap_t prev_cap = src_تھاں->cap;
    لفظ_t بلاک_ناپ = cap_untyped_cap_get_cap_بلاک_ناپ(prev_cap);
    فراغ *region_ادھار = لفظ_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    صحيح chunk = CONFIG_RESET_CHUNK_بٹ;
    لفظ_t وتھ = FREE_اشاریہ_منزل_وتھ(cap_untyped_cap_get_cap_Free_اشاریہ(prev_cap));
    exception_t حالت;
    bool_t device_حافظہ = cap_untyped_cap_get_capIsDevice(prev_cap);

    إذا (وتھ == 0) {
        أرجع EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    إذا (device_حافظہ || بلاک_ناپ < chunk) {
        إذا (! device_حافظہ) {
            clear_حافظہ(region_ادھار, بلاک_ناپ);
        }
        src_تھاں->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(prev_cap, 0);
    } وإلا {
        لكل (وتھ = گیڑا_تھلے(وتھ - 1, chunk);
             وتھ != - ثنائی_ہندسہ(chunk); وتھ -= ثنائی_ہندسہ(chunk)) {
            clear_حافظہ(GET_وتھ_FREE_PTR(region_ادھار, وتھ), chunk);
            src_تھاں->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(prev_cap, وتھ_منزل_FREE_اشاریہ(وتھ));
            حالت = preemption_نقطہ();
            إذا (حالت != EXCEPTION_NONE) {
                أرجع حالت;
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_تھاں,
                                 bool_t reset, فراغ *retype_ادھار,
                                 object_t newType, لفظ_t user_ناپ,
                                 cte_t *destCNode, لفظ_t dest_وتھ, لفظ_t dest_لمائی,
                                 bool_t device_حافظہ)
{
    لفظ_t freeRef;
    لفظ_t کل_Object_ناپ;
    فراغ *region_ادھار = لفظ_PTR(cap_untyped_cap_get_capPtr(src_تھاں->cap));
    exception_t حالت;

    إذا (reset) {
        حالت = resetUntypedCap(src_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    کل_Object_ناپ = dest_لمائی << get_Object_ناپ(newType, user_ناپ);
    freeRef = (لفظ_t)retype_ادھار + کل_Object_ناپ;
    src_تھاں->cap = cap_untyped_cap_سیٹ_cap_Free_اشاریہ(src_تھاں->cap,
                                                    GET_FREE_اشاریہ(region_ادھار, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_تھاں, destCNode, dest_وتھ, dest_لمائی,
                     retype_ادھار, user_ناپ, device_حافظہ);

    أرجع EXCEPTION_NONE;
}
