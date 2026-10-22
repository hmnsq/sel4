/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>

bool_t Arch_is_ਚੌਖਟਾ_Type(ਸ਼ਬਦ_t type)
{
    ਚੋਣ (type) {
    ਹਾਲਤ se_L4_ARM_Small_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_ARM_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_ARM_Huge_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_vspace_cap:
        ਜੇ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ret.cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_asid_control_cap:
    ਹਾਲਤ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਹਾਲਤ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਹਾਲਤ cap_sid_control_cap:
    ਹਾਲਤ cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_sid_cap:
    ਹਾਲਤ cap_cb_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    ਹਾਲਤ cap_smc_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_ਅੰਕੜੇ(bool_t preserve, ਸ਼ਬਦ_t ਅੰਕੜੇ, cap_t cap)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    ਜੇ (cap_get_capType(cap) == cap_smc_cap) {
        ਜੇ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ਵਾਪਸ cap_smc_cap_ਸੈੱਟ_cap_SMCBadge(cap, ਅੰਕੜੇ);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ cap_null_cap_new();
        }
    } ਨਹੀਂਤਾਂ {
#ਜੇ_ਅੰਤ
        ਵਾਪਸ cap;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    }
#ਜੇ_ਅੰਤ
}

cap_t CONST Arch_ਨਕਾਬ_Cap_Rights(seL4_CapRights_t cap_rights_ਨਕਾਬ, cap_t cap)
{
    ਜੇ (cap_get_capType(cap) == cap_ਚੌਖਟਾ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ਸ਼ਬਦ(cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap));
        vm_rights = ਨਕਾਬ_VMRights(vm_rights, cap_rights_ਨਕਾਬ);

        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FVMRights(cap, ਸ਼ਬਦ_From_VMRights(vm_rights));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_asid_pool_cap:
        ਜੇ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_vspace_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
        ਜੇ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#ਜੇ_ਅੰਤ
        ਜੇ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ਪੰਨਾ_ਸਾਰਣੀ(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
                           cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
                           PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap)));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
            unmap_ਪੰਨਾ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
        }
        ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap:
        ਜੇ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਹਾਲਤ cap_sgi_signal_cap:
        // do nothing
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਹਾਲਤ cap_cb_cap:
        ਜੇ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ਜੇ (final) {
            smmu_delete_cb(cap);
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_sid_cap:
        ਜੇ (final) {
            smmu_delete_sid(cap);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ਚੋਣ (cap_get_capType(cap_a)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {

            ਸ਼ਬਦ_t botA, botB, ਸਿਖਰ_A, ਸਿਖਰ_B;
            botA = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b);
            ਸਿਖਰ_A = botA + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a)));
            ਸਿਖਰ_B = botB + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b))) ;
            ਵਾਪਸ ((botA <= botB) && (ਸਿਖਰ_A >= ਸਿਖਰ_B) && (botB <= ਸਿਖਰ_B));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
            ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_vspace_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_vspace_cap) {
            ਵਾਪਸ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ਵਾਪਸ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ਵਾਪਸ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਹਾਲਤ cap_sgi_signal_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ਵਾਪਸ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਹਾਲਤ cap_sid_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_cb_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_sid_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_sid_cap) {
            ਵਾਪਸ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_cb_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_cb_cap) {
            ਵਾਪਸ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    ਹਾਲਤ cap_smc_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_smc_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }
    ਵਾਪਸ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ਜੇ (cap_get_capType(cap_a) == cap_ਚੌਖਟਾ_cap) {
        ਜੇ (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {
            ਵਾਪਸ ((cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a) ==
                     cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b)) &&
                    ((cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਜੇ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ਵਾਪਸ false;
    }
    ਜੇ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਜੇ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ਵਾਪਸ false;
    }
#ਜੇ_ਅੰਤ

    ਵਾਪਸ Arch_sameRegionAs(cap_a, cap_b);
}

ਸ਼ਬਦ_t Arch_get_Object_ਮਾਪ(ਸ਼ਬਦ_t t)
{
    ਚੋਣ (t) {
    ਹਾਲਤ se_L4_ARM_Small_ਪੰਨਾ_Object:
        ਵਾਪਸ ARMSmall_ਪੰਨਾ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_ARM_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ ARMLarge_ਪੰਨਾ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_ARM_Huge_ਪੰਨਾ_Object:
        ਵਾਪਸ ARMHuge_ਪੰਨਾ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        ਵਾਪਸ se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ;
    ਹਾਲਤ seL4_ARM_VSpaceObject:
        ਵਾਪਸ se_L4_VSpace_ਬਿੱਟ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_ARM_VCPUObject:
        ਵਾਪਸ VCPU_ਮਾਪ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        fail("Invalid object type");
        ਵਾਪਸ 0;
    }
}

cap_t Arch_createObject(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
    ਚੋਣ (t) {
    ਹਾਲਤ se_L4_ARM_Small_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSmall_ਪੰਨਾ)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capFBasePtr */
                   ARMSmall_ਪੰਨਾ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ਸਮ੍ਰਿਤੀ         /* capFIsDevice */
               );

    ਹਾਲਤ se_L4_ARM_Large_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMLarge_ਪੰਨਾ)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capFBasePtr */
                   ARMLarge_ਪੰਨਾ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ਸਮ੍ਰਿਤੀ         /* capFIsDevice */
               );

    ਹਾਲਤ se_L4_ARM_Huge_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMHuge_ਪੰਨਾ)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capFBasePtr */
                   ARMHuge_ਪੰਨਾ,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ਸਮ੍ਰਿਤੀ         /* capFIsDevice */
               );
    ਹਾਲਤ seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                            (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(se_L4_VSpace_ਬਿੱਟ),
                            addrFromPPtr(region_ਅਧਾਰ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
        ਵਾਪਸ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#ਜੇ_ਅੰਤ /*!CONFIG_ARM_SMMU*/
    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                            (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ),
                            addrFromPPtr(region_ਅਧਾਰ));
        ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_ਅਧਾਰ));
        ਵਾਪਸ cap_vcpu_cap_new(VCPU_REF(region_ਅਧਾਰ));
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(ਸ਼ਬਦ_t label, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                  cte_t *ਥਾਂ, cap_t cap,
                                  bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ਜੇ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ਚੋਣ (cap_get_capType(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap:
        ਵਾਪਸ decodeARMVCPUInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
#ਜੇ_ਅੰਤ /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਹਾਲਤ cap_sgi_signal_cap:
        ਵਾਪਸ decodeSGISignalInvocation(label, ਲੰਮਾਈ, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_SMMU
    ਹਾਲਤ cap_sid_control_cap:
        ਵਾਪਸ decodeARMSIDControlInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਹਾਲਤ cap_sid_cap:
        ਵਾਪਸ decodeARMSIDInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਹਾਲਤ cap_cb_control_cap:
        ਵਾਪਸ decodeARMCBControlInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਹਾਲਤ cap_cb_cap:
        ਵਾਪਸ decodeARMCBInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
#ਜੇ_ਅੰਤ /*CONFIG_ARM_SMMU*/
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ALLOW_SMC_CALLS
    ਹਾਲਤ cap_smc_cap:
        ਵਾਪਸ decodeARMSMCInvocation(label, ਲੰਮਾਈ, cap, call, ਬਫਰ);
#ਜੇ_ਅੰਤ
    ਮੂਲ:
#ਨਹੀਂਤਾਂ
{
#ਜੇ_ਅੰਤ
    ਵਾਪਸ decodeARMMMUInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
}
}

ਖਾਲੀ
Arch_prepareThreadDelete(tcb_t * thread) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਜੇ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    fpuRelease(thread);
#ਜੇ_ਅੰਤ
}
