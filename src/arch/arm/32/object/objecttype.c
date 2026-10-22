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
    निवड (type) {
    स्थिती seL4_ARM_SmallPageObject:
        परत true;
    स्थिती seL4_ARM_LargePageObject:
        परत true;
    स्थिती seL4_ARM_SectionObject:
        परत true;
    स्थिती seL4_ARM_SuperSectionObject:
        परत true;
    पूर्वनिर्धारित:
        परत false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_table_cap:
        जर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_page_directory_cap:
        जर (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    स्थिती cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        परत ret;

    स्थिती cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        परत ret;

    स्थिती cap_asid_control_cap:
    स्थिती cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    स्थिती cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
#endif

#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;

    स्थिती cap_io_page_table_cap:
        जर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;
#endif
    पूर्वनिर्धारित:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    परत cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    जर (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        परत cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } अन्यथा जर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        परत cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } अन्यथा {
        परत cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_asid_pool_cap:
        जर (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        थांबा;

    स्थिती cap_page_directory_cap:
        जर (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        थांबा;

    स्थिती cap_small_frame_cap:
        जर (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            जर (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                थांबा;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (शून्य *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        थांबा;

    स्थिती cap_frame_cap:
        जर (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            जर (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                जर (pptr_to_paddr((शून्य *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((शून्य *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((शून्य *)&armKSGlobalLogPT[0]));

                    साठी (पूर्णांक idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
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
        थांबा;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap:
        जर (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        थांबा;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    स्थिती cap_sgi_signal_cap:
        // do nothing
        थांबा;
#endif


#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap:
        जर (final) {
            clearIOPageDirectory(cap);
        }
        थांबा;

    स्थिती cap_io_page_table_cap:
        जर (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        थांबा;
#endif

    पूर्वनिर्धारित:
        थांबा;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    परत fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    निवड (cap_get_capType(cap_a)) {
    स्थिती cap_small_frame_cap:
    स्थिती cap_frame_cap:
        जर (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            परत ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (cap_get_capType(cap_b) == cap_page_table_cap) {
            परत cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_page_directory_cap:
        जर (cap_get_capType(cap_b) == cap_page_directory_cap) {
            परत cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_asid_control_cap:
        जर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            परत true;
        }
        थांबा;

    स्थिती cap_asid_pool_cap:
        जर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            परत cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        थांबा;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap:
        जर (cap_get_capType(cap_b) == cap_vcpu_cap) {
            परत cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        थांबा;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    स्थिती cap_sgi_signal_cap:
        जर (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            परत (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        थांबा;
#endif


#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap:
        जर (cap_get_capType(cap_b) == cap_io_space_cap) {
            परत cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        थांबा;

    स्थिती cap_io_page_table_cap:
        जर (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            परत cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        थांबा;
#endif
    }

    परत false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जर (cap_get_capType(cap_a) == cap_small_frame_cap) {
        जर (cap_get_capType(cap_b) == cap_small_frame_cap) {
            परत ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा जर (cap_get_capType(cap_b) == cap_frame_cap) {
            परत false;
        }
    }
    जर (cap_get_capType(cap_a) == cap_frame_cap) {
        जर (cap_get_capType(cap_b) == cap_frame_cap) {
            परत ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } अन्यथा जर (cap_get_capType(cap_b) == cap_small_frame_cap) {
            परत false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    जर (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        परत false;
    }
#endif

    परत Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    निवड (t) {
    स्थिती seL4_ARM_SmallPageObject:
        परत ARMSmallPageBits;
    स्थिती seL4_ARM_LargePageObject:
        परत ARMLargePageBits;
    स्थिती seL4_ARM_SectionObject:
        परत ARMSectionBits;
    स्थिती seL4_ARM_SuperSectionObject:
        परत ARMSuperSectionBits;
    स्थिती seL4_ARM_PageTableObject:
        परत PTE_SIZE_BITS + PT_INDEX_BITS;
    स्थिती seL4_ARM_PageDirectoryObject:
        परत PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    स्थिती seL4_ARM_IOPageTableObject:
        परत seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती seL4_ARM_VCPUObject:
        परत VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    पूर्वनिर्धारित:
        fail("Invalid object type");
        परत 0;
    }
}

cap_t Arch_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    निवड (t) {
    स्थिती seL4_ARM_SmallPageObject:
        जर (deviceMemory) {
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        परत cap_small_frame_cap_new(
                   ASID_खालचा(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_वरचा(asidInvalid),
                   (word_t)regionBase);

    स्थिती seL4_ARM_LargePageObject:
        जर (deviceMemory) {
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        परत cap_frame_cap_new(
                   ARMLargePage, ASID_खालचा(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_वरचा(asidInvalid),
                   (word_t)regionBase);

    स्थिती seL4_ARM_SectionObject:
        जर (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } अन्यथा {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        परत cap_frame_cap_new(
                   ARMSection, ASID_खालचा(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_वरचा(asidInvalid),
                   (word_t)regionBase);

    स्थिती seL4_ARM_SuperSectionObject:
        जर (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } अन्यथा {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        परत cap_frame_cap_new(
                   ARMSuperSection, ASID_खालचा(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_वरचा(asidInvalid),
                   (word_t)regionBase);

    स्थिती seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        परत cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    स्थिती seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#अन्यथा  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        परत cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        परत cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    स्थिती seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        परत cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    पूर्वनिर्धारित:
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
#जर defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    निवड (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    स्थिती cap_io_space_cap:
        परत decodeARMIOSpaceInvocation(invLabel, cap);
    स्थिती cap_io_page_table_cap:
        परत decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap:
        परत decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    स्थिती cap_sgi_signal_cap:
        परत decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    पूर्वनिर्धारित:
#अन्यथा
{
#endif
    परत decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

शून्य
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    जर (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

