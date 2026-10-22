/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_isFrameType(word_t type)
{
    switch (type) {
    pak_sa seL4_ARM_SmallPageObject:
        ཕྱིར true;
    pak_sa seL4_ARM_LargePageObject:
        ཕྱིར true;
    pak_sa seL4_ARM_HugePageObject:
        ཕྱིར true;
    default:
        ཕྱིར false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_vspace_cap:
        གལ་ཏེ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } དེ་མིན {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ཕྱིར ret;

    pak_sa cap_page_table_cap:
        གལ་ཏེ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } དེ་མིན {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ཕྱིར ret;

    pak_sa cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;

    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;
#endif

#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
    pak_sa cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;
    pak_sa cap_sid_cap:
    pak_sa cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ཕྱིར ret;
#endif
    default:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
#ifdef CONFIG_ALLOW_SMC_CALLS
    གལ་ཏེ (cap_get_capType(cap) == cap_smc_cap) {
        གལ་ཏེ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ཕྱིར cap_smc_cap_set_capSMCBadge(cap, data);
        } དེ་མིན {
            ཕྱིར cap_null_cap_new();
        }
    } དེ་མིན {
#endif
        ཕྱིར cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    གལ་ཏེ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        ཕྱིར cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } དེ་མིན {
        ཕྱིར cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_asid_pool_cap:
        གལ་ཏེ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        bha_nga;

    pak_sa cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        གལ་ཏེ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        གལ་ཏེ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        གལ་ཏེ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_frame_cap:
        གལ་ཏེ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        bha_nga;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        གལ་ཏེ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        bha_nga;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        // do nothing
        bha_nga;
#endif


#ifdef CONFIG_ARM_SMMU
    pak_sa cap_cb_cap:
        གལ་ཏེ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        གལ་ཏེ (final) {
            smmu_delete_cb(cap);
        }
        bha_nga;
    pak_sa cap_sid_cap:
        གལ་ཏེ (final) {
            smmu_delete_sid(cap);
        }
        bha_nga;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ཕྱིར fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    pak_sa cap_frame_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ཕྱིར ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ཕྱིར cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_vspace_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_vspace_cap) {
            ཕྱིར cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_asid_control_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ཕྱིར true;
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ཕྱིར cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        bha_nga;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ཕྱིར cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        bha_nga;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ཕྱིར (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        bha_nga;
#endif

#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ཕྱིར true;
        }
        bha_nga;
    pak_sa cap_cb_control_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ཕྱིར true;
        }
        bha_nga;
    pak_sa cap_sid_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_sid_cap) {
            ཕྱིར cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        bha_nga;
    pak_sa cap_cb_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_cb_cap) {
            ཕྱིར cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        bha_nga;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        གལ་ཏེ (cap_get_capType(cap_b) == cap_smc_cap) {
            ཕྱིར true;
        }
        bha_nga;
#endif
    }
    ཕྱིར false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    གལ་ཏེ (cap_get_capType(cap_a) == cap_frame_cap) {
        གལ་ཏེ (cap_get_capType(cap_b) == cap_frame_cap) {
            ཕྱིར ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    གལ་ཏེ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ཕྱིར false;
    }
    གལ་ཏེ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ཕྱིར false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    གལ་ཏེ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ཕྱིར false;
    }
#endif

    ཕྱིར Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        ཕྱིར ARMSmallPageBits;
    pak_sa seL4_ARM_LargePageObject:
        ཕྱིར ARMLargePageBits;
    pak_sa seL4_ARM_HugePageObject:
        ཕྱིར ARMHugePageBits;
    pak_sa seL4_ARM_PageTableObject:
        ཕྱིར seL4_PageTableBits;
    pak_sa seL4_ARM_VSpaceObject:
        ཕྱིར seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_ARM_VCPUObject:
        ཕྱིར VCPU_SIZE_BITS;
#endif
    default:
        fail("Invalid object type");
        ཕྱིར 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        གལ་ཏེ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } དེ་མིན {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ཕྱིར cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    pak_sa seL4_ARM_LargePageObject:
        གལ་ཏེ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } དེ་མིན {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ཕྱིར cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    pak_sa seL4_ARM_HugePageObject:
        གལ་ཏེ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } དེ་མིན {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        ཕྱིར cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMHugePage,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );
    pak_sa seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_VSpaceBits),
                            addrFromPPtr(regionBase));
#ifdef CONFIG_ARM_SMMU
        ཕྱིར cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#དེ་མིན
        ཕྱིར cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#endif /*!CONFIG_ARM_SMMU*/
    pak_sa seL4_ARM_PageTableObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((word_t)regionBase,
                            (word_t)regionBase + MASK(seL4_PageTableBits),
                            addrFromPPtr(regionBase));
        ཕྱིར cap_page_table_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (word_t)regionBase,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(regionBase));
        ཕྱིར cap_vcpu_cap_new(VCPU_REF(regionBase));
#endif

    default:
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
#གལ་ཏེ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    switch (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ཕྱིར decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ཕྱིར decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
        ཕྱིར decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_sid_cap:
        ཕྱིར decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_cb_control_cap:
        ཕྱིར decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_cb_cap:
        ཕྱིར decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        ཕྱིར decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    default:
#དེ་མིན
{
#endif
    ཕྱིར decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}
}

void
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    གལ་ཏེ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
