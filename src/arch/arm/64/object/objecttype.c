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
    निवड (type) {
    स्थिती seL4_ARM_SmallPageObject:
        परत true;
    स्थिती seL4_ARM_LargePageObject:
        परत true;
    स्थिती seL4_ARM_HugePageObject:
        परत true;
    पूर्वनिर्धारित:
        परत false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_vspace_cap:
        जर (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_page_table_cap:
        जर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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

#ifdef CONFIG_ARM_SMMU
    स्थिती cap_sid_control_cap:
    स्थिती cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        परत ret;
    स्थिती cap_sid_cap:
    स्थिती cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    स्थिती cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_ALLOW_SMC_CALLS
    जर (cap_get_capType(cap) == cap_smc_cap) {
        जर (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            परत cap_smc_cap_set_capSMCBadge(cap, data);
        } अन्यथा {
            परत cap_null_cap_new();
        }
    } अन्यथा {
#endif
        परत cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    जर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        परत cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
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

    स्थिती cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        जर (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        जर (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        थांबा;

    स्थिती cap_frame_cap:
        जर (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_ARM_SMMU
    स्थिती cap_cb_cap:
        जर (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        जर (final) {
            smmu_delete_cb(cap);
        }
        थांबा;
    स्थिती cap_sid_cap:
        जर (final) {
            smmu_delete_sid(cap);
        }
        थांबा;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    परत fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    निवड (cap_get_capType(cap_a)) {
    स्थिती cap_frame_cap:
        जर (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            परत ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (cap_get_capType(cap_b) == cap_page_table_cap) {
            परत cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_vspace_cap:
        जर (cap_get_capType(cap_b) == cap_vspace_cap) {
            परत cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#ifdef CONFIG_ARM_SMMU
    स्थिती cap_sid_control_cap:
        जर (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            परत true;
        }
        थांबा;
    स्थिती cap_cb_control_cap:
        जर (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            परत true;
        }
        थांबा;
    स्थिती cap_sid_cap:
        जर (cap_get_capType(cap_b) == cap_sid_cap) {
            परत cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        थांबा;
    स्थिती cap_cb_cap:
        जर (cap_get_capType(cap_b) == cap_cb_cap) {
            परत cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        थांबा;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    स्थिती cap_smc_cap:
        जर (cap_get_capType(cap_b) == cap_smc_cap) {
            परत true;
        }
        थांबा;
#endif
    }
    परत false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जर (cap_get_capType(cap_a) == cap_frame_cap) {
        जर (cap_get_capType(cap_b) == cap_frame_cap) {
            परत ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    जर (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        परत false;
    }
    जर (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        परत false;
    }
#endif
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
    स्थिती seL4_ARM_HugePageObject:
        परत ARMHugePageBits;
    स्थिती seL4_ARM_PageTableObject:
        परत seL4_PageTableBits;
    स्थिती seL4_ARM_VSpaceObject:
        परत seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती seL4_ARM_VCPUObject:
        परत VCPU_SIZE_BITS;
#endif
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
        परत cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    स्थिती seL4_ARM_LargePageObject:
        जर (deviceMemory) {
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        परत cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    स्थिती seL4_ARM_HugePageObject:
        जर (deviceMemory) {
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
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        परत cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    स्थिती seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        परत cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#अन्यथा
        परत cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    स्थिती seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        परत cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        परत cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    पूर्वनिर्धारित:
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
#जर defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    निवड (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    स्थिती cap_vcpu_cap:
        परत decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    स्थिती cap_sgi_signal_cap:
        परत decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    स्थिती cap_sid_control_cap:
        परत decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    स्थिती cap_sid_cap:
        परत decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    स्थिती cap_cb_control_cap:
        परत decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    स्थिती cap_cb_cap:
        परत decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    स्थिती cap_smc_cap:
        परत decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    पूर्वनिर्धारित:
#अन्यथा
{
#endif
    परत decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
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
