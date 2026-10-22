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


bool_t Arch_is_قاب_Type(کلمه_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_صفحه_Object:
        أرجع true;
    حالة se_L4_X64_Huge_صفحه_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t نما_derive_Cap(cte_t *جایگاه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_pml4_cap:
        إذا (cap_pml4_cap_get_capPML4IsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving a PML4 cap without an assigned ASID");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_pdpt_cap:
        إذا (cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PTPD cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_قاب_cap:
        cap = cap_قاب_cap_مجموعه_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asidInvalid);
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t نما_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        إذا (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_پایه_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_pdpt_cap:
        إذا (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_آدرس(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        اكسر;

    حالة cap_قاب_cap:
        إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_قاب_cap_get_cap_FMap_Type(cap)) {
#اگر_تعریف_شده CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_قاب_cap_get_cap_FSize(cap),
                    cap_قاب_cap_get_cap_FMapped_ASID(cap),
                    cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                    (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#پایان_اگر
#اگر_تعریف_شده CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#پایان_اگر
            حالة X86_MappingVSpace:
                unmap_صفحه(
                    cap_قاب_cap_get_cap_FSize(cap),
                    cap_قاب_cap_get_cap_FMapped_ASID(cap),
                    cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                    (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
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

bool_t CONST نما_same_Region_As(cap_t cap_a, cap_t cap_b)
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
            أرجع cap_pml4_cap_get_cap_PML4_پایه_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_پایه_Ptr(cap_b);
        }
        أرجع false;
    افتراضي:
        أرجع false;
    }
}

کلمه_t نما_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة seL4_X64_PML4Object:
        أرجع se_L4_PML4_بیتها;

    حالة se_L4_X64_Huge_صفحه_Object:
        أرجع صفحه_بیتها_For_اندازه(X64_Huge_صفحه);

    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t نما_create_Object(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t device_حافظه)
{
    بدل (t) {

    حالة seL4_X86_4K:
        إذا (device_حافظه) {
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
        أرجع cap_قاب_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_پایه, /* capFBasePtr              */
                   X86_Small_صفحه,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X86_Large_صفحه_Object:
        إذا (device_حافظه) {
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
        أرجع cap_قاب_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_پایه, /* capFBasePtr              */
                   X86_Large_صفحه,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X64_Huge_صفحه_Object:
        إذا (device_حافظه) {
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
        أرجع cap_قاب_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_پایه, /* capFBasePtr              */
                   X64_Huge_صفحه,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X86_صفحه_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        أرجع cap_صفحه_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (کلمه_t)region_پایه,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    حالة se_L4_X86_صفحه_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        أرجع cap_صفحه_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (کلمه_t)region_پایه,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    حالة seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        أرجع cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (کلمه_t)region_پایه,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    حالة seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_پایه));
        أرجع cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (کلمه_t)region_پایه,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#اگر_تعریف_شده CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_صفحه_جدول_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (کلمه_t)region_پایه
               );
#پایان_اگر

    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t نما_decode_Invocation(
    کلمه_t label,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_pml4_cap:
    حالة cap_pdpt_cap:
    حالة cap_صفحه_directory_cap:
    حالة cap_صفحه_جدول_cap:
    حالة cap_قاب_cap:
        أرجع decodeX86MMUInvocation(label, درازی, cptr, جایگاه, cap, call, میانگیر);

    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
