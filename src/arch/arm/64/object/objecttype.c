/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    विकल्प (type) {
    पक्ष seL4_ARM_SmallPageObject:
        पटिनिवत्त true;
    पक्ष seL4_ARM_LargePageObject:
        पटिनिवत्त true;
    पक्ष seL4_ARM_HugePageObject:
        पटिनिवत्त true;
    स्वभाव:
        पटिनिवत्त false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_vspace_cap:
        सचे (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अञ्ञथा {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        पटिनिवत्त ret;

    पक्ष cap_page_table_cap:
        सचे (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अञ्ञथा {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        पटिनिवत्त ret;

    पक्ष cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif

#ifdef CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
    पक्ष cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
    पक्ष cap_sid_cap:
    पक्ष cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        पटिनिवत्त ret;
#endif
    स्वभाव:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
#ifdef CONFIG_ALLOW_SMC_CALLS
    सचे (cap_get_capType(cap) == cap_smc_cap) {
        सचे (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            पटिनिवत्त cap_smc_cap_set_capSMCBadge(cap, data);
        } अञ्ञथा {
            पटिनिवत्त cap_null_cap_new();
        }
    } अञ्ञथा {
#endif
        पटिनिवत्त cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    सचे (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        पटिनिवत्त cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अञ्ञथा {
        पटिनिवत्त cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_asid_pool_cap:
        सचे (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        भङ्ग;

    पक्ष cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        सचे (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        सचे (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        सचे (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        भङ्ग;

    पक्ष cap_frame_cap:
        सचे (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        भङ्ग;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        सचे (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        भङ्ग;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        // do nothing
        भङ्ग;
#endif


#ifdef CONFIG_ARM_SMMU
    पक्ष cap_cb_cap:
        सचे (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        सचे (final) {
            smmu_delete_cb(cap);
        }
        भङ्ग;
    पक्ष cap_sid_cap:
        सचे (final) {
            smmu_delete_sid(cap);
        }
        भङ्ग;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    पटिनिवत्त fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_frame_cap:
        सचे (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            पटिनिवत्त ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        सचे (cap_get_capType(cap_b) == cap_page_table_cap) {
            पटिनिवत्त cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_vspace_cap:
        सचे (cap_get_capType(cap_b) == cap_vspace_cap) {
            पटिनिवत्त cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_asid_control_cap:
        सचे (cap_get_capType(cap_b) == cap_asid_control_cap) {
            पटिनिवत्त true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        सचे (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            पटिनिवत्त cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        सचे (cap_get_capType(cap_b) == cap_vcpu_cap) {
            पटिनिवत्त cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        भङ्ग;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        सचे (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            पटिनिवत्त (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        भङ्ग;
#endif

#ifdef CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
        सचे (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            पटिनिवत्त true;
        }
        भङ्ग;
    पक्ष cap_cb_control_cap:
        सचे (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            पटिनिवत्त true;
        }
        भङ्ग;
    पक्ष cap_sid_cap:
        सचे (cap_get_capType(cap_b) == cap_sid_cap) {
            पटिनिवत्त cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        भङ्ग;
    पक्ष cap_cb_cap:
        सचे (cap_get_capType(cap_b) == cap_cb_cap) {
            पटिनिवत्त cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        भङ्ग;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        सचे (cap_get_capType(cap_b) == cap_smc_cap) {
            पटिनिवत्त true;
        }
        भङ्ग;
#endif
    }
    पटिनिवत्त false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    सचे (cap_get_capType(cap_a) == cap_frame_cap) {
        सचे (cap_get_capType(cap_b) == cap_frame_cap) {
            पटिनिवत्त ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    सचे (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        पटिनिवत्त false;
    }
    सचे (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        पटिनिवत्त false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    सचे (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        पटिनिवत्त false;
    }
#endif

    पटिनिवत्त Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        पटिनिवत्त ARMSmallPageBits;
    पक्ष seL4_ARM_LargePageObject:
        पटिनिवत्त ARMLargePageBits;
    पक्ष seL4_ARM_HugePageObject:
        पटिनिवत्त ARMHugePageBits;
    पक्ष seL4_ARM_PageTableObject:
        पटिनिवत्त seL4_PageTableBits;
    पक्ष seL4_ARM_VSpaceObject:
        पटिनिवत्त seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        पटिनिवत्त VCPU_SIZE_BITS;
#endif
    स्वभाव:
        fail("Invalid object type");
        पटिनिवत्त 0;
    }
}

cap_t Arch_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
    विकल्प (t) {
    पक्ष seL4_ARM_SmallPageObject:
        सचे (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } अञ्ञथा {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    पक्ष seL4_ARM_LargePageObject:
        सचे (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } अञ्ञथा {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    पक्ष seL4_ARM_HugePageObject:
        सचे (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } अञ्ञथा {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        पटिनिवत्त cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    पक्ष seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        पटिनिवत्त cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#अञ्ञथा
        पटिनिवत्त cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    पक्ष seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        पटिनिवत्त cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        पटिनिवत्त cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    स्वभाव:
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
#सचे defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    विकल्प (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    पक्ष cap_vcpu_cap:
        पटिनिवत्त decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    पक्ष cap_sgi_signal_cap:
        पटिनिवत्त decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    पक्ष cap_sid_control_cap:
        पटिनिवत्त decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    पक्ष cap_sid_cap:
        पटिनिवत्त decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    पक्ष cap_cb_control_cap:
        पटिनिवत्त decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    पक्ष cap_cb_cap:
        पटिनिवत्त decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    पक्ष cap_smc_cap:
        पटिनिवत्त decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    स्वभाव:
#अञ्ञथा
{
#endif
    पटिनिवत्त decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}
}

शून्य
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    सचे (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
