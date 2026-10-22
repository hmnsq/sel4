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
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
#ضمن <arch/object/vcpu.h>
#جے_انت

bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    بدل (type) {
    حالة se_L4_ARM_Small_ورقہ_Object:
        أرجع true;
    حالة se_L4_ARM_Large_ورقہ_Object:
        أرجع true;
    حالة seL4_ARM_SectionObject:
        أرجع true;
    حالة seL4_ARM_SuperSectionObject:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

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

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    حالة cap_small_چوکھٹا_cap:
        ret.cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_چوکھٹا_cap:
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جے_انت

#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
#جے_انت

#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_io_ورقہ_جدول_cap:
        إذا (cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;
#جے_انت
    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_انکڑے(bool_t preserve, لفظ_t انکڑے, cap_t cap)
{
    أرجع cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_small_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_small_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        أرجع cap_small_چوکھٹا_cap_سیٹ_cap_FVMRights(cap,
                                                    لفظ_From_VMRights(vm_rights));
    } وإلا إذا (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        أرجع cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap,
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

    حالة cap_ورقہ_directory_cap:
        إذا (final && cap_ورقہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_ورقہ_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_ورقہ_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_ورقہ_جدول_cap:
        إذا (final && cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_ورقہ_جدول(
                cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
                PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        اكسر;

    حالة cap_small_چوکھٹا_cap:
        إذا (cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
#جے_تعریف_شدہ CONFIG_TK1_SMMU
            إذا (is_IOSpace_چوکھٹا_Cap(cap)) {
                unmapIOPage(cap);
                اكسر;
            }
#جے_انت

            unmap_ورقہ(ARMSmall_ورقہ,
                      cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_small_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      (فراغ *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

    حالة cap_چوکھٹا_cap:
        إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
#جے_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            إذا (unlikely(cap_چوکھٹا_cap_get_cap_FSize(cap) == ARMSection)) {
                إذا (pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                    ks_User_Log_بفر = 0;

                    /* Invalidate log page table entries */
                    clear_حافظہ((فراغ *) armKSGlobalLogPT, se_L4_ورقہ_جدول_بٹ);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ),
                                        addrFromKPPtr((فراغ *)&armKSGlobalLogPT[0]));

                    لكل (صحيح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_ورقہ_بٹ));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#جے_انت /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_ورقہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        اكسر;
#جے_انت

#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        // do nothing
        اكسر;
#جے_انت


#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (final) {
            clearIOPageDirectory(cap);
        }
        اكسر;

    حالة cap_io_ورقہ_جدول_cap:
        إذا (final && cap_io_ورقہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        اكسر;
#جے_انت

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
    حالة cap_small_چوکھٹا_cap:
    حالة cap_چوکھٹا_cap:
        إذا (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap ||
            cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(ورقہ_بٹ_For_ناپ(generic_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(ورقہ_بٹ_For_ناپ(generic_چوکھٹا_cap_get_cap_FSize(cap_b))) ;
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

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        إذا (cap_get_capType(cap_b) == cap_vcpu_cap) {
            أرجع cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        اكسر;
#جے_انت

#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        إذا (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            أرجع (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        اكسر;
#جے_انت


#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        إذا (cap_get_capType(cap_b) == cap_io_space_cap) {
            أرجع cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        اكسر;

    حالة cap_io_ورقہ_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_io_ورقہ_جدول_cap) {
            أرجع cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_ورقہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        اكسر;
#جے_انت
    }

    أرجع false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا (cap_get_capType(cap_a) == cap_small_چوکھٹا_cap) {
        إذا (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap) {
            أرجع ((cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            أرجع false;
        }
    }
    إذا (cap_get_capType(cap_a) == cap_چوکھٹا_cap) {
        إذا (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            أرجع ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        } وإلا إذا (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap) {
            أرجع false;
        }
    }
#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    إذا (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        أرجع false;
    }
#جے_انت

    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_ناپ(لفظ_t t)
{
    بدل (t) {
    حالة se_L4_ARM_Small_ورقہ_Object:
        أرجع ARMSmall_ورقہ_بٹ;
    حالة se_L4_ARM_Large_ورقہ_Object:
        أرجع ARMLarge_ورقہ_بٹ;
    حالة seL4_ARM_SectionObject:
        أرجع ARMSection_بٹ;
    حالة seL4_ARM_SuperSectionObject:
        أرجع ARMSuper_Section_بٹ;
    حالة se_L4_ARM_ورقہ_جدول_Object:
        أرجع PTE_ناپ_بٹ + PT_اشاریہ_بٹ;
    حالة se_L4_ARM_ورقہ_Directory_Object:
        أرجع PDE_ناپ_بٹ + PD_اشاریہ_بٹ;
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        أرجع se_L4_IOPage_جدول_بٹ;
#جے_انت
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        أرجع VCPU_ناپ_بٹ;
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_ادھار, لفظ_t user_ناپ, bool_t device_حافظہ)
{
    بدل (t) {
    حالة se_L4_ARM_Small_ورقہ_Object:
        إذا (device_حافظہ) {
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
            cleanCacheRange_RAM((لفظ_t)region_ادھار,
                                (لفظ_t)region_ادھار + نقاب(ورقہ_بٹ_For_ناپ(ARMSmall_ورقہ)),
                                addrFromPPtr(region_ادھار));
        }
        أرجع cap_small_چوکھٹا_cap_new(
                   ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ,
#جے_تعریف_شدہ CONFIG_TK1_SMMU
                   0,
#جے_انت
                   ASID_بلند(asidInvalid),
                   (لفظ_t)region_ادھار);

    حالة se_L4_ARM_Large_ورقہ_Object:
        إذا (device_حافظہ) {
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
            cleanCacheRange_RAM((لفظ_t)region_ادھار,
                                (لفظ_t)region_ادھار + نقاب(ورقہ_بٹ_For_ناپ(ARMLarge_ورقہ)),
                                addrFromPPtr(region_ادھار));
        }
        أرجع cap_چوکھٹا_cap_new(
                   ARMLarge_ورقہ, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_ادھار);

    حالة seL4_ARM_SectionObject:
        إذا (device_حافظہ) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } وإلا {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_ادھار,
                                (لفظ_t)region_ادھار + نقاب(ورقہ_بٹ_For_ناپ(ARMSection)),
                                addrFromPPtr(region_ادھار));
        }
        أرجع cap_چوکھٹا_cap_new(
                   ARMSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_ادھار);

    حالة seL4_ARM_SuperSectionObject:
        إذا (device_حافظہ) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } وإلا {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_ادھار,
                                (لفظ_t)region_ادھار + نقاب(ورقہ_بٹ_For_ناپ(ARMSuperSection)),
                                addrFromPPtr(region_ادھار));
        }
        أرجع cap_چوکھٹا_cap_new(
                   ARMSuperSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_ادھار);

    حالة se_L4_ARM_ورقہ_جدول_Object:
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((لفظ_t)region_ادھار,
                            (لفظ_t)region_ادھار + نقاب(se_L4_ورقہ_جدول_بٹ),
                            addrFromPPtr(region_ادھار));
        أرجع cap_ورقہ_جدول_cap_new(false, asidInvalid, 0,
                                      (لفظ_t)region_ادھار);

    حالة se_L4_ARM_ورقہ_Directory_Object:
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#وإلا  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_ادھار);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((لفظ_t)region_ادھار,
                            (لفظ_t)region_ادھار + نقاب(se_L4_ورقہ_Dir_بٹ),
                            addrFromPPtr(region_ادھار));

        أرجع cap_ورقہ_directory_cap_new(false, asidInvalid,
                                          (لفظ_t)region_ادھار);
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_ادھار));
        أرجع cap_vcpu_cap_new(VCPU_REF(region_ادھار));
#جے_انت

#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة se_L4_ARM_IOPage_جدول_Object:
        cleanCacheRange_RAM((لفظ_t)region_ادھار,
                            (لفظ_t)region_ادھار + نقاب(se_L4_IOPage_جدول_بٹ),
                            addrFromPPtr(region_ادھار));
        أرجع cap_io_ورقہ_جدول_cap_new(0, asidInvalid, (لفظ_t)region_ادھار, 0);
#جے_انت
    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(لفظ_t invLabel, لفظ_t لمائی, cptr_t cptr,
                                  cte_t *تھاں, cap_t cap,
                                  bool_t call, لفظ_t *بفر)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#إذا defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    بدل (cap_get_capType(cap)) {
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap:
        أرجع decodeARMIOSpaceInvocation(invLabel, cap);
    حالة cap_io_ورقہ_جدول_cap:
        أرجع decodeARMIOPTInvocation(invLabel, لمائی, تھاں, cap, بفر);
#جے_انت
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap:
        أرجع decodeARMVCPUInvocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
#جے_انت /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#جے_بنا_تعریف CONFIG_ENABLE_SMP_SUPPORT
    حالة cap_sgi_signal_cap:
        أرجع decodeSGISignalInvocation(invLabel, لمائی, cap, بفر);
#جے_انت /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    افتراضي:
#وإلا
{
#جے_انت
    أرجع decodeARMMMUInvocation(invLabel, لمائی, cptr, تھاں, cap, call, بفر);
}
}

فراغ
Arch_prepareThreadDelete(tcb_t * thread) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    إذا (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#جے_تعریف_شدہ CONFIG_HAVE_FPU
    fpuRelease(thread);
#جے_انت
}

