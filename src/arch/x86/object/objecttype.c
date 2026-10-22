/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <api/failures.h>
#अन्तर्भाव <kernel/vspace.h>
#अन्तर्भाव <object/structures.h>
#अन्तर्भाव <arch/machine.h>
#अन्तर्भाव <arch/model/statedata.h>
#अन्तर्भाव <machine/fpu.h>
#अन्तर्भाव <arch/object/objecttype.h>
#अन्तर्भाव <arch/object/ioport.h>
#अन्तर्भाव <plat/machine/devices.h>

#अन्तर्भाव <arch/object/iospace.h>
#अन्तर्भाव <arch/object/vcpu.h>
#अन्तर्भाव <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_सारणी_cap:
        यदि (cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_page_directory_cap:
        यदि (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_io_पत्तनम्_control_cap:
        ret.स्थितिः = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    पक्ष cap_io_पत्तनम्_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;

#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_io_page_सारणी_cap:
        यदि (cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap:
        ret.cap = cap;
        ret.स्थितिः = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_ept_pml4_cap:
        यदि (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
    पक्ष cap_ept_pdpt_cap:
        यदि (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_ept_pd_cap:
        यदि (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_ept_pt_cap:
        यदि (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.स्थितिः = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.स्थितिः = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
#यदि_अन्तः

    स्वभाव:
        प्रत्यागम बहुलकम्_derive_Cap(slot, cap);
    }
}

cap_t CONST Arch_update_Cap_दत्त(bool_t preserve, word_t दत्त, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#यदि_परिभाषितम् CONFIG_IOMMU
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_दत्त_t w = { { दत्त } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_दत्त_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_दत्त_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_दत्त_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_दत्त_get_domain_ID(w);
        यदि (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= आवरणम्(x86KSnumIODomainIDBits)) {
            प्रत्यागम cap_io_space_cap_new(domainID, PCIDevice);
        } अन्यथा {
            प्रत्यागम cap_null_cap_new();
        }
    }

    स्वभाव:
        प्रत्यागम cap;
    }
#यदि_अन्तः
    प्रत्यागम cap;
}

cap_t CONST Arch_आवरणम्_Cap_Rights(seL4_CapRights_t cap_rights_आवरणम्, cap_t cap)
{
    यदि (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = आवरणम्_VMRights(vm_rights, cap_rights_आवरणम्);
        प्रत्यागम cap_frame_cap_समुच्चयः_cap_FVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        प्रत्यागम cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
        यदि (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_cap_PDMapped_स्थानाङ्कः(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (final && cap_page_सारणी_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_Page_सारणी(
                cap_page_सारणी_cap_get_cap_PTMapped_ASID(cap),
                cap_page_सारणी_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
                PT_PTR(cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        यदि (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        भङ्ग;
    पक्ष cap_asid_control_cap:
    पक्ष cap_io_पत्तनम्_control_cap:
        भङ्ग;
    पक्ष cap_io_पत्तनम्_cap:
#यदि_परिभाषितम् CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_पत्तनम्_cap_get_cap_IOPort_VPID(cap),
                                cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap),
                                cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap));
#यदि_अन्तः
        यदि (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            प्रत्यागम fc_ret;
        }
        भङ्ग;
#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        यदि (final) {
            unmap_VTDContext_प्रविष्टिः(cap);
        }
        भङ्ग;

    पक्ष cap_io_page_सारणी_cap:
        यदि (final && cap_io_page_सारणी_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_सारणी(cap);
        }
        भङ्ग;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap:
        यदि (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        भङ्ग;
    पक्ष cap_ept_pml4_cap:
        यदि (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pdpt_cap:
        यदि (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_स्थानाङ्कः(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pd_cap:
        यदि (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_स्थानाङ्कः(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pt_cap:
        यदि (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_सारणी(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_स्थानाङ्कः(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        भङ्ग;
#यदि_अन्तः

    स्वभाव:
        प्रत्यागम बहुलकम्_finalise_Cap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_frame_cap:
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + आवरणम्(page_द्वयङ्काः_For_आकार(cap_frame_cap_get_capFSize(cap_b)));
            प्रत्यागम ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_page_सारणी_cap) {
            प्रत्यागम cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_page_सारणी_cap_get_cap_PTBase_Ptr(cap_b);
        }
        भङ्ग;

    पक्ष cap_page_directory_cap:
        यदि (cap_get_capType(cap_b) == cap_page_directory_cap) {
            प्रत्यागम cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_asid_control_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        यदि (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            प्रत्यागम cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_पत्तनम्_control_cap:
        यदि (cap_get_capType(cap_b) == cap_io_पत्तनम्_control_cap ||
            cap_get_capType(cap_b) == cap_io_पत्तनम्_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_io_पत्तनम्_cap:
        यदि (cap_get_capType(cap_b) == cap_io_पत्तनम्_cap) {
            प्रत्यागम  cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap_a) ==
                    cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap_b) &&
                    cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap_a) ==
                    cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap_b);
        }
        भङ्ग;

#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        यदि (cap_get_capType(cap_b) == cap_io_space_cap) {
            प्रत्यागम cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_page_सारणी_cap:
        यदि (cap_get_capType(cap_b) == cap_io_page_सारणी_cap) {
            प्रत्यागम cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_page_सारणी_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        भङ्ग;
#यदि_अन्तः

#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap:
        यदि (cap_get_capType(cap_b) == cap_vcpu_cap) {
            प्रत्यागम cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pml4_cap:
        यदि (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            प्रत्यागम cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_आधारः_Ptr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pdpt_cap:
        यदि (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            प्रत्यागम cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pd_cap:
        यदि (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            प्रत्यागम cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pt_cap:
        यदि (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            प्रत्यागम cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        भङ्ग;

#यदि_अन्तः

    }

    प्रत्यागम बहुलकम्_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    यदि (cap_get_capType(cap_a) == cap_io_पत्तनम्_control_cap &&
        cap_get_capType(cap_b) == cap_io_पत्तनम्_cap) {
        प्रत्यागम false;
    }
    यदि (cap_get_capType(cap_a) == cap_frame_cap) {
        यदि (cap_get_capType(cap_b) == cap_frame_cap) {
            प्रत्यागम ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    प्रत्यागम Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_X86_4K:
        प्रत्यागम page_द्वयङ्काः_For_आकार(X86_SmallPage);
    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम page_द्वयङ्काः_For_आकार(X86_LargePage);
    पक्ष se_L4_X86_Page_सारणी_Object:
        प्रत्यागम se_L4_Page_सारणी_द्वयङ्काः;
    पक्ष seL4_X86_PageDirectoryObject:
        प्रत्यागम se_L4_Page_Dir_द्वयङ्काः;
    पक्ष seL4_IA32_PDPTObject:
        प्रत्यागम seL4_PDPTBits;
    पक्ष se_L4_X86_IOPage_सारणी_Object:
        प्रत्यागम se_L4_IOPage_सारणी_द्वयङ्काः;
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष seL4_X86_VCPUObject:
        प्रत्यागम seL4_X86_VCPUBits;
    पक्ष seL4_X86_EPTPML4Object:
        प्रत्यागम se_L4_X86_EPTPML4_द्वयङ्काः;
    पक्ष seL4_X86_EPTPDPTObject:
        प्रत्यागम seL4_X86_EPTPDPTBits;
    पक्ष seL4_X86_EPTPDObject:
        प्रत्यागम seL4_X86_EPTPDBits;
    पक्ष seL4_X86_EPTPTObject:
        प्रत्यागम seL4_X86_EPTPTBits;
#यदि_अन्तः
    स्वभाव:
        प्रत्यागम बहुलकम्_get_Object_आकार(t);
    }
}

cap_t Arch_createObject(object_t t, शून्य *region_आधारः, word_t user_आकार, bool_t device_स्मृतिः)
{
#यदि_परिभाषितम् CONFIG_VTX
    विकल्प (t) {
    पक्ष seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)region_आधारः);
        vcpu_init(vcpu);
        प्रत्यागम cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    पक्ष seL4_X86_EPTPML4Object:
        प्रत्यागम cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)region_आधारः  /* capPML4BasePtr       */
               );
    पक्ष seL4_X86_EPTPDPTObject:
        प्रत्यागम cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)region_आधारः   /* capPDPTBasePtr      */
               );
    पक्ष seL4_X86_EPTPDObject:
        प्रत्यागम cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)region_आधारः  /* capPDBasePtr         */
               );
    पक्ष seL4_X86_EPTPTObject:
        प्रत्यागम cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)region_आधारः  /* capPTBasePtr         */
               );
    स्वभाव:
#यदि_अन्तः
        प्रत्यागम बहुलकम्_create_Object(t, region_आधारः, user_आकार, device_स्मृतिः);
#यदि_परिभाषितम् CONFIG_VTX
    }
#यदि_अन्तः
}

exception_t Arch_decodeInvocation(
    word_t invLabel,
    word_t दीर्घता,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        प्रत्यागम decodeX86MMUInvocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
    पक्ष cap_io_पत्तनम्_control_cap:
        प्रत्यागम decode_X86_पत्तनम्_Control_Invocation(invLabel, दीर्घता, cptr, slot, cap, बफर);
    पक्ष cap_io_पत्तनम्_cap:
        प्रत्यागम decode_X86_पत्तनम्_Invocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
#यदि_परिभाषितम् CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        प्रत्यागम decodeX86IOSpaceInvocation(invLabel, cap);
    पक्ष cap_io_page_सारणी_cap:
        प्रत्यागम decodeX86IOPTInvocation(invLabel, दीर्घता, slot, cap, बफर);
#यदि_अन्तः
#यदि_परिभाषितम् CONFIG_VTX
    पक्ष cap_vcpu_cap:
        प्रत्यागम decodeX86VCPUInvocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
    पक्ष cap_ept_pml4_cap:
    पक्ष cap_ept_pdpt_cap:
    पक्ष cap_ept_pd_cap:
    पक्ष cap_ept_pt_cap:
        प्रत्यागम decodeX86EPTInvocation(invLabel, दीर्घता, cptr, slot, cap, बफर);
#यदि_अन्तः
    स्वभाव:
        प्रत्यागम बहुलकम्_decode_Invocation(invLabel, दीर्घता, cptr, slot, cap, call, बफर);
    }
}

शून्य Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

शून्य Arch_postCapDeletion(cap_t cap)
{
    यदि (cap_get_capType(cap) == cap_io_पत्तनम्_cap) {
        uint16_t first_पत्तनम् = cap_io_पत्तनम्_cap_get_cap_IOPort_First_पत्तनम्(cap);
        uint16_t चरमः_पत्तनम् = cap_io_पत्तनम्_cap_get_cap_IOPort_चरमः_पत्तनम्(cap);

        freeIOPortRange(first_पत्तनम्, चरमः_पत्तनम्);
    }
}
