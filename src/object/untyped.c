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

ساكن کلمه_t align_به_بالا(کلمه_t پایه_مقدار, کلمه_t alignment)
{
    أرجع (پایه_مقدار + (بیت(alignment) - 1)) & ~ماسک(alignment);
}

exception_t decodeUntypedInvocation(کلمه_t invLabel, کلمه_t درازی, cte_t *جایگاه,
                                    cap_t cap, bool_t call, کلمه_t *میانگیر)
{
    کلمه_t newType, user_Obj_اندازه, گره_شاخص;
    کلمه_t گره_عمق, گره_جابه_جایی, گره_پنجره;
    cte_t *ریشه_جایگاه;
    exception_t وضعیت;
    cap_t گره_Cap;
    lookup_جایگاه_ret_t lu_ret;
    کلمه_t گره_اندازه;
    کلمه_t i;
    cte_t *destCNode;
    کلمه_t freeRef, alignedFreeRef, object_اندازه, untyped_Free_بایتها;
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
    إذا (درازی < 6 || current_extra_caps.excaprefs[0] == عدم) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, میانگیر);
    user_Obj_اندازه = getSyscallArg(1, میانگیر);
    گره_شاخص   = getSyscallArg(2, میانگیر);
    گره_عمق   = getSyscallArg(3, میانگیر);
    گره_جابه_جایی  = getSyscallArg(4, میانگیر);
    گره_پنجره  = getSyscallArg(5, میانگیر);
    ریشه_جایگاه = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    إذا (newType >= se_L4_Object_Type_شمار) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    object_اندازه = get_Object_اندازه(newType, user_Obj_اندازه);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    إذا (user_Obj_اندازه >= کلمه_بیتها || object_اندازه > se_L4_Max_Untyped_بیتها) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_بیتها;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    إذا (newType == se_L4_Cap_جدول_Object && user_Obj_اندازه == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    إذا (newType == seL4_UntypedObject && user_Obj_اندازه < se_L4_Min_Untyped_بیتها) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

#اگر_تعریف_شده CONFIG_KERNEL_MCS
    إذا (newType == seL4_SchedContextObject && user_Obj_اندازه < se_L4_Min_Sched_Context_بیتها) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
#پایان_اگر

    /* Lookup the destination CNode (where our caps will be placed in). */
    إذا (گره_عمق == 0) {
        گره_Cap = ریشه_جایگاه->cap;
    } وإلا {
        cap_t ریشه_Cap = ریشه_جایگاه->cap;
        lu_ret = lookup_هدف_جایگاه(ریشه_Cap, گره_شاخص, گره_عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            أرجع lu_ret.وضعیت;
        }
        گره_Cap = lu_ret.جایگاه->cap;
    }

    /* Is the destination actually a CNode? */
    إذا (cap_get_capType(گره_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(گره_عمق);
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    گره_اندازه = 1ul << cap_cnode_cap_get_capCNodeRadix(گره_Cap);
    إذا (گره_جابه_جایی > گره_اندازه - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (صحيح)گره_جابه_جایی);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = گره_اندازه - 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گره_پنجره < 1 || گره_پنجره > CONFIG_RETYPE_FAN_بیرون_حد) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (صحيح)گره_پنجره);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_بیرون_حد;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (گره_پنجره > گره_اندازه - گره_جابه_جایی) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = گره_اندازه - گره_جابه_جایی;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(گره_Cap));
    لكل (i = گره_جابه_جایی; i < گره_جابه_جایی + گره_پنجره; i++) {
        وضعیت = ensure_خالی_جایگاه(destCNode + i);
        إذا (وضعیت != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (صحيح)i);
            أرجع وضعیت;
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
    وضعیت = ensureNoChildren(جایگاه);
    إذا (وضعیت != EXCEPTION_NONE) {
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
    untyped_Free_بایتها = بیت(cap_untyped_cap_get_cap_بلوک_اندازه(cap)) -
                       FREE_شاخص_مقصد_جابه_جایی(free_شاخص);

    إذا ((untyped_Free_بایتها >> object_اندازه) < گره_پنجره) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (کلمه_t)گره_پنجره,
                  (object_اندازه >= کلمه_بیتها ? -1 : (1ul << object_اندازه)),
                  (کلمه_t)(untyped_Free_بایتها));
        current_syscall_error.type = se_L4_Not_Enough_حافظه;
        current_syscall_error.حافظه_چپ = untyped_Free_بایتها;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    device_حافظه = cap_untyped_cap_get_capIsDevice(cap);
    إذا ((device_حافظه && !Arch_is_قاب_Type(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 1;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_به_بالا(freeRef, object_اندازه);

    /* Perform the retype. */
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع invokeUntyped_Retype(جایگاه, reset,
                                (فراغ *)alignedFreeRef, newType, user_Obj_اندازه,
                                destCNode, گره_جابه_جایی, گره_پنجره, device_حافظه);
}

ساكن exception_t resetUntypedCap(cte_t *src_جایگاه)
{
    cap_t prev_cap = src_جایگاه->cap;
    کلمه_t بلوک_اندازه = cap_untyped_cap_get_cap_بلوک_اندازه(prev_cap);
    فراغ *region_پایه = کلمه_PTR(cap_untyped_cap_get_capPtr(prev_cap));
    صحيح chunk = CONFIG_RESET_CHUNK_بیتها;
    کلمه_t جابه_جایی = FREE_شاخص_مقصد_جابه_جایی(cap_untyped_cap_get_cap_Free_شاخص(prev_cap));
    exception_t وضعیت;
    bool_t device_حافظه = cap_untyped_cap_get_capIsDevice(prev_cap);

    إذا (جابه_جایی == 0) {
        أرجع EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    إذا (device_حافظه || بلوک_اندازه < chunk) {
        إذا (! device_حافظه) {
            clear_حافظه(region_پایه, بلوک_اندازه);
        }
        src_جایگاه->cap = cap_untyped_cap_مجموعه_cap_Free_شاخص(prev_cap, 0);
    } وإلا {
        لكل (جابه_جایی = دور_پایین(جابه_جایی - 1, chunk);
             جابه_جایی != - بیت(chunk); جابه_جایی -= بیت(chunk)) {
            clear_حافظه(GET_جابه_جایی_FREE_PTR(region_پایه, جابه_جایی), chunk);
            src_جایگاه->cap = cap_untyped_cap_مجموعه_cap_Free_شاخص(prev_cap, جابه_جایی_مقصد_FREE_شاخص(جابه_جایی));
            وضعیت = preemption_نقطه();
            إذا (وضعیت != EXCEPTION_NONE) {
                أرجع وضعیت;
            }
        }
    }
    أرجع EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *src_جایگاه,
                                 bool_t reset, فراغ *retype_پایه,
                                 object_t newType, کلمه_t user_اندازه,
                                 cte_t *destCNode, کلمه_t dest_جابه_جایی, کلمه_t dest_درازی,
                                 bool_t device_حافظه)
{
    کلمه_t freeRef;
    کلمه_t مجموع_Object_اندازه;
    فراغ *region_پایه = کلمه_PTR(cap_untyped_cap_get_capPtr(src_جایگاه->cap));
    exception_t وضعیت;

    إذا (reset) {
        وضعیت = resetUntypedCap(src_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    مجموع_Object_اندازه = dest_درازی << get_Object_اندازه(newType, user_اندازه);
    freeRef = (کلمه_t)retype_پایه + مجموع_Object_اندازه;
    src_جایگاه->cap = cap_untyped_cap_مجموعه_cap_Free_شاخص(src_جایگاه->cap,
                                                    GET_FREE_شاخص(region_پایه, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, src_جایگاه, destCNode, dest_جابه_جایی, dest_درازی,
                     retype_پایه, user_اندازه, device_حافظه);

    أرجع EXCEPTION_NONE;
}
