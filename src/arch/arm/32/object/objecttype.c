/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <arch/object/objecttype.h>
#અન્તર્ભાવ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#અન્તર્ભાવ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    પસંદગી (type) {
    સ્થિતિ seL4_ARM_SmallPageObject:
        પાછા true;
    સ્થિતિ seL4_ARM_LargePageObject:
        પાછા true;
    સ્થિતિ seL4_ARM_SectionObject:
        પાછા true;
    સ્થિતિ seL4_ARM_SuperSectionObject:
        પાછા true;
    મૂળભૂત:
        પાછા false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_table_cap:
        જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_page_directory_cap:
        જો (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    સ્થિતિ cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ cap_asid_control_cap:
    સ્થિતિ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    સ્થિતિ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
#endif

#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;

    સ્થિતિ cap_io_page_table_cap:
        જો (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;
#endif
    મૂળભૂત:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    પાછા cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    જો (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        પાછા cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } નહીંતર જો (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        પાછા cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } નહીંતર {
        પાછા cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_asid_pool_cap:
        જો (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        તોડો;

    સ્થિતિ cap_page_directory_cap:
        જો (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        તોડો;

    સ્થિતિ cap_page_table_cap:
        જો (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        તોડો;

    સ્થિતિ cap_small_frame_cap:
        જો (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            જો (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                તોડો;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (શૂન્ય *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        તોડો;

    સ્થિતિ cap_frame_cap:
        જો (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            જો (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                જો (pptr_to_paddr((શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((શૂન્ય *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((શૂન્ય *)&armKSGlobalLogPT[0]));

                    માટે (પૂર્ણાંક idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (શૂન્ય *)cap_frame_cap_get_capFBasePtr(cap));
        }
        તોડો;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap:
        જો (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        તોડો;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    સ્થિતિ cap_sgi_signal_cap:
        // do nothing
        તોડો;
#endif


#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap:
        જો (final) {
            clearIOPageDirectory(cap);
        }
        તોડો;

    સ્થિતિ cap_io_page_table_cap:
        જો (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        તોડો;
#endif

    મૂળભૂત:
        તોડો;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    પાછા fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    પસંદગી (cap_get_capType(cap_a)) {
    સ્થિતિ cap_small_frame_cap:
    સ્થિતિ cap_frame_cap:
        જો (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ટોચ_A, ટોચ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            ટોચ_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            ટોચ_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            પાછા ((botA <= botB) && (ટોચ_A >= ટોચ_B) && (botB <= ટોચ_B));
        }
        તોડો;

    સ્થિતિ cap_page_table_cap:
        જો (cap_get_capType(cap_b) == cap_page_table_cap) {
            પાછા cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_page_directory_cap:
        જો (cap_get_capType(cap_b) == cap_page_directory_cap) {
            પાછા cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_asid_control_cap:
        જો (cap_get_capType(cap_b) == cap_asid_control_cap) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_asid_pool_cap:
        જો (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            પાછા cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        તોડો;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap:
        જો (cap_get_capType(cap_b) == cap_vcpu_cap) {
            પાછા cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        તોડો;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    સ્થિતિ cap_sgi_signal_cap:
        જો (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            પાછા (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        તોડો;
#endif


#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap:
        જો (cap_get_capType(cap_b) == cap_io_space_cap) {
            પાછા cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        તોડો;

    સ્થિતિ cap_io_page_table_cap:
        જો (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            પાછા cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        તોડો;
#endif
    }

    પાછા false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    જો (cap_get_capType(cap_a) == cap_small_frame_cap) {
        જો (cap_get_capType(cap_b) == cap_small_frame_cap) {
            પાછા ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } નહીંતર જો (cap_get_capType(cap_b) == cap_frame_cap) {
            પાછા false;
        }
    }
    જો (cap_get_capType(cap_a) == cap_frame_cap) {
        જો (cap_get_capType(cap_b) == cap_frame_cap) {
            પાછા ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } નહીંતર જો (cap_get_capType(cap_b) == cap_small_frame_cap) {
            પાછા false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    જો (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        પાછા false;
    }
#endif

    પાછા Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    પસંદગી (t) {
    સ્થિતિ seL4_ARM_SmallPageObject:
        પાછા ARMSmallPageBits;
    સ્થિતિ seL4_ARM_LargePageObject:
        પાછા ARMLargePageBits;
    સ્થિતિ seL4_ARM_SectionObject:
        પાછા ARMSectionBits;
    સ્થિતિ seL4_ARM_SuperSectionObject:
        પાછા ARMSuperSectionBits;
    સ્થિતિ seL4_ARM_PageTableObject:
        પાછા PTE_SIZE_BITS + PT_INDEX_BITS;
    સ્થિતિ seL4_ARM_PageDirectoryObject:
        પાછા PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ seL4_ARM_IOPageTableObject:
        પાછા seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ seL4_ARM_VCPUObject:
        પાછા VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    મૂળભૂત:
        fail("Invalid object type");
        પાછા 0;
    }
}

cap_t Arch_createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
    પસંદગી (t) {
    સ્થિતિ seL4_ARM_SmallPageObject:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        પાછા cap_small_frame_cap_new(
                   ASID_નીચલું(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_ઉપલું(asidInvalid),
                   (word_t)regionBase);

    સ્થિતિ seL4_ARM_LargePageObject:
        જો (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } નહીંતર {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        પાછા cap_frame_cap_new(
                   ARMLargePage, ASID_નીચલું(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ઉપલું(asidInvalid),
                   (word_t)regionBase);

    સ્થિતિ seL4_ARM_SectionObject:
        જો (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } નહીંતર {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        પાછા cap_frame_cap_new(
                   ARMSection, ASID_નીચલું(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ઉપલું(asidInvalid),
                   (word_t)regionBase);

    સ્થિતિ seL4_ARM_SuperSectionObject:
        જો (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } નહીંતર {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        પાછા cap_frame_cap_new(
                   ARMSuperSection, ASID_નીચલું(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_ઉપલું(asidInvalid),
                   (word_t)regionBase);

    સ્થિતિ seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        પાછા cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    સ્થિતિ seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#નહીંતર  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        પાછા cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        પાછા cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    સ્થિતિ seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        પાછા cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    મૂળભૂત:
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
#જો defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    પસંદગી (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    સ્થિતિ cap_io_space_cap:
        પાછા decodeARMIOSpaceInvocation(invLabel, cap);
    સ્થિતિ cap_io_page_table_cap:
        પાછા decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    સ્થિતિ cap_vcpu_cap:
        પાછા decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    સ્થિતિ cap_sgi_signal_cap:
        પાછા decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    મૂળભૂત:
#નહીંતર
{
#endif
    પાછા decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

શૂન્ય
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    જો (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

