/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    തിരഞ്ഞെടുപ്പ് (type) {
    അവസ്ഥ seL4_ARM_SmallPageObject:
        തിരികെ true;
    അവസ്ഥ seL4_ARM_LargePageObject:
        തിരികെ true;
    അവസ്ഥ seL4_ARM_HugePageObject:
        തിരികെ true;
    സ്ഥിരസ്ഥിതി:
        തിരികെ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_vspace_cap:
        എങ്കിൽ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
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

#ifdef CONFIG_ARM_SMMU
    അവസ്ഥ cap_sid_control_cap:
    അവസ്ഥ cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
    അവസ്ഥ cap_sid_cap:
    അവസ്ഥ cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    അവസ്ഥ cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
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
#ifdef CONFIG_ALLOW_SMC_CALLS
    എങ്കിൽ (cap_get_capType(cap) == cap_smc_cap) {
        എങ്കിൽ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            തിരികെ cap_smc_cap_set_capSMCBadge(cap, data);
        } അല്ലെങ്കിൽ {
            തിരികെ cap_null_cap_new();
        }
    } അല്ലെങ്കിൽ {
#endif
        തിരികെ cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        തിരികെ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
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

    അവസ്ഥ cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        എങ്കിൽ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        എങ്കിൽ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        നിർത്തുക;

    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
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


#ifdef CONFIG_ARM_SMMU
    അവസ്ഥ cap_cb_cap:
        എങ്കിൽ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        എങ്കിൽ (final) {
            smmu_delete_cb(cap);
        }
        നിർത്തുക;
    അവസ്ഥ cap_sid_cap:
        എങ്കിൽ (final) {
            smmu_delete_sid(cap);
        }
        നിർത്തുക;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap_a)) {
    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, മുകൾ_A, മുകൾ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            മുകൾ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            മുകൾ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            തിരികെ ((botA <= botB) && (മുകൾ_A >= മുകൾ_B) && (botB <= മുകൾ_B));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_table_cap) {
            തിരികെ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_vspace_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_vspace_cap) {
            തിരികെ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#ifdef CONFIG_ARM_SMMU
    അവസ്ഥ cap_sid_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            തിരികെ true;
        }
        നിർത്തുക;
    അവസ്ഥ cap_cb_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            തിരികെ true;
        }
        നിർത്തുക;
    അവസ്ഥ cap_sid_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_sid_cap) {
            തിരികെ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        നിർത്തുക;
    അവസ്ഥ cap_cb_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_cb_cap) {
            തിരികെ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        നിർത്തുക;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    അവസ്ഥ cap_smc_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_smc_cap) {
            തിരികെ true;
        }
        നിർത്തുക;
#endif
    }
    തിരികെ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    എങ്കിൽ (cap_get_capType(cap_a) == cap_frame_cap) {
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            തിരികെ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    എങ്കിൽ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        തിരികെ false;
    }
    എങ്കിൽ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        തിരികെ false;
    }
#endif
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
    അവസ്ഥ seL4_ARM_HugePageObject:
        തിരികെ ARMHugePageBits;
    അവസ്ഥ seL4_ARM_PageTableObject:
        തിരികെ seL4_PageTableBits;
    അവസ്ഥ seL4_ARM_VSpaceObject:
        തിരികെ seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_ARM_VCPUObject:
        തിരികെ VCPU_SIZE_BITS;
#endif
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
        തിരികെ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    അവസ്ഥ seL4_ARM_LargePageObject:
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        തിരികെ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    അവസ്ഥ seL4_ARM_HugePageObject:
        എങ്കിൽ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } അല്ലെങ്കിൽ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        തിരികെ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    അവസ്ഥ seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        തിരികെ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#അല്ലെങ്കിൽ
        തിരികെ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    അവസ്ഥ seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        തിരികെ cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        തിരികെ cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    സ്ഥിരസ്ഥിതി:
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
#എങ്കിൽ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    അവസ്ഥ cap_vcpu_cap:
        തിരികെ decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    അവസ്ഥ cap_sgi_signal_cap:
        തിരികെ decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    അവസ്ഥ cap_sid_control_cap:
        തിരികെ decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    അവസ്ഥ cap_sid_cap:
        തിരികെ decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    അവസ്ഥ cap_cb_control_cap:
        തിരികെ decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    അവസ്ഥ cap_cb_cap:
        തിരികെ decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    അവസ്ഥ cap_smc_cap:
        തിരികെ decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    സ്ഥിരസ്ഥിതി:
#അല്ലെങ്കിൽ
{
#endif
    തിരികെ decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
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
