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
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
#ضمن <arch/object/vcpu.h>
#جيڪڏهن_پورو

bool_t Arch_is_چوکٺو_Type(لفظ_t type)
{
    بدل (type) {
    حالة se_L4_ARM_Small_صفحو_Object:
        أرجع true;
    حالة se_L4_ARM_Large_صفحو_Object:
        أرجع true;
    حالة seL4_ARM_SectionObject:
        أرجع true;
    حالة seL4_ARM_SuperSectionObject:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *جڳهه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_جدول_cap:
        إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_صفحو_directory_cap:
        إذا (cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    حالة cap_small_چوکٺو_cap:
        ret.cap = cap_small_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_چوکٺو_cap:
        ret.cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_io_صفحو_جدول_cap:
        إذا (cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#جيڪڏهن_پورو
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_معلومات(bool_t preserve, لفظ_t معلومات, cap_t cap)
{
    أرجع cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_small_چوکٺو_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_small_چوکٺو_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        أرجع cap_small_چوکٺو_cap_سيٽ_cap_FVMRights(cap,
                                                    لفظ_From_VMRights(vm_rights));
    } وإلا إذا (cap_get_capType(cap) == cap_چوکٺو_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_چوکٺو_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        أرجع cap_چوکٺو_cap_سيٽ_cap_FVMRights(cap,
                                              لفظ_From_VMRights(vm_rights));
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

    حالة cap_صفحو_directory_cap:
        إذا (final && cap_صفحو_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_صفحو_جدول_cap:
        إذا (final && cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحو_جدول(
                cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap),
                PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_small_چوکٺو_cap:
        إذا (cap_small_چوکٺو_cap_get_cap_FMapped_ASID(cap)) {
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
            إذا (is_IOSpace_چوکٺو_Cap(cap)) {
                unmapIOPage(cap);
                اكسر;
            }
#جيڪڏهن_پورو

            unmap_صفحو(ARMSmall_صفحو,
                      cap_small_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                      cap_small_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                      (فراغ *)cap_small_چوکٺو_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

    حالة cap_چوکٺو_cap:
        إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap)) {
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_LOG_بفر
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            إذا (unlikely(cap_چوکٺو_cap_get_cap_FSize(cap) == ARMSection)) {
                إذا (pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                    ks_User_Log_بفر = 0;

                    /* Invalidate log page table entries */
                    clear_يادگيري((فراغ *) armKSGlobalLogPT, se_L4_صفحو_جدول_بٽ);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + ٻٽو_انگ(se_L4_صفحو_جدول_بٽ),
                                        addrFromKPPtr((فراغ *)&armKSGlobalLogPT[0]));

                    لكل (صحيح idx = 0; idx < ٻٽو_انگ(PT_اشاريو_بٽ); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_صفحو_بٽ));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#جيڪڏهن_پورو /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_صفحو(cap_چوکٺو_cap_get_cap_FSize(cap),
                      cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
                      (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (final) {
            clearIOPageDirectory(cap);
        }
        اكسر;

    حالة cap_io_صفحو_جدول_cap:
        إذا (final && cap_io_صفحو_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        اكسر;
#جيڪڏهن_پورو

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
    حالة cap_small_چوکٺو_cap:
    حالة cap_چوکٺو_cap:
        إذا (cap_get_capType(cap_b) == cap_small_چوکٺو_cap ||
            cap_get_capType(cap_b) == cap_چوکٺو_cap) {
            لفظ_t botA, botB, چوٽي_A, چوٽي_B;
            botA = generic_چوکٺو_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_چوکٺو_cap_get_cap_FBase_Ptr(cap_b);
            چوٽي_A = botA + نقاب(صفحو_بٽ_For_سائيز(generic_چوکٺو_cap_get_cap_FSize(cap_a)));
            چوٽي_B = botB + نقاب(صفحو_بٽ_For_سائيز(generic_چوکٺو_cap_get_cap_FSize(cap_b))) ;
            أرجع ((botA <= botB) && (چوٽي_A >= چوٽي_B) && (botB <= چوٽي_B));
        }
        اكسر;

    حالة cap_صفحو_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحو_جدول_cap) {
            أرجع cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        اكسر;

    حالة cap_صفحو_directory_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحو_directory_cap) {
            أرجع cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_صفحو_directory_cap_get_cap_PDBase_Ptr(cap_b);
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

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        إذا (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        اكسر;

    حالة cap_io_صفحو_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_io_صفحو_جدول_cap) {
            أرجع cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        اكسر;
#جيڪڏهن_پورو
    }

    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_small_چوکٺو_cap) {
        إذا (cap_get_capType(cap_b) == cap_small_چوکٺو_cap) {
            أرجع ((cap_small_چوکٺو_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_چوکٺو_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_چوکٺو_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_چوکٺو_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_چوکٺو_cap) {
            أرجع false;
        }
    }
    إذا (cap_get_capType(cap_a) == cap_چوکٺو_cap) {
        إذا (cap_get_capType(cap_b) == cap_چوکٺو_cap) {
            أرجع ((cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکٺو_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکٺو_cap_get_cap_FSize(cap_a) ==
                     cap_چوکٺو_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکٺو_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکٺو_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_small_چوکٺو_cap) {
            أرجع false;
        }
    }
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    إذا (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#جيڪڏهن_پورو

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائيز(لفظ_t t)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحو_Object:
        أرجع ARMSmall_صفحو_بٽ;
    حالة se_L4_ARM_Large_صفحو_Object:
        أرجع ARMLarge_صفحو_بٽ;
    حالة seL4_ARM_SectionObject:
        أرجع ARMSection_بٽ;
    حالة seL4_ARM_SuperSectionObject:
        أرجع ARMSuper_Section_بٽ;
    حالة se_L4_ARM_صفحو_جدول_Object:
        أرجع PTE_سائيز_بٽ + PT_اشاريو_بٽ;
    حالة se_L4_ARM_صفحو_Directory_Object:
        أرجع PDE_سائيز_بٽ + PD_اشاريو_بٽ;
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        أرجع se_L4_IOPage_جدول_بٽ;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_سائيز_بٽ;
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_بنياد, لفظ_t user_سائيز, bool_t device_يادگيري)
{
    بدل (t) {
    حالة se_L4_ARM_Small_صفحو_Object:
        إذا (device_يادگيري) {
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
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMSmall_صفحو)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_small_چوکٺو_cap_new(
                   ASID_هيٺيون(asidInvalid), VMReadWrite,
                   0, !!device_يادگيري,
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
                   0,
#جيڪڏهن_پورو
                   ASID_مٿاهون(asidInvalid),
                   (لفظ_t)region_بنياد);

    حالة se_L4_ARM_Large_صفحو_Object:
        إذا (device_يادگيري) {
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
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMLarge_صفحو)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   ARMLarge_صفحو, ASID_هيٺيون(asidInvalid), VMReadWrite,
                   0, !!device_يادگيري, ASID_مٿاهون(asidInvalid),
                   (لفظ_t)region_بنياد);

    حالة seL4_ARM_SectionObject:
        إذا (device_يادگيري) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } وإلا {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMSection)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   ARMSection, ASID_هيٺيون(asidInvalid), VMReadWrite,
                   0, !!device_يادگيري, ASID_مٿاهون(asidInvalid),
                   (لفظ_t)region_بنياد);

    حالة seL4_ARM_SuperSectionObject:
        إذا (device_يادگيري) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } وإلا {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنياد,
                                (لفظ_t)region_بنياد + نقاب(صفحو_بٽ_For_سائيز(ARMSuperSection)),
                                addrFromPPtr(region_بنياد));
        }
        أرجع cap_چوکٺو_cap_new(
                   ARMSuperSection, ASID_هيٺيون(asidInvalid), VMReadWrite,
                   0, !!device_يادگيري, ASID_مٿاهون(asidInvalid),
                   (لفظ_t)region_بنياد);

    حالة se_L4_ARM_صفحو_جدول_Object:
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((لفظ_t)region_بنياد,
                            (لفظ_t)region_بنياد + نقاب(se_L4_صفحو_جدول_بٽ),
                            addrFromPPtr(region_بنياد));
        أرجع cap_صفحو_جدول_cap_new(false, asidInvalid, 0,
                                      (لفظ_t)region_بنياد);

    حالة se_L4_ARM_صفحو_Directory_Object:
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_بنياد);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((لفظ_t)region_بنياد,
                            (لفظ_t)region_بنياد + نقاب(se_L4_صفحو_Dir_بٽ),
                            addrFromPPtr(region_بنياد));

        أرجع cap_صفحو_directory_cap_new(false, asidInvalid,
                                          (لفظ_t)region_بنياد);
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_بنياد));
        أرجع cap_vcpu_cap_new(VCPU_REF(region_بنياد));
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        cleanCacheRange_RAM((لفظ_t)region_بنياد,
                            (لفظ_t)region_بنياد + نقاب(se_L4_IOPage_جدول_بٽ),
                            addrFromPPtr(region_بنياد));
        أرجع cap_io_صفحو_جدول_cap_new(0, asidInvalid, (لفظ_t)region_بنياد, 0);
#جيڪڏهن_پورو
    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(لفظ_t invLabel, لفظ_t ڊيگهائي, cptr_t cptr,
                                  cte_t *جڳهه, cap_t cap,
                                  bool_t call, لفظ_t *بفر)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#إذا defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        أرجع decodeARMIOSpaceInvocation(invLabel, cap);
    حالة cap_io_صفحو_جدول_cap:
        أرجع decodeARMIOPTInvocation(invLabel, ڊيگهائي, جڳهه, cap, بفر);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(invLabel, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
#جيڪڏهن_پورو /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#جيڪڏهن_وصف_نه_ٿيل CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(invLabel, ڊيگهائي, cap, بفر);
#جيڪڏهن_پورو /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    افتراضي:
#وإلا
{
#جيڪڏهن_پورو
    أرجع decodeARMMMUInvocation(invLabel, ڊيگهائي, cptr, جڳهه, cap, call, بفر);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    fpuRelease(thread);
#جيڪڏهن_پورو
}

