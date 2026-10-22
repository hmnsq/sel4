/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/objecttype.h>
#అన్తర్భావ <arch/machine/tlb.h>
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
#అన్తర్భావ <arch/object/vcpu.h>
#అయితే_ముగింపు

bool_t Arch_is_చట్రం_Type(మాట_t type)
{
    ఎంపిక (type) {
    స్థితి se_L4_ARM_Small_పుట_Object:
        తిరిగి true;
    స్థితి se_L4_ARM_Large_పుట_Object:
        తిరిగి true;
    స్థితి seL4_ARM_SectionObject:
        తిరిగి true;
    స్థితి seL4_ARM_SuperSectionObject:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a PD cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    /* This is a deviation from haskell, which has only
     * one frame cap type on ARM */
    స్థితి cap_small_చట్రం_cap:
        ret.cap = cap_small_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asidInvalid);
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి cap_చట్రం_cap:
        ret.cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asidInvalid);
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి cap_asid_control_cap:
    స్థితి cap_asid_pool_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    స్థితి cap_sgi_signal_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a IOPT cap without an assigned IOASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;
#అయితే_ముగింపు
    అప్రమేయ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap");
    }
}

cap_t CONST Arch_update_Cap_దత్తాంశం(bool_t preserve, మాట_t దత్తాంశం, cap_t cap)
{
    తిరిగి cap;
}

cap_t CONST Arch_ముసుగు_Cap_Rights(seL4_CapRights_t cap_rights_ముసుగు, cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) == cap_small_చట్రం_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_మాట(
                        cap_small_చట్రం_cap_get_cap_FVMRights(cap));
        vm_rights = ముసుగు_VMRights(vm_rights, cap_rights_ముసుగు);
        తిరిగి cap_small_చట్రం_cap_సమితి_cap_FVMRights(cap,
                                                    మాట_From_VMRights(vm_rights));
    } లేకపోతే ఒకవేళ (cap_get_capType(cap) == cap_చట్రం_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_మాట(
                        cap_చట్రం_cap_get_cap_FVMRights(cap));
        vm_rights = ముసుగు_VMRights(vm_rights, cap_rights_ముసుగు);
        తిరిగి cap_చట్రం_cap_సమితి_cap_FVMRights(cap,
                                              మాట_From_VMRights(vm_rights));
    } లేకపోతే {
        తిరిగి cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_asid_pool_cap:
        ఒకవేళ (final) {
            deleteASIDPool(cap_asid_pool_cap_get_capASIDBase(cap),
                           ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap)));
        }
        ఆపు;

    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (final && cap_పుట_directory_cap_get_cap_PDIs_Mapped(cap)) {
            deleteASID(cap_పుట_directory_cap_get_cap_PDMapped_ASID(cap),
                       PDE_PTR(cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap)));
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_పుట_పట్టిక(
                cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
                cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
                PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap)));
        }
        ఆపు;

    స్థితి cap_small_చట్రం_cap:
        ఒకవేళ (cap_small_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
#నిర్వచితమైతే CONFIG_TK1_SMMU
            ఒకవేళ (is_IOSpace_చట్రం_Cap(cap)) {
                unmapIOPage(cap);
                ఆపు;
            }
#అయితే_ముగింపు

            unmap_పుట(ARMSmall_పుట,
                      cap_small_చట్రం_cap_get_cap_FMapped_ASID(cap),
                      cap_small_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                      (శూన్య *)cap_small_చట్రం_cap_get_cap_FBase_Ptr(cap));
        }
        ఆపు;

    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
#నిర్వచితమైతే CONFIG_KERNEL_LOG_బఫరు
            /* If the last cap to the user-level log buffer frame is being revoked,
             * reset the ksLog so that the kernel doesn't log anymore
             */
            ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FSize(cap) == ARMSection)) {
                ఒకవేళ (pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)) == ks_User_Log_బఫరు) {
                    ks_User_Log_బఫరు = 0;

                    /* Invalidate log page table entries */
                    clear_జ్ఞాపకం((శూన్య *) armKSGlobalLogPT, se_L4_పుట_పట్టిక_బిట్లు);

                    cleanCacheRange_PoU((pptr_t) &armKSGlobalLogPT[0],
                                        (pptr_t) &armKSGlobalLogPT[0] + బిట్టు(se_L4_పుట_పట్టిక_బిట్లు),
                                        addrFromKPPtr((శూన్య *)&armKSGlobalLogPT[0]));

                    కొరకు (పూర్ణాంకం idx = 0; idx < బిట్టు(PT_సూచీ_బిట్లు); idx++) {
                        invalidateTranslationSingle(KS_LOG_PPTR + (idx << se_L4_పుట_బిట్లు));
                    }

                    userError("Log buffer frame is invalidated, kernel can't benchmark anymore");
                }
            }
#అయితే_ముగింపు /* CONFIG_BENCHMARK_KERNEL_LOG_BUFFER */

            unmap_పుట(cap_చట్రం_cap_get_cap_FSize(cap),
                      cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                      cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                      (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap));
        }
        ఆపు;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap:
        ఒకవేళ (final) {
            vcpu_finalise(VCPU_PTR(cap_vcpu_cap_get_capVCPUPtr(cap)));
        }
        ఆపు;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    స్థితి cap_sgi_signal_cap:
        // do nothing
        ఆపు;
#అయితే_ముగింపు


#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap:
        ఒకవేళ (final) {
            clearIOPageDirectory(cap);
        }
        ఆపు;

    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_io_పుట_పట్టిక_cap_get_cap_IOPTIs_Mapped(cap)) {
            delete_IOPage_పట్టిక(cap);
        }
        ఆపు;
#అయితే_ముగింపు

    అప్రమేయ:
        ఆపు;
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ఎంపిక (cap_get_capType(cap_a)) {
    స్థితి cap_small_చట్రం_cap:
    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_small_చట్రం_cap ||
            cap_get_capType(cap_b) == cap_చట్రం_cap) {
            మాట_t botA, botB, శిఖరం_A, శిఖరం_B;
            botA = generic_చట్రం_cap_get_cap_FBase_Ptr(cap_a);
            botB = generic_చట్రం_cap_get_cap_FBase_Ptr(cap_b);
            శిఖరం_A = botA + ముసుగు(పుట_బిట్లు_For_కొలత(generic_చట్రం_cap_get_cap_FSize(cap_a)));
            శిఖరం_B = botB + ముసుగు(పుట_బిట్లు_For_కొలత(generic_చట్రం_cap_get_cap_FSize(cap_b))) ;
            తిరిగి ((botA <= botB) && (శిఖరం_A >= శిఖరం_B) && (botB <= శిఖరం_B));
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_పుట_పట్టిక_cap) {
            తిరిగి cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_పుట_directory_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_పుట_directory_cap) {
            తిరిగి cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap_a) ==
                   cap_పుట_directory_cap_get_cap_PDBase_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_asid_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_asid_control_cap) {
            తిరిగి true;
        }
        ఆపు;

    స్థితి cap_asid_pool_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            తిరిగి cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        ఆపు;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_vcpu_cap) {
            తిరిగి cap_vcpu_cap_get_capVCPUPtr(cap_a) ==
                   cap_vcpu_cap_get_capVCPUPtr(cap_b);
        }
        ఆపు;
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    స్థితి cap_sgi_signal_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_sgi_signal_cap) {
            తిరిగి (cap_sgi_signal_cap_get_capSGIIRQ(cap_a) ==
                    cap_sgi_signal_cap_get_capSGIIRQ(cap_b) &&
                    cap_sgi_signal_cap_get_capSGITarget(cap_a) ==
                    cap_sgi_signal_cap_get_capSGITarget(cap_b));
        }
        ఆపు;
#అయితే_ముగింపు


#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_space_cap) {
            తిరిగి cap_io_space_cap_get_capModuleID(cap_a) ==
                   cap_io_space_cap_get_capModuleID(cap_b);
        }
        ఆపు;

    స్థితి cap_io_పుట_పట్టిక_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_io_పుట_పట్టిక_cap) {
            తిరిగి cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap_a) ==
                   cap_io_పుట_పట్టిక_cap_get_cap_IOPTBase_Ptr(cap_b);
        }
        ఆపు;
#అయితే_ముగింపు
    }

    తిరిగి false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ఒకవేళ (cap_get_capType(cap_a) == cap_small_చట్రం_cap) {
        ఒకవేళ (cap_get_capType(cap_b) == cap_small_చట్రం_cap) {
            తిరిగి ((cap_small_చట్రం_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_small_చట్రం_cap_get_cap_FBase_Ptr(cap_b)) &&
                    ((cap_small_చట్రం_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_small_చట్రం_cap_get_cap_FIs_Device(cap_b) == 0)));
        } లేకపోతే ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {
            తిరిగి false;
        }
    }
    ఒకవేళ (cap_get_capType(cap_a) == cap_చట్రం_cap) {
        ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {
            తిరిగి ((cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_చట్రం_cap_get_cap_FSize(cap_a) ==
                     cap_చట్రం_cap_get_cap_FSize(cap_b)) &&
                    ((cap_చట్రం_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_చట్రం_cap_get_cap_FIs_Device(cap_b) == 0)));
        } లేకపోతే ఒకవేళ (cap_get_capType(cap_b) == cap_small_చట్రం_cap) {
            తిరిగి false;
        }
    }
#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    ఒకవేళ (cap_get_capType(cap_a) == cap_sgi_signal_cap) {
        తిరిగి false;
    }
#అయితే_ముగింపు

    తిరిగి Arch_sameRegionAs(cap_a, cap_b);
}

మాట_t Arch_get_Object_కొలత(మాట_t t)
{
    ఎంపిక (t) {
    స్థితి se_L4_ARM_Small_పుట_Object:
        తిరిగి ARMSmall_పుట_బిట్లు;
    స్థితి se_L4_ARM_Large_పుట_Object:
        తిరిగి ARMLarge_పుట_బిట్లు;
    స్థితి seL4_ARM_SectionObject:
        తిరిగి ARMSection_బిట్లు;
    స్థితి seL4_ARM_SuperSectionObject:
        తిరిగి ARMSuper_Section_బిట్లు;
    స్థితి se_L4_ARM_పుట_పట్టిక_Object:
        తిరిగి PTE_కొలత_బిట్లు + PT_సూచీ_బిట్లు;
    స్థితి se_L4_ARM_పుట_Directory_Object:
        తిరిగి PDE_కొలత_బిట్లు + PD_సూచీ_బిట్లు;
#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి se_L4_ARM_IOPage_పట్టిక_Object:
        తిరిగి se_L4_IOPage_పట్టిక_బిట్లు;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_ARM_VCPUObject:
        తిరిగి VCPU_కొలత_బిట్లు;
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    అప్రమేయ:
        fail("Invalid object type");
        తిరిగి 0;
    }
}

cap_t Arch_createObject(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
    ఎంపిక (t) {
    స్థితి se_L4_ARM_Small_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMSmallPageBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMSmall_పుట)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_small_చట్రం_cap_new(
                   ASID_దిగువ(asidInvalid), VMReadWrite,
                   0, !!device_జ్ఞాపకం,
#నిర్వచితమైతే CONFIG_TK1_SMMU
                   0,
#అయితే_ముగింపు
                   ASID_ఎగువ(asidInvalid),
                   (మాట_t)region_పునాది);

    స్థితి se_L4_ARM_Large_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 16
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMLarge_పుట)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_చట్రం_cap_new(
                   ARMLarge_పుట, ASID_దిగువ(asidInvalid), VMReadWrite,
                   0, !!device_జ్ఞాపకం, ASID_ఎగువ(asidInvalid),
                   (మాట_t)region_పునాది);

    స్థితి seL4_ARM_SectionObject:
        ఒకవేళ (device_జ్ఞాపకం) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
        } లేకపోతే {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 512
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 256
                 (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSection
                                            (ptr_val \<acute>regionBase)
                                            (unat ARMSectionBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMSection)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_చట్రం_cap_new(
                   ARMSection, ASID_దిగువ(asidInvalid), VMReadWrite,
                   0, !!device_జ్ఞాపకం, ASID_ఎగువ(asidInvalid),
                   (మాట_t)region_పునాది);

    స్థితి seL4_ARM_SuperSectionObject:
        ఒకవేళ (device_జ్ఞాపకం) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
        } లేకపోతే {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
            /** AUXUPD: "(True, ptr_retyps 8192
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** AUXUPD: "(True, ptr_retyps 4096
                    (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMSuperSection
                                                (ptr_val \<acute>regionBase)
                                                (unat ARMSuperSectionBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMSuperSection)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_చట్రం_cap_new(
                   ARMSuperSection, ASID_దిగువ(asidInvalid), VMReadWrite,
                   0, !!device_జ్ఞాపకం, ASID_ఎగువ(asidInvalid),
                   (మాట_t)region_పునాది);

    స్థితి se_L4_ARM_పుట_పట్టిక_Object:
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[256]) ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        cleanCacheRange_PoU((మాట_t)region_పునాది,
                            (మాట_t)region_పునాది + ముసుగు(se_L4_పుట_పట్టిక_బిట్లు),
                            addrFromPPtr(region_పునాది));
        తిరిగి cap_పుట_పట్టిక_cap_new(false, asidInvalid, 0,
                                      (మాట_t)region_పునాది);

    స్థితి se_L4_ARM_పుట_Directory_Object:
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[2048]) ptr))" */
#లేకపోతే  /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[4096]) ptr))" */
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */
        copyGlobalMappings((pde_t *)region_పునాది);
        /* Clean the entire PD to PoU for table walker */
        cleanCacheRange_PoU((మాట_t)region_పునాది,
                            (మాట_t)region_పునాది + ముసుగు(se_L4_పుట_Dir_బిట్లు),
                            addrFromPPtr(region_పునాది));

        తిరిగి cap_పుట_directory_cap_new(false, asidInvalid,
                                          (మాట_t)region_పునాది);
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_పునాది));
        తిరిగి cap_vcpu_cap_new(VCPU_REF(region_పునాది));
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి se_L4_ARM_IOPage_పట్టిక_Object:
        cleanCacheRange_RAM((మాట_t)region_పునాది,
                            (మాట_t)region_పునాది + ముసుగు(se_L4_IOPage_పట్టిక_బిట్లు),
                            addrFromPPtr(region_పునాది));
        తిరిగి cap_io_పుట_పట్టిక_cap_new(0, asidInvalid, (మాట_t)region_పునాది, 0);
#అయితే_ముగింపు
    అప్రమేయ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(మాట_t invLabel, మాట_t నిడివి, cptr_t cptr,
                                  cte_t *స్థానం, cap_t cap,
                                  bool_t call, మాట_t *బఫరు)
{
    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ఒకవేళ defined(CONFIG_TK1_SMMU) || defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ఎంపిక (cap_get_capType(cap)) {
#నిర్వచితమైతే CONFIG_TK1_SMMU
    స్థితి cap_io_space_cap:
        తిరిగి decodeARMIOSpaceInvocation(invLabel, cap);
    స్థితి cap_io_పుట_పట్టిక_cap:
        తిరిగి decodeARMIOPTInvocation(invLabel, నిడివి, స్థానం, cap, బఫరు);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap:
        తిరిగి decodeARMVCPUInvocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
#అయితే_ముగింపు /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    స్థితి cap_sgi_signal_cap:
        తిరిగి decodeSGISignalInvocation(invLabel, నిడివి, cap, బఫరు);
#అయితే_ముగింపు /* end of !CONFIG_ENABLE_SMP_SUPPORT */

    అప్రమేయ:
#లేకపోతే
{
#అయితే_ముగింపు
    తిరిగి decodeARMMMUInvocation(invLabel, నిడివి, cptr, స్థానం, cap, call, బఫరు);
}
}

శూన్య
Arch_prepareThreadDelete(tcb_t * thread) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    ఒకవేళ (thread->tcbArch.tcbVCPU) {
        dissociateVCPUTCB(thread->tcbArch.tcbVCPU, thread);
    }
#అయితే_ముగింపు /* CONFIG_ARM_HYPERVISOR_SUPPORT */

#నిర్వచితమైతే CONFIG_HAVE_FPU
    fpuRelease(thread);
#అయితే_ముగింపు
}

