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

bool_t Arch_is_چوکٺو_Type(لفظ_t type)
{
    بدل (type) {
    حالة se_L4_ARM_Small_صفحو_Object:
        أرجع true;
    حالة se_L4_ARM_Large_صفحو_Object:
        أرجع true;
    حالة se_L4_ARM_Huge_صفحو_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *جڳهه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_vspace_cap:
        إذا (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_صفحو_جدول_cap:
        إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_چوکٺو_cap:
        ret.cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
    حالة cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_sid_cap:
    حالة cap_cb_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_معلومات(bool_t preserve, لفظ_t معلومات, cap_t cap)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    إذا (cap_get_capType(cap) == cap_smc_cap) {
        إذا (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            أرجع cap_smc_cap_سيٽ_cap_SMCBadge(cap, معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    } وإلا {
#جيڪڏهن_پورو
        أرجع cap;
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    }
#جيڪڏهن_پورو
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_چوکٺو_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(cap_چوکٺو_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);

        أرجع cap_چوکٺو_cap_سيٽ_cap_FVMRights(cap, لفظ_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_asid_pool_cap:
        إذا (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        اكسر;

    حالة cap_vspace_cap:
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
        إذا (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#جيڪڏهن_پورو
        إذا (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        اكسر;

    حالة cap_صفحو_جدول_cap:
        إذا (final && cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحو_جدول(cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
                           cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap),
                           PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_چوکٺو_cap:
        إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap)) {
            unmap_صفحو(cap_چوکٺو_cap_get_cap_FSize(cap),
                      cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                      cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    حالة cap_cb_cap:
        إذا (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        إذا (final) {
            smmu_delete_cb(cap);
        }
        اكسر;
    حالة cap_sid_cap:
        إذا (final) {
            smmu_delete_sid(cap);
        }
        اكسر;
#جيڪڏهن_پورو
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_چوکٺو_cap:
        إذا (cap_get_capType(cap_b) == cap_چوکٺو_cap) {

            لفظ_t botA, botB, چوٽي_A, چوٽي_B;
            botA = cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_b);
            چوٽي_A = botA + نقاب(صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(cap_a)));
            چوٽي_B = botB + نقاب(صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(cap_b))) ;
            أرجع ((botA <= botB) && (چوٽي_A >= چوٽي_B) && (botB <= چوٽي_B));
        }
        اكسر;

    حالة cap_صفحو_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحو_جدول_cap) {
            أرجع cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        اكسر;

    حالة cap_vspace_cap:
        إذا (cap_get_capType(cap_b) == cap_vspace_cap) {
            أرجع cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        اكسر;

    حالة cap_asid_control_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_control_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            أرجع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        إذا (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
        إذا (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            أرجع true;
        }
        اكسر;
    حالة cap_cb_control_cap:
        إذا (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            أرجع true;
        }
        اكسر;
    حالة cap_sid_cap:
        إذا (cap_get_capType(cap_b) == cap_sid_cap) {
            أرجع cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        اكسر;
    حالة cap_cb_cap:
        إذا (cap_get_capType(cap_b) == cap_cb_cap) {
            أرجع cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        اكسر;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        إذا (cap_get_capType(cap_b) == cap_smc_cap) {
            أرجع true;
        }
        اكسر;
#جيڪڏهن_پورو
    }
    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_چوکٺو_cap) {
        إذا (cap_get_capType(cap_b) == cap_چوکٺو_cap) {
            أرجع ((cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکٺو_cap_get_cap_FSize(cap_a) ==
                     cap_چوکٺو_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکٺو_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکٺو_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    إذا (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        أرجع false;
    }
    إذا (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        أرجع false;
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    إذا (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#جيڪڏهن_پورو

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائيز(لفظ_t t)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحو_Object:
        أرجع ARMSmall_صفحو_بٽ;
    حالة se_L4_ARM_Large_صفحو_Object:
        أرجع ARMLarge_صفحو_بٽ;
    حالة se_L4_ARM_Huge_صفحو_Object:
        أرجع ARMHuge_صفحو_بٽ;
    حالة se_L4_ARM_صفحو_جدول_Object:
        أرجع se_L4_صفحو_جدول_بٽ;
    حالة seL4_ARM_VSpaceObject:
        أرجع se_L4_VSpace_بٽ;
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_سائيز_بٽ;
#جيڪڏهن_پورو
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_بنياد, لفظ_t user_سائيز, bool_t device_يادگيري)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحو_Object:
        إذا (device_يادگيري) {
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
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMSmall_صفحو)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنياد,    /* capFBasePtr */
                   ARMSmall_صفحو,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_يادگيري         /* capFIsDevice */
               );

    حالة se_L4_ARM_Large_صفحو_Object:
        إذا (device_يادگيري) {
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
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMLarge_صفحو)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنياد,    /* capFBasePtr */
                   ARMLarge_صفحو,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_يادگيري         /* capFIsDevice */
               );

    حالة se_L4_ARM_Huge_صفحو_Object:
        إذا (device_يادگيري) {
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
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMHuge_صفحو)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (لفظ_t)region_بنياد,    /* capFBasePtr */
                   ARMHuge_صفحو,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_يادگيري         /* capFIsDevice */
               );
    حالة seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((لفظ_t)region_بنياد,
                            (لفظ_t)region_بنياد + نقاب(se_L4_VSpace_بٽ),
                            addrFromPPtr(region_بنياد));
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (لفظ_t)region_بنياد,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#وإلا
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (لفظ_t)region_بنياد,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#جيڪڏهن_پورو /*!CONFIG_ARM_SMMU*/
    حالة se_L4_ARM_صفحو_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((لفظ_t)region_بنياد,
                            (لفظ_t)region_بنياد + نقاب(se_L4_صفحو_جدول_بٽ),
                            addrFromPPtr(region_بنياد));
        أرجع cap_صفحو_جدول_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (لفظ_t)region_بنياد,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_بنياد));
        أرجع cap_vcpu_cap_new(VCPU_REF(region_بنياد));
#جيڪڏهن_پورو

    افتراضي:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(لفظ_t label, لفظ_t ڊيگهائي, cptr_t cptr,
                                  cte_t *جڳهه, cap_t cap,
                                  bool_t call, لفظ_t *بفر)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#إذا defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
#جيڪڏهن_پورو /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(label, ڊيگهائي, cap, بفر);
#جيڪڏهن_پورو /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
        أرجع decodeARMSIDControlInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
    حالة cap_sid_cap:
        أرجع decodeARMSIDInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
    حالة cap_cb_control_cap:
        أرجع decodeARMCBControlInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
    حالة cap_cb_cap:
        أرجع decodeARMCBInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
#جيڪڏهن_پورو /*CONFIG_ARM_SMMU*/
#جيڪڏهن_وصف_ٿيل CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        أرجع decodeARMSMCInvocation(label, ڊيگهائي, cap, call, بفر);
#جيڪڏهن_پورو
    افتراضي:
#وإلا
{
#جيڪڏهن_پورو
    أرجع decodeARMMMUInvocation(label, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    fpuRelease(thread);
#جيڪڏهن_پورو
}
