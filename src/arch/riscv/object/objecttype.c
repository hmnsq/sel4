/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <types.h>
#ضمن <api/failures.h>
#ضمن <kernel/vspace.h>
#ضمن <object/structures.h>
#ضمن <arch/machine.h>
#ضمن <arch/model/statedata.h>
#ضمن <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *جایگاه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_صفحه_جدول_cap:
        إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.وضعیت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_قاب_cap:
        cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, 0);
        ret.cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asidInvalid);
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_داده(bool_t preserve, کلمه_t داده, cap_t cap)
{
    أرجع cap;
}

cap_t CONST Arch_ماسک_Cap_Rights(seL4_CapRights_t cap_rights_ماسک, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_قاب_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(cap_قاب_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);
        أرجع cap_قاب_cap_مجموعه_cap_FVMRights(cap, کلمه_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_قاب_cap:

        إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap)) {
            unmap_صفحه(cap_قاب_cap_get_cap_FSize(cap),
                      cap_قاب_cap_get_cap_FMapped_ASID(cap),
                      cap_قاب_cap_get_cap_FMapped_آدرس(cap),
                      cap_قاب_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;
    حالة cap_صفحه_جدول_cap:
        إذا (final && cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
            پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (پیدا_کن_ret.وضعیت == EXCEPTION_NONE && پیدا_کن_ret.vspace_ریشه == pte) {
                deleteASID(asid, pte);
            } وإلا {
                unmap_صفحه_جدول(asid, cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap), pte);
            }
        }
        اكسر;
    حالة cap_asid_pool_cap:
        إذا (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        اكسر;
    حالة cap_asid_control_cap:
        اكسر;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    أرجع fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    بدل (cap_get_capType(cap_a)) {
    حالة cap_قاب_cap:
        إذا (cap_get_capType(cap_b) == cap_قاب_cap) {
            کلمه_t botA, botB, بالاترین_A, بالاترین_B;
            botA = cap_قاب_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_قاب_cap_get_cap_FBase_Ptr(cap_b);
            بالاترین_A = botA + ماسک(صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap_a)));
            بالاترین_B = botB + ماسک(صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(cap_b))) ;
            أرجع ((botA <= botB) && (بالاترین_A >= بالاترین_B) && (botB <= بالاترین_B));
        }
        اكسر;

    حالة cap_صفحه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_صفحه_جدول_cap) {
            أرجع cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        اكسر;
    حالة cap_asid_control_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_control_cap) {
            أرجع true;
        }
        اكسر;

    حالة cap_asid_pool_cap:
        إذا (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            أرجع cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        اكسر;
    }

    أرجع false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    إذا ((cap_get_capType(cap_a) == cap_قاب_cap) &&
        (cap_get_capType(cap_b) == cap_قاب_cap)) {
        أرجع ((cap_قاب_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_قاب_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_قاب_cap_get_cap_FSize(cap_a) ==
                 cap_قاب_cap_get_cap_FSize(cap_b)) &&
                ((cap_قاب_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_قاب_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

کلمه_t Arch_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة se_L4_RISCV_4_K_صفحه:
    حالة se_L4_RISCV_صفحه_جدول_Object:
        أرجع se_L4_صفحه_بیتها;
    حالة se_L4_RISCV_Mega_صفحه:
        أرجع se_L4_Large_صفحه_بیتها;
#إذا CONFIG_PT_سطوح > 2
    حالة se_L4_RISCV_Giga_صفحه:
        أرجع se_L4_Huge_صفحه_بیتها;
#پایان_اگر
#إذا CONFIG_PT_سطوح > 3
    حالة se_L4_RISCV_Tera_صفحه:
        أرجع se_L4_Tera_صفحه_بیتها;
#پایان_اگر
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_پایه, کلمه_t user_اندازه, bool_t
                        device_حافظه)
{
    بدل (t) {
    حالة se_L4_RISCV_4_K_صفحه:
        إذا (device_حافظه) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        أرجع cap_قاب_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_پایه,            /* capFBasePtr          */
                   RISCV_4_K_صفحه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    حالة se_L4_RISCV_Mega_صفحه: {
        إذا (device_حافظه) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        أرجع cap_قاب_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_پایه,            /* capFBasePtr          */
                   RISCV_Mega_صفحه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#إذا CONFIG_PT_سطوح > 2
    حالة se_L4_RISCV_Giga_صفحه: {
        إذا (device_حافظه) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } وإلا {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        أرجع cap_قاب_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_پایه,            /* capFBasePtr          */
                   RISCV_Giga_صفحه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#پایان_اگر

    حالة se_L4_RISCV_صفحه_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        أرجع cap_صفحه_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (کلمه_t)region_پایه,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    افتراضي:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    کلمه_t label,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *جایگاه,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    أرجع decodeRISCVMMUInvocation(label, درازی, cptr, جایگاه, cap, call, میانگیر);
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
#اگر_تعریف_شده CONFIG_HAVE_FPU
    fpuRelease(thread);
#پایان_اگر
}

bool_t Arch_is_قاب_Type(کلمه_t type)
{
    بدل (type) {
#إذا CONFIG_PT_سطوح == 4
    حالة se_L4_RISCV_Tera_صفحه:
#پایان_اگر
#إذا CONFIG_PT_سطوح > 2
    حالة se_L4_RISCV_Giga_صفحه:
#پایان_اگر
    حالة se_L4_RISCV_Mega_صفحه:
    حالة se_L4_RISCV_4_K_صفحه:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
