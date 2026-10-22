/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <api/failures.h>
#समावेश <kernel/vspace.h>
#समावेश <object/structures.h>
#समावेश <arch/machine.h>
#समावेश <arch/model/statedata.h>
#समावेश <machine/fpu.h>
#समावेश <arch/object/objecttype.h>
#समावेश <arch/object/ioport.h>
#समावेश <plat/machine/devices.h>

#समावेश <arch/object/iospace.h>
#समावेश <arch/object/vcpu.h>
#समावेश <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_page_table_cap:
        अगर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_page_directory_cap:
        अगर (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_asid_control_cap:
    हाल cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
    हाल cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        वापस ret;
    हाल cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;

#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
    हाल cap_io_page_table_cap:
        अगर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;
#पूर्ण_अगर

#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        वापस ret;
    हाल cap_ept_pml4_cap:
        अगर (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;
    हाल cap_ept_pdpt_cap:
        अगर (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_ept_pd_cap:
        अगर (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;

    हाल cap_ept_pt_cap:
        अगर (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        वापस ret;
#पूर्ण_अगर

    शेष:
        वापस Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_update_Cap_तथ्य(bool_t preserve, word_t तथ्य, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#अगर_घोषित CONFIG_IOMMU
    चयन (cap_get_capType(cap)) {
    हाल cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_तथ्य_t w = { { तथ्य } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_तथ्य_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_तथ्य_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_तथ्य_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_तथ्य_get_domain_ID(w);
        अगर (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            वापस cap_io_space_cap_new(domainID, PCIDevice);
        } अन्यथा {
            वापस cap_null_cap_new();
        }
    }

    शेष:
        वापस cap;
    }
#पूर्ण_अगर
    वापस cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    अगर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        वापस cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        वापस cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    चयन (cap_get_capType(cap)) {
    हाल cap_page_directory_cap:
        अगर (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        अवरोध;

    हाल cap_asid_pool_cap:
        अगर (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        अवरोध;
    हाल cap_asid_control_cap:
    हाल cap_io_port_control_cap:
        अवरोध;
    हाल cap_io_port_cap:
#अगर_घोषित CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#पूर्ण_अगर
        अगर (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            वापस fc_ret;
        }
        अवरोध;
#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap:
        अगर (final) {
            unmapVTDContextEntry(cap);
        }
        अवरोध;

    हाल cap_io_page_table_cap:
        अगर (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        अवरोध;
#पूर्ण_अगर

#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap:
        अगर (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        अवरोध;
    हाल cap_ept_pml4_cap:
        अगर (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        अवरोध;

    हाल cap_ept_pdpt_cap:
        अगर (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        अवरोध;

    हाल cap_ept_pd_cap:
        अगर (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        अवरोध;

    हाल cap_ept_pt_cap:
        अगर (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        अवरोध;
#पूर्ण_अगर

    शेष:
        वापस Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    वापस fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    चयन (cap_get_capType(cap_a)) {
    हाल cap_frame_cap:
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(page_Bits_For_आकार(cap_frame_cap_get_capFSize(cap_b)));
            वापस ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        अवरोध;

    हाल cap_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_page_table_cap) {
            वापस cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_page_directory_cap:
        अगर (cap_get_capType(cap_b) == cap_page_directory_cap) {
            वापस cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_asid_control_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_asid_pool_cap:
        अगर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            वापस cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        अवरोध;

    हाल cap_io_port_control_cap:
        अगर (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            वापस true;
        }
        अवरोध;

    हाल cap_io_port_cap:
        अगर (cap_get_capType(cap_b) == cap_io_port_cap) {
            वापस  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        अवरोध;

#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap:
        अगर (cap_get_capType(cap_b) == cap_io_space_cap) {
            वापस cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        अवरोध;

    हाल cap_io_page_table_cap:
        अगर (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            वापस cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        अवरोध;
#पूर्ण_अगर

#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap:
        अगर (cap_get_capType(cap_b) == cap_vcpu_cap) {
            वापस cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        अवरोध;

    हाल cap_ept_pml4_cap:
        अगर (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            वापस cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        अवरोध;

    हाल cap_ept_pdpt_cap:
        अगर (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            वापस cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_ept_pd_cap:
        अगर (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            वापस cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        अवरोध;

    हाल cap_ept_pt_cap:
        अगर (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            वापस cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        अवरोध;

#पूर्ण_अगर

    }

    वापस Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    अगर (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        वापस false;
    }
    अगर (cap_get_capType(cap_a) == cap_frame_cap) {
        अगर (cap_get_capType(cap_b) == cap_frame_cap) {
            वापस ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    वापस Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_आकार(word_t t)
{
    चयन (t) {
    हाल seL4_X86_4K:
        वापस page_Bits_For_आकार(X86_SmallPage);
    हाल seL4_X86_LargePageObject:
        वापस page_Bits_For_आकार(X86_LargePage);
    हाल seL4_X86_PageTableObject:
        वापस seL4_PageTableBits;
    हाल seL4_X86_PageDirectoryObject:
        वापस seL4_PageDirBits;
    हाल seL4_IA32_PDPTObject:
        वापस seL4_PDPTBits;
    हाल seL4_X86_IOPageTableObject:
        वापस seL4_IOPageTableBits;
#अगर_घोषित CONFIG_VTX
    हाल seL4_X86_VCPUObject:
        वापस seL4_X86_VCPUBits;
    हाल seL4_X86_EPTPML4Object:
        वापस seL4_X86_EPTPML4Bits;
    हाल seL4_X86_EPTPDPTObject:
        वापस seL4_X86_EPTPDPTBits;
    हाल seL4_X86_EPTPDObject:
        वापस seL4_X86_EPTPDBits;
    हाल seL4_X86_EPTPTObject:
        वापस seL4_X86_EPTPTBits;
#पूर्ण_अगर
    शेष:
        वापस Mode_get_Object_आकार(t);
    }
}

cap_t Arch_createObject(object_t t, व्योम *regionBase, word_t user_आकार, bool_t deviceMemory)
{
#अगर_घोषित CONFIG_VTX
    चयन (t) {
    हाल seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        वापस cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    हाल seL4_X86_EPTPML4Object:
        वापस cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    हाल seL4_X86_EPTPDPTObject:
        वापस cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    हाल seL4_X86_EPTPDObject:
        वापस cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    हाल seL4_X86_EPTPTObject:
        वापस cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    शेष:
#पूर्ण_अगर
        वापस Mode_createObject(t, regionBase, user_आकार, deviceMemory);
#अगर_घोषित CONFIG_VTX
    }
#पूर्ण_अगर
}

exception_t Arch_decodeInvocation(
    word_t invLabel,
    word_t लंबाई,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *बफर
)
{
    चयन (cap_get_capType(cap)) {
    हाल cap_asid_control_cap:
    हाल cap_asid_pool_cap:
        वापस decodeX86MMUInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
    हाल cap_io_port_control_cap:
        वापस decodeX86PortControlInvocation(invLabel, लंबाई, cptr, slot, cap, बफर);
    हाल cap_io_port_cap:
        वापस decodeX86PortInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
#अगर_घोषित CONFIG_IOMMU
    हाल cap_io_space_cap:
        वापस decodeX86IOSpaceInvocation(invLabel, cap);
    हाल cap_io_page_table_cap:
        वापस decodeX86IOPTInvocation(invLabel, लंबाई, slot, cap, बफर);
#पूर्ण_अगर
#अगर_घोषित CONFIG_VTX
    हाल cap_vcpu_cap:
        वापस decodeX86VCPUInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
    हाल cap_ept_pml4_cap:
    हाल cap_ept_pdpt_cap:
    हाल cap_ept_pd_cap:
    हाल cap_ept_pt_cap:
        वापस decodeX86EPTInvocation(invLabel, लंबाई, cptr, slot, cap, बफर);
#पूर्ण_अगर
    शेष:
        वापस Mode_decodeInvocation(invLabel, लंबाई, cptr, slot, cap, call, बफर);
    }
}

व्योम Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

व्योम Arch_postCapDeletion(cap_t cap)
{
    अगर (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
