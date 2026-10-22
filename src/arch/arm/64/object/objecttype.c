/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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

bool_t Arch_is_చట్రం_Type(మాట_t type)
{
    ఎంపిక (type) {
    స్థితి se_L4_ARM_Small_పుట_Object:
        తిరిగి true;
    స్థితి se_L4_ARM_Large_పుట_Object:
        తిరిగి true;
    స్థితి se_L4_ARM_Huge_పుట_Object:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}

deriveCap_ret_t Arch_deriveCap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_vspace_cap:
        ఒకవేళ (cap_vspace_cap_get_capVSIsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a VSpace cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a PT cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
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

#నిర్వచితమైతే CONFIG_ARM_SMMU
    స్థితి cap_sid_control_cap:
    స్థితి cap_cb_control_cap:
        ret.cap = cap_null_cap_new();
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    స్థితి cap_sid_cap:
    స్థితి cap_cb_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    స్థితి cap_smc_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
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
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    ఒకవేళ (cap_get_capType(cap) == cap_smc_cap) {
        ఒకవేళ (!preserve && cap_smc_cap_get_capSMCBadge(cap) == 0) {
            తిరిగి cap_smc_cap_సమితి_cap_SMCBadge(cap, దత్తాంశం);
        } లేకపోతే {
            తిరిగి cap_null_cap_new();
        }
    } లేకపోతే {
#అయితే_ముగింపు
        తిరిగి cap;
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    }
#అయితే_ముగింపు
}

cap_t CONST Arch_ముసుగు_Cap_Rights(seL4_CapRights_t cap_rights_ముసుగు, cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) == cap_చట్రం_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_మాట(cap_చట్రం_cap_get_cap_FVMRights(cap));
        vm_rights = ముసుగు_VMRights(vm_rights, cap_rights_ముసుగు);

        తిరిగి cap_చట్రం_cap_సమితి_cap_FVMRights(cap, మాట_From_VMRights(vm_rights));
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

    స్థితి cap_vspace_cap:
#నిర్వచితమైతే CONFIG_ARM_SMMU
        ఒకవేళ (cap_vspace_cap_get_capVSMappedCB(cap) != CB_INVALID) {
            smmu_cb_delete_vspace(cap_vspace_cap_get_capVSMappedCB(cap),
                                  cap_vspace_cap_get_capVSMappedASID(cap));
        }
#అయితే_ముగింపు
        ఒకవేళ (final && cap_vspace_cap_get_capVSIsMapped(cap)) {
            deleteASID(cap_vspace_cap_get_capVSMappedASID(cap),
                       VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(cap)));
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            unmap_పుట_పట్టిక(cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
                           cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
                           PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap)));
        }
        ఆపు;

    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
            unmap_పుట(cap_చట్రం_cap_get_cap_FSize(cap),
                      cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                      cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                      cap_చట్రం_cap_get_cap_FBase_Ptr(cap));
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


#నిర్వచితమైతే CONFIG_ARM_SMMU
    స్థితి cap_cb_cap:
        ఒకవేళ (cap_cb_cap_get_capBindSID(cap) != SID_INVALID) {
            smmu_sid_unbind(cap_cb_cap_get_capBindSID(cap));
        }
        ఒకవేళ (final) {
            smmu_delete_cb(cap);
        }
        ఆపు;
    స్థితి cap_sid_cap:
        ఒకవేళ (final) {
            smmu_delete_sid(cap);
        }
        ఆపు;
#అయితే_ముగింపు
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    ఎంపిక (cap_get_capType(cap_a)) {
    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {

            మాట_t botA, botB, శిఖరం_A, శిఖరం_B;
            botA = cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b);
            శిఖరం_A = botA + ముసుగు(పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap_a)));
            శిఖరం_B = botB + ముసుగు(పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(cap_b))) ;
            తిరిగి ((botA <= botB) && (శిఖరం_A >= శిఖరం_B) && (botB <= శిఖరం_B));
        }
        ఆపు;

    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_పుట_పట్టిక_cap) {
            తిరిగి cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap_b);
        }
        ఆపు;

    స్థితి cap_vspace_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_vspace_cap) {
            తిరిగి cap_vspace_cap_get_capVSBasePtr(cap_a) ==
                   cap_vspace_cap_get_capVSBasePtr(cap_b);
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

#నిర్వచితమైతే CONFIG_ARM_SMMU
    స్థితి cap_sid_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_sid_control_cap ||
            cap_get_capType(cap_b) == cap_sid_cap) {
            తిరిగి true;
        }
        ఆపు;
    స్థితి cap_cb_control_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_cb_control_cap ||
            cap_get_capType(cap_b) == cap_cb_cap) {
            తిరిగి true;
        }
        ఆపు;
    స్థితి cap_sid_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_sid_cap) {
            తిరిగి cap_sid_cap_get_capSID(cap_a) ==
                   cap_sid_cap_get_capSID(cap_b);
        }
        ఆపు;
    స్థితి cap_cb_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_cb_cap) {
            తిరిగి cap_cb_cap_get_capCB(cap_a) ==
                   cap_cb_cap_get_capCB(cap_b);
        }
        ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    స్థితి cap_smc_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_smc_cap) {
            తిరిగి true;
        }
        ఆపు;
#అయితే_ముగింపు
    }
    తిరిగి false;
}

bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ఒకవేళ (cap_get_capType(cap_a) == cap_చట్రం_cap) {
        ఒకవేళ (cap_get_capType(cap_b) == cap_చట్రం_cap) {
            తిరిగి ((cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a) ==
                     cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b)) &&
                    (cap_చట్రం_cap_get_cap_FSize(cap_a) ==
                     cap_చట్రం_cap_get_cap_FSize(cap_b)) &&
                    ((cap_చట్రం_cap_get_cap_FIs_Device(cap_a) == 0) ==
                     (cap_చట్రం_cap_get_cap_FIs_Device(cap_b) == 0)));
        }
    }
#నిర్వచితమైతే CONFIG_ARM_SMMU
    ఒకవేళ (cap_get_capType(cap_a) == cap_sid_control_cap &&
        cap_get_capType(cap_b) == cap_sid_cap) {
        తిరిగి false;
    }
    ఒకవేళ (cap_get_capType(cap_a) == cap_cb_control_cap &&
        cap_get_capType(cap_b) == cap_cb_cap) {
        తిరిగి false;
    }
#అయితే_ముగింపు
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
    స్థితి se_L4_ARM_Huge_పుట_Object:
        తిరిగి ARMHuge_పుట_బిట్లు;
    స్థితి se_L4_ARM_పుట_పట్టిక_Object:
        తిరిగి se_L4_పుట_పట్టిక_బిట్లు;
    స్థితి seL4_ARM_VSpaceObject:
        తిరిగి se_L4_VSpace_బిట్లు;
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_ARM_VCPUObject:
        తిరిగి VCPU_కొలత_బిట్లు;
#అయితే_ముగింపు
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
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (మాట_t)region_పునాది,    /* capFBasePtr */
                   ARMSmall_పుట,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_జ్ఞాపకం         /* capFIsDevice */
               );

    స్థితి se_L4_ARM_Large_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMLargePageBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMLarge_పుట)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (మాట_t)region_పునాది,    /* capFBasePtr */
                   ARMLarge_పుట,          /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_జ్ఞాపకం         /* capFIsDevice */
               );

    స్థితి se_L4_ARM_Huge_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.ARMHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat ARMHugePageBits))" */
            cleanCacheRange_RAM((మాట_t)region_పునాది,
                                (మాట_t)region_పునాది + ముసుగు(పుట_బిట్లు_For_కొలత(ARMHuge_పుట)),
                                addrFromPPtr(region_పునాది));
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,           /* capFMappedASID */
                   (మాట_t)region_పునాది,    /* capFBasePtr */
                   ARMHuge_పుట,           /* capFSize */
                   0,                     /* capFMappedAddress */
                   VMReadWrite,           /* capFVMRights */
                   !!device_జ్ఞాపకం         /* capFIsDevice */
               );
    స్థితి seL4_ARM_VSpaceObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[vs_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t VSRootPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((మాట_t)region_పునాది,
                            (మాట_t)region_పునాది + ముసుగు(se_L4_VSpace_బిట్లు),
                            addrFromPPtr(region_పునాది));
#నిర్వచితమైతే CONFIG_ARM_SMMU
        తిరిగి cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (మాట_t)region_పునాది,    /* capVSBasePtr    */
                   0,                     /* capVSIsMapped   */
                   CB_INVALID             /* capVSMappedCB   */
               );
#లేకపోతే
        తిరిగి cap_vspace_cap_new(
                   asidInvalid,           /* capVSMappedASID */
                   (మాట_t)region_పునాది,    /* capVSBasePtr    */
                   0                      /* capVSIsMapped   */
               );
#అయితే_ముగింపు /*!CONFIG_ARM_SMMU*/
    స్థితి se_L4_ARM_పుట_పట్టిక_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[pt_array_len]) ptr))" */
        /** GHOSTUPD: "(True, gs_new_pt_t NormalPT_T (ptr_val \<acute>regionBase))" */
        cleanCacheRange_PoU((మాట_t)region_పునాది,
                            (మాట_t)region_పునాది + ముసుగు(se_L4_పుట_పట్టిక_బిట్లు),
                            addrFromPPtr(region_పునాది));
        తిరిగి cap_పుట_పట్టిక_cap_new(
                   asidInvalid,           /* capPTMappedASID    */
                   (మాట_t)region_పునాది,    /* capPTBasePtr       */
                   0,                     /* capPTIsMapped      */
                   0                      /* capPTMappedAddress */
               );

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి seL4_ARM_VCPUObject:
        /** AUXUPD: "(True, ptr_retyp
          (Ptr (ptr_val \<acute>regionBase) :: vcpu_C ptr))" */
        vcpu_init(VCPU_PTR(region_పునాది));
        తిరిగి cap_vcpu_cap_new(VCPU_REF(region_పునాది));
#అయితే_ముగింపు

    అప్రమేయ:
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(మాట_t label, మాట_t నిడివి, cptr_t cptr,
                                  cte_t *స్థానం, cap_t cap,
                                  bool_t call, మాట_t *బఫరు)
{

    /* The C parser cannot handle a switch statement with only a default
     * case. So we need to do some gymnastics to remove the switch if
     * there are no other cases */
#ఒకవేళ defined(CONFIG_ARM_HYPERVISOR_SUPPORT) || defined(CONFIG_ARM_SMMU) || defined(CONFIG_ALLOW_SMC_CALLS) || !defined(CONFIG_ENABLE_SMP_SUPPORT)
    ఎంపిక (cap_get_capType(cap)) {
#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    స్థితి cap_vcpu_cap:
        తిరిగి decodeARMVCPUInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
#అయితే_ముగింపు /* end of CONFIG_ARM_HYPERVISOR_SUPPORT */
#నిర్వచితం_కాకపోతే CONFIG_ENABLE_SMP_SUPPORT
    స్థితి cap_sgi_signal_cap:
        తిరిగి decodeSGISignalInvocation(label, నిడివి, cap, బఫరు);
#అయితే_ముగింపు /* end of !CONFIG_ENABLE_SMP_SUPPORT */
#నిర్వచితమైతే CONFIG_ARM_SMMU
    స్థితి cap_sid_control_cap:
        తిరిగి decodeARMSIDControlInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    స్థితి cap_sid_cap:
        తిరిగి decodeARMSIDInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    స్థితి cap_cb_control_cap:
        తిరిగి decodeARMCBControlInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
    స్థితి cap_cb_cap:
        తిరిగి decodeARMCBInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
#అయితే_ముగింపు /*CONFIG_ARM_SMMU*/
#నిర్వచితమైతే CONFIG_ALLOW_SMC_CALLS
    స్థితి cap_smc_cap:
        తిరిగి decodeARMSMCInvocation(label, నిడివి, cap, call, బఫరు);
#అయితే_ముగింపు
    అప్రమేయ:
#లేకపోతే
{
#అయితే_ముగింపు
    తిరిగి decodeARMMMUInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
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
