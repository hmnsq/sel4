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

    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_table_cap:
        जर (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_page_directory_cap:
        जर (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_asid_control_cap:
    स्थिती cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
    स्थिती cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        परत ret;
    स्थिती cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;

#ifdef CONFIG_IOMMU
    स्थिती cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
    स्थिती cap_io_page_table_cap:
        जर (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;
#endif

#ifdef CONFIG_VTX
    स्थिती cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        परत ret;
    स्थिती cap_ept_pml4_cap:
        जर (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;
    स्थिती cap_ept_pdpt_cap:
        जर (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_ept_pd_cap:
        जर (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;

    स्थिती cap_ept_pt_cap:
        जर (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अन्यथा {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        परत ret;
#endif

    पूर्वनिर्धारित:
        परत Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    निवड (cap_get_capType(cap)) {
    स्थिती cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        जर (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            परत cap_io_space_cap_new(domainID, PCIDevice);
        } अन्यथा {
            परत cap_null_cap_new();
        }
    }

    पूर्वनिर्धारित:
        परत cap;
    }
#endif
    परत cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    जर (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        परत cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अन्यथा {
        परत cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    निवड (cap_get_capType(cap)) {
    स्थिती cap_page_directory_cap:
        जर (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        थांबा;

    स्थिती cap_asid_pool_cap:
        जर (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        थांबा;
    स्थिती cap_asid_control_cap:
    स्थिती cap_io_port_control_cap:
        थांबा;
    स्थिती cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        जर (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            परत fc_ret;
        }
        थांबा;
#ifdef CONFIG_IOMMU
    स्थिती cap_io_space_cap:
        जर (final) {
            unmapVTDContextEntry(cap);
        }
        थांबा;

    स्थिती cap_io_page_table_cap:
        जर (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        थांबा;
#endif

#ifdef CONFIG_VTX
    स्थिती cap_vcpu_cap:
        जर (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        थांबा;
    स्थिती cap_ept_pml4_cap:
        जर (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        थांबा;

    स्थिती cap_ept_pdpt_cap:
        जर (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        थांबा;

    स्थिती cap_ept_pd_cap:
        जर (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        थांबा;

    स्थिती cap_ept_pt_cap:
        जर (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        थांबा;
#endif

    पूर्वनिर्धारित:
        परत Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    परत fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    निवड (cap_get_capType(cap_a)) {
    स्थिती cap_frame_cap:
        जर (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शीर्ष_A, शीर्ष_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शीर्ष_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            शीर्ष_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            परत ((botA <= botB) && (शीर्ष_A >= शीर्ष_B) && (botB <= शीर्ष_B));
        }
        थांबा;

    स्थिती cap_page_table_cap:
        जर (cap_get_capType(cap_b) == cap_page_table_cap) {
            परत cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_page_directory_cap:
        जर (cap_get_capType(cap_b) == cap_page_directory_cap) {
            परत cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_asid_control_cap:
        जर (cap_get_capType(cap_b) == cap_asid_control_cap) {
            परत true;
        }
        थांबा;

    स्थिती cap_asid_pool_cap:
        जर (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            परत cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        थांबा;

    स्थिती cap_io_port_control_cap:
        जर (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            परत true;
        }
        थांबा;

    स्थिती cap_io_port_cap:
        जर (cap_get_capType(cap_b) == cap_io_port_cap) {
            परत  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        थांबा;

#ifdef CONFIG_IOMMU
    स्थिती cap_io_space_cap:
        जर (cap_get_capType(cap_b) == cap_io_space_cap) {
            परत cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        थांबा;

    स्थिती cap_io_page_table_cap:
        जर (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            परत cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        थांबा;
#endif

#ifdef CONFIG_VTX
    स्थिती cap_vcpu_cap:
        जर (cap_get_capType(cap_b) == cap_vcpu_cap) {
            परत cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        थांबा;

    स्थिती cap_ept_pml4_cap:
        जर (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            परत cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_ept_pdpt_cap:
        जर (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            परत cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_ept_pd_cap:
        जर (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            परत cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        थांबा;

    स्थिती cap_ept_pt_cap:
        जर (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            परत cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        थांबा;

#endif

    }

    परत Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जर (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        परत false;
    }
    जर (cap_get_capType(cap_a) == cap_frame_cap) {
        जर (cap_get_capType(cap_b) == cap_frame_cap) {
            परत ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    परत Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    निवड (t) {
    स्थिती seL4_X86_4K:
        परत pageBitsForSize(X86_SmallPage);
    स्थिती seL4_X86_LargePageObject:
        परत pageBitsForSize(X86_LargePage);
    स्थिती seL4_X86_PageTableObject:
        परत seL4_PageTableBits;
    स्थिती seL4_X86_PageDirectoryObject:
        परत seL4_PageDirBits;
    स्थिती seL4_IA32_PDPTObject:
        परत seL4_PDPTBits;
    स्थिती seL4_X86_IOPageTableObject:
        परत seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    स्थिती seL4_X86_VCPUObject:
        परत seL4_X86_VCPUBits;
    स्थिती seL4_X86_EPTPML4Object:
        परत seL4_X86_EPTPML4Bits;
    स्थिती seL4_X86_EPTPDPTObject:
        परत seL4_X86_EPTPDPTBits;
    स्थिती seL4_X86_EPTPDObject:
        परत seL4_X86_EPTPDBits;
    स्थिती seL4_X86_EPTPTObject:
        परत seL4_X86_EPTPTBits;
#endif
    पूर्वनिर्धारित:
        परत Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    निवड (t) {
    स्थिती seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        परत cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    स्थिती seL4_X86_EPTPML4Object:
        परत cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    स्थिती seL4_X86_EPTPDPTObject:
        परत cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    स्थिती seL4_X86_EPTPDObject:
        परत cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    स्थिती seL4_X86_EPTPTObject:
        परत cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    पूर्वनिर्धारित:
#endif
        परत Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    निवड (cap_get_capType(cap)) {
    स्थिती cap_asid_control_cap:
    स्थिती cap_asid_pool_cap:
        परत decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    स्थिती cap_io_port_control_cap:
        परत decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    स्थिती cap_io_port_cap:
        परत decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    स्थिती cap_io_space_cap:
        परत decodeX86IOSpaceInvocation(invLabel, cap);
    स्थिती cap_io_page_table_cap:
        परत decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    स्थिती cap_vcpu_cap:
        परत decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    स्थिती cap_ept_pml4_cap:
    स्थिती cap_ept_pdpt_cap:
    स्थिती cap_ept_pd_cap:
    स्थिती cap_ept_pt_cap:
        परत decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    पूर्वनिर्धारित:
        परत Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

शून्य Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

शून्य Arch_postCapDeletion(cap_t cap)
{
    जर (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
