/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <kernel/vspace.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    चयन (type) {
    हाल seL4_ARM_SmallPageObject:
        वापस true;
    हाल seL4_ARM_LargePageObject:
        वापस true;
    हाल seL4_ARM_HugePageObject:
        वापस true;
    शेष:
        वापस false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_vspace_cap:
        अगर (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_page_table_cap:
        अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        वापस ret;

    हाल cap_asid_control_cap:
    हाल cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_SMMU
    हाल cap_sid_control_cap:
    हाल cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        वापस ret;
    हाल cap_sid_cap:
    हाल cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    हाल cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
#पूर्ण_अगर
    शेष:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_तथ्य(bool_t preserve, word_t तथ्य, cap_t cap)
{
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    अगर (cap_get_capType(cap) == cap_smc_cap) {
        अगर (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            वापस cap_smc_cap_set_capSMCBadge(cap, तथ्य);
        } अन्यथा {
            वापस cap_null_cap_new();
        }
    } अन्यथा {
#पूर्ण_अगर
        वापस cap;
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    }
#पूर्ण_अगर
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    अगर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        वापस cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        वापस cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_asid_pool_cap:
        अगर (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        अवरोध;

    हाल cap_vspace_cap:
#अगर_घोषित CONFIG_ARM_SMMU
        अगर (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#पूर्ण_अगर
        अगर (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        अवरोध;

    हाल cap_frame_cap:
        अगर (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        अवरोध;
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        अगर (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        // do nothing
        अवरोध;
#पूर्ण_अगर


#अगर_घोषित CONFIG_ARM_SMMU
    हाल cap_cb_cap:
        अगर (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        अगर (final) {
            smmu_delete_cb(cap);
        }
        अवरोध;
    हाल cap_sid_cap:
        अगर (final) {
            smmu_delete_sid(cap);
        }
        अवरोध;
#पूर्ण_अगर
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    चयन (cap_get_capType(cap_a)) {
    हाल cap_frame_cap:
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_b))) ;
            वापस ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_page_table_cap) {
            वापस cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_vspace_cap:
        अगर (cap_get_capType(cap_b) == cap_vspace_cap) {
            वापस cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_asid_control_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_asid_pool_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            वापस cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        अवरोध;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        अगर (cap_get_capType(cap_b) == cap_vcpu_cap) {
            वापस cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        अवरोध;
#पूर्ण_अगर
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        अगर (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            वापस (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        अवरोध;
#पूर्ण_अगर

#अगर_घोषित CONFIG_ARM_SMMU
    हाल cap_sid_control_cap:
        अगर (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            वापस true;
        }
        अवरोध;
    हाल cap_cb_control_cap:
        अगर (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            वापस true;
        }
        अवरोध;
    हाल cap_sid_cap:
        अगर (cap_get_capType(cap_b) == cap_sid_cap) {
            वापस cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        अवरोध;
    हाल cap_cb_cap:
        अगर (cap_get_capType(cap_b) == cap_cb_cap) {
            वापस cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        अवरोध;
#पूर्ण_अगर
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    हाल cap_smc_cap:
        अगर (cap_get_capType(cap_b) == cap_smc_cap) {
            वापस true;
        }
        अवरोध;
#पूर्ण_अगर
    }
    वापस false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    अगर (cap_get_capType(cap_a) == cap_frame_cap) {
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            वापस ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#अगर_घोषित CONFIG_ARM_SMMU
    अगर (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        वापस false;
    }
    अगर (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        वापस false;
    }
#पूर्ण_अगर
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    अगर (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        वापस false;
    }
#पूर्ण_अगर

    वापस Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    चयन (t) {
    हाल seL4_ARM_SmallPageObject:
        वापस ARMSmallPageBits;
    हाल seL4_ARM_LargePageObject:
        वापस ARMLargePageBits;
    हाल seL4_ARM_HugePageObject:
        वापस ARMHugePageBits;
    हाल seL4_ARM_PageTableObject:
        वापस seL4_PageTableBits;
    हाल seL4_ARM_VSpaceObject:
        वापस seL4_VSpaceBits;
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_ARM_VCPUObject:
        वापस VCPU_आकार_BITS;
#पूर्ण_अगर
    शेष:
        fail("Invalid object type");
        वापस 0;
    }
}

cap_t Arch_createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
    चयन (t) {
    हाल seL4_ARM_SmallPageObject:
        अगर (deviceMemory) {
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
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    हाल seL4_ARM_LargePageObject:
        अगर (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    हाल seL4_ARM_HugePageObject:
        अगर (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } अन्यथा {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(page_Bits_For_आकार(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        वापस cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    हाल seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#अगर_घोषित CONFIG_ARM_SMMU
        वापस cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#अन्यथा
        वापस cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#पूर्ण_अगर /*!CONFIG_ARM_SMMU*/
    हाल seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        वापस cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        वापस cap_vcpu_cap_new(VCPU_REF(regionBase));
#पूर्ण_अगर

    शेष:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(word_t label, word_t लंबाई, cptr_t cptr,
                                  cte_t *slot, cap_t cap,
                                  bool_t call, word_t *बफर)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#अगर defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    चयन (cap_get_capType(cap)) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    हाल cap_vcpu_cap:
        वापस decodeARMVCPUInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
#पूर्ण_अगर /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#अगर_अघोषित CONFIG_ENABLE_SMP_SUPPORT
    हाल cap_sgi_signal_cap:
        वापस decodeSGISignalInvocation(label, लंबाई, cap, बफर);
#पूर्ण_अगर /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#अगर_घोषित CONFIG_ARM_SMMU
    हाल cap_sid_control_cap:
        वापस decodeARMSIDControlInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
    हाल cap_sid_cap:
        वापस decodeARMSIDInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
    हाल cap_cb_control_cap:
        वापस decodeARMCBControlInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
    हाल cap_cb_cap:
        वापस decodeARMCBInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
#पूर्ण_अगर /*CONFIG_ARM_SMMU*/
#अगर_घोषित CONFIG_ALLOW_SMC_CALLS
    हाल cap_smc_cap:
        वापस decodeARMSMCInvocation(label, लंबाई, cap, call, बफर);
#पूर्ण_अगर
    शेष:
#अन्यथा
{
#पूर्ण_अगर
    वापस decodeARMMMUInvocation(label, लंबाई, cptr, slot, cap, call, बफर);
}
}

व्योम
Arch_prepareThreadDelete(tcb_t * thread) {
#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    अगर (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#पूर्ण_अगर /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#अगर_घोषित CONFIG_HAVE_FPU
    fpuRelease(thread);
#पूर्ण_अगर
}
