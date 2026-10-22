/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    தேர்வு (type) {
    நிலை seL4_ARM_SmallPageObject:
        திரும்பு true;
    நிலை seL4_ARM_LargePageObject:
        திரும்பு true;
    நிலை seL4_ARM_HugePageObject:
        திரும்பு true;
    இயல்பு:
        திரும்பு false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_vspace_cap:
        எனில் (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_page_table_cap:
        எனில் (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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

#ifdef CONFIG_ARM_SMMU
    நிலை cap_sid_control_cap:
    நிலை cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
    நிலை cap_sid_cap:
    நிலை cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    நிலை cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_ALLOW_SMC_CALLS
    எனில் (cap_get_capType(cap) == cap_smc_cap) {
        எனில் (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            திரும்பு cap_smc_cap_set_capSMCBadge(cap, data);
        } இல்லையெனில் {
            திரும்பு cap_null_cap_new();
        }
    } இல்லையெனில் {
#endif
        திரும்பு cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    எனில் (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        திரும்பு cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
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

    நிலை cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        எனில் (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        எனில் (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        நிறுத்து;

    நிலை cap_frame_cap:
        எனில் (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_ARM_SMMU
    நிலை cap_cb_cap:
        எனில் (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        எனில் (final) {
            smmu_delete_cb(cap);
        }
        நிறுத்து;
    நிலை cap_sid_cap:
        எனில் (final) {
            smmu_delete_sid(cap);
        }
        நிறுத்து;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    திரும்பு fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    தேர்வு (cap_get_capType(cap_a)) {
    நிலை cap_frame_cap:
        எனில் (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, உச்சி_A, உச்சி_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            உச்சி_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            உச்சி_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            திரும்பு ((botA <= botB) && (உச்சி_A >= உச்சி_B) && (botB <= உச்சி_B));
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (cap_get_capType(cap_b) == cap_page_table_cap) {
            திரும்பு cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_vspace_cap:
        எனில் (cap_get_capType(cap_b) == cap_vspace_cap) {
            திரும்பு cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#ifdef CONFIG_ARM_SMMU
    நிலை cap_sid_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            திரும்பு true;
        }
        நிறுத்து;
    நிலை cap_cb_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            திரும்பு true;
        }
        நிறுத்து;
    நிலை cap_sid_cap:
        எனில் (cap_get_capType(cap_b) == cap_sid_cap) {
            திரும்பு cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        நிறுத்து;
    நிலை cap_cb_cap:
        எனில் (cap_get_capType(cap_b) == cap_cb_cap) {
            திரும்பு cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        நிறுத்து;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    நிலை cap_smc_cap:
        எனில் (cap_get_capType(cap_b) == cap_smc_cap) {
            திரும்பு true;
        }
        நிறுத்து;
#endif
    }
    திரும்பு false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    எனில் (cap_get_capType(cap_a) == cap_frame_cap) {
        எனில் (cap_get_capType(cap_b) == cap_frame_cap) {
            திரும்பு ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    எனில் (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        திரும்பு false;
    }
    எனில் (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        திரும்பு false;
    }
#endif
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
    நிலை seL4_ARM_HugePageObject:
        திரும்பு ARMHugePageBits;
    நிலை seL4_ARM_PageTableObject:
        திரும்பு seL4_PageTableBits;
    நிலை seL4_ARM_VSpaceObject:
        திரும்பு seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_ARM_VCPUObject:
        திரும்பு VCPU_SIZE_BITS;
#endif
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
        திரும்பு cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    நிலை seL4_ARM_LargePageObject:
        எனில் (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } இல்லையெனில் {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        திரும்பு cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    நிலை seL4_ARM_HugePageObject:
        எனில் (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } இல்லையெனில் {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        திரும்பு cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    நிலை seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        திரும்பு cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#இல்லையெனில்
        திரும்பு cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    நிலை seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        திரும்பு cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        திரும்பு cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    இயல்பு:
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
#எனில் defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    தேர்வு (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    நிலை cap_vcpu_cap:
        திரும்பு decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    நிலை cap_sgi_signal_cap:
        திரும்பு decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    நிலை cap_sid_control_cap:
        திரும்பு decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    நிலை cap_sid_cap:
        திரும்பு decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    நிலை cap_cb_control_cap:
        திரும்பு decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    நிலை cap_cb_cap:
        திரும்பு decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    நிலை cap_smc_cap:
        திரும்பு decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    இயல்பு:
#இல்லையெனில்
{
#endif
    திரும்பு decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
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
