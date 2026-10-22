/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <arch/object/objecttype.h>
#അന്തര്ഭാവ <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#അന്തര്ഭാവ <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    തിരഞ്ഞെടുപ്പ് (type) {
    അവസ്ഥ seL4_ARM_SmallPageObject:
        തിരികെ true;
    അവസ്ഥ seL4_ARM_LargePageObject:
        തിരികെ true;
    അവസ്ഥ seL4_ARM_SectionObject:
        തിരികെ true;
    അവസ്ഥ seL4_ARM_SuperSectionObject:
        തിരികെ true;
    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    അവസ്ഥ cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    അവസ്ഥ cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    അവസ്ഥ cap_asid_control_cap:
    അവസ്ഥ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    അവസ്ഥ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
#endif

#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;
#endif
    സ്ഥിരസ്ഥിതി:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    തിരികെ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        തിരികെ cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } അല്ലെങ്കിൽ എങ്കിൽ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        തിരികെ cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } അല്ലെങ്കിൽ {
        തിരികെ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        നിർത്തുക;

    അവസ്ഥ cap_small_frame_cap:
        എങ്കിൽ (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            എങ്കിൽ (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                നിർത്തുക;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (ശൂന്യം *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        നിർത്തുക;

    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            എങ്കിൽ (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                എങ്കിൽ (pptr_to_paddr((ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((ശൂന്യം *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((ശൂന്യം *)&armKSGlobalLogPT[0]));

                    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (ശൂന്യം *)cap_frame_cap_get_capFBasePtr(cap));
        }
        നിർത്തുക;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap:
        എങ്കിൽ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        നിർത്തുക;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    അവസ്ഥ cap_sgi_signal_cap:
        // do nothing
        നിർത്തുക;
#endif


#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap:
        എങ്കിൽ (final) {
            clearIOPageDirectory(cap);
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        നിർത്തുക;
#endif

    സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap_a)) {
    അവസ്ഥ cap_small_frame_cap:
    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, മുകൾ_A, മുകൾ_B;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            മുകൾ_A = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            മുകൾ_B = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            തിരികെ ((botA <= botB) && (മുകൾ_A >= മുകൾ_B) && (botB <= മുകൾ_B));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_table_cap) {
            തിരികെ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            തിരികെ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            തിരികെ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        നിർത്തുക;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            തിരികെ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        നിർത്തുക;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    അവസ്ഥ cap_sgi_signal_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            തിരികെ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        നിർത്തുക;
#endif


#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_space_cap) {
            തിരികെ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            തിരികെ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        നിർത്തുക;
#endif
    }

    തിരികെ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    എങ്കിൽ (cap_get_capType(cap_a) == cap_small_frame_cap) {
        എങ്കിൽ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            തിരികെ ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } അല്ലെങ്കിൽ എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            തിരികെ false;
        }
    }
    എങ്കിൽ (cap_get_capType(cap_a) == cap_frame_cap) {
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            തിരികെ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } അല്ലെങ്കിൽ എങ്കിൽ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            തിരികെ false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    എങ്കിൽ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        തിരികെ false;
    }
#endif

    തിരികെ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_ARM_SmallPageObject:
        തിരികെ ARMSmallPageBits;
    അവസ്ഥ seL4_ARM_LargePageObject:
        തിരികെ ARMLargePageBits;
    അവസ്ഥ seL4_ARM_SectionObject:
        തിരികെ ARMSectionBits;
    അവസ്ഥ seL4_ARM_SuperSectionObject:
        തിരികെ ARMSuperSectionBits;
    അവസ്ഥ seL4_ARM_PageTableObject:
        തിരികെ PTE_SIZE_BITS + PT_INDEX_BITS;
    അവസ്ഥ seL4_ARM_PageDirectoryObject:
        തിരികെ PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ seL4_ARM_IOPageTableObject:
        തിരികെ seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_ARM_VCPUObject:
        തിരികെ VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    സ്ഥിരസ്ഥിതി:
        fail("Invalid object type");
        തിരികെ 0;
    }
}

cap_t Arch_createObject(object_t t, ശൂന്യം *regionBase, word_t userSize, bool_t deviceMemory)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_ARM_SmallPageObject:
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        തിരികെ cap_small_frame_cap_new(
                   ASID_താഴെയുള്ള(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_മുകളിലെ(asidInvalid),
                   (word_t)regionBase);

    അവസ്ഥ seL4_ARM_LargePageObject:
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        തിരികെ cap_frame_cap_new(
                   ARMLargePage, ASID_താഴെയുള്ള(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_മുകളിലെ(asidInvalid),
                   (word_t)regionBase);

    അവസ്ഥ seL4_ARM_SectionObject:
        എങ്കിൽ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } അല്ലെങ്കിൽ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        തിരികെ cap_frame_cap_new(
                   ARMSection, ASID_താഴെയുള്ള(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_മുകളിലെ(asidInvalid),
                   (word_t)regionBase);

    അവസ്ഥ seL4_ARM_SuperSectionObject:
        എങ്കിൽ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } അല്ലെങ്കിൽ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        തിരികെ cap_frame_cap_new(
                   ARMSuperSection, ASID_താഴെയുള്ള(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_മുകളിലെ(asidInvalid),
                   (word_t)regionBase);

    അവസ്ഥ seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        തിരികെ cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    അവസ്ഥ seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#അല്ലെങ്കിൽ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        തിരികെ cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        തിരികെ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        തിരികെ cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    സ്ഥിരസ്ഥിതി:
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
#എങ്കിൽ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    അവസ്ഥ cap_io_space_cap:
        തിരികെ decodeARMIOSpaceInvocation(invLabel, cap);
    അവസ്ഥ cap_io_page_table_cap:
        തിരികെ decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap:
        തിരികെ decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    അവസ്ഥ cap_sgi_signal_cap:
        തിരികെ decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    സ്ഥിരസ്ഥിതി:
#അല്ലെങ്കിൽ
{
#endif
    തിരികെ decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

ശൂന്യം
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    എങ്കിൽ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

