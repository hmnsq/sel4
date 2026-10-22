/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/objecttype.h>

bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    انتخاب (type) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        واپس true;
    حالت se_L4_ARM_Large_صفحہ_Object:
        واپس true;
    حالت se_L4_ARM_Huge_صفحہ_Object:
        واپس true;
    طےشدہ:
        واپس false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_vspace_cap:
        اگر (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_صفحہ_جدول_cap:
        اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_چوکھٹا_cap:
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
    حالت cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    حالت cap_sid_cap:
    حالت cap_cb_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم
    طےشدہ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_ڈیٹا(bool_t preserve, لفظ_t ڈیٹا, cap_t cap)
{
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    اگر (cap_get_capType(cap) == cap_smc_cap) {
        اگر (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            واپس cap_smc_cap_سیٹ_cap_SMCBadge(cap, ڈیٹا);
        } ورنہ {
            واپس cap_null_cap_new();
        }
    } ورنہ {
#اگر_ختم
        واپس cap;
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    }
#اگر_ختم
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);

        واپس cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap, لفظ_From_VMRights(vm_rights));
    } ورنہ {
        واپس cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        توڑو;

    حالت cap_vspace_cap:
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
        اگر (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#اگر_ختم
        اگر (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (final && cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحہ_جدول(cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                           cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
                           PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        توڑو;

    حالت cap_چوکھٹا_cap:
        اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            unmap_صفحہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        توڑو;
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        // do nothing
        توڑو;
#اگر_ختم


#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    حالت cap_cb_cap:
        اگر (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        اگر (final) {
            smmu_delete_cb(cap);
        }
        توڑو;
    حالت cap_sid_cap:
        اگر (final) {
            smmu_delete_sid(cap);
        }
        توڑو;
#اگر_ختم
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_چوکھٹا_cap:
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {

            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_b))) ;
            واپس ((botA <= botB) && (چوٹی_A >= چوٹی_B) && (botB <= چوٹی_B));
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (cap_get_capType(cap_b) == cap_صفحہ_جدول_cap) {
            واپس cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        توڑو;

    حالت cap_vspace_cap:
        اگر (cap_get_capType(cap_b) == cap_vspace_cap) {
            واپس cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        توڑو;

    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            واپس true;
        }
        توڑو;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            واپس cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        توڑو;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            واپس cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        توڑو;
#اگر_ختم
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        اگر (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            واپس (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        توڑو;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            واپس true;
        }
        توڑو;
    حالت cap_cb_control_cap:
        اگر (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            واپس true;
        }
        توڑو;
    حالت cap_sid_cap:
        اگر (cap_get_capType(cap_b) == cap_sid_cap) {
            واپس cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        توڑو;
    حالت cap_cb_cap:
        اگر (cap_get_capType(cap_b) == cap_cb_cap) {
            واپس cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        توڑو;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        اگر (cap_get_capType(cap_b) == cap_smc_cap) {
            واپس true;
        }
        توڑو;
#اگر_ختم
    }
    واپس false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_چوکھٹا_cap) {
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            واپس ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    اگر (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        واپس false;
    }
    اگر (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        واپس false;
    }
#اگر_ختم
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    اگر (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        واپس false;
    }
#اگر_ختم

    واپس Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائز(لفظ_t t)
{
    انتخاب (t) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        واپس ARMSmall_صفحہ_بٹ;
    حالت se_L4_ARM_Large_صفحہ_Object:
        واپس ARMLarge_صفحہ_بٹ;
    حالت se_L4_ARM_Huge_صفحہ_Object:
        واپس ARMHuge_صفحہ_بٹ;
    حالت se_L4_ARM_صفحہ_جدول_Object:
        واپس se_L4_صفحہ_جدول_بٹ;
    حالت seL4_ARM_VSpaceObject:
        واپس se_L4_VSpace_بٹ;
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        واپس VCPU_سائز_بٹ;
#اگر_ختم
    طےشدہ:
        fail("Invalid object type");
        واپس 0;
    }
}

cap_t Arch_createObject(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    انتخاب (t) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMSmall_صفحہ)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنیاد,    /* capFBasePtr */
                   ARMSmall_صفحہ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظہ         /* capFIsDevice */
               );

    حالت se_L4_ARM_Large_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMLarge_صفحہ)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنیاد,    /* capFBasePtr */
                   ARMLarge_صفحہ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظہ         /* capFIsDevice */
               );

    حالت se_L4_ARM_Huge_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMHuge_صفحہ)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنیاد,    /* capFBasePtr */
                   ARMHuge_صفحہ,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظہ         /* capFIsDevice */
               );
    حالت seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((لفظ_t)region_بنیاد,
                            (لفظ_t)region_بنیاد + نقاب(se_L4_VSpace_بٹ),
                            addrFromPPtr(region_بنیاد));
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
        واپس cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (لفظ_t)region_بنیاد,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ورنہ
        واپس cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (لفظ_t)region_بنیاد,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#اگر_ختم /*!CONFIG_ARM_SMMU*/
    حالت se_L4_ARM_صفحہ_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((لفظ_t)region_بنیاد,
                            (لفظ_t)region_بنیاد + نقاب(se_L4_صفحہ_جدول_بٹ),
                            addrFromPPtr(region_بنیاد));
        واپس cap_صفحہ_جدول_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (لفظ_t)region_بنیاد,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_بنیاد));
        واپس cap_vcpu_cap_new(VCPU_REF(region_بنیاد));
#اگر_ختم

    طےشدہ:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(لفظ_t label, لفظ_t لمبائی, cptr_t cptr,
                                  cte_t *خانہ, cap_t cap,
                                  bool_t call, لفظ_t *بفر)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#اگر defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    انتخاب (cap_get_capType(cap)) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        واپس decodeARMVCPUInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
#اگر_ختم /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        واپس decodeSGISignalInvocation(label, لمبائی, cap, بفر);
#اگر_ختم /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#اگر_تعریف_شدہ CONFIG_ARM_SMMU
    حالت cap_sid_control_cap:
        واپس decodeARMSIDControlInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
    حالت cap_sid_cap:
        واپس decodeARMSIDInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
    حالت cap_cb_control_cap:
        واپس decodeARMCBControlInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
    حالت cap_cb_cap:
        واپس decodeARMCBInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
#اگر_ختم /*CONFIG_ARM_SMMU*/
#اگر_تعریف_شدہ CONFIG_ALLOW_SMC_CALLS
    حالت cap_smc_cap:
        واپس decodeARMSMCInvocation(label, لمبائی, cap, call, بفر);
#اگر_ختم
    طےشدہ:
#ورنہ
{
#اگر_ختم
    واپس decodeARMMMUInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
}
}

خالی
Arch_prepareThreadDelete(tcb_t * thread) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    اگر (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    fpuRelease(thread);
#اگر_ختم
}
