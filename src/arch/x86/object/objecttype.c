/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <api/failures.h>
#அந்தர்பாவ <kernel/vspace.h>
#அந்தர்பாவ <object/structures.h>
#அந்தர்பாவ <arch/machine.h>
#அந்தர்பாவ <arch/model/statedata.h>
#அந்தர்பாவ <machine/fpu.h>
#அந்தர்பாவ <arch/object/objecttype.h>
#அந்தர்பாவ <arch/object/ioport.h>
#அந்தர்பாவ <plat/machine/devices.h>

#அந்தர்பாவ <arch/object/iospace.h>
#அந்தர்பாவ <arch/object/vcpu.h>
#அந்தர்பாவ <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_table_cap:
        எனில் (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_page_directory_cap:
        எனில் (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_asid_control_cap:
    நிலை cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
    நிலை cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        திரும்பு ret;
    நிலை cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;

#ifdef CONFIG_IOMMU
    நிலை cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
    நிலை cap_io_page_table_cap:
        எனில் (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;
#endif

#ifdef CONFIG_VTX
    நிலை cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
    நிலை cap_ept_pml4_cap:
        எனில் (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;
    நிலை cap_ept_pdpt_cap:
        எனில் (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_ept_pd_cap:
        எனில் (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;

    நிலை cap_ept_pt_cap:
        எனில் (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } இல்லையெனில் {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        திரும்பு ret;
#endif

    இயல்பு:
        திரும்பு Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        எனில் (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            திரும்பு cap_io_space_cap_new(domainID, PCIDevice);
        } இல்லையெனில் {
            திரும்பு cap_null_cap_new();
        }
    }

    இயல்பு:
        திரும்பு cap;
    }
#endif
    திரும்பு cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    எனில் (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        திரும்பு cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } இல்லையெனில் {
        திரும்பு cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_page_directory_cap:
        எனில் (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        நிறுத்து;

    நிலை cap_asid_pool_cap:
        எனில் (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        நிறுத்து;
    நிலை cap_asid_control_cap:
    நிலை cap_io_port_control_cap:
        நிறுத்து;
    நிலை cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        எனில் (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            திரும்பு fc_ret;
        }
        நிறுத்து;
#ifdef CONFIG_IOMMU
    நிலை cap_io_space_cap:
        எனில் (final) {
            unmapVTDContextEntry(cap);
        }
        நிறுத்து;

    நிலை cap_io_page_table_cap:
        எனில் (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        நிறுத்து;
#endif

#ifdef CONFIG_VTX
    நிலை cap_vcpu_cap:
        எனில் (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        நிறுத்து;
    நிலை cap_ept_pml4_cap:
        எனில் (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        நிறுத்து;

    நிலை cap_ept_pdpt_cap:
        எனில் (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        நிறுத்து;

    நிலை cap_ept_pd_cap:
        எனில் (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        நிறுத்து;

    நிலை cap_ept_pt_cap:
        எனில் (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        நிறுத்து;
#endif

    இயல்பு:
        திரும்பு Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    திரும்பு fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    தேர்வு (cap_get_capType(cap_a)) {
    நிலை cap_frame_cap:
        எனில் (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, உச்சி_A, உச்சி_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            உச்சி_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            உச்சி_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            திரும்பு ((botA <= botB) && (உச்சி_A >= உச்சி_B) && (botB <= உச்சி_B));
        }
        நிறுத்து;

    நிலை cap_page_table_cap:
        எனில் (cap_get_capType(cap_b) == cap_page_table_cap) {
            திரும்பு cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_page_directory_cap:
        எனில் (cap_get_capType(cap_b) == cap_page_directory_cap) {
            திரும்பு cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_asid_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_asid_control_cap) {
            திரும்பு true;
        }
        நிறுத்து;

    நிலை cap_asid_pool_cap:
        எனில் (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            திரும்பு cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        நிறுத்து;

    நிலை cap_io_port_control_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            திரும்பு true;
        }
        நிறுத்து;

    நிலை cap_io_port_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_port_cap) {
            திரும்பு  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        நிறுத்து;

#ifdef CONFIG_IOMMU
    நிலை cap_io_space_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_space_cap) {
            திரும்பு cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        நிறுத்து;

    நிலை cap_io_page_table_cap:
        எனில் (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            திரும்பு cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        நிறுத்து;
#endif

#ifdef CONFIG_VTX
    நிலை cap_vcpu_cap:
        எனில் (cap_get_capType(cap_b) == cap_vcpu_cap) {
            திரும்பு cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_ept_pml4_cap:
        எனில் (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            திரும்பு cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_ept_pdpt_cap:
        எனில் (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            திரும்பு cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_ept_pd_cap:
        எனில் (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            திரும்பு cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        நிறுத்து;

    நிலை cap_ept_pt_cap:
        எனில் (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            திரும்பு cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        நிறுத்து;

#endif

    }

    திரும்பு Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    எனில் (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        திரும்பு false;
    }
    எனில் (cap_get_capType(cap_a) == cap_frame_cap) {
        எனில் (cap_get_capType(cap_b) == cap_frame_cap) {
            திரும்பு ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    திரும்பு Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    தேர்வு (t) {
    நிலை seL4_X86_4K:
        திரும்பு pageBitsForSize(X86_SmallPage);
    நிலை seL4_X86_LargePageObject:
        திரும்பு pageBitsForSize(X86_LargePage);
    நிலை seL4_X86_PageTableObject:
        திரும்பு seL4_PageTableBits;
    நிலை seL4_X86_PageDirectoryObject:
        திரும்பு seL4_PageDirBits;
    நிலை seL4_IA32_PDPTObject:
        திரும்பு seL4_PDPTBits;
    நிலை seL4_X86_IOPageTableObject:
        திரும்பு seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    நிலை seL4_X86_VCPUObject:
        திரும்பு seL4_X86_VCPUBits;
    நிலை seL4_X86_EPTPML4Object:
        திரும்பு seL4_X86_EPTPML4Bits;
    நிலை seL4_X86_EPTPDPTObject:
        திரும்பு seL4_X86_EPTPDPTBits;
    நிலை seL4_X86_EPTPDObject:
        திரும்பு seL4_X86_EPTPDBits;
    நிலை seL4_X86_EPTPTObject:
        திரும்பு seL4_X86_EPTPTBits;
#endif
    இயல்பு:
        திரும்பு Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, வெறுமை *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    தேர்வு (t) {
    நிலை seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        திரும்பு cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    நிலை seL4_X86_EPTPML4Object:
        திரும்பு cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    நிலை seL4_X86_EPTPDPTObject:
        திரும்பு cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    நிலை seL4_X86_EPTPDObject:
        திரும்பு cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    நிலை seL4_X86_EPTPTObject:
        திரும்பு cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    இயல்பு:
#endif
        திரும்பு Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    தேர்வு (cap_get_capType(cap)) {
    நிலை cap_asid_control_cap:
    நிலை cap_asid_pool_cap:
        திரும்பு decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    நிலை cap_io_port_control_cap:
        திரும்பு decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    நிலை cap_io_port_cap:
        திரும்பு decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    நிலை cap_io_space_cap:
        திரும்பு decodeX86IOSpaceInvocation(invLabel, cap);
    நிலை cap_io_page_table_cap:
        திரும்பு decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    நிலை cap_vcpu_cap:
        திரும்பு decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    நிலை cap_ept_pml4_cap:
    நிலை cap_ept_pdpt_cap:
    நிலை cap_ept_pd_cap:
    நிலை cap_ept_pt_cap:
        திரும்பு decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    இயல்பு:
        திரும்பு Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

வெறுமை Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

வெறுமை Arch_postCapDeletion(cap_t cap)
{
    எனில் (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
