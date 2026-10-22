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
#ಅನ್ತರ್ಭಾವ <machine/fpu.h>
#ಅನ್ತರ್ಭಾವ <arch/object/objecttype.h>
#ಅನ್ತರ್ಭಾವ <arch/object/ioport.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices.h>

#ಅನ್ತರ್ಭಾವ <arch/object/iospace.h>
#ಅನ್ತರ್ಭಾವ <arch/object/vcpu.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/intel-vtd.h>

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

    ಸ್ಥಿತಿ cap_asid_control_cap:
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_control_cap:
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ret.cap = cap;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_ept_pml4_cap:
        ಆದರೆ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;
    ಸ್ಥಿತಿ cap_ept_pdpt_cap:
        ಆದರೆ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ept_pd_cap:
        ಆದರೆ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;

    ಸ್ಥಿತಿ cap_ept_pt_cap:
        ಆದರೆ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        } ಇಲ್ಲದಿದ್ದರೆ {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        }
        ಹಿಂತಿರುಗಿ ret;
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_derive_Cap(ಸ್ಥಾನ, cap);
    }
}

cap_t CONST Arch_update_Cap_ದತ್ತಾಂಶ(bool_t preserve, ಶಬ್ದ_t ದತ್ತಾಂಶ, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_ದತ್ತಾಂಶ_t w = { { ದತ್ತಾಂಶ } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_ದತ್ತಾಂಶ_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_ದತ್ತಾಂಶ_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_ದತ್ತಾಂಶ_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_ದತ್ತಾಂಶ_get_domain_ID(w);
        ಆದರೆ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= ಮುಖವಾಡ(x86KSnumIODomainIDBits)) {
            ಹಿಂತಿರುಗಿ cap_io_space_cap_new(domainID, PCIDevice);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ cap_null_cap_new();
        }
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ cap;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ cap;
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
    ಸ್ಥಿತಿ cap_ಪುಟ_directory_cap:
        ಆದರೆ (final && cap_ಪುಟ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_ಪುಟ_Directory(
                cap_ಪುಟ_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_ಪುಟ_directory_cap_get_cap_PDMapped_ವಿಳಾಸ(cap),
                PDE_PTR(cap_ಪುಟ_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ಪುಟ_ಕೋಷ್ಟಕ(
                cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
                cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
                PT_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಆದರೆ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_asid_control_cap:
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_control_cap:
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_VPID(cap),
                                cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap),
                                cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap));
#ಆದರೆ_ಅಂತ್ಯ
        ಆದರೆ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ಹಿಂತಿರುಗಿ fc_ret;
        }
        ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಆದರೆ (final) {
            unmap_VTDContext_ನಮೂದು(cap);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (final && cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_ಕೋಷ್ಟಕ(cap);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಆದರೆ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ cap_ept_pml4_cap:
        ಆದರೆ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pdpt_cap:
        ಆದರೆ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_ವಿಳಾಸ(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pd_cap:
        ಆದರೆ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_ವಿಳಾಸ(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pt_cap:
        ಆದರೆ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_ಕೋಷ್ಟಕ(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_finalise_Cap(cap, final);
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
            ತುದಿ_B = botB + ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap_b)));
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

    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_control_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_ಪೋರ್ಟ್_control_cap ||
            cap_get_capType(cap_b) == cap_io_ಪೋರ್ಟ್_cap) {
            ಹಿಂತಿರುಗಿ true;
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_ಪೋರ್ಟ್_cap) {
            ಹಿಂತಿರುಗಿ  cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap_a) ==
                    cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap_b) &&
                    cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap_a) ==
                    cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap_b);
        }
        ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ಹಿಂತಿರುಗಿ cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap) {
            ಹಿಂತಿರುಗಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ಹಿಂತಿರುಗಿ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pml4_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ಹಿಂತಿರುಗಿ cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_ತಳ_Ptr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pdpt_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ಹಿಂತಿರುಗಿ cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pd_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ಹಿಂತಿರುಗಿ cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ cap_ept_pt_cap:
        ಆದರೆ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ಹಿಂತಿರುಗಿ cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ನಿಲ್ಲಿಸು;

#ಆದರೆ_ಅಂತ್ಯ

    }

    ಹಿಂತಿರುಗಿ ಬಹುಲಕ_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಆದರೆ (cap_get_capType(cap_a) == cap_io_ಪೋರ್ಟ್_control_cap &&
        cap_get_capType(cap_b) == cap_io_ಪೋರ್ಟ್_cap) {
        ಹಿಂತಿರುಗಿ false;
    }
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
    ಹಿಂತಿರುಗಿ Arch_sameRegionAs(cap_a, cap_b);
}

ಶಬ್ದ_t Arch_get_Object_ಗಾತ್ರ(ಶಬ್ದ_t t)
{
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ seL4_X86_4K:
        ಹಿಂತಿರುಗಿ ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(X86_Small_ಪುಟ);
    ಸ್ಥಿತಿ se_L4_X86_Large_ಪುಟ_Object:
        ಹಿಂತಿರುಗಿ ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(X86_Large_ಪುಟ);
    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ se_L4_X86_ಪುಟ_Directory_Object:
        ಹಿಂತಿರುಗಿ se_L4_ಪುಟ_Dir_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ seL4_IA32_PDPTObject:
        ಹಿಂತಿರುಗಿ seL4_PDPTBits;
    ಸ್ಥಿತಿ se_L4_X86_IOPage_ಕೋಷ್ಟಕ_Object:
        ಹಿಂತಿರುಗಿ se_L4_IOPage_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ seL4_X86_VCPUObject:
        ಹಿಂತಿರುಗಿ seL4_X86_VCPUBits;
    ಸ್ಥಿತಿ seL4_X86_EPTPML4Object:
        ಹಿಂತಿರುಗಿ se_L4_X86_EPTPML4_ಬಿಟ್ಟುಗಳು;
    ಸ್ಥಿತಿ seL4_X86_EPTPDPTObject:
        ಹಿಂತಿರುಗಿ seL4_X86_EPTPDPTBits;
    ಸ್ಥಿತಿ seL4_X86_EPTPDObject:
        ಹಿಂತಿರುಗಿ seL4_X86_EPTPDBits;
    ಸ್ಥಿತಿ seL4_X86_EPTPTObject:
        ಹಿಂತಿರುಗಿ seL4_X86_EPTPTBits;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_get_Object_ಗಾತ್ರ(t);
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *region_ತಳ, ಶಬ್ದ_t user_ಗಾತ್ರ, bool_t device_ಸ್ಮರಣೆ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಆಯ್ಕೆ (t) {
    ಸ್ಥಿತಿ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((ಶಬ್ದ_t)region_ತಳ);
        vcpu_init(vcpu);
        ಹಿಂತಿರುಗಿ cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    ಸ್ಥಿತಿ seL4_X86_EPTPML4Object:
        ಹಿಂತಿರುಗಿ cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ  /* capPML4BasePtr       */
               );
    ಸ್ಥಿತಿ seL4_X86_EPTPDPTObject:
        ಹಿಂತಿರುಗಿ cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (ಶಬ್ದ_t)region_ತಳ   /* capPDPTBasePtr      */
               );
    ಸ್ಥಿತಿ seL4_X86_EPTPDObject:
        ಹಿಂತಿರುಗಿ cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (ಶಬ್ದ_t)region_ತಳ  /* capPDBasePtr         */
               );
    ಸ್ಥಿತಿ seL4_X86_EPTPTObject:
        ಹಿಂತಿರುಗಿ cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (ಶಬ್ದ_t)region_ತಳ  /* capPTBasePtr         */
               );
    ಪೂರ್ವನಿಯೋಜಿತ:
#ಆದರೆ_ಅಂತ್ಯ
        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_create_Object(t, region_ತಳ, user_ಗಾತ್ರ, device_ಸ್ಮರಣೆ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    }
#ಆದರೆ_ಅಂತ್ಯ
}

exception_t Arch_decodeInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *ಸ್ಥಾನ,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {
    ಸ್ಥಿತಿ cap_asid_control_cap:
    ಸ್ಥಿತಿ cap_asid_pool_cap:
        ಹಿಂತಿರುಗಿ decodeX86MMUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_control_cap:
        ಹಿಂತಿರುಗಿ decode_X86_ಪೋರ್ಟ್_Control_Invocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, ಬಫರ್);
    ಸ್ಥಿತಿ cap_io_ಪೋರ್ಟ್_cap:
        ಹಿಂತಿರುಗಿ decode_X86_ಪೋರ್ಟ್_Invocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ cap_io_space_cap:
        ಹಿಂತಿರುಗಿ decodeX86IOSpaceInvocation(invLabel, cap);
    ಸ್ಥಿತಿ cap_io_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decodeX86IOPTInvocation(invLabel, ಉದ್ದಳತೆ, ಸ್ಥಾನ, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ cap_vcpu_cap:
        ಹಿಂತಿರುಗಿ decodeX86VCPUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    ಸ್ಥಿತಿ cap_ept_pml4_cap:
    ಸ್ಥಿತಿ cap_ept_pdpt_cap:
    ಸ್ಥಿತಿ cap_ept_pd_cap:
    ಸ್ಥಿತಿ cap_ept_pt_cap:
        ಹಿಂತಿರುಗಿ decodeX86EPTInvocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ ಬಹುಲಕ_decode_Invocation(invLabel, ಉದ್ದಳತೆ, cptr, ಸ್ಥಾನ, cap, call, ಬಫರ್);
    }
}

ಶೂನ್ಯ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

ಶೂನ್ಯ Arch_postCapDeletion(cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) == cap_io_ಪೋರ್ಟ್_cap) {
        uint16_t ಮೊದಲ_ಪೋರ್ಟ್ = cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಮೊದಲ_ಪೋರ್ಟ್(cap);
        uint16_t ಕೊನೆಯ_ಪೋರ್ಟ್ = cap_io_ಪೋರ್ಟ್_cap_get_cap_IOPort_ಕೊನೆಯ_ಪೋರ್ಟ್(cap);

        freeIOPortRange(ಮೊದಲ_ಪೋರ್ಟ್, ಕೊನೆಯ_ಪೋರ್ಟ್);
    }
}
