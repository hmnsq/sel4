/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <types.h>
#অন্তর্ভাৱ <api/failures.h>
#অন্তর্ভাৱ <kernel/vspace.h>
#অন্তর্ভাৱ <object/structures.h>
#অন্তর্ভাৱ <arch/machine.h>
#অন্তর্ভাৱ <arch/model/statedata.h>
#অন্তর্ভাৱ <arch/object/objecttype.h>
#অন্তর্ভাৱ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#অন্তর্ভাৱ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    ৱিকল্প (type) {
    পক্ষ seL4_ARM_SmallPageObject:
        ঘূৰাই true;
    পক্ষ seL4_ARM_LargePageObject:
        ঘূৰাই true;
    পক্ষ seL4_ARM_SectionObject:
        ঘূৰাই true;
    পক্ষ seL4_ARM_SuperSectionObject:
        ঘূৰাই true;
    স্ৱভাৱ:
        ঘূৰাই false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ৱিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ঘূৰাই ret;

    পক্ষ cap_page_directory_cap:
        যদি (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ঘূৰাই ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    পক্ষ cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;

    পক্ষ cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;

    পক্ষ cap_asid_control_cap:
    পক্ষ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    পক্ষ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;
#endif

#ifdef CONFIG_TK1_SMMU
    পক্ষ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ঘূৰাই ret;

    পক্ষ cap_io_page_table_cap:
        যদি (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ঘূৰাই ret;
#endif
    স্ৱভাৱ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ঘূৰাই cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ঘূৰাই cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } অন্যথা যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ঘূৰাই cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } অন্যথা {
        ঘূৰাই cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ৱিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_asid_pool_cap:
        যদি (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ভঙ্গ;

    পক্ষ cap_page_directory_cap:
        যদি (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        ভঙ্গ;

    পক্ষ cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        ভঙ্গ;

    পক্ষ cap_small_frame_cap:
        যদি (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            যদি (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                ভঙ্গ;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (শূন্য *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        ভঙ্গ;

    পক্ষ cap_frame_cap:
        যদি (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            যদি (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                যদি (pptr_to_paddr((শূন্য *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((শূন্য *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((শূন্য *)&armKSGlobalLogPT[0]));

                    বাবে (পূর্ণাঙ্ক idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
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
        ভঙ্গ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ cap_vcpu_cap:
        যদি (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ভঙ্গ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    পক্ষ cap_sgi_signal_cap:
        // do nothing
        ভঙ্গ;
#endif


#ifdef CONFIG_TK1_SMMU
    পক্ষ cap_io_space_cap:
        যদি (final) {
            clearIOPageDirectory(cap);
        }
        ভঙ্গ;

    পক্ষ cap_io_page_table_cap:
        যদি (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ভঙ্গ;
#endif

    স্ৱভাৱ:
        ভঙ্গ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ঘূৰাই fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ৱিকল্প (cap_get_capType(cap_a)) {
    পক্ষ cap_small_frame_cap:
    পক্ষ cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, শীৰ্ষ_A, শীৰ্ষ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            শীৰ্ষ_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            শীৰ্ষ_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            ঘূৰাই ((botA <= botB) && (শীৰ্ষ_A >= শীৰ্ষ_B) && (botB <= শীৰ্ষ_B));
        }
        ভঙ্গ;

    পক্ষ cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            ঘূৰাই cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_page_directory_cap:
        যদি (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ঘূৰাই cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_asid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ঘূৰাই true;
        }
        ভঙ্গ;

    পক্ষ cap_asid_pool_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ঘূৰাই cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ভঙ্গ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ cap_vcpu_cap:
        যদি (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ঘূৰাই cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ভঙ্গ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    পক্ষ cap_sgi_signal_cap:
        যদি (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ঘূৰাই (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ভঙ্গ;
#endif


#ifdef CONFIG_TK1_SMMU
    পক্ষ cap_io_space_cap:
        যদি (cap_get_capType(cap_b) == cap_io_space_cap) {
            ঘূৰাই cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_io_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ঘূৰাই cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ভঙ্গ;
#endif
    }

    ঘূৰাই false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_small_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ঘূৰাই ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } অন্যথা যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ঘূৰাই false;
        }
    }
    যদি (cap_get_capType(cap_a) == cap_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ঘূৰাই ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } অন্যথা যদি (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ঘূৰাই false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    যদি (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ঘূৰাই false;
    }
#endif

    ঘূৰাই Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ৱিকল্প (t) {
    পক্ষ seL4_ARM_SmallPageObject:
        ঘূৰাই ARMSmallPageBits;
    পক্ষ seL4_ARM_LargePageObject:
        ঘূৰাই ARMLargePageBits;
    পক্ষ seL4_ARM_SectionObject:
        ঘূৰাই ARMSectionBits;
    পক্ষ seL4_ARM_SuperSectionObject:
        ঘূৰাই ARMSuperSectionBits;
    পক্ষ seL4_ARM_PageTableObject:
        ঘূৰাই PTE_SIZE_BITS + PT_INDEX_BITS;
    পক্ষ seL4_ARM_PageDirectoryObject:
        ঘূৰাই PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    পক্ষ seL4_ARM_IOPageTableObject:
        ঘূৰাই seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ seL4_ARM_VCPUObject:
        ঘূৰাই VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    স্ৱভাৱ:
        fail("Invalid object type");
        ঘূৰাই 0;
    }
}

cap_t Arch_createObject(object_t t, শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
    ৱিকল্প (t) {
    পক্ষ seL4_ARM_SmallPageObject:
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ঘূৰাই cap_small_frame_cap_new(
                   ASID_নিচেৰ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_উপৰেৰ(asidInvalid),
                   (word_t)regionBase);

    পক্ষ seL4_ARM_LargePageObject:
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ঘূৰাই cap_frame_cap_new(
                   ARMLargePage, ASID_নিচেৰ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপৰেৰ(asidInvalid),
                   (word_t)regionBase);

    পক্ষ seL4_ARM_SectionObject:
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSection)),
                                addrFromPPtr(regionBase));
        }
        ঘূৰাই cap_frame_cap_new(
                   ARMSection, ASID_নিচেৰ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপৰেৰ(asidInvalid),
                   (word_t)regionBase);

    পক্ষ seL4_ARM_SuperSectionObject:
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSuperSection)),
                                addrFromPPtr(regionBase));
        }
        ঘূৰাই cap_frame_cap_new(
                   ARMSuperSection, ASID_নিচেৰ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_উপৰেৰ(asidInvalid),
                   (word_t)regionBase);

    পক্ষ seL4_ARM_PageTableObject:
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
        ঘূৰাই cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    পক্ষ seL4_ARM_PageDirectoryObject:
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

        ঘূৰাই cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ঘূৰাই cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    পক্ষ seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        ঘূৰাই cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    স্ৱভাৱ:
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
#যদি defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ৱিকল্প (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    পক্ষ cap_io_space_cap:
        ঘূৰাই decodeARMIOSpaceInvocation(invLabel, cap);
    পক্ষ cap_io_page_table_cap:
        ঘূৰাই decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    পক্ষ cap_vcpu_cap:
        ঘূৰাই decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    পক্ষ cap_sgi_signal_cap:
        ঘূৰাই decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    স্ৱভাৱ:
#অন্যথা
{
#endif
    ঘূৰাই decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
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

