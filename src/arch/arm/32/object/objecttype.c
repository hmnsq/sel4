/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <kernel/vspace.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/objecttype.h>
#समावेश <arch/machine/tlb.h>
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
#समावेश <arch/object/vcpu.h>
#पूर्ण_अगर

bool_t Arch_isFrameType(word_t type)
{
    चयन (type) {
    हाल seL4_ARM_SmallPageObject:
        वापस true;
    हाल seL4_ARM_LargePageObject:
        वापस true;
    हाल seL4_ARM_SectionObject:
        वापस true;
    हाल seL4_ARM_SuperSectionObject:
        वापस true;
    शेष:
        वापस false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_page_table_cap:
        अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_page_directory_cap:
        अगर (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    हाल cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल cap_asid_control_cap:
    हाल cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर

#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर

#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल cap_io_page_table_cap:
        अगर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;
#पूर्ण_अगर
    शेष:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_तथ्य(bool_t preserve, word_t तथ्य, cap_t cap)
{
    वापस cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    अगर (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        वापस cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } अन्यथा अगर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        वापस cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } अन्यथा {
        वापस cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_asid_pool_cap:
        अगर (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        अवरोध;

    हाल cap_page_directory_cap:
        अगर (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        अवरोध;

    हाल cap_small_frame_cap:
        अगर (cap_small_frame_cap_get_capFMappedASID(cap)) {
#अगर_घोषित CONFIG_TK1_SMMU
            अगर (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                अवरोध;
            }
#पूर्ण_अगर

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (व्योम *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        अवरोध;

    हाल cap_frame_cap:
        अगर (cap_frame_cap_get_capFMappedASID(cap)) {
#अगर_घोषित CONFIG_KERNEL_LOG_बफर
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            अगर (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                अगर (pptr_to_paddr((व्योम *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_बफर) {
                    ks_User_Log_बफर = 0;

                    /* Invalidate log page table entries */
                    clearMemory((व्योम *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((व्योम *)&armKSGlobalLogPT[0]));

                    क्रम (पूर्णांक idx = 0; idx < BIT(PT_सूचकांक_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#पूर्ण_अगर /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (व्योम *)cap_frame_cap_get_capFBasePtr(cap));
        }
        अवरोध;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        अगर (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        अवरोध;
#पूर्ण_अगर

#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        // do nothing
        अवरोध;
#पूर्ण_अगर


#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap:
        अगर (final) {
            clearIOPageDirectory(cap);
        }
        अवरोध;

    हाल cap_io_page_table_cap:
        अगर (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        अवरोध;
#पूर्ण_अगर

    शेष:
        अवरोध;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    चयन (cap_get_capType(cap_a)) {
    हाल cap_small_frame_cap:
    हाल cap_frame_cap:
        अगर (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(page_Bits_For_आकार(generic_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(page_Bits_For_आकार(generic_frame_cap_get_capFSize(cap_b))) ;
            वापस ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_page_table_cap) {
            वापस cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_page_directory_cap:
        अगर (cap_get_capType(cap_b) == cap_page_directory_cap) {
            वापस cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_asid_control_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_asid_pool_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            वापस cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        अवरोध;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        अगर (cap_get_capType(cap_b) == cap_vcpu_cap) {
            वापस cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        अवरोध;
#पूर्ण_अगर

#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        अगर (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            वापस (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        अवरोध;
#पूर्ण_अगर


#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap:
        अगर (cap_get_capType(cap_b) == cap_io_space_cap) {
            वापस cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        अवरोध;

    हाल cap_io_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            वापस cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        अवरोध;
#पूर्ण_अगर
    }

    वापस false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    अगर (cap_get_capType(cap_a) == cap_small_frame_cap) {
        अगर (cap_get_capType(cap_b) == cap_small_frame_cap) {
            वापस ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            वापस false;
        }
    }
    अगर (cap_get_capType(cap_a) == cap_frame_cap) {
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            वापस ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा अगर (cap_get_capType(cap_b) == cap_small_frame_cap) {
            वापस false;
        }
    }
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    अगर (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        वापस false;
    }
#पूर्ण_अगर

    वापस Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    चयन (t) {
    हाल seL4_ARM_SmallPageObject:
        वापस ARMSmallPageBits;
    हाल seL4_ARM_LargePageObject:
        वापस ARMLargePageBits;
    हाल seL4_ARM_SectionObject:
        वापस ARMSectionBits;
    हाल seL4_ARM_SuperSectionObject:
        वापस ARMSuperSectionBits;
    हाल seL4_ARM_PageTableObject:
        वापस PTE_आकार_BITS + PT_सूचकांक_BITS;
    हाल seL4_ARM_PageDirectoryObject:
        वापस PDE_आकार_BITS + PD_सूचकांक_BITS;
#अगर_घोषित CONFIG_TK1_SMMU
    हाल seL4_ARM_IOPageTableObject:
        वापस seL4_IOPageTableBits;
#पूर्ण_अगर
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_ARM_VCPUObject:
        वापस VCPU_आकार_BITS;
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    शेष:
        fail("Invalid object type");
        वापस 0;
    }
}

cap_t Arch_createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
    चयन (t) {
    हाल seL4_ARM_SmallPageObject:
        अगर (deviceMemory) {
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
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_small_frame_cap_new(
                   ASID_निचला(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#अगर_घोषित CONFIG_TK1_SMMU
                   0,
#पूर्ण_अगर
                   ASID_ऊपरी(asidInvalid),
                   (word_t)regionBase);

    हाल seL4_ARM_LargePageObject:
        अगर (deviceMemory) {
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
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   ARMLargePage, ASID_निचला(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ऊपरी(asidInvalid),
                   (word_t)regionBase);

    हाल seL4_ARM_SectionObject:
        अगर (deviceMemory) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } अन्यथा {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMSection)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   ARMSection, ASID_निचला(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ऊपरी(asidInvalid),
                   (word_t)regionBase);

    हाल seL4_ARM_SuperSectionObject:
        अगर (deviceMemory) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } अन्यथा {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMSuperSection)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   ARMSuperSection, ASID_निचला(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ऊपरी(asidInvalid),
                   (word_t)regionBase);

    हाल seL4_ARM_PageTableObject:
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        वापस cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    हाल seL4_ARM_PageDirectoryObject:
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        वापस cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        वापस cap_vcpu_cap_new(VCPU_REF(regionBase));
#पूर्ण_अगर

#अगर_घोषित CONFIG_TK1_SMMU
    हाल seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        वापस cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#पूर्ण_अगर
    शेष:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t invLabel, word_t लंबाई, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *बफर)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#अगर defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    चयन (cap_get_capType(cap)) {
#अगर_घोषित CONFIG_TK1_SMMU
    हाल cap_io_space_cap:
        वापस decodeARMIOSpaceInvocation(invLabel, cap);
    हाल cap_io_page_table_cap:
        वापस decodeARMIOPTInvocation(invLabel, लंबाई, slot, cap, बफर);
#पूर्ण_अगर
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        वापस decodeARMVCPUInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
#पूर्ण_अगर /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        वापस decodeSGISignalInvocation(invLabel, लंबाई, cap, बफर);
#पूर्ण_अगर /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    शेष:
#अन्यथा
{
#पूर्ण_अगर
    वापस decodeARMMMUInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
}
}

व्योम
Arch_prepareThreadDelete(tcb_t * thread) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    अगर (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#अगर_घोषित CONFIG_HAVE_FPU
    fpuRelease(thread);
#पूर्ण_अगर
}

