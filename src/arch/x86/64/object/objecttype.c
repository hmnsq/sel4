/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#شامل <machine/fpu.h>
#شامل <arch/object/objecttype.h>
#شامل <arch/object/ioport.h>
#شامل <arch/kernel/ept.h>

#شامل <arch/object/iospace.h>
#شامل <plat/machine/intel-vtd.h>


bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    انتخاب (type) {
    حالت seL4_X86_4K:
        واپس true;
    حالت se_L4_X86_Large_صفحہ_Object:
        واپس true;
    حالت se_L4_X64_Huge_صفحہ_Object:
        واپس true;
    طےشدہ:
        واپس false;
    }
}

deriveCap_ret_t منوال_derive_Cap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_pml4_cap:
        اگر (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_pdpt_cap:
        اگر (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_چوکھٹا_cap:
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    طےشدہ:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t منوال_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {

    حالت cap_pml4_cap:
        اگر (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap))
            );
        }
        توڑو;

    حالت cap_pdpt_cap:
        اگر (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        توڑو;

    حالت cap_چوکھٹا_cap:
        اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            انتخاب (cap_چوکھٹا_cap_get_cap_FMap_Type(cap)) {
#اگر_تعریف_شدہ CONFIG_VTX
            حالت X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                توڑو;
#اگر_ختم
#اگر_تعریف_شدہ CONFIG_IOMMU
            حالت X86_MappingIOSpace:
                unmapIOPage(cap);
                توڑو;
#اگر_ختم
            حالت X86_MappingVSpace:
                unmap_صفحہ(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (خالی *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                توڑو;
            طےشدہ:
                fail("Invalid map type");
            }
        }
        توڑو;

    طےشدہ:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST منوال_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {

    حالت cap_pdpt_cap:
        اگر (cap_get_capType(cap_b) == cap_pdpt_cap) {
            واپس cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        واپس false;

    حالت cap_pml4_cap:
        اگر (cap_get_capType(cap_b) == cap_pml4_cap) {
            واپس cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_بنیاد_Ptr(cap_b);
        }
        واپس false;
    طےشدہ:
        واپس false;
    }
}

لفظ_t منوال_get_Object_سائز(لفظ_t t)
{
    انتخاب (t) {
    حالت seL4_X64_PML4Object:
        واپس se_L4_PML4_بٹ;

    حالت se_L4_X64_Huge_صفحہ_Object:
        واپس صفحہ_بٹ_For_سائز(X64_Huge_صفحہ);

    طےشدہ:
        fail("Invalid object type");
        واپس 0;
    }
}

cap_t منوال_create_Object(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t device_حافظہ)
{
    انتخاب (t) {

    حالت seL4_X86_4K:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_بنیاد, /* capFBasePtr              */
                   X86_Small_صفحہ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالت se_L4_X86_Large_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_بنیاد, /* capFBasePtr              */
                   X86_Large_صفحہ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالت se_L4_X64_Huge_صفحہ_Object:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_بنیاد, /* capFBasePtr              */
                   X64_Huge_صفحہ,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالت se_L4_X86_صفحہ_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        واپس cap_صفحہ_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (لفظ_t)region_بنیاد,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    حالت se_L4_X86_صفحہ_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        واپس cap_صفحہ_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (لفظ_t)region_بنیاد,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    حالت seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        واپس cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (لفظ_t)region_بنیاد,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    حالت seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_بنیاد));
        واپس cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (لفظ_t)region_بنیاد,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#اگر_تعریف_شدہ CONFIG_IOMMU
    حالت se_L4_X86_IOPage_جدول_Object:
        واپس cap_io_صفحہ_جدول_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (لفظ_t)region_بنیاد
               );
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

exception_t منوال_decode_Invocation(
    لفظ_t label,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_pml4_cap:
    حالت cap_pdpt_cap:
    حالت cap_صفحہ_directory_cap:
    حالت cap_صفحہ_جدول_cap:
    حالت cap_چوکھٹا_cap:
        واپس decodeX86MMUInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);

    طےشدہ:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        واپس EXCEPTION_SYSCALL_ERROR;
    }
}
