/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_ARM_SmallPageObject:
        प्रत्यागम true;
    पक्ष seL4_ARM_LargePageObject:
        प्रत्यागम true;
    पक्ष seL4_ARM_HugePageObject:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_vspace_cap:
        यदि (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_page_सारणी_cap:
        यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_frame_cap:
        ret.cap = cap_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asidInvalid);
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
#यदि_अन्तः
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
    पक्ष cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_sid_cap:
    पक्ष cap_cb_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
#यदि_अन्तः
    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_दत्त(bool_t preserve, word_t दत्त, cap_t cap)
{
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    यदि (cap_get_capType(cap) == cap_smc_cap) {
        यदि (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            प्रत्यागम cap_smc_cap_समुच्चयः_cap_SMCBadge(cap, दत्त);
        } अन्यथा {
            प्रत्यागम cap_null_cap_new();
        }
    } अन्यथा {
#यदि_अन्तः
        प्रत्यागम cap;
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    }
#यदि_अन्तः
}

cap_t CONST Arch_आवरणम्_Cap_Rights(seL4_CapRights_t cap_rights_आवरणम्, cap_t cap)
{
    यदि (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = आवरणम्_VMRights(vm_rights, cap_rights_आवरणम्);

        प्रत्यागम cap_frame_cap_समुच्चयः_cap_FVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        प्रत्यागम cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_asid_pool_cap:
        यदि (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        भङ्ग;

    पक्ष cap_vspace_cap:
#यदि_परिभाषितम् CONFIG_ARM_SMMU
        यदि (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#यदि_अन्तः
        यदि (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (final && cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_Page_सारणी(cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
                           cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
                           PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap)));
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        यदि (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        भङ्ग;
#यदि_अन्तः
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        // do nothing
        भङ्ग;
#यदि_अन्तः


#यदि_परिभाषितम् CONFIG_ARM_SMMU
    पक्ष cap_cb_cap:
        यदि (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        यदि (final) {
            smmu_delete_cb(cap);
        }
        भङ्ग;
    पक्ष cap_sid_cap:
        यदि (final) {
            smmu_delete_sid(cap);
        }
        भङ्ग;
#यदि_अन्तः
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_frame_cap:
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_b))) ;
            प्रत्यागम ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_page_सारणी_cap) {
            प्रत्यागम cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_b);
        }
        भङ्ग;

    पक्ष cap_vspace_cap:
        यदि (cap_get_capType(cap_b) == cap_vspace_cap) {
            प्रत्यागम cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_asid_control_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            प्रत्यागम cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        यदि (cap_get_capType(cap_b) == cap_vcpu_cap) {
            प्रत्यागम cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        भङ्ग;
#यदि_अन्तः
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        यदि (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            प्रत्यागम (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        भङ्ग;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
        यदि (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            प्रत्यागम true;
        }
        भङ्ग;
    पक्ष cap_cb_control_cap:
        यदि (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            प्रत्यागम true;
        }
        भङ्ग;
    पक्ष cap_sid_cap:
        यदि (cap_get_capType(cap_b) == cap_sid_cap) {
            प्रत्यागम cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        भङ्ग;
    पक्ष cap_cb_cap:
        यदि (cap_get_capType(cap_b) == cap_cb_cap) {
            प्रत्यागम cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        भङ्ग;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        यदि (cap_get_capType(cap_b) == cap_smc_cap) {
            प्रत्यागम true;
        }
        भङ्ग;
#यदि_अन्तः
    }
    प्रत्यागम false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    यदि (cap_get_capType(cap_a) == cap_frame_cap) {
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            प्रत्यागम ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#यदि_परिभाषितम् CONFIG_ARM_SMMU
    यदि (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        प्रत्यागम false;
    }
    यदि (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        प्रत्यागम false;
    }
#यदि_अन्तः
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    यदि (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        प्रत्यागम false;
    }
#यदि_अन्तः

    प्रत्यागम Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        प्रत्यागम ARMSmall_Page_द्वयङ्काः;
    पक्ष seL4_ARM_LargePageObject:
        प्रत्यागम ARMLarge_Page_द्वयङ्काः;
    पक्ष seL4_ARM_HugePageObject:
        प्रत्यागम ARMHuge_Page_द्वयङ्काः;
    पक्ष se_L4_ARM_Page_सारणी_Object:
        प्रत्यागम se_L4_Page_सारणी_द्वयङ्काः;
    पक्ष seL4_ARM_VSpaceObject:
        प्रत्यागम se_L4_VSpace_द्वयङ्काः;
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        प्रत्यागम VCPU_आकार_द्वयङ्काः;
#यदि_अन्तः
    स्वभाव:
        fail("Invalid object type");
        प्रत्यागम 0;
    }
}

cap_t Arch_createObject(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMSmallPage)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)region_आधारः,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_स्मृतिः         /* capFIsDevice */
               );

    पक्ष seL4_ARM_LargePageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMLargePage)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)region_आधारः,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_स्मृतिः         /* capFIsDevice */
               );

    पक्ष seL4_ARM_HugePageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMHugePage)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)region_आधारः,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_स्मृतिः         /* capFIsDevice */
               );
    पक्ष seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)region_आधारः,
                            (word_t)region_आधारः + आवरणम्(se_L4_VSpace_द्वयङ्काः),
                            addrFromPPtr(region_आधारः));
#यदि_परिभाषितम् CONFIG_ARM_SMMU
        प्रत्यागम cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)region_आधारः,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#अन्यथा
        प्रत्यागम cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)region_आधारः,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#यदि_अन्तः /*!CONFIG_ARM_SMMU*/
    पक्ष se_L4_ARM_Page_सारणी_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)region_आधारः,
                            (word_t)region_आधारः + आवरणम्(se_L4_Page_सारणी_द्वयङ्काः),
                            addrFromPPtr(region_आधारः));
        प्रत्यागम cap_page_सारणी_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)region_आधारः,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_आधारः));
        प्रत्यागम cap_vcpu_cap_new(VCPU_REF(region_आधारः));
#यदि_अन्तः

    स्वभाव:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t label, word_t दीर्घता, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *बफर)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#यदि defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    विकल्प (cap_get_capType(cap)) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        प्रत्यागम decodeARMVCPUInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
#यदि_अन्तः /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        प्रत्यागम decodeSGISignalInvocation(label, दीर्घता, cap, बफर);
#यदि_अन्तः /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#यदि_परिभाषितम् CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
        प्रत्यागम decodeARMSIDControlInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
    पक्ष cap_sid_cap:
        प्रत्यागम decodeARMSIDInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
    पक्ष cap_cb_control_cap:
        प्रत्यागम decodeARMCBControlInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
    पक्ष cap_cb_cap:
        प्रत्यागम decodeARMCBInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
#यदि_अन्तः /*CONFIG_ARM_SMMU*/
#यदि_परिभाषितम् CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        प्रत्यागम decodeARMSMCInvocation(label, दीर्घता, cap, call, बफर);
#यदि_अन्तः
    स्वभाव:
#अन्यथा
{
#यदि_अन्तः
    प्रत्यागम decodeARMMMUInvocation(label, दीर्घता, cptr, slot, cap, call, बफर);
}
}

शून्य
Arch_prepareThreadDelete(tcb_t * thread) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    यदि (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#यदि_परिभाषितम् CONFIG_HAVE_FPU
    fpuRelease(thread);
#यदि_अन्तः
}
