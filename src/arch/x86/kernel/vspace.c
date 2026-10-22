/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <api/syscall.h>
#ضمن <machine/io.h>
#ضمن <kernel/boot.h>
#ضمن <model/statedata.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <arch/api/invocation.h>
#ضمن <arch/kernel/tlb_bitmap.h>
#ضمن <mode/kernel/tlb.h>
#ضمن <mode/kernel/vspace.h>

ساكن exception_t perform_ورقہ_Get_پتہ(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گرہ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سیٹ_رجسٹر(thread, badge_رجسٹر, 0);
        بنانشانی صحيح لمائی = سیٹ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سیٹ_رجسٹر(thread, پیغام_Info_رجسٹر, لفظ_From_سنیہا_Info(
                        se_L4_سنیہا_Info_new(0, 0, 0, لمائی)));
    }
    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

فراغ deleteASIDPool(asid_t asid_ادھار, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_ادھار, asid_کم_بٹ));

    إذا (x86KSASIDTable[ASID_بلند(asid_ادھار)] == pool) {
        لكل (بنانشانی صحيح وتھ = 0; وتھ < ثنائی_ہندسہ(asid_کم_بٹ); وتھ++) {
            asid_map_t asid_map = pool->array[وتھ];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_جڑ_t *vspace = (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map);
                hwASIDInvalidate(asid_ادھار + وتھ, vspace);
            }
        }
        x86KSASIDTable[ASID_بلند(asid_ادھار)] = عدم;
        سیٹ_VMRoot(گرہ_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(فراغ *چوکھٹا, cte_t *تھاں, cte_t *ماپا, asid_t asid_ادھار)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سیٹ_cap_Free_اشاریہ(&(ماپا->cap),
                                         MAX_FREE_اشاریہ(cap_untyped_cap_get_cap_بلاک_ناپ(ماپا->cap)));

    memzero(چوکھٹا, ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(X86_Small_ورقہ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_داخل_کرو(
        cap_asid_pool_cap_new(
            asid_ادھار,          /* capASIDBase  */
            لفظ_REF(چوکھٹا)     /* capASIDPool  */
        ),
        ماپا,
        تھاں
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_ادھار & نقاب(asid_کم_بٹ)) == 0);
    x86KSASIDTable[ASID_بلند(asid_ادھار)] = (asid_pool_t *)چوکھٹا;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, vspace_جڑ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بلند(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
            سیٹ_VMRoot(گرہ_STATE(ksCurThread));
        }
    }
}

لفظ_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    لفظ_t      w_بفر_Ptr;
    cap_t       بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (cap_get_capType(بفر_Cap) != cap_چوکھٹا_cap) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکھٹا_cap_get_cap_FVMRights(بفر_Cap);
    إذا (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        لفظ_t ادھار_Ptr, ورقہ_بٹ_2;

        ادھار_Ptr = cap_چوکھٹا_cap_get_cap_FBase_Ptr(بفر_Cap);
        ورقہ_بٹ_2 = ورقہ_بٹ_For_ناپ(cap_چوکھٹا_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(ادھار_Ptr + (w_بفر_Ptr & نقاب(ورقہ_بٹ_2)));
    } وإلا {
        أرجع عدم;
    }
}

bool_t CONST is_Valid_VTable_جڑ(cap_t cap)
{
    أرجع is_Valid_Native_جڑ(cap);
}


BOOT_کوڈ bool_t map_kernel_کھڑکی_devices(pte_t *pt, uint32_t عدد_ioapic, paddr_t *ioapic_paddrs, uint32_t عدد_drhu,
                                           paddr_t *drhu_list)
{
    لفظ_t idx = (KDEV_ادھار & نقاب(LARGE_ورقہ_بٹ)) >> ورقہ_بٹ;
    paddr_t phys;
    pte_t pte;
    بنانشانی صحيح i;
    /* map kernel devices: APIC */
    phys = apic_get_ادھار_paddr();
    إذا (!phys) {
        أرجع false;
    }
    إذا (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        أرجع false;
    }
    pte = x86_make_device_pte(phys);

    أكد(idx == (PPTR_APIC & نقاب(LARGE_ورقہ_بٹ)) >> ورقہ_بٹ);
    pt[idx] = pte;
    idx++;
    لكل (i = 0; i < عدد_ioapic; i++) {
        phys = ioapic_paddrs[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);
        أكد(idx == ((PPTR_IOAPIC_شروع + i * ثنائی_ہندسہ(ورقہ_بٹ)) & نقاب(LARGE_ورقہ_بٹ)) >> ورقہ_بٹ);
        pt[idx] = pte;
        idx++;
        إذا (idx == ثنائی_ہندسہ(PT_اشاریہ_بٹ)) {
            أرجع false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    لكل (; i < CONFIG_MAX_عدد_IOAPIC; i++) {
        pte = x86_make_خالی_تھاواں_pte();
        أكد(idx == ((PPTR_IOAPIC_شروع + i * ثنائی_ہندسہ(ورقہ_بٹ)) & نقاب(LARGE_ورقہ_بٹ)) >> ورقہ_بٹ);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    لكل (i = 0; i < عدد_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);

        أكد(idx == ((PPTR_DRHU_شروع + i * ثنائی_ہندسہ(ورقہ_بٹ)) & نقاب(LARGE_ورقہ_بٹ)) >> ورقہ_بٹ);
        pt[idx] = pte;
        idx++;
        إذا (idx == ثنائی_ہندسہ(PT_اشاریہ_بٹ)) {
            أرجع false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    طالما (idx < ثنائی_ہندسہ(PT_اشاریہ_بٹ)) {
        pte = x86_make_خالی_تھاواں_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    أكد(idx == ثنائی_ہندسہ(PT_اشاریہ_بٹ));
    أرجع true;
}

BOOT_کوڈ ساكن فراغ init_idt(idt_اندراج_t *idt)
{
    init_idt_اندراج(idt, 0x00, int_00);
    init_idt_اندراج(idt, 0x01, int_01);
    init_idt_اندراج(idt, 0x02, int_02);
    init_idt_اندراج(idt, 0x03, int_03);
    init_idt_اندراج(idt, 0x04, int_04);
    init_idt_اندراج(idt, 0x05, int_05);
    init_idt_اندراج(idt, 0x06, int_06);
    init_idt_اندراج(idt, 0x07, int_07);
    init_idt_اندراج(idt, 0x08, int_08);
    init_idt_اندراج(idt, 0x09, int_09);
    init_idt_اندراج(idt, 0x0a, int_0a);
    init_idt_اندراج(idt, 0x0b, int_0b);
    init_idt_اندراج(idt, 0x0c, int_0c);
    init_idt_اندراج(idt, 0x0d, int_0d);
    init_idt_اندراج(idt, 0x0e, int_0e);
    init_idt_اندراج(idt, 0x0f, int_0f);

    init_idt_اندراج(idt, 0x10, int_10);
    init_idt_اندراج(idt, 0x11, int_11);
    init_idt_اندراج(idt, 0x12, int_12);
    init_idt_اندراج(idt, 0x13, int_13);
    init_idt_اندراج(idt, 0x14, int_14);
    init_idt_اندراج(idt, 0x15, int_15);
    init_idt_اندراج(idt, 0x16, int_16);
    init_idt_اندراج(idt, 0x17, int_17);
    init_idt_اندراج(idt, 0x18, int_18);
    init_idt_اندراج(idt, 0x19, int_19);
    init_idt_اندراج(idt, 0x1a, int_1a);
    init_idt_اندراج(idt, 0x1b, int_1b);
    init_idt_اندراج(idt, 0x1c, int_1c);
    init_idt_اندراج(idt, 0x1d, int_1d);
    init_idt_اندراج(idt, 0x1e, int_1e);
    init_idt_اندراج(idt, 0x1f, int_1f);

    init_idt_اندراج(idt, 0x20, int_20);
    init_idt_اندراج(idt, 0x21, int_21);
    init_idt_اندراج(idt, 0x22, int_22);
    init_idt_اندراج(idt, 0x23, int_23);
    init_idt_اندراج(idt, 0x24, int_24);
    init_idt_اندراج(idt, 0x25, int_25);
    init_idt_اندراج(idt, 0x26, int_26);
    init_idt_اندراج(idt, 0x27, int_27);
    init_idt_اندراج(idt, 0x28, int_28);
    init_idt_اندراج(idt, 0x29, int_29);
    init_idt_اندراج(idt, 0x2a, int_2a);
    init_idt_اندراج(idt, 0x2b, int_2b);
    init_idt_اندراج(idt, 0x2c, int_2c);
    init_idt_اندراج(idt, 0x2d, int_2d);
    init_idt_اندراج(idt, 0x2e, int_2e);
    init_idt_اندراج(idt, 0x2f, int_2f);

    init_idt_اندراج(idt, 0x30, int_30);
    init_idt_اندراج(idt, 0x31, int_31);
    init_idt_اندراج(idt, 0x32, int_32);
    init_idt_اندراج(idt, 0x33, int_33);
    init_idt_اندراج(idt, 0x34, int_34);
    init_idt_اندراج(idt, 0x35, int_35);
    init_idt_اندراج(idt, 0x36, int_36);
    init_idt_اندراج(idt, 0x37, int_37);
    init_idt_اندراج(idt, 0x38, int_38);
    init_idt_اندراج(idt, 0x39, int_39);
    init_idt_اندراج(idt, 0x3a, int_3a);
    init_idt_اندراج(idt, 0x3b, int_3b);
    init_idt_اندراج(idt, 0x3c, int_3c);
    init_idt_اندراج(idt, 0x3d, int_3d);
    init_idt_اندراج(idt, 0x3e, int_3e);
    init_idt_اندراج(idt, 0x3f, int_3f);

    init_idt_اندراج(idt, 0x40, int_40);
    init_idt_اندراج(idt, 0x41, int_41);
    init_idt_اندراج(idt, 0x42, int_42);
    init_idt_اندراج(idt, 0x43, int_43);
    init_idt_اندراج(idt, 0x44, int_44);
    init_idt_اندراج(idt, 0x45, int_45);
    init_idt_اندراج(idt, 0x46, int_46);
    init_idt_اندراج(idt, 0x47, int_47);
    init_idt_اندراج(idt, 0x48, int_48);
    init_idt_اندراج(idt, 0x49, int_49);
    init_idt_اندراج(idt, 0x4a, int_4a);
    init_idt_اندراج(idt, 0x4b, int_4b);
    init_idt_اندراج(idt, 0x4c, int_4c);
    init_idt_اندراج(idt, 0x4d, int_4d);
    init_idt_اندراج(idt, 0x4e, int_4e);
    init_idt_اندراج(idt, 0x4f, int_4f);

    init_idt_اندراج(idt, 0x50, int_50);
    init_idt_اندراج(idt, 0x51, int_51);
    init_idt_اندراج(idt, 0x52, int_52);
    init_idt_اندراج(idt, 0x53, int_53);
    init_idt_اندراج(idt, 0x54, int_54);
    init_idt_اندراج(idt, 0x55, int_55);
    init_idt_اندراج(idt, 0x56, int_56);
    init_idt_اندراج(idt, 0x57, int_57);
    init_idt_اندراج(idt, 0x58, int_58);
    init_idt_اندراج(idt, 0x59, int_59);
    init_idt_اندراج(idt, 0x5a, int_5a);
    init_idt_اندراج(idt, 0x5b, int_5b);
    init_idt_اندراج(idt, 0x5c, int_5c);
    init_idt_اندراج(idt, 0x5d, int_5d);
    init_idt_اندراج(idt, 0x5e, int_5e);
    init_idt_اندراج(idt, 0x5f, int_5f);

    init_idt_اندراج(idt, 0x60, int_60);
    init_idt_اندراج(idt, 0x61, int_61);
    init_idt_اندراج(idt, 0x62, int_62);
    init_idt_اندراج(idt, 0x63, int_63);
    init_idt_اندراج(idt, 0x64, int_64);
    init_idt_اندراج(idt, 0x65, int_65);
    init_idt_اندراج(idt, 0x66, int_66);
    init_idt_اندراج(idt, 0x67, int_67);
    init_idt_اندراج(idt, 0x68, int_68);
    init_idt_اندراج(idt, 0x69, int_69);
    init_idt_اندراج(idt, 0x6a, int_6a);
    init_idt_اندراج(idt, 0x6b, int_6b);
    init_idt_اندراج(idt, 0x6c, int_6c);
    init_idt_اندراج(idt, 0x6d, int_6d);
    init_idt_اندراج(idt, 0x6e, int_6e);
    init_idt_اندراج(idt, 0x6f, int_6f);

    init_idt_اندراج(idt, 0x70, int_70);
    init_idt_اندراج(idt, 0x71, int_71);
    init_idt_اندراج(idt, 0x72, int_72);
    init_idt_اندراج(idt, 0x73, int_73);
    init_idt_اندراج(idt, 0x74, int_74);
    init_idt_اندراج(idt, 0x75, int_75);
    init_idt_اندراج(idt, 0x76, int_76);
    init_idt_اندراج(idt, 0x77, int_77);
    init_idt_اندراج(idt, 0x78, int_78);
    init_idt_اندراج(idt, 0x79, int_79);
    init_idt_اندراج(idt, 0x7a, int_7a);
    init_idt_اندراج(idt, 0x7b, int_7b);
    init_idt_اندراج(idt, 0x7c, int_7c);
    init_idt_اندراج(idt, 0x7d, int_7d);
    init_idt_اندراج(idt, 0x7e, int_7e);
    init_idt_اندراج(idt, 0x7f, int_7f);

    init_idt_اندراج(idt, 0x80, int_80);
    init_idt_اندراج(idt, 0x81, int_81);
    init_idt_اندراج(idt, 0x82, int_82);
    init_idt_اندراج(idt, 0x83, int_83);
    init_idt_اندراج(idt, 0x84, int_84);
    init_idt_اندراج(idt, 0x85, int_85);
    init_idt_اندراج(idt, 0x86, int_86);
    init_idt_اندراج(idt, 0x87, int_87);
    init_idt_اندراج(idt, 0x88, int_88);
    init_idt_اندراج(idt, 0x89, int_89);
    init_idt_اندراج(idt, 0x8a, int_8a);
    init_idt_اندراج(idt, 0x8b, int_8b);
    init_idt_اندراج(idt, 0x8c, int_8c);
    init_idt_اندراج(idt, 0x8d, int_8d);
    init_idt_اندراج(idt, 0x8e, int_8e);
    init_idt_اندراج(idt, 0x8f, int_8f);

    init_idt_اندراج(idt, 0x90, int_90);
    init_idt_اندراج(idt, 0x91, int_91);
    init_idt_اندراج(idt, 0x92, int_92);
    init_idt_اندراج(idt, 0x93, int_93);
    init_idt_اندراج(idt, 0x94, int_94);
    init_idt_اندراج(idt, 0x95, int_95);
    init_idt_اندراج(idt, 0x96, int_96);
    init_idt_اندراج(idt, 0x97, int_97);
    init_idt_اندراج(idt, 0x98, int_98);
    init_idt_اندراج(idt, 0x99, int_99);
    init_idt_اندراج(idt, 0x9a, int_9a);
    init_idt_اندراج(idt, 0x9b, int_9b);
    init_idt_اندراج(idt, 0x9c, int_9c);
    init_idt_اندراج(idt, 0x9d, int_9d);
    init_idt_اندراج(idt, 0x9e, int_9e);
    init_idt_اندراج(idt, 0x9f, int_9f);

    init_idt_اندراج(idt, 0xa0, int_a0);
    init_idt_اندراج(idt, 0xa1, int_a1);
    init_idt_اندراج(idt, 0xa2, int_a2);
    init_idt_اندراج(idt, 0xa3, int_a3);
    init_idt_اندراج(idt, 0xa4, int_a4);
    init_idt_اندراج(idt, 0xa5, int_a5);
    init_idt_اندراج(idt, 0xa6, int_a6);
    init_idt_اندراج(idt, 0xa7, int_a7);
    init_idt_اندراج(idt, 0xa8, int_a8);
    init_idt_اندراج(idt, 0xa9, int_a9);
    init_idt_اندراج(idt, 0xaa, int_aa);
    init_idt_اندراج(idt, 0xab, int_ab);
    init_idt_اندراج(idt, 0xac, int_ac);
    init_idt_اندراج(idt, 0xad, int_ad);
    init_idt_اندراج(idt, 0xae, int_ae);
    init_idt_اندراج(idt, 0xaf, int_af);

    init_idt_اندراج(idt, 0xb0, int_b0);
    init_idt_اندراج(idt, 0xb1, int_b1);
    init_idt_اندراج(idt, 0xb2, int_b2);
    init_idt_اندراج(idt, 0xb3, int_b3);
    init_idt_اندراج(idt, 0xb4, int_b4);
    init_idt_اندراج(idt, 0xb5, int_b5);
    init_idt_اندراج(idt, 0xb6, int_b6);
    init_idt_اندراج(idt, 0xb7, int_b7);
    init_idt_اندراج(idt, 0xb8, int_b8);
    init_idt_اندراج(idt, 0xb9, int_b9);
    init_idt_اندراج(idt, 0xba, int_ba);
    init_idt_اندراج(idt, 0xbb, int_bb);
    init_idt_اندراج(idt, 0xbc, int_bc);
    init_idt_اندراج(idt, 0xbd, int_bd);
    init_idt_اندراج(idt, 0xbe, int_be);
    init_idt_اندراج(idt, 0xbf, int_bf);

    init_idt_اندراج(idt, 0xc0, int_c0);
    init_idt_اندراج(idt, 0xc1, int_c1);
    init_idt_اندراج(idt, 0xc2, int_c2);
    init_idt_اندراج(idt, 0xc3, int_c3);
    init_idt_اندراج(idt, 0xc4, int_c4);
    init_idt_اندراج(idt, 0xc5, int_c5);
    init_idt_اندراج(idt, 0xc6, int_c6);
    init_idt_اندراج(idt, 0xc7, int_c7);
    init_idt_اندراج(idt, 0xc8, int_c8);
    init_idt_اندراج(idt, 0xc9, int_c9);
    init_idt_اندراج(idt, 0xca, int_ca);
    init_idt_اندراج(idt, 0xcb, int_cb);
    init_idt_اندراج(idt, 0xcc, int_cc);
    init_idt_اندراج(idt, 0xcd, int_cd);
    init_idt_اندراج(idt, 0xce, int_ce);
    init_idt_اندراج(idt, 0xcf, int_cf);

    init_idt_اندراج(idt, 0xd0, int_d0);
    init_idt_اندراج(idt, 0xd1, int_d1);
    init_idt_اندراج(idt, 0xd2, int_d2);
    init_idt_اندراج(idt, 0xd3, int_d3);
    init_idt_اندراج(idt, 0xd4, int_d4);
    init_idt_اندراج(idt, 0xd5, int_d5);
    init_idt_اندراج(idt, 0xd6, int_d6);
    init_idt_اندراج(idt, 0xd7, int_d7);
    init_idt_اندراج(idt, 0xd8, int_d8);
    init_idt_اندراج(idt, 0xd9, int_d9);
    init_idt_اندراج(idt, 0xda, int_da);
    init_idt_اندراج(idt, 0xdb, int_db);
    init_idt_اندراج(idt, 0xdc, int_dc);
    init_idt_اندراج(idt, 0xdd, int_dd);
    init_idt_اندراج(idt, 0xde, int_de);
    init_idt_اندراج(idt, 0xdf, int_df);

    init_idt_اندراج(idt, 0xe0, int_e0);
    init_idt_اندراج(idt, 0xe1, int_e1);
    init_idt_اندراج(idt, 0xe2, int_e2);
    init_idt_اندراج(idt, 0xe3, int_e3);
    init_idt_اندراج(idt, 0xe4, int_e4);
    init_idt_اندراج(idt, 0xe5, int_e5);
    init_idt_اندراج(idt, 0xe6, int_e6);
    init_idt_اندراج(idt, 0xe7, int_e7);
    init_idt_اندراج(idt, 0xe8, int_e8);
    init_idt_اندراج(idt, 0xe9, int_e9);
    init_idt_اندراج(idt, 0xea, int_ea);
    init_idt_اندراج(idt, 0xeb, int_eb);
    init_idt_اندراج(idt, 0xec, int_ec);
    init_idt_اندراج(idt, 0xed, int_ed);
    init_idt_اندراج(idt, 0xee, int_ee);
    init_idt_اندراج(idt, 0xef, int_ef);

    init_idt_اندراج(idt, 0xf0, int_f0);
    init_idt_اندراج(idt, 0xf1, int_f1);
    init_idt_اندراج(idt, 0xf2, int_f2);
    init_idt_اندراج(idt, 0xf3, int_f3);
    init_idt_اندراج(idt, 0xf4, int_f4);
    init_idt_اندراج(idt, 0xf5, int_f5);
    init_idt_اندراج(idt, 0xf6, int_f6);
    init_idt_اندراج(idt, 0xf7, int_f7);
    init_idt_اندراج(idt, 0xf8, int_f8);
    init_idt_اندراج(idt, 0xf9, int_f9);
    init_idt_اندراج(idt, 0xfa, int_fa);
    init_idt_اندراج(idt, 0xfb, int_fb);
    init_idt_اندراج(idt, 0xfc, int_fc);
    init_idt_اندراج(idt, 0xfd, int_fd);
    init_idt_اندراج(idt, 0xfe, int_fe);
    init_idt_اندراج(idt, 0xff, int_ff);
}

BOOT_کوڈ bool_t init_vm_state(فراغ)
{
    لفظ_t cache_سطر_ناپ;
    x86_KScache_سطر_ناپ_بٹ = get_Cache_سطر_ناپ_بٹ();
    إذا (!x86_KScache_سطر_ناپ_بٹ) {
        أرجع false;
    }

    cache_سطر_ناپ = ثنائی_ہندسہ(x86_KScache_سطر_ناپ_بٹ);
    إذا (cache_سطر_ناپ != L1_CACHE_سطر_ناپ) {
        لکھو("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_سطر_ناپ, cache_سطر_ناپ);
        SMP_COND_STATEMENT(أرجع false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    فراغ *tss_ptr = &x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_اشاریہ()].x86KSidt);
    أرجع true;
}

BOOT_کوڈ bool_t init_pat_msr(فراغ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    إذا ((x86_cpuid_edx(0x1, 0x0) & ثنائی_ہندسہ(16)) == 0) {
        لکھو("PAT support not found\n");
        أرجع false;
    }
    pat_msr.الفاظ[0] = x86_rdmsr_کم(IA32_PAT_MSR);
    pat_msr.الفاظ[1] = x86_rdmsr_بلند(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_سیٹ_pa0(pat_msr, IA32_PAT_MT_WRITE_پٹھ);
    pat_msr = x86_pat_msr_سیٹ_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_سیٹ_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_سیٹ_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_سیٹ_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.الفاظ[1]) << 32 | pat_msr.الفاظ[0]);
    أرجع true;
}

BOOT_کوڈ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_دا_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_دا_cap(it_vspace_cap));
    x86KSASIDTable[ASID_بلند(IT_ASID)] = ap;
}

asid_map_t لبھو_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بلند(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_کم(asid)];
}

لبھو_VSpace_For_ASID_ret_t لبھو_VSpace_For_ASID(asid_t asid)
{
    لبھو_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = لبھو_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_جڑ_new();

        ret.vspace_جڑ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_جڑ = (vspace_جڑ_t *)asid_map_asid_map_vspace_get_vspace_جڑ(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    لفظ_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_رجسٹر(thread, Error);

    بدل (vm_faultType) {
    حالة X86_انکڑے_Fault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        أرجع EXCEPTION_FAULT;

    حالة X86InstructionFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, true);
        أرجع EXCEPTION_FAULT;

    افتراضي:
        fail("Invalid VM fault type");
    }
}

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMReadOnly:
        أرجع 0;

    حالة VMKernelOnly:
    حالة VMReadWrite:
        أرجع 1;

    افتراضي:
        fail("Invalid VM rights");
    }
}

uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights)
{
    بدل (vm_rights) {
    حالة VMKernelOnly:
        أرجع 0;

    حالة VMReadOnly:
    حالة VMReadWrite:
        أرجع 1;

    افتراضي:
        fail("Invalid VM rights");
    }
}

lookupPTSlot_ret_t lookupPTSlot(vspace_جڑ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_تھاں;

    pd_تھاں = lookupPDSlot(vspace, vptr);
    إذا (pd_تھاں.حالت != EXCEPTION_NONE) {
        ret.pt_تھاں = عدم;
        ret.حالت = pd_تھاں.حالت;
        أرجع ret;
    }
    إذا ((pde_ptr_get_ورقہ_ناپ(pd_تھاں.pd_تھاں) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_تھاں.pd_تھاں)) {
        current_lookup_fault = lookup_fault_missing_capability_new(ورقہ_بٹ + PT_اشاریہ_بٹ);
        ret.pt_تھاں = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt;
        pte_t *pt_تھاں;
        لفظ_t pt_اشاریہ;

        pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_ادھار_پتہ(pd_تھاں.pd_تھاں));
        pt_اشاریہ = (vptr >> ورقہ_بٹ) & نقاب(PT_اشاریہ_بٹ);
        pt_تھاں = pt + pt_اشاریہ;

        ret.pt_تھاں = pt_تھاں;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکھٹا_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_چوکھٹا_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ناپ_بٹ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

vm_rights_t CONST نقاب_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_نقاب)
{
    إذا (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_نقاب)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_نقاب) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_نقاب)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
}

فراغ flush_جدول(vspace_جڑ_t *vspace, لفظ_t vptr, pte_t *pt, asid_t asid)
{
    لفظ_t i;
    cap_t        thread_جڑ;

    أكد(IS_ALIGNED(vptr, PT_اشاریہ_بٹ + ورقہ_بٹ));

    /* check if page table belongs to current address space */
    thread_جڑ = TCB_PTR_CTE_PTR(گرہ_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    لكل (i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        إذا (pte_get_present(pt[i])) {
            إذا (config_سیٹ(CONFIG_SUPPORT_PCID) || (is_Valid_Native_جڑ(thread_جڑ)
                                                    && (vspace_جڑ_t *)pptr_دا_cap(thread_جڑ) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << ورقہ_بٹ), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


فراغ unmap_ورقہ(vm_ورقہ_ناپ_t ورقہ_ناپ, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (ورقہ_ناپ) {
    حالة X86_Small_ورقہ:
        lu_ret = lookupPTSlot(لبھو_ret.vspace_جڑ, vptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!(pte_ptr_get_present(lu_ret.pt_تھاں)
              && (pte_ptr_get_ورقہ_ادھار_پتہ(lu_ret.pt_تھاں)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *lu_ret.pt_تھاں = makeUserPTEInvalid();
        اكسر;

    حالة X86_Large_ورقہ:
        pd_ret = lookupPDSlot(لبھو_ret.vspace_جڑ, vptr);
        إذا (pd_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        pde = pd_ret.pd_تھاں;
        إذا (!(pde_ptr_get_ورقہ_ناپ(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_ورقہ_ادھار_پتہ(pde)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *pde = makeUserPDEInvalid();
        اكسر;

    افتراضي:
        إذا (!منوال_Unmap_ورقہ(ورقہ_ناپ, لبھو_ret.vspace_جڑ, vptr, pptr)) {
            أرجع;
        }
        اكسر;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(لبھو_ret.vspace_جڑ), 0));
}

فراغ unmap_ورقہ_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    لبھو_VSpace_For_ASID_ret_t لبھو_ret;
    lookupPDSlot_ret_t    lu_ret;

    لبھو_ret = لبھو_VSpace_For_ASID(asid);
    إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDSlot(لبھو_ret.vspace_جڑ, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PD actually refers to the PT */
    إذا (!(pde_ptr_get_ورقہ_ناپ(lu_ret.pd_تھاں) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_تھاں) &&
          (pde_pde_pt_ptr_get_pt_ادھار_پتہ(lu_ret.pd_تھاں) == pptr_منزل_paddr(pt)))) {
        أرجع;
    }

    flush_جدول(لبھو_ret.vspace_جڑ, vaddr, pt, asid);

    *lu_ret.pd_تھاں = makeUserPDEInvalid();

    invalidate_ورقہ_Structure_Cache_ASID(pptr_منزل_paddr(لبھو_ret.vspace_جڑ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(لبھو_ret.vspace_جڑ), 0));
}

ساكن exception_t perform_X86_ورقہ_Invocation_Map_PTE(cap_t cap, cte_t *ct_تھاں, pte_t *pt_تھاں, pte_t pte,
                                                  vspace_جڑ_t *vspace)
{
    ct_تھاں->cap = cap;
    *pt_تھاں = pte;
    invalidate_ورقہ_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_ورقہ_Invocation_Map_PDE(cap_t cap, cte_t *ct_تھاں, pde_t *pd_تھاں, pde_t pde,
                                                  vspace_جڑ_t *vspace)
{
    ct_تھاں->cap = cap;
    *pd_تھاں = pde;
    invalidate_ورقہ_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_X86_ورقہ_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{
    أكد(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));
    أكد(cap_چوکھٹا_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap)) {
        unmap_ورقہ(
            cap_چوکھٹا_cap_get_cap_FSize(cap),
            cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap),
            cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap),
            (فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_پتہ(&ct_تھاں->cap, 0);
    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMapped_ASID(&ct_تھاں->cap, asidInvalid);
    cap_چوکھٹا_cap_ptr_سیٹ_cap_FMap_Type(&ct_تھاں->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_چوکھٹا_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        بدل (cap_چوکھٹا_cap_get_cap_FMap_Type(cap)) {
        حالة X86_MappingVSpace:
            أرجع perform_X86_ورقہ_Invocation_Unmap(cap, cte);
#جے_تعریف_شدہ CONFIG_IOMMU
        حالة X86_MappingIOSpace:
            أرجع performX86IOUnMapInvocation(cap, cte);
#جے_انت
#جے_تعریف_شدہ CONFIG_VTX
        حالة X86_MappingEPT:
            أرجع performX86EPTPageInvocationUnmap(cap, cte);
#جے_انت
        حالة X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            اكسر;
        }
    }

    أرجع EXCEPTION_NONE;
}

بنية create_mapping_pte_return {
    exception_t حالت;
    pte_t pte;
    pte_t *pt_تھاں;
};
تعريفنوع بنية create_mapping_pte_return create_mapping_pte_return_t;

ساكن create_mapping_pte_return_t create_محفوظ_Mapping_Entries_PTE(paddr_t ادھار, لفظ_t vaddr, vm_rights_t vmRights,
                                                                vm_گن_t attr,
                                                                vspace_جڑ_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع ret;
    }

    ret.pte = makeUserPTE(ادھار, attr, vmRights);
    ret.pt_تھاں = lu_ret.pt_تھاں;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

بنية create_mapping_pde_return {
    exception_t حالت;
    pde_t pde;
    pde_t *pd_تھاں;
};
تعريفنوع بنية create_mapping_pde_return create_mapping_pde_return_t;

ساكن create_mapping_pde_return_t create_محفوظ_Mapping_Entries_PDE(paddr_t ادھار, لفظ_t vaddr, vm_rights_t vmRights,
                                                                vm_گن_t attr,
                                                                vspace_جڑ_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pd_تھاں = lu_ret.pd_تھاں;

    /* check for existing page table */
    إذا ((pde_ptr_get_ورقہ_ناپ(ret.pd_تھاں) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_تھاں))) {
        current_syscall_error.type = se_L4_Delete_پہلا;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }


    ret.pde = make_User_PDELarge_ورقہ(ادھار, attr, vmRights);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}


exception_t decode_X86_چوکھٹا_Invocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (invLabel) {
    حالة X86_ورقہ_Map: { /* Map */
        لفظ_t          vaddr;
        لفظ_t          vtop;
        لفظ_t          w_rights_نقاب;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_جڑ_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_گن_t vmAttr;
        vm_ورقہ_ناپ_t  چوکھٹا_ناپ;
        asid_t          asid;

        إذا (لمائی < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        چوکھٹا_ناپ = cap_چوکھٹا_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, بفر);
        w_rights_نقاب = getSyscallArg(1, بفر);
        vmAttr = vm_گن_From_لفظ(getSyscallArg(2, بفر));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_چوکھٹا_cap_get_cap_FVMRights(cap);

        إذا (!is_Valid_Native_جڑ(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_جڑ_t *)pptr_دا_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            إذا (cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکھٹا_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + ثنائی_ہندسہ(ورقہ_بٹ_For_ناپ(چوکھٹا_ناپ));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            إذا (vaddr > USER_چوٹی || vtop > USER_چوٹی) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_نمبر = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            لبھو_VSpace_For_ASID_ret_t لبھو_ret;

            لبھو_ret = لبھو_VSpace_For_ASID(asid);
            إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_سروت = false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (لبھو_ret.vspace_جڑ != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_نمبر = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

        إذا (!checkVPAlignment(چوکھٹا_ناپ, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_منزل_paddr((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap));

        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMapped_پتہ(cap, vaddr);
        cap = cap_چوکھٹا_cap_سیٹ_cap_FMap_Type(cap, X86_MappingVSpace);

        بدل (چوکھٹا_ناپ) {
        /* PTE mappings */
        حالة X86_Small_ورقہ: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_محفوظ_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_ورقہ_Invocation_Map_PTE(cap, cte, map_ret.pt_تھاں, map_ret.pte, vspace);
        }

        /* PDE mappings */
        حالة X86_Large_ورقہ: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_محفوظ_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_ورقہ_Invocation_Map_PDE(cap, cte, map_ret.pd_تھاں, map_ret.pde, vspace);
        }

        افتراضي: {
            أرجع decode_X86_منوال_Map_ورقہ(invLabel, چوکھٹا_ناپ, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    حالة X86_ورقہ_Unmap: { /* Unmap */
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_چوکھٹا_Invocation_Unmap(cap, cte);
    }

#جے_تعریف_شدہ CONFIG_IOMMU
    حالة X86_ورقہ_Map_IO: { /* MapIO */
        أرجع decodeX86IOMapInvocation(لمائی, cte, cap, بفر);
    }
#جے_انت

#جے_تعریف_شدہ CONFIG_VTX
    حالة X86_ورقہ_Map_EPT:
        أرجع decodeX86EPTPageMap(invLabel, لمائی, cte, cap, بفر);
#جے_انت

    حالة X86_ورقہ_Get_پتہ: {
        /* Return it in the first message register. */
        أكد(n_پیغام_Registers >= 1);

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_ورقہ_Get_پتہ((فراغ *)cap_چوکھٹا_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

ساكن exception_t perform_X86_ورقہ_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_تھاں)
{

    إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_ورقہ_جدول(
            cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap),
            pt
        );
        clear_حافظہ((فراغ *)pt, cap_get_cap_ناپ_بٹ(cap));
    }
    cap_ورقہ_جدول_cap_ptr_سیٹ_cap_PTIs_Mapped(&(ct_تھاں->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_ورقہ_جدول_Invocation_Map(cap_t cap, cte_t *ct_تھاں, pde_t pde, pde_t *pd_تھاں,
                                                    vspace_جڑ_t *جڑ)
{
    ct_تھاں->cap = cap;
    *pd_تھاں = pde;
    invalidate_ورقہ_Structure_Cache_ASID(pptr_منزل_paddr(جڑ), cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(جڑ), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_X86_ورقہ_جدول_Invocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cte_t *cte, cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t          vaddr;
    vm_گن_t attr;
    lookupPDSlot_ret_t pd_تھاں;
    cap_t           vspaceCap;
    vspace_جڑ_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    إذا (invLabel == X86_ورقہ_جدول_Unmap) {
        إذا (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پہلا;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_ورقہ_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (invLabel != X86_ورقہ_جدول_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (لمائی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_سنیہا;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_ورقہ_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PT_اشاریہ_بٹ + ورقہ_بٹ));
    attr = vm_گن_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_جڑ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_نمبر = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_جڑ_t *)pptr_دا_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_چوٹی) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_نمبر = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        لبھو_VSpace_For_ASID_ret_t لبھو_ret;

        لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (لبھو_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (لبھو_ret.vspace_جڑ != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_تھاں = lookupPDSlot(vspace, vaddr);
    إذا (pd_تھاں.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سروت = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((pde_ptr_get_ورقہ_ناپ(pd_تھاں.pd_تھاں) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_تھاں.pd_تھاں)) ||
        ((pde_ptr_get_ورقہ_ناپ(pd_تھاں.pd_تھاں) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_تھاں.pd_تھاں))) {
        current_syscall_error.type = se_L4_Delete_پہلا;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PTE_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_جدول(paddr, attr);

    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ورقہ_جدول_cap_سیٹ_cap_PTMapped_پتہ(cap, vaddr);

    سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X86_ورقہ_جدول_Invocation_Map(cap, cte, pde, pd_تھاں.pd_تھاں, vspace);
}

exception_t decodeX86MMUInvocation(
    لفظ_t invLabel,
    لفظ_t لمائی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_چوکھٹا_cap:
        أرجع decode_X86_چوکھٹا_Invocation(invLabel, لمائی, cte, cap, call, بفر);

    حالة cap_ورقہ_جدول_cap:
        أرجع decode_X86_ورقہ_جدول_Invocation(invLabel, لمائی, cte, cap, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t     i;
        asid_t           asid_ادھار;
        لفظ_t           اشاریہ;
        لفظ_t           ڈونگھائی;
        cap_t            untyped;
        cap_t            جڑ;
        cte_t           *ماپا_تھاں;
        cte_t           *dest_تھاں;
        lookup_تھاں_ret_t lu_ret;
        فراغ            *چوکھٹا;
        exception_t      حالت;

        إذا (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (لمائی < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاریہ = getSyscallArg(0, بفر);
        ڈونگھائی = getSyscallArg(1, بفر);
        ماپا_تھاں = current_extra_caps.excaprefs[0];
        untyped = ماپا_تھاں->cap;
        جڑ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_ادھار = i << asid_کم_بٹ;


        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاک_ناپ(untyped) != se_L4_ASIDPool_بٹ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(ماپا_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکھٹا = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_نشانہ_تھاں(جڑ, اشاریہ, ڈونگھائی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_تھاں = lu_ret.تھاں;

        حالت = ensure_خالی_تھاواں_تھاں(dest_تھاں);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکھٹا, dest_تھاں, ماپا_تھاں, asid_ادھار);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_تھاں;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t       asid;

        إذا (invLabel != X86_ASIDPool_سونپو) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_سنیہا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_تھاں = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_تھاں->cap;

        إذا (!(is_VTable_جڑ(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_بلند(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سروت = false;
            current_lookup_fault = lookup_fault_invalid_جڑ_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_نمبر = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < ثنائی_ہندسہ(asid_کم_بٹ) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        إذا (i == ثنائی_ہندسہ(asid_کم_بٹ)) {
            current_syscall_error.type = se_L4_Delete_پہلا;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سیٹ_Thread_State(گرہ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_تھاں);
    }

    افتراضي:
        أرجع decode_X86_منوال_MMUInvocation(invLabel, لمائی, cptr, cte, cap, call, بفر);
    }
}
