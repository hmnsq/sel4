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

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_page_table_cap:
        যদি (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_page_directory_cap:
        যদি (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_asid_control_cap:
    পক্ষ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        প্রত্যাগম ret;
    পক্ষ cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        প্রত্যাগম ret;
    পক্ষ cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        প্রত্যাগম ret;

#ifdef CONFIG_IOMMU
    পক্ষ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        প্রত্যাগম ret;
    পক্ষ cap_io_page_table_cap:
        যদি (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;
#endif

#ifdef CONFIG_VTX
    পক্ষ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        প্রত্যাগম ret;
    পক্ষ cap_ept_pml4_cap:
        যদি (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;
    পক্ষ cap_ept_pdpt_cap:
        যদি (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_ept_pd_cap:
        যদি (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;

    পক্ষ cap_ept_pt_cap:
        যদি (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } অন্যথা {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        প্রত্যাগম ret;
#endif

    স্বভাব:
        প্রত্যাগম Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        যদি (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            প্রত্যাগম cap_io_space_cap_new(domainID, PCIDevice);
        } অন্যথা {
            প্রত্যাগম cap_null_cap_new();
        }
    }

    স্বভাব:
        প্রত্যাগম cap;
    }
#endif
    প্রত্যাগম cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    যদি (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        প্রত্যাগম cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } অন্যথা {
        প্রত্যাগম cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_page_directory_cap:
        যদি (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        ভঙ্গ;

    পক্ষ cap_page_table_cap:
        যদি (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        ভঙ্গ;

    পক্ষ cap_asid_pool_cap:
        যদি (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ভঙ্গ;
    পক্ষ cap_asid_control_cap:
    পক্ষ cap_io_port_control_cap:
        ভঙ্গ;
    পক্ষ cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        যদি (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            প্রত্যাগম fc_ret;
        }
        ভঙ্গ;
#ifdef CONFIG_IOMMU
    পক্ষ cap_io_space_cap:
        যদি (final) {
            unmapVTDContextEntry(cap);
        }
        ভঙ্গ;

    পক্ষ cap_io_page_table_cap:
        যদি (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        ভঙ্গ;
#endif

#ifdef CONFIG_VTX
    পক্ষ cap_vcpu_cap:
        যদি (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ভঙ্গ;
    পক্ষ cap_ept_pml4_cap:
        যদি (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        ভঙ্গ;

    পক্ষ cap_ept_pdpt_cap:
        যদি (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        ভঙ্গ;

    পক্ষ cap_ept_pd_cap:
        যদি (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        ভঙ্গ;

    পক্ষ cap_ept_pt_cap:
        যদি (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        ভঙ্গ;
#endif

    স্বভাব:
        প্রত্যাগম Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    প্রত্যাগম fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    বিকল্প (cap_get_capType(cap_a)) {
    পক্ষ cap_frame_cap:
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, শীর্ষ_A, শীর্ষ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            শীর্ষ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            শীর্ষ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            প্রত্যাগম ((botA <= botB) && (শীর্ষ_A >= শীর্ষ_B) && (botB <= শীর্ষ_B));
        }
        ভঙ্গ;

    পক্ষ cap_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_page_table_cap) {
            প্রত্যাগম cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_page_directory_cap:
        যদি (cap_get_capType(cap_b) == cap_page_directory_cap) {
            প্রত্যাগম cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_asid_control_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_control_cap) {
            প্রত্যাগম true;
        }
        ভঙ্গ;

    পক্ষ cap_asid_pool_cap:
        যদি (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            প্রত্যাগম cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_io_port_control_cap:
        যদি (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            প্রত্যাগম true;
        }
        ভঙ্গ;

    পক্ষ cap_io_port_cap:
        যদি (cap_get_capType(cap_b) == cap_io_port_cap) {
            প্রত্যাগম  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        ভঙ্গ;

#ifdef CONFIG_IOMMU
    পক্ষ cap_io_space_cap:
        যদি (cap_get_capType(cap_b) == cap_io_space_cap) {
            প্রত্যাগম cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_io_page_table_cap:
        যদি (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            প্রত্যাগম cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        ভঙ্গ;
#endif

#ifdef CONFIG_VTX
    পক্ষ cap_vcpu_cap:
        যদি (cap_get_capType(cap_b) == cap_vcpu_cap) {
            প্রত্যাগম cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_ept_pml4_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            প্রত্যাগম cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_ept_pdpt_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            প্রত্যাগম cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_ept_pd_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            প্রত্যাগম cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        ভঙ্গ;

    পক্ষ cap_ept_pt_cap:
        যদি (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            প্রত্যাগম cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        ভঙ্গ;

#endif

    }

    প্রত্যাগম Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    যদি (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        প্রত্যাগম false;
    }
    যদি (cap_get_capType(cap_a) == cap_frame_cap) {
        যদি (cap_get_capType(cap_b) == cap_frame_cap) {
            প্রত্যাগম ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    প্রত্যাগম Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    বিকল্প (t) {
    পক্ষ seL4_X86_4K:
        প্রত্যাগম pageBitsForSize(X86_SmallPage);
    পক্ষ seL4_X86_LargePageObject:
        প্রত্যাগম pageBitsForSize(X86_LargePage);
    পক্ষ seL4_X86_PageTableObject:
        প্রত্যাগম seL4_PageTableBits;
    পক্ষ seL4_X86_PageDirectoryObject:
        প্রত্যাগম seL4_PageDirBits;
    পক্ষ seL4_IA32_PDPTObject:
        প্রত্যাগম seL4_PDPTBits;
    পক্ষ seL4_X86_IOPageTableObject:
        প্রত্যাগম seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    পক্ষ seL4_X86_VCPUObject:
        প্রত্যাগম seL4_X86_VCPUBits;
    পক্ষ seL4_X86_EPTPML4Object:
        প্রত্যাগম seL4_X86_EPTPML4Bits;
    পক্ষ seL4_X86_EPTPDPTObject:
        প্রত্যাগম seL4_X86_EPTPDPTBits;
    পক্ষ seL4_X86_EPTPDObject:
        প্রত্যাগম seL4_X86_EPTPDBits;
    পক্ষ seL4_X86_EPTPTObject:
        প্রত্যাগম seL4_X86_EPTPTBits;
#endif
    স্বভাব:
        প্রত্যাগম Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, শূন্য *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    বিকল্প (t) {
    পক্ষ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        প্রত্যাগম cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    পক্ষ seL4_X86_EPTPML4Object:
        প্রত্যাগম cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    পক্ষ seL4_X86_EPTPDPTObject:
        প্রত্যাগম cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    পক্ষ seL4_X86_EPTPDObject:
        প্রত্যাগম cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    পক্ষ seL4_X86_EPTPTObject:
        প্রত্যাগম cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    স্বভাব:
#endif
        প্রত্যাগম Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    বিকল্প (cap_get_capType(cap)) {
    পক্ষ cap_asid_control_cap:
    পক্ষ cap_asid_pool_cap:
        প্রত্যাগম decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    পক্ষ cap_io_port_control_cap:
        প্রত্যাগম decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    পক্ষ cap_io_port_cap:
        প্রত্যাগম decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    পক্ষ cap_io_space_cap:
        প্রত্যাগম decodeX86IOSpaceInvocation(invLabel, cap);
    পক্ষ cap_io_page_table_cap:
        প্রত্যাগম decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    পক্ষ cap_vcpu_cap:
        প্রত্যাগম decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    পক্ষ cap_ept_pml4_cap:
    পক্ষ cap_ept_pdpt_cap:
    পক্ষ cap_ept_pd_cap:
    পক্ষ cap_ept_pt_cap:
        প্রত্যাগম decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    স্বভাব:
        প্রত্যাগম Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
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
