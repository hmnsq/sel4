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


bool_t Arch_is_چوکاټ_Type(کلمه_t type)
{
    بدل (type) {
    حالة seL4_X86_4K:
        أرجع true;
    حالة se_L4_X86_Large_پاڼه_Object:
        أرجع true;
    حالة se_L4_X64_Huge_پاڼه_Object:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}

deriveCap_ret_t ډېرپېښ_derive_Cap(cte_t *خونه, cap_t cap)
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

    حالة cap_چوکاټ_cap:
        cap = cap_چوکاټ_cap_سېټ_cap_FMap_Type(cap, X86_MappingNone);
        ret.cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        fail("Invalid arch cap type");
    }
}

finaliseCap_ret_t ډېرپېښ_finalise_Cap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_pml4_cap:
        إذا (final && cap_pml4_cap_get_capPML4IsMapped(cap)) {
            deleteASID(
                cap_pml4_cap_get_capPML4MappedASID(cap),
                PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(cap))
            );
        }
        اكسر;

    حالة cap_pdpt_cap:
        إذا (final && cap_pdpt_cap_get_capPDPTIsMapped(cap)) {
            unmapPDPT(
                cap_pdpt_cap_get_capPDPTMappedASID(cap),
                cap_pdpt_cap_get_cap_PDPTMapped_پته(cap),
                PDPTE_PTR(cap_pdpt_cap_get_capPDPTBasePtr(cap))
            );
        }
        اكسر;

    حالة cap_چوکاټ_cap:
        إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap)) {
            بدل (cap_چوکاټ_cap_get_cap_FMap_Type(cap)) {
#که_تعریف_شوی CONFIG_VTX
            حالة X86_MappingEPT:
                unmapEPTPage(
                    cap_چوکاټ_cap_get_cap_FSize(cap),
                    cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                    (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)
                );
                اكسر;
#د_که_پای
#که_تعریف_شوی CONFIG_IOMMU
            حالة X86_MappingIOSpace:
                unmapIOPage(cap);
                اكسر;
#د_که_پای
            حالة X86_MappingVSpace:
                unmap_پاڼه(
                    cap_چوکاټ_cap_get_cap_FSize(cap),
                    cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                    cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                    (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)
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

bool_t CONST ډېرپېښ_same_Region_As(cap_t cap_a, cap_t cap_b)
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
            أرجع cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(cap_a) ==
                   cap_pml4_cap_get_cap_PML4_بنسټ_Ptr(cap_b);
        }
        أرجع false;
    افتراضي:
        أرجع false;
    }
}

کلمه_t ډېرپېښ_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة seL4_X64_PML4Object:
        أرجع se_L4_PML4_بټونه;

    حالة se_L4_X64_Huge_پاڼه_Object:
        أرجع پاڼه_بټونه_For_اندازه(X64_Huge_پاڼه);

    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t ډېرپېښ_create_Object(object_t t, فراغ *region_بنسټ, کلمه_t user_اندازه, bool_t device_حافظه)
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_بنسټ, /* capFBasePtr              */
                   X86_Small_پاڼه,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X86_Large_پاڼه_Object:
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_بنسټ, /* capFBasePtr              */
                   X86_Large_پاڼه,      /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X64_Huge_پاڼه_Object:
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,        /* capFMappedASID           */
                   (کلمه_t)region_بنسټ, /* capFBasePtr              */
                   X64_Huge_پاڼه,       /* capFSize                 */
                   X86_MappingNone,    /* capFMapType              */
                   0,                  /* capFMappedAddress        */
                   VMReadWrite,        /* capFVMRights             */
                   device_حافظه        /* capFIsDevice             */
               );

    حالة se_L4_X86_پاڼه_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        أرجع cap_پاڼه_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (کلمه_t)region_بنسټ,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    حالة se_L4_X86_پاڼه_Directory_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pde_C[512]) ptr))" */
        أرجع cap_پاڼه_directory_cap_new(
                   asidInvalid,                /* capPDMappedASID      */
                   (کلمه_t)region_بنسټ,         /* capPDBasePtr         */
                   0,                          /* capPDIsMapped        */
                   0                           /* capPDMappedAddress   */
               );

    حالة seL4_X86_PDPTObject:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pdpte_C[512]) ptr))" */
        أرجع cap_pdpt_cap_new(
                   asidInvalid,                /* capPDPTMappedASID    */
                   (کلمه_t)region_بنسټ,         /* capPDPTBasePtr       */
                   0,                          /* capPDPTIsMapped      */
                   0                           /* capPDPTMappedAddress */
               );

    حالة seL4_X64_PML4Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pml4e_C[512]) ptr))" */
        copyGlobalMappings(PML4_PTR(region_بنسټ));
        أرجع cap_pml4_cap_new(
                   asidInvalid,                /* capPML4MappedASID   */
                   (کلمه_t)region_بنسټ,         /* capPML4BasePtr      */
                   0                           /* capPML4IsMapped     */
               );

#که_تعریف_شوی CONFIG_IOMMU
    حالة se_L4_X86_IOPage_جدول_Object:
        أرجع cap_io_پاڼه_جدول_cap_new(
                   0,
                   0,
                   0,
                   asidInvalid,
                   (کلمه_t)region_بنسټ
               );
#د_که_پای

    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t ډېرپېښ_decode_Invocation(
    کلمه_t label,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *خونه,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_pml4_cap:
    حالة cap_pdpt_cap:
    حالة cap_پاڼه_directory_cap:
    حالة cap_پاڼه_جدول_cap:
    حالة cap_چوکاټ_cap:
        أرجع decodeX86MMUInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);

    افتراضي:
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}
