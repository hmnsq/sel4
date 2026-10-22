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
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/vcpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>

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

    ਹਾਲਤ cap_asid_control_cap:
    ਹਾਲਤ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_io_ਪੋਰਟ_control_cap:
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_io_ਪੋਰਟ_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap:
        ret.cap = cap;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_ept_pml4_cap:
        ਜੇ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;
    ਹਾਲਤ cap_ept_pdpt_cap:
        ਜੇ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ept_pd_cap:
        ਜੇ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ept_pt_cap:
        ਜੇ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਵਾਪਸ ਬਹੁਲਕ_derive_Cap(ਥਾਂ, cap);
    }
}

cap_t CONST Arch_update_Cap_ਅੰਕੜੇ(bool_t preserve, ਸ਼ਬਦ_t ਅੰਕੜੇ, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_ਅੰਕੜੇ_t w = { { ਅੰਕੜੇ } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_ਅੰਕੜੇ_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_ਅੰਕੜੇ_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_ਅੰਕੜੇ_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_ਅੰਕੜੇ_get_domain_ID(w);
        ਜੇ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= ਨਕਾਬ(x86KSnumIODomainIDBits)) {
            ਵਾਪਸ cap_io_space_cap_new(domainID, PCIDevice);
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ cap_null_cap_new();
        }
    }

    ਮੂਲ:
        ਵਾਪਸ cap;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ cap;
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
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
        ਜੇ (final && cap_ਪੰਨਾ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_ਪੰਨਾ_Directory(
                cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_ਪੰਨਾ_directory_cap_get_cap_PDMapped_ਪਤਾ(cap),
                PDE_PTR(cap_ਪੰਨਾ_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ਪੰਨਾ_ਸਾਰਣੀ(
                cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
                cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
                PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap:
        ਜੇ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_asid_control_cap:
    ਹਾਲਤ cap_io_ਪੋਰਟ_control_cap:
        ਰੋਕੋ;
    ਹਾਲਤ cap_io_ਪੋਰਟ_cap:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_ਪੋਰਟ_cap_get_cap_IOPort_VPID(cap),
                                cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap),
                                cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap));
#ਜੇ_ਅੰਤ
        ਜੇ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ਵਾਪਸ fc_ret;
        }
        ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap:
        ਜੇ (final) {
            unmap_VTDContext_ਇੰਦਰਾਜ(cap);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (final && cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_ਸਾਰਣੀ(cap);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap:
        ਜੇ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ਰੋਕੋ;
    ਹਾਲਤ cap_ept_pml4_cap:
        ਜੇ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pdpt_cap:
        ਜੇ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pd_cap:
        ਜੇ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_ਪਤਾ(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pt_cap:
        ਜੇ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_ਸਾਰਣੀ(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_ਪਤਾ(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        ਵਾਪਸ ਬਹੁਲਕ_finalise_Cap(cap, final);
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
            ਸਿਖਰ_B = botB + ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap_b)));
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

    ਹਾਲਤ cap_io_ਪੋਰਟ_control_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_ਪੋਰਟ_control_cap ||
            cap_get_capType(cap_b) == cap_io_ਪੋਰਟ_cap) {
            ਵਾਪਸ true;
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੋਰਟ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_ਪੋਰਟ_cap) {
            ਵਾਪਸ  cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap_a) ==
                    cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap_b) &&
                    cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap_a) ==
                    cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap_b);
        }
        ਰੋਕੋ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ਵਾਪਸ cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap) {
            ਵਾਪਸ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ਰੋਕੋ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ਵਾਪਸ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pml4_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ਵਾਪਸ cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pdpt_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ਵਾਪਸ cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pd_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ਵਾਪਸ cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ept_pt_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ਵਾਪਸ cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ਰੋਕੋ;

#ਜੇ_ਅੰਤ

    }

    ਵਾਪਸ ਬਹੁਲਕ_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ਜੇ (cap_get_capType(cap_a) == cap_io_ਪੋਰਟ_control_cap &&
        cap_get_capType(cap_b) == cap_io_ਪੋਰਟ_cap) {
        ਵਾਪਸ false;
    }
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
    ਵਾਪਸ Arch_sameRegionAs(cap_a, cap_b);
}

ਸ਼ਬਦ_t Arch_get_Object_ਮਾਪ(ਸ਼ਬਦ_t t)
{
    ਚੋਣ (t) {
    ਹਾਲਤ seL4_X86_4K:
        ਵਾਪਸ ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(X86_Small_ਪੰਨਾ);
    ਹਾਲਤ se_L4_X86_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(X86_Large_ਪੰਨਾ);
    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        ਵਾਪਸ se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ;
    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_Directory_Object:
        ਵਾਪਸ se_L4_ਪੰਨਾ_Dir_ਬਿੱਟ;
    ਹਾਲਤ seL4_IA32_PDPTObject:
        ਵਾਪਸ seL4_PDPTBits;
    ਹਾਲਤ se_L4_X86_IOPage_ਸਾਰਣੀ_Object:
        ਵਾਪਸ se_L4_IOPage_ਸਾਰਣੀ_ਬਿੱਟ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ seL4_X86_VCPUObject:
        ਵਾਪਸ seL4_X86_VCPUBits;
    ਹਾਲਤ seL4_X86_EPTPML4Object:
        ਵਾਪਸ se_L4_X86_EPTPML4_ਬਿੱਟ;
    ਹਾਲਤ seL4_X86_EPTPDPTObject:
        ਵਾਪਸ seL4_X86_EPTPDPTBits;
    ਹਾਲਤ seL4_X86_EPTPDObject:
        ਵਾਪਸ seL4_X86_EPTPDBits;
    ਹਾਲਤ seL4_X86_EPTPTObject:
        ਵਾਪਸ seL4_X86_EPTPTBits;
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਵਾਪਸ ਬਹੁਲਕ_get_Object_ਮਾਪ(t);
    }
}

cap_t Arch_createObject(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਚੋਣ (t) {
    ਹਾਲਤ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((ਸ਼ਬਦ_t)region_ਅਧਾਰ);
        vcpu_init(vcpu);
        ਵਾਪਸ cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    ਹਾਲਤ seL4_X86_EPTPML4Object:
        ਵਾਪਸ cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capPML4BasePtr       */
               );
    ਹਾਲਤ seL4_X86_EPTPDPTObject:
        ਵਾਪਸ cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ   /* capPDPTBasePtr      */
               );
    ਹਾਲਤ seL4_X86_EPTPDObject:
        ਵਾਪਸ cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capPDBasePtr         */
               );
    ਹਾਲਤ seL4_X86_EPTPTObject:
        ਵਾਪਸ cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ  /* capPTBasePtr         */
               );
    ਮੂਲ:
#ਜੇ_ਅੰਤ
        ਵਾਪਸ ਬਹੁਲਕ_create_Object(t, region_ਅਧਾਰ, user_ਮਾਪ, device_ਸਮ੍ਰਿਤੀ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    }
#ਜੇ_ਅੰਤ
}

exception_t Arch_decodeInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_asid_control_cap:
    ਹਾਲਤ cap_asid_pool_cap:
        ਵਾਪਸ decodeX86MMUInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਹਾਲਤ cap_io_ਪੋਰਟ_control_cap:
        ਵਾਪਸ decode_X86_ਪੋਰਟ_Control_Invocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, ਬਫਰ);
    ਹਾਲਤ cap_io_ਪੋਰਟ_cap:
        ਵਾਪਸ decode_X86_ਪੋਰਟ_Invocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ cap_io_space_cap:
        ਵਾਪਸ decodeX86IOSpaceInvocation(invLabel, cap);
    ਹਾਲਤ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decodeX86IOPTInvocation(invLabel, ਲੰਮਾਈ, ਥਾਂ, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ cap_vcpu_cap:
        ਵਾਪਸ decodeX86VCPUInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    ਹਾਲਤ cap_ept_pml4_cap:
    ਹਾਲਤ cap_ept_pdpt_cap:
    ਹਾਲਤ cap_ept_pd_cap:
    ਹਾਲਤ cap_ept_pt_cap:
        ਵਾਪਸ decodeX86EPTInvocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ
    ਮੂਲ:
        ਵਾਪਸ ਬਹੁਲਕ_decode_Invocation(invLabel, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);
    }
}

ਖਾਲੀ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

ਖਾਲੀ Arch_postCapDeletion(cap_t cap)
{
    ਜੇ (cap_get_capType(cap) == cap_io_ਪੋਰਟ_cap) {
        uint16_t ਪਹਿਲਾ_ਪੋਰਟ = cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਪਹਿਲਾ_ਪੋਰਟ(cap);
        uint16_t ਆਖਰੀ_ਪੋਰਟ = cap_io_ਪੋਰਟ_cap_get_cap_IOPort_ਆਖਰੀ_ਪੋਰਟ(cap);

        freeIOPortRange(ਪਹਿਲਾ_ਪੋਰਟ, ਆਖਰੀ_ਪੋਰਟ);
    }
}
