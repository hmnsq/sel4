/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    নির্বাচন (type) {
    ক্ষেত্র seL4_ARM_SmallPageObject:
        ফেরত true;
    ক্ষেত্র seL4_ARM_LargePageObject:
        ফেরত true;
    ক্ষেত্র seL4_ARM_HugePageObject:
        ফেরত true;
    পূর্বনির্ধারিত:
        ফেরত false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_vspace_cap:
        যদি (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    ক্ষেত্র cap_asid_control_cap:
    ক্ষেত্র cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ক্ষেত্র cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
#endif

#ifdef CONFIG_ARM_SMMU
    ক্ষেত্র cap_sid_control_cap:
    ক্ষেত্র cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    ক্ষেত্র cap_sid_cap:
    ক্ষেত্র cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ক্ষেত্র cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
#endif
    পূর্বনির্ধারিত:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_তথ্য(bool_t preserve, word_t তথ্য, cap_t cap)
{
#ifdef CONFIG_ALLOW_SMC_CALLS
    যদি (cap_get_capType(cap) == cap_smc_cap) {
        যদি (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ফেরত cap_smc_cap_set_capSMCBadge(cap, তথ্য);
        } অন্যথা {
            ফেরত cap_null_cap_new();
        }
    } অন্যথা {
#endif
        ফেরত cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        ফেরত cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } অন্যথা {
        ফেরত cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_asid_pool_cap:
        যদি (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        বিরতি;

    ক্ষেত্র cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        যদি (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        যদি (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        বিরতি;

    ক্ষেত্র cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        বিরতি;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap:
        যদি (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        বিরতি;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ক্ষেত্র cap_sgi_signal_cap:
        // do nothing
        বিরতি;
#endif


#ifdef CONFIG_ARM_SMMU
    ক্ষেত্র cap_cb_cap:
        যদি (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        যদি (final) {
            smmu_delete_cb(cap);
        }
        বিরতি;
    ক্ষেত্র cap_sid_cap:
        যদি (final) {
            smmu_delete_sid(cap);
        }
        বিরতি;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    নির্বাচন (cap_get_capType(cap_a)) {
    ক্ষেত্র cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, শীর্ষ_A, শীর্ষ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            শীর্ষ_A = botA + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_a)));
            শীর্ষ_B = botB + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_b))) ;
            ফেরত ((botA <= botB) && (শীর্ষ_A >= শীর্ষ_B) && (botB <= শীর্ষ_B));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            ফেরত cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_vspace_cap:
        যদি (cap_get_capType(cap_b) == cap_vspace_cap) {
            ফেরত cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_asid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ফেরত cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        বিরতি;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap:
        যদি (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ফেরত cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        বিরতি;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ক্ষেত্র cap_sgi_signal_cap:
        যদি (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ফেরত (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        বিরতি;
#endif

#ifdef CONFIG_ARM_SMMU
    ক্ষেত্র cap_sid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ফেরত true;
        }
        বিরতি;
    ক্ষেত্র cap_cb_control_cap:
        যদি (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ফেরত true;
        }
        বিরতি;
    ক্ষেত্র cap_sid_cap:
        যদি (cap_get_capType(cap_b) == cap_sid_cap) {
            ফেরত cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        বিরতি;
    ক্ষেত্র cap_cb_cap:
        যদি (cap_get_capType(cap_b) == cap_cb_cap) {
            ফেরত cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        বিরতি;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ক্ষেত্র cap_smc_cap:
        যদি (cap_get_capType(cap_b) == cap_smc_cap) {
            ফেরত true;
        }
        বিরতি;
#endif
    }
    ফেরত false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ফেরত ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    যদি (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ফেরত false;
    }
    যদি (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ফেরত false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    যদি (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ফেরত false;
    }
#endif

    ফেরত Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_আকার(word_t t)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_ARM_SmallPageObject:
        ফেরত ARMSmallPageBits;
    ক্ষেত্র seL4_ARM_LargePageObject:
        ফেরত ARMLargePageBits;
    ক্ষেত্র seL4_ARM_HugePageObject:
        ফেরত ARMHugePageBits;
    ক্ষেত্র seL4_ARM_PageTableObject:
        ফেরত seL4_PageTableBits;
    ক্ষেত্র seL4_ARM_VSpaceObject:
        ফেরত seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_ARM_VCPUObject:
        ফেরত VCPU_আকার_BITS;
#endif
    পূর্বনির্ধারিত:
        fail("Invalid object type");
        ফেরত 0;
    }
}

cap_t Arch_createObject(object_t t, শূন্য *regionBase, word_t user_আকার, bool_t deviceMemory)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_ARM_SmallPageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ক্ষেত্র seL4_ARM_LargePageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ক্ষেত্র seL4_ARM_HugePageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    ক্ষেত্র seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        ফেরত cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#অন্যথা
        ফেরত cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    ক্ষেত্র seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ফেরত cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ফেরত cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    পূর্বনির্ধারিত:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t label, word_t দৈর্ঘ্য, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *বাফার)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#যদি defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    নির্বাচন (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap:
        ফেরত decodeARMVCPUInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ক্ষেত্র cap_sgi_signal_cap:
        ফেরত decodeSGISignalInvocation(label, দৈর্ঘ্য, cap, বাফার);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    ক্ষেত্র cap_sid_control_cap:
        ফেরত decodeARMSIDControlInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    ক্ষেত্র cap_sid_cap:
        ফেরত decodeARMSIDInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    ক্ষেত্র cap_cb_control_cap:
        ফেরত decodeARMCBControlInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    ক্ষেত্র cap_cb_cap:
        ফেরত decodeARMCBInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    ক্ষেত্র cap_smc_cap:
        ফেরত decodeARMSMCInvocation(label, দৈর্ঘ্য, cap, call, বাফার);
#endif
    পূর্বনির্ধারিত:
#অন্যথা
{
#endif
    ফেরত decodeARMMMUInvocation(label, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
}
}

শূন্য
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    যদি (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
