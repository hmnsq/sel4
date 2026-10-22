/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <kernel/vspace.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/objecttype.h>

bool_t Arch_isFrameType(word_t type)
{
    بدل (type) {
    حالة seL4_ARM_SmallPageObject:
        أرجع true;
    حالة seL4_ARM_LargePageObject:
        أرجع true;
    حالة seL4_ARM_HugePageObject:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        𐤀𐤌 (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_page_table_cap:
        𐤀𐤌 (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
#endif

#ifdef CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
    حالة cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_sid_cap:
    حالة cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
#endif
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
#ifdef CONFIG_ALLOW_SMC_CALLS
    𐤀𐤌 (cap_get_capType(cap) == cap_smc_cap) {
        𐤀𐤌 (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            أرجع cap_smc_cap_set_capSMCBadge(cap, data);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    } وإلا {
#endif
        أرجع cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    𐤀𐤌 (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        أرجع cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_asid_pool_cap:
        𐤀𐤌 (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        اكسر;

    حالة cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        𐤀𐤌 (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        𐤀𐤌 (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        اكسر;

    حالة cap_page_table_cap:
        𐤀𐤌 (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        اكسر;

    حالة cap_frame_cap:
        𐤀𐤌 (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        اكسر;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        𐤀𐤌 (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#endif


#ifdef CONFIG_ARM_SMMU
    حالة cap_cb_cap:
        𐤀𐤌 (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        𐤀𐤌 (final) {
            smmu_delete_cb(cap);
        }
        اكسر;
    حالة cap_sid_cap:
        𐤀𐤌 (final) {
            smmu_delete_sid(cap);
        }
        اكسر;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_frame_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            أرجع ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        اكسر;

    حالة cap_page_table_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_page_table_cap) {
            أرجع cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        اكسر;

    حالة cap_vspace_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_vspace_cap) {
            أرجع cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        اكسر;

    حالة cap_asid_control_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_asid_control_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            أرجع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#endif

#ifdef CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            أرجع true;
        }
        اكسر;
    حالة cap_cb_control_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            أرجع true;
        }
        اكسر;
    حالة cap_sid_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_sid_cap) {
            أرجع cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        اكسر;
    حالة cap_cb_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_cb_cap) {
            أرجع cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        اكسر;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_smc_cap) {
            أرجع true;
        }
        اكسر;
#endif
    }
    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    𐤀𐤌 (cap_get_capType(cap_a) == cap_frame_cap) {
        𐤀𐤌 (cap_get_capType(cap_b) == cap_frame_cap) {
            أرجع ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    𐤀𐤌 (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        أرجع false;
    }
    𐤀𐤌 (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        أرجع false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    𐤀𐤌 (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#endif

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    بدل (t) {
    حالة seL4_ARM_SmallPageObject:
        أرجع ARMSmallPageBits;
    حالة seL4_ARM_LargePageObject:
        أرجع ARMLargePageBits;
    حالة seL4_ARM_HugePageObject:
        أرجع ARMHugePageBits;
    حالة seL4_ARM_PageTableObject:
        أرجع seL4_PageTableBits;
    حالة seL4_ARM_VSpaceObject:
        أرجع seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_SIZE_BITS;
#endif
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *regionBase, word_t userSize, bool_t deviceMemory)
{
    بدل (t) {
    حالة seL4_ARM_SmallPageObject:
        𐤀𐤌 (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        أرجع cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    حالة seL4_ARM_LargePageObject:
        𐤀𐤌 (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        أرجع cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    حالة seL4_ARM_HugePageObject:
        𐤀𐤌 (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        أرجع cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    حالة seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#وإلا
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    حالة seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        أرجع cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        أرجع cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    افتراضي:
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
#𐤀𐤌 defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
        أرجع decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    حالة cap_sid_cap:
        أرجع decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    حالة cap_cb_control_cap:
        أرجع decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    حالة cap_cb_cap:
        أرجع decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        أرجع decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    افتراضي:
#وإلا
{
#endif
    أرجع decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    𐤀𐤌 (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
