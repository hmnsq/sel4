/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    انتخاب (type) {
    حالت seL4_ARM_SmallPageObject:
        بازگشت true;
    حالت seL4_ARM_LargePageObject:
        بازگشت true;
    حالت seL4_ARM_HugePageObject:
        بازگشت true;
    پیشفرض:
        بازگشت false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_vspace_cap:
        اگر (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_page_table_cap:
        اگر (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif

#ifdef CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
    حالت cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
    حالت cap_sid_cap:
    حالت cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif
    پیشفرض:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
#ifdef CONFIG_ALLOW_SMC_CALLS
    اگر (cap_get_capType(cap) == cap_smc_cap) {
        اگر (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            بازگشت cap_smc_cap_set_capSMCBadge(cap, data);
        } وگرنه {
            بازگشت cap_null_cap_new();
        }
    } وگرنه {
#endif
        بازگشت cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        بازگشت cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } وگرنه {
        بازگشت cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        بشکن;

    حالت cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        اگر (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        اگر (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        بشکن;

    حالت cap_frame_cap:
        اگر (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        بشکن;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        بشکن;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        // do nothing
        بشکن;
#endif


#ifdef CONFIG_ARM_SMMU
    حالت cap_cb_cap:
        اگر (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        اگر (final) {
            smmu_delete_cb(cap);
        }
        بشکن;
    حالت cap_sid_cap:
        اگر (final) {
            smmu_delete_sid(cap);
        }
        بشکن;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    بازگشت fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_frame_cap:
        اگر (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, بالاترین_A, بالاترین_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            بالاترین_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            بالاترین_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            بازگشت ((botA <= botB) && (بالاترین_A >= بالاترین_B) && (botB <= بالاترین_B));
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (cap_get_capType(cap_b) == cap_page_table_cap) {
            بازگشت cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        بشکن;

    حالت cap_vspace_cap:
        اگر (cap_get_capType(cap_b) == cap_vspace_cap) {
            بازگشت cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        بشکن;

    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            بازگشت true;
        }
        بشکن;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            بازگشت cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        بشکن;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            بازگشت cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        بشکن;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        اگر (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            بازگشت (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        بشکن;
#endif

#ifdef CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            بازگشت true;
        }
        بشکن;
    حالت cap_cb_control_cap:
        اگر (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            بازگشت true;
        }
        بشکن;
    حالت cap_sid_cap:
        اگر (cap_get_capType(cap_b) == cap_sid_cap) {
            بازگشت cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        بشکن;
    حالت cap_cb_cap:
        اگر (cap_get_capType(cap_b) == cap_cb_cap) {
            بازگشت cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        بشکن;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        اگر (cap_get_capType(cap_b) == cap_smc_cap) {
            بازگشت true;
        }
        بشکن;
#endif
    }
    بازگشت false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_frame_cap) {
        اگر (cap_get_capType(cap_b) == cap_frame_cap) {
            بازگشت ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    اگر (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        بازگشت false;
    }
    اگر (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        بازگشت false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    اگر (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        بازگشت false;
    }
#endif

    بازگشت Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    انتخاب (t) {
    حالت seL4_ARM_SmallPageObject:
        بازگشت ARMSmallPageBits;
    حالت seL4_ARM_LargePageObject:
        بازگشت ARMLargePageBits;
    حالت seL4_ARM_HugePageObject:
        بازگشت ARMHugePageBits;
    حالت seL4_ARM_PageTableObject:
        بازگشت seL4_PageTableBits;
    حالت seL4_ARM_VSpaceObject:
        بازگشت seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        بازگشت VCPU_SIZE_BITS;
#endif
    پیشفرض:
        fail("Invalid object type");
        بازگشت 0;
    }
}

cap_t Arch_createObject(object_t t, تهی *regionBase, word_t userSize, bool_t deviceMemory)
{
    انتخاب (t) {
    حالت seL4_ARM_SmallPageObject:
        اگر (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } وگرنه {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        بازگشت cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    حالت seL4_ARM_LargePageObject:
        اگر (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } وگرنه {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        بازگشت cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    حالت seL4_ARM_HugePageObject:
        اگر (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } وگرنه {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        بازگشت cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    حالت seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        بازگشت cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#وگرنه
        بازگشت cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    حالت seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        بازگشت cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        بازگشت cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    پیشفرض:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t label, word_t length, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *buffer)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#اگر defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    انتخاب (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        بازگشت decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        بازگشت decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
        بازگشت decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    حالت cap_sid_cap:
        بازگشت decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    حالت cap_cb_control_cap:
        بازگشت decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    حالت cap_cb_cap:
        بازگشت decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        بازگشت decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    پیشفرض:
#وگرنه
{
#endif
    بازگشت decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}
}

تهی
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    اگر (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
