/*
 * Copyright 2014, General Dynamics C4 Systems
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
#अन्तर्भाव <arch/machine/tlb.h>
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
#अन्तर्भाव <arch/object/vcpu.h>
#यदि_अन्तः

bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_ARM_SmallPageObject:
        प्रत्यागम true;
    पक्ष seL4_ARM_LargePageObject:
        प्रत्यागम true;
    पक्ष seL4_ARM_SectionObject:
        प्रत्यागम true;
    पक्ष seL4_ARM_SuperSectionObject:
        प्रत्यागम true;
    स्वभाव:
        प्रत्यागम false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_सारणी_cap:
        यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_page_directory_cap:
        यदि (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    पक्ष cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_समुच्चयः_cap_FMapped_ASID(cap, asidInvalid);
        ret.स्थितिः = EXCEPTION_NONE;
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

#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

    पक्ष cap_io_page_सारणी_cap:
        यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
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
    प्रत्यागम cap;
}

cap_t CONST Arch_आवरणम्_Cap_Rights(seL4_CapRights_t cap_rights_आवरणम्, cap_t cap)
{
    यदि (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = आवरणम्_VMRights(vm_rights, cap_rights_आवरणम्);
        प्रत्यागम cap_small_frame_cap_समुच्चयः_cap_FVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } अन्यथा यदि (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = आवरणम्_VMRights(vm_rights, cap_rights_आवरणम्);
        प्रत्यागम cap_frame_cap_समुच्चयः_cap_FVMRights(cap,
                                              wordFromVMRights(vm_rights));
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

    पक्ष cap_page_directory_cap:
        यदि (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (final && cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_Page_सारणी(
                cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
                cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
                PTE_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap)));
        }
        भङ्ग;

    पक्ष cap_small_frame_cap:
        यदि (cap_small_frame_cap_get_capFMappedASID(cap)) {
#यदि_परिभाषितम् CONFIG_TK1_SMMU
            यदि (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                भङ्ग;
            }
#यदि_अन्तः

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                      (शून्य *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        यदि (cap_frame_cap_get_capFMappedASID(cap)) {
#यदि_परिभाषितम् CONFIG_KERNEL_LOG_बफर
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            यदि (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                यदि (pptr_गन्तव्यम्_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_बफर) {
                    ks_User_Log_बफर = 0;

                    /* Invalidate log page table entries */
                    clear_स्मृतिः((शून्य *) armKSGlobalLogPT, se_L4_Page_सारणी_द्वयङ्काः);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(se_L4_Page_सारणी_द्वयङ्काः),
                                        addrFromKPPtr((शून्य *)&armKSGlobalLogPT[0]));

                    प्रति (पूर्णाङ्क idx = 0; idx < BIT(PT_सूचकांक_द्वयङ्काः); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_Page_द्वयङ्काः));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#यदि_अन्तः /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_cap_FMapped_स्थानाङ्कः(cap),
                      (शून्य *)cap_frame_cap_get_capFBasePtr(cap));
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


#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        यदि (final) {
            clearIOPageDirectory(cap);
        }
        भङ्ग;

    पक्ष cap_io_page_सारणी_cap:
        यदि (final && cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_सारणी(cap);
        }
        भङ्ग;
#यदि_अन्तः

    स्वभाव:
        भङ्ग;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_small_frame_cap:
    पक्ष cap_frame_cap:
        यदि (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + आवरणम्(page_द्वयङ्काः_For_आकार(generic_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + आवरणम्(page_द्वयङ्काः_For_आकार(generic_frame_cap_get_capFSize(cap_b))) ;
            प्रत्यागम ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_page_सारणी_cap) {
            प्रत्यागम cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_b);
        }
        भङ्ग;

    पक्ष cap_page_directory_cap:
        यदि (cap_get_capType(cap_b) == cap_page_directory_cap) {
            प्रत्यागम cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
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


#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        यदि (cap_get_capType(cap_b) == cap_io_space_cap) {
            प्रत्यागम cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_io_page_सारणी_cap) {
            प्रत्यागम cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        भङ्ग;
#यदि_अन्तः
    }

    प्रत्यागम false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    यदि (cap_get_capType(cap_a) == cap_small_frame_cap) {
        यदि (cap_get_capType(cap_b) == cap_small_frame_cap) {
            प्रत्यागम ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            प्रत्यागम false;
        }
    }
    यदि (cap_get_capType(cap_a) == cap_frame_cap) {
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            प्रत्यागम ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा यदि (cap_get_capType(cap_b) == cap_small_frame_cap) {
            प्रत्यागम false;
        }
    }
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
    पक्ष seL4_ARM_SectionObject:
        प्रत्यागम ARMSection_द्वयङ्काः;
    पक्ष seL4_ARM_SuperSectionObject:
        प्रत्यागम ARMSuper_Section_द्वयङ्काः;
    पक्ष se_L4_ARM_Page_सारणी_Object:
        प्रत्यागम PTE_आकार_द्वयङ्काः + PT_सूचकांक_द्वयङ्काः;
    पक्ष seL4_ARM_PageDirectoryObject:
        प्रत्यागम PDE_आकार_द्वयङ्काः + PD_सूचकांक_द्वयङ्काः;
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष se_L4_ARM_IOPage_सारणी_Object:
        प्रत्यागम se_L4_IOPage_सारणी_द्वयङ्काः;
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        प्रत्यागम VCPU_आकार_द्वयङ्काः;
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        प्रत्यागम cap_small_frame_cap_new(
                   ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!device_स्मृतिः,
#यदि_परिभाषितम् CONFIG_TK1_SMMU
                   0,
#यदि_अन्तः
                   ASID_उच्चः(asidInvalid),
                   (word_t)region_आधारः);

    पक्ष seL4_ARM_LargePageObject:
        यदि (device_स्मृतिः) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMLargePage)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   ARMLargePage, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!device_स्मृतिः, ASID_उच्चः(asidInvalid),
                   (word_t)region_आधारः);

    पक्ष seL4_ARM_SectionObject:
        यदि (device_स्मृतिः) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } अन्यथा {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMSection)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   ARMSection, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!device_स्मृतिः, ASID_उच्चः(asidInvalid),
                   (word_t)region_आधारः);

    पक्ष seL4_ARM_SuperSectionObject:
        यदि (device_स्मृतिः) {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } अन्यथा {
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((word_t)region_आधारः,
                                (word_t)region_आधारः + आवरणम्(page_द्वयङ्काः_For_आकार(ARMSuperSection)),
                                addrFromPPtr(region_आधारः));
        }
        प्रत्यागम cap_frame_cap_new(
                   ARMSuperSection, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!device_स्मृतिः, ASID_उच्चः(asidInvalid),
                   (word_t)region_आधारः);

    पक्ष se_L4_ARM_Page_सारणी_Object:
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)region_आधारः,
                            (word_t)region_आधारः + आवरणम्(se_L4_Page_सारणी_द्वयङ्काः),
                            addrFromPPtr(region_आधारः));
        प्रत्यागम cap_page_सारणी_cap_new(false, asidInvalid, 0,
                                      (word_t)region_आधारः);

    पक्ष seL4_ARM_PageDirectoryObject:
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#यदि_अन्तः /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_आधारः);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)region_आधारः,
                            (word_t)region_आधारः + आवरणम्(se_L4_Page_Dir_द्वयङ्काः),
                            addrFromPPtr(region_आधारः));

        प्रत्यागम cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)region_आधारः);
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_आधारः));
        प्रत्यागम cap_vcpu_cap_new(VCPU_REF(region_आधारः));
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष se_L4_ARM_IOPage_सारणी_Object:
        cleanCacheRange_RAM((word_t)region_आधारः,
                            (word_t)region_आधारः + आवरणम्(se_L4_IOPage_सारणी_द्वयङ्काः),
                            addrFromPPtr(region_आधारः));
        प्रत्यागम cap_io_page_सारणी_cap_new(0, asidInvalid, (word_t)region_आधारः, 0);
#यदि_अन्तः
    स्वभाव:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t invLabel, word_t दीर्घता, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *बफर)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#यदि defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    विकल्प (cap_get_capType(cap)) {
#यदि_परिभाषितम् CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        प्रत्यागम decodeARMIOSpaceInvocation(invLabel, cap);
    पक्ष cap_io_page_सारणी_cap:
        प्रत्यागम decodeARMIOPTInvocation(invLabel, दीर्घता, slot, cap, बफर);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        प्रत्यागम decodeARMVCPUInvocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
#यदि_अन्तः /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#यदि_अपरिभाषितम् CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        प्रत्यागम decodeSGISignalInvocation(invLabel, दीर्घता, cap, बफर);
#यदि_अन्तः /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    स्वभाव:
#अन्यथा
{
#यदि_अन्तः
    प्रत्यागम decodeARMMMUInvocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
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

