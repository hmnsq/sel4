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

deriveCap_ret_t Arch_deriveCap(cte_t *خونه, cap_t cap)
{
    deriveCap_ret_t ret;

    بدل (cap_get_capType(cap)) {

    حالة cap_پاڼه_جدول_cap:
        إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت = EXCEPTION_NONE;
        } وإلا {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت = EXCEPTION_SYSCALL_ERROR;
        }
        أرجع ret;

    حالة cap_چوکاټ_cap:
        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, 0);
        ret.cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    حالة cap_asid_control_cap:
    حالة cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;

    افتراضي:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_معلومات(bool_t preserve, کلمه_t معلومات, cap_t cap)
{
    أرجع cap;
}

cap_t CONST Arch_ماسک_Cap_Rights(seL4_CapRights_t cap_rights_ماسک, cap_t cap)
{
    إذا (cap_get_capType(cap) == cap_چوکاټ_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_کلمه(cap_چوکاټ_cap_get_cap_FVMRights(cap));
        vm_rights = ماسک_VMRights(vm_rights, cap_rights_ماسک);
        أرجع cap_چوکاټ_cap_سېټ_cap_FVMRights(cap, کلمه_From_VMRights(vm_rights));
    } وإلا {
        أرجع cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    بدل (cap_get_capType(cap)) {
    حالة cap_چوکاټ_cap:

        إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap)) {
            unmap_پاڼه(cap_چوکاټ_cap_get_cap_FSize(cap),
                      cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
                      cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));
        }
        اكسر;
    حالة cap_پاڼه_جدول_cap:
        إذا (final && cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
            پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
            إذا (پیدا_کړه_ret.حالت == EXCEPTION_NONE && پیدا_کړه_ret.vspace_ریښه == pte) {
                deleteASID(asid, pte);
            } وإلا {
                unmap_پاڼه_جدول(asid, cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap), pte);
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
    حالة cap_چوکاټ_cap:
        إذا (cap_get_capType(cap_b) == cap_چوکاټ_cap) {
            کلمه_t botA, botB, سر_A, سر_B;
            botA = cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_b);
            سر_A = botA + ماسک(پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap_a)));
            سر_B = botB + ماسک(پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(cap_b))) ;
            أرجع ((botA <= botB) && (سر_A >= سر_B) && (botB <= سر_B));
        }
        اكسر;

    حالة cap_پاڼه_جدول_cap:
        إذا (cap_get_capType(cap_b) == cap_پاڼه_جدول_cap) {
            أرجع cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap_b);
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
    إذا ((cap_get_capType(cap_a) == cap_چوکاټ_cap) &&
        (cap_get_capType(cap_b) == cap_چوکاټ_cap)) {
        أرجع ((cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_چوکاټ_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_چوکاټ_cap_get_cap_FSize(cap_a) ==
                 cap_چوکاټ_cap_get_cap_FSize(cap_b)) &&
                ((cap_چوکاټ_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_چوکاټ_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    أرجع Arch_sameRegionAs(cap_a, cap_b);
}

کلمه_t Arch_get_Object_اندازه(کلمه_t t)
{
    بدل (t) {
    حالة se_L4_RISCV_4_K_پاڼه:
    حالة se_L4_RISCV_پاڼه_جدول_Object:
        أرجع se_L4_پاڼه_بټونه;
    حالة se_L4_RISCV_Mega_پاڼه:
        أرجع se_L4_Large_پاڼه_بټونه;
#إذا CONFIG_PT_پوړونه > 2
    حالة se_L4_RISCV_Giga_پاڼه:
        أرجع se_L4_Huge_پاڼه_بټونه;
#د_که_پای
#إذا CONFIG_PT_پوړونه > 3
    حالة se_L4_RISCV_Tera_پاڼه:
        أرجع se_L4_Tera_پاڼه_بټونه;
#د_که_پای
    افتراضي:
        fail("Invalid object type");
        أرجع 0;
    }
}

cap_t Arch_createObject(object_t t, فراغ *region_بنسټ, کلمه_t user_اندازه, bool_t
                        device_حافظه)
{
    بدل (t) {
    حالة se_L4_RISCV_4_K_پاڼه:
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_بنسټ,            /* capFBasePtr          */
                   RISCV_4_K_پاڼه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    حالة se_L4_RISCV_Mega_پاڼه: {
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_بنسټ,            /* capFBasePtr          */
                   RISCV_Mega_پاڼه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#إذا CONFIG_PT_پوړونه > 2
    حالة se_L4_RISCV_Giga_پاڼه: {
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
        أرجع cap_چوکاټ_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (کلمه_t) region_بنسټ,            /* capFBasePtr          */
                   RISCV_Giga_پاڼه,                  /* capFSize             */
                   کلمه_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظه,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#د_که_پای

    حالة se_L4_RISCV_پاڼه_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        أرجع cap_پاڼه_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (کلمه_t)region_بنسټ,     /* capPTBasePtr       */
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
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *خونه,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    أرجع decodeRISCVMMUInvocation(label, اوږدتیا, cptr, خونه, cap, call, بفر);
}

فراغ Arch_prepareThreadDelete(tcb_t *thread)
{
#که_تعریف_شوی CONFIG_HAVE_FPU
    fpuRelease(thread);
#د_که_پای
}

bool_t Arch_is_چوکاټ_Type(کلمه_t type)
{
    بدل (type) {
#إذا CONFIG_PT_پوړونه == 4
    حالة se_L4_RISCV_Tera_پاڼه:
#د_که_پای
#إذا CONFIG_PT_پوړونه > 2
    حالة se_L4_RISCV_Giga_پاڼه:
#د_که_پای
    حالة se_L4_RISCV_Mega_پاڼه:
    حالة se_L4_RISCV_4_K_پاڼه:
        أرجع true;
    افتراضي:
        أرجع false;
    }
}
