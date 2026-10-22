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
#شامل <arch/object/objecttype.h>
#شامل <arch/machine/tlb.h>
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
#شامل <arch/object/vcpu.h>
#اگر_ختم

bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    انتخاب (type) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        واپس true;
    حالت se_L4_ARM_Large_صفحہ_Object:
        واپس true;
    حالت seL4_ARM_SectionObject:
        واپس true;
    حالت seL4_ARM_SuperSectionObject:
        واپس true;
    طےشدہ:
        واپس false;
    }
}

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

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    حالت cap_small_چوکھٹا_cap:
        ret.cap = cap_small_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت cap_چوکھٹا_cap:
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت cap_io_صفحہ_جدول_cap:
        اگر (cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;
#اگر_ختم
    طےشدہ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_ڈیٹا(bool_t preserve, لفظ_t ڈیٹا, cap_t cap)
{
    واپس cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_small_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_small_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        واپس cap_small_چوکھٹا_cap_سیٹ_cap_FVMRights(cap,
                                                    لفظ_From_VMRights(vm_rights));
    } ورنہ اگر (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(
                        cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        واپس cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap,
                                              لفظ_From_VMRights(vm_rights));
    } ورنہ {
        واپس cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        توڑو;

    حالت cap_صفحہ_directory_cap:
        اگر (final && cap_صفحہ_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_صفحہ_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_صفحہ_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (final && cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_صفحہ_جدول(
                cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
                PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
        }
        توڑو;

    حالت cap_small_چوکھٹا_cap:
        اگر (cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
            اگر (is_IOSpace_چوکھٹا_Cap(cap)) {
                unmapIOPage(cap);
                توڑو;
            }
#اگر_ختم

            unmap_صفحہ(ARMSmall_صفحہ,
                      cap_small_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_small_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      (خالی *)cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        توڑو;

    حالت cap_چوکھٹا_cap:
        اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
#اگر_تعریف_شدہ CONFIG_KERNEL_LOG_بفر
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            اگر (unlikely(cap_چوکھٹا_cap_get_cap_FSize(cap) == ARMSection)) {
                اگر (pptr_مقصد_paddr((خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_بفر) {
                    ks_User_Log_بفر = 0;

                    /* Invalidate log page table entries */
                    clear_حافظہ((خالی *) armKSGlobalLogPT, se_L4_صفحہ_جدول_بٹ);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + ثنائی_ہندسہ(se_L4_صفحہ_جدول_بٹ),
                                        addrFromKPPtr((خالی *)&armKSGlobalLogPT[0]));

                    برائے (صحیح idx = 0; idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_صفحہ_بٹ));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#اگر_ختم /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_صفحہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        توڑو;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        توڑو;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        // do nothing
        توڑو;
#اگر_ختم


#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        اگر (final) {
            clearIOPageDirectory(cap);
        }
        توڑو;

    حالت cap_io_صفحہ_جدول_cap:
        اگر (final && cap_io_صفحہ_جدول_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_جدول(cap);
        }
        توڑو;
#اگر_ختم

    طےشدہ:
        توڑو;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_small_چوکھٹا_cap:
    حالت cap_چوکھٹا_cap:
        اگر (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap ||
            cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(صفحہ_بٹ_For_سائز(generic_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(صفحہ_بٹ_For_سائز(generic_چوکھٹا_cap_get_cap_FSize(cap_b))) ;
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

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        اگر (cap_get_capType(cap_b) == cap_vcpu_cap) {
            واپس cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        توڑو;
#اگر_ختم

#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        اگر (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            واپس (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        توڑو;
#اگر_ختم


#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        اگر (cap_get_capType(cap_b) == cap_io_space_cap) {
            واپس cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        توڑو;

    حالت cap_io_صفحہ_جدول_cap:
        اگر (cap_get_capType(cap_b) == cap_io_صفحہ_جدول_cap) {
            واپس cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_صفحہ_جدول_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        توڑو;
#اگر_ختم
    }

    واپس false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر (cap_get_capType(cap_a) == cap_small_چوکھٹا_cap) {
        اگر (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap) {
            واپس ((cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ورنہ اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            واپس false;
        }
    }
    اگر (cap_get_capType(cap_a) == cap_چوکھٹا_cap) {
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            واپس ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                     cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                    ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
        } ورنہ اگر (cap_get_capType(cap_b) == cap_small_چوکھٹا_cap) {
            واپس false;
        }
    }
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    اگر (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        واپس false;
    }
#اگر_ختم

    واپس Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائز(لفظ_t t)
{
    انتخاب (t) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        واپس ARMSmall_صفحہ_بٹ;
    حالت se_L4_ARM_Large_صفحہ_Object:
        واپس ARMLarge_صفحہ_بٹ;
    حالت seL4_ARM_SectionObject:
        واپس ARMSection_بٹ;
    حالت seL4_ARM_SuperSectionObject:
        واپس ARMSuper_Section_بٹ;
    حالت se_L4_ARM_صفحہ_جدول_Object:
        واپس PTE_سائز_بٹ + PT_اشاریہ_بٹ;
    حالت se_L4_ARM_صفحہ_Directory_Object:
        واپس PDE_سائز_بٹ + PD_اشاریہ_بٹ;
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت se_L4_ARM_IOPage_جدول_Object:
        واپس se_L4_IOPage_جدول_بٹ;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        واپس VCPU_سائز_بٹ;
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    طےشدہ:
        fail("Invalid object type");
        واپس 0;
    }
}

cap_t Arch_createObject(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    انتخاب (t) {
    حالت se_L4_ARM_Small_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMSmall_صفحہ)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_small_چوکھٹا_cap_new(
                   ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ,
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
                   0,
#اگر_ختم
                   ASID_بلند(asidInvalid),
                   (لفظ_t)region_بنیاد);

    حالت se_L4_ARM_Large_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMLarge_صفحہ)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   ARMLarge_صفحہ, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_بنیاد);

    حالت seL4_ARM_SectionObject:
        اگر (device_حافظہ) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } ورنہ {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMSection)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   ARMSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_بنیاد);

    حالت seL4_ARM_SuperSectionObject:
        اگر (device_حافظہ) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } ورنہ {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                                (لفظ_t)region_بنیاد + نقاب(صفحہ_بٹ_For_سائز(ARMSuperSection)),
                                addrFromPPtr(region_بنیاد));
        }
        واپس cap_چوکھٹا_cap_new(
                   ARMSuperSection, ASID_کم(asidInvalid), VMReadWrite,
                   0, !!device_حافظہ, ASID_بلند(asidInvalid),
                   (لفظ_t)region_بنیاد);

    حالت se_L4_ARM_صفحہ_جدول_Object:
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((لفظ_t)region_بنیاد,
                            (لفظ_t)region_بنیاد + نقاب(se_L4_صفحہ_جدول_بٹ),
                            addrFromPPtr(region_بنیاد));
        واپس cap_صفحہ_جدول_cap_new(false, asidInvalid, 0,
                                      (لفظ_t)region_بنیاد);

    حالت se_L4_ARM_صفحہ_Directory_Object:
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#ورنہ  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_بنیاد);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((لفظ_t)region_بنیاد,
                            (لفظ_t)region_بنیاد + نقاب(se_L4_صفحہ_Dir_بٹ),
                            addrFromPPtr(region_بنیاد));

        واپس cap_صفحہ_directory_cap_new(false, asidInvalid,
                                          (لفظ_t)region_بنیاد);
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_بنیاد));
        واپس cap_vcpu_cap_new(VCPU_REF(region_بنیاد));
#اگر_ختم

#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت se_L4_ARM_IOPage_جدول_Object:
        cleanCacheRange_RAM((لفظ_t)region_بنیاد,
                            (لفظ_t)region_بنیاد + نقاب(se_L4_IOPage_جدول_بٹ),
                            addrFromPPtr(region_بنیاد));
        واپس cap_io_صفحہ_جدول_cap_new(0, asidInvalid, (لفظ_t)region_بنیاد, 0);
#اگر_ختم
    طےشدہ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(لفظ_t invLabel, لفظ_t لمبائی, cptr_t cptr,
                                  cte_t *خانہ, cap_t cap,
                                  bool_t call, لفظ_t *بفر)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#اگر defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    انتخاب (cap_get_capType(cap)) {
#اگر_تعریف_شدہ CONFIG_TK1_SMMU
    حالت cap_io_space_cap:
        واپس decodeARMIOSpaceInvocation(invLabel, cap);
    حالت cap_io_صفحہ_جدول_cap:
        واپس decodeARMIOPTInvocation(invLabel, لمبائی, خانہ, cap, بفر);
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالت cap_vcpu_cap:
        واپس decodeARMVCPUInvocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
#اگر_ختم /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#اگر_غیر_تعریف_شدہ CONFIG_ENABLE_SMP_SUPPORT
    حالت cap_sgi_signal_cap:
        واپس decodeSGISignalInvocation(invLabel, لمبائی, cap, بفر);
#اگر_ختم /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    طےشدہ:
#ورنہ
{
#اگر_ختم
    واپس decodeARMMMUInvocation(invLabel, لمبائی, cptr, خانہ, cap, call, بفر);
}
}

خالی
Arch_prepareThreadDelete(tcb_t * thread) {
#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    اگر (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#اگر_ختم /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    fpuRelease(thread);
#اگر_ختم
}

