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

deriveCap_ret_t Arch_deriveCap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_جدول_cap:
        اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_صفحہ_directory_cap:
        اگر (cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    حالت cap_io_پورٹ_control_cap:
        ret.حالت_2 = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        واپس ret;
    حالت cap_io_پورٹ_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    حالت cap_io_صفحہ_جدول_cap:
        اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
    حالت cap_ept_pml4_cap:
        اگر (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;
    حالت cap_ept_pdpt_cap:
        اگر (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_ept_pd_cap:
        اگر (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_ept_pt_cap:
        اگر (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;
#اگر_ختم

    طےشدہ:
        واپس منوال_derive_Cap(خانہ, cap);
    }
}

cap_t CONST Arch_update_Cap_ڈیٹا(bool_t preserve, لفظ_t ڈیٹا, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#اگر_تعریف_شدہ CONFIG_IOMMU
    انتخاب (cap_get_capType(cap)) {
    حالت cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_ڈیٹا_t w = { { ڈیٹا } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_ڈیٹا_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_ڈیٹا_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_ڈیٹا_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_ڈیٹا_get_domain_ID(w);
        اگر (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= نقاب(x86KSnumIODomainIDBits)) {
            واپس cap_io_space_cap_new(domainID, PCIDevice);
        } ورنہ {
            واپس cap_null_cap_new();
        }
    }

    طےشدہ:
        واپس cap;
    }
#اگر_ختم
    واپس cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        واپس cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap, لفظ_From_VMRights(vm_rights));
    } ورنہ {
        واپس cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_directory_cap:
        اگر (final && cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_صفحہ_Directory(
                cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_صفحہ_directory_cap_get_cap_PDMapped_پتہ(cap),
                PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (final && cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحہ_جدول(
                cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
                PT_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        توڑو;

    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        توڑو;
    حالت cap_asid_control_cap:
    حالت cap_io_پورٹ_control_cap:
        توڑو;
    حالت cap_io_پورٹ_cap:
#اگر_تعریف_شدہ CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_پورٹ_cap_get_cap_IOPort_VPID(cap),
                                cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
                                cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
#اگر_ختم
        اگر (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            واپس fc_ret;
        }
        توڑو;
#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap:
        اگر (final) {
            unmap_VTDContext_اندراج(cap);
        }
        توڑو;

    حالت cap_io_صفحہ_جدول_cap:
        اگر (final && cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        توڑو;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        توڑو;
    حالت cap_ept_pml4_cap:
        اگر (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap));
        }
        توڑو;

    حالت cap_ept_pdpt_cap:
        اگر (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        توڑو;

    حالت cap_ept_pd_cap:
        اگر (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_پتہ(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        توڑو;

    حالت cap_ept_pt_cap:
        اگر (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_جدول(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_پتہ(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        توڑو;
#اگر_ختم

    طےشدہ:
        واپس منوال_finalise_Cap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_چوکھٹا_cap:
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_b)));
            واپس ((botA <= botB) && (چوٹی_A >= چوٹی_B) && (botB <= چوٹی_B));
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (cap_get_capType(cap_b) == cap_صفحہ_جدول_cap) {
            واپس cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        توڑو;

    حالت cap_صفحہ_directory_cap:
        اگر (cap_get_capType(cap_b) == cap_صفحہ_directory_cap) {
            واپس cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap_b);
        }
        توڑو;

    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            واپس true;
        }
        توڑو;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            واپس cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        توڑو;

    حالت cap_io_پورٹ_control_cap:
        اگر (cap_get_capType(cap_b) == cap_io_پورٹ_control_cap ||
            cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
            واپس true;
        }
        توڑو;

    حالت cap_io_پورٹ_cap:
        اگر (cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
            واپس  cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap_a) ==
                    cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap_b) &&
                    cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap_a) ==
                    cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap_b);
        }
        توڑو;

#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap:
        اگر (cap_get_capType(cap_b) == cap_io_space_cap) {
            واپس cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        توڑو;

    حالت cap_io_صفحہ_جدول_cap:
        اگر (cap_get_capType(cap_b) == cap_io_صفحہ_جدول_cap) {
            واپس cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        توڑو;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            واپس cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        توڑو;

    حالت cap_ept_pml4_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            واپس cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap_b);
        }
        توڑو;

    حالت cap_ept_pdpt_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            واپس cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        توڑو;

    حالت cap_ept_pd_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            واپس cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        توڑو;

    حالت cap_ept_pt_cap:
        اگر (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            واپس cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        توڑو;

#اگر_ختم

    }

    واپس منوال_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_io_پورٹ_control_cap &&
        cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
        واپس false;
    }
    اگر (cap_get_capType(cap_a) == cap_چوکھٹا_cap) {
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            واپس ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
    واپس Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائز(لفظ_t t)
{
    انتخاب (t) {
    حالت seL4_X86_4K:
        واپس صفحہ_بٹ_For_سائز(X86_Small_صفحہ);
    حالت se_L4_X86_Large_صفحہ_Object:
        واپس صفحہ_بٹ_For_سائز(X86_Large_صفحہ);
    حالت se_L4_X86_صفحہ_جدول_Object:
        واپس se_L4_صفحہ_جدول_بٹ;
    حالت se_L4_X86_صفحہ_Directory_Object:
        واپس se_L4_صفحہ_Dir_بٹ;
    حالت seL4_IA32_PDPTObject:
        واپس seL4_PDPTBits;
    حالت se_L4_X86_IOPage_جدول_Object:
        واپس se_L4_IOPage_جدول_بٹ;
#اگر_تعریف_شدہ CONFIG_VTX
    حالت seL4_X86_VCPUObject:
        واپس seL4_X86_VCPUBits;
    حالت seL4_X86_EPTPML4Object:
        واپس se_L4_X86_EPTPML4_بٹ;
    حالت seL4_X86_EPTPDPTObject:
        واپس seL4_X86_EPTPDPTBits;
    حالت seL4_X86_EPTPDObject:
        واپس seL4_X86_EPTPDBits;
    حالت seL4_X86_EPTPTObject:
        واپس seL4_X86_EPTPTBits;
#اگر_ختم
    طےشدہ:
        واپس منوال_get_Object_سائز(t);
    }
}

cap_t Arch_createObject(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
#اگر_تعریف_شدہ CONFIG_VTX
    انتخاب (t) {
    حالت seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((لفظ_t)region_بنیاد);
        vcpu_init(vcpu);
        واپس cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    حالت seL4_X86_EPTPML4Object:
        واپس cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (لفظ_t)region_بنیاد  /* capPML4BasePtr       */
               );
    حالت seL4_X86_EPTPDPTObject:
        واپس cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (لفظ_t)region_بنیاد   /* capPDPTBasePtr      */
               );
    حالت seL4_X86_EPTPDObject:
        واپس cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (لفظ_t)region_بنیاد  /* capPDBasePtr         */
               );
    حالت seL4_X86_EPTPTObject:
        واپس cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (لفظ_t)region_بنیاد  /* capPTBasePtr         */
               );
    طےشدہ:
#اگر_ختم
        واپس منوال_create_Object(t, region_بنیاد, user_سائز, device_حافظہ);
#اگر_تعریف_شدہ CONFIG_VTX
    }
#اگر_ختم
}

exception_t Arch_decodeInvocation(
    لفظ_t invLabel,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        واپس decodeX86MMUInvocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
    حالت cap_io_پورٹ_control_cap:
        واپس decode_X86_پورٹ_Control_Invocation(invLabel, لمبائی, cptr, خانہ, cap, بفر);
    حالت cap_io_پورٹ_cap:
        واپس decode_X86_پورٹ_Invocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت cap_io_space_cap:
        واپس decodeX86IOSpaceInvocation(invLabel, cap);
    حالت cap_io_صفحہ_جدول_cap:
        واپس decodeX86IOPTInvocation(invLabel, لمبائی, خانہ, cap, بفر);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_VTX
    حالت cap_vcpu_cap:
        واپس decodeX86VCPUInvocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
    حالت cap_ept_pml4_cap:
    حالت cap_ept_pdpt_cap:
    حالت cap_ept_pd_cap:
    حالت cap_ept_pt_cap:
        واپس decodeX86EPTInvocation(invLabel, لمبائی, cptr, خانہ, cap, بفر);
#اگر_ختم
    طےشدہ:
        واپس منوال_decode_Invocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
    }
}

خالی Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

خالی Arch_postCapDeletion(cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_io_پورٹ_cap) {
        uint16_t پہلا_پورٹ = cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap);
        uint16_t آخری_پورٹ = cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap);

        freeIOPortRange(پہلا_پورٹ, آخری_پورٹ);
    }
}
