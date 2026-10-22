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
        ᰀᰧᰌ true;
    pak_sa seL4_ARM_LargePageObject:
        ᰀᰧᰌ true;
    pak_sa seL4_ARM_HugePageObject:
        ᰀᰧᰌ true;
    default:
        ᰀᰧᰌ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_vspace_cap:
        ᰀᰧ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ᰀᰧᰀ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ᰀᰧᰌ ret;

    pak_sa cap_page_table_cap:
        ᰀᰧ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ᰀᰧᰀ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ᰀᰧᰌ ret;

    pak_sa cap_frame_cap:
        ret.cap = cap_frame_cap_set_capFMappedASID(cap, asidInvalid);
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;

    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;
#endif

#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
    pak_sa cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;
    pak_sa cap_sid_cap:
    pak_sa cap_cb_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ᰀᰧᰌ ret;
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
    ᰀᰧ (cap_get_capType(cap) == cap_smc_cap) {
        ᰀᰧ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ᰀᰧᰌ cap_smc_cap_set_capSMCBadge(cap, data);
        } ᰀᰧᰀ {
            ᰀᰧᰌ cap_null_cap_new();
        }
    } ᰀᰧᰀ {
#endif
        ᰀᰧᰌ cap;
#ifdef CONFIG_ALLOW_SMC_CALLS
    }
#endif
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ᰀᰧ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);

        ᰀᰧᰌ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ᰀᰧᰀ {
        ᰀᰧᰌ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_asid_pool_cap:
        ᰀᰧ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        bha_nga;

    pak_sa cap_vspace_cap:
#ifdef CONFIG_ARM_SMMU
        ᰀᰧ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#endif
        ᰀᰧ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        ᰀᰧ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(cap_page_table_cap_get_capPTMappedASID(cap),
                           cap_page_table_cap_get_capPTMappedAddress(cap),
                           PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
        }
        bha_nga;

    pak_sa cap_frame_cap:
        ᰀᰧ (cap_frame_cap_get_capFMappedASID(cap)) {
            unmapPage(cap_frame_cap_get_capFSize(cap),
                      cap_frame_cap_get_capFMappedASID(cap),
                      cap_frame_cap_get_capFMappedAddress(cap),
                      cap_frame_cap_get_capFBasePtr(cap));
        }
        bha_nga;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ᰀᰧ (final) {
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
        ᰀᰧ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ᰀᰧ (final) {
            smmu_delete_cb(cap);
        }
        bha_nga;
    pak_sa cap_sid_cap:
        ᰀᰧ (final) {
            smmu_delete_sid(cap);
        }
        bha_nga;
#endif
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ᰀᰧᰌ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    pak_sa cap_frame_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_frame_cap) {

            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b))) ;
            ᰀᰧᰌ ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ᰀᰧᰌ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_vspace_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_vspace_cap) {
            ᰀᰧᰌ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_asid_control_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ᰀᰧᰌ true;
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ᰀᰧᰌ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        bha_nga;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ᰀᰧᰌ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        bha_nga;
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ᰀᰧᰌ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        bha_nga;
#endif

#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ᰀᰧᰌ true;
        }
        bha_nga;
    pak_sa cap_cb_control_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ᰀᰧᰌ true;
        }
        bha_nga;
    pak_sa cap_sid_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_sid_cap) {
            ᰀᰧᰌ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        bha_nga;
    pak_sa cap_cb_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_cb_cap) {
            ᰀᰧᰌ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        bha_nga;
#endif
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        ᰀᰧ (cap_get_capType(cap_b) == cap_smc_cap) {
            ᰀᰧᰌ true;
        }
        bha_nga;
#endif
    }
    ᰀᰧᰌ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ᰀᰧ (cap_get_capType(cap_a) == cap_frame_cap) {
        ᰀᰧ (cap_get_capType(cap_b) == cap_frame_cap) {
            ᰀᰧᰌ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
#ifdef CONFIG_ARM_SMMU
    ᰀᰧ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ᰀᰧᰌ false;
    }
    ᰀᰧ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ᰀᰧᰌ false;
    }
#endif
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    ᰀᰧ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ᰀᰧᰌ false;
    }
#endif

    ᰀᰧᰌ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        ᰀᰧᰌ ARMSmallPageBits;
    pak_sa seL4_ARM_LargePageObject:
        ᰀᰧᰌ ARMLargePageBits;
    pak_sa seL4_ARM_HugePageObject:
        ᰀᰧᰌ ARMHugePageBits;
    pak_sa seL4_ARM_PageTableObject:
        ᰀᰧᰌ seL4_PageTableBits;
    pak_sa seL4_ARM_VSpaceObject:
        ᰀᰧᰌ seL4_VSpaceBits;
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa seL4_ARM_VCPUObject:
        ᰀᰧᰌ VCPU_SIZE_BITS;
#endif
    default:
        fail("Invalid object type");
        ᰀᰧᰌ 0;
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
    switch (t) {
    pak_sa seL4_ARM_SmallPageObject:
        ᰀᰧ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ᰀᰧᰀ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMSmallPage)),
                                addrFromPPtr(regionBase));
        }
        ᰀᰧᰌ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMSmallPage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    pak_sa seL4_ARM_LargePageObject:
        ᰀᰧ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ᰀᰧᰀ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMLargePage)),
                                addrFromPPtr(regionBase));
        }
        ᰀᰧᰌ cap_frame_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (word_t)regionBase,    /* capFBasePtr */
                   ARMLargePage,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!deviceMemory         /* capFIsDevice */
               );

    pak_sa seL4_ARM_HugePageObject:
        ᰀᰧ (deviceMemory) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ᰀᰧᰀ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((word_t)regionBase,
                                (word_t)regionBase + MASK(pageBitsForSize(ARMHugePage)),
                                addrFromPPtr(regionBase));
        }
        ᰀᰧᰌ cap_frame_cap_new(
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
        ᰀᰧᰌ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (word_t)regionBase,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ᰀᰧᰀ
        ᰀᰧᰌ cap_vspace_cap_new(
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
        ᰀᰧᰌ cap_page_table_cap_new(
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
        ᰀᰧᰌ cap_vcpu_cap_new(VCPU_REF(regionBase));
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
#ᰀᰧ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    switch (cap_get_capType(cap)) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    pak_sa cap_vcpu_cap:
        ᰀᰧᰌ decodeARMVCPUInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ifndef CONFIG_ENABLE_SMP_SUPPORT
    pak_sa cap_sgi_signal_cap:
        ᰀᰧᰌ decodeSGISignalInvocation(label, length, cap, buffer);
#endif /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ifdef CONFIG_ARM_SMMU
    pak_sa cap_sid_control_cap:
        ᰀᰧᰌ decodeARMSIDControlInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_sid_cap:
        ᰀᰧᰌ decodeARMSIDInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_cb_control_cap:
        ᰀᰧᰌ decodeARMCBControlInvocation(label, length, cptr, slot, cap, call, buffer);
    pak_sa cap_cb_cap:
        ᰀᰧᰌ decodeARMCBInvocation(label, length, cptr, slot, cap, call, buffer);
#endif /*CONFIG_ARM_SMMU*/
#ifdef CONFIG_ALLOW_SMC_CALLS
    pak_sa cap_smc_cap:
        ᰀᰧᰌ decodeARMSMCInvocation(label, length, cap, call, buffer);
#endif
    default:
#ᰀᰧᰀ
{
#endif
    ᰀᰧᰌ decodeARMMMUInvocation(label, length, cptr, slot, cap, call, buffer);
}
}

void
Arch_prepareThreadDelete(tcb_t * thread) {
#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    ᰀᰧ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#endif /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ifdef CONFIG_HAVE_FPU
    fpuRelease(thread);
#endif
}
