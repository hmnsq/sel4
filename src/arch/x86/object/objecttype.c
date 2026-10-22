/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <kernel/vspace.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <machine/fpu.h>
#ضمن <arch/object/objecttype.h>
#ضمن <arch/object/ioport.h>
#ضمن <plat/machine/devices.h>

#ضمن <arch/object/iospace.h>
#ضمن <arch/object/vcpu.h>
#ضمن <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_page_table_cap:
        𐤀𐤌 (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_page_directory_cap:
        𐤀𐤌 (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    حالة cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;

#ifdef CONFIG_IOMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_page_table_cap:
        𐤀𐤌 (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#endif

#ifdef CONFIG_VTX
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_ept_pml4_cap:
        𐤀𐤌 (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
    حالة cap_ept_pdpt_cap:
        𐤀𐤌 (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pd_cap:
        𐤀𐤌 (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pt_cap:
        𐤀𐤌 (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#endif

    افتراضي:
        أرجع Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        𐤀𐤌 (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            أرجع cap_io_space_cap_new(domainID, PCIDevice);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    }

    افتراضي:
        أرجع cap;
    }
#endif
    أرجع cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    𐤀𐤌 (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        أرجع cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_page_directory_cap:
        𐤀𐤌 (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        اكسر;

    حالة cap_page_table_cap:
        𐤀𐤌 (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        اكسر;

    حالة cap_asid_pool_cap:
        𐤀𐤌 (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        اكسر;
    حالة cap_asid_control_cap:
    حالة cap_io_port_control_cap:
        اكسر;
    حالة cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        𐤀𐤌 (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            أرجع fc_ret;
        }
        اكسر;
#ifdef CONFIG_IOMMU
    حالة cap_io_space_cap:
        𐤀𐤌 (final) {
            unmapVTDContextEntry(cap);
        }
        اكسر;

    حالة cap_io_page_table_cap:
        𐤀𐤌 (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        اكسر;
#endif

#ifdef CONFIG_VTX
    حالة cap_vcpu_cap:
        𐤀𐤌 (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
    حالة cap_ept_pml4_cap:
        𐤀𐤌 (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pdpt_cap:
        𐤀𐤌 (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pd_cap:
        𐤀𐤌 (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pt_cap:
        𐤀𐤌 (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        اكسر;
#endif

    افتراضي:
        أرجع Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_frame_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            أرجع ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        اكسر;

    حالة cap_page_table_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_page_table_cap) {
            أرجع cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        اكسر;

    حالة cap_page_directory_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_page_directory_cap) {
            أرجع cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        اكسر;

    حالة cap_asid_control_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_asid_control_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            أرجع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;

    حالة cap_io_port_control_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_io_port_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_io_port_cap) {
            أرجع  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        اكسر;

#ifdef CONFIG_IOMMU
    حالة cap_io_space_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        اكسر;

    حالة cap_io_page_table_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            أرجع cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        اكسر;
#endif

#ifdef CONFIG_VTX
    حالة cap_vcpu_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pml4_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            أرجع cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pdpt_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            أرجع cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pd_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            أرجع cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pt_cap:
        𐤀𐤌 (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            أرجع cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        اكسر;

#endif

    }

    أرجع Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    𐤀𐤌 (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        أرجع false;
    }
    𐤀𐤌 (cap_get_capType(cap_a) == cap_frame_cap) {
        𐤀𐤌 (cap_get_capType(cap_b) == cap_frame_cap) {
            أرجع ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع pageBitsForSize(X86_SmallPage);
    حالة seL4_X86_LargePageObject:
        أرجع pageBitsForSize(X86_LargePage);
    حالة seL4_X86_PageTableObject:
        أرجع seL4_PageTableBits;
    حالة seL4_X86_PageDirectoryObject:
        أرجع seL4_PageDirBits;
    حالة seL4_IA32_PDPTObject:
        أرجع seL4_PDPTBits;
    حالة seL4_X86_IOPageTableObject:
        أرجع seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    حالة seL4_X86_VCPUObject:
        أرجع seL4_X86_VCPUBits;
    حالة seL4_X86_EPTPML4Object:
        أرجع seL4_X86_EPTPML4Bits;
    حالة seL4_X86_EPTPDPTObject:
        أرجع seL4_X86_EPTPDPTBits;
    حالة seL4_X86_EPTPDObject:
        أرجع seL4_X86_EPTPDBits;
    حالة seL4_X86_EPTPTObject:
        أرجع seL4_X86_EPTPTBits;
#endif
    افتراضي:
        أرجع Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, فراغ *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    بدل (t) {
    حالة seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        أرجع cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    حالة seL4_X86_EPTPML4Object:
        أرجع cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    حالة seL4_X86_EPTPDPTObject:
        أرجع cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    حالة seL4_X86_EPTPDObject:
        أرجع cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    حالة seL4_X86_EPTPTObject:
        أرجع cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    افتراضي:
#endif
        أرجع Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    بدل (cap_get_capType(cap)) {
    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        أرجع decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    حالة cap_io_port_control_cap:
        أرجع decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    حالة cap_io_port_cap:
        أرجع decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    حالة cap_io_space_cap:
        أرجع decodeX86IOSpaceInvocation(invLabel, cap);
    حالة cap_io_page_table_cap:
        أرجع decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    حالة cap_vcpu_cap:
        أرجع decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    حالة cap_ept_pml4_cap:
    حالة cap_ept_pdpt_cap:
    حالة cap_ept_pd_cap:
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    افتراضي:
        أرجع Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

فراغ Arch_postCapDeletion(cap_t cap)
{
    𐤀𐤌 (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
