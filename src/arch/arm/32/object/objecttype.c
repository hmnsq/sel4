/*
 * Copyright 2014, General Dynamics C4 Systems
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
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/tlb.h>
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਜੇ_ਅੰਤ

bool_t Arch_is_ਚੌਖਟਾ_Type(ਸ਼ਬਦ_t type)
{
    ਚੋਣ (type) {
    ਹਾਲਤ se_L4_ARM_Small_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_ARM_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਹਾਲਤ seL4_ARM_SectionObject:
        ਵਾਪਸ true;
    ਹਾਲਤ seL4_ARM_SuperSectionObject:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਜੇ (cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap:
        ret.cap = cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
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
    ਵਾਪਸ cap;
}

cap_t CONST Arch_ਨਕਾਬ_Cap_Rights(seL4_CapRights_t cap_rights_ਨਕਾਬ, cap_t cap)
{
    ਜੇ (cap_get_capType(cap) == cap_small_ਚੌਖਟਾ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ਸ਼ਬਦ(
                        cap_small_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap));
        vm_rights = ਨਕਾਬ_VMRights(vm_rights, cap_rights_ਨਕਾਬ);
        ਵਾਪਸ cap_small_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FVMRights(cap,
                                                    ਸ਼ਬਦ_From_VMRights(vm_rights));
    } ਨਹੀਂਤਾਂ ਜੇ (cap_get_capType(cap) == cap_ਚੌਖਟਾ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_ਸ਼ਬਦ(
                        cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap));
        vm_rights = ਨਕਾਬ_VMRights(vm_rights, cap_rights_ਨਕਾਬ);
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FVMRights(cap,
                                              ਸ਼ਬਦ_From_VMRights(vm_rights));
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

    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਜੇ (final && cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ਪੰਨਾ_ਸਾਰਣੀ(
                cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
                cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
                PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap)));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap:
        ਜੇ (cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
            ਜੇ (is_IOSpace_ਚੌਖਟਾ_Cap(cap)) {
                unmapIOPage(cap);
                ਰੋਕੋ;
            }
#ਜੇ_ਅੰਤ

            unmap_ਪੰਨਾ(ARMSmall_ਪੰਨਾ,
                      cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                      cap_small_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                      (ਖਾਲੀ *)cap_small_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_LOG_ਬਫਰ
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap) == ARMSection)) {
                ਜੇ (pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_ਬਫਰ) {
                    ks_User_Log_ਬਫਰ = 0;

                    /* Invalidate log page table entries */
                    clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *) armKSGlobalLogPT, se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ),
                                        addrFromKPPtr((ਖਾਲੀ *)&armKSGlobalLogPT[0]));

                    ਲਈ (ਪੂਰਨ idx = 0; idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_ਪੰਨਾ_ਬਿੱਟ));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#ਜੇ_ਅੰਤ /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_ਪੰਨਾ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                      cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                      (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));
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


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap:
        ਜੇ (final) {
            clearIOPageDirectory(cap);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_ਸਾਰਣੀ(cap);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਰੋਕੋ;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ਚੋਣ (cap_get_capType(cap_a)) {
    ਹਾਲਤ cap_small_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_small_ਚੌਖਟਾ_cap ||
            cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {
            ਸ਼ਬਦ_t botA, botB, ਸਿਖਰ_A, ਸਿਖਰ_B;
            botA = generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b);
            ਸਿਖਰ_A = botA + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a)));
            ਸਿਖਰ_B = botB + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(generic_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b))) ;
            ਵਾਪਸ ((botA <= botB) && (ਸਿਖਰ_A >= ਸਿਖਰ_B) && (botB <= ਸਿਖਰ_B));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
            ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ਪੰਨਾ_directory_cap) {
            ਵਾਪਸ cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap_b);
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


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ਵਾਪਸ cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
            ਵਾਪਸ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ
    }

    ਵਾਪਸ false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ਜੇ (cap_get_capType(cap_a) == cap_small_ਚੌਖਟਾ_cap) {
        ਜੇ (cap_get_capType(cap_b) == cap_small_ਚੌਖਟਾ_cap) {
            ਵਾਪਸ ((cap_small_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ਨਹੀਂਤਾਂ ਜੇ (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {
            ਵਾਪਸ false;
        }
    }
    ਜੇ (cap_get_capType(cap_a) == cap_ਚੌਖਟਾ_cap) {
        ਜੇ (cap_get_capType(cap_b) == cap_ਚੌਖਟਾ_cap) {
            ਵਾਪਸ ((cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_a) ==
                     cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b)) &&
                    ((cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ਨਹੀਂਤਾਂ ਜੇ (cap_get_capType(cap_b) == cap_small_ਚੌਖਟਾ_cap) {
            ਵਾਪਸ false;
        }
    }
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
    ਹਾਲਤ seL4_ARM_SectionObject:
        ਵਾਪਸ ARMSection_ਬਿੱਟ;
    ਹਾਲਤ seL4_ARM_SuperSectionObject:
        ਵਾਪਸ ARMSuper_Section_ਬਿੱਟ;
    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        ਵਾਪਸ PTE_ਮਾਪ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_Directory_Object:
        ਵਾਪਸ PDE_ਮਾਪ_ਬਿੱਟ + PD_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ se_L4_ARM_IOPage_ਸਾਰਣੀ_Object:
        ਵਾਪਸ se_L4_IOPage_ਸਾਰਣੀ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_ARM_VCPUObject:
        ਵਾਪਸ VCPU_ਮਾਪ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
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
        ਵਾਪਸ cap_small_ਚੌਖਟਾ_cap_new(
                   ASID_ਨੀਵਾਂ(asidInvalid), VMReadWrite,
                   0, !!device_ਸਮ੍ਰਿਤੀ,
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
                   0,
#ਜੇ_ਅੰਤ
                   ASID_ਉੱਚਾ(asidInvalid),
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ);

    ਹਾਲਤ se_L4_ARM_Large_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMLarge_ਪੰਨਾ)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   ARMLarge_ਪੰਨਾ, ASID_ਨੀਵਾਂ(asidInvalid), VMReadWrite,
                   0, !!device_ਸਮ੍ਰਿਤੀ, ASID_ਉੱਚਾ(asidInvalid),
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ);

    ਹਾਲਤ seL4_ARM_SectionObject:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSection)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   ARMSection, ASID_ਨੀਵਾਂ(asidInvalid), VMReadWrite,
                   0, !!device_ਸਮ੍ਰਿਤੀ, ASID_ਉੱਚਾ(asidInvalid),
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ);

    ਹਾਲਤ seL4_ARM_SuperSectionObject:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } ਨਹੀਂਤਾਂ {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                                (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ARMSuperSection)),
                                addrFromPPtr(region_ਅਧਾਰ));
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   ARMSuperSection, ASID_ਨੀਵਾਂ(asidInvalid), VMReadWrite,
                   0, !!device_ਸਮ੍ਰਿਤੀ, ASID_ਉੱਚਾ(asidInvalid),
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ);

    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_ਸਾਰਣੀ_Object:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                            (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ),
                            addrFromPPtr(region_ਅਧਾਰ));
        ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(false, asidInvalid, 0,
                                      (ਸ਼ਬਦ_t)region_ਅਧਾਰ);

    ਹਾਲਤ se_L4_ARM_ਪੰਨਾ_Directory_Object:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#ਨਹੀਂਤਾਂ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_ਅਧਾਰ);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                            (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ),
                            addrFromPPtr(region_ਅਧਾਰ));

        ਵਾਪਸ cap_ਪੰਨਾ_directory_cap_new(false, asidInvalid,
                                          (ਸ਼ਬਦ_t)region_ਅਧਾਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_ਅਧਾਰ));
        ਵਾਪਸ cap_vcpu_cap_new(VCPU_REF(region_ਅਧਾਰ));
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ se_L4_ARM_IOPage_ਸਾਰਣੀ_Object:
        cleanCacheRange_RAM((ਸ਼ਬਦ_t)region_ਅਧਾਰ,
                            (ਸ਼ਬਦ_t)region_ਅਧਾਰ + ਨਕਾਬ(se_L4_IOPage_ਸਾਰਣੀ_ਬਿੱਟ),
                            addrFromPPtr(region_ਅਧਾਰ));
        ਵਾਪਸ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(0, asidInvalid, (ਸ਼ਬਦ_t)region_ਅਧਾਰ, 0);
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(ਸ਼ਬਦ_t invLabel, ਸ਼ਬਦ_t ਲੰਮਾਈ, cptr_t cptr,
                                  cte_t *ਥਾਂ, cap_t cap,
                                  bool_t call, ਸ਼ਬਦ_t *ਬਫਰ)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ਜੇ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ਚੋਣ (cap_get_capType(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap:
        ਵਾਪਸ decodeARMIOSpaceInvocation(invLabel, cap);
    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decodeARMIOPTInvocation(invLabel, ਲੰਮਾਈ, ਥਾਂ, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap:
        ਵਾਪਸ decodeARMVCPUInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
#ਜੇ_ਅੰਤ /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_ENABLE_SMP_SUPPORT
    ਹਾਲਤ cap_sgi_signal_cap:
        ਵਾਪਸ decodeSGISignalInvocation(invLabel, ਲੰਮਾਈ, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    ਮੂਲ:
#ਨਹੀਂਤਾਂ
{
#ਜੇ_ਅੰਤ
    ਵਾਪਸ decodeARMMMUInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
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

