/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ seL4_ARM_SmallPageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_ARM_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_ARM_SectionObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_ARM_SuperSectionObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    ಪಕ್ಷ cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_asid_control_cap:
    ಪಕ್ಷ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಪಕ್ಷ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
#endif

#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;
#endif
    ಸ್ವಭಾವ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ಪ್ರತ್ಯಾಗಮ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ಪ್ರತ್ಯಾಗಮ cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } ಅನ್ಯಥಾ ಯದಿ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_small_frame_cap:
        ಯದಿ (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            ಯದಿ (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                ಭಙ್ಗ;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (ಶೂನ್ಯ *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ಯದಿ (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                ಯದಿ (pptr_to_paddr((ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((ಶೂನ್ಯ *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((ಶೂನ್ಯ *)&armKSGlobalLogPT[0]));

                    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (ಶೂನ್ಯ *)cap_frame_cap_get_capFBasePtr(cap));
        }
        ಭಙ್ಗ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap:
        ಯದಿ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ಭಙ್ಗ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಪಕ್ಷ cap_sgi_signal_cap:
        // do nothing
        ಭಙ್ಗ;
#endif


#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಯದಿ (final) {
            clearIOPageDirectory(cap);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ಭಙ್ಗ;
#endif

    ಸ್ವಭಾವ:
        ಭಙ್ಗ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {
    ಪಕ್ಷ cap_small_frame_cap:
    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            ತುದಿ_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            ತುದಿ_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            ಪ್ರತ್ಯಾಗಮ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ಭಙ್ಗ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ಭಙ್ಗ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಪಕ್ಷ cap_sgi_signal_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ಪ್ರತ್ಯಾಗಮ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ಭಙ್ಗ;
#endif


#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;
#endif
    }

    ಪ್ರತ್ಯಾಗಮ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಯದಿ (cap_get_capType(cap_a) == cap_small_frame_cap) {
        ಯದಿ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } ಅನ್ಯಥಾ ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
    }
    ಯದಿ (cap_get_capType(cap_a) == cap_frame_cap) {
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } ಅನ್ಯಥಾ ಯದಿ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಯದಿ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
#endif

    ಪ್ರತ್ಯಾಗಮ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_ARM_SmallPageObject:
        ಪ್ರತ್ಯಾಗಮ ARMSmallPageBits;
    ಪಕ್ಷ seL4_ARM_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ ARMLargePageBits;
    ಪಕ್ಷ seL4_ARM_SectionObject:
        ಪ್ರತ್ಯಾಗಮ ARMSectionBits;
    ಪಕ್ಷ seL4_ARM_SuperSectionObject:
        ಪ್ರತ್ಯಾಗಮ ARMSuperSectionBits;
    ಪಕ್ಷ seL4_ARM_PageTableObject:
        ಪ್ರತ್ಯಾಗಮ PTE_SIZE_BITS + PT_INDEX_BITS;
    ಪಕ್ಷ seL4_ARM_PageDirectoryObject:
        ಪ್ರತ್ಯಾಗಮ PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ seL4_ARM_IOPageTableObject:
        ಪ್ರತ್ಯಾಗಮ seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ seL4_ARM_VCPUObject:
        ಪ್ರತ್ಯಾಗಮ VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ಸ್ವಭಾವ:
        fail("Invalid object type");
        ಪ್ರತ್ಯಾಗಮ 0;
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_ARM_SmallPageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ಪ್ರತ್ಯಾಗಮ cap_small_frame_cap_new(
                   ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_ಮೇಲಿನ(asidInvalid),
                   (word_t)regionBase);

    ಪಕ್ಷ seL4_ARM_LargePageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   ARMLargePage, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ಮೇಲಿನ(asidInvalid),
                   (word_t)regionBase);

    ಪಕ್ಷ seL4_ARM_SectionObject:
        ಯದಿ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } ಅನ್ಯಥಾ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   ARMSection, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ಮೇಲಿನ(asidInvalid),
                   (word_t)regionBase);

    ಪಕ್ಷ seL4_ARM_SuperSectionObject:
        ಯದಿ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } ಅನ್ಯಥಾ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   ARMSuperSection, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ಮೇಲಿನ(asidInvalid),
                   (word_t)regionBase);

    ಪಕ್ಷ seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    ಪಕ್ಷ seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#ಅನ್ಯಥಾ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        ಪ್ರತ್ಯಾಗಮ cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    ಸ್ವಭಾವ:
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
#ಯದಿ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMIOSpaceInvocation(invLabel, cap);
    ಪಕ್ಷ cap_io_page_table_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಪಕ್ಷ cap_sgi_signal_cap:
        ಪ್ರತ್ಯಾಗಮ decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    ಸ್ವಭಾವ:
#ಅನ್ಯಥಾ
{
#endif
    ಪ್ರತ್ಯಾಗಮ decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

ಶೂನ್ಯ
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಯದಿ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

