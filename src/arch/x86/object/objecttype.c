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

deriveCap_ret_t Arch_deriveCap(cte_t *تھاں, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_جدول_cap:
        إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ورقہ_directory_cap:
        إذا (cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_پورٹ_control_cap:
        ret.حالت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    حالة cap_io_پورٹ_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_ورقہ_جدول_cap:
        إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#جے_انت

#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_ept_pml4_cap:
        إذا (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
    حالة cap_ept_pdpt_cap:
        إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pd_cap:
        إذا (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pt_cap:
        إذا (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#جے_انت

    افتراضي:
        أرجع منوال_derive_Cap(تھاں, cap);
    }
}

cap_t CONST Arch_update_Cap_انکڑے(bool_t preserve, لفظ_t انکڑے, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#جے_تعریف_شدہ CONFIG_IOMMU
    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_انکڑے_t w = { { انکڑے } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_انکڑے_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_انکڑے_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_انکڑے_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_انکڑے_get_domain_ID(w);
        إذا (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= نقاب(x86KSnumIODomainIDBits)) {
            أرجع cap_io_space_cap_new(domainID, PCIDevice);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    }

    افتراضي:
        أرجع cap;
    }
#جے_انت
    أرجع cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        أرجع cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap, لفظ_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_directory_cap:
        إذا (final && cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_ورقہ_Directory(
                cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_ورقہ_directory_cap_get_cap_PDMapped_پتہ(cap),
                PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_ورقہ_جدول_cap:
        إذا (final && cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ورقہ_جدول(
                cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
                PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_asid_pool_cap:
        إذا (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        اكسر;
    حالة cap_asid_control_cap:
    حالة cap_io_پورٹ_control_cap:
        اكسر;
    حالة cap_io_پورٹ_cap:
#جے_تعریف_شدہ CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_پورٹ_cap_get_cap_IOPort_VPID(cap),
                                cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap),
                                cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap));
#جے_انت
        إذا (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            أرجع fc_ret;
        }
        اكسر;
#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap:
        إذا (final) {
            unmap_VTDContext_اندراج(cap);
        }
        اكسر;

    حالة cap_io_ورقہ_جدول_cap:
        إذا (final && cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        اكسر;
#جے_انت

#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
    حالة cap_ept_pml4_cap:
        إذا (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_ادھار_Ptr(cap));
        }
        اكسر;

    حالة cap_ept_pdpt_cap:
        إذا (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pd_cap:
        إذا (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_پتہ(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pt_cap:
        إذا (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_جدول(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_پتہ(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        اكسر;
#جے_انت

    افتراضي:
        أرجع منوال_finalise_Cap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_چوکھٹا_cap:
        إذا (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(cap_b)));
            أرجع ((botA <= botB) && (چوٹی_A >= چوٹی_B) && (botB <= چوٹی_B));
        }
        اكسر;

    حالة cap_ورقہ_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_ورقہ_جدول_cap) {
            أرجع cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        اكسر;

    حالة cap_ورقہ_directory_cap:
        إذا (cap_get_capType(cap_b) == cap_ورقہ_directory_cap) {
            أرجع cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(cap_b);
        }
        اكسر;

    حالة cap_asid_control_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_control_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            أرجع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;

    حالة cap_io_پورٹ_control_cap:
        إذا (cap_get_capType(cap_b) == cap_io_پورٹ_control_cap ||
            cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_io_پورٹ_cap:
        إذا (cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
            أرجع  cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap_a) ==
                    cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap_b) &&
                    cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap_a) ==
                    cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap_b);
        }
        اكسر;

#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap:
        إذا (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        اكسر;

    حالة cap_io_ورقہ_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_io_ورقہ_جدول_cap) {
            أرجع cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        اكسر;
#جے_انت

#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pml4_cap:
        إذا (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            أرجع cap_ept_pml4_cap_get_cap_PML4_ادھار_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_ادھار_Ptr(cap_b);
        }
        اكسر;

    حالة cap_ept_pdpt_cap:
        إذا (cap_get_capType(cap_b) == cap_ept_pdpt_cap) {
            أرجع cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_ept_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pd_cap:
        إذا (cap_get_capType(cap_b) == cap_ept_pd_cap) {
            أرجع cap_ept_pd_cap_get_capPDBasePtr(cap_a) ==
                   cap_ept_pd_cap_get_capPDBasePtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pt_cap:
        إذا (cap_get_capType(cap_b) == cap_ept_pt_cap) {
            أرجع cap_ept_pt_cap_get_capPTBasePtr(cap_a) ==
                   cap_ept_pt_cap_get_capPTBasePtr(cap_b);
        }
        اكسر;

#جے_انت

    }

    أرجع منوال_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_io_پورٹ_control_cap &&
        cap_get_capType(cap_b) == cap_io_پورٹ_cap) {
        أرجع false;
    }
    إذا (cap_get_capType(cap_a) == cap_چوکھٹا_cap) {
        إذا (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            أرجع ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_ناپ(لفظ_t t)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع ورقہ_بٹ_For_ناپ(X86_Small_ورقہ);
    حالة se_L4_X86_Large_ورقہ_Object:
        أرجع ورقہ_بٹ_For_ناپ(X86_Large_ورقہ);
    حالة se_L4_X86_ورقہ_جدول_Object:
        أرجع se_L4_ورقہ_جدول_بٹ;
    حالة se_L4_X86_ورقہ_Directory_Object:
        أرجع se_L4_ورقہ_Dir_بٹ;
    حالة seL4_IA32_PDPTObject:
        أرجع seL4_PDPTBits;
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع se_L4_IOPage_جدول_بٹ;
#جے_تعریف_شدہ CONFIG_VTX
    حالة seL4_X86_VCPUObject:
        أرجع seL4_X86_VCPUBits;
    حالة seL4_X86_EPTPML4Object:
        أرجع se_L4_X86_EPTPML4_بٹ;
    حالة seL4_X86_EPTPDPTObject:
        أرجع seL4_X86_EPTPDPTBits;
    حالة seL4_X86_EPTPDObject:
        أرجع seL4_X86_EPTPDBits;
    حالة seL4_X86_EPTPTObject:
        أرجع seL4_X86_EPTPTBits;
#جے_انت
    افتراضي:
        أرجع منوال_get_Object_ناپ(t);
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_ادھار, لفظ_t user_ناپ, bool_t device_حافظہ)
{
#جے_تعریف_شدہ CONFIG_VTX
    بدل (t) {
    حالة seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((لفظ_t)region_ادھار);
        vcpu_init(vcpu);
        أرجع cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    حالة seL4_X86_EPTPML4Object:
        أرجع cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (لفظ_t)region_ادھار  /* capPML4BasePtr       */
               );
    حالة seL4_X86_EPTPDPTObject:
        أرجع cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (لفظ_t)region_ادھار   /* capPDPTBasePtr      */
               );
    حالة seL4_X86_EPTPDObject:
        أرجع cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (لفظ_t)region_ادھار  /* capPDBasePtr         */
               );
    حالة seL4_X86_EPTPTObject:
        أرجع cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (لفظ_t)region_ادھار  /* capPTBasePtr         */
               );
    افتراضي:
#جے_انت
        أرجع منوال_create_Object(t, region_ادھار, user_ناپ, device_حافظہ);
#جے_تعریف_شدہ CONFIG_VTX
    }
#جے_انت
}

exception_t Arch_decodeInvocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *تھاں,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        أرجع decodeX86MMUInvocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
    حالة cap_io_پورٹ_control_cap:
        أرجع decode_X86_پورٹ_Control_Invocation(invLabel, لمائی, cptr, تھاں, cap, بفر);
    حالة cap_io_پورٹ_cap:
        أرجع decode_X86_پورٹ_Invocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
#جے_تعریف_شدہ CONFIG_IOMMU
    حالة cap_io_space_cap:
        أرجع decodeX86IOSpaceInvocation(invLabel, cap);
    حالة cap_io_ورقہ_جدول_cap:
        أرجع decodeX86IOPTInvocation(invLabel, لمائی, تھاں, cap, بفر);
#جے_انت
#جے_تعریف_شدہ CONFIG_VTX
    حالة cap_vcpu_cap:
        أرجع decodeX86VCPUInvocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
    حالة cap_ept_pml4_cap:
    حالة cap_ept_pdpt_cap:
    حالة cap_ept_pd_cap:
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTInvocation(invLabel, لمائی, cptr, تھاں, cap, بفر);
#جے_انت
    افتراضي:
        أرجع منوال_decode_Invocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
    }
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

فراغ Arch_postCapDeletion(cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_io_پورٹ_cap) {
        uint16_t پہلا_پورٹ = cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(cap);
        uint16_t آخری_پورٹ = cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(cap);

        freeIOPortRange(پہلا_پورٹ, آخری_پورٹ);
    }
}
