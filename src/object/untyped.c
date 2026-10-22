/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <api/syscall.h>
#अन्तर्भाव <api/invocation.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <object/untyped.h>
#अन्तर्भाव <object/objecttype.h>
#अन्तर्भाव <object/cnode.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <kernel/thread.h>
#अन्तर्भाव <util.h>

स्थिर word_t align_ऊर्ध्वम्(word_t आधारः_मानम्, word_t alignment)
{
    प्रत्यागम (आधारः_मानम् + (BIT(alignment) - 1)) & ~आवरणम्(alignment);
}

exception_t decodeUntypedInvocation(word_t invLabel, word_t दीर्घता, cte_t *slot,
                                    cap_t cap, bool_t call, word_t *बफर)
{
    word_t newType, user_Obj_आकार, ग्रन्थिः_सूचकांक;
    word_t ग्रन्थिः_गभीरता, ग्रन्थिः_विस्थापनम्, ग्रन्थिः_गवाक्षः;
    cte_t *मूलम्_Slot;
    exception_t स्थितिः;
    cap_t ग्रन्थिः_Cap;
    lookupSlot_ret_t lu_ret;
    word_t ग्रन्थिः_आकार;
    word_t i;
    cte_t *destCNode;
    word_t freeRef, alignedFreeRef, object_आकार, untyped_Free_अष्टकानि;
    word_t free_सूचकांक;
    bool_t device_स्मृतिः;
    bool_t reset;

    /* Ensure operation is valid. */
    यदि (invLabel != UntypedRetype) {
        userError("Untyped cap: Illegal operation attempted.");
        current_syscall_error.type = seL4_IllegalOperation;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure message length valid. */
    यदि (दीर्घता < 6 || current_extra_caps.excaprefs[0] == रिक्त) {
        userError("Untyped invocation: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_सन्देशः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Fetch arguments. */
    newType     = getSyscallArg(0, बफर);
    user_Obj_आकार = getSyscallArg(1, बफर);
    ग्रन्थिः_सूचकांक   = getSyscallArg(2, बफर);
    ग्रन्थिः_गभीरता   = getSyscallArg(3, बफर);
    ग्रन्थिः_विस्थापनम्  = getSyscallArg(4, बफर);
    ग्रन्थिः_गवाक्षः  = getSyscallArg(5, बफर);
    मूलम्_Slot = current_extra_caps.excaprefs[0];

    /* Is the requested object type valid? */
    यदि (newType >= se_L4_Object_Type_गणनम्) {
        userError("Untyped Retype: Invalid object type.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 0;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    object_आकार = get_Object_आकार(newType, user_Obj_आकार);

    /* Exclude impossibly large object sizes. getObjectSize can overflow if userObjSize
       is close to 2^wordBits, which is nonsensical in any case, so we check that this
       did not happen. userObjSize will always need to be less than wordBits. */
    यदि (user_Obj_आकार >= word_द्वयङ्काः || object_आकार > se_L4_Max_Untyped_द्वयङ्काः) {
        userError("Untyped Retype: Invalid object size.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = se_L4_Max_Untyped_द्वयङ्काः;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a CNode, is it at least size 1? */
    यदि (newType == se_L4_Cap_सारणी_Object && user_Obj_आकार == 0) {
        userError("Untyped Retype: Requested CapTable size too small.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* If the target object is a Untyped, is it at least size 4? */
    यदि (newType == seL4_UntypedObject && user_Obj_आकार < se_L4_Min_Untyped_द्वयङ्काः) {
        userError("Untyped Retype: Requested UntypedItem size too small.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

#यदि_परिभाषितम् CONFIG_KERNEL_MCS
    यदि (newType == seL4_SchedContextObject && user_Obj_आकार < se_L4_Min_Sched_Context_द्वयङ्काः) {
        userError("Untyped retype: Requested a scheduling context too small.");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
#यदि_अन्तः

    /* Lookup the destination CNode (where our caps will be placed in). */
    यदि (ग्रन्थिः_गभीरता == 0) {
        ग्रन्थिः_Cap = मूलम्_Slot->cap;
    } अन्यथा {
        cap_t मूलम्_Cap = मूलम्_Slot->cap;
        lu_ret = lookupTargetSlot(मूलम्_Cap, ग्रन्थिः_सूचकांक, ग्रन्थिः_गभीरता);
        यदि (lu_ret.स्थितिः != EXCEPTION_NONE) {
            userError("Untyped Retype: Invalid destination address.");
            प्रत्यागम lu_ret.स्थितिः;
        }
        ग्रन्थिः_Cap = lu_ret.slot->cap;
    }

    /* Is the destination actually a CNode? */
    यदि (cap_get_capType(ग्रन्थिः_Cap) != cap_cnode_cap) {
        userError("Untyped Retype: Destination cap invalid or read-only.");
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = 0;
        current_lookup_fault = lookup_fault_missing_capability_new(ग्रन्थिः_गभीरता);
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Is the region where the user wants to put the caps valid? */
    ग्रन्थिः_आकार = 1ul << cap_cnode_cap_get_capCNodeRadix(ग्रन्थिः_Cap);
    यदि (ग्रन्थिः_विस्थापनम् > ग्रन्थिः_आकार - 1) {
        userError("Untyped Retype: Destination node offset #%d too large.",
                  (पूर्णाङ्क)ग्रन्थिः_विस्थापनम्);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ग्रन्थिः_आकार - 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (ग्रन्थिः_गवाक्षः < 1 || ग्रन्थिः_गवाक्षः > CONFIG_RETYPE_FAN_बहिर्गमनम्_सीमा) {
        userError("Untyped Retype: Number of requested objects (%d) too small or large.",
                  (पूर्णाङ्क)ग्रन्थिः_गवाक्षः);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = CONFIG_RETYPE_FAN_बहिर्गमनम्_सीमा;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }
    यदि (ग्रन्थिः_गवाक्षः > ग्रन्थिः_आकार - ग्रन्थिः_विस्थापनम्) {
        userError("Untyped Retype: Requested destination window overruns size of node.");
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 1;
        current_syscall_error.rangeErrorMax = ग्रन्थिः_आकार - ग्रन्थिः_विस्थापनम्;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Ensure that the destination slots are all empty. */
    destCNode = CTE_PTR(cap_cnode_cap_get_capCNodePtr(ग्रन्थिः_Cap));
    प्रति (i = ग्रन्थिः_विस्थापनम्; i < ग्रन्थिः_विस्थापनम् + ग्रन्थिः_गवाक्षः; i++) {
        स्थितिः = ensure_रिक्तानि_Slot(destCNode + i);
        यदि (स्थितिः != EXCEPTION_NONE) {
            userError("Untyped Retype: Slot #%d in destination window non-empty.",
                      (पूर्णाङ्क)i);
            प्रत्यागम स्थितिः;
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
    स्थितिः = ensureNoChildren(slot);
    यदि (स्थितिः != EXCEPTION_NONE) {
        free_सूचकांक = cap_untyped_cap_get_cap_Free_सूचकांक(cap);
        reset = false;
    } अन्यथा {
        free_सूचकांक = 0;
        reset = true;
    }
    freeRef = GET_FREE_REF(cap_untyped_cap_get_capPtr(cap), free_सूचकांक);

    /*
     * Determine the maximum number of objects we can create, and return an
     * error if we don't have enough space.
     *
     * We don't need to worry about alignment in this case, because if anything
     * fits, it will also fit aligned up (by packing it on the right hand side
     * of the untyped).
     */
    untyped_Free_अष्टकानि = BIT(cap_untyped_cap_get_cap_खण्डम्_आकार(cap)) -
                       FREE_सूचकांक_गन्तव्यम्_विस्थापनम्(free_सूचकांक);

    यदि ((untyped_Free_अष्टकानि >> object_आकार) < ग्रन्थिः_गवाक्षः) {
        userError("Untyped Retype: Insufficient memory "
                  "(%lu * %lu bytes needed, %lu bytes available).",
                  (word_t)ग्रन्थिः_गवाक्षः,
                  (object_आकार >= word_द्वयङ्काः ? -1 : (1ul << object_आकार)),
                  (word_t)(untyped_Free_अष्टकानि));
        current_syscall_error.type = se_L4_Not_Enough_स्मृतिः;
        current_syscall_error.स्मृतिः_वामः = untyped_Free_अष्टकानि;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    device_स्मृतिः = cap_untyped_cap_get_capIsDevice(cap);
    यदि ((device_स्मृतिः && !Arch_isFrameType(newType))
        && newType != seL4_UntypedObject) {
        userError("Untyped Retype: Creating kernel objects with device untyped");
        current_syscall_error.type = se_L4_Invalid_तर्कः;
        current_syscall_error.invalid_तर्कः_सङ्ख्या = 1;
        प्रत्यागम EXCEPTION_SYSCALL_ERROR;
    }

    /* Align up the free region so that it is aligned to the target object's
     * size. */
    alignedFreeRef = align_ऊर्ध्वम्(freeRef, object_आकार);

    /* Perform the retype. */
    समुच्चयः_Thread_State(ग्रन्थिः_STATE(ksCurThread), ThreadState_Restart);
    प्रत्यागम invokeUntyped_Retype(slot, reset,
                                (शून्य *)alignedFreeRef, newType, user_Obj_आकार,
                                destCNode, ग्रन्थिः_विस्थापनम्, ग्रन्थिः_गवाक्षः, device_स्मृतिः);
}

स्थिर exception_t resetUntypedCap(cte_t *srcSlot)
{
    cap_t पूर्व_cap = srcSlot->cap;
    word_t खण्डम्_आकार = cap_untyped_cap_get_cap_खण्डम्_आकार(पूर्व_cap);
    शून्य *region_आधारः = WORD_PTR(cap_untyped_cap_get_capPtr(पूर्व_cap));
    पूर्णाङ्क chunk = CONFIG_RESET_CHUNK_द्वयङ्काः;
    word_t विस्थापनम् = FREE_सूचकांक_गन्तव्यम्_विस्थापनम्(cap_untyped_cap_get_cap_Free_सूचकांक(पूर्व_cap));
    exception_t स्थितिः;
    bool_t device_स्मृतिः = cap_untyped_cap_get_capIsDevice(पूर्व_cap);

    यदि (विस्थापनम् == 0) {
        प्रत्यागम EXCEPTION_NONE;
    }

    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>regionBase)
        (unat \<acute>block_size))" */

    यदि (device_स्मृतिः || खण्डम्_आकार < chunk) {
        यदि (! device_स्मृतिः) {
            clear_स्मृतिः(region_आधारः, खण्डम्_आकार);
        }
        srcSlot->cap = cap_untyped_cap_समुच्चयः_cap_Free_सूचकांक(पूर्व_cap, 0);
    } अन्यथा {
        प्रति (विस्थापनम् = आवर्तनम्_अधस्तात्(विस्थापनम् - 1, chunk);
             विस्थापनम् != - BIT(chunk); विस्थापनम् -= BIT(chunk)) {
            clear_स्मृतिः(GET_विस्थापनम्_FREE_PTR(region_आधारः, विस्थापनम्), chunk);
            srcSlot->cap = cap_untyped_cap_समुच्चयः_cap_Free_सूचकांक(पूर्व_cap, विस्थापनम्_गन्तव्यम्_FREE_सूचकांक(विस्थापनम्));
            स्थितिः = preemptionPoint();
            यदि (स्थितिः != EXCEPTION_NONE) {
                प्रत्यागम स्थितिः;
            }
        }
    }
    प्रत्यागम EXCEPTION_NONE;
}

exception_t invokeUntyped_Retype(cte_t *srcSlot,
                                 bool_t reset, शून्य *retype_आधारः,
                                 object_t newType, word_t user_आकार,
                                 cte_t *destCNode, word_t dest_विस्थापनम्, word_t dest_दीर्घता,
                                 bool_t device_स्मृतिः)
{
    word_t freeRef;
    word_t योग_Object_आकार;
    शून्य *region_आधारः = WORD_PTR(cap_untyped_cap_get_capPtr(srcSlot->cap));
    exception_t स्थितिः;

    यदि (reset) {
        स्थितिः = resetUntypedCap(srcSlot);
        यदि (स्थितिः != EXCEPTION_NONE) {
            प्रत्यागम स्थितिः;
        }
    }

    /* Update the amount of free space left in this untyped cap.
     *
     * Note that userSize is not necessarily the true size of the object in
     * memory. In the case where newType is seL4_CapTableObject, the size is
     * transformed by getObjectSize. */
    योग_Object_आकार = dest_दीर्घता << get_Object_आकार(newType, user_आकार);
    freeRef = (word_t)retype_आधारः + योग_Object_आकार;
    srcSlot->cap = cap_untyped_cap_समुच्चयः_cap_Free_सूचकांक(srcSlot->cap,
                                                    GET_FREE_सूचकांक(region_आधारः, freeRef));

    /* Create new objects and caps. */
    createNewObjects(newType, srcSlot, destCNode, dest_विस्थापनम्, dest_दीर्घता,
                     retype_आधारः, user_आकार, device_स्मृतिः);

    प्रत्यागम EXCEPTION_NONE;
}
