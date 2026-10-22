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
#అన్తర్భావ <machine/fpu.h>
#అన్తర్భావ <arch/object/objecttype.h>
#అన్తర్భావ <arch/object/ioport.h>
#అన్తర్భావ <arch/kernel/ept.h>

#అన్తర్భావ <arch/object/iospace.h>
#అన్తర్భావ <plat/machine/intel-vtd.h>


bool_t Arch_is_చట్రం_Type(మాట_t type)
{
    ఎంపిక (type) {
    స్థితి seL4_X86_4K:
        తిరిగి true;
    స్థితి se_L4_X86_Large_పుట_Object:
        తిరిగి true;
    స్థితి se_L4_X64_Huge_పుట_Object:
        తిరిగి true;
    అప్రమేయ:
        తిరిగి false;
    }
}

deriveCap_ret_t బహుళకం_derive_Cap(cte_t *స్థానం, cap_t cap)
{
    deriveCap_ret_t ret;

    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_pml4_cap:
        ఒకవేళ (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_pdpt_cap:
        ఒకవేళ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.స్థితి_2 = EXCEPTION_NONE;
        } లేకపోతే {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        }
        తిరిగి ret;

    స్థితి cap_చట్రం_cap:
        cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asidInvalid);
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;

    అప్రమేయ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t బహుళకం_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ఎంపిక (cap_get_capType(cap)) {

    స్థితి cap_pml4_cap:
        ఒకవేళ (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_పునాది_Ptr(cap))
            );
        }
        ఆపు;

    స్థితి cap_pdpt_cap:
        ఒకవేళ (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_చిరునామా(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ఆపు;

    స్థితి cap_చట్రం_cap:
        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
            ఎంపిక (cap_చట్రం_cap_get_cap_FMap_Type(cap)) {
#నిర్వచితమైతే CONFIG_VTX
            స్థితి X86_MappingEPT:
                unmapEPTPage(
                    cap_చట్రం_cap_get_cap_FSize(cap),
                    cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                    cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                    (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
                );
                ఆపు;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_IOMMU
            స్థితి X86_MappingIOSpace:
                unmapIOPage(cap);
                ఆపు;
#అయితే_ముగింపు
            స్థితి X86_MappingVSpace:
                unmap_పుట(
                    cap_చట్రం_cap_get_cap_FSize(cap),
                    cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                    cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
                    (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
                );
                ఆపు;
            అప్రమేయ:
                fail("Invalid map type");
            }
        }
        ఆపు;

    అప్రమేయ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    తిరిగి fc_ret;
}

bool_t CONST బహుళకం_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    ఎంపిక (cap_get_capType(cap_a)) {

    స్థితి cap_pdpt_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_pdpt_cap) {
            తిరిగి cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        తిరిగి false;

    స్థితి cap_pml4_cap:
        ఒకవేళ (cap_get_capType(cap_b) == cap_pml4_cap) {
            తిరిగి cap_pml4_cap_get_cap_PML4_పునాది_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_పునాది_Ptr(cap_b);
        }
        తిరిగి false;
    అప్రమేయ:
        తిరిగి false;
    }
}

మాట_t బహుళకం_get_Object_కొలత(మాట_t t)
{
    ఎంపిక (t) {
    స్థితి seL4_X64_PML4Object:
        తిరిగి se_L4_PML4_బిట్లు;

    స్థితి se_L4_X64_Huge_పుట_Object:
        తిరిగి పుట_బిట్లు_For_కొలత(X64_Huge_పుట);

    అప్రమేయ:
        fail("Invalid object type");
        తిరిగి 0;
    }
}

cap_t బహుళకం_create_Object(object_t t, శూన్య *region_పునాది, మాట_t user_కొలత, bool_t device_జ్ఞాపకం)
{
    ఎంపిక (t) {

    స్థితి seL4_X86_4K:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (మాట_t)region_పునాది, /* capFBasePtr              */
                   X86_Small_పుట,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_జ్ఞాపకం        /* capFIsDevice             */
               );

    స్థితి se_L4_X86_Large_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (మాట_t)region_పునాది, /* capFBasePtr              */
                   X86_Large_పుట,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_జ్ఞాపకం        /* capFIsDevice             */
               );

    స్థితి se_L4_X64_Huge_పుట_Object:
        ఒకవేళ (device_జ్ఞాపకం) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } లేకపోతే {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        తిరిగి cap_చట్రం_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (మాట_t)region_పునాది, /* capFBasePtr              */
                   X64_Huge_పుట,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_జ్ఞాపకం        /* capFIsDevice             */
               );

    స్థితి se_L4_X86_పుట_పట్టిక_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        తిరిగి cap_పుట_పట్టిక_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (మాట_t)region_పునాది,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    స్థితి se_L4_X86_పుట_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        తిరిగి cap_పుట_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (మాట_t)region_పునాది,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    స్థితి seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        తిరిగి cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (మాట_t)region_పునాది,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    స్థితి seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_పునాది));
        తిరిగి cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (మాట_t)region_పునాది,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి se_L4_X86_IOPage_పట్టిక_Object:
        తిరిగి cap_io_పుట_పట్టిక_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (మాట_t)region_పునాది
               );
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

exception_t బహుళకం_decode_Invocation(
    మాట_t label,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *స్థానం,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {
    స్థితి cap_pml4_cap:
    స్థితి cap_pdpt_cap:
    స్థితి cap_పుట_directory_cap:
    స్థితి cap_పుట_పట్టిక_cap:
    స్థితి cap_చట్రం_cap:
        తిరిగి decodeX86MMUInvocation(label, నిడివి, cptr, స్థానం, cap, call, బఫరు);

    అప్రమేయ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}
