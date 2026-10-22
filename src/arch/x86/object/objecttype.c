/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <types.h>
#അന്തര്ഭാവ <api/failures.h>
#അന്തര്ഭാവ <kernel/vspace.h>
#അന്തര്ഭാവ <object/structures.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/model/statedata.h>
#അന്തര്ഭാവ <machine/fpu.h>
#അന്തര്ഭാവ <arch/object/objecttype.h>
#അന്തര്ഭാവ <arch/object/ioport.h>
#അന്തര്ഭാവ <plat/machine/devices.h>

#അന്തര്ഭാവ <arch/object/iospace.h>
#അന്തര്ഭാവ <arch/object/vcpu.h>
#അന്തര്ഭാവ <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_asid_control_cap:
    അവസ്ഥ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
    അവസ്ഥ cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        തിരികെ ret;
    അവസ്ഥ cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;

#ifdef CONFIG_IOMMU
    അവസ്ഥ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;
#endif

#ifdef CONFIG_VTX
    അവസ്ഥ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        തിരികെ ret;
    അവസ്ഥ cap_ept_pml4_cap:
        എങ്കിൽ (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;
    അവസ്ഥ cap_ept_pdpt_cap:
        എങ്കിൽ (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_ept_pd_cap:
        എങ്കിൽ (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;

    അവസ്ഥ cap_ept_pt_cap:
        എങ്കിൽ (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } അല്ലെങ്കിൽ {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        തിരികെ ret;
#endif

    സ്ഥിരസ്ഥിതി:
        തിരികെ Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        എങ്കിൽ (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            തിരികെ cap_io_space_cap_new(domainID, PCIDevice);
        } അല്ലെങ്കിൽ {
            തിരികെ cap_null_cap_new();
        }
    }

    സ്ഥിരസ്ഥിതി:
        തിരികെ cap;
    }
#endif
    തിരികെ cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        തിരികെ cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } അല്ലെങ്കിൽ {
        തിരികെ cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        നിർത്തുക;
    അവസ്ഥ cap_asid_control_cap:
    അവസ്ഥ cap_io_port_control_cap:
        നിർത്തുക;
    അവസ്ഥ cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        എങ്കിൽ (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            തിരികെ fc_ret;
        }
        നിർത്തുക;
#ifdef CONFIG_IOMMU
    അവസ്ഥ cap_io_space_cap:
        എങ്കിൽ (final) {
            unmapVTDContextEntry(cap);
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        നിർത്തുക;
#endif

#ifdef CONFIG_VTX
    അവസ്ഥ cap_vcpu_cap:
        എങ്കിൽ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        നിർത്തുക;
    അവസ്ഥ cap_ept_pml4_cap:
        എങ്കിൽ (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pdpt_cap:
        എങ്കിൽ (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pd_cap:
        എങ്കിൽ (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pt_cap:
        എങ്കിൽ (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        നിർത്തുക;
#endif

    സ്ഥിരസ്ഥിതി:
        തിരികെ Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    തിരികെ fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap_a)) {
    അവസ്ഥ cap_frame_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, മുകൾ_A, മുകൾ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            മുകൾ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            മുകൾ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            തിരികെ ((botA <= botB) && (മുകൾ_A >= മുകൾ_B) && (botB <= മുകൾ_B));
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_table_cap) {
            തിരികെ cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_page_directory_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_page_directory_cap) {
            തിരികെ cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_asid_pool_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            തിരികെ cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_port_control_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            തിരികെ true;
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_port_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_port_cap) {
            തിരികെ  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        നിർത്തുക;

#ifdef CONFIG_IOMMU
    അവസ്ഥ cap_io_space_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_space_cap) {
            തിരികെ cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_io_page_table_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            തിരികെ cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        നിർത്തുക;
#endif

#ifdef CONFIG_VTX
    അവസ്ഥ cap_vcpu_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            തിരികെ cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pml4_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            തിരികെ cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pdpt_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            തിരികെ cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pd_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            തിരികെ cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        നിർത്തുക;

    അവസ്ഥ cap_ept_pt_cap:
        എങ്കിൽ (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            തിരികെ cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        നിർത്തുക;

#endif

    }

    തിരികെ Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    എങ്കിൽ (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        തിരികെ false;
    }
    എങ്കിൽ (cap_get_capType(cap_a) == cap_frame_cap) {
        എങ്കിൽ (cap_get_capType(cap_b) == cap_frame_cap) {
            തിരികെ ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    തിരികെ Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_X86_4K:
        തിരികെ pageBitsForSize(X86_SmallPage);
    അവസ്ഥ seL4_X86_LargePageObject:
        തിരികെ pageBitsForSize(X86_LargePage);
    അവസ്ഥ seL4_X86_PageTableObject:
        തിരികെ seL4_PageTableBits;
    അവസ്ഥ seL4_X86_PageDirectoryObject:
        തിരികെ seL4_PageDirBits;
    അവസ്ഥ seL4_IA32_PDPTObject:
        തിരികെ seL4_PDPTBits;
    അവസ്ഥ seL4_X86_IOPageTableObject:
        തിരികെ seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    അവസ്ഥ seL4_X86_VCPUObject:
        തിരികെ seL4_X86_VCPUBits;
    അവസ്ഥ seL4_X86_EPTPML4Object:
        തിരികെ seL4_X86_EPTPML4Bits;
    അവസ്ഥ seL4_X86_EPTPDPTObject:
        തിരികെ seL4_X86_EPTPDPTBits;
    അവസ്ഥ seL4_X86_EPTPDObject:
        തിരികെ seL4_X86_EPTPDBits;
    അവസ്ഥ seL4_X86_EPTPTObject:
        തിരികെ seL4_X86_EPTPTBits;
#endif
    സ്ഥിരസ്ഥിതി:
        തിരികെ Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, ശൂന്യം *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    തിരഞ്ഞെടുപ്പ് (t) {
    അവസ്ഥ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        തിരികെ cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    അവസ്ഥ seL4_X86_EPTPML4Object:
        തിരികെ cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    അവസ്ഥ seL4_X86_EPTPDPTObject:
        തിരികെ cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    അവസ്ഥ seL4_X86_EPTPDObject:
        തിരികെ cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    അവസ്ഥ seL4_X86_EPTPTObject:
        തിരികെ cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    സ്ഥിരസ്ഥിതി:
#endif
        തിരികെ Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    തിരഞ്ഞെടുപ്പ് (cap_get_capType(cap)) {
    അവസ്ഥ cap_asid_control_cap:
    അവസ്ഥ cap_asid_pool_cap:
        തിരികെ decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    അവസ്ഥ cap_io_port_control_cap:
        തിരികെ decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    അവസ്ഥ cap_io_port_cap:
        തിരികെ decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    അവസ്ഥ cap_io_space_cap:
        തിരികെ decodeX86IOSpaceInvocation(invLabel, cap);
    അവസ്ഥ cap_io_page_table_cap:
        തിരികെ decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    അവസ്ഥ cap_vcpu_cap:
        തിരികെ decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    അവസ്ഥ cap_ept_pml4_cap:
    അവസ്ഥ cap_ept_pdpt_cap:
    അവസ്ഥ cap_ept_pd_cap:
    അവസ്ഥ cap_ept_pt_cap:
        തിരികെ decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    സ്ഥിരസ്ഥിതി:
        തിരികെ Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

ശൂന്യം Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

ശൂന്യം Arch_postCapDeletion(cap_t cap)
{
    എങ്കിൽ (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
