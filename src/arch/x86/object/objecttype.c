/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <config.h>
#অন্তর্ভাব <types.h>
#অন্তর্ভাব <api/failures.h>
#অন্তর্ভাব <kernel/vspace.h>
#অন্তর্ভাব <object/structures.h>
#অন্তর্ভাব <arch/machine.h>
#অন্তর্ভাব <arch/model/statedata.h>
#অন্তর্ভাব <machine/fpu.h>
#অন্তর্ভাব <arch/object/objecttype.h>
#অন্তর্ভাব <arch/object/ioport.h>
#অন্তর্ভাব <plat/machine/devices.h>

#অন্তর্ভাব <arch/object/iospace.h>
#অন্তর্ভাব <arch/object/vcpu.h>
#অন্তর্ভাব <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_page_directory_cap:
        যদি (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_asid_control_cap:
    ক্ষেত্র cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    ক্ষেত্র cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ফেরত ret;
    ক্ষেত্র cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;

#ifdef CONFIG_IOMMU
    ক্ষেত্র cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    ক্ষেত্র cap_io_page_table_cap:
        যদি (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;
#endif

#ifdef CONFIG_VTX
    ক্ষেত্র cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ফেরত ret;
    ক্ষেত্র cap_ept_pml4_cap:
        যদি (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;
    ক্ষেত্র cap_ept_pdpt_cap:
        যদি (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_ept_pd_cap:
        যদি (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;

    ক্ষেত্র cap_ept_pt_cap:
        যদি (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ফেরত ret;
#endif

    পূর্বনির্ধারিত:
        ফেরত Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_update_Cap_তথ্য(bool_t preserve, word_t তথ্য, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_তথ্য_t w = { { তথ্য } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_তথ্য_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_তথ্য_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_তথ্য_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_তথ্য_get_domain_ID(w);
        যদি (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            ফেরত cap_io_space_cap_new(domainID, PCIDevice);
        } অন্যথা {
            ফেরত cap_null_cap_new();
        }
    }

    পূর্বনির্ধারিত:
        ফেরত cap;
    }
#endif
    ফেরত cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ফেরত cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } অন্যথা {
        ফেরত cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_page_directory_cap:
        যদি (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap:
        যদি (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        বিরতি;
    ক্ষেত্র cap_asid_control_cap:
    ক্ষেত্র cap_io_port_control_cap:
        বিরতি;
    ক্ষেত্র cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        যদি (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ফেরত fc_ret;
        }
        বিরতি;
#ifdef CONFIG_IOMMU
    ক্ষেত্র cap_io_space_cap:
        যদি (final) {
            unmapVTDContextEntry(cap);
        }
        বিরতি;

    ক্ষেত্র cap_io_page_table_cap:
        যদি (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        বিরতি;
#endif

#ifdef CONFIG_VTX
    ক্ষেত্র cap_vcpu_cap:
        যদি (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        বিরতি;
    ক্ষেত্র cap_ept_pml4_cap:
        যদি (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        বিরতি;

    ক্ষেত্র cap_ept_pdpt_cap:
        যদি (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        বিরতি;

    ক্ষেত্র cap_ept_pd_cap:
        যদি (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        বিরতি;

    ক্ষেত্র cap_ept_pt_cap:
        যদি (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        বিরতি;
#endif

    পূর্বনির্ধারিত:
        ফেরত Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ফেরত fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    নির্বাচন (cap_get_capType(cap_a)) {
    ক্ষেত্র cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, শীর্ষ_A, শীর্ষ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            শীর্ষ_A = botA + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_a)));
            শীর্ষ_B = botB + MASK(page_Bits_For_আকার(cap_frame_cap_get_capFSize(cap_b)));
            ফেরত ((botA <= botB) && (শীর্ষ_A >= শীর্ষ_B) && (botB <= শীর্ষ_B));
        }
        বিরতি;

    ক্ষেত্র cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            ফেরত cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_page_directory_cap:
        যদি (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ফেরত cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_asid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_asid_pool_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ফেরত cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_io_port_control_cap:
        যদি (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            ফেরত true;
        }
        বিরতি;

    ক্ষেত্র cap_io_port_cap:
        যদি (cap_get_capType(cap_b) == cap_io_port_cap) {
            ফেরত  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        বিরতি;

#ifdef CONFIG_IOMMU
    ক্ষেত্র cap_io_space_cap:
        যদি (cap_get_capType(cap_b) == cap_io_space_cap) {
            ফেরত cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_io_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ফেরত cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        বিরতি;
#endif

#ifdef CONFIG_VTX
    ক্ষেত্র cap_vcpu_cap:
        যদি (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ফেরত cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_ept_pml4_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ফেরত cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_ept_pdpt_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ফেরত cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_ept_pd_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ফেরত cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        বিরতি;

    ক্ষেত্র cap_ept_pt_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ফেরত cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        বিরতি;

#endif

    }

    ফেরত Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        ফেরত false;
    }
    যদি (cap_get_capType(cap_a) == cap_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            ফেরত ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    ফেরত Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_get_Object_আকার(word_t t)
{
    নির্বাচন (t) {
    ক্ষেত্র seL4_X86_4K:
        ফেরত page_Bits_For_আকার(X86_SmallPage);
    ক্ষেত্র seL4_X86_LargePageObject:
        ফেরত page_Bits_For_আকার(X86_LargePage);
    ক্ষেত্র seL4_X86_PageTableObject:
        ফেরত seL4_PageTableBits;
    ক্ষেত্র seL4_X86_PageDirectoryObject:
        ফেরত seL4_PageDirBits;
    ক্ষেত্র seL4_IA32_PDPTObject:
        ফেরত seL4_PDPTBits;
    ক্ষেত্র seL4_X86_IOPageTableObject:
        ফেরত seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    ক্ষেত্র seL4_X86_VCPUObject:
        ফেরত seL4_X86_VCPUBits;
    ক্ষেত্র seL4_X86_EPTPML4Object:
        ফেরত seL4_X86_EPTPML4Bits;
    ক্ষেত্র seL4_X86_EPTPDPTObject:
        ফেরত seL4_X86_EPTPDPTBits;
    ক্ষেত্র seL4_X86_EPTPDObject:
        ফেরত seL4_X86_EPTPDBits;
    ক্ষেত্র seL4_X86_EPTPTObject:
        ফেরত seL4_X86_EPTPTBits;
#endif
    পূর্বনির্ধারিত:
        ফেরত Mode_get_Object_আকার(t);
    }
}

cap_t Arch_createObject(object_t t, শূন্য *regionBase, word_t user_আকার, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    নির্বাচন (t) {
    ক্ষেত্র seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        ফেরত cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    ক্ষেত্র seL4_X86_EPTPML4Object:
        ফেরত cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    ক্ষেত্র seL4_X86_EPTPDPTObject:
        ফেরত cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    ক্ষেত্র seL4_X86_EPTPDObject:
        ফেরত cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    ক্ষেত্র seL4_X86_EPTPTObject:
        ফেরত cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    পূর্বনির্ধারিত:
#endif
        ফেরত Mode_createObject(t, regionBase, user_আকার, deviceMemory);
#ifdef CONFIG_VTX
    }
#endif
}

exception_t Arch_decodeInvocation(
    word_t invLabel,
    word_t দৈর্ঘ্য,
    cptr_t cptr,
    cte_t *slot,
    cap_t cap,
    bool_t call,
    word_t *বাফার
)
{
    নির্বাচন (cap_get_capType(cap)) {
    ক্ষেত্র cap_asid_control_cap:
    ক্ষেত্র cap_asid_pool_cap:
        ফেরত decodeX86MMUInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    ক্ষেত্র cap_io_port_control_cap:
        ফেরত decodeX86PortControlInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, বাফার);
    ক্ষেত্র cap_io_port_cap:
        ফেরত decodeX86PortInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
#ifdef CONFIG_IOMMU
    ক্ষেত্র cap_io_space_cap:
        ফেরত decodeX86IOSpaceInvocation(invLabel, cap);
    ক্ষেত্র cap_io_page_table_cap:
        ফেরত decodeX86IOPTInvocation(invLabel, দৈর্ঘ্য, slot, cap, বাফার);
#endif
#ifdef CONFIG_VTX
    ক্ষেত্র cap_vcpu_cap:
        ফেরত decodeX86VCPUInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    ক্ষেত্র cap_ept_pml4_cap:
    ক্ষেত্র cap_ept_pdpt_cap:
    ক্ষেত্র cap_ept_pd_cap:
    ক্ষেত্র cap_ept_pt_cap:
        ফেরত decodeX86EPTInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, বাফার);
#endif
    পূর্বনির্ধারিত:
        ফেরত Mode_decodeInvocation(invLabel, দৈর্ঘ্য, cptr, slot, cap, call, বাফার);
    }
}

শূন্য Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

শূন্য Arch_postCapDeletion(cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
