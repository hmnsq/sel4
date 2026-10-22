/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <api/failures.h>
#ಅನ್ತರ್ಭಾವ <kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <object/structures.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>

bool_t Arch_is_ಚೌಕಟ್ಟು_Type(ಶಬ್ದ_t type)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ se_L4_ARM_Small_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_ARM_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_ARM_Huge_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_vspace_cap:
        ಆದರೆ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ret.cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_asid_control_cap:
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ cap_sgi_signal_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಸ್ಥಿತಿ cap_sid_control_cap:
    ಸ್ಥಿತಿ cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_sid_cap:
    ಸ್ಥಿತಿ cap_cb_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    ಸ್ಥಿತಿ cap_smc_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_ದತ್ತಾಂಶ(bool_t preserve, ಶಬ್ದ_t ದತ್ತಾಂಶ, cap_t cap)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    ಆದರೆ (cap_get_capType(cap) == cap_smc_cap) {
        ಆದರೆ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            ಹಿಂತಿರುಗಿ cap_smc_cap_ಗಣ_cap_SMCBadge(cap, ದತ್ತಾಂಶ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    } ಇಲ್ಲದಿದ್ದರೆ {
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ cap;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    }
#ಆದರೆ_ಅಂತ್ಯ
}

cap_t CONST Arch_ಮುಖವಾಡ_Cap_Rights(seL4_CapRights_t cap_rights_ಮುಖವಾಡ, cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) == cap_ಚೌಕಟ್ಟು_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ಶಬ್ದ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap));
        vm_rights = ಮುಖವಾಡ_VMRights(vm_rights, cap_rights_ಮುಖವಾಡ);

        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FVMRights(cap, ಶಬ್ದ_From_VMRights(vm_rights));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಆದರೆ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_vspace_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
        ಆದರೆ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ಪುಟ_ಕೋಷ್ಟಕ(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
                           cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
                           PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap)));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
            unmap_ಪುಟ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
        }
        ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಆದರೆ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ cap_sgi_signal_cap:
        // do nothing
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಸ್ಥಿತಿ cap_cb_cap:
        ಆದರೆ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ಆದರೆ (final) {
            smmu_delete_cb(cap);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_sid_cap:
        ಆದರೆ (final) {
            smmu_delete_sid(cap);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ಆಯ್ಕೆ (cap_get_capType(cap_a)) {
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {

            ಶಬ್ದ_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b);
            ತುದಿ_A = botA + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a)));
            ತುದಿ_B = botB + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b))) ;
            ಹಿಂತಿರುಗಿ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
            ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_vspace_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_vspace_cap) {
            ಹಿಂತಿರುಗಿ cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ಹಿಂತಿರುಗಿ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ಹಿಂತಿರುಗಿ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ cap_sgi_signal_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            ಹಿಂತಿರುಗಿ (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಸ್ಥಿತಿ cap_sid_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_cb_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_sid_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_sid_cap) {
            ಹಿಂತಿರುಗಿ cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_cb_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_cb_cap) {
            ಹಿಂತಿರುಗಿ cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    ಸ್ಥಿತಿ cap_smc_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_smc_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಹಿಂತಿರುಗಿ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಆದರೆ (cap_get_capType(cap_a) == cap_ಚೌಕಟ್ಟು_cap) {
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {
            ಹಿಂತಿರುಗಿ ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a) ==
                     cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b)) &&
                    ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಆದರೆ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಆದರೆ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ Arch_sameRegionAs(cap_a, cap_b);
}

ಶಬ್ದ_t Arch_get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ se_L4_ARM_Small_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ ARMSmall_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_ARM_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ ARMLarge_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_ARM_Huge_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ ARMHuge_ಪುಟ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ seL4_ARM_VSpaceObject:
        ಹಿಂತಿರುಗಿ se_L4_VSpace_ಬಿಟ್ಟುಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_ARM_VCPUObject:
        ಹಿಂತಿರುಗಿ VCPU_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid object type");
        ಹಿಂತಿರುಗಿ 0;
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ se_L4_ARM_Small_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSmall_ಪುಟ)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capFBasePtr */
                   ARMSmall_ಪುಟ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ಸ್ಮರಣೆ         /* capFIsDevice */
               );

    ಸ್ಥಿತಿ se_L4_ARM_Large_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMLarge_ಪುಟ)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capFBasePtr */
                   ARMLarge_ಪುಟ,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ಸ್ಮರಣೆ         /* capFIsDevice */
               );

    ಸ್ಥಿತಿ se_L4_ARM_Huge_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMHuge_ಪುಟ)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capFBasePtr */
                   ARMHuge_ಪುಟ,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_ಸ್ಮರಣೆ         /* capFIsDevice */
               );
    ಸ್ಥಿತಿ seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((ಶಬ್ದ_t)region_ತಳ,
                            (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(se_L4_VSpace_ಬಿಟ್ಟುಗಳು),
                            addrFromPPtr(region_ತಳ));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
        ಹಿಂತಿರುಗಿ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#ಆದರೆ_ಅಂತ್ಯ /*!CONFIG_ARM_SMMU*/
    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((ಶಬ್ದ_t)region_ತಳ,
                            (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು),
                            addrFromPPtr(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_vcpu_cap_new(VCPU_REF(region_ತಳ));
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(ಶಬ್ದ_t label, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                  cte_t *ಸ್ಥಾನ, cap_t cap,
                                  bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ಆದರೆ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಹಿಂತಿರುಗಿ decodeARMVCPUInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ cap_sgi_signal_cap:
        ಹಿಂತಿರುಗಿ decodeSGISignalInvocation(label, ಉದ್ದಳತೆ, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_SMMU
    ಸ್ಥಿತಿ cap_sid_control_cap:
        ಹಿಂತಿರುಗಿ decodeARMSIDControlInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಸ್ಥಿತಿ cap_sid_cap:
        ಹಿಂತಿರುಗಿ decodeARMSIDInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಸ್ಥಿತಿ cap_cb_control_cap:
        ಹಿಂತಿರುಗಿ decodeARMCBControlInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಸ್ಥಿತಿ cap_cb_cap:
        ಹಿಂತಿರುಗಿ decodeARMCBInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /*CONFIG_ARM_SMMU*/
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ALLOW_SMC_CALLS
    ಸ್ಥಿತಿ cap_smc_cap:
        ಹಿಂತಿರುಗಿ decodeARMSMCInvocation(label, ಉದ್ದಳತೆ, cap, call, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
#ಇಲ್ಲದಿದ್ದರೆ
{
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ decodeARMMMUInvocation(label, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
}
}

ಶೂನ್ಯ
Arch_prepareThreadDelete(tcb_t * thread) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಆದರೆ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    fpuRelease(thread);
#ಆದರೆ_ಅಂತ್ಯ
}
