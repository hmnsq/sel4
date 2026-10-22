/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    ବାଛ (type) {
    ସ୍ଥିତି seL4_ARM_SmallPageObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_ARM_LargePageObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_ARM_SectionObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_ARM_SuperSectionObject:
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    ସ୍ଥିତି cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_asid_control_cap:
    ସ୍ଥିତି cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ସ୍ଥିତି cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
#endif

#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ଫେରନ୍ତୁ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ଯଦି (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ଫେରନ୍ତୁ cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } ଅନ୍ୟଥା ଯଦି (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ଫେରନ୍ତୁ cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_small_frame_cap:
        ଯଦି (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            ଯଦି (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                ଭାଙ୍ଗ;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (ଶୂନ୍ୟ *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ଯଦି (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                ଯଦି (pptr_to_paddr((ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((ଶୂନ୍ୟ *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((ଶୂନ୍ୟ *)&armKSGlobalLogPT[0]));

                    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (ଶୂନ୍ୟ *)cap_frame_cap_get_capFBasePtr(cap));
        }
        ଭାଙ୍ଗ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap:
        ଯଦି (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ଭାଙ୍ଗ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ସ୍ଥିତି cap_sgi_signal_cap:
        // do nothing
        ଭାଙ୍ଗ;
#endif


#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଯଦି (final) {
            clearIOPageDirectory(cap);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ଭାଙ୍ଗ;
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {
    ସ୍ଥିତି cap_small_frame_cap:
    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ଶୀର୍ଷ_A, ଶୀର୍ଷ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            ଶୀର୍ଷ_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            ଶୀର୍ଷ_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            ଫେରନ୍ତୁ ((botA <= botB) && (ଶୀର୍ଷ_A >= ଶୀର୍ଷ_B) && (botB <= ଶୀର୍ଷ_B));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_table_cap) {
            ଫେରନ୍ତୁ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ଫେରନ୍ତୁ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ଫେରନ୍ତୁ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ଭାଙ୍ଗ;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ଫେରନ୍ତୁ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ଭାଙ୍ଗ;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ସ୍ଥିତି cap_sgi_signal_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ଫେରନ୍ତୁ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ଭାଙ୍ଗ;
#endif


#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_space_cap) {
            ଫେରନ୍ତୁ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ଫେରନ୍ତୁ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;
#endif
    }

    ଫେରନ୍ତୁ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ଯଦି (cap_get_capType(cap_a) == cap_small_frame_cap) {
        ଯଦି (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ଫେରନ୍ତୁ ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } ଅନ୍ୟଥା ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            ଫେରନ୍ତୁ false;
        }
    }
    ଯଦି (cap_get_capType(cap_a) == cap_frame_cap) {
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            ଫେରନ୍ତୁ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } ଅନ୍ୟଥା ଯଦି (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ଫେରନ୍ତୁ false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ଯଦି (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ଫେରନ୍ତୁ false;
    }
#endif

    ଫେରନ୍ତୁ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_ARM_SmallPageObject:
        ଫେରନ୍ତୁ ARMSmallPageBits;
    ସ୍ଥିତି seL4_ARM_LargePageObject:
        ଫେରନ୍ତୁ ARMLargePageBits;
    ସ୍ଥିତି seL4_ARM_SectionObject:
        ଫେରନ୍ତୁ ARMSectionBits;
    ସ୍ଥିତି seL4_ARM_SuperSectionObject:
        ଫେରନ୍ତୁ ARMSuperSectionBits;
    ସ୍ଥିତି seL4_ARM_PageTableObject:
        ଫେରନ୍ତୁ PTE_SIZE_BITS + PT_INDEX_BITS;
    ସ୍ଥିତି seL4_ARM_PageDirectoryObject:
        ଫେରନ୍ତୁ PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି seL4_ARM_IOPageTableObject:
        ଫେରନ୍ତୁ seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_ARM_VCPUObject:
        ଫେରନ୍ତୁ VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        fail("Invalid object type");
        ଫେରନ୍ତୁ 0;
    }
}

cap_t Arch_createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_ARM_SmallPageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ଫେରନ୍ତୁ cap_small_frame_cap_new(
                   ASID_ତଳ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_ଉପର(asidInvalid),
                   (word_t)regionBase);

    ସ୍ଥିତି seL4_ARM_LargePageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   ARMLargePage, ASID_ତଳ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ଉପର(asidInvalid),
                   (word_t)regionBase);

    ସ୍ଥିତି seL4_ARM_SectionObject:
        ଯଦି (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } ଅନ୍ୟଥା {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   ARMSection, ASID_ତଳ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ଉପର(asidInvalid),
                   (word_t)regionBase);

    ସ୍ଥିତି seL4_ARM_SuperSectionObject:
        ଯଦି (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } ଅନ୍ୟଥା {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   ARMSuperSection, ASID_ତଳ(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ଉପର(asidInvalid),
                   (word_t)regionBase);

    ସ୍ଥିତି seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ଫେରନ୍ତୁ cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    ସ୍ଥିତି seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#ଅନ୍ୟଥା  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        ଫେରନ୍ତୁ cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ଫେରନ୍ତୁ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        ଫେରନ୍ତୁ cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
#ଯଦି defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ବାଛ (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଫେରନ୍ତୁ decodeARMIOSpaceInvocation(invLabel, cap);
    ସ୍ଥିତି cap_io_page_table_cap:
        ଫେରନ୍ତୁ decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap:
        ଫେରନ୍ତୁ decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ସ୍ଥିତି cap_sgi_signal_cap:
        ଫେରନ୍ତୁ decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ଅନ୍ୟଥା
{
#endif
    ଫେରନ୍ତୁ decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

ଶୂନ୍ୟ
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ଯଦି (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

