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
    पक्ष cap_page_table_cap:
        जइ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_page_directory_cap:
        जइ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        प्रत्यागम ret;
    पक्ष cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        प्रत्यागम ret;

#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_io_page_table_cap:
        जइ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
#endif

#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        प्रत्यागम ret;
    पक्ष cap_ept_pml4_cap:
        जइ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
    पक्ष cap_ept_pdpt_cap:
        जइ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_ept_pd_cap:
        जइ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;

    पक्ष cap_ept_pt_cap:
        जइ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } अण्णहा {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        प्रत्यागम ret;
#endif

    स्वभाव:
        प्रत्यागम Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        जइ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            प्रत्यागम cap_io_space_cap_new(domainID, PCIDevice);
        } अण्णहा {
            प्रत्यागम cap_null_cap_new();
        }
    }

    स्वभाव:
        प्रत्यागम cap;
    }
#endif
    प्रत्यागम cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    जइ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        प्रत्यागम cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } अण्णहा {
        प्रत्यागम cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_page_directory_cap:
        जइ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        जइ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        जइ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        भङ्ग;
    पक्ष cap_asid_control_cap:
    पक्ष cap_io_port_control_cap:
        भङ्ग;
    पक्ष cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        जइ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            प्रत्यागम fc_ret;
        }
        भङ्ग;
#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        जइ (final) {
            unmapVTDContextEntry(cap);
        }
        भङ्ग;

    पक्ष cap_io_page_table_cap:
        जइ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        भङ्ग;
#endif

#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap:
        जइ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        भङ्ग;
    पक्ष cap_ept_pml4_cap:
        जइ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pdpt_cap:
        जइ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pd_cap:
        जइ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        भङ्ग;

    पक्ष cap_ept_pt_cap:
        जइ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        भङ्ग;
#endif

    स्वभाव:
        प्रत्यागम Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    प्रत्यागम fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    विकल्प (cap_get_capType(cap_a)) {
    पक्ष cap_frame_cap:
        जइ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, शिखरम्_A, शिखरम्_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            शिखरम्_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            शिखरम्_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            प्रत्यागम ((botA <= botB) && (शिखरम्_A >= शिखरम्_B) && (botB <= शिखरम्_B));
        }
        भङ्ग;

    पक्ष cap_page_table_cap:
        जइ (cap_get_capType(cap_b) == cap_page_table_cap) {
            प्रत्यागम cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_page_directory_cap:
        जइ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            प्रत्यागम cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_asid_control_cap:
        जइ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_asid_pool_cap:
        जइ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            प्रत्यागम cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_port_control_cap:
        जइ (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            प्रत्यागम true;
        }
        भङ्ग;

    पक्ष cap_io_port_cap:
        जइ (cap_get_capType(cap_b) == cap_io_port_cap) {
            प्रत्यागम  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        भङ्ग;

#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        जइ (cap_get_capType(cap_b) == cap_io_space_cap) {
            प्रत्यागम cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        भङ्ग;

    पक्ष cap_io_page_table_cap:
        जइ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            प्रत्यागम cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        भङ्ग;
#endif

#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap:
        जइ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            प्रत्यागम cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pml4_cap:
        जइ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            प्रत्यागम cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pdpt_cap:
        जइ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            प्रत्यागम cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pd_cap:
        जइ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            प्रत्यागम cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        भङ्ग;

    पक्ष cap_ept_pt_cap:
        जइ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            प्रत्यागम cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        भङ्ग;

#endif

    }

    प्रत्यागम Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    जइ (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        प्रत्यागम false;
    }
    जइ (cap_get_capType(cap_a) == cap_frame_cap) {
        जइ (cap_get_capType(cap_b) == cap_frame_cap) {
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

word_t Arch_getObjectSize(word_t t)
{
    विकल्प (t) {
    पक्ष seL4_X86_4K:
        प्रत्यागम pageBitsForSize(X86_SmallPage);
    पक्ष seL4_X86_LargePageObject:
        प्रत्यागम pageBitsForSize(X86_LargePage);
    पक्ष seL4_X86_PageTableObject:
        प्रत्यागम seL4_PageTableBits;
    पक्ष seL4_X86_PageDirectoryObject:
        प्रत्यागम seL4_PageDirBits;
    पक्ष seL4_IA32_PDPTObject:
        प्रत्यागम seL4_PDPTBits;
    पक्ष seL4_X86_IOPageTableObject:
        प्रत्यागम seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    पक्ष seL4_X86_VCPUObject:
        प्रत्यागम seL4_X86_VCPUBits;
    पक्ष seL4_X86_EPTPML4Object:
        प्रत्यागम seL4_X86_EPTPML4Bits;
    पक्ष seL4_X86_EPTPDPTObject:
        प्रत्यागम seL4_X86_EPTPDPTBits;
    पक्ष seL4_X86_EPTPDObject:
        प्रत्यागम seL4_X86_EPTPDBits;
    पक्ष seL4_X86_EPTPTObject:
        प्रत्यागम seL4_X86_EPTPTBits;
#endif
    स्वभाव:
        प्रत्यागम Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, शून्य *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    विकल्प (t) {
    पक्ष seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        प्रत्यागम cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    पक्ष seL4_X86_EPTPML4Object:
        प्रत्यागम cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    पक्ष seL4_X86_EPTPDPTObject:
        प्रत्यागम cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    पक्ष seL4_X86_EPTPDObject:
        प्रत्यागम cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    पक्ष seL4_X86_EPTPTObject:
        प्रत्यागम cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    स्वभाव:
#endif
        प्रत्यागम Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    विकल्प (cap_get_capType(cap)) {
    पक्ष cap_asid_control_cap:
    पक्ष cap_asid_pool_cap:
        प्रत्यागम decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    पक्ष cap_io_port_control_cap:
        प्रत्यागम decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    पक्ष cap_io_port_cap:
        प्रत्यागम decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    पक्ष cap_io_space_cap:
        प्रत्यागम decodeX86IOSpaceInvocation(invLabel, cap);
    पक्ष cap_io_page_table_cap:
        प्रत्यागम decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    पक्ष cap_vcpu_cap:
        प्रत्यागम decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    पक्ष cap_ept_pml4_cap:
    पक्ष cap_ept_pdpt_cap:
    पक्ष cap_ept_pd_cap:
    पक्ष cap_ept_pt_cap:
        प्रत्यागम decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    स्वभाव:
        प्रत्यागम Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

शून्य Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

शून्य Arch_postCapDeletion(cap_t cap)
{
    जइ (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
