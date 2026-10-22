/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    ವಿಕಲ್ಪ (type) {
    ಪಕ್ಷ seL4_ARM_SmallPageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_ARM_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಪಕ್ಷ seL4_ARM_HugePageObject:
        ಪ್ರತ್ಯಾಗಮ true;
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_vspace_cap:
        ಯದಿ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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

#ifdef CONFIG_ARM_SMMU
    ಪಕ್ಷ cap_sid_control_cap:
    ಪಕ್ಷ cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_sid_cap:
    ಪಕ್ಷ cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ಪಕ್ಷ cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_ALLOW_SMC_CALLS
    ಯದಿ (cap_get_capType(cap) == cap_smc_cap) {
        ಯದಿ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ಪ್ರತ್ಯಾಗಮ cap_smc_cap_set_capSMCBadge(cap, data);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    } ಅನ್ಯಥಾ {
#endif
        ಪ್ರತ್ಯಾಗಮ cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
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

    ಪಕ್ಷ cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        ಯದಿ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        ಯದಿ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_ARM_SMMU
    ಪಕ್ಷ cap_cb_cap:
        ಯದಿ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ಯದಿ (final) {
            smmu_delete_cb(cap);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_sid_cap:
        ಯದಿ (final) {
            smmu_delete_sid(cap);
        }
        ಭಙ್ಗ;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {
    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ತುದಿ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ತುದಿ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ಪ್ರತ್ಯಾಗಮ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_vspace_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_vspace_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#ifdef CONFIG_ARM_SMMU
    ಪಕ್ಷ cap_sid_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_cb_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_sid_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_sid_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_cb_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_cb_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        ಭಙ್ಗ;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ಪಕ್ಷ cap_smc_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_smc_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;
#endif
    }
    ಪ್ರತ್ಯಾಗಮ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಯದಿ (cap_get_capType(cap_a) == cap_frame_cap) {
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    ಯದಿ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
#endif
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
    ಪಕ್ಷ seL4_ARM_HugePageObject:
        ಪ್ರತ್ಯಾಗಮ ARMHugePageBits;
    ಪಕ್ಷ seL4_ARM_PageTableObject:
        ಪ್ರತ್ಯಾಗಮ seL4_PageTableBits;
    ಪಕ್ಷ seL4_ARM_VSpaceObject:
        ಪ್ರತ್ಯಾಗಮ seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ seL4_ARM_VCPUObject:
        ಪ್ರತ್ಯಾಗಮ VCPU_SIZE_BITS;
#endif
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
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ಪಕ್ಷ seL4_ARM_LargePageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ಪಕ್ಷ seL4_ARM_HugePageObject:
        ಯದಿ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ಅನ್ಯಥಾ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    ಪಕ್ಷ seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        ಪ್ರತ್ಯಾಗಮ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    ಪಕ್ಷ seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ಪ್ರತ್ಯಾಗಮ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    ಸ್ವಭಾವ:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t label, word_t length, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *buffer)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ಯದಿ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ಪಕ್ಷ cap_vcpu_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ಪಕ್ಷ cap_sgi_signal_cap:
        ಪ್ರತ್ಯಾಗಮ decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    ಪಕ್ಷ cap_sid_control_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    ಪಕ್ಷ cap_sid_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    ಪಕ್ಷ cap_cb_control_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    ಪಕ್ಷ cap_cb_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    ಪಕ್ಷ cap_smc_cap:
        ಪ್ರತ್ಯಾಗಮ decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    ಸ್ವಭಾವ:
#ಅನ್ಯಥಾ
{
#endif
    ಪ್ರತ್ಯಾಗಮ decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
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
