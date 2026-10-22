/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <arch/object/objecttype.h>
#அந்தர்பாவ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#அந்தர்பாவ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    தேர்வு (type) {
    நிலை seL4_ARM_SmallPageObject:
        திரும்பு true;
    நிலை seL4_ARM_LargePageObject:
        திரும்பு true;
    நிலை seL4_ARM_SectionObject:
        திரும்பு true;
    நிலை seL4_ARM_SuperSectionObject:
        திரும்பு true;
    இயல்பு:
        திரும்பு false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_table_cap:
        எனில் (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_page_directory_cap:
        எனில் (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    நிலை cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

    நிலை cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

    நிலை cap_asid_control_cap:
    நிலை cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    நிலை cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
#endif

#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

    நிலை cap_io_page_table_cap:
        எனில் (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;
#endif
    இயல்பு:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    திரும்பு cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    எனில் (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        திரும்பு cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } இல்லையெனில் எனில் (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        திரும்பு cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } இல்லையெனில் {
        திரும்பு cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_asid_pool_cap:
        எனில் (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        நிறுத்து;

    நிலை cap_page_directory_cap:
        எனில் (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        நிறுத்து;

    நிலை cap_small_frame_cap:
        எனில் (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            எனில் (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                நிறுத்து;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (வெறுமை *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        நிறுத்து;

    நிலை cap_frame_cap:
        எனில் (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            எனில் (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                எனில் (pptr_to_paddr((வெறுமை *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((வெறுமை *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((வெறுமை *)&armKSGlobalLogPT[0]));

                    க்கு (முழு idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (வெறுமை *)cap_frame_cap_get_capFBasePtr(cap));
        }
        நிறுத்து;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap:
        எனில் (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        நிறுத்து;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    நிலை cap_sgi_signal_cap:
        // do nothing
        நிறுத்து;
#endif


#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap:
        எனில் (final) {
            clearIOPageDirectory(cap);
        }
        நிறுத்து;

    நிலை cap_io_page_table_cap:
        எனில் (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        நிறுத்து;
#endif

    இயல்பு:
        நிறுத்து;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    திரும்பு fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    தேர்வு (cap_get_capType(cap_a)) {
    நிலை cap_small_frame_cap:
    நிலை cap_frame_cap:
        எனில் (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, உச்சி_A, உச்சி_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            உச்சி_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            உச்சி_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            திரும்பு ((botA <= botB) && (உச்சி_A >= உச்சி_B) && (botB <= உச்சி_B));
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (cap_get_capType(cap_b) == cap_page_table_cap) {
            திரும்பு cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_page_directory_cap:
        எனில் (cap_get_capType(cap_b) == cap_page_directory_cap) {
            திரும்பு cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_asid_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_asid_control_cap) {
            திரும்பு true;
        }
        நிறுத்து;

    நிலை cap_asid_pool_cap:
        எனில் (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            திரும்பு cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        நிறுத்து;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap:
        எனில் (cap_get_capType(cap_b) == cap_vcpu_cap) {
            திரும்பு cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        நிறுத்து;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    நிலை cap_sgi_signal_cap:
        எனில் (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            திரும்பு (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        நிறுத்து;
#endif


#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_space_cap) {
            திரும்பு cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        நிறுத்து;

    நிலை cap_io_page_table_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            திரும்பு cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        நிறுத்து;
#endif
    }

    திரும்பு false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    எனில் (cap_get_capType(cap_a) == cap_small_frame_cap) {
        எனில் (cap_get_capType(cap_b) == cap_small_frame_cap) {
            திரும்பு ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } இல்லையெனில் எனில் (cap_get_capType(cap_b) == cap_frame_cap) {
            திரும்பு false;
        }
    }
    எனில் (cap_get_capType(cap_a) == cap_frame_cap) {
        எனில் (cap_get_capType(cap_b) == cap_frame_cap) {
            திரும்பு ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } இல்லையெனில் எனில் (cap_get_capType(cap_b) == cap_small_frame_cap) {
            திரும்பு false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    எனில் (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        திரும்பு false;
    }
#endif

    திரும்பு Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    தேர்வு (t) {
    நிலை seL4_ARM_SmallPageObject:
        திரும்பு ARMSmallPageBits;
    நிலை seL4_ARM_LargePageObject:
        திரும்பு ARMLargePageBits;
    நிலை seL4_ARM_SectionObject:
        திரும்பு ARMSectionBits;
    நிலை seL4_ARM_SuperSectionObject:
        திரும்பு ARMSuperSectionBits;
    நிலை seL4_ARM_PageTableObject:
        திரும்பு PTE_SIZE_BITS + PT_INDEX_BITS;
    நிலை seL4_ARM_PageDirectoryObject:
        திரும்பு PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    நிலை seL4_ARM_IOPageTableObject:
        திரும்பு seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_ARM_VCPUObject:
        திரும்பு VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    இயல்பு:
        fail("Invalid object type");
        திரும்பு 0;
    }
}

cap_t Arch_createObject(object_t t, வெறுமை *regionBase, word_t userSize, bool_t deviceMemory)
{
    தேர்வு (t) {
    நிலை seL4_ARM_SmallPageObject:
        எனில் (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } இல்லையெனில் {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        திரும்பு cap_small_frame_cap_new(
                   ASID_கீழ்(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_உயர்வு(asidInvalid),
                   (word_t)regionBase);

    நிலை seL4_ARM_LargePageObject:
        எனில் (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } இல்லையெனில் {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        திரும்பு cap_frame_cap_new(
                   ARMLargePage, ASID_கீழ்(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_உயர்வு(asidInvalid),
                   (word_t)regionBase);

    நிலை seL4_ARM_SectionObject:
        எனில் (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } இல்லையெனில் {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        திரும்பு cap_frame_cap_new(
                   ARMSection, ASID_கீழ்(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_உயர்வு(asidInvalid),
                   (word_t)regionBase);

    நிலை seL4_ARM_SuperSectionObject:
        எனில் (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } இல்லையெனில் {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        திரும்பு cap_frame_cap_new(
                   ARMSuperSection, ASID_கீழ்(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_உயர்வு(asidInvalid),
                   (word_t)regionBase);

    நிலை seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        திரும்பு cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    நிலை seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#இல்லையெனில்  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        திரும்பு cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        திரும்பு cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    நிலை seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        திரும்பு cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    இயல்பு:
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
#எனில் defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    தேர்வு (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    நிலை cap_io_space_cap:
        திரும்பு decodeARMIOSpaceInvocation(invLabel, cap);
    நிலை cap_io_page_table_cap:
        திரும்பு decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap:
        திரும்பு decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    நிலை cap_sgi_signal_cap:
        திரும்பு decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    இயல்பு:
#இல்லையெனில்
{
#endif
    திரும்பு decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

வெறுமை
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    எனில் (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

