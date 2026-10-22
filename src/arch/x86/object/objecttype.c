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

deriveCap_ret_t Arch_deriveCap(cte_t *جایگاه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_جدول_cap:
        إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_صفحه_directory_cap:
        إذا (cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_پورت_control_cap:
        ret.وضعیت = EXCEPTION_NONE;
        ret.cap = cap_null_cap_new();
        أرجع ret;
    حالة cap_io_پورت_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_io_صفحه_جدول_cap:
        إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#پایان_اگر

#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    حالة cap_ept_pml4_cap:
        إذا (cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a EPT PML4 cap without an assigned ASID.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
    حالة cap_ept_pdpt_cap:
        إذا (cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PDPT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pd_cap:
        إذا (cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PD cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_ept_pt_cap:
        إذا (cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped EPT PT cap.");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#پایان_اگر

    افتراضي:
        أرجع نما_derive_Cap(جایگاه, cap);
    }
}

cap_t CONST Arch_update_Cap_داده(bool_t preserve, کلمه_t داده, cap_t cap)
{
    /* Avoid a switch statement with just a 'default' case as the C parser does not like this */
#اگر_تعریف_شده CONFIG_IOMMU
    بدل (cap_get_capType(cap)) {
    حالة cap_io_space_cap: {
        se_L4_X86_IOSpace_Cap_داده_t w = { { داده } };
        uint16_t PCIDevice = get_dev_id(
                                 se_L4_X86_IOSpace_Cap_داده_get_pci_Bus(w),
                                 se_L4_X86_IOSpace_Cap_داده_get_pci_Dev(w),
                                 se_L4_X86_IOSpace_Cap_داده_get_pci_Fun(w)
                             );
        uint16_t domainID = se_L4_X86_IOSpace_Cap_داده_get_domain_ID(w);
        إذا (!preserve && cap_io_space_cap_get_capPCIDevice(cap) == 0 &&
            domainID >= x86KSFirstValidIODomain &&
            domainID != 0                        &&
            domainID <= ماسک(x86KSnumIODomainIDBits)) {
            أرجع cap_io_space_cap_new(domainID, PCIDevice);
        } وإلا {
            أرجع cap_null_cap_new();
        }
    }

    افتراضي:
        أرجع cap;
    }
#پایان_اگر
    أرجع cap;
}

cap_t CONST Arch_ماسک_Cap_Rights(seL4_CapRights_t cap_rights_ماسک, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_قاب_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(cap_قاب_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);
        أرجع cap_قاب_cap_مجموعه_cap_FVMRights(cap, کلمه_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_directory_cap:
        إذا (final && cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap)) {
            unmap_صفحه_Directory(
                cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap),
                cap_صفحه_directory_cap_get_cap_PDMapped_آدرس(cap),
                PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_صفحه_جدول_cap:
        إذا (final && cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحه_جدول(
                cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap),
                PT_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap))
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
    حالة cap_io_پورت_control_cap:
        اكسر;
    حالة cap_io_پورت_cap:
#اگر_تعریف_شده CONFIG_VTX
        clearVPIDIOPortMappings(cap_io_پورت_cap_get_cap_IOPort_VPID(cap),
                                cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap),
                                cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap));
#پایان_اگر
        إذا (final) {
            fc_ret.remainder = cap_null_cap_new();
            fc_ret.cleanupInfo = cap;
            أرجع fc_ret;
        }
        اكسر;
#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap:
        إذا (final) {
            unmap_VTDContext_مدخل(cap);
        }
        اكسر;

    حالة cap_io_صفحه_جدول_cap:
        إذا (final && cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        اكسر;
#پایان_اگر

#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
    حالة cap_ept_pml4_cap:
        إذا (final && cap_ept_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteEPTASID(cap_ept_pml4_cap_get_capPML4MappedASID(cap),
                          (ept_pml4e_t *)cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(cap));
        }
        اكسر;

    حالة cap_ept_pdpt_cap:
        إذا (final && cap_ept_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapEPTPDPT(
                cap_ept_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_ept_pdpt_cap_get_cap_PDPTMapped_آدرس(cap),
                (ept_pdpte_t *)cap_ept_pdpt_cap_get_capPDPTBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pd_cap:
        إذا (final && cap_ept_pd_cap_get_capPDIsMapped(cap)) {
            unmapEPTPageDirectory(
                cap_ept_pd_cap_get_capPDMappedASID(cap),
                cap_ept_pd_cap_get_cap_PDMapped_آدرس(cap),
                (ept_pde_t *)cap_ept_pd_cap_get_capPDBasePtr(cap));
        }
        اكسر;

    حالة cap_ept_pt_cap:
        إذا (final && cap_ept_pt_cap_get_capPTIsMapped(cap)) {
            unmap_EPTPage_جدول(
                cap_ept_pt_cap_get_capPTMappedASID(cap),
                cap_ept_pt_cap_get_cap_PTMapped_آدرس(cap),
                (ept_pte_t *)cap_ept_pt_cap_get_capPTBasePtr(cap));
        }
        اكسر;
#پایان_اگر

    افتراضي:
        أرجع نما_finalise_Cap(cap, final);
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_قاب_cap:
        إذا (cap_get_capType(cap_b) == cap_قاب_cap) {
            کلمه_t botA, botB, بالاترین_A, بالاترین_B;
            botA = cap_قاب_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_قاب_cap_get_cap_FBase_Ptr(cap_b);
            بالاترین_A = botA + ماسک(صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap_a)));
            بالاترین_B = botB + ماسک(صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap_b)));
            أرجع ((botA <= botB) && (بالاترین_A >= بالاترین_B) && (botB <= بالاترین_B));
        }
        اكسر;

    حالة cap_صفحه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحه_جدول_cap) {
            أرجع cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        اكسر;

    حالة cap_صفحه_directory_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحه_directory_cap) {
            أرجع cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap_b);
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

    حالة cap_io_پورت_control_cap:
        إذا (cap_get_capType(cap_b) == cap_io_پورت_control_cap ||
            cap_get_capType(cap_b) == cap_io_پورت_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_io_پورت_cap:
        إذا (cap_get_capType(cap_b) == cap_io_پورت_cap) {
            أرجع  cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap_a) ==
                    cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap_b) &&
                    cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap_a) ==
                    cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap_b);
        }
        اكسر;

#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap:
        إذا (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capPCIDevice(cap_a) ==
                   cap_io_space_cap_get_capPCIDevice(cap_b);
        }
        اكسر;

    حالة cap_io_صفحه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_io_صفحه_جدول_cap) {
            أرجع cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        اكسر;
#پایان_اگر

#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;

    حالة cap_ept_pml4_cap:
        إذا (cap_get_capType(cap_b) == cap_ept_pml4_cap) {
            أرجع cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(cap_a) ==
                   cap_ept_pml4_cap_get_cap_PML4_پایه_Ptr(cap_b);
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

#پایان_اگر

    }

    أرجع نما_same_Region_As(cap_a, cap_b);
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_io_پورت_control_cap &&
        cap_get_capType(cap_b) == cap_io_پورت_cap) {
        أرجع false;
    }
    إذا (cap_get_capType(cap_a) == cap_قاب_cap) {
        إذا (cap_get_capType(cap_b) == cap_قاب_cap) {
            أرجع ((cap_قاب_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_قاب_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_قاب_cap_get_cap_FSize(cap_a) ==
                     cap_قاب_cap_get_cap_FSize(cap_b)) &&
                    ((cap_قاب_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_قاب_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

کلمه_t Arch_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة seL4_X86_4K:
        أرجع صفحه_بیتها_For_اندازه(X86_Small_صفحه);
    حالة se_L4_X86_Large_صفحه_Object:
        أرجع صفحه_بیتها_For_اندازه(X86_Large_صفحه);
    حالة se_L4_X86_صفحه_جدول_Object:
        أرجع se_L4_صفحه_جدول_بیتها;
    حالة se_L4_X86_صفحه_Directory_Object:
        أرجع se_L4_صفحه_Dir_بیتها;
    حالة seL4_IA32_PDPTObject:
        أرجع seL4_PDPTBits;
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع se_L4_IOPage_جدول_بیتها;
#اگر_تعریف_شده CONFIG_VTX
    حالة seL4_X86_VCPUObject:
        أرجع seL4_X86_VCPUBits;
    حالة seL4_X86_EPTPML4Object:
        أرجع se_L4_X86_EPTPML4_بیتها;
    حالة seL4_X86_EPTPDPTObject:
        أرجع seL4_X86_EPTPDPTBits;
    حالة seL4_X86_EPTPDObject:
        أرجع seL4_X86_EPTPDBits;
    حالة seL4_X86_EPTPTObject:
        أرجع seL4_X86_EPTPTBits;
#پایان_اگر
    افتراضي:
        أرجع نما_get_Object_اندازه(t);
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
#اگر_تعریف_شده CONFIG_VTX
    بدل (t) {
    حالة seL4_X86_VCPUObject: {
        vcpu_t *vcpu;
        vcpu = VCPU_PTR((کلمه_t)region_پایه);
        vcpu_init(vcpu);
        أرجع cap_vcpu_cap_new(VCPU_REF(vcpu));
    }
    حالة seL4_X86_EPTPML4Object:
        أرجع cap_ept_pml4_cap_new(
                   0,                  /* capPML4IsMapped      */
                   VPID_INVALID,       /* capPML4MappedASID    */
                   (کلمه_t)region_پایه  /* capPML4BasePtr       */
               );
    حالة seL4_X86_EPTPDPTObject:
        أرجع cap_ept_pdpt_cap_new(
                   0,                  /* capPDPTMappedAddress */
                   0,                  /* capPDPTIsMapped      */
                   VPID_INVALID,       /* capPDPTMappedASID    */
                   (کلمه_t)region_پایه   /* capPDPTBasePtr      */
               );
    حالة seL4_X86_EPTPDObject:
        أرجع cap_ept_pd_cap_new(
                   0,                  /* capPDMappedAddress   */
                   0,                  /* capPDIsMapped        */
                   VPID_INVALID,       /* capPDMappedASID      */
                   (کلمه_t)region_پایه  /* capPDBasePtr         */
               );
    حالة seL4_X86_EPTPTObject:
        أرجع cap_ept_pt_cap_new(
                   0,                  /* capPTMappedAddress   */
                   0,                  /* capPTIsMapped        */
                   VPID_INVALID,       /* capPTMappedASID      */
                   (کلمه_t)region_پایه  /* capPTBasePtr         */
               );
    افتراضي:
#پایان_اگر
        أرجع نما_create_Object(t, region_پایه, user_اندازه, device_حافظه);
#اگر_تعریف_شده CONFIG_VTX
    }
#پایان_اگر
}

exception_t Arch_decodeInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        أرجع decodeX86MMUInvocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
    حالة cap_io_پورت_control_cap:
        أرجع decode_X86_پورت_Control_Invocation(invLabel, درازی, cptr, جایگاه, cap, میانگیر);
    حالة cap_io_پورت_cap:
        أرجع decode_X86_پورت_Invocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap:
        أرجع decodeX86IOSpaceInvocation(invLabel, cap);
    حالة cap_io_صفحه_جدول_cap:
        أرجع decodeX86IOPTInvocation(invLabel, درازی, جایگاه, cap, میانگیر);
#پایان_اگر
#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap:
        أرجع decodeX86VCPUInvocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
    حالة cap_ept_pml4_cap:
    حالة cap_ept_pdpt_cap:
    حالة cap_ept_pd_cap:
    حالة cap_ept_pt_cap:
        أرجع decodeX86EPTInvocation(invLabel, درازی, cptr, جایگاه, cap, میانگیر);
#پایان_اگر
    افتراضي:
        أرجع نما_decode_Invocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
    }
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
    /* Notify the lazy FPU module about this thread's deletion. */
    fpuRelease(thread);
}

فراغ Arch_postCapDeletion(cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_io_پورت_cap) {
        uint16_t اولی_پورت = cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap);
        uint16_t آخرین_پورت = cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap);

        freeIOPortRange(اولی_پورت, آخرین_پورت);
    }
}
