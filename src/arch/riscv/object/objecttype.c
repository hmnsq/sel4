/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 * Copyright 2015, 2016 Hesham Almatary <heshamelmatary@gmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <types.h>
#شامل <api/failures.h>
#شامل <kernel/vspace.h>
#شامل <object/structures.h>
#شامل <arch/machine.h>
#شامل <arch/model/statedata.h>
#شامل <arch/object/objecttype.h>

deriveCap_ret_t Arch_deriveCap(cte_t *خانہ, cap_t cap)
{
    deriveCap_ret_t ret;

    انتخاب (cap_get_capType(cap)) {

    حالت cap_صفحہ_جدول_cap:
        اگر (cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            ret.cap = cap;
            ret.حالت_2 = EXCEPTION_NONE;
        } ورنہ {
            userError("Deriving an unmapped PT cap");
            current_syscall_error.type = seL4_IllegalOperation;
            ret.cap = cap_null_cap_new();
            ret.حالت_2 = EXCEPTION_SYSCALL_ERROR;
        }
        واپس ret;

    حالت cap_چوکھٹا_cap:
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, 0);
        ret.cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asidInvalid);
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    حالت cap_asid_control_cap:
    حالت cap_asid_pool_cap:
        ret.cap = cap;
        ret.حالت_2 = EXCEPTION_NONE;
        واپس ret;

    طےشدہ:
        /* This assert has no equivalent in haskell,
         * as the options are restricted by type */
        fail("Invalid arch cap type");
    }
}

cap_t CONST Arch_update_Cap_ڈیٹا(bool_t preserve, لفظ_t ڈیٹا, cap_t cap)
{
    واپس cap;
}

cap_t CONST Arch_نقاب_Cap_Rights(seL4_CapRights_t cap_rights_نقاب, cap_t cap)
{
    اگر (cap_get_capType(cap) == cap_چوکھٹا_cap) {
        vm_rights_t vm_rights;

        vm_rights = vm_Rights_From_لفظ(cap_چوکھٹا_cap_get_cap_FVMRights(cap));
        vm_rights = نقاب_VMRights(vm_rights, cap_rights_نقاب);
        واپس cap_چوکھٹا_cap_سیٹ_cap_FVMRights(cap, لفظ_From_VMRights(vm_rights));
    } ورنہ {
        واپس cap;
    }
}

finaliseCap_ret_t Arch_finaliseCap(cap_t cap, bool_t final)
{
    finaliseCap_ret_t fc_ret;

    انتخاب (cap_get_capType(cap)) {
    حالت cap_چوکھٹا_cap:

        اگر (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
            unmap_صفحہ(cap_چوکھٹا_cap_get_cap_FSize(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                      cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
                      cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));
        }
        توڑو;
    حالت cap_صفحہ_جدول_cap:
        اگر (final && cap_صفحہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
            /*
             * This PageTable is either mapped as a vspace_root or otherwise exists
             * as an entry in another PageTable. We check if it is a vspace_root and
             * if it is delete the entry from the ASID pool otherwise we treat it as
             * a mapped PageTable and unmap it from whatever page table it is mapped
             * into.
             */
            asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
            ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
            pte_t *pte = PTE_PTR(cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap));
            اگر (ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE && ڈھونڈو_ret.vspace_جڑ == pte) {
                deleteASID(asid, pte);
            } ورنہ {
                unmap_صفحہ_جدول(asid, cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap), pte);
            }
        }
        توڑو;
    حالت cap_asid_pool_cap:
        اگر (final) {
            deleteASIDPool(
                cap_asid_pool_cap_get_capASIDBase(cap),
                ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))
            );
        }
        توڑو;
    حالت cap_asid_control_cap:
        توڑو;
    }
    fc_ret.remainder = cap_null_cap_new();
    fc_ret.cleanupInfo = cap_null_cap_new();
    واپس fc_ret;
}

bool_t CONST Arch_sameRegionAs(cap_t cap_a, cap_t cap_b)
{
    انتخاب (cap_get_capType(cap_a)) {
    حالت cap_چوکھٹا_cap:
        اگر (cap_get_capType(cap_b) == cap_چوکھٹا_cap) {
            لفظ_t botA, botB, چوٹی_A, چوٹی_B;
            botA = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a);
            botB = cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b);
            چوٹی_A = botA + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_a)));
            چوٹی_B = botB + نقاب(صفحہ_بٹ_For_سائز(cap_چوکھٹا_cap_get_cap_FSize(cap_b))) ;
            واپس ((botA <= botB) && (چوٹی_A >= چوٹی_B) && (botB <= چوٹی_B));
        }
        توڑو;

    حالت cap_صفحہ_جدول_cap:
        اگر (cap_get_capType(cap_b) == cap_صفحہ_جدول_cap) {
            واپس cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_a) ==
                   cap_صفحہ_جدول_cap_get_cap_PTBase_Ptr(cap_b);
        }
        توڑو;
    حالت cap_asid_control_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_control_cap) {
            واپس true;
        }
        توڑو;

    حالت cap_asid_pool_cap:
        اگر (cap_get_capType(cap_b) == cap_asid_pool_cap) {
            واپس cap_asid_pool_cap_get_capASIDPool(cap_a) ==
                   cap_asid_pool_cap_get_capASIDPool(cap_b);
        }
        توڑو;
    }

    واپس false;
}


bool_t CONST Arch_sameObjectAs(cap_t cap_a, cap_t cap_b)
{
    اگر ((cap_get_capType(cap_a) == cap_چوکھٹا_cap) &&
        (cap_get_capType(cap_b) == cap_چوکھٹا_cap)) {
        واپس ((cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_a) ==
                 cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap_b)) &&
                (cap_چوکھٹا_cap_get_cap_FSize(cap_a) ==
                 cap_چوکھٹا_cap_get_cap_FSize(cap_b)) &&
                ((cap_چوکھٹا_cap_get_cap_FIs_Device(cap_a) == 0) ==
                 (cap_چوکھٹا_cap_get_cap_FIs_Device(cap_b) == 0)));
    }
    واپس Arch_sameRegionAs(cap_a, cap_b);
}

لفظ_t Arch_get_Object_سائز(لفظ_t t)
{
    انتخاب (t) {
    حالت se_L4_RISCV_4_K_صفحہ:
    حالت se_L4_RISCV_صفحہ_جدول_Object:
        واپس se_L4_صفحہ_بٹ;
    حالت se_L4_RISCV_Mega_صفحہ:
        واپس se_L4_Large_صفحہ_بٹ;
#اگر CONFIG_PT_سطحیں > 2
    حالت se_L4_RISCV_Giga_صفحہ:
        واپس se_L4_Huge_صفحہ_بٹ;
#اگر_ختم
#اگر CONFIG_PT_سطحیں > 3
    حالت se_L4_RISCV_Tera_صفحہ:
        واپس se_L4_Tera_صفحہ_بٹ;
#اگر_ختم
    طےشدہ:
        fail("Invalid object type");
        واپس 0;
    }
}

cap_t Arch_createObject(object_t t, خالی *region_بنیاد, لفظ_t user_سائز, bool_t
                        device_حافظہ)
{
    انتخاب (t) {
    حالت se_L4_RISCV_4_K_صفحہ:
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps 1
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVSmallPage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVPageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (لفظ_t) region_بنیاد,            /* capFBasePtr          */
                   RISCV_4_K_صفحہ,                  /* capFSize             */
                   لفظ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظہ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );

    حالت se_L4_RISCV_Mega_صفحہ: {
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps (2^9)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVLargePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVMegaPageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (لفظ_t) region_بنیاد,            /* capFBasePtr          */
                   RISCV_Mega_صفحہ,                  /* capFSize             */
                   لفظ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظہ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }

#اگر CONFIG_PT_سطحیں > 2
    حالت se_L4_RISCV_Giga_صفحہ: {
        اگر (device_حافظہ) {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_device_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        } ورنہ {
            /** AUXUPD: "(True, ptr_retyps (2^18)
                     (Ptr (ptr_val \<acute>regionBase) :: user_data_C ptr))" */
            /** GHOSTUPD: "(True, gs_new_frames vmpage_size.RISCVHugePage
                                                    (ptr_val \<acute>regionBase)
                                                    (unat RISCVGigaPageBits))" */
        }
        واپس cap_چوکھٹا_cap_new(
                   asidInvalid,                    /* capFMappedASID       */
                   (لفظ_t) region_بنیاد,            /* capFBasePtr          */
                   RISCV_Giga_صفحہ,                  /* capFSize             */
                   لفظ_From_VMRights(VMReadWrite),  /* capFVMRights         */
                   device_حافظہ,                   /* capFIsDevice         */
                   0                               /* capFMappedAddress    */
               );
    }
#اگر_ختم

    حالت se_L4_RISCV_صفحہ_جدول_Object:
        /** AUXUPD: "(True, ptr_retyps 1
              (Ptr (ptr_val \<acute>regionBase) :: (pte_C[512]) ptr))" */
        واپس cap_صفحہ_جدول_cap_new(
                   asidInvalid,            /* capPTMappedASID    */
                   (لفظ_t)region_بنیاد,     /* capPTBasePtr       */
                   0,                      /* capPTIsMapped      */
                   0                       /* capPTMappedAddress */
               );

    طےشدہ:
        /*
         * This is a conflation of the haskell error: "Arch.createNewCaps
         * got an API type" and the case where an invalid object type is
         * passed (which is impossible in haskell).
         */
        fail("Arch_createObject got an API type or invalid object type");
    }
}

exception_t Arch_decodeInvocation(
    لفظ_t label,
    لفظ_t لمبائی,
    cptr_t cptr,
    cte_t *خانہ,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    واپس decodeRISCVMMUInvocation(label, لمبائی, cptr, خانہ, cap, call, بفر);
}

خالی Arch_prepareThreadDelete(tcb_t *thread)
{
#اگر_تعریف_شدہ CONFIG_HAVE_FPU
    fpuRelease(thread);
#اگر_ختم
}

bool_t Arch_is_چوکھٹا_Type(لفظ_t type)
{
    انتخاب (type) {
#اگر CONFIG_PT_سطحیں == 4
    حالت se_L4_RISCV_Tera_صفحہ:
#اگر_ختم
#اگر CONFIG_PT_سطحیں > 2
    حالت se_L4_RISCV_Giga_صفحہ:
#اگر_ختم
    حالت se_L4_RISCV_Mega_صفحہ:
    حالت se_L4_RISCV_4_K_صفحہ:
        واپس true;
    طےشدہ:
        واپس false;
    }
}
