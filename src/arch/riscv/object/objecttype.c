/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <types.h>
#అన్తర్భావ <api/failures.h>
#అన్తర్భావ <kernel/vspace.h>
#అన్తర్భావ <object/structures.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/model/statedata.h>
#అన్తర్భావ <arch/object/objecttype.h>

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

    స్థితి cap_చట్రం_cap:
        cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, 0);
        ret.cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asidInvalid);
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    స్థితి cap_asid_control_cap:
    స్థితి cap_asid_pool_cap:
        ret.cap = cap;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    అప్రమేయ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_దత్తాంశం(bool_t preserve, మాట_t దత్తాంశం, cap_t cap)
{
    తిరిగి cap;
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
    స్థితి cap_చట్రం_cap:

        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
            unmap_పుట(cap_చట్రం_cap_get_cap_FSize(cap),
                      cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                      cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                      cap_చట్రం_cap_get_cap_FBase_Ptr(cap));
        }
        ఆపు;
    స్థితి cap_పుట_పట్టిక_cap:
        ఒకవేళ (final && cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap);
            కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
            ఒకవేళ (కనుగొను_ret.స్థితి_2 == EXCEPTION_NONE && కనుగొను_ret.vspace_మూలం == pte) {
                deleteASID(asid, pte);
            } లేకపోతే {
                unmap_పుట_పట్టిక(asid, cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap), pte);
            }
        }
        ఆపు;
    స్థితి cap_asid_pool_cap:
        ఒకవేళ (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        ఆపు;
    స్థితి cap_asid_control_cap:
        ఆపు;
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
    }

    తిరిగి false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    ఒకవేళ ((cap_get_capType(cap_a) == cap_చట్రం_cap) &&
        (cap_get_capType(cap_b) == cap_చట్రం_cap)) {
        తిరిగి ((cap_చట్రం_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_చట్రం_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_చట్రం_cap_get_cap_FSize(cap_a) ==
                 cap_చట్రం_cap_get_cap_FSize(cap_b)) &&
                ((cap_చట్రం_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_చట్రం_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    తిరిగి Arch_sameRegionAs(cap_a, cap_b);
}

మాట_t Arch_get_Object_కొలత(మాట_t t)
{
    ఎంపిక (t) {
    స్థితి se_L4_RISCV_4_K_పుట:
    స్థితి se_L4_RISCV_పుట_పట్టిక_Object:
        తిరిగి se_L4_పుట_బిట్లు;
    స్థితి se_L4_RISCV_Mega_పుట:
        తిరిగి se_L4_Large_పుట_బిట్లు;
#ఒకవేళ CONFIG_PT_స్థాయిలు > 2
    స్థితి se_L4_RISCV_Giga_పుట:
        తిరిగి se_L4_Huge_పుట_బిట్లు;
#అయితే_ముగింపు
#ఒకవేళ CONFIG_PT_స్థాయిలు > 3
    స్థితి se_L4_RISCV_Tera_పుట:
        తిరిగి se_L4_Tera_పుట_బిట్లు;
#అయితే_ముగింపు
    అప్రమేయ:
        fail("Invalid object type");
        తిరిగి 0;
    }
}

cap_t Arch_createObject(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t
                        device_జ్ఞాపకం)
{
    ఎంపిక (t) {
    స్థితి se_L4_RISCV_4_K_పుట:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (మాట_t) region_పునాది,            /* capFBasePtr          */
                   RISCV_4_K_పుట,                  /* capFSize             */
                   మాట_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_జ్ఞాపకం,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    స్థితి se_L4_RISCV_Mega_పుట: {
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (మాట_t) region_పునాది,            /* capFBasePtr          */
                   RISCV_Mega_పుట,                  /* capFSize             */
                   మాట_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_జ్ఞాపకం,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#ఒకవేళ CONFIG_PT_స్థాయిలు > 2
    స్థితి se_L4_RISCV_Giga_పుట: {
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (మాట_t) region_పునాది,            /* capFBasePtr          */
                   RISCV_Giga_పుట,                  /* capFSize             */
                   మాట_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_జ్ఞాపకం,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#అయితే_ముగింపు

    స్థితి se_L4_RISCV_పుట_పట్టిక_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        తిరిగి cap_పుట_పట్టిక_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (మాట_t)region_పునాది,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    అప్రమేయ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    మాట_t label,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    తిరిగి decodeRISCVMMUInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);
}

శూన్య Arch_prepareThreadDelete(tcb_t *thread)
{
#నిర్వచితమైతే CONFIG_HAVE_FPU
    fpuRelease(thread);
#అయితే_ముగింపు
}

bool_t Arch_is_చట్రం_Type(మాట_t type)
{
    ఎంపిక (type) {
#ఒకవేళ CONFIG_PT_స్థాయిలు == 4
    స్థితి se_L4_RISCV_Tera_పుట:
#అయితే_ముగింపు
#ఒకవేళ CONFIG_PT_స్థాయిలు > 2
    స్థితి se_L4_RISCV_Giga_పుట:
#అయితే_ముగింపు
    స్థితి se_L4_RISCV_Mega_పుట:
    స్థితి se_L4_RISCV_4_K_పుట:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}
