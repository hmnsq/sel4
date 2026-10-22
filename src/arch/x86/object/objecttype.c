/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <machine/fpu.h>
#شامل <arch/object/objecttype.h>
#شامل <arch/object/ioport.h>
#شامل <plat/machine/devices.h>

#شامل <arch/object/iospace.h>
#شامل <arch/object/vcpu.h>
#شامل <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_table_cap:
        اگر (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_page_directory_cap:
        اگر (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
    حالت cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        بازگشت ret;
    حالت cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;

#ifdef CONFIG_IOMMU
    حالت cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
    حالت cap_io_page_table_cap:
        اگر (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;
#endif

#ifdef CONFIG_VTX
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        بازگشت ret;
    حالت cap_ept_pml4_cap:
        اگر (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;
    حالت cap_ept_pdpt_cap:
        اگر (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_ept_pd_cap:
        اگر (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;

    حالت cap_ept_pt_cap:
        اگر (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } وگرنه {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        بازگشت ret;
#endif

    پیشفرض:
        بازگشت Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    انتخاب (cap_get_capType(cap)) {
    حالت cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        اگر (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            بازگشت cap_io_space_cap_new(domainID, PCIDevice);
        } وگرنه {
            بازگشت cap_null_cap_new();
        }
    }

    پیشفرض:
        بازگشت cap;
    }
#endif
    بازگشت cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        بازگشت cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } وگرنه {
        بازگشت cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_page_directory_cap:
        اگر (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        بشکن;

    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        بشکن;
    حالت cap_asid_control_cap:
    حالت cap_io_port_control_cap:
        بشکن;
    حالت cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        اگر (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            بازگشت fc_ret;
        }
        بشکن;
#ifdef CONFIG_IOMMU
    حالت cap_io_space_cap:
        اگر (final) {
            unmapVTDContextEntry(cap);
        }
        بشکن;

    حالت cap_io_page_table_cap:
        اگر (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        بشکن;
#endif

#ifdef CONFIG_VTX
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        بشکن;
    حالت cap_ept_pml4_cap:
        اگر (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        بشکن;

    حالت cap_ept_pdpt_cap:
        اگر (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        بشکن;

    حالت cap_ept_pd_cap:
        اگر (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        بشکن;

    حالت cap_ept_pt_cap:
        اگر (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        بشکن;
#endif

    پیشفرض:
        بازگشت Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    بازگشت fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_frame_cap:
        اگر (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, بالاترین_A, بالاترین_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            بالاترین_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            بالاترین_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            بازگشت ((botA <= botB) && (بالاترین_A >= بالاترین_B) && (botB <= بالاترین_B));
        }
        بشکن;

    حالت cap_page_table_cap:
        اگر (cap_get_capType(cap_b) == cap_page_table_cap) {
            بازگشت cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        بشکن;

    حالت cap_page_directory_cap:
        اگر (cap_get_capType(cap_b) == cap_page_directory_cap) {
            بازگشت cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        بشکن;

    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            بازگشت true;
        }
        بشکن;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            بازگشت cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        بشکن;

    حالت cap_io_port_control_cap:
        اگر (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            بازگشت true;
        }
        بشکن;

    حالت cap_io_port_cap:
        اگر (cap_get_capType(cap_b) == cap_io_port_cap) {
            بازگشت  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        بشکن;

#ifdef CONFIG_IOMMU
    حالت cap_io_space_cap:
        اگر (cap_get_capType(cap_b) == cap_io_space_cap) {
            بازگشت cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        بشکن;

    حالت cap_io_page_table_cap:
        اگر (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            بازگشت cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        بشکن;
#endif

#ifdef CONFIG_VTX
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            بازگشت cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        بشکن;

    حالت cap_ept_pml4_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            بازگشت cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        بشکن;

    حالت cap_ept_pdpt_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            بازگشت cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        بشکن;

    حالت cap_ept_pd_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            بازگشت cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        بشکن;

    حالت cap_ept_pt_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            بازگشت cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        بشکن;

#endif

    }

    بازگشت Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        بازگشت false;
    }
    اگر (cap_get_capType(cap_a) == cap_frame_cap) {
        اگر (cap_get_capType(cap_b) == cap_frame_cap) {
            بازگشت ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    بازگشت Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    انتخاب (t) {
    حالت seL4_X86_4K:
        بازگشت pageBitsForSize(X86_SmallPage);
    حالت seL4_X86_LargePageObject:
        بازگشت pageBitsForSize(X86_LargePage);
    حالت seL4_X86_PageTableObject:
        بازگشت seL4_PageTableBits;
    حالت seL4_X86_PageDirectoryObject:
        بازگشت seL4_PageDirBits;
    حالت seL4_IA32_PDPTObject:
        بازگشت seL4_PDPTBits;
    حالت seL4_X86_IOPageTableObject:
        بازگشت seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    حالت seL4_X86_VCPUObject:
        بازگشت seL4_X86_VCPUBits;
    حالت seL4_X86_EPTPML4Object:
        بازگشت seL4_X86_EPTPML4Bits;
    حالت seL4_X86_EPTPDPTObject:
        بازگشت seL4_X86_EPTPDPTBits;
    حالت seL4_X86_EPTPDObject:
        بازگشت seL4_X86_EPTPDBits;
    حالت seL4_X86_EPTPTObject:
        بازگشت seL4_X86_EPTPTBits;
#endif
    پیشفرض:
        بازگشت Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, تهی *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    انتخاب (t) {
    حالت seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        بازگشت cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    حالت seL4_X86_EPTPML4Object:
        بازگشت cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    حالت seL4_X86_EPTPDPTObject:
        بازگشت cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    حالت seL4_X86_EPTPDObject:
        بازگشت cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    حالت seL4_X86_EPTPTObject:
        بازگشت cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    پیشفرض:
#endif
        بازگشت Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        بازگشت decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    حالت cap_io_port_control_cap:
        بازگشت decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    حالت cap_io_port_cap:
        بازگشت decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    حالت cap_io_space_cap:
        بازگشت decodeX86IOSpaceInvocation(invLabel, cap);
    حالت cap_io_page_table_cap:
        بازگشت decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    حالت cap_vcpu_cap:
        بازگشت decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    حالت cap_ept_pml4_cap:
    حالت cap_ept_pdpt_cap:
    حالت cap_ept_pd_cap:
    حالت cap_ept_pt_cap:
        بازگشت decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    پیشفرض:
        بازگشت Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

تهی Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

تهی Arch_postCapDeletion(cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
