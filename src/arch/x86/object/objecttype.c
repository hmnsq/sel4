/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>
#અન્તર્ભાવ <types.h>
#અન્તર્ભાવ <api/failures.h>
#અન્તર્ભાવ <kernel/vspace.h>
#અન્તર્ભાવ <object/structures.h>
#અન્તર્ભાવ <arch/machine.h>
#અન્તર્ભાવ <arch/model/statedata.h>
#અન્તર્ભાવ <machine/fpu.h>
#અન્તર્ભાવ <arch/object/objecttype.h>
#અન્તર્ભાવ <arch/object/ioport.h>
#અન્તર્ભાવ <plat/machine/devices.h>

#અન્તર્ભાવ <arch/object/iospace.h>
#અન્તર્ભાવ <arch/object/vcpu.h>
#અન્તર્ભાવ <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_table_cap:
        જો (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_page_directory_cap:
        જો (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_asid_control_cap:
    સ્થિતિ cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
    સ્થિતિ cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        પાછા ret;
    સ્થિતિ cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;

#ifdef CONFIG_IOMMU
    સ્થિતિ cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
    સ્થિતિ cap_io_page_table_cap:
        જો (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;
#endif

#ifdef CONFIG_VTX
    સ્થિતિ cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        પાછા ret;
    સ્થિતિ cap_ept_pml4_cap:
        જો (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;
    સ્થિતિ cap_ept_pdpt_cap:
        જો (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_ept_pd_cap:
        જો (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;

    સ્થિતિ cap_ept_pt_cap:
        જો (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } નહીંતર {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        પાછા ret;
#endif

    મૂળભૂત:
        પાછા Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        જો (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            પાછા cap_io_space_cap_new(domainID, PCIDevice);
        } નહીંતર {
            પાછા cap_null_cap_new();
        }
    }

    મૂળભૂત:
        પાછા cap;
    }
#endif
    પાછા cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    જો (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        પાછા cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } નહીંતર {
        પાછા cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_page_directory_cap:
        જો (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        તોડો;

    સ્થિતિ cap_page_table_cap:
        જો (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        તોડો;

    સ્થિતિ cap_asid_pool_cap:
        જો (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        તોડો;
    સ્થિતિ cap_asid_control_cap:
    સ્થિતિ cap_io_port_control_cap:
        તોડો;
    સ્થિતિ cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        જો (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            પાછા fc_ret;
        }
        તોડો;
#ifdef CONFIG_IOMMU
    સ્થિતિ cap_io_space_cap:
        જો (final) {
            unmapVTDContextEntry(cap);
        }
        તોડો;

    સ્થિતિ cap_io_page_table_cap:
        જો (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        તોડો;
#endif

#ifdef CONFIG_VTX
    સ્થિતિ cap_vcpu_cap:
        જો (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        તોડો;
    સ્થિતિ cap_ept_pml4_cap:
        જો (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        તોડો;

    સ્થિતિ cap_ept_pdpt_cap:
        જો (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        તોડો;

    સ્થિતિ cap_ept_pd_cap:
        જો (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        તોડો;

    સ્થિતિ cap_ept_pt_cap:
        જો (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        તોડો;
#endif

    મૂળભૂત:
        પાછા Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    પાછા fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    પસંદગી (cap_get_capType(cap_a)) {
    સ્થિતિ cap_frame_cap:
        જો (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, ટોચ_A, ટોચ_B;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            ટોચ_A = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            ટોચ_B = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            પાછા ((botA <= botB) && (ટોચ_A >= ટોચ_B) && (botB <= ટોચ_B));
        }
        તોડો;

    સ્થિતિ cap_page_table_cap:
        જો (cap_get_capType(cap_b) == cap_page_table_cap) {
            પાછા cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_page_directory_cap:
        જો (cap_get_capType(cap_b) == cap_page_directory_cap) {
            પાછા cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_asid_control_cap:
        જો (cap_get_capType(cap_b) == cap_asid_control_cap) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_asid_pool_cap:
        જો (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            પાછા cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        તોડો;

    સ્થિતિ cap_io_port_control_cap:
        જો (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            પાછા true;
        }
        તોડો;

    સ્થિતિ cap_io_port_cap:
        જો (cap_get_capType(cap_b) == cap_io_port_cap) {
            પાછા  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        તોડો;

#ifdef CONFIG_IOMMU
    સ્થિતિ cap_io_space_cap:
        જો (cap_get_capType(cap_b) == cap_io_space_cap) {
            પાછા cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        તોડો;

    સ્થિતિ cap_io_page_table_cap:
        જો (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            પાછા cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        તોડો;
#endif

#ifdef CONFIG_VTX
    સ્થિતિ cap_vcpu_cap:
        જો (cap_get_capType(cap_b) == cap_vcpu_cap) {
            પાછા cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_ept_pml4_cap:
        જો (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            પાછા cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_ept_pdpt_cap:
        જો (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            પાછા cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_ept_pd_cap:
        જો (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            પાછા cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        તોડો;

    સ્થિતિ cap_ept_pt_cap:
        જો (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            પાછા cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        તોડો;

#endif

    }

    પાછા Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    જો (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        પાછા false;
    }
    જો (cap_get_capType(cap_a) == cap_frame_cap) {
        જો (cap_get_capType(cap_b) == cap_frame_cap) {
            પાછા ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    પાછા Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    પસંદગી (t) {
    સ્થિતિ seL4_X86_4K:
        પાછા pageBitsForSize(X86_SmallPage);
    સ્થિતિ seL4_X86_LargePageObject:
        પાછા pageBitsForSize(X86_LargePage);
    સ્થિતિ seL4_X86_PageTableObject:
        પાછા seL4_PageTableBits;
    સ્થિતિ seL4_X86_PageDirectoryObject:
        પાછા seL4_PageDirBits;
    સ્થિતિ seL4_IA32_PDPTObject:
        પાછા seL4_PDPTBits;
    સ્થિતિ seL4_X86_IOPageTableObject:
        પાછા seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    સ્થિતિ seL4_X86_VCPUObject:
        પાછા seL4_X86_VCPUBits;
    સ્થિતિ seL4_X86_EPTPML4Object:
        પાછા seL4_X86_EPTPML4Bits;
    સ્થિતિ seL4_X86_EPTPDPTObject:
        પાછા seL4_X86_EPTPDPTBits;
    સ્થિતિ seL4_X86_EPTPDObject:
        પાછા seL4_X86_EPTPDBits;
    સ્થિતિ seL4_X86_EPTPTObject:
        પાછા seL4_X86_EPTPTBits;
#endif
    મૂળભૂત:
        પાછા Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, શૂન્ય *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    પસંદગી (t) {
    સ્થિતિ seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        પાછા cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    સ્થિતિ seL4_X86_EPTPML4Object:
        પાછા cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    સ્થિતિ seL4_X86_EPTPDPTObject:
        પાછા cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    સ્થિતિ seL4_X86_EPTPDObject:
        પાછા cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    સ્થિતિ seL4_X86_EPTPTObject:
        પાછા cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    મૂળભૂત:
#endif
        પાછા Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    પસંદગી (cap_get_capType(cap)) {
    સ્થિતિ cap_asid_control_cap:
    સ્થિતિ cap_asid_pool_cap:
        પાછા decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    સ્થિતિ cap_io_port_control_cap:
        પાછા decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    સ્થિતિ cap_io_port_cap:
        પાછા decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    સ્થિતિ cap_io_space_cap:
        પાછા decodeX86IOSpaceInvocation(invLabel, cap);
    સ્થિતિ cap_io_page_table_cap:
        પાછા decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    સ્થિતિ cap_vcpu_cap:
        પાછા decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    સ્થિતિ cap_ept_pml4_cap:
    સ્થિતિ cap_ept_pdpt_cap:
    સ્થિતિ cap_ept_pd_cap:
    સ્થિતિ cap_ept_pt_cap:
        પાછા decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    મૂળભૂત:
        પાછા Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

શૂન્ય Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

શૂન્ય Arch_postCapDeletion(cap_t cap)
{
    જો (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
