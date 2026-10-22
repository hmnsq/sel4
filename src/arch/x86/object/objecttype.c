/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>
#ଅନ୍ତର୍ଭାଵ <types.h>
#ଅନ୍ତର୍ଭାଵ <api/failures.h>
#ଅନ୍ତର୍ଭାଵ <kernel/vspace.h>
#ଅନ୍ତର୍ଭାଵ <object/structures.h>
#ଅନ୍ତର୍ଭାଵ <arch/machine.h>
#ଅନ୍ତର୍ଭାଵ <arch/model/statedata.h>
#ଅନ୍ତର୍ଭାଵ <machine/fpu.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/objecttype.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/ioport.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/devices.h>

#ଅନ୍ତର୍ଭାଵ <arch/object/iospace.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/vcpu.h>
#ଅନ୍ତର୍ଭାଵ <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_asid_control_cap:
    ସ୍ଥିତି cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;

#ifdef CONFIG_IOMMU
    ସ୍ଥିତି cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;
#endif

#ifdef CONFIG_VTX
    ସ୍ଥିତି cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_ept_pml4_cap:
        ଯଦି (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;
    ସ୍ଥିତି cap_ept_pdpt_cap:
        ଯଦି (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_ept_pd_cap:
        ଯଦି (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;

    ସ୍ଥିତି cap_ept_pt_cap:
        ଯଦି (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } ଅନ୍ୟଥା {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ଫେରନ୍ତୁ ret;
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        ଯଦି (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            ଫେରନ୍ତୁ cap_io_space_cap_new(domainID, PCIDevice);
        } ଅନ୍ୟଥା {
            ଫେରନ୍ତୁ cap_null_cap_new();
        }
    }

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ cap;
    }
#endif
    ଫେରନ୍ତୁ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    ଯଦି (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ଫେରନ୍ତୁ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } ଅନ୍ୟଥା {
        ଫେରନ୍ତୁ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_asid_control_cap:
    ସ୍ଥିତି cap_io_port_control_cap:
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        ଯଦି (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ଫେରନ୍ତୁ fc_ret;
        }
        ଭାଙ୍ଗ;
#ifdef CONFIG_IOMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଯଦି (final) {
            unmapVTDContextEntry(cap);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ଭାଙ୍ଗ;
#endif

#ifdef CONFIG_VTX
    ସ୍ଥିତି cap_vcpu_cap:
        ଯଦି (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ଭାଙ୍ଗ;
    ସ୍ଥିତି cap_ept_pml4_cap:
        ଯଦି (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pdpt_cap:
        ଯଦି (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pd_cap:
        ଯଦି (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pt_cap:
        ଯଦି (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ଭାଙ୍ଗ;
#endif

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ଫେରନ୍ତୁ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ବାଛ (cap_get_capType(cap_a)) {
    ସ୍ଥିତି cap_frame_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ଶୀର୍ଷ_A, ଶୀର୍ଷ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ଶୀର୍ଷ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ଶୀର୍ଷ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            ଫେରନ୍ତୁ ((botA <= botB) && (ଶୀର୍ଷ_A >= ଶୀର୍ଷ_B) && (botB <= ଶୀର୍ଷ_B));
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_table_cap) {
            ଫେରନ୍ତୁ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_page_directory_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ଫେରନ୍ତୁ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_asid_pool_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ଫେରନ୍ତୁ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_port_control_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            ଫେରନ୍ତୁ true;
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_port_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_port_cap) {
            ଫେରନ୍ତୁ  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        ଭାଙ୍ଗ;

#ifdef CONFIG_IOMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_space_cap) {
            ଫେରନ୍ତୁ cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_io_page_table_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ଫେରନ୍ତୁ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;
#endif

#ifdef CONFIG_VTX
    ସ୍ଥିତି cap_vcpu_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ଫେରନ୍ତୁ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pml4_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ଫେରନ୍ତୁ cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pdpt_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ଫେରନ୍ତୁ cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pd_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ଫେରନ୍ତୁ cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

    ସ୍ଥିତି cap_ept_pt_cap:
        ଯଦି (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ଫେରନ୍ତୁ cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ଭାଙ୍ଗ;

#endif

    }

    ଫେରନ୍ତୁ Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ଯଦି (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        ଫେରନ୍ତୁ false;
    }
    ଯଦି (cap_get_capType(cap_a) == cap_frame_cap) {
        ଯଦି (cap_get_capType(cap_b) == cap_frame_cap) {
            ଫେରନ୍ତୁ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    ଫେରନ୍ତୁ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    ବାଛ (t) {
    ସ୍ଥିତି seL4_X86_4K:
        ଫେରନ୍ତୁ pageBitsForSize(X86_SmallPage);
    ସ୍ଥିତି seL4_X86_LargePageObject:
        ଫେରନ୍ତୁ pageBitsForSize(X86_LargePage);
    ସ୍ଥିତି seL4_X86_PageTableObject:
        ଫେରନ୍ତୁ seL4_PageTableBits;
    ସ୍ଥିତି seL4_X86_PageDirectoryObject:
        ଫେରନ୍ତୁ seL4_PageDirBits;
    ସ୍ଥିତି seL4_IA32_PDPTObject:
        ଫେରନ୍ତୁ seL4_PDPTBits;
    ସ୍ଥିତି seL4_X86_IOPageTableObject:
        ଫେରନ୍ତୁ seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    ସ୍ଥିତି seL4_X86_VCPUObject:
        ଫେରନ୍ତୁ seL4_X86_VCPUBits;
    ସ୍ଥିତି seL4_X86_EPTPML4Object:
        ଫେରନ୍ତୁ seL4_X86_EPTPML4Bits;
    ସ୍ଥିତି seL4_X86_EPTPDPTObject:
        ଫେରନ୍ତୁ seL4_X86_EPTPDPTBits;
    ସ୍ଥିତି seL4_X86_EPTPDObject:
        ଫେରନ୍ତୁ seL4_X86_EPTPDBits;
    ସ୍ଥିତି seL4_X86_EPTPTObject:
        ଫେରନ୍ତୁ seL4_X86_EPTPTBits;
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, ଶୂନ୍ୟ *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    ବାଛ (t) {
    ସ୍ଥିତି seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        ଫେରନ୍ତୁ cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    ସ୍ଥିତି seL4_X86_EPTPML4Object:
        ଫେରନ୍ତୁ cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    ସ୍ଥିତି seL4_X86_EPTPDPTObject:
        ଫେରନ୍ତୁ cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    ସ୍ଥିତି seL4_X86_EPTPDObject:
        ଫେରନ୍ତୁ cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    ସ୍ଥିତି seL4_X86_EPTPTObject:
        ଫେରନ୍ତୁ cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
#endif
        ଫେରନ୍ତୁ Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    ବାଛ (cap_get_capType(cap)) {
    ସ୍ଥିତି cap_asid_control_cap:
    ସ୍ଥିତି cap_asid_pool_cap:
        ଫେରନ୍ତୁ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ସ୍ଥିତି cap_io_port_control_cap:
        ଫେରନ୍ତୁ decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    ସ୍ଥିତି cap_io_port_cap:
        ଫେରନ୍ତୁ decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    ସ୍ଥିତି cap_io_space_cap:
        ଫେରନ୍ତୁ decodeX86IOSpaceInvocation(invLabel, cap);
    ସ୍ଥିତି cap_io_page_table_cap:
        ଫେରନ୍ତୁ decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    ସ୍ଥିତି cap_vcpu_cap:
        ଫେରନ୍ତୁ decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    ସ୍ଥିତି cap_ept_pml4_cap:
    ସ୍ଥିତି cap_ept_pdpt_cap:
    ସ୍ଥିତି cap_ept_pd_cap:
    ସ୍ଥିତି cap_ept_pt_cap:
        ଫେରନ୍ତୁ decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଫେରନ୍ତୁ Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

ଶୂନ୍ୟ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

ଶୂନ୍ୟ Arch_postCapDeletion(cap_t cap)
{
    ଯଦି (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
