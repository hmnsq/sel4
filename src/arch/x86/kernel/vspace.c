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

ساكن exception_t perform_صفحو_Get_پتو(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ڳنڍ_STATE(ksCurThread);
    إذا (call) {
        لفظ_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سيٽ_رجسٽر(thread, badge_رجسٽر, 0);
        بنانشان صحيح ڊيگهائي = سيٽ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سيٽ_رجسٽر(thread, نياپو_Info_رجسٽر, لفظ_From_پيغام_Info(
                        se_L4_پيغام_Info_new(0, 0, 0, ڊيگهائي)));
    }
    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

فراغ deleteASIDPool(asid_t asid_بنياد, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_بنياد, asid_هيٺيون_بٽ));

    إذا (x86KSASIDTable[ASID_مٿاهون(asid_بنياد)] == pool) {
        لكل (بنانشان صحيح وٿي = 0; وٿي < ٻٽو_انگ(asid_هيٺيون_بٽ); وٿي++) {
            asid_map_t asid_map = pool->array[وٿي];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_پاڙ_t *vspace = (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map);
                hwASIDInvalidate(asid_بنياد + وٿي, vspace);
            }
        }
        x86KSASIDTable[ASID_مٿاهون(asid_بنياد)] = عدم;
        سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(فراغ *چوکٺو, cte_t *جڳهه, cte_t *والد, asid_t asid_بنياد)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سيٽ_cap_Free_اشاريو(&(والد->cap),
                                         MAX_FREE_اشاريو(cap_untyped_cap_get_cap_بلاڪ_سائيز(والد->cap)));

    memzero(چوکٺو, ٻٽو_انگ(صفحو_بٽ_For_سائيز(X86_Small_صفحو)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_وجهو(
        cap_asid_pool_cap_new(
            asid_بنياد,          /* capASIDBase  */
            لفظ_REF(چوکٺو)     /* capASIDPool  */
        ),
        والد,
        جڳهه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنياد & نقاب(asid_هيٺيون_بٽ)) == 0);
    x86KSASIDTable[ASID_مٿاهون(asid_بنياد)] = (asid_pool_t *)چوکٺو;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, vspace_پاڙ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_مٿاهون(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_هيٺيون(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_هيٺيون(asid)] = asid_map_asid_map_none_new();
            سيٽ_VMRoot(ڳنڍ_STATE(ksCurThread));
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

    إذا (cap_get_capType(بفر_Cap) != cap_چوکٺو_cap) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکٺو_cap_get_cap_FVMRights(بفر_Cap);
    إذا (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        لفظ_t بنياد_Ptr, صفحو_بٽ_2;

        بنياد_Ptr = cap_چوکٺو_cap_get_cap_FBase_Ptr(بفر_Cap);
        صفحو_بٽ_2 = صفحو_بٽ_For_سائيز(cap_چوکٺو_cap_get_cap_FSize(بفر_Cap));
        أرجع (لفظ_t *)(بنياد_Ptr + (w_بفر_Ptr & نقاب(صفحو_بٽ_2)));
    } وإلا {
        أرجع عدم;
    }
}

bool_t CONST is_Valid_VTable_پاڙ(cap_t cap)
{
    أرجع is_Valid_Native_پاڙ(cap);
}


BOOT_ڪوڊ bool_t map_kernel_دري_devices(pte_t *pt, uint32_t انگ_ioapic, paddr_t *ioapic_paddrs, uint32_t انگ_drhu,
                                           paddr_t *drhu_list)
{
    لفظ_t idx = (KDEV_بنياد & نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ;
    paddr_t phys;
    pte_t pte;
    بنانشان صحيح i;
    /* map kernel devices: APIC */
    phys = apic_get_بنياد_paddr();
    إذا (!phys) {
        أرجع false;
    }
    إذا (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        أرجع false;
    }
    pte = x86_make_device_pte(phys);

    أكد(idx == (PPTR_APIC & نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ);
    pt[idx] = pte;
    idx++;
    لكل (i = 0; i < انگ_ioapic; i++) {
        phys = ioapic_paddrs[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);
        أكد(idx == ((PPTR_IOAPIC_شروعات + i * ٻٽو_انگ(صفحو_بٽ)) & نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ);
        pt[idx] = pte;
        idx++;
        إذا (idx == ٻٽو_انگ(PT_اشاريو_بٽ)) {
            أرجع false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    لكل (; i < CONFIG_MAX_انگ_IOAPIC; i++) {
        pte = x86_make_خالي_جايون_pte();
        أكد(idx == ((PPTR_IOAPIC_شروعات + i * ٻٽو_انگ(صفحو_بٽ)) & نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    لكل (i = 0; i < انگ_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);

        أكد(idx == ((PPTR_DRHU_شروعات + i * ٻٽو_انگ(صفحو_بٽ)) & نقاب(LARGE_صفحو_بٽ)) >> صفحو_بٽ);
        pt[idx] = pte;
        idx++;
        إذا (idx == ٻٽو_انگ(PT_اشاريو_بٽ)) {
            أرجع false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    طالما (idx < ٻٽو_انگ(PT_اشاريو_بٽ)) {
        pte = x86_make_خالي_جايون_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    أكد(idx == ٻٽو_انگ(PT_اشاريو_بٽ));
    أرجع true;
}

BOOT_ڪوڊ ساكن فراغ init_idt(idt_داخلا_t *idt)
{
    init_idt_داخلا(idt, 0x00, int_00);
    init_idt_داخلا(idt, 0x01, int_01);
    init_idt_داخلا(idt, 0x02, int_02);
    init_idt_داخلا(idt, 0x03, int_03);
    init_idt_داخلا(idt, 0x04, int_04);
    init_idt_داخلا(idt, 0x05, int_05);
    init_idt_داخلا(idt, 0x06, int_06);
    init_idt_داخلا(idt, 0x07, int_07);
    init_idt_داخلا(idt, 0x08, int_08);
    init_idt_داخلا(idt, 0x09, int_09);
    init_idt_داخلا(idt, 0x0a, int_0a);
    init_idt_داخلا(idt, 0x0b, int_0b);
    init_idt_داخلا(idt, 0x0c, int_0c);
    init_idt_داخلا(idt, 0x0d, int_0d);
    init_idt_داخلا(idt, 0x0e, int_0e);
    init_idt_داخلا(idt, 0x0f, int_0f);

    init_idt_داخلا(idt, 0x10, int_10);
    init_idt_داخلا(idt, 0x11, int_11);
    init_idt_داخلا(idt, 0x12, int_12);
    init_idt_داخلا(idt, 0x13, int_13);
    init_idt_داخلا(idt, 0x14, int_14);
    init_idt_داخلا(idt, 0x15, int_15);
    init_idt_داخلا(idt, 0x16, int_16);
    init_idt_داخلا(idt, 0x17, int_17);
    init_idt_داخلا(idt, 0x18, int_18);
    init_idt_داخلا(idt, 0x19, int_19);
    init_idt_داخلا(idt, 0x1a, int_1a);
    init_idt_داخلا(idt, 0x1b, int_1b);
    init_idt_داخلا(idt, 0x1c, int_1c);
    init_idt_داخلا(idt, 0x1d, int_1d);
    init_idt_داخلا(idt, 0x1e, int_1e);
    init_idt_داخلا(idt, 0x1f, int_1f);

    init_idt_داخلا(idt, 0x20, int_20);
    init_idt_داخلا(idt, 0x21, int_21);
    init_idt_داخلا(idt, 0x22, int_22);
    init_idt_داخلا(idt, 0x23, int_23);
    init_idt_داخلا(idt, 0x24, int_24);
    init_idt_داخلا(idt, 0x25, int_25);
    init_idt_داخلا(idt, 0x26, int_26);
    init_idt_داخلا(idt, 0x27, int_27);
    init_idt_داخلا(idt, 0x28, int_28);
    init_idt_داخلا(idt, 0x29, int_29);
    init_idt_داخلا(idt, 0x2a, int_2a);
    init_idt_داخلا(idt, 0x2b, int_2b);
    init_idt_داخلا(idt, 0x2c, int_2c);
    init_idt_داخلا(idt, 0x2d, int_2d);
    init_idt_داخلا(idt, 0x2e, int_2e);
    init_idt_داخلا(idt, 0x2f, int_2f);

    init_idt_داخلا(idt, 0x30, int_30);
    init_idt_داخلا(idt, 0x31, int_31);
    init_idt_داخلا(idt, 0x32, int_32);
    init_idt_داخلا(idt, 0x33, int_33);
    init_idt_داخلا(idt, 0x34, int_34);
    init_idt_داخلا(idt, 0x35, int_35);
    init_idt_داخلا(idt, 0x36, int_36);
    init_idt_داخلا(idt, 0x37, int_37);
    init_idt_داخلا(idt, 0x38, int_38);
    init_idt_داخلا(idt, 0x39, int_39);
    init_idt_داخلا(idt, 0x3a, int_3a);
    init_idt_داخلا(idt, 0x3b, int_3b);
    init_idt_داخلا(idt, 0x3c, int_3c);
    init_idt_داخلا(idt, 0x3d, int_3d);
    init_idt_داخلا(idt, 0x3e, int_3e);
    init_idt_داخلا(idt, 0x3f, int_3f);

    init_idt_داخلا(idt, 0x40, int_40);
    init_idt_داخلا(idt, 0x41, int_41);
    init_idt_داخلا(idt, 0x42, int_42);
    init_idt_داخلا(idt, 0x43, int_43);
    init_idt_داخلا(idt, 0x44, int_44);
    init_idt_داخلا(idt, 0x45, int_45);
    init_idt_داخلا(idt, 0x46, int_46);
    init_idt_داخلا(idt, 0x47, int_47);
    init_idt_داخلا(idt, 0x48, int_48);
    init_idt_داخلا(idt, 0x49, int_49);
    init_idt_داخلا(idt, 0x4a, int_4a);
    init_idt_داخلا(idt, 0x4b, int_4b);
    init_idt_داخلا(idt, 0x4c, int_4c);
    init_idt_داخلا(idt, 0x4d, int_4d);
    init_idt_داخلا(idt, 0x4e, int_4e);
    init_idt_داخلا(idt, 0x4f, int_4f);

    init_idt_داخلا(idt, 0x50, int_50);
    init_idt_داخلا(idt, 0x51, int_51);
    init_idt_داخلا(idt, 0x52, int_52);
    init_idt_داخلا(idt, 0x53, int_53);
    init_idt_داخلا(idt, 0x54, int_54);
    init_idt_داخلا(idt, 0x55, int_55);
    init_idt_داخلا(idt, 0x56, int_56);
    init_idt_داخلا(idt, 0x57, int_57);
    init_idt_داخلا(idt, 0x58, int_58);
    init_idt_داخلا(idt, 0x59, int_59);
    init_idt_داخلا(idt, 0x5a, int_5a);
    init_idt_داخلا(idt, 0x5b, int_5b);
    init_idt_داخلا(idt, 0x5c, int_5c);
    init_idt_داخلا(idt, 0x5d, int_5d);
    init_idt_داخلا(idt, 0x5e, int_5e);
    init_idt_داخلا(idt, 0x5f, int_5f);

    init_idt_داخلا(idt, 0x60, int_60);
    init_idt_داخلا(idt, 0x61, int_61);
    init_idt_داخلا(idt, 0x62, int_62);
    init_idt_داخلا(idt, 0x63, int_63);
    init_idt_داخلا(idt, 0x64, int_64);
    init_idt_داخلا(idt, 0x65, int_65);
    init_idt_داخلا(idt, 0x66, int_66);
    init_idt_داخلا(idt, 0x67, int_67);
    init_idt_داخلا(idt, 0x68, int_68);
    init_idt_داخلا(idt, 0x69, int_69);
    init_idt_داخلا(idt, 0x6a, int_6a);
    init_idt_داخلا(idt, 0x6b, int_6b);
    init_idt_داخلا(idt, 0x6c, int_6c);
    init_idt_داخلا(idt, 0x6d, int_6d);
    init_idt_داخلا(idt, 0x6e, int_6e);
    init_idt_داخلا(idt, 0x6f, int_6f);

    init_idt_داخلا(idt, 0x70, int_70);
    init_idt_داخلا(idt, 0x71, int_71);
    init_idt_داخلا(idt, 0x72, int_72);
    init_idt_داخلا(idt, 0x73, int_73);
    init_idt_داخلا(idt, 0x74, int_74);
    init_idt_داخلا(idt, 0x75, int_75);
    init_idt_داخلا(idt, 0x76, int_76);
    init_idt_داخلا(idt, 0x77, int_77);
    init_idt_داخلا(idt, 0x78, int_78);
    init_idt_داخلا(idt, 0x79, int_79);
    init_idt_داخلا(idt, 0x7a, int_7a);
    init_idt_داخلا(idt, 0x7b, int_7b);
    init_idt_داخلا(idt, 0x7c, int_7c);
    init_idt_داخلا(idt, 0x7d, int_7d);
    init_idt_داخلا(idt, 0x7e, int_7e);
    init_idt_داخلا(idt, 0x7f, int_7f);

    init_idt_داخلا(idt, 0x80, int_80);
    init_idt_داخلا(idt, 0x81, int_81);
    init_idt_داخلا(idt, 0x82, int_82);
    init_idt_داخلا(idt, 0x83, int_83);
    init_idt_داخلا(idt, 0x84, int_84);
    init_idt_داخلا(idt, 0x85, int_85);
    init_idt_داخلا(idt, 0x86, int_86);
    init_idt_داخلا(idt, 0x87, int_87);
    init_idt_داخلا(idt, 0x88, int_88);
    init_idt_داخلا(idt, 0x89, int_89);
    init_idt_داخلا(idt, 0x8a, int_8a);
    init_idt_داخلا(idt, 0x8b, int_8b);
    init_idt_داخلا(idt, 0x8c, int_8c);
    init_idt_داخلا(idt, 0x8d, int_8d);
    init_idt_داخلا(idt, 0x8e, int_8e);
    init_idt_داخلا(idt, 0x8f, int_8f);

    init_idt_داخلا(idt, 0x90, int_90);
    init_idt_داخلا(idt, 0x91, int_91);
    init_idt_داخلا(idt, 0x92, int_92);
    init_idt_داخلا(idt, 0x93, int_93);
    init_idt_داخلا(idt, 0x94, int_94);
    init_idt_داخلا(idt, 0x95, int_95);
    init_idt_داخلا(idt, 0x96, int_96);
    init_idt_داخلا(idt, 0x97, int_97);
    init_idt_داخلا(idt, 0x98, int_98);
    init_idt_داخلا(idt, 0x99, int_99);
    init_idt_داخلا(idt, 0x9a, int_9a);
    init_idt_داخلا(idt, 0x9b, int_9b);
    init_idt_داخلا(idt, 0x9c, int_9c);
    init_idt_داخلا(idt, 0x9d, int_9d);
    init_idt_داخلا(idt, 0x9e, int_9e);
    init_idt_داخلا(idt, 0x9f, int_9f);

    init_idt_داخلا(idt, 0xa0, int_a0);
    init_idt_داخلا(idt, 0xa1, int_a1);
    init_idt_داخلا(idt, 0xa2, int_a2);
    init_idt_داخلا(idt, 0xa3, int_a3);
    init_idt_داخلا(idt, 0xa4, int_a4);
    init_idt_داخلا(idt, 0xa5, int_a5);
    init_idt_داخلا(idt, 0xa6, int_a6);
    init_idt_داخلا(idt, 0xa7, int_a7);
    init_idt_داخلا(idt, 0xa8, int_a8);
    init_idt_داخلا(idt, 0xa9, int_a9);
    init_idt_داخلا(idt, 0xaa, int_aa);
    init_idt_داخلا(idt, 0xab, int_ab);
    init_idt_داخلا(idt, 0xac, int_ac);
    init_idt_داخلا(idt, 0xad, int_ad);
    init_idt_داخلا(idt, 0xae, int_ae);
    init_idt_داخلا(idt, 0xaf, int_af);

    init_idt_داخلا(idt, 0xb0, int_b0);
    init_idt_داخلا(idt, 0xb1, int_b1);
    init_idt_داخلا(idt, 0xb2, int_b2);
    init_idt_داخلا(idt, 0xb3, int_b3);
    init_idt_داخلا(idt, 0xb4, int_b4);
    init_idt_داخلا(idt, 0xb5, int_b5);
    init_idt_داخلا(idt, 0xb6, int_b6);
    init_idt_داخلا(idt, 0xb7, int_b7);
    init_idt_داخلا(idt, 0xb8, int_b8);
    init_idt_داخلا(idt, 0xb9, int_b9);
    init_idt_داخلا(idt, 0xba, int_ba);
    init_idt_داخلا(idt, 0xbb, int_bb);
    init_idt_داخلا(idt, 0xbc, int_bc);
    init_idt_داخلا(idt, 0xbd, int_bd);
    init_idt_داخلا(idt, 0xbe, int_be);
    init_idt_داخلا(idt, 0xbf, int_bf);

    init_idt_داخلا(idt, 0xc0, int_c0);
    init_idt_داخلا(idt, 0xc1, int_c1);
    init_idt_داخلا(idt, 0xc2, int_c2);
    init_idt_داخلا(idt, 0xc3, int_c3);
    init_idt_داخلا(idt, 0xc4, int_c4);
    init_idt_داخلا(idt, 0xc5, int_c5);
    init_idt_داخلا(idt, 0xc6, int_c6);
    init_idt_داخلا(idt, 0xc7, int_c7);
    init_idt_داخلا(idt, 0xc8, int_c8);
    init_idt_داخلا(idt, 0xc9, int_c9);
    init_idt_داخلا(idt, 0xca, int_ca);
    init_idt_داخلا(idt, 0xcb, int_cb);
    init_idt_داخلا(idt, 0xcc, int_cc);
    init_idt_داخلا(idt, 0xcd, int_cd);
    init_idt_داخلا(idt, 0xce, int_ce);
    init_idt_داخلا(idt, 0xcf, int_cf);

    init_idt_داخلا(idt, 0xd0, int_d0);
    init_idt_داخلا(idt, 0xd1, int_d1);
    init_idt_داخلا(idt, 0xd2, int_d2);
    init_idt_داخلا(idt, 0xd3, int_d3);
    init_idt_داخلا(idt, 0xd4, int_d4);
    init_idt_داخلا(idt, 0xd5, int_d5);
    init_idt_داخلا(idt, 0xd6, int_d6);
    init_idt_داخلا(idt, 0xd7, int_d7);
    init_idt_داخلا(idt, 0xd8, int_d8);
    init_idt_داخلا(idt, 0xd9, int_d9);
    init_idt_داخلا(idt, 0xda, int_da);
    init_idt_داخلا(idt, 0xdb, int_db);
    init_idt_داخلا(idt, 0xdc, int_dc);
    init_idt_داخلا(idt, 0xdd, int_dd);
    init_idt_داخلا(idt, 0xde, int_de);
    init_idt_داخلا(idt, 0xdf, int_df);

    init_idt_داخلا(idt, 0xe0, int_e0);
    init_idt_داخلا(idt, 0xe1, int_e1);
    init_idt_داخلا(idt, 0xe2, int_e2);
    init_idt_داخلا(idt, 0xe3, int_e3);
    init_idt_داخلا(idt, 0xe4, int_e4);
    init_idt_داخلا(idt, 0xe5, int_e5);
    init_idt_داخلا(idt, 0xe6, int_e6);
    init_idt_داخلا(idt, 0xe7, int_e7);
    init_idt_داخلا(idt, 0xe8, int_e8);
    init_idt_داخلا(idt, 0xe9, int_e9);
    init_idt_داخلا(idt, 0xea, int_ea);
    init_idt_داخلا(idt, 0xeb, int_eb);
    init_idt_داخلا(idt, 0xec, int_ec);
    init_idt_داخلا(idt, 0xed, int_ed);
    init_idt_داخلا(idt, 0xee, int_ee);
    init_idt_داخلا(idt, 0xef, int_ef);

    init_idt_داخلا(idt, 0xf0, int_f0);
    init_idt_داخلا(idt, 0xf1, int_f1);
    init_idt_داخلا(idt, 0xf2, int_f2);
    init_idt_داخلا(idt, 0xf3, int_f3);
    init_idt_داخلا(idt, 0xf4, int_f4);
    init_idt_داخلا(idt, 0xf5, int_f5);
    init_idt_داخلا(idt, 0xf6, int_f6);
    init_idt_داخلا(idt, 0xf7, int_f7);
    init_idt_داخلا(idt, 0xf8, int_f8);
    init_idt_داخلا(idt, 0xf9, int_f9);
    init_idt_داخلا(idt, 0xfa, int_fa);
    init_idt_داخلا(idt, 0xfb, int_fb);
    init_idt_داخلا(idt, 0xfc, int_fc);
    init_idt_داخلا(idt, 0xfd, int_fd);
    init_idt_داخلا(idt, 0xfe, int_fe);
    init_idt_داخلا(idt, 0xff, int_ff);
}

BOOT_ڪوڊ bool_t init_vm_state(فراغ)
{
    لفظ_t cache_سٽ_سائيز;
    x86_KScache_سٽ_سائيز_بٽ = get_Cache_سٽ_سائيز_بٽ();
    إذا (!x86_KScache_سٽ_سائيز_بٽ) {
        أرجع false;
    }

    cache_سٽ_سائيز = ٻٽو_انگ(x86_KScache_سٽ_سائيز_بٽ);
    إذا (cache_سٽ_سائيز != L1_CACHE_سٽ_سائيز) {
        لکو("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_سٽ_سائيز, cache_سٽ_سائيز);
        SMP_COND_STATEMENT(أرجع false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    فراغ *tss_ptr = &x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_اشاريو()].x86KSidt);
    أرجع true;
}

BOOT_ڪوڊ bool_t init_pat_msr(فراغ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    إذا ((x86_cpuid_edx(0x1, 0x0) & ٻٽو_انگ(16)) == 0) {
        لکو("PAT support not found\n");
        أرجع false;
    }
    pat_msr.لفظ[0] = x86_rdmsr_هيٺيون(IA32_PAT_MSR);
    pat_msr.لفظ[1] = x86_rdmsr_مٿاهون(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_سيٽ_pa0(pat_msr, IA32_PAT_MT_WRITE_پٺ);
    pat_msr = x86_pat_msr_سيٽ_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_سيٽ_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_سيٽ_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_سيٽ_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.لفظ[1]) << 32 | pat_msr.لفظ[0]);
    أرجع true;
}

BOOT_ڪوڊ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_جو_cap(it_ap_cap));
    ap->array[ASID_هيٺيون(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_جو_cap(it_vspace_cap));
    x86KSASIDTable[ASID_مٿاهون(IT_ASID)] = ap;
}

asid_map_t ڳوليو_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_مٿاهون(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_هيٺيون(asid)];
}

ڳوليو_VSpace_For_ASID_ret_t ڳوليو_VSpace_For_ASID(asid_t asid)
{
    ڳوليو_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ڳوليو_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_پاڙ_new();

        ret.vspace_پاڙ = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_پاڙ = (vspace_پاڙ_t *)asid_map_asid_map_vspace_get_vspace_پاڙ(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    لفظ_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_رجسٽر(thread, Error);

    بدل (vm_faultType) {
    حالة X86_معلومات_Fault:
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

lookupPTSlot_ret_t lookupPTSlot(vspace_پاڙ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_جڳهه;

    pd_جڳهه = lookupPDSlot(vspace, vptr);
    إذا (pd_جڳهه.حالت != EXCEPTION_NONE) {
        ret.pt_جڳهه = عدم;
        ret.حالت = pd_جڳهه.حالت;
        أرجع ret;
    }
    إذا ((pde_ptr_get_صفحو_سائيز(pd_جڳهه.pd_جڳهه) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_جڳهه.pd_جڳهه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(صفحو_بٽ + PT_اشاريو_بٽ);
        ret.pt_جڳهه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt;
        pte_t *pt_جڳهه;
        لفظ_t pt_اشاريو;

        pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنياد_پتو(pd_جڳهه.pd_جڳهه));
        pt_اشاريو = (vptr >> صفحو_بٽ) & نقاب(PT_اشاريو_بٽ);
        pt_جڳهه = pt + pt_اشاريو;

        ret.pt_جڳهه = pt_جڳهه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکٺو_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_چوکٺو_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_سائيز_بٽ)) {
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

فراغ flush_جدول(vspace_پاڙ_t *vspace, لفظ_t vptr, pte_t *pt, asid_t asid)
{
    لفظ_t i;
    cap_t        thread_پاڙ;

    أكد(IS_ALIGNED(vptr, PT_اشاريو_بٽ + صفحو_بٽ));

    /* check if page table belongs to current address space */
    thread_پاڙ = TCB_PTR_CTE_PTR(ڳنڍ_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    لكل (i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        إذا (pte_get_present(pt[i])) {
            إذا (config_سيٽ(CONFIG_SUPPORT_PCID) || (is_Valid_Native_پاڙ(thread_پاڙ)
                                                    && (vspace_پاڙ_t *)pptr_جو_cap(thread_پاڙ) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << صفحو_بٽ), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


فراغ unmap_صفحو(vm_صفحو_سائيز_t صفحو_سائيز, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (صفحو_سائيز) {
    حالة X86_Small_صفحو:
        lu_ret = lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!(pte_ptr_get_present(lu_ret.pt_جڳهه)
              && (pte_ptr_get_صفحو_بنياد_پتو(lu_ret.pt_جڳهه)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *lu_ret.pt_جڳهه = makeUserPTEInvalid();
        اكسر;

    حالة X86_Large_صفحو:
        pd_ret = lookupPDSlot(ڳوليو_ret.vspace_پاڙ, vptr);
        إذا (pd_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        pde = pd_ret.pd_جڳهه;
        إذا (!(pde_ptr_get_صفحو_سائيز(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_صفحو_بنياد_پتو(pde)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *pde = makeUserPDEInvalid();
        اكسر;

    افتراضي:
        إذا (!گهڻائي_Unmap_صفحو(صفحو_سائيز, ڳوليو_ret.vspace_پاڙ, vptr, pptr)) {
            أرجع;
        }
        اكسر;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(ڳوليو_ret.vspace_پاڙ), 0));
}

فراغ unmap_صفحو_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;
    lookupPDSlot_ret_t    lu_ret;

    ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDSlot(ڳوليو_ret.vspace_پاڙ, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PD actually refers to the PT */
    إذا (!(pde_ptr_get_صفحو_سائيز(lu_ret.pd_جڳهه) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_جڳهه) &&
          (pde_pde_pt_ptr_get_pt_بنياد_پتو(lu_ret.pd_جڳهه) == pptr_منزل_paddr(pt)))) {
        أرجع;
    }

    flush_جدول(ڳوليو_ret.vspace_پاڙ, vaddr, pt, asid);

    *lu_ret.pd_جڳهه = makeUserPDEInvalid();

    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(ڳوليو_ret.vspace_پاڙ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ڳوليو_ret.vspace_پاڙ), 0));
}

ساكن exception_t perform_X86_صفحو_Invocation_Map_PTE(cap_t cap, cte_t *ct_جڳهه, pte_t *pt_جڳهه, pte_t pte,
                                                  vspace_پاڙ_t *vspace)
{
    ct_جڳهه->cap = cap;
    *pt_جڳهه = pte;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_صفحو_Invocation_Map_PDE(cap_t cap, cte_t *ct_جڳهه, pde_t *pd_جڳهه, pde_t pde,
                                                  vspace_پاڙ_t *vspace)
{
    ct_جڳهه->cap = cap;
    *pd_جڳهه = pde;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_X86_صفحو_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{
    أكد(cap_چوکٺو_cap_get_cap_FMapped_ASID(cap));
    أكد(cap_چوکٺو_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap)) {
        unmap_صفحو(
            cap_چوکٺو_cap_get_cap_FSize(cap),
            cap_چوکٺو_cap_get_cap_FMapped_ASID(cap),
            cap_چوکٺو_cap_get_cap_FMapped_پتو(cap),
            (فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_چوکٺو_cap_ptr_سيٽ_cap_FMapped_پتو(&ct_جڳهه->cap, 0);
    cap_چوکٺو_cap_ptr_سيٽ_cap_FMapped_ASID(&ct_جڳهه->cap, asidInvalid);
    cap_چوکٺو_cap_ptr_سيٽ_cap_FMap_Type(&ct_جڳهه->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_چوکٺو_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        بدل (cap_چوکٺو_cap_get_cap_FMap_Type(cap)) {
        حالة X86_MappingVSpace:
            أرجع perform_X86_صفحو_Invocation_Unmap(cap, cte);
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
        حالة X86_MappingIOSpace:
            أرجع performX86IOUnMapInvocation(cap, cte);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
        حالة X86_MappingEPT:
            أرجع performX86EPTPageInvocationUnmap(cap, cte);
#جيڪڏهن_پورو
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
    pte_t *pt_جڳهه;
};
تعريفنوع بنية create_mapping_pte_return create_mapping_pte_return_t;

ساكن create_mapping_pte_return_t create_محفوظ_Mapping_Entries_PTE(paddr_t بنياد, لفظ_t vaddr, vm_rights_t vmRights,
                                                                vm_خاصيتون_t attr,
                                                                vspace_پاڙ_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع ret;
    }

    ret.pte = makeUserPTE(بنياد, attr, vmRights);
    ret.pt_جڳهه = lu_ret.pt_جڳهه;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

بنية create_mapping_pde_return {
    exception_t حالت;
    pde_t pde;
    pde_t *pd_جڳهه;
};
تعريفنوع بنية create_mapping_pde_return create_mapping_pde_return_t;

ساكن create_mapping_pde_return_t create_محفوظ_Mapping_Entries_PDE(paddr_t بنياد, لفظ_t vaddr, vm_rights_t vmRights,
                                                                vm_خاصيتون_t attr,
                                                                vspace_پاڙ_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pd_جڳهه = lu_ret.pd_جڳهه;

    /* check for existing page table */
    إذا ((pde_ptr_get_صفحو_سائيز(ret.pd_جڳهه) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_جڳهه))) {
        current_syscall_error.type = se_L4_Delete_پهريون;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }


    ret.pde = make_User_PDELarge_صفحو(بنياد, attr, vmRights);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}


exception_t decode_X86_چوکٺو_Invocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (invLabel) {
    حالة X86_صفحو_Map: { /* Map */
        لفظ_t          vaddr;
        لفظ_t          vtop;
        لفظ_t          w_rights_نقاب;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_پاڙ_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_خاصيتون_t vmAttr;
        vm_صفحو_سائيز_t  چوکٺو_سائيز;
        asid_t          asid;

        إذا (ڊيگهائي < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        چوکٺو_سائيز = cap_چوکٺو_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, بفر);
        w_rights_نقاب = getSyscallArg(1, بفر);
        vmAttr = vm_خاصيتون_From_لفظ(getSyscallArg(2, بفر));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_چوکٺو_cap_get_cap_FVMRights(cap);

        إذا (!is_Valid_Native_پاڙ(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_پاڙ_t *)pptr_جو_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            إذا (cap_چوکٺو_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکٺو_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکٺو_cap_get_cap_FMapped_پتو(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + ٻٽو_انگ(صفحو_بٽ_For_سائيز(چوکٺو_سائيز));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            إذا (vaddr > USER_چوٽي || vtop > USER_چوٽي) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_دليل;
                current_syscall_error.invalid_دليل_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

            ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
            إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ذريعو = false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (ڳوليو_ret.vspace_پاڙ != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = نقاب_VMRights(capVMRights, rights_From_لفظ(w_rights_نقاب));

        إذا (!checkVPAlignment(چوکٺو_سائيز, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_منزل_paddr((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap));

        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکٺو_cap_سيٽ_cap_FMapped_پتو(cap, vaddr);
        cap = cap_چوکٺو_cap_سيٽ_cap_FMap_Type(cap, X86_MappingVSpace);

        بدل (چوکٺو_سائيز) {
        /* PTE mappings */
        حالة X86_Small_صفحو: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_محفوظ_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_صفحو_Invocation_Map_PTE(cap, cte, map_ret.pt_جڳهه, map_ret.pte, vspace);
        }

        /* PDE mappings */
        حالة X86_Large_صفحو: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_محفوظ_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_صفحو_Invocation_Map_PDE(cap, cte, map_ret.pd_جڳهه, map_ret.pde, vspace);
        }

        افتراضي: {
            أرجع decode_X86_گهڻائي_Map_صفحو(invLabel, چوکٺو_سائيز, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    حالة X86_صفحو_Unmap: { /* Unmap */
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_چوکٺو_Invocation_Unmap(cap, cte);
    }

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    حالة X86_صفحو_Map_IO: { /* MapIO */
        أرجع decodeX86IOMapInvocation(ڊيگهائي, cte, cap, بفر);
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    حالة X86_صفحو_Map_EPT:
        أرجع decodeX86EPTPageMap(invLabel, ڊيگهائي, cte, cap, بفر);
#جيڪڏهن_پورو

    حالة X86_صفحو_Get_پتو: {
        /* Return it in the first message register. */
        أكد(n_نياپو_Registers >= 1);

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحو_Get_پتو((فراغ *)cap_چوکٺو_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

ساكن exception_t perform_X86_صفحو_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جڳهه)
{

    إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحو_جدول(
            cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap),
            pt
        );
        clear_يادگيري((فراغ *)pt, cap_get_cap_سائيز_بٽ(cap));
    }
    cap_صفحو_جدول_cap_ptr_سيٽ_cap_PTIs_Mapped(&(ct_جڳهه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_صفحو_جدول_Invocation_Map(cap_t cap, cte_t *ct_جڳهه, pde_t pde, pde_t *pd_جڳهه,
                                                    vspace_پاڙ_t *پاڙ)
{
    ct_جڳهه->cap = cap;
    *pd_جڳهه = pde;
    invalidate_صفحو_Structure_Cache_ASID(pptr_منزل_paddr(پاڙ), cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(پاڙ), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_X86_صفحو_جدول_Invocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cte_t *cte, cap_t cap,
    لفظ_t *بفر
)
{
    لفظ_t          vaddr;
    vm_خاصيتون_t attr;
    lookupPDSlot_ret_t pd_جڳهه;
    cap_t           vspaceCap;
    vspace_پاڙ_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    إذا (invLabel == X86_صفحو_جدول_Unmap) {
        إذا (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_پهريون;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_صفحو_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (invLabel != X86_صفحو_جدول_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پيغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_صفحو_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~نقاب(PT_اشاريو_بٽ + صفحو_بٽ));
    attr = vm_خاصيتون_From_لفظ(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_پاڙ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_پاڙ_t *)pptr_جو_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_چوٽي) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دليل;
        current_syscall_error.invalid_دليل_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret;

        ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (ڳوليو_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (ڳوليو_ret.vspace_پاڙ != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_جڳهه = lookupPDSlot(vspace, vaddr);
    إذا (pd_جڳهه.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ذريعو = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((pde_ptr_get_صفحو_سائيز(pd_جڳهه.pd_جڳهه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_جڳهه.pd_جڳهه)) ||
        ((pde_ptr_get_صفحو_سائيز(pd_جڳهه.pd_جڳهه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جڳهه.pd_جڳهه))) {
        current_syscall_error.type = se_L4_Delete_پهريون;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PTE_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_جدول(paddr, attr);

    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحو_جدول_cap_سيٽ_cap_PTMapped_پتو(cap, vaddr);

    سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X86_صفحو_جدول_Invocation_Map(cap, cte, pde, pd_جڳهه.pd_جڳهه, vspace);
}

exception_t decodeX86MMUInvocation(
    لفظ_t invLabel,
    لفظ_t ڊيگهائي,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    لفظ_t *بفر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_چوکٺو_cap:
        أرجع decode_X86_چوکٺو_Invocation(invLabel, ڊيگهائي, cte, cap, call, بفر);

    حالة cap_صفحو_جدول_cap:
        أرجع decode_X86_صفحو_جدول_Invocation(invLabel, ڊيگهائي, cte, cap, بفر);

    حالة cap_asid_control_cap: {
        لفظ_t     i;
        asid_t           asid_بنياد;
        لفظ_t           اشاريو;
        لفظ_t           اونهائي;
        cap_t            untyped;
        cap_t            پاڙ;
        cte_t           *والد_جڳهه;
        cte_t           *dest_جڳهه;
        lookup_جڳهه_ret_t lu_ret;
        فراغ            *چوکٺو;
        exception_t      حالت;

        إذا (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (ڊيگهائي < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        اشاريو = getSyscallArg(0, بفر);
        اونهائي = getSyscallArg(1, بفر);
        والد_جڳهه = current_extra_caps.excaprefs[0];
        untyped = والد_جڳهه->cap;
        پاڙ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنياد = i << asid_هيٺيون_بٽ;


        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاڪ_سائيز(untyped) != se_L4_ASIDPool_بٽ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(والد_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکٺو = لفظ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جڳهه(پاڙ, اشاريو, اونهائي);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_جڳهه = lu_ret.جڳهه;

        حالت = ensure_خالي_جايون_جڳهه(dest_جڳهه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکٺو, dest_جڳهه, والد_جڳهه, asid_بنياد);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_جڳهه;
        asid_pool_t *pool;
        لفظ_t i;
        asid_t       asid;

        إذا (invLabel != X86_ASIDPool_سونپيو) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پيغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جڳهه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جڳهه->cap;

        إذا (!(is_VTable_پاڙ(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_مٿاهون(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ذريعو = false;
            current_lookup_fault = lookup_fault_invalid_پاڙ_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < ٻٽو_انگ(asid_هيٺيون_بٽ) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        إذا (i == ٻٽو_انگ(asid_هيٺيون_بٽ)) {
            current_syscall_error.type = se_L4_Delete_پهريون;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سيٽ_Thread_State(ڳنڍ_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جڳهه);
    }

    افتراضي:
        أرجع decode_X86_گهڻائي_MMUInvocation(invLabel, ڊيگهائي, cptr, cte, cap, call, بفر);
    }
}
