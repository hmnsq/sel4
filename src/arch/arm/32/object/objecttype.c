/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <arch/object/objecttype.h>
#include <arch/machine/tlb.h>
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
#include <arch/object/vcpu.h>
#endif

bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
    pak_sa seL4_ARM_SmallPageObject:
        ត្រឡប់ true;
    pak_sa seL4_ARM_LargePageObject:
        ត្រឡប់ true;
    pak_sa seL4_ARM_SectionObject:
        ត្រឡប់ true;
    pak_sa seL4_ARM_SuperSectionObject:
        ត្រឡប់ true;
    default:
        ត្រឡប់ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_page_table_cap:
        ប្រសិនបើ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } បើមិនដូច្នេះ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ត្រឡប់ ret;

    pak_sa cap_page_directory_cap:
        ប្រសិនបើ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } បើមិនដូច្នេះ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ត្រឡប់ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    pak_sa cap_small_frame_cap:
        ret.cap = cap_small_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;

    pak_sa cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;

    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;
#endif

#ifdef CONFIG_TK1_SMMU
    pak_sa cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ត្រឡប់ ret;

    pak_sa cap_io_page_table_cap:
        ប្រសិនបើ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } បើមិនដូច្នេះ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ត្រឡប់ ret;
#endif
    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    ត្រឡប់ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ប្រសិនបើ (cap_get_capType(cap) == cap_small_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_small_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ត្រឡប់ cap_small_frame_cap_set_capFVMRights(cap,
                                                    wordFromVMRights(vm_rights));
    } បើមិនដូច្នេះ ប្រសិនបើ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(
                        cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ត្រឡប់ cap_frame_cap_set_capFVMRights(cap,
                                              wordFromVMRights(vm_rights));
    } បើមិនដូច្នេះ {
        ត្រឡប់ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_asid_pool_cap:
        ប្រសិនបើ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        bha_nga;

    pak_sa cap_page_directory_cap:
        ប្រសិនបើ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            deleteASID(cap_page_directory_cap_get_capPDMappedASID(cap),
                       PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        ប្រសិនបើ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_small_frame_cap:
        ប្រសិនបើ (cap_small_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_TK1_SMMU
            ប្រសិនបើ (isIOSpaceFrameCap(cap)) {
                unmapIOPage(cap);
                bha_nga;
            }
#endif

            unmapPage(ARMSmallPage,
                      cap_small_frame_cap_get_capFMappedASID(cap),
                      cap_small_frame_cap_get_capFMappedAddress(cap),
                      (void *)cap_small_frame_cap_get_capFBasePtr(cap));
        }
        bha_nga;

    pak_sa cap_frame_cap:
        ប្រសិនបើ (cap_frame_cap_get_capFMappedASID(cap)) {
#ifdef CONFIG_KERNEL_LOG_BUFFER
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ប្រសិនបើ (unlikely(cap_frame_cap_get_capFSize(cap) == ARMSection)) {
                ប្រសិនបើ (pptr_to_paddr((void *)cap_frame_cap_get_capFBasePtr(cap)) == ksUserLogBuffer) {
                    ksUserLogBuffer = 0;

                    /* Invalidate log page table entries */
                    clearMemory((void *) armKSGlobalLogPT, seL4_PageTableBits);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + BIT(seL4_PageTableBits),
                                        addrFromKPPtr((void *)&armKSGlobalLogPT[0]));

                    សម្រាប់ (ចំនួនគត់ idx = 0; idx < BIT(PT_INDEX_BITS); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << seL4_PageBits));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#endif /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      (void *)cap_frame_cap_get_capFBasePtr(cap));
        }
        bha_nga;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ប្រសិនបើ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        bha_nga;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        // do nothing
        bha_nga;
#endif


#ifdef CONFIG_TK1_SMMU
    pak_sa cap_io_space_cap:
        ប្រសិនបើ (final) {
            clearIOPageDirectory(cap);
        }
        bha_nga;

    pak_sa cap_io_page_table_cap:
        ប្រសិនបើ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        bha_nga;
#endif

    default:
        bha_nga;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ត្រឡប់ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    pak_sa cap_small_frame_cap:
    pak_sa cap_frame_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_small_frame_cap ||
            cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, topA, topB;
            botA = generic_frame_cap_get_capFBasePtr(cap_a);
            botB = generic_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(generic_frame_cap_get_capFSize(cap_b))) ;
            ត្រឡប់ ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ត្រឡប់ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_page_directory_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ត្រឡប់ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_asid_control_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ត្រឡប់ true;
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ត្រឡប់ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        bha_nga;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ត្រឡប់ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        bha_nga;
#endif

#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ត្រឡប់ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        bha_nga;
#endif


#ifdef CONFIG_TK1_SMMU
    pak_sa cap_io_space_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ត្រឡប់ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        bha_nga;

    pak_sa cap_io_page_table_cap:
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ត្រឡប់ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        bha_nga;
#endif
    }

    ត្រឡប់ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ប្រសិនបើ (cap_get_capType(cap_a) == cap_small_frame_cap) {
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ត្រឡប់ ((cap_small_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_small_frame_cap_get_capFBasePtr(cap_b)) &&
                    ((cap_small_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_small_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } បើមិនដូច្នេះ ប្រសិនបើ (cap_get_capType(cap_b) == cap_frame_cap) {
            ត្រឡប់ false;
        }
    }
    ប្រសិនបើ (cap_get_capType(cap_a) == cap_frame_cap) {
        ប្រសិនបើ (cap_get_capType(cap_b) == cap_frame_cap) {
            ត្រឡប់ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        } បើមិនដូច្នេះ ប្រសិនបើ (cap_get_capType(cap_b) == cap_small_frame_cap) {
            ត្រឡប់ false;
        }
    }
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ប្រសិនបើ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ត្រឡប់ false;
    }
#endif

    ត្រឡប់ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        ត្រឡប់ ARMSmallPageBits;
    pak_sa seL4_ARM_LargePageObject:
        ត្រឡប់ ARMLargePageBits;
    pak_sa seL4_ARM_SectionObject:
        ត្រឡប់ ARMSectionBits;
    pak_sa seL4_ARM_SuperSectionObject:
        ត្រឡប់ ARMSuperSectionBits;
    pak_sa seL4_ARM_PageTableObject:
        ត្រឡប់ PTE_SIZE_BITS + PT_INDEX_BITS;
    pak_sa seL4_ARM_PageDirectoryObject:
        ត្រឡប់ PDE_SIZE_BITS + PD_INDEX_BITS;
#ifdef CONFIG_TK1_SMMU
    pak_sa seL4_ARM_IOPageTableObject:
        ត្រឡប់ seL4_IOPageTableBits;
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_ARM_VCPUObject:
        ត្រឡប់ VCPU_SIZE_BITS;
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    default:
        fail("Invalid object type");
        ត្រឡប់ 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        ប្រសិនបើ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } បើមិនដូច្នេះ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ត្រឡប់ cap_small_frame_cap_new(
                   ASID_LOW(asidInvalid), VMReadWrite,
                   0, !!deviceMemory,
#ifdef CONFIG_TK1_SMMU
                   0,
#endif
                   ASID_HIGH(asidInvalid),
                   (word_t)regionBase);

    pak_sa seL4_ARM_LargePageObject:
        ប្រសិនបើ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } បើមិនដូច្នេះ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ត្រឡប់ cap_frame_cap_new(
                   ARMLargePage, ASID_LOW(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_HIGH(asidInvalid),
                   (word_t)regionBase);

    pak_sa seL4_ARM_SectionObject:
        ប្រសិនបើ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } បើមិនដូច្នេះ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ត្រឡប់ cap_frame_cap_new(
                   ARMSection, ASID_LOW(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_HIGH(asidInvalid),
                   (word_t)regionBase);

    pak_sa seL4_ARM_SuperSectionObject:
        ប្រសិនបើ (deviceMemory) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } បើមិនដូច្នេះ {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ត្រឡប់ cap_frame_cap_new(
                   ARMSuperSection, ASID_LOW(asidInvalid), VMReadWrite,
                   0, !!deviceMemory, ASID_HIGH(asidInvalid),
                   (word_t)regionBase);

    pak_sa seL4_ARM_PageTableObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ត្រឡប់ cap_page_table_cap_new(false, asidInvalid, 0,
                                      (word_t)regionBase);

    pak_sa seL4_ARM_PageDirectoryObject:
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#បើមិនដូច្នេះ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)regionBase);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageDirBits),
                            addrFromPPtr(regionBase));

        ត្រឡប់ cap_page_directory_cap_new(false, asidInvalid,
                                          (word_t)regionBase);
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ត្រឡប់ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

#ifdef CONFIG_TK1_SMMU
    pak_sa seL4_ARM_IOPageTableObject:
        cleanCacheRange_RAM((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_IOPageTableBits),
                            addrFromPPtr(regionBase));
        ត្រឡប់ cap_io_page_table_cap_new(0, asidInvalid, (word_t)regionBase, 0);
#endif
    default:
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
#ប្រសិនបើ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    switch (cap_get_capType(cap)) {
#ifdef CONFIG_TK1_SMMU
    pak_sa cap_io_space_cap:
        ត្រឡប់ decodeARMIOSpaceInvocation(invLabel, cap);
    pak_sa cap_io_page_table_cap:
        ត្រឡប់ decodeARMIOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ត្រឡប់ decodeARMVCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ត្រឡប់ decodeSGISignalInvocation(invLabel, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    default:
#បើមិនដូច្នេះ
{
#endif
    ត្រឡប់ decodeARMMMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
}
}

void
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ប្រសិនបើ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}

