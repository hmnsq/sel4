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

ساكن exception_t perform_صفحه_Get_آدرس(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = گره_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_میانگیر = lookupIPCBuffer(true, thread);
        مجموعه_ثبات(thread, badge_ثبات, 0);
        بیعلامت صحيح درازی = مجموعه_MR(thread, ipc_میانگیر, 0, capFBasePtr);
        مجموعه_ثبات(thread, پیام_Info_ثبات, کلمه_From_پیام_Info(
                        se_L4_پیام_Info_new(0, 0, 0, درازی)));
    }
    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

فراغ deleteASIDPool(asid_t asid_پایه, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_پایه, asid_کم_بیتها));

    إذا (x86KSASIDTable[ASID_بالا(asid_پایه)] == pool) {
        لكل (بیعلامت صحيح جابه_جایی = 0; جابه_جایی < بیت(asid_کم_بیتها); جابه_جایی++) {
            asid_map_t asid_map = pool->array[جابه_جایی];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_ریشه_t *vspace = (vspace_ریشه_t *)asid_map_asid_map_vspace_get_vspace_ریشه(asid_map);
                hwASIDInvalidate(asid_پایه + جابه_جایی, vspace);
            }
        }
        x86KSASIDTable[ASID_بالا(asid_پایه)] = عدم;
        مجموعه_VMRoot(گره_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(فراغ *قاب, cte_t *جایگاه, cte_t *والد, asid_t asid_پایه)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_مجموعه_cap_Free_شاخص(&(والد->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلوک_اندازه(والد->cap)));

    memzero(قاب, بیت(صفحه_بیتها_For_اندازه(X86_Small_صفحه)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_درج(
        cap_asid_pool_cap_new(
            asid_پایه,          /* capASIDBase  */
            کلمه_REF(قاب)     /* capASIDPool  */
        ),
        والد,
        جایگاه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_پایه & ماسک(asid_کم_بیتها)) == 0);
    x86KSASIDTable[ASID_بالا(asid_پایه)] = (asid_pool_t *)قاب;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, vspace_ریشه_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بالا(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_کم(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ریشه_t *)asid_map_asid_map_vspace_get_vspace_ریشه(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_کم(asid)] = asid_map_asid_map_none_new();
            مجموعه_VMRoot(گره_STATE(ksCurThread));
        }
    }
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t      w_میانگیر_Ptr;
    cap_t       میانگیر_Cap;
    vm_rights_t vm_rights;

    w_میانگیر_Ptr = thread->tcbIPCBuffer;
    میانگیر_Cap = TCB_PTR_CTE_PTR(thread, tcb_میانگیر)->cap;

    إذا (cap_get_capType(میانگیر_Cap) != cap_قاب_cap) {
        أرجع عدم;
    }
    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(میانگیر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_قاب_cap_get_cap_FVMRights(میانگیر_Cap);
    إذا (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        کلمه_t پایه_Ptr, صفحه_بیتها_2;

        پایه_Ptr = cap_قاب_cap_get_cap_FBase_Ptr(میانگیر_Cap);
        صفحه_بیتها_2 = صفحه_بیتها_For_اندازه(cap_قاب_cap_get_cap_FSize(میانگیر_Cap));
        أرجع (کلمه_t *)(پایه_Ptr + (w_میانگیر_Ptr & ماسک(صفحه_بیتها_2)));
    } وإلا {
        أرجع عدم;
    }
}

bool_t CONST is_Valid_VTable_ریشه(cap_t cap)
{
    أرجع is_Valid_Native_ریشه(cap);
}


BOOT_کد bool_t map_kernel_پنجره_devices(pte_t *pt, uint32_t رقم_ioapic, paddr_t *ioapic_paddrs, uint32_t رقم_drhu,
                                           paddr_t *drhu_list)
{
    کلمه_t idx = (KDEV_پایه & ماسک(LARGE_صفحه_بیتها)) >> صفحه_بیتها;
    paddr_t phys;
    pte_t pte;
    بیعلامت صحيح i;
    /* map kernel devices: APIC */
    phys = apic_get_پایه_paddr();
    إذا (!phys) {
        أرجع false;
    }
    إذا (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        أرجع false;
    }
    pte = x86_make_device_pte(phys);

    أكد(idx == (PPTR_APIC & ماسک(LARGE_صفحه_بیتها)) >> صفحه_بیتها);
    pt[idx] = pte;
    idx++;
    لكل (i = 0; i < رقم_ioapic; i++) {
        phys = ioapic_paddrs[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);
        أكد(idx == ((PPTR_IOAPIC_شروع + i * بیت(صفحه_بیتها)) & ماسک(LARGE_صفحه_بیتها)) >> صفحه_بیتها);
        pt[idx] = pte;
        idx++;
        إذا (idx == بیت(PT_شاخص_بیتها)) {
            أرجع false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    لكل (; i < CONFIG_MAX_رقم_IOAPIC; i++) {
        pte = x86_make_خالی_pte();
        أكد(idx == ((PPTR_IOAPIC_شروع + i * بیت(صفحه_بیتها)) & ماسک(LARGE_صفحه_بیتها)) >> صفحه_بیتها);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    لكل (i = 0; i < رقم_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        إذا (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            أرجع false;
        }
        pte = x86_make_device_pte(phys);

        أكد(idx == ((PPTR_DRHU_شروع + i * بیت(صفحه_بیتها)) & ماسک(LARGE_صفحه_بیتها)) >> صفحه_بیتها);
        pt[idx] = pte;
        idx++;
        إذا (idx == بیت(PT_شاخص_بیتها)) {
            أرجع false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    طالما (idx < بیت(PT_شاخص_بیتها)) {
        pte = x86_make_خالی_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    أكد(idx == بیت(PT_شاخص_بیتها));
    أرجع true;
}

BOOT_کد ساكن فراغ init_idt(idt_مدخل_t *idt)
{
    init_idt_مدخل(idt, 0x00, int_00);
    init_idt_مدخل(idt, 0x01, int_01);
    init_idt_مدخل(idt, 0x02, int_02);
    init_idt_مدخل(idt, 0x03, int_03);
    init_idt_مدخل(idt, 0x04, int_04);
    init_idt_مدخل(idt, 0x05, int_05);
    init_idt_مدخل(idt, 0x06, int_06);
    init_idt_مدخل(idt, 0x07, int_07);
    init_idt_مدخل(idt, 0x08, int_08);
    init_idt_مدخل(idt, 0x09, int_09);
    init_idt_مدخل(idt, 0x0a, int_0a);
    init_idt_مدخل(idt, 0x0b, int_0b);
    init_idt_مدخل(idt, 0x0c, int_0c);
    init_idt_مدخل(idt, 0x0d, int_0d);
    init_idt_مدخل(idt, 0x0e, int_0e);
    init_idt_مدخل(idt, 0x0f, int_0f);

    init_idt_مدخل(idt, 0x10, int_10);
    init_idt_مدخل(idt, 0x11, int_11);
    init_idt_مدخل(idt, 0x12, int_12);
    init_idt_مدخل(idt, 0x13, int_13);
    init_idt_مدخل(idt, 0x14, int_14);
    init_idt_مدخل(idt, 0x15, int_15);
    init_idt_مدخل(idt, 0x16, int_16);
    init_idt_مدخل(idt, 0x17, int_17);
    init_idt_مدخل(idt, 0x18, int_18);
    init_idt_مدخل(idt, 0x19, int_19);
    init_idt_مدخل(idt, 0x1a, int_1a);
    init_idt_مدخل(idt, 0x1b, int_1b);
    init_idt_مدخل(idt, 0x1c, int_1c);
    init_idt_مدخل(idt, 0x1d, int_1d);
    init_idt_مدخل(idt, 0x1e, int_1e);
    init_idt_مدخل(idt, 0x1f, int_1f);

    init_idt_مدخل(idt, 0x20, int_20);
    init_idt_مدخل(idt, 0x21, int_21);
    init_idt_مدخل(idt, 0x22, int_22);
    init_idt_مدخل(idt, 0x23, int_23);
    init_idt_مدخل(idt, 0x24, int_24);
    init_idt_مدخل(idt, 0x25, int_25);
    init_idt_مدخل(idt, 0x26, int_26);
    init_idt_مدخل(idt, 0x27, int_27);
    init_idt_مدخل(idt, 0x28, int_28);
    init_idt_مدخل(idt, 0x29, int_29);
    init_idt_مدخل(idt, 0x2a, int_2a);
    init_idt_مدخل(idt, 0x2b, int_2b);
    init_idt_مدخل(idt, 0x2c, int_2c);
    init_idt_مدخل(idt, 0x2d, int_2d);
    init_idt_مدخل(idt, 0x2e, int_2e);
    init_idt_مدخل(idt, 0x2f, int_2f);

    init_idt_مدخل(idt, 0x30, int_30);
    init_idt_مدخل(idt, 0x31, int_31);
    init_idt_مدخل(idt, 0x32, int_32);
    init_idt_مدخل(idt, 0x33, int_33);
    init_idt_مدخل(idt, 0x34, int_34);
    init_idt_مدخل(idt, 0x35, int_35);
    init_idt_مدخل(idt, 0x36, int_36);
    init_idt_مدخل(idt, 0x37, int_37);
    init_idt_مدخل(idt, 0x38, int_38);
    init_idt_مدخل(idt, 0x39, int_39);
    init_idt_مدخل(idt, 0x3a, int_3a);
    init_idt_مدخل(idt, 0x3b, int_3b);
    init_idt_مدخل(idt, 0x3c, int_3c);
    init_idt_مدخل(idt, 0x3d, int_3d);
    init_idt_مدخل(idt, 0x3e, int_3e);
    init_idt_مدخل(idt, 0x3f, int_3f);

    init_idt_مدخل(idt, 0x40, int_40);
    init_idt_مدخل(idt, 0x41, int_41);
    init_idt_مدخل(idt, 0x42, int_42);
    init_idt_مدخل(idt, 0x43, int_43);
    init_idt_مدخل(idt, 0x44, int_44);
    init_idt_مدخل(idt, 0x45, int_45);
    init_idt_مدخل(idt, 0x46, int_46);
    init_idt_مدخل(idt, 0x47, int_47);
    init_idt_مدخل(idt, 0x48, int_48);
    init_idt_مدخل(idt, 0x49, int_49);
    init_idt_مدخل(idt, 0x4a, int_4a);
    init_idt_مدخل(idt, 0x4b, int_4b);
    init_idt_مدخل(idt, 0x4c, int_4c);
    init_idt_مدخل(idt, 0x4d, int_4d);
    init_idt_مدخل(idt, 0x4e, int_4e);
    init_idt_مدخل(idt, 0x4f, int_4f);

    init_idt_مدخل(idt, 0x50, int_50);
    init_idt_مدخل(idt, 0x51, int_51);
    init_idt_مدخل(idt, 0x52, int_52);
    init_idt_مدخل(idt, 0x53, int_53);
    init_idt_مدخل(idt, 0x54, int_54);
    init_idt_مدخل(idt, 0x55, int_55);
    init_idt_مدخل(idt, 0x56, int_56);
    init_idt_مدخل(idt, 0x57, int_57);
    init_idt_مدخل(idt, 0x58, int_58);
    init_idt_مدخل(idt, 0x59, int_59);
    init_idt_مدخل(idt, 0x5a, int_5a);
    init_idt_مدخل(idt, 0x5b, int_5b);
    init_idt_مدخل(idt, 0x5c, int_5c);
    init_idt_مدخل(idt, 0x5d, int_5d);
    init_idt_مدخل(idt, 0x5e, int_5e);
    init_idt_مدخل(idt, 0x5f, int_5f);

    init_idt_مدخل(idt, 0x60, int_60);
    init_idt_مدخل(idt, 0x61, int_61);
    init_idt_مدخل(idt, 0x62, int_62);
    init_idt_مدخل(idt, 0x63, int_63);
    init_idt_مدخل(idt, 0x64, int_64);
    init_idt_مدخل(idt, 0x65, int_65);
    init_idt_مدخل(idt, 0x66, int_66);
    init_idt_مدخل(idt, 0x67, int_67);
    init_idt_مدخل(idt, 0x68, int_68);
    init_idt_مدخل(idt, 0x69, int_69);
    init_idt_مدخل(idt, 0x6a, int_6a);
    init_idt_مدخل(idt, 0x6b, int_6b);
    init_idt_مدخل(idt, 0x6c, int_6c);
    init_idt_مدخل(idt, 0x6d, int_6d);
    init_idt_مدخل(idt, 0x6e, int_6e);
    init_idt_مدخل(idt, 0x6f, int_6f);

    init_idt_مدخل(idt, 0x70, int_70);
    init_idt_مدخل(idt, 0x71, int_71);
    init_idt_مدخل(idt, 0x72, int_72);
    init_idt_مدخل(idt, 0x73, int_73);
    init_idt_مدخل(idt, 0x74, int_74);
    init_idt_مدخل(idt, 0x75, int_75);
    init_idt_مدخل(idt, 0x76, int_76);
    init_idt_مدخل(idt, 0x77, int_77);
    init_idt_مدخل(idt, 0x78, int_78);
    init_idt_مدخل(idt, 0x79, int_79);
    init_idt_مدخل(idt, 0x7a, int_7a);
    init_idt_مدخل(idt, 0x7b, int_7b);
    init_idt_مدخل(idt, 0x7c, int_7c);
    init_idt_مدخل(idt, 0x7d, int_7d);
    init_idt_مدخل(idt, 0x7e, int_7e);
    init_idt_مدخل(idt, 0x7f, int_7f);

    init_idt_مدخل(idt, 0x80, int_80);
    init_idt_مدخل(idt, 0x81, int_81);
    init_idt_مدخل(idt, 0x82, int_82);
    init_idt_مدخل(idt, 0x83, int_83);
    init_idt_مدخل(idt, 0x84, int_84);
    init_idt_مدخل(idt, 0x85, int_85);
    init_idt_مدخل(idt, 0x86, int_86);
    init_idt_مدخل(idt, 0x87, int_87);
    init_idt_مدخل(idt, 0x88, int_88);
    init_idt_مدخل(idt, 0x89, int_89);
    init_idt_مدخل(idt, 0x8a, int_8a);
    init_idt_مدخل(idt, 0x8b, int_8b);
    init_idt_مدخل(idt, 0x8c, int_8c);
    init_idt_مدخل(idt, 0x8d, int_8d);
    init_idt_مدخل(idt, 0x8e, int_8e);
    init_idt_مدخل(idt, 0x8f, int_8f);

    init_idt_مدخل(idt, 0x90, int_90);
    init_idt_مدخل(idt, 0x91, int_91);
    init_idt_مدخل(idt, 0x92, int_92);
    init_idt_مدخل(idt, 0x93, int_93);
    init_idt_مدخل(idt, 0x94, int_94);
    init_idt_مدخل(idt, 0x95, int_95);
    init_idt_مدخل(idt, 0x96, int_96);
    init_idt_مدخل(idt, 0x97, int_97);
    init_idt_مدخل(idt, 0x98, int_98);
    init_idt_مدخل(idt, 0x99, int_99);
    init_idt_مدخل(idt, 0x9a, int_9a);
    init_idt_مدخل(idt, 0x9b, int_9b);
    init_idt_مدخل(idt, 0x9c, int_9c);
    init_idt_مدخل(idt, 0x9d, int_9d);
    init_idt_مدخل(idt, 0x9e, int_9e);
    init_idt_مدخل(idt, 0x9f, int_9f);

    init_idt_مدخل(idt, 0xa0, int_a0);
    init_idt_مدخل(idt, 0xa1, int_a1);
    init_idt_مدخل(idt, 0xa2, int_a2);
    init_idt_مدخل(idt, 0xa3, int_a3);
    init_idt_مدخل(idt, 0xa4, int_a4);
    init_idt_مدخل(idt, 0xa5, int_a5);
    init_idt_مدخل(idt, 0xa6, int_a6);
    init_idt_مدخل(idt, 0xa7, int_a7);
    init_idt_مدخل(idt, 0xa8, int_a8);
    init_idt_مدخل(idt, 0xa9, int_a9);
    init_idt_مدخل(idt, 0xaa, int_aa);
    init_idt_مدخل(idt, 0xab, int_ab);
    init_idt_مدخل(idt, 0xac, int_ac);
    init_idt_مدخل(idt, 0xad, int_ad);
    init_idt_مدخل(idt, 0xae, int_ae);
    init_idt_مدخل(idt, 0xaf, int_af);

    init_idt_مدخل(idt, 0xb0, int_b0);
    init_idt_مدخل(idt, 0xb1, int_b1);
    init_idt_مدخل(idt, 0xb2, int_b2);
    init_idt_مدخل(idt, 0xb3, int_b3);
    init_idt_مدخل(idt, 0xb4, int_b4);
    init_idt_مدخل(idt, 0xb5, int_b5);
    init_idt_مدخل(idt, 0xb6, int_b6);
    init_idt_مدخل(idt, 0xb7, int_b7);
    init_idt_مدخل(idt, 0xb8, int_b8);
    init_idt_مدخل(idt, 0xb9, int_b9);
    init_idt_مدخل(idt, 0xba, int_ba);
    init_idt_مدخل(idt, 0xbb, int_bb);
    init_idt_مدخل(idt, 0xbc, int_bc);
    init_idt_مدخل(idt, 0xbd, int_bd);
    init_idt_مدخل(idt, 0xbe, int_be);
    init_idt_مدخل(idt, 0xbf, int_bf);

    init_idt_مدخل(idt, 0xc0, int_c0);
    init_idt_مدخل(idt, 0xc1, int_c1);
    init_idt_مدخل(idt, 0xc2, int_c2);
    init_idt_مدخل(idt, 0xc3, int_c3);
    init_idt_مدخل(idt, 0xc4, int_c4);
    init_idt_مدخل(idt, 0xc5, int_c5);
    init_idt_مدخل(idt, 0xc6, int_c6);
    init_idt_مدخل(idt, 0xc7, int_c7);
    init_idt_مدخل(idt, 0xc8, int_c8);
    init_idt_مدخل(idt, 0xc9, int_c9);
    init_idt_مدخل(idt, 0xca, int_ca);
    init_idt_مدخل(idt, 0xcb, int_cb);
    init_idt_مدخل(idt, 0xcc, int_cc);
    init_idt_مدخل(idt, 0xcd, int_cd);
    init_idt_مدخل(idt, 0xce, int_ce);
    init_idt_مدخل(idt, 0xcf, int_cf);

    init_idt_مدخل(idt, 0xd0, int_d0);
    init_idt_مدخل(idt, 0xd1, int_d1);
    init_idt_مدخل(idt, 0xd2, int_d2);
    init_idt_مدخل(idt, 0xd3, int_d3);
    init_idt_مدخل(idt, 0xd4, int_d4);
    init_idt_مدخل(idt, 0xd5, int_d5);
    init_idt_مدخل(idt, 0xd6, int_d6);
    init_idt_مدخل(idt, 0xd7, int_d7);
    init_idt_مدخل(idt, 0xd8, int_d8);
    init_idt_مدخل(idt, 0xd9, int_d9);
    init_idt_مدخل(idt, 0xda, int_da);
    init_idt_مدخل(idt, 0xdb, int_db);
    init_idt_مدخل(idt, 0xdc, int_dc);
    init_idt_مدخل(idt, 0xdd, int_dd);
    init_idt_مدخل(idt, 0xde, int_de);
    init_idt_مدخل(idt, 0xdf, int_df);

    init_idt_مدخل(idt, 0xe0, int_e0);
    init_idt_مدخل(idt, 0xe1, int_e1);
    init_idt_مدخل(idt, 0xe2, int_e2);
    init_idt_مدخل(idt, 0xe3, int_e3);
    init_idt_مدخل(idt, 0xe4, int_e4);
    init_idt_مدخل(idt, 0xe5, int_e5);
    init_idt_مدخل(idt, 0xe6, int_e6);
    init_idt_مدخل(idt, 0xe7, int_e7);
    init_idt_مدخل(idt, 0xe8, int_e8);
    init_idt_مدخل(idt, 0xe9, int_e9);
    init_idt_مدخل(idt, 0xea, int_ea);
    init_idt_مدخل(idt, 0xeb, int_eb);
    init_idt_مدخل(idt, 0xec, int_ec);
    init_idt_مدخل(idt, 0xed, int_ed);
    init_idt_مدخل(idt, 0xee, int_ee);
    init_idt_مدخل(idt, 0xef, int_ef);

    init_idt_مدخل(idt, 0xf0, int_f0);
    init_idt_مدخل(idt, 0xf1, int_f1);
    init_idt_مدخل(idt, 0xf2, int_f2);
    init_idt_مدخل(idt, 0xf3, int_f3);
    init_idt_مدخل(idt, 0xf4, int_f4);
    init_idt_مدخل(idt, 0xf5, int_f5);
    init_idt_مدخل(idt, 0xf6, int_f6);
    init_idt_مدخل(idt, 0xf7, int_f7);
    init_idt_مدخل(idt, 0xf8, int_f8);
    init_idt_مدخل(idt, 0xf9, int_f9);
    init_idt_مدخل(idt, 0xfa, int_fa);
    init_idt_مدخل(idt, 0xfb, int_fb);
    init_idt_مدخل(idt, 0xfc, int_fc);
    init_idt_مدخل(idt, 0xfd, int_fd);
    init_idt_مدخل(idt, 0xfe, int_fe);
    init_idt_مدخل(idt, 0xff, int_ff);
}

BOOT_کد bool_t init_vm_state(فراغ)
{
    کلمه_t cache_سطر_اندازه;
    x86_KScache_سطر_اندازه_بیتها = get_Cache_سطر_اندازه_بیتها();
    إذا (!x86_KScache_سطر_اندازه_بیتها) {
        أرجع false;
    }

    cache_سطر_اندازه = بیت(x86_KScache_سطر_اندازه_بیتها);
    إذا (cache_سطر_اندازه != L1_CACHE_سطر_اندازه) {
        چاپ_کن("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_سطر_اندازه, cache_سطر_اندازه);
        SMP_COND_STATEMENT(أرجع false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    فراغ *tss_ptr = &x86KSGlobalState[CURRENT_CPU_شاخص()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_شاخص()].x86KSidt);
    أرجع true;
}

BOOT_کد bool_t init_pat_msr(فراغ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    إذا ((x86_cpuid_edx(0x1, 0x0) & بیت(16)) == 0) {
        چاپ_کن("PAT support not found\n");
        أرجع false;
    }
    pat_msr.واژگان[0] = x86_rdmsr_کم(IA32_PAT_MSR);
    pat_msr.واژگان[1] = x86_rdmsr_بالا(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_مجموعه_pa0(pat_msr, IA32_PAT_MT_WRITE_پشت);
    pat_msr = x86_pat_msr_مجموعه_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_مجموعه_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_مجموعه_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_مجموعه_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.واژگان[1]) << 32 | pat_msr.واژگان[0]);
    أرجع true;
}

BOOT_کد فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_از_cap(it_ap_cap));
    ap->array[ASID_کم(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_از_cap(it_vspace_cap));
    x86KSASIDTable[ASID_بالا(IT_ASID)] = ap;
}

asid_map_t پیدا_کن_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_بالا(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_کم(asid)];
}

پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_VSpace_For_ASID(asid_t asid)
{
    پیدا_کن_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کن_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ریشه_new();

        ret.vspace_ریشه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریشه = (vspace_ریشه_t *)asid_map_asid_map_vspace_get_vspace_ریشه(asid_map);
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    کلمه_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_ثبات(thread, Error);

    بدل (vm_faultType) {
    حالة X86_داده_Fault:
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

lookupPTSlot_ret_t lookupPTSlot(vspace_ریشه_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_جایگاه;

    pd_جایگاه = lookupPDSlot(vspace, vptr);
    إذا (pd_جایگاه.وضعیت != EXCEPTION_NONE) {
        ret.pt_جایگاه = عدم;
        ret.وضعیت = pd_جایگاه.وضعیت;
        أرجع ret;
    }
    إذا ((pde_ptr_get_صفحه_اندازه(pd_جایگاه.pd_جایگاه) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_جایگاه.pd_جایگاه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(صفحه_بیتها + PT_شاخص_بیتها);
        ret.pt_جایگاه = عدم;
        ret.وضعیت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt;
        pte_t *pt_جایگاه;
        کلمه_t pt_شاخص;

        pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_پایه_آدرس(pd_جایگاه.pd_جایگاه));
        pt_شاخص = (vptr >> صفحه_بیتها) & ماسک(PT_شاخص_بیتها);
        pt_جایگاه = pt + pt_شاخص;

        ret.pt_جایگاه = pt_جایگاه;
        ret.وضعیت = EXCEPTION_NONE;
        أرجع ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_قاب_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_قاب_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بیتها)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (صحيح)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    أرجع EXCEPTION_NONE;
}

vm_rights_t CONST ماسک_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ماسک)
{
    إذا (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        أرجع VMReadOnly;
    }
    إذا (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ماسک)) {
        إذا (!seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
            أرجع VMReadOnly;
        } وإلا {
            أرجع VMReadWrite;
        }
    }
    إذا (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ماسک) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ماسک)) {
        userError("Attempted to make unsupported write only mapping");
    }
    أرجع VMKernelOnly;
}

فراغ flush_جدول(vspace_ریشه_t *vspace, کلمه_t vptr, pte_t *pt, asid_t asid)
{
    کلمه_t i;
    cap_t        thread_ریشه;

    أكد(IS_ALIGNED(vptr, PT_شاخص_بیتها + صفحه_بیتها));

    /* check if page table belongs to current address space */
    thread_ریشه = TCB_PTR_CTE_PTR(گره_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    لكل (i = 0; i < بیت(PT_شاخص_بیتها); i++) {
        إذا (pte_get_present(pt[i])) {
            إذا (config_مجموعه(CONFIG_SUPPORT_PCID) || (is_Valid_Native_ریشه(thread_ریشه)
                                                    && (vspace_ریشه_t *)pptr_از_cap(thread_ریشه) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << صفحه_بیتها), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


فراغ unmap_صفحه(vm_صفحه_اندازه_t صفحه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (صفحه_اندازه) {
    حالة X86_Small_صفحه:
        lu_ret = lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vptr);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!(pte_ptr_get_present(lu_ret.pt_جایگاه)
              && (pte_ptr_get_صفحه_پایه_آدرس(lu_ret.pt_جایگاه)
                  == pptr_مقصد_paddr(pptr)))) {
            أرجع;
        }
        *lu_ret.pt_جایگاه = makeUserPTEInvalid();
        اكسر;

    حالة X86_Large_صفحه:
        pd_ret = lookupPDSlot(پیدا_کن_ret.vspace_ریشه, vptr);
        إذا (pd_ret.وضعیت != EXCEPTION_NONE) {
            أرجع;
        }
        pde = pd_ret.pd_جایگاه;
        إذا (!(pde_ptr_get_صفحه_اندازه(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_صفحه_پایه_آدرس(pde)
                  == pptr_مقصد_paddr(pptr)))) {
            أرجع;
        }
        *pde = makeUserPDEInvalid();
        اكسر;

    افتراضي:
        إذا (!نما_Unmap_صفحه(صفحه_اندازه, پیدا_کن_ret.vspace_ریشه, vptr, pptr)) {
            أرجع;
        }
        اكسر;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(پیدا_کن_ret.vspace_ریشه), 0));
}

فراغ unmap_صفحه_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;
    lookupPDSlot_ret_t    lu_ret;

    پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDSlot(پیدا_کن_ret.vspace_ریشه, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PD actually refers to the PT */
    إذا (!(pde_ptr_get_صفحه_اندازه(lu_ret.pd_جایگاه) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_جایگاه) &&
          (pde_pde_pt_ptr_get_pt_پایه_آدرس(lu_ret.pd_جایگاه) == pptr_مقصد_paddr(pt)))) {
        أرجع;
    }

    flush_جدول(پیدا_کن_ret.vspace_ریشه, vaddr, pt, asid);

    *lu_ret.pd_جایگاه = makeUserPDEInvalid();

    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(پیدا_کن_ret.vspace_ریشه), asid,
                                     SMP_TERNARY(tlb_bitmap_get(پیدا_کن_ret.vspace_ریشه), 0));
}

ساكن exception_t perform_X86_صفحه_Invocation_Map_PTE(cap_t cap, cte_t *ct_جایگاه, pte_t *pt_جایگاه, pte_t pte,
                                                  vspace_ریشه_t *vspace)
{
    ct_جایگاه->cap = cap;
    *pt_جایگاه = pte;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_قاب_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_صفحه_Invocation_Map_PDE(cap_t cap, cte_t *ct_جایگاه, pde_t *pd_جایگاه, pde_t pde,
                                                  vspace_ریشه_t *vspace)
{
    ct_جایگاه->cap = cap;
    *pd_جایگاه = pde;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(vspace), cap_قاب_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_X86_صفحه_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{
    أكد(cap_قاب_cap_get_cap_FMapped_ASID(cap));
    أكد(cap_قاب_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap)) {
        unmap_صفحه(
            cap_قاب_cap_get_cap_FSize(cap),
            cap_قاب_cap_get_cap_FMapped_ASID(cap),
            cap_قاب_cap_get_cap_FMapped_آدرس(cap),
            (فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_قاب_cap_ptr_مجموعه_cap_FMapped_آدرس(&ct_جایگاه->cap, 0);
    cap_قاب_cap_ptr_مجموعه_cap_FMapped_ASID(&ct_جایگاه->cap, asidInvalid);
    cap_قاب_cap_ptr_مجموعه_cap_FMap_Type(&ct_جایگاه->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_قاب_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        بدل (cap_قاب_cap_get_cap_FMap_Type(cap)) {
        حالة X86_MappingVSpace:
            أرجع perform_X86_صفحه_Invocation_Unmap(cap, cte);
#اگر_تعریف_شده CONFIG_IOMMU
        حالة X86_MappingIOSpace:
            أرجع performX86IOUnMapInvocation(cap, cte);
#پایان_اگر
#اگر_تعریف_شده CONFIG_VTX
        حالة X86_MappingEPT:
            أرجع performX86EPTPageInvocationUnmap(cap, cte);
#پایان_اگر
        حالة X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            اكسر;
        }
    }

    أرجع EXCEPTION_NONE;
}

بنية create_mapping_pte_return {
    exception_t وضعیت;
    pte_t pte;
    pte_t *pt_جایگاه;
};
تعريفنوع بنية create_mapping_pte_return create_mapping_pte_return_t;

ساكن create_mapping_pte_return_t create_امن_Mapping_Entries_PTE(paddr_t پایه, کلمه_t vaddr, vm_rights_t vmRights,
                                                                vm_صفات_t attr,
                                                                vspace_ریشه_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع ret;
    }

    ret.pte = makeUserPTE(پایه, attr, vmRights);
    ret.pt_جایگاه = lu_ret.pt_جایگاه;
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}

بنية create_mapping_pde_return {
    exception_t وضعیت;
    pde_t pde;
    pde_t *pd_جایگاه;
};
تعريفنوع بنية create_mapping_pde_return create_mapping_pde_return_t;

ساكن create_mapping_pde_return_t create_امن_Mapping_Entries_PDE(paddr_t پایه, کلمه_t vaddr, vm_rights_t vmRights,
                                                                vm_صفات_t attr,
                                                                vspace_ریشه_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pd_جایگاه = lu_ret.pd_جایگاه;

    /* check for existing page table */
    إذا ((pde_ptr_get_صفحه_اندازه(ret.pd_جایگاه) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_جایگاه))) {
        current_syscall_error.type = se_L4_Delete_اولی;
        ret.وضعیت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }


    ret.pde = make_User_PDELarge_صفحه(پایه, attr, vmRights);
    ret.وضعیت = EXCEPTION_NONE;
    أرجع ret;
}


exception_t decode_X86_قاب_Invocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (invLabel) {
    حالة X86_صفحه_Map: { /* Map */
        کلمه_t          vaddr;
        کلمه_t          vtop;
        کلمه_t          w_rights_ماسک;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_ریشه_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_صفات_t vmAttr;
        vm_صفحه_اندازه_t  قاب_اندازه;
        asid_t          asid;

        إذا (درازی < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        قاب_اندازه = cap_قاب_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, میانگیر);
        w_rights_ماسک = getSyscallArg(1, میانگیر);
        vmAttr = vm_صفات_From_کلمه(getSyscallArg(2, میانگیر));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_قاب_cap_get_cap_FVMRights(cap);

        إذا (!is_Valid_Native_ریشه(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_ریشه_t *)pptr_از_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            إذا (cap_قاب_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_قاب_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_قاب_cap_get_cap_FMapped_آدرس(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + بیت(صفحه_بیتها_For_اندازه(قاب_اندازه));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            إذا (vaddr > USER_بالاترین || vtop > USER_بالاترین) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_آرگومان;
                current_syscall_error.invalid_آرگومان_عدد = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

            پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
            إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_منبع = false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (پیدا_کن_ret.vspace_ریشه != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_عدد = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

        إذا (!checkVPAlignment(قاب_اندازه, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_مقصد_paddr((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap));

        cap = cap_قاب_cap_مجموعه_cap_FMapped_ASID(cap, asid);
        cap = cap_قاب_cap_مجموعه_cap_FMapped_آدرس(cap, vaddr);
        cap = cap_قاب_cap_مجموعه_cap_FMap_Type(cap, X86_MappingVSpace);

        بدل (قاب_اندازه) {
        /* PTE mappings */
        حالة X86_Small_صفحه: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_امن_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.وضعیت != EXCEPTION_NONE) {
                أرجع map_ret.وضعیت;
            }

            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_صفحه_Invocation_Map_PTE(cap, cte, map_ret.pt_جایگاه, map_ret.pte, vspace);
        }

        /* PDE mappings */
        حالة X86_Large_صفحه: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_امن_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.وضعیت != EXCEPTION_NONE) {
                أرجع map_ret.وضعیت;
            }

            مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_صفحه_Invocation_Map_PDE(cap, cte, map_ret.pd_جایگاه, map_ret.pde, vspace);
        }

        افتراضي: {
            أرجع decode_X86_نما_Map_صفحه(invLabel, قاب_اندازه, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    حالة X86_صفحه_Unmap: { /* Unmap */
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_قاب_Invocation_Unmap(cap, cte);
    }

#اگر_تعریف_شده CONFIG_IOMMU
    حالة X86_صفحه_Map_IO: { /* MapIO */
        أرجع decodeX86IOMapInvocation(درازی, cte, cap, میانگیر);
    }
#پایان_اگر

#اگر_تعریف_شده CONFIG_VTX
    حالة X86_صفحه_Map_EPT:
        أرجع decodeX86EPTPageMap(invLabel, درازی, cte, cap, میانگیر);
#پایان_اگر

    حالة X86_صفحه_Get_آدرس: {
        /* Return it in the first message register. */
        أكد(n_پیام_Registers >= 1);

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_صفحه_Get_آدرس((فراغ *)cap_قاب_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

ساكن exception_t perform_X86_صفحه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_جایگاه)
{

    إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_صفحه_جدول(
            cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap),
            pt
        );
        clear_حافظه((فراغ *)pt, cap_get_cap_اندازه_بیتها(cap));
    }
    cap_صفحه_جدول_cap_ptr_مجموعه_cap_PTIs_Mapped(&(ct_جایگاه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_صفحه_جدول_Invocation_Map(cap_t cap, cte_t *ct_جایگاه, pde_t pde, pde_t *pd_جایگاه,
                                                    vspace_ریشه_t *ریشه)
{
    ct_جایگاه->cap = cap;
    *pd_جایگاه = pde;
    invalidate_صفحه_Structure_Cache_ASID(pptr_مقصد_paddr(ریشه), cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(ریشه), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_X86_صفحه_جدول_Invocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cte_t *cte, cap_t cap,
    کلمه_t *میانگیر
)
{
    کلمه_t          vaddr;
    vm_صفات_t attr;
    lookupPDSlot_ret_t pd_جایگاه;
    cap_t           vspaceCap;
    vspace_ریشه_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    إذا (invLabel == X86_صفحه_جدول_Unmap) {
        إذا (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_اولی;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_صفحه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (invLabel != X86_صفحه_جدول_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_صفحه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, میانگیر) & (~ماسک(PT_شاخص_بیتها + صفحه_بیتها));
    attr = vm_صفات_From_کلمه(getSyscallArg(1, میانگیر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریشه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_عدد = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریشه_t *)pptr_از_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_بالاترین) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_آرگومان;
        current_syscall_error.invalid_آرگومان_عدد = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret;

        پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (پیدا_کن_ret.وضعیت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (پیدا_کن_ret.vspace_ریشه != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_جایگاه = lookupPDSlot(vspace, vaddr);
    إذا (pd_جایگاه.وضعیت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_منبع = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((pde_ptr_get_صفحه_اندازه(pd_جایگاه.pd_جایگاه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_جایگاه.pd_جایگاه)) ||
        ((pde_ptr_get_صفحه_اندازه(pd_جایگاه.pd_جایگاه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جایگاه.pd_جایگاه))) {
        current_syscall_error.type = se_L4_Delete_اولی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_مقصد_paddr(PTE_PTR(cap_صفحه_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_جدول(paddr, attr);

    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTIs_Mapped(cap, 1);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_ASID(cap, asid);
    cap = cap_صفحه_جدول_cap_مجموعه_cap_PTMapped_آدرس(cap, vaddr);

    مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X86_صفحه_جدول_Invocation_Map(cap, cte, pde, pd_جایگاه.pd_جایگاه, vspace);
}

exception_t decodeX86MMUInvocation(
    کلمه_t invLabel,
    کلمه_t درازی,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *میانگیر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_قاب_cap:
        أرجع decode_X86_قاب_Invocation(invLabel, درازی, cte, cap, call, میانگیر);

    حالة cap_صفحه_جدول_cap:
        أرجع decode_X86_صفحه_جدول_Invocation(invLabel, درازی, cte, cap, میانگیر);

    حالة cap_asid_control_cap: {
        کلمه_t     i;
        asid_t           asid_پایه;
        کلمه_t           شاخص;
        کلمه_t           عمق;
        cap_t            untyped;
        cap_t            ریشه;
        cte_t           *والد_جایگاه;
        cte_t           *dest_جایگاه;
        lookup_جایگاه_ret_t lu_ret;
        فراغ            *قاب;
        exception_t      وضعیت;

        إذا (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (درازی < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, میانگیر);
        عمق = getSyscallArg(1, میانگیر);
        والد_جایگاه = current_extra_caps.excaprefs[0];
        untyped = والد_جایگاه->cap;
        ریشه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_پایه = i << asid_کم_بیتها;


        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلوک_اندازه(untyped) != se_L4_ASIDPool_بیتها ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        وضعیت = ensureNoChildren(والد_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        قاب = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_هدف_جایگاه(ریشه, شاخص, عمق);
        إذا (lu_ret.وضعیت != EXCEPTION_NONE) {
            أرجع lu_ret.وضعیت;
        }
        dest_جایگاه = lu_ret.جایگاه;

        وضعیت = ensure_خالی_جایگاه(dest_جایگاه);
        إذا (وضعیت != EXCEPTION_NONE) {
            أرجع وضعیت;
        }

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(قاب, dest_جایگاه, والد_جایگاه, asid_پایه);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_جایگاه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t       asid;

        إذا (invLabel != X86_ASIDPool_تخصیص_بده) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_جایگاه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_جایگاه->cap;

        إذا (!(is_VTable_ریشه(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_بالا(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_منبع = false;
            current_lookup_fault = lookup_fault_invalid_ریشه_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_عدد = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < بیت(asid_کم_بیتها) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        إذا (i == بیت(asid_کم_بیتها)) {
            current_syscall_error.type = se_L4_Delete_اولی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        مجموعه_Thread_State(گره_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_جایگاه);
    }

    افتراضي:
        أرجع decode_X86_نما_MMUInvocation(invLabel, درازی, cptr, cte, cap, call, میانگیر);
    }
}
