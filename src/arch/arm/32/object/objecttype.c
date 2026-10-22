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
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#अन्तर्भाव <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_ARM_SmallPageObject:
        पटिनिवत्त true;
    पक्ष seL4_ARM_LargePageObject:
        पटिनिवत्त true;
    पक्ष seL4_ARM_SectionObject:
        पटिनिवत्त true;
    पक्ष seL4_ARM_SuperSectionObject:
        पटिनिवत्त true;
    स्वभाव:
        पटिनिवत्त false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_table_cap:
        सचे (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अञ्ञथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        पटिनिवत्त ret;

    पक्ष cap_page_directory_cap:
        सचे (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अञ्ञथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        पटिनिवत्त ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    पक्ष cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

    पक्ष cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif

#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

    पक्ष cap_io_page_table_cap:
        सचे (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अञ्ञथा {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        पटिनिवत्त ret;
#endif
    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    पटिनिवत्त cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    सचे (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        पटिनिवत्त cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } अञ्ञथा सचे (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        पटिनिवत्त cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } अञ्ञथा {
        पटिनिवत्त cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_asid_pool_cap:
        सचे (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        भङ्ग;

    पक्ष cap_page_directory_cap:
        सचे (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        सचे (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_small_frame_cap:
        सचे (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            सचे (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                भङ्ग;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (शून्य *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        सचे (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            सचे (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                सचे (pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((शून्य *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((शून्य *)&armKSGlobalLogPT[0]));

                    अत्थाय (पूर्णाङ्क idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (शून्य *)cap_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        सचे (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        भङ्ग;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        // do nothing
        भङ्ग;
#endif


#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        सचे (final) {
            clearIOPageDirectory(cap);
        }
        भङ्ग;

    पक्ष cap_io_page_table_cap:
        सचे (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        भङ्ग;
#endif

    स्वभाव:
        भङ्ग;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    पटिनिवत्त fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_small_frame_cap:
    पक्ष cap_frame_cap:
        सचे (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            पटिनिवत्त ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        सचे (cap_get_capType(cap_b) == cap_page_table_cap) {
            पटिनिवत्त cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_page_directory_cap:
        सचे (cap_get_capType(cap_b) == cap_page_directory_cap) {
            पटिनिवत्त cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_asid_control_cap:
        सचे (cap_get_capType(cap_b) == cap_asid_control_cap) {
            पटिनिवत्त true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        सचे (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            पटिनिवत्त cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        सचे (cap_get_capType(cap_b) == cap_vcpu_cap) {
            पटिनिवत्त cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        भङ्ग;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        सचे (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            पटिनिवत्त (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        भङ्ग;
#endif


#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        सचे (cap_get_capType(cap_b) == cap_io_space_cap) {
            पटिनिवत्त cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_page_table_cap:
        सचे (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            पटिनिवत्त cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        भङ्ग;
#endif
    }

    पटिनिवत्त false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    सचे (cap_get_capType(cap_a) == cap_small_frame_cap) {
        सचे (cap_get_capType(cap_b) == cap_small_frame_cap) {
            पटिनिवत्त ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अञ्ञथा सचे (cap_get_capType(cap_b) == cap_frame_cap) {
            पटिनिवत्त false;
        }
    }
    सचे (cap_get_capType(cap_a) == cap_frame_cap) {
        सचे (cap_get_capType(cap_b) == cap_frame_cap) {
            पटिनिवत्त ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अञ्ञथा सचे (cap_get_capType(cap_b) == cap_small_frame_cap) {
            पटिनिवत्त false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    सचे (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        पटिनिवत्त false;
    }
#endif

    पटिनिवत्त Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        पटिनिवत्त ARMSmallPageBits;
    पक्ष seL4_ARM_LargePageObject:
        पटिनिवत्त ARMLargePageBits;
    पक्ष seL4_ARM_SectionObject:
        पटिनिवत्त ARMSectionBits;
    पक्ष seL4_ARM_SuperSectionObject:
        पटिनिवत्त ARMSuperSectionBits;
    पक्ष seL4_ARM_PageTableObject:
        पटिनिवत्त PTE_SIZE_BITS + PT_INDEX_BITS;
    पक्ष seL4_ARM_PageDirectoryObject:
        पटिनिवत्त PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    पक्ष seL4_ARM_IOPageTableObject:
        पटिनिवत्त seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        पटिनिवत्त VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    स्वभाव:
        fail("Invalid object type");
        पटिनिवत्त 0;
    }
}

cap_t Arch_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        सचे (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } अञ्ञथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_small_frame_cap_new(
                   ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_उच्चः(asidInvalid),
                   (word_t)regionBase);

    पक्ष seL4_ARM_LargePageObject:
        सचे (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } अञ्ञथा {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   ARMLargePage, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_उच्चः(asidInvalid),
                   (word_t)regionBase);

    पक्ष seL4_ARM_SectionObject:
        सचे (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } अञ्ञथा {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSection)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   ARMSection, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_उच्चः(asidInvalid),
                   (word_t)regionBase);

    पक्ष seL4_ARM_SuperSectionObject:
        सचे (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } अञ्ञथा {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSuperSection)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   ARMSuperSection, ASID_नीचः(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_उच्चः(asidInvalid),
                   (word_t)regionBase);

    पक्ष seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        पटिनिवत्त cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    पक्ष seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#अञ्ञथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        पटिनिवत्त cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        पटिनिवत्त cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    पक्ष seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        पटिनिवत्त cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    स्वभाव:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t invLabel, word_t length, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *buffer)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#सचे defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    विकल्प (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    पक्ष cap_io_space_cap:
        पटिनिवत्त decodeARMIOSpaceInvocation(invLabel, cap);
    पक्ष cap_io_page_table_cap:
        पटिनिवत्त decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        पटिनिवत्त decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        पटिनिवत्त decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    स्वभाव:
#अञ्ञथा
{
#endif
    पटिनिवत्त decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

शून्य
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    सचे (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

