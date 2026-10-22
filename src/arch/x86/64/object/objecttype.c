/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/failures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/structures.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/fpu.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/objecttype.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/ioport.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/ept.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/object/iospace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/intel-vtd.h>


bool_t Arch_is_ਚੌਖਟਾ_Type(ਸ਼ਬਦ_t type)
{
    ਚੋਣ (type) {
    ਹਾਲਤ seL4_X86_4K:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_X86_Large_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਹਾਲਤ se_L4_X64_Huge_ਪੰਨਾ_Object:
        ਵਾਪਸ true;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

deriveCap_ret_t ਬਹੁਲਕ_derive_Cap(cte_t *ਥਾਂ, cap_t cap)
{
    deriveCap_ret_t ret;

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_pml4_cap:
        ਜੇ (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_pdpt_cap:
        ਜੇ (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        } ਨਹੀਂਤਾਂ {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        }
        ਵਾਪਸ ret;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asidInvalid);
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;

    ਮੂਲ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t ਬਹੁਲਕ_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_pml4_cap:
        ਜੇ (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap))
            );
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_pdpt_cap:
        ਜੇ (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_ਪਤਾ(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        ਰੋਕੋ;

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
            ਚੋਣ (cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap)) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
            ਹਾਲਤ X86_MappingEPT:
                unmapEPTPage(
                    cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                    (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
                );
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
            ਹਾਲਤ X86_MappingIOSpace:
                unmapIOPage(cap);
                ਰੋਕੋ;
#ਜੇ_ਅੰਤ
            ਹਾਲਤ X86_MappingVSpace:
                unmap_ਪੰਨਾ(
                    cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                    cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
                    (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
                );
                ਰੋਕੋ;
            ਮੂਲ:
                fail("Invalid map type");
            }
        }
        ਰੋਕੋ;

    ਮੂਲ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    ਵਾਪਸ fc_ret;
}

bool_t CONST ਬਹੁਲਕ_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    ਚੋਣ (cap_get_capType(cap_a)) {

    ਹਾਲਤ cap_pdpt_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_pdpt_cap) {
            ਵਾਪਸ cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        ਵਾਪਸ false;

    ਹਾਲਤ cap_pml4_cap:
        ਜੇ (cap_get_capType(cap_b) == cap_pml4_cap) {
            ਵਾਪਸ cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_ਅਧਾਰ_Ptr(cap_b);
        }
        ਵਾਪਸ false;
    ਮੂਲ:
        ਵਾਪਸ false;
    }
}

ਸ਼ਬਦ_t ਬਹੁਲਕ_get_Object_ਮਾਪ(ਸ਼ਬਦ_t t)
{
    ਚੋਣ (t) {
    ਹਾਲਤ seL4_X64_PML4Object:
        ਵਾਪਸ se_L4_PML4_ਬਿੱਟ;

    ਹਾਲਤ se_L4_X64_Huge_ਪੰਨਾ_Object:
        ਵਾਪਸ ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(X64_Huge_ਪੰਨਾ);

    ਮੂਲ:
        fail("Invalid object type");
        ਵਾਪਸ 0;
    }
}

cap_t ਬਹੁਲਕ_create_Object(object_t t, ਖਾਲੀ *region_ਅਧਾਰ, ਸ਼ਬਦ_t user_ਮਾਪ, bool_t device_ਸਮ੍ਰਿਤੀ)
{
    ਚੋਣ (t) {

    ਹਾਲਤ seL4_X86_4K:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ, /* capFBasePtr              */
                   X86_Small_ਪੰਨਾ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ਸਮ੍ਰਿਤੀ        /* capFIsDevice             */
               );

    ਹਾਲਤ se_L4_X86_Large_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ, /* capFBasePtr              */
                   X86_Large_ਪੰਨਾ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ਸਮ੍ਰਿਤੀ        /* capFIsDevice             */
               );

    ਹਾਲਤ se_L4_X64_Huge_ਪੰਨਾ_Object:
        ਜੇ (device_ਸਮ੍ਰਿਤੀ) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } ਨਹੀਂਤਾਂ {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        ਵਾਪਸ cap_ਚੌਖਟਾ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ, /* capFBasePtr              */
                   X64_Huge_ਪੰਨਾ,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_ਸਮ੍ਰਿਤੀ        /* capFIsDevice             */
               );

    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_ਸਾਰਣੀ_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        ਵਾਪਸ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    ਹਾਲਤ se_L4_X86_ਪੰਨਾ_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        ਵਾਪਸ cap_ਪੰਨਾ_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    ਹਾਲਤ seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        ਵਾਪਸ cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    ਹਾਲਤ seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_ਅਧਾਰ));
        ਵਾਪਸ cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ se_L4_X86_IOPage_ਸਾਰਣੀ_Object:
        ਵਾਪਸ cap_io_ਪੰਨਾ_ਸਾਰਣੀ_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (ਸ਼ਬਦ_t)region_ਅਧਾਰ
               );
#ਜੇ_ਅੰਤ

    ਮੂਲ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t ਬਹੁਲਕ_decode_Invocation(
    ਸ਼ਬਦ_t label,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *ਥਾਂ,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_pml4_cap:
    ਹਾਲਤ cap_pdpt_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_directory_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decodeX86MMUInvocation(label, ਲੰਮਾਈ, cptr, ਥਾਂ, cap, call, ਬਫਰ);

    ਮੂਲ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}
