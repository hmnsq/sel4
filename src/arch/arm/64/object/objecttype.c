/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    ବାଛ (type) {
    ସ୍ଥିତି seL4_ARM_SmallPageObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_ARM_LargePageObject:
        ଫେରନ୍ତୁ true;
    ସ୍ଥିତି seL4_ARM_HugePageObject:
        ଫେରନ୍ତୁ true;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_vspace_cap:
        ଯଦି (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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

#ifdef CONFIG_ARM_SMMU
    ସ୍ଥିତି cap_sid_control_cap:
    ସ୍ଥିତି cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_sid_cap:
    ସ୍ଥିତି cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ସ୍ଥିତି cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_ALLOW_SMC_CALLS
    ଯଦି (cap_get_capType(cap) == cap_smc_cap) {
        ଯଦି (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ଫେରନ୍ତୁ cap_smc_cap_set_capSMCBadge(cap, data);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ cap_null_cap_new();
        }
    } ଅନ୍ୟଥା {
#endif
        ଫେରନ୍ତୁ cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ଯଦି (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        ଫେରନ୍ତୁ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
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

    ସ୍ଥିତି cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        ଯଦି (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        ଯଦି (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_ARM_SMMU
    ସ୍ଥିତି cap_cb_cap:
        ଯଦି (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ଯଦି (final) {
            smmu_delete_cb(cap);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_sid_cap:
        ଯଦି (final) {
            smmu_delete_sid(cap);
        }
        ଭାଙ୍ଗ;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {
    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, ଶୀର୍ଷ_A, ଶୀର୍ଷ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ଶୀର୍ଷ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ଶୀର୍ଷ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ଫେରନ୍ତୁ ((botA <= botB) && (ଶୀର୍ଷ_A >= ଶୀର୍ଷ_B) && (botB <= ଶୀର୍ଷ_B));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_table_cap) {
            ଫେରନ୍ତୁ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_vspace_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_vspace_cap) {
            ଫେରନ୍ତୁ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#ifdef CONFIG_ARM_SMMU
    ସ୍ଥିତି cap_sid_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_cb_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_sid_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_sid_cap) {
            ଫେରନ୍ତୁ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_cb_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_cb_cap) {
            ଫେରନ୍ତୁ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        ଭାଙ୍ଗ;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    ସ୍ଥିତି cap_smc_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_smc_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;
#endif
    }
    ଫେରନ୍ତୁ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ଯଦି (cap_get_capType(cap_a) == cap_frame_cap) {
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            ଫେରନ୍ତୁ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    ଯଦି (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ଫେରନ୍ତୁ false;
    }
#endif
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
    ସ୍ଥିତି seL4_ARM_HugePageObject:
        ଫେରନ୍ତୁ ARMHugePageBits;
    ସ୍ଥିତି seL4_ARM_PageTableObject:
        ଫେରନ୍ତୁ seL4_PageTableBits;
    ସ୍ଥିତି seL4_ARM_VSpaceObject:
        ଫେରନ୍ତୁ seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_ARM_VCPUObject:
        ଫେରନ୍ତୁ VCPU_SIZE_BITS;
#endif
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
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ସ୍ଥିତି seL4_ARM_LargePageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    ସ୍ଥିତି seL4_ARM_HugePageObject:
        ଯଦି (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ଅନ୍ୟଥା {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        ଫେରନ୍ତୁ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    ସ୍ଥିତି seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        ଫେରନ୍ତୁ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ଅନ୍ୟଥା
        ଫେରନ୍ତୁ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    ସ୍ଥିତି seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ଫେରନ୍ତୁ cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ଫେରନ୍ତୁ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
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
#ଯଦି defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ବାଛ (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ସ୍ଥିତି cap_vcpu_cap:
        ଫେରନ୍ତୁ decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ସ୍ଥିତି cap_sgi_signal_cap:
        ଫେରନ୍ତୁ decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    ସ୍ଥିତି cap_sid_control_cap:
        ଫେରନ୍ତୁ decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    ସ୍ଥିତି cap_sid_cap:
        ଫେରନ୍ତୁ decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    ସ୍ଥିତି cap_cb_control_cap:
        ଫେରନ୍ତୁ decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    ସ୍ଥିତି cap_cb_cap:
        ଫେରନ୍ତୁ decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    ସ୍ଥିତି cap_smc_cap:
        ଫେରନ୍ତୁ decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#ଅନ୍ୟଥା
{
#endif
    ଫେରନ୍ତୁ decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
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
