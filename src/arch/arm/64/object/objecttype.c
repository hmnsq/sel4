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

bool_t Arch_is_چوکاټ_Type(کلمه_t type)
{
    بدل (type) {
    حالة se_L4_ARM_Small_پاڼه_Object:
        أرجع true;
    حالة se_L4_ARM_Large_پاڼه_Object:
        أرجع true;
    حالة se_L4_ARM_Huge_پاڼه_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *خونه, cap_t cap)
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

    حالة cap_پاڼه_جدول_cap:
        إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_چوکاټ_cap:
        ret.cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#د_که_پای
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_SMMU
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
#د_که_پای
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#د_که_پای
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_معلومات(bool_t preserve, کلمه_t معلومات, cap_t cap)
{
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    إذا (cap_get_capType(cap) == cap_smc_cap) {
        إذا (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            أرجع cap_smc_cap_سېټ_cap_SMCBadge(cap, معلومات);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    } وإلا {
#د_که_پای
        أرجع cap;
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    }
#د_که_پای
}

cap_t CONST Arch_ماسک_Cap_Rights(seL4_CapRights_t cap_rights_ماسک, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_چوکاټ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(cap_چوکاټ_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);

        أرجع cap_چوکاټ_cap_سېټ_cap_FVMRights(cap, کلمه_From_VMRights(vm_rights));
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
#که_تعریف_شوی CONFIG_ARM_SMMU
        إذا (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#د_که_پای
        إذا (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        اكسر;

    حالة cap_پاڼه_جدول_cap:
        إذا (final && cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_پاڼه_جدول(cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
                           cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap),
                           PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_چوکاټ_cap:
        إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap)) {
            unmap_پاڼه(cap_چوکاټ_cap_get_cap_FSize(cap),
                      cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                      cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#د_که_پای
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#د_که_پای


#که_تعریف_شوی CONFIG_ARM_SMMU
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
#د_که_پای
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_چوکاټ_cap:
        إذا (cap_get_capType(cap_b) == cap_چوکاټ_cap) {

            کلمه_t botA, botB, سر_A, سر_B;
            botA = cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_b);
            سر_A = botA + ماسک(پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap_a)));
            سر_B = botB + ماسک(پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap_b))) ;
            أرجع ((botA <= botB) && (سر_A >= سر_B) && (botB <= سر_B));
        }
        اكسر;

    حالة cap_پاڼه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_پاڼه_جدول_cap) {
            أرجع cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap_b);
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

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#د_که_پای
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        إذا (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#د_که_پای

#که_تعریف_شوی CONFIG_ARM_SMMU
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
#د_که_پای
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        إذا (cap_get_capType(cap_b) == cap_smc_cap) {
            أرجع true;
        }
        اكسر;
#د_که_پای
    }
    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_چوکاټ_cap) {
        إذا (cap_get_capType(cap_b) == cap_چوکاټ_cap) {
            أرجع ((cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکاټ_cap_get_cap_FSize(cap_a) ==
                     cap_چوکاټ_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکاټ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکاټ_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#که_تعریف_شوی CONFIG_ARM_SMMU
    إذا (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        أرجع false;
    }
    إذا (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        أرجع false;
    }
#د_که_پای
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    إذا (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#د_که_پای

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

کلمه_t Arch_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة se_L4_ARM_Small_پاڼه_Object:
        أرجع ARMSmall_پاڼه_بټونه;
    حالة se_L4_ARM_Large_پاڼه_Object:
        أرجع ARMLarge_پاڼه_بټونه;
    حالة se_L4_ARM_Huge_پاڼه_Object:
        أرجع ARMHuge_پاڼه_بټونه;
    حالة se_L4_ARM_پاڼه_جدول_Object:
        أرجع se_L4_پاڼه_جدول_بټونه;
    حالة seL4_ARM_VSpaceObject:
        أرجع se_L4_VSpace_بټونه;
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_اندازه_بټونه;
#د_که_پای
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_بنسټ, کلمه_t user_اندازه, bool_t device_حافظه)
{
    بدل (t) {
    حالة se_L4_ARM_Small_پاڼه_Object:
        إذا (device_حافظه) {
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
            cleanCacheRange_RAM((کلمه_t)region_بنسټ,
                                (کلمه_t)region_بنسټ + ماسک(پاڼه_بټونه_For_اندازه(ARMSmall_پاڼه)),
                                addrFromPPtr(region_بنسټ));
        }
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (کلمه_t)region_بنسټ,    /* capFBasePtr */
                   ARMSmall_پاڼه,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظه         /* capFIsDevice */
               );

    حالة se_L4_ARM_Large_پاڼه_Object:
        إذا (device_حافظه) {
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
            cleanCacheRange_RAM((کلمه_t)region_بنسټ,
                                (کلمه_t)region_بنسټ + ماسک(پاڼه_بټونه_For_اندازه(ARMLarge_پاڼه)),
                                addrFromPPtr(region_بنسټ));
        }
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (کلمه_t)region_بنسټ,    /* capFBasePtr */
                   ARMLarge_پاڼه,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظه         /* capFIsDevice */
               );

    حالة se_L4_ARM_Huge_پاڼه_Object:
        إذا (device_حافظه) {
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
            cleanCacheRange_RAM((کلمه_t)region_بنسټ,
                                (کلمه_t)region_بنسټ + ماسک(پاڼه_بټونه_For_اندازه(ARMHuge_پاڼه)),
                                addrFromPPtr(region_بنسټ));
        }
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (کلمه_t)region_بنسټ,    /* capFBasePtr */
                   ARMHuge_پاڼه,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_حافظه         /* capFIsDevice */
               );
    حالة seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((کلمه_t)region_بنسټ,
                            (کلمه_t)region_بنسټ + ماسک(se_L4_VSpace_بټونه),
                            addrFromPPtr(region_بنسټ));
#که_تعریف_شوی CONFIG_ARM_SMMU
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (کلمه_t)region_بنسټ,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#وإلا
        أرجع cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (کلمه_t)region_بنسټ,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#د_که_پای /*!CONFIG_ARM_SMMU*/
    حالة se_L4_ARM_پاڼه_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((کلمه_t)region_بنسټ,
                            (کلمه_t)region_بنسټ + ماسک(se_L4_پاڼه_جدول_بټونه),
                            addrFromPPtr(region_بنسټ));
        أرجع cap_پاڼه_جدول_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (کلمه_t)region_بنسټ,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_بنسټ));
        أرجع cap_vcpu_cap_new(VCPU_REF(region_بنسټ));
#د_که_پای

    افتراضي:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(کلمه_t label, کلمه_t اوږدتیا, cptr_t cptr,
                                  cte_t *خونه, cap_t cap,
                                  bool_t call, کلمه_t *بفر)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#إذا defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
#د_که_پای /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#که_نه_تعریف_شوی CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(label, اوږدتیا, cap, بفر);
#د_که_پای /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#که_تعریف_شوی CONFIG_ARM_SMMU
    حالة cap_sid_control_cap:
        أرجع decodeARMSIDControlInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
    حالة cap_sid_cap:
        أرجع decodeARMSIDInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
    حالة cap_cb_control_cap:
        أرجع decodeARMCBControlInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
    حالة cap_cb_cap:
        أرجع decodeARMCBInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
#د_که_پای /*CONFIG_ARM_SMMU*/
#که_تعریف_شوی CONFIG_ALLOW_SMC_CALLS
    حالة cap_smc_cap:
        أرجع decodeARMSMCInvocation(label, اوږدتیا, cap, call, بفر);
#د_که_پای
    افتراضي:
#وإلا
{
#د_که_پای
    أرجع decodeARMMMUInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#د_که_پای /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#که_تعریف_شوی CONFIG_HAVE_FPU
    fpuRelease(thread);
#د_که_پای
}
