/*
 * Copyright 2014, General Dynamics C4 Systems
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
#ಅನ್ತರ್ಭಾವ <arch/machine/tlb.h>
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಆದರೆ_ಅಂತ್ಯ

bool_t Arch_is_ಚೌಕಟ್ಟು_Type(ಶಬ್ದ_t type)
{
    ಆಯ್ಕೆ (type) {
    ಸ್ಥಿತಿ se_L4_ARM_Small_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ se_L4_ARM_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ seL4_ARM_SectionObject:
        ಹಿಂತಿರುಗಿ true;
    ಸ್ಥಿತಿ seL4_ARM_SuperSectionObject:
        ಹಿಂತಿರುಗಿ true;
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *ಸ್ಥಾನ, cap_t cap)
{
    deriveCap_ret_t ret;

    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಆದರೆ (cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap:
        ret.cap = cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
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
    ಹಿಂತಿರುಗಿ cap;
}

cap_t CONST Arch_ಮುಖವಾಡ_Cap_Rights(seL4_CapRights_t cap_rights_ಮುಖವಾಡ, cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) == cap_small_ಚೌಕಟ್ಟು_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ಶಬ್ದ(
                        cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap));
        vm_rights = ಮುಖವಾಡ_VMRights(vm_rights, cap_rights_ಮುಖವಾಡ);
        ಹಿಂತಿರುಗಿ cap_small_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FVMRights(cap,
                                                    ಶಬ್ದ_From_VMRights(vm_rights));
    } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_get_capType(cap) == cap_ಚೌಕಟ್ಟು_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ಶಬ್ದ(
                        cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap));
        vm_rights = ಮುಖವಾಡ_VMRights(vm_rights, cap_rights_ಮುಖವಾಡ);
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FVMRights(cap,
                                              ಶಬ್ದ_From_VMRights(vm_rights));
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

    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಆದರೆ (final && cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ಪುಟ_ಕೋಷ್ಟಕ(
                cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
                cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
                PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap)));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
            ಆದರೆ (is_IOSpace_ಚೌಕಟ್ಟು_Cap(cap)) {
                unmapIOPage(cap);
                ನಿಲ್ಲಿಸು;
            }
#ಆದರೆ_ಅಂತ್ಯ

            unmap_ಪುಟ(ARMSmall_ಪುಟ,
                      cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                      cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                      (ಶೂನ್ಯ *)cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_LOG_ಬಫರ್
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap) == ARMSection)) {
                ಆದರೆ (pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_ಬಫರ್) {
                    ks_User_Log_ಬಫರ್ = 0;

                    /* Invalidate log page table entries */
                    clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *) armKSGlobalLogPT, se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು),
                                        addrFromKPPtr((ಶೂನ್ಯ *)&armKSGlobalLogPT[0]));

                    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_ಪುಟ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                      cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
                      (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));
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


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಆದರೆ (final) {
            clearIOPageDirectory(cap);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_ಕೋಷ್ಟಕ(cap);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಹಿಂತಿರುಗಿ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ಆಯ್ಕೆ (cap_get_capType(cap_a)) {
    ಸ್ಥಿತಿ cap_small_ಚೌಕಟ್ಟು_cap:
    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_small_ಚೌಕಟ್ಟು_cap ||
            cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {
            ಶಬ್ದ_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b);
            ತುದಿ_A = botA + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a)));
            ತುದಿ_B = botB + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(generic_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b))) ;
            ಹಿಂತಿರುಗಿ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
            ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಪುಟ_directory_cap) {
            ಹಿಂತಿರುಗಿ cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap_b);
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


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ಹಿಂತಿರುಗಿ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
            ಹಿಂತಿರುಗಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಹಿಂತಿರುಗಿ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಆದರೆ (cap_get_capType(cap_a) == cap_small_ಚೌಕಟ್ಟು_cap) {
        ಆದರೆ (cap_get_capType(cap_b) == cap_small_ಚೌಕಟ್ಟು_cap) {
            ಹಿಂತಿರುಗಿ ((cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {
            ಹಿಂತಿರುಗಿ false;
        }
    }
    ಆದರೆ (cap_get_capType(cap_a) == cap_ಚೌಕಟ್ಟು_cap) {
        ಆದರೆ (cap_get_capType(cap_b) == cap_ಚೌಕಟ್ಟು_cap) {
            ಹಿಂತಿರುಗಿ ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_a) ==
                     cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b)) &&
                    ((cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cap_get_capType(cap_b) == cap_small_ಚೌಕಟ್ಟು_cap) {
            ಹಿಂತಿರುಗಿ false;
        }
    }
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
    ಸ್ಥಿತಿ seL4_ARM_SectionObject:
        ಹಿಂತಿರುಗಿ ARMSection_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ seL4_ARM_SuperSectionObject:
        ಹಿಂತಿರುಗಿ ARMSuper_Section_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ PTE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_Directory_Object:
        ಹಿಂತಿರುಗಿ PDE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + PD_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ se_L4_ARM_IOPage_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ se_L4_IOPage_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_ARM_VCPUObject:
        ಹಿಂತಿರುಗಿ VCPU_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ಹಿಂತಿರುಗಿ cap_small_ಚೌಕಟ್ಟು_cap_new(
                   ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!device_ಸ್ಮರಣೆ,
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
                   0,
#ಆದರೆ_ಅಂತ್ಯ
                   ASID_ಮೇಲಿನ(asidInvalid),
                   (ಶಬ್ದ_t)region_ತಳ);

    ಸ್ಥಿತಿ se_L4_ARM_Large_ಪುಟ_Object:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMLarge_ಪುಟ)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   ARMLarge_ಪುಟ, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!device_ಸ್ಮರಣೆ, ASID_ಮೇಲಿನ(asidInvalid),
                   (ಶಬ್ದ_t)region_ತಳ);

    ಸ್ಥಿತಿ seL4_ARM_SectionObject:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSection)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   ARMSection, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!device_ಸ್ಮರಣೆ, ASID_ಮೇಲಿನ(asidInvalid),
                   (ಶಬ್ದ_t)region_ತಳ);

    ಸ್ಥಿತಿ seL4_ARM_SuperSectionObject:
        ಆದರೆ (device_ಸ್ಮರಣೆ) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } ಇಲ್ಲದಿದ್ದರೆ {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                                (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ARMSuperSection)),
                                addrFromPPtr(region_ತಳ));
        }
        ಹಿಂತಿರುಗಿ cap_ಚೌಕಟ್ಟು_cap_new(
                   ARMSuperSection, ASID_ಕೆಳಗಿನ(asidInvalid), VMReadWrite,
                   0, !!device_ಸ್ಮರಣೆ, ASID_ಮೇಲಿನ(asidInvalid),
                   (ಶಬ್ದ_t)region_ತಳ);

    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_ಕೋಷ್ಟಕ_Object:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((ಶಬ್ದ_t)region_ತಳ,
                            (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು),
                            addrFromPPtr(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(false, asidInvalid, 0,
                                      (ಶಬ್ದ_t)region_ತಳ);

    ಸ್ಥಿತಿ se_L4_ARM_ಪುಟ_Directory_Object:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#ಇಲ್ಲದಿದ್ದರೆ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_ತಳ);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((ಶಬ್ದ_t)region_ತಳ,
                            (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು),
                            addrFromPPtr(region_ತಳ));

        ಹಿಂತಿರುಗಿ cap_ಪುಟ_directory_cap_new(false, asidInvalid,
                                          (ಶಬ್ದ_t)region_ತಳ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_vcpu_cap_new(VCPU_REF(region_ತಳ));
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ se_L4_ARM_IOPage_ಕೋಷ್ಟಕ_Object:
        cleanCacheRange_RAM((ಶಬ್ದ_t)region_ತಳ,
                            (ಶಬ್ದ_t)region_ತಳ + ಮುಖವಾಡ(se_L4_IOPage_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು),
                            addrFromPPtr(region_ತಳ));
        ಹಿಂತಿರುಗಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_new(0, asidInvalid, (ಶಬ್ದ_t)region_ತಳ, 0);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(ಶಬ್ದ_t invLabel, ಶಬ್ದ_t ಉದ್ದಳತೆ, cptr_t cptr,
                                  cte_t *ಸ್ಥಾನ, cap_t cap,
                                  bool_t call, ಶಬ್ದ_t *ಬಫರ್)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ಆದರೆ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_TK1_SMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಹಿಂತಿರುಗಿ decodeARMIOSpaceInvocation(invLabel, cap);
    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decodeARMIOPTInvocation(invLabel, ಉದ್ದಳತೆ, ಸ್ಥಾನ, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಹಿಂತಿರುಗಿ decodeARMVCPUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_ENABLE_SMP_SUPPORT
    ಸ್ಥಿತಿ cap_sgi_signal_cap:
        ಹಿಂತಿರುಗಿ decodeSGISignalInvocation(invLabel, ಉದ್ದಳತೆ, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    ಪೂರ್ವನಿಯೋಜಿತ:
#ಇಲ್ಲದಿದ್ದರೆ
{
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ decodeARMMMUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
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

