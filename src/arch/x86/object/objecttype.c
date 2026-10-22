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

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_asid_control_cap:
    ಪಕ್ಷ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;

#ifdef CONFIG_IOMMU
    ಪಕ್ಷ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;
#endif

#ifdef CONFIG_VTX
    ಪಕ್ಷ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_ept_pml4_cap:
        ಯದಿ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;
    ಪಕ್ಷ cap_ept_pdpt_cap:
        ಯದಿ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_ept_pd_cap:
        ಯದಿ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;

    ಪಕ್ಷ cap_ept_pt_cap:
        ಯದಿ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ಅನ್ಯಥಾ {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ಪ್ರತ್ಯಾಗಮ ret;
#endif

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        ಯದಿ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            ಪ್ರತ್ಯಾಗಮ cap_io_space_cap_new(domainID, PCIDevice);
        } ಅನ್ಯಥಾ {
            ಪ್ರತ್ಯಾಗಮ cap_null_cap_new();
        }
    }

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ cap;
    }
#endif
    ಪ್ರತ್ಯಾಗಮ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ಪ್ರತ್ಯಾಗಮ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ಅನ್ಯಥಾ {
        ಪ್ರತ್ಯಾಗಮ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_asid_control_cap:
    ಪಕ್ಷ cap_io_port_control_cap:
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        ಯದಿ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ಪ್ರತ್ಯಾಗಮ fc_ret;
        }
        ಭಙ್ಗ;
#ifdef CONFIG_IOMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಯದಿ (final) {
            unmapVTDContextEntry(cap);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ಭಙ್ಗ;
#endif

#ifdef CONFIG_VTX
    ಪಕ್ಷ cap_vcpu_cap:
        ಯದಿ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ಭಙ್ಗ;
    ಪಕ್ಷ cap_ept_pml4_cap:
        ಯದಿ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pdpt_cap:
        ಯದಿ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pd_cap:
        ಯದಿ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pt_cap:
        ಯದಿ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ಭಙ್ಗ;
#endif

    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ಪ್ರತ್ಯಾಗಮ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap_a)) {
    ಪಕ್ಷ cap_frame_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ತುದಿ_A, ತುದಿ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ತುದಿ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ತುದಿ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            ಪ್ರತ್ಯಾಗಮ ((botA <= botB) && (ತುದಿ_A >= ತುದಿ_B) && (botB <= ತುದಿ_B));
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_page_directory_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_asid_pool_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_port_control_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            ಪ್ರತ್ಯಾಗಮ true;
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_port_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_port_cap) {
            ಪ್ರತ್ಯಾಗಮ  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        ಭಙ್ಗ;

#ifdef CONFIG_IOMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_space_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_io_page_table_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;
#endif

#ifdef CONFIG_VTX
    ಪಕ್ಷ cap_vcpu_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pml4_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pdpt_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pd_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ಭಙ್ಗ;

    ಪಕ್ಷ cap_ept_pt_cap:
        ಯದಿ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ಪ್ರತ್ಯಾಗಮ cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ಭಙ್ಗ;

#endif

    }

    ಪ್ರತ್ಯಾಗಮ Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ಯದಿ (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಯದಿ (cap_get_capType(cap_a) == cap_frame_cap) {
        ಯದಿ (cap_get_capType(cap_b) == cap_frame_cap) {
            ಪ್ರತ್ಯಾಗಮ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    ಪ್ರತ್ಯಾಗಮ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_X86_4K:
        ಪ್ರತ್ಯಾಗಮ pageBitsForSize(X86_SmallPage);
    ಪಕ್ಷ seL4_X86_LargePageObject:
        ಪ್ರತ್ಯಾಗಮ pageBitsForSize(X86_LargePage);
    ಪಕ್ಷ seL4_X86_PageTableObject:
        ಪ್ರತ್ಯಾಗಮ seL4_PageTableBits;
    ಪಕ್ಷ seL4_X86_PageDirectoryObject:
        ಪ್ರತ್ಯಾಗಮ seL4_PageDirBits;
    ಪಕ್ಷ seL4_IA32_PDPTObject:
        ಪ್ರತ್ಯಾಗಮ seL4_PDPTBits;
    ಪಕ್ಷ seL4_X86_IOPageTableObject:
        ಪ್ರತ್ಯಾಗಮ seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    ಪಕ್ಷ seL4_X86_VCPUObject:
        ಪ್ರತ್ಯಾಗಮ seL4_X86_VCPUBits;
    ಪಕ್ಷ seL4_X86_EPTPML4Object:
        ಪ್ರತ್ಯಾಗಮ seL4_X86_EPTPML4Bits;
    ಪಕ್ಷ seL4_X86_EPTPDPTObject:
        ಪ್ರತ್ಯಾಗಮ seL4_X86_EPTPDPTBits;
    ಪಕ್ಷ seL4_X86_EPTPDObject:
        ಪ್ರತ್ಯಾಗಮ seL4_X86_EPTPDBits;
    ಪಕ್ಷ seL4_X86_EPTPTObject:
        ಪ್ರತ್ಯಾಗಮ seL4_X86_EPTPTBits;
#endif
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, ಶೂನ್ಯ *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    ವಿಕಲ್ಪ (t) {
    ಪಕ್ಷ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        ಪ್ರತ್ಯಾಗಮ cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    ಪಕ್ಷ seL4_X86_EPTPML4Object:
        ಪ್ರತ್ಯಾಗಮ cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    ಪಕ್ಷ seL4_X86_EPTPDPTObject:
        ಪ್ರತ್ಯಾಗಮ cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    ಪಕ್ಷ seL4_X86_EPTPDObject:
        ಪ್ರತ್ಯಾಗಮ cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    ಪಕ್ಷ seL4_X86_EPTPTObject:
        ಪ್ರತ್ಯಾಗಮ cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    ಸ್ವಭಾವ:
#endif
        ಪ್ರತ್ಯಾಗಮ Mode_createObject(t, regionBase, userSize, deviceMemory);
#ifdef CONFIG_VTX
    }
#endif
}

exception_t Arch_decodeInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    ವಿಕಲ್ಪ (cap_get_capType(cap)) {
    ಪಕ್ಷ cap_asid_control_cap:
    ಪಕ್ಷ cap_asid_pool_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ಪಕ್ಷ cap_io_port_control_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    ಪಕ್ಷ cap_io_port_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    ಪಕ್ಷ cap_io_space_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86IOSpaceInvocation(invLabel, cap);
    ಪಕ್ಷ cap_io_page_table_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    ಪಕ್ಷ cap_vcpu_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ಪಕ್ಷ cap_ept_pml4_cap:
    ಪಕ್ಷ cap_ept_pdpt_cap:
    ಪಕ್ಷ cap_ept_pd_cap:
    ಪಕ್ಷ cap_ept_pt_cap:
        ಪ್ರತ್ಯಾಗಮ decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    ಸ್ವಭಾವ:
        ಪ್ರತ್ಯಾಗಮ Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

ಶೂನ್ಯ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

ಶೂನ್ಯ Arch_postCapDeletion(cap_t cap)
{
    ಯದಿ (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
