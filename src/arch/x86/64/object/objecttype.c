/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
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
#ضمن <machine/fpu.h>
#ضمن <arch/object/objecttype.h>
#ضمن <arch/object/ioport.h>
#ضمن <arch/kernel/ept.h>

#ضمن <arch/object/iospace.h>
#ضمن <plat/machine/intel-vtd.h>


bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_ورقہ_Object:
        أرجع true;
    حالة se_L4_X64_Huge_ورقہ_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t منوال_derive_Cap(cte_t *تھاں, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_pml4_cap:
        إذا (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_pdpt_cap:
        إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_چوکھٹا_cap:
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t منوال_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        إذا (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_ادھار_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_pdpt_cap:
        إذا (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_پتہ(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        اكسر;

    حالة cap_چوکھٹا_cap:
        إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_چوکھٹا_cap_get_cap_FMap_Type(cap)) {
#جے_تعریف_شدہ CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#جے_انت
#جے_تعریف_شدہ CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#جے_انت
            حالة X86_MappingVSpace:
                unmap_ورقہ(
                    cap_چوکھٹا_cap_get_cap_FSize(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                    (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
            افتراضي:
                fail("Invalid map type");
            }
        }
        اكسر;

    افتراضي:
        fail("Invalid arch cap type");
    }

    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST منوال_same_Region_As(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {

    حالة cap_pdpt_cap:
        إذا (cap_get_capType(cap_b) == cap_pdpt_cap) {
            أرجع cap_pdpt_cap_get_capPDPTBasePtr(cap_a) ==
                   cap_pdpt_cap_get_capPDPTBasePtr(cap_b);
        }
        أرجع false;

    حالة cap_pml4_cap:
        إذا (cap_get_capType(cap_b) == cap_pml4_cap) {
            أرجع cap_pml4_cap_get_cap_PML4_ادھار_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_ادھار_Ptr(cap_b);
        }
        أرجع false;
    افتراضي:
        أرجع false;
    }
}

لفظ_t منوال_get_Object_ناپ(لفظ_t t)
{
    بدل (t) {
    حالة seL4_X64_PML4Object:
        أرجع se_L4_PML4_بٹ;

    حالة se_L4_X64_Huge_ورقہ_Object:
        أرجع ورقہ_بٹ_For_ناپ(X64_Huge_ورقہ);

    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t منوال_create_Object(object_t t, فراغ *region_ادھار, لفظ_t user_ناپ, bool_t device_حافظہ)
{
    بدل (t) {

    حالة seL4_X86_4K:
        إذا (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64SmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64SmallPageBits))" */
        }
        أرجع cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_ادھار, /* capFBasePtr              */
                   X86_Small_ورقہ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالة se_L4_X86_Large_ورقہ_Object:
        إذا (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 512
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64LargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64LargePageBits))" */
        }
        أرجع cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_ادھار, /* capFBasePtr              */
                   X86_Large_ورقہ,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالة se_L4_X64_Huge_ورقہ_Object:
        إذا (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 262144
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.X64HugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat X64HugePageBits))" */
        }
        أرجع cap_چوکھٹا_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (لفظ_t)region_ادھار, /* capFBasePtr              */
                   X64_Huge_ورقہ,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظہ        /* capFIsDevice             */
               );

    حالة se_L4_X86_ورقہ_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        أرجع cap_ورقہ_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (لفظ_t)region_ادھار,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    حالة se_L4_X86_ورقہ_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        أرجع cap_ورقہ_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (لفظ_t)region_ادھار,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    حالة seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        أرجع cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (لفظ_t)region_ادھار,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    حالة seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_ادھار));
        أرجع cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (لفظ_t)region_ادھار,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#جے_تعریف_شدہ CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_ورقہ_جدول_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (لفظ_t)region_ادھار
               );
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

exception_t منوال_decode_Invocation(
    لفظ_t label,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *تھاں,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_pml4_cap:
    حالة cap_pdpt_cap:
    حالة cap_ورقہ_directory_cap:
    حالة cap_ورقہ_جدول_cap:
    حالة cap_چوکھٹا_cap:
        أرجع decodeX86MMUInvocation(label, لمائی, cptr, تھاں, cap, call, بفر);

    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
