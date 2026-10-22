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
#ضمن <arch/object/objecttype.h>
#ضمن <arch/machine/tlb.h>
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
#ضمن <arch/object/vcpu.h>
#پایان_اگر

bool_t Arch_is_قاب_Type(کلمه_t type)
{
    بدل (type) {
    حالة se_L4_ARM_Small_صفحه_Object:
        أرجع true;
    حالة se_L4_ARM_Large_صفحه_Object:
        أرجع true;
    حالة seL4_ARM_SectionObject:
        أرجع true;
    حالة seL4_ARM_SuperSectionObject:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

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

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    حالة cap_small_قاب_cap:
        ret.cap = cap_small_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asidInvalid);
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_قاب_cap:
        ret.cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asidInvalid);
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
#پایان_اگر

#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
#پایان_اگر

#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_io_صفحه_جدول_cap:
        إذا (cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#پایان_اگر
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_داده(bool_t preserve, کلمه_t داده, cap_t cap)
{
    أرجع cap;
}

cap_t CONST Arch_ماسک_Cap_Rights(seL4_CapRights_t cap_rights_ماسک, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_small_قاب_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(
                        cap_small_قاب_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);
        أرجع cap_small_قاب_cap_مجموعه_cap_FVMRights(cap,
                                                    کلمه_From_VMRights(vm_rights));
    } وإلا إذا (cap_get_capType(cap) == cap_قاب_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(
                        cap_قاب_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);
        أرجع cap_قاب_cap_مجموعه_cap_FVMRights(cap,
                                              کلمه_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_asid_pool_cap:
        إذا (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        اكسر;

    حالة cap_صفحه_directory_cap:
        إذا (final && cap_صفحه_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_صفحه_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_صفحه_جدول_cap:
        إذا (final && cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحه_جدول(
                cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap),
                PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_small_قاب_cap:
        إذا (cap_small_قاب_cap_get_cap_FMapped_ASID(cap)) {
#اگر_تعریف_شده CONFIG_TK1_SMMU
            إذا (is_IOSpace_قاب_Cap(cap)) {
                unmapIOPage(cap);
                اكسر;
            }
#پایان_اگر

            unmap_صفحه(ARMSmall_صفحه,
                      cap_small_قاب_cap_get_cap_FMapped_ASID(cap),
                      cap_small_قاب_cap_get_cap_FMapped_آدرس(cap),
                      (فراغ *)cap_small_قاب_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

    حالة cap_قاب_cap:
        إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap)) {
#اگر_تعریف_شده CONFIG_KERNEL_LOG_میانگیر
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            إذا (unlikely(cap_قاب_cap_get_cap_FSize(cap) == ARMSection)) {
                إذا (pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_میانگیر) {
                    ks_User_Log_میانگیر = 0;

                    /* Invalidate log page table entries */
                    clear_حافظه((فراغ *) armKSGlobalLogPT, se_L4_صفحه_جدول_بیتها);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + بیت(se_L4_صفحه_جدول_بیتها),
                                        addrFromKPPtr((فراغ *)&armKSGlobalLogPT[0]));

                    لكل (صحيح idx = 0; idx < بیت(PT_شاخص_بیتها); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_صفحه_بیتها));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#پایان_اگر /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_صفحه(cap_قاب_cap_get_cap_FSize(cap),
                      cap_قاب_cap_get_cap_FMapped_ASID(cap),
                      cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                      (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#پایان_اگر

#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#پایان_اگر


#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (final) {
            clearIOPageDirectory(cap);
        }
        اكسر;

    حالة cap_io_صفحه_جدول_cap:
        إذا (final && cap_io_صفحه_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        اكسر;
#پایان_اگر

    افتراضي:
        اكسر;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_small_قاب_cap:
    حالة cap_قاب_cap:
        إذا (cap_get_capType(cap_b) == cap_small_قاب_cap ||
            cap_get_capType(cap_b) == cap_قاب_cap) {
            کلمه_t botA, botB, بالاترین_A, بالاترین_B;
            botA = generic_قاب_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_قاب_cap_get_cap_FBase_Ptr(cap_b);
            بالاترین_A = botA + ماسک(صفحه_بیتها_For_اندازه(generic_قاب_cap_get_cap_FSize(cap_a)));
            بالاترین_B = botB + ماسک(صفحه_بیتها_For_اندازه(generic_قاب_cap_get_cap_FSize(cap_b))) ;
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

#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#پایان_اگر

#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        إذا (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#پایان_اگر


#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        اكسر;

    حالة cap_io_صفحه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_io_صفحه_جدول_cap) {
            أرجع cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        اكسر;
#پایان_اگر
    }

    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_small_قاب_cap) {
        إذا (cap_get_capType(cap_b) == cap_small_قاب_cap) {
            أرجع ((cap_small_قاب_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_قاب_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_قاب_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_قاب_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_قاب_cap) {
            أرجع false;
        }
    }
    إذا (cap_get_capType(cap_a) == cap_قاب_cap) {
        إذا (cap_get_capType(cap_b) == cap_قاب_cap) {
            أرجع ((cap_قاب_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_قاب_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_قاب_cap_get_cap_FSize(cap_a) ==
                     cap_قاب_cap_get_cap_FSize(cap_b)) &&
                    ((cap_قاب_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_قاب_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_small_قاب_cap) {
            أرجع false;
        }
    }
#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    إذا (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#پایان_اگر

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

کلمه_t Arch_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحه_Object:
        أرجع ARMSmall_صفحه_بیتها;
    حالة se_L4_ARM_Large_صفحه_Object:
        أرجع ARMLarge_صفحه_بیتها;
    حالة seL4_ARM_SectionObject:
        أرجع ARMSection_بیتها;
    حالة seL4_ARM_SuperSectionObject:
        أرجع ARMSuper_Section_بیتها;
    حالة se_L4_ARM_صفحه_جدول_Object:
        أرجع PTE_اندازه_بیتها + PT_شاخص_بیتها;
    حالة se_L4_ARM_صفحه_Directory_Object:
        أرجع PDE_اندازه_بیتها + PD_شاخص_بیتها;
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        أرجع se_L4_IOPage_جدول_بیتها;
#پایان_اگر
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_اندازه_بیتها;
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحه_Object:
        إذا (device_حافظه) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((کلمه_t)region_پایه,
                                (کلمه_t)region_پایه + ماسک(صفحه_بیتها_For_اندازه(ARMSmall_صفحه)),
                                addrFromPPtr(region_پایه));
        }
        أرجع cap_small_قاب_cap_new(
                   ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظه,
#اگر_تعریف_شده CONFIG_TK1_SMMU
                   0,
#پایان_اگر
                   ASID_بالا(asidInvalid),
                   (کلمه_t)region_پایه);

    حالة se_L4_ARM_Large_صفحه_Object:
        إذا (device_حافظه) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((کلمه_t)region_پایه,
                                (کلمه_t)region_پایه + ماسک(صفحه_بیتها_For_اندازه(ARMLarge_صفحه)),
                                addrFromPPtr(region_پایه));
        }
        أرجع cap_قاب_cap_new(
                   ARMLarge_صفحه, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظه, ASID_بالا(asidInvalid),
                   (کلمه_t)region_پایه);

    حالة seL4_ARM_SectionObject:
        إذا (device_حافظه) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } وإلا {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((کلمه_t)region_پایه,
                                (کلمه_t)region_پایه + ماسک(صفحه_بیتها_For_اندازه(ARMSection)),
                                addrFromPPtr(region_پایه));
        }
        أرجع cap_قاب_cap_new(
                   ARMSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظه, ASID_بالا(asidInvalid),
                   (کلمه_t)region_پایه);

    حالة seL4_ARM_SuperSectionObject:
        إذا (device_حافظه) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } وإلا {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((کلمه_t)region_پایه,
                                (کلمه_t)region_پایه + ماسک(صفحه_بیتها_For_اندازه(ARMSuperSection)),
                                addrFromPPtr(region_پایه));
        }
        أرجع cap_قاب_cap_new(
                   ARMSuperSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظه, ASID_بالا(asidInvalid),
                   (کلمه_t)region_پایه);

    حالة se_L4_ARM_صفحه_جدول_Object:
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((کلمه_t)region_پایه,
                            (کلمه_t)region_پایه + ماسک(se_L4_صفحه_جدول_بیتها),
                            addrFromPPtr(region_پایه));
        أرجع cap_صفحه_جدول_cap_new(false, asidInvalid, 0,
                                      (کلمه_t)region_پایه);

    حالة se_L4_ARM_صفحه_Directory_Object:
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_پایه);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((کلمه_t)region_پایه,
                            (کلمه_t)region_پایه + ماسک(se_L4_صفحه_Dir_بیتها),
                            addrFromPPtr(region_پایه));

        أرجع cap_صفحه_directory_cap_new(false, asidInvalid,
                                          (کلمه_t)region_پایه);
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_پایه));
        أرجع cap_vcpu_cap_new(VCPU_REF(region_پایه));
#پایان_اگر

#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        cleanCacheRange_RAM((کلمه_t)region_پایه,
                            (کلمه_t)region_پایه + ماسک(se_L4_IOPage_جدول_بیتها),
                            addrFromPPtr(region_پایه));
        أرجع cap_io_صفحه_جدول_cap_new(0, asidInvalid, (کلمه_t)region_پایه, 0);
#پایان_اگر
    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(کلمه_t invLabel, کلمه_t درازی, cptr_t cptr,
                                  cte_t *جایگاه, cap_t cap,
                                  bool_t call, کلمه_t *میانگیر)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#إذا defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#اگر_تعریف_شده CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        أرجع decodeARMIOSpaceInvocation(invLabel, cap);
    حالة cap_io_صفحه_جدول_cap:
        أرجع decodeARMIOPTInvocation(invLabel, درازی, جایگاه, cap, میانگیر);
#پایان_اگر
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
#پایان_اگر /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#اگر_تعریف_نشده CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(invLabel, درازی, cap, میانگیر);
#پایان_اگر /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    افتراضي:
#وإلا
{
#پایان_اگر
    أرجع decodeARMMMUInvocation(invLabel, درازی, cptr, جایگاه, cap, call, میانگیر);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#اگر_تعریف_شده CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#پایان_اگر /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_تعریف_شده CONFIG_HAVE_FPU
    fpuRelease(thread);
#پایان_اگر
}

