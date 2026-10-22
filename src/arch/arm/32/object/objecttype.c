/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/objecttype.h>
#شامل <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#شامل <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    انتخاب (type) {
    حالت seL4_ARM_SmallPageObject:
        بازگشت true;
    حالت seL4_ARM_LargePageObject:
        بازگشت true;
    حالت seL4_ARM_SectionObject:
        بازگشت true;
    حالت seL4_ARM_SuperSectionObject:
        بازگشت true;
    پیشفرض:
        بازگشت false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_table_cap:
        اگر (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_page_directory_cap:
        اگر (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    حالت cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

    حالت cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
#endif

#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

    حالت cap_io_page_table_cap:
        اگر (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;
#endif
    پیشفرض:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    بازگشت cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        بازگشت cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } وگرنه اگر (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        بازگشت cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } وگرنه {
        بازگشت cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        بشکن;

    حالت cap_page_directory_cap:
        اگر (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        بشکن;

    حالت cap_small_frame_cap:
        اگر (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            اگر (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                بشکن;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (تهی *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        بشکن;

    حالت cap_frame_cap:
        اگر (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            اگر (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                اگر (pptr_to_paddr((تهی *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((تهی *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((تهی *)&armKSGlobalLogPT[0]));

                    برای (صحیح idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (تهی *)cap_frame_cap_get_capFBasePtr(cap));
        }
        بشکن;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        بشکن;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        // do nothing
        بشکن;
#endif


#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        اگر (final) {
            clearIOPageDirectory(cap);
        }
        بشکن;

    حالت cap_io_page_table_cap:
        اگر (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        بشکن;
#endif

    پیشفرض:
        بشکن;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    بازگشت fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_small_frame_cap:
    حالت cap_frame_cap:
        اگر (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, بالاترین_A, بالاترین_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            بالاترین_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            بالاترین_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            بازگشت ((botA <= botB) && (بالاترین_A >= بالاترین_B) && (botB <= بالاترین_B));
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (cap_get_capType(cap_b) == cap_page_table_cap) {
            بازگشت cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        بشکن;

    حالت cap_page_directory_cap:
        اگر (cap_get_capType(cap_b) == cap_page_directory_cap) {
            بازگشت cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        بشکن;

    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            بازگشت true;
        }
        بشکن;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            بازگشت cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        بشکن;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            بازگشت cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        بشکن;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        اگر (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            بازگشت (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        بشکن;
#endif


#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        اگر (cap_get_capType(cap_b) == cap_io_space_cap) {
            بازگشت cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        بشکن;

    حالت cap_io_page_table_cap:
        اگر (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            بازگشت cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        بشکن;
#endif
    }

    بازگشت false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_small_frame_cap) {
        اگر (cap_get_capType(cap_b) == cap_small_frame_cap) {
            بازگشت ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } وگرنه اگر (cap_get_capType(cap_b) == cap_frame_cap) {
            بازگشت false;
        }
    }
    اگر (cap_get_capType(cap_a) == cap_frame_cap) {
        اگر (cap_get_capType(cap_b) == cap_frame_cap) {
            بازگشت ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } وگرنه اگر (cap_get_capType(cap_b) == cap_small_frame_cap) {
            بازگشت false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    اگر (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        بازگشت false;
    }
#endif

    بازگشت Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    انتخاب (t) {
    حالت seL4_ARM_SmallPageObject:
        بازگشت ARMSmallPageBits;
    حالت seL4_ARM_LargePageObject:
        بازگشت ARMLargePageBits;
    حالت seL4_ARM_SectionObject:
        بازگشت ARMSectionBits;
    حالت seL4_ARM_SuperSectionObject:
        بازگشت ARMSuperSectionBits;
    حالت seL4_ARM_PageTableObject:
        بازگشت PTE_SIZE_BITS + PT_INDEX_BITS;
    حالت seL4_ARM_PageDirectoryObject:
        بازگشت PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    حالت seL4_ARM_IOPageTableObject:
        بازگشت seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        بازگشت VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    پیشفرض:
        fail("Invalid object type");
        بازگشت 0;
    }
}

cap_t Arch_createObject(object_t t, تهی *regionBase, word_t userSize, bool_t deviceMemory)
{
    انتخاب (t) {
    حالت seL4_ARM_SmallPageObject:
        اگر (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } وگرنه {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        بازگشت cap_small_frame_cap_new(
                   ASID_کم(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_بالا(asidInvalid),
                   (word_t)regionBase);

    حالت seL4_ARM_LargePageObject:
        اگر (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } وگرنه {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        بازگشت cap_frame_cap_new(
                   ARMLargePage, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_بالا(asidInvalid),
                   (word_t)regionBase);

    حالت seL4_ARM_SectionObject:
        اگر (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } وگرنه {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        بازگشت cap_frame_cap_new(
                   ARMSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_بالا(asidInvalid),
                   (word_t)regionBase);

    حالت seL4_ARM_SuperSectionObject:
        اگر (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } وگرنه {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        بازگشت cap_frame_cap_new(
                   ARMSuperSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_بالا(asidInvalid),
                   (word_t)regionBase);

    حالت seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        بازگشت cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    حالت seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#وگرنه  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        بازگشت cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        بازگشت cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    حالت seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        بازگشت cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    پیشفرض:
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
#اگر defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    انتخاب (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        بازگشت decodeARMIOSpaceInvocation(invLabel, cap);
    حالت cap_io_page_table_cap:
        بازگشت decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        بازگشت decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        بازگشت decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    پیشفرض:
#وگرنه
{
#endif
    بازگشت decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

تهی
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    اگر (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

