/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#include <config.h>
#include <types.h>
#include <api/failures.h>
#include <kernel/vspace.h>
#include <object/structures.h>
#include <arch/machine.h>
#include <arch/model/statedata.h>
#include <machine/fpu.h>
#include <arch/object/objecttype.h>
#include <arch/object/ioport.h>
#include <plat/machine/devices.h>

#include <arch/object/iospace.h>
#include <arch/object/vcpu.h>
#include <plat/machine/intel-vtd.h>

deriveCap_ret_t Arch_deriveCap(cte_t *slot, cap_t cap)
{
    deriveCap_ret_t ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_page_table_cap:
        නම් (cap_page_table_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;

    pak_sa cap_page_directory_cap:
        නම් (cap_page_directory_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;

    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ආපසු ret;
    pak_sa cap_io_port_control_cap:
        ret.status = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        ආපසු ret;
    pak_sa cap_io_port_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ආපසු ret;

#ifdef CONFIG_IOMMU
    pak_sa cap_io_space_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ආපසු ret;
    pak_sa cap_io_page_table_cap:
        නම් (cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;
#endif

#ifdef CONFIG_VTX
    pak_sa cap_vcpu_cap:
        ret.cap = cap;
        ret.status = EXCEPTION_NONE;
        ආපසු ret;
    pak_sa cap_ept_pml4_cap:
        නම් (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;
    pak_sa cap_ept_pdpt_cap:
        නම් (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;

    pak_sa cap_ept_pd_cap:
        නම් (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;

    pak_sa cap_ept_pt_cap:
        නම් (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.status = EXCEPTION_NONE;
        } නැතහොත් {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.status = EXCEPTION_SYSCALL_ERROR;
        }
        ආපසු ret;
#endif

    default:
        ආපසු Mode_deriveCap(slot, cap);
    }
}

cap_t CONST Arch_updateCapData(bool_t preserve, word_t data, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#ifdef CONFIG_IOMMU
    switch (cap_get_capType(cap)) {
    pak_sa cap_io_space_cap: {
        seL4_X86_IOSpace_CapData_t w = { { data } };
        uint16_t PCIDevice = get_dev_id(
                                 seL4_X86_IOSpace_CapData_get_pciBus(w),
                                 seL4_X86_IOSpace_CapData_get_pciDev(w),
                                 seL4_X86_IOSpace_CapData_get_pciFun(w)
                             );
        uint16_t domainID = seL4_X86_IOSpace_CapData_get_domainID(w);
        නම් (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= MASK(x86KSnumIODomainIDBits)) {
            ආපසු cap_io_space_cap_new(domainID, PCIDevice);
        } නැතහොත් {
            ආපසු cap_null_cap_new();
        }
    }

    default:
        ආපසු cap;
    }
#endif
    ආපසු cap;
}

cap_t CONST Arch_maskCapRights(seL4_CapRights_t cap_rights_mask, cap_t cap)
{
    නම් (cap_get_capType(cap) == cap_frame_cap) {
        vm_rights_t vm_rights;

        vm_rights = vmRightsFromWord(cap_frame_cap_get_capFVMRights(cap));
        vm_rights = maskVMRights(vm_rights, cap_rights_mask);
        ආපසු cap_frame_cap_set_capFVMRights(cap, wordFromVMRights(vm_rights));
    } නැතහොත් {
        ආපසු cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    switch (cap_get_capType(cap)) {
    pak_sa cap_page_directory_cap:
        නම් (final && cap_page_directory_cap_get_capPDIsMapped(cap)) {
            unmapPageDirectory(
                cap_page_directory_cap_get_capPDMappedASID(cap),
                cap_page_directory_cap_get_capPDMappedAddress(cap),
                PDE_PTR(cap_page_directory_cap_get_capPDBasePtr(cap))
            );
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        නම් (final && cap_page_table_cap_get_capPTIsMapped(cap)) {
            unmapPageTable(
                cap_page_table_cap_get_capPTMappedASID(cap),
                cap_page_table_cap_get_capPTMappedAddress(cap),
                PT_PTR(cap_page_table_cap_get_capPTBasePtr(cap))
            );
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        නම් (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        bha_nga;
    pak_sa cap_asid_control_cap:
    pak_sa cap_io_port_control_cap:
        bha_nga;
    pak_sa cap_io_port_cap:
#ifdef CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_port_cap_get_capIOPortVPID(cap),
                                cap_io_port_cap_get_capIOPortFirstPort(cap),
                                cap_io_port_cap_get_capIOPortLastPort(cap));
#endif
        නම් (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            ආපසු fc_ret;
        }
        bha_nga;
#ifdef CONFIG_IOMMU
    pak_sa cap_io_space_cap:
        නම් (final) {
            unmapVTDContextEntry(cap);
        }
        bha_nga;

    pak_sa cap_io_page_table_cap:
        නම් (final && cap_io_page_table_cap_get_capIOPTIsMapped(cap)) {
            deleteIOPageTable(cap);
        }
        bha_nga;
#endif

#ifdef CONFIG_VTX
    pak_sa cap_vcpu_cap:
        නම් (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        bha_nga;
    pak_sa cap_ept_pml4_cap:
        නම් (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_capPML4BasePtr(cap));
        }
        bha_nga;

    pak_sa cap_ept_pdpt_cap:
        නම් (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_capPDPTMappedAddress(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        bha_nga;

    pak_sa cap_ept_pd_cap:
        නම් (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_capPDMappedAddress(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        bha_nga;

    pak_sa cap_ept_pt_cap:
        නම් (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmapEPTPageTable(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_capPTMappedAddress(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        bha_nga;
#endif

    default:
        ආපසු Mode_finaliseCap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ආපසු fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    switch (cap_get_capType(cap_a)) {
    pak_sa cap_frame_cap:
        නම් (cap_get_capType(cap_b) == cap_frame_cap) {
            word_t botA, botB, topA, topB;
            botA = cap_frame_cap_get_capFBasePtr(cap_a);
            botB = cap_frame_cap_get_capFBasePtr(cap_b);
            topA = botA + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_a)));
            topB = botB + MASK(pageBitsForSize(cap_frame_cap_get_capFSize(cap_b)));
            ආපසු ((botA <= botB) && (topA >= topB) && (botB <= topB));
        }
        bha_nga;

    pak_sa cap_page_table_cap:
        නම් (cap_get_capType(cap_b) == cap_page_table_cap) {
            ආපසු cap_page_table_cap_get_capPTBasePtr(cap_a) ==
                   cap_page_table_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_page_directory_cap:
        නම් (cap_get_capType(cap_b) == cap_page_directory_cap) {
            ආපසු cap_page_directory_cap_get_capPDBasePtr(cap_a) ==
                   cap_page_directory_cap_get_capPDBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_asid_control_cap:
        නම් (cap_get_capType(cap_b) == cap_asid_control_cap) {
            ආපසු true;
        }
        bha_nga;

    pak_sa cap_asid_pool_cap:
        නම් (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            ආපසු cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        bha_nga;

    pak_sa cap_io_port_control_cap:
        නම් (cap_get_capType(cap_b) == cap_io_port_control_cap ||
            cap_get_capType(cap_b) == cap_io_port_cap) {
            ආපසු true;
        }
        bha_nga;

    pak_sa cap_io_port_cap:
        නම් (cap_get_capType(cap_b) == cap_io_port_cap) {
            ආපසු  cap_io_port_cap_get_capIOPortFirstPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortFirstPort(cap_b) &&
                    cap_io_port_cap_get_capIOPortLastPort(cap_a) ==
                    cap_io_port_cap_get_capIOPortLastPort(cap_b);
        }
        bha_nga;

#ifdef CONFIG_IOMMU
    pak_sa cap_io_space_cap:
        නම් (cap_get_capType(cap_b) == cap_io_space_cap) {
            ආපසු cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        bha_nga;

    pak_sa cap_io_page_table_cap:
        නම් (cap_get_capType(cap_b) == cap_io_page_table_cap) {
            ආපසු cap_io_page_table_cap_get_capIOPTBasePtr(cap_a) ==
                   cap_io_page_table_cap_get_capIOPTBasePtr(cap_b);
        }
        bha_nga;
#endif

#ifdef CONFIG_VTX
    pak_sa cap_vcpu_cap:
        නම් (cap_get_capType(cap_b) == cap_vcpu_cap) {
            ආපසු cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        bha_nga;

    pak_sa cap_ept_pml4_cap:
        නම් (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            ආපසු cap_ept_pml4_cap_get_capPML4BasePtr(cap_a) ==
                   cap_ept_pml4_cap_get_capPML4BasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_ept_pdpt_cap:
        නම් (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            ආපසු cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_ept_pd_cap:
        නම් (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            ආපසු cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        bha_nga;

    pak_sa cap_ept_pt_cap:
        නම් (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            ආපසු cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        bha_nga;

#endif

    }

    ආපසු Mode_sameRegionAs(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    නම් (cap_get_capType(cap_a) == cap_io_port_control_cap &&
        cap_get_capType(cap_b) == cap_io_port_cap) {
        ආපසු false;
    }
    නම් (cap_get_capType(cap_a) == cap_frame_cap) {
        නම් (cap_get_capType(cap_b) == cap_frame_cap) {
            ආපසු ((cap_frame_cap_get_capFBasePtr(cap_a) ==
                     cap_frame_cap_get_capFBasePtr(cap_b)) &&
                    (cap_frame_cap_get_capFSize(cap_a) ==
                     cap_frame_cap_get_capFSize(cap_b)) &&
                    ((cap_frame_cap_get_capFIsDevice(cap_a) == 0) ==
                     (cap_frame_cap_get_capFIsDevice(cap_b) == 0)));
        }
    }
    ආපසු Arch_sameRegionAs(cap_a, cap_b);
}

word_t Arch_getObjectSize(word_t t)
{
    switch (t) {
    pak_sa seL4_X86_4K:
        ආපසු pageBitsForSize(X86_SmallPage);
    pak_sa seL4_X86_LargePageObject:
        ආපසු pageBitsForSize(X86_LargePage);
    pak_sa seL4_X86_PageTableObject:
        ආපසු seL4_PageTableBits;
    pak_sa seL4_X86_PageDirectoryObject:
        ආපසු seL4_PageDirBits;
    pak_sa seL4_IA32_PDPTObject:
        ආපසු seL4_PDPTBits;
    pak_sa seL4_X86_IOPageTableObject:
        ආපසු seL4_IOPageTableBits;
#ifdef CONFIG_VTX
    pak_sa seL4_X86_VCPUObject:
        ආපසු seL4_X86_VCPUBits;
    pak_sa seL4_X86_EPTPML4Object:
        ආපසු seL4_X86_EPTPML4Bits;
    pak_sa seL4_X86_EPTPDPTObject:
        ආපසු seL4_X86_EPTPDPTBits;
    pak_sa seL4_X86_EPTPDObject:
        ආපසු seL4_X86_EPTPDBits;
    pak_sa seL4_X86_EPTPTObject:
        ආපසු seL4_X86_EPTPTBits;
#endif
    default:
        ආපසු Mode_getObjectSize(t);
    }
}

cap_t Arch_createObject(object_t t, void *regionBase, word_t userSize, bool_t deviceMemory)
{
#ifdef CONFIG_VTX
    switch (t) {
    pak_sa seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((word_t)regionBase);
        vcpu_init(vcpu);
        ආපසු cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    pak_sa seL4_X86_EPTPML4Object:
        ආපසු cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (word_t)regionBase  /* capPML4BasePtr       */
               );
    pak_sa seL4_X86_EPTPDPTObject:
        ආපසු cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (word_t)regionBase   /* capPDPTBasePtr      */
               );
    pak_sa seL4_X86_EPTPDObject:
        ආපසු cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (word_t)regionBase  /* capPDBasePtr         */
               );
    pak_sa seL4_X86_EPTPTObject:
        ආපසු cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (word_t)regionBase  /* capPTBasePtr         */
               );
    default:
#endif
        ආපසු Mode_createObject(t, regionBase, userSize, deviceMemory);
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
    switch (cap_get_capType(cap)) {
    pak_sa cap_asid_control_cap:
    pak_sa cap_asid_pool_cap:
        ආපසු decodeX86MMUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    pak_sa cap_io_port_control_cap:
        ආපසු decodeX86PortControlInvocation(invLabel, length, cptr, slot, cap, buffer);
    pak_sa cap_io_port_cap:
        ආපසු decodeX86PortInvocation(invLabel, length, cptr, slot, cap, call, buffer);
#ifdef CONFIG_IOMMU
    pak_sa cap_io_space_cap:
        ආපසු decodeX86IOSpaceInvocation(invLabel, cap);
    pak_sa cap_io_page_table_cap:
        ආපසු decodeX86IOPTInvocation(invLabel, length, slot, cap, buffer);
#endif
#ifdef CONFIG_VTX
    pak_sa cap_vcpu_cap:
        ආපසු decodeX86VCPUInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    pak_sa cap_ept_pml4_cap:
    pak_sa cap_ept_pdpt_cap:
    pak_sa cap_ept_pd_cap:
    pak_sa cap_ept_pt_cap:
        ආපසු decodeX86EPTInvocation(invLabel, length, cptr, slot, cap, buffer);
#endif
    default:
        ආපසු Mode_decodeInvocation(invLabel, length, cptr, slot, cap, call, buffer);
    }
}

void Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

void Arch_postCapDeletion(cap_t cap)
{
    නම් (cap_get_capType(cap) == cap_io_port_cap) {
        uint16_t first_port = cap_io_port_cap_get_capIOPortFirstPort(cap);
        uint16_t last_port = cap_io_port_cap_get_capIOPortLastPort(cap);

        freeIOPortRange(first_port, last_port);
    }
}
