/*
 * Copyright 2014, General Dynamics C4 Systems
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
#অন্তর্ভাব <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#অন্তর্ভাব <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    নির্বাচন (type) {
    ক্ষেত্র seL4_ARM_SmallPageObject:
        ফেরত true;
    ক্ষেত্র seL4_ARM_LargePageObject:
        ফেরত true;
    ক্ষেত্র seL4_ARM_SectionObject:
        ফেরত true;
    ক্ষেত্র seL4_ARM_SuperSectionObject:
        ফেরত true;
    পূর্বনির্ধারিত:
        ফেরত false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_page_directory_cap:
        যদি (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    ক্ষেত্র cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
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

#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

    ক্ষেত্র cap_io_page_table_cap:
        যদি (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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
    ফেরত cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ফেরত cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } অন্যথা যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ফেরত cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
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

    ক্ষেত্র cap_page_directory_cap:
        যদি (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        বিরতি;

    ক্ষেত্র cap_small_frame_cap:
        যদি (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            যদি (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                বিরতি;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (শূন্য *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        বিরতি;

    ক্ষেত্র cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_বাফার
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            যদি (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                যদি (pptr_to_paddr((শূন্য *)cap_frame_cap_get_capFBasePtr(cap)) == ks_User_Log_বাফার) {
                    ks_User_Log_বাফার = 0;

                    /* Invalidate log page table entries */
                    clearMemory((শূন্য *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((শূন্য *)&armKSGlobalLogPT[0]));

                    জন্য (পূর্ণসংখ্যা idx = 0; idx < BIT(PT_সূচক_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (শূন্য *)cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap:
        যদি (final) {
            clearIOPageDirectory(cap);
        }
        বিরতি;

    ক্ষেত্র cap_io_page_table_cap:
        যদি (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        বিরতি;
#endif

    পূর্বনির্ধারিত:
        বিরতি;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    নির্বাচন (cap_get_capType(cap_a)) {
    ক্ষেত্র cap_small_frame_cap:
    ক্ষেত্র cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, শীর্ষ_A, শীর্ষ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            শীর্ষ_A = botA + MASK(page_Bits_For_আকার(generic_frame_cap_get_capFSize(cap_a)));
            শীর্ষ_B = botB + MASK(page_Bits_For_আকার(generic_frame_cap_get_capFSize(cap_b))) ;
            ফেরত ((botA <= botB) && (শীর্ষ_A >= শীর্ষ_B) && (botB <= শীর্ষ_B));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            ফেরত cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_page_directory_cap:
        যদি (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ফেরত cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
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


#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap:
        যদি (cap_get_capType(cap_b) == cap_io_space_cap) {
            ফেরত cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_io_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ফেরত cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        বিরতি;
#endif
    }

    ফেরত false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_small_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ফেরত ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } অন্যথা যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ফেরত false;
        }
    }
    যদি (cap_get_capType(cap_a) == cap_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ফেরত ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } অন্যথা যদি (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ফেরত false;
        }
    }
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
    ক্ষেত্র seL4_ARM_SectionObject:
        ফেরত ARMSectionBits;
    ক্ষেত্র seL4_ARM_SuperSectionObject:
        ফেরত ARMSuperSectionBits;
    ক্ষেত্র seL4_ARM_PageTableObject:
        ফেরত PTE_আকার_BITS + PT_সূচক_BITS;
    ক্ষেত্র seL4_ARM_PageDirectoryObject:
        ফেরত PDE_আকার_BITS + PD_সূচক_BITS;
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র seL4_ARM_IOPageTableObject:
        ফেরত seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_ARM_VCPUObject:
        ফেরত VCPU_আকার_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ফেরত cap_small_frame_cap_new(
                   ASID_নিচের(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_উপরের(asidInvalid),
                   (word_t)regionBase);

    ক্ষেত্র seL4_ARM_LargePageObject:
        যদি (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } অন্যথা {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   ARMLargePage, ASID_নিচের(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপরের(asidInvalid),
                   (word_t)regionBase);

    ক্ষেত্র seL4_ARM_SectionObject:
        যদি (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } অন্যথা {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMSection)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   ARMSection, ASID_নিচের(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপরের(asidInvalid),
                   (word_t)regionBase);

    ক্ষেত্র seL4_ARM_SuperSectionObject:
        যদি (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } অন্যথা {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_আকার(ARMSuperSection)),
                                addrFromPPtr(regionBase));
        }
        ফেরত cap_frame_cap_new(
                   ARMSuperSection, ASID_নিচের(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপরের(asidInvalid),
                   (word_t)regionBase);

    ক্ষেত্র seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ফেরত cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    ক্ষেত্র seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#অন্যথা  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        ফেরত cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ফেরত cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        ফেরত cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    পূর্বনির্ধারিত:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t invLabel, word_t দৈর্ঘ্য, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *বাফার)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#যদি defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    নির্বাচন (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    ক্ষেত্র cap_io_space_cap:
        ফেরত decodeARMIOSpaceInvocation(invLabel, cap);
    ক্ষেত্র cap_io_page_table_cap:
        ফেরত decodeARMIOPTInvocation(invLabel, দৈর্ঘ্য, slot, cap, বাফার);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ক্ষেত্র cap_vcpu_cap:
        ফেরত decodeARMVCPUInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ক্ষেত্র cap_sgi_signal_cap:
        ফেরত decodeSGISignalInvocation(invLabel, দৈর্ঘ্য, cap, বাফার);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    পূর্বনির্ধারিত:
#অন্যথা
{
#endif
    ফেরত decodeARMMMUInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
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

