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

ساكن exception_t perform_پاڼه_Get_پته(فراغ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = غوټه_STATE(ksCurThread);
    إذا (call) {
        کلمه_t *ipc_بفر = lookupIPCBuffer(true, thread);
        سېټ_ثبتوونکی(thread, badge_ثبتوونکی, 0);
        بینښه صحيح اوږدتیا = سېټ_MR(thread, ipc_بفر, 0, capFBasePtr);
        سېټ_ثبتوونکی(thread, پیغام_Info_ثبتوونکی, کلمه_From_پیغام_Info(
                        se_L4_پیغام_Info_new(0, 0, 0, اوږدتیا)));
    }
    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Running);
    أرجع EXCEPTION_NONE;
}

فراغ deleteASIDPool(asid_t asid_بنسټ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد(IS_ALIGNED(asid_بنسټ, asid_ټیټ_بټونه));

    إذا (x86KSASIDTable[ASID_لوړ(asid_بنسټ)] == pool) {
        لكل (بینښه صحيح دننه_واټن = 0; دننه_واټن < بټ(asid_ټیټ_بټونه); دننه_واټن++) {
            asid_map_t asid_map = pool->array[دننه_واټن];
            إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_ریښه_t *vspace = (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map);
                hwASIDInvalidate(asid_بنسټ + دننه_واټن, vspace);
            }
        }
        x86KSASIDTable[ASID_لوړ(asid_بنسټ)] = عدم;
        سېټ_VMRoot(غوټه_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(فراغ *چوکاټ, cte_t *خونه, cte_t *پلار, asid_t asid_بنسټ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_سېټ_cap_Free_شاخص(&(پلار->cap),
                                         MAX_FREE_شاخص(cap_untyped_cap_get_cap_بلاک_اندازه(پلار->cap)));

    memzero(چوکاټ, بټ(پاڼه_بټونه_For_اندازه(X86_Small_پاڼه)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ورننباسه(
        cap_asid_pool_cap_new(
            asid_بنسټ,          /* capASIDBase  */
            کلمه_REF(چوکاټ)     /* capASIDPool  */
        ),
        پلار,
        خونه
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    أكد((asid_بنسټ & ماسک(asid_ټیټ_بټونه)) == 0);
    x86KSASIDTable[ASID_لوړ(asid_بنسټ)] = (asid_pool_t *)چوکاټ;

    أرجع EXCEPTION_NONE;
}

فراغ deleteASID(asid_t asid, vspace_ریښه_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_لوړ(asid)];
    إذا (poolPtr != عدم) {
        asid_map_t asid_map = poolPtr->array[ASID_ټیټ(asid)];
        إذا (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_ټیټ(asid)] = asid_map_asid_map_none_new();
            سېټ_VMRoot(غوټه_STATE(ksCurThread));
        }
    }
}

کلمه_t *خالص lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    کلمه_t      w_بفر_Ptr;
    cap_t       بفر_Cap;
    vm_rights_t vm_rights;

    w_بفر_Ptr = thread->tcbIPCBuffer;
    بفر_Cap = TCB_PTR_CTE_PTR(thread, tcb_بفر)->cap;

    إذا (cap_get_capType(بفر_Cap) != cap_چوکاټ_cap) {
        أرجع عدم;
    }
    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(بفر_Cap))) {
        أرجع عدم;
    }

    vm_rights = cap_چوکاټ_cap_get_cap_FVMRights(بفر_Cap);
    إذا (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        کلمه_t بنسټ_Ptr, پاڼه_بټونه_2;

        بنسټ_Ptr = cap_چوکاټ_cap_get_cap_FBase_Ptr(بفر_Cap);
        پاڼه_بټونه_2 = پاڼه_بټونه_For_اندازه(cap_چوکاټ_cap_get_cap_FSize(بفر_Cap));
        أرجع (کلمه_t *)(بنسټ_Ptr + (w_بفر_Ptr & ماسک(پاڼه_بټونه_2)));
    } وإلا {
        أرجع عدم;
    }
}

bool_t CONST is_Valid_VTable_ریښه(cap_t cap)
{
    أرجع is_Valid_Native_ریښه(cap);
}


BOOT_کوډ bool_t map_kernel_کړکۍ_devices(pte_t *pt, uint32_t عدد_ioapic, paddr_t *ioapic_paddrs, uint32_t عدد_drhu,
                                           paddr_t *drhu_list)
{
    کلمه_t idx = (KDEV_بنسټ & ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه;
    paddr_t phys;
    pte_t pte;
    بینښه صحيح i;
    /* map kernel devices: APIC */
    phys = apic_get_بنسټ_paddr();
    إذا (!phys) {
        أرجع false;
    }
    إذا (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        أرجع false;
    }
    pte = x86_make_device_pte(phys);

    أكد(idx == (PPTR_APIC & ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه);
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
        أكد(idx == ((PPTR_IOAPIC_پیل + i * بټ(پاڼه_بټونه)) & ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه);
        pt[idx] = pte;
        idx++;
        إذا (idx == بټ(PT_شاخص_بټونه)) {
            أرجع false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    لكل (; i < CONFIG_MAX_عدد_IOAPIC; i++) {
        pte = x86_make_تشه_pte();
        أكد(idx == ((PPTR_IOAPIC_پیل + i * بټ(پاڼه_بټونه)) & ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه);
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

        أكد(idx == ((PPTR_DRHU_پیل + i * بټ(پاڼه_بټونه)) & ماسک(LARGE_پاڼه_بټونه)) >> پاڼه_بټونه);
        pt[idx] = pte;
        idx++;
        إذا (idx == بټ(PT_شاخص_بټونه)) {
            أرجع false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    طالما (idx < بټ(PT_شاخص_بټونه)) {
        pte = x86_make_تشه_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    أكد(idx == بټ(PT_شاخص_بټونه));
    أرجع true;
}

BOOT_کوډ ساكن فراغ init_idt(idt_ثبت_t *idt)
{
    init_idt_ثبت(idt, 0x00, int_00);
    init_idt_ثبت(idt, 0x01, int_01);
    init_idt_ثبت(idt, 0x02, int_02);
    init_idt_ثبت(idt, 0x03, int_03);
    init_idt_ثبت(idt, 0x04, int_04);
    init_idt_ثبت(idt, 0x05, int_05);
    init_idt_ثبت(idt, 0x06, int_06);
    init_idt_ثبت(idt, 0x07, int_07);
    init_idt_ثبت(idt, 0x08, int_08);
    init_idt_ثبت(idt, 0x09, int_09);
    init_idt_ثبت(idt, 0x0a, int_0a);
    init_idt_ثبت(idt, 0x0b, int_0b);
    init_idt_ثبت(idt, 0x0c, int_0c);
    init_idt_ثبت(idt, 0x0d, int_0d);
    init_idt_ثبت(idt, 0x0e, int_0e);
    init_idt_ثبت(idt, 0x0f, int_0f);

    init_idt_ثبت(idt, 0x10, int_10);
    init_idt_ثبت(idt, 0x11, int_11);
    init_idt_ثبت(idt, 0x12, int_12);
    init_idt_ثبت(idt, 0x13, int_13);
    init_idt_ثبت(idt, 0x14, int_14);
    init_idt_ثبت(idt, 0x15, int_15);
    init_idt_ثبت(idt, 0x16, int_16);
    init_idt_ثبت(idt, 0x17, int_17);
    init_idt_ثبت(idt, 0x18, int_18);
    init_idt_ثبت(idt, 0x19, int_19);
    init_idt_ثبت(idt, 0x1a, int_1a);
    init_idt_ثبت(idt, 0x1b, int_1b);
    init_idt_ثبت(idt, 0x1c, int_1c);
    init_idt_ثبت(idt, 0x1d, int_1d);
    init_idt_ثبت(idt, 0x1e, int_1e);
    init_idt_ثبت(idt, 0x1f, int_1f);

    init_idt_ثبت(idt, 0x20, int_20);
    init_idt_ثبت(idt, 0x21, int_21);
    init_idt_ثبت(idt, 0x22, int_22);
    init_idt_ثبت(idt, 0x23, int_23);
    init_idt_ثبت(idt, 0x24, int_24);
    init_idt_ثبت(idt, 0x25, int_25);
    init_idt_ثبت(idt, 0x26, int_26);
    init_idt_ثبت(idt, 0x27, int_27);
    init_idt_ثبت(idt, 0x28, int_28);
    init_idt_ثبت(idt, 0x29, int_29);
    init_idt_ثبت(idt, 0x2a, int_2a);
    init_idt_ثبت(idt, 0x2b, int_2b);
    init_idt_ثبت(idt, 0x2c, int_2c);
    init_idt_ثبت(idt, 0x2d, int_2d);
    init_idt_ثبت(idt, 0x2e, int_2e);
    init_idt_ثبت(idt, 0x2f, int_2f);

    init_idt_ثبت(idt, 0x30, int_30);
    init_idt_ثبت(idt, 0x31, int_31);
    init_idt_ثبت(idt, 0x32, int_32);
    init_idt_ثبت(idt, 0x33, int_33);
    init_idt_ثبت(idt, 0x34, int_34);
    init_idt_ثبت(idt, 0x35, int_35);
    init_idt_ثبت(idt, 0x36, int_36);
    init_idt_ثبت(idt, 0x37, int_37);
    init_idt_ثبت(idt, 0x38, int_38);
    init_idt_ثبت(idt, 0x39, int_39);
    init_idt_ثبت(idt, 0x3a, int_3a);
    init_idt_ثبت(idt, 0x3b, int_3b);
    init_idt_ثبت(idt, 0x3c, int_3c);
    init_idt_ثبت(idt, 0x3d, int_3d);
    init_idt_ثبت(idt, 0x3e, int_3e);
    init_idt_ثبت(idt, 0x3f, int_3f);

    init_idt_ثبت(idt, 0x40, int_40);
    init_idt_ثبت(idt, 0x41, int_41);
    init_idt_ثبت(idt, 0x42, int_42);
    init_idt_ثبت(idt, 0x43, int_43);
    init_idt_ثبت(idt, 0x44, int_44);
    init_idt_ثبت(idt, 0x45, int_45);
    init_idt_ثبت(idt, 0x46, int_46);
    init_idt_ثبت(idt, 0x47, int_47);
    init_idt_ثبت(idt, 0x48, int_48);
    init_idt_ثبت(idt, 0x49, int_49);
    init_idt_ثبت(idt, 0x4a, int_4a);
    init_idt_ثبت(idt, 0x4b, int_4b);
    init_idt_ثبت(idt, 0x4c, int_4c);
    init_idt_ثبت(idt, 0x4d, int_4d);
    init_idt_ثبت(idt, 0x4e, int_4e);
    init_idt_ثبت(idt, 0x4f, int_4f);

    init_idt_ثبت(idt, 0x50, int_50);
    init_idt_ثبت(idt, 0x51, int_51);
    init_idt_ثبت(idt, 0x52, int_52);
    init_idt_ثبت(idt, 0x53, int_53);
    init_idt_ثبت(idt, 0x54, int_54);
    init_idt_ثبت(idt, 0x55, int_55);
    init_idt_ثبت(idt, 0x56, int_56);
    init_idt_ثبت(idt, 0x57, int_57);
    init_idt_ثبت(idt, 0x58, int_58);
    init_idt_ثبت(idt, 0x59, int_59);
    init_idt_ثبت(idt, 0x5a, int_5a);
    init_idt_ثبت(idt, 0x5b, int_5b);
    init_idt_ثبت(idt, 0x5c, int_5c);
    init_idt_ثبت(idt, 0x5d, int_5d);
    init_idt_ثبت(idt, 0x5e, int_5e);
    init_idt_ثبت(idt, 0x5f, int_5f);

    init_idt_ثبت(idt, 0x60, int_60);
    init_idt_ثبت(idt, 0x61, int_61);
    init_idt_ثبت(idt, 0x62, int_62);
    init_idt_ثبت(idt, 0x63, int_63);
    init_idt_ثبت(idt, 0x64, int_64);
    init_idt_ثبت(idt, 0x65, int_65);
    init_idt_ثبت(idt, 0x66, int_66);
    init_idt_ثبت(idt, 0x67, int_67);
    init_idt_ثبت(idt, 0x68, int_68);
    init_idt_ثبت(idt, 0x69, int_69);
    init_idt_ثبت(idt, 0x6a, int_6a);
    init_idt_ثبت(idt, 0x6b, int_6b);
    init_idt_ثبت(idt, 0x6c, int_6c);
    init_idt_ثبت(idt, 0x6d, int_6d);
    init_idt_ثبت(idt, 0x6e, int_6e);
    init_idt_ثبت(idt, 0x6f, int_6f);

    init_idt_ثبت(idt, 0x70, int_70);
    init_idt_ثبت(idt, 0x71, int_71);
    init_idt_ثبت(idt, 0x72, int_72);
    init_idt_ثبت(idt, 0x73, int_73);
    init_idt_ثبت(idt, 0x74, int_74);
    init_idt_ثبت(idt, 0x75, int_75);
    init_idt_ثبت(idt, 0x76, int_76);
    init_idt_ثبت(idt, 0x77, int_77);
    init_idt_ثبت(idt, 0x78, int_78);
    init_idt_ثبت(idt, 0x79, int_79);
    init_idt_ثبت(idt, 0x7a, int_7a);
    init_idt_ثبت(idt, 0x7b, int_7b);
    init_idt_ثبت(idt, 0x7c, int_7c);
    init_idt_ثبت(idt, 0x7d, int_7d);
    init_idt_ثبت(idt, 0x7e, int_7e);
    init_idt_ثبت(idt, 0x7f, int_7f);

    init_idt_ثبت(idt, 0x80, int_80);
    init_idt_ثبت(idt, 0x81, int_81);
    init_idt_ثبت(idt, 0x82, int_82);
    init_idt_ثبت(idt, 0x83, int_83);
    init_idt_ثبت(idt, 0x84, int_84);
    init_idt_ثبت(idt, 0x85, int_85);
    init_idt_ثبت(idt, 0x86, int_86);
    init_idt_ثبت(idt, 0x87, int_87);
    init_idt_ثبت(idt, 0x88, int_88);
    init_idt_ثبت(idt, 0x89, int_89);
    init_idt_ثبت(idt, 0x8a, int_8a);
    init_idt_ثبت(idt, 0x8b, int_8b);
    init_idt_ثبت(idt, 0x8c, int_8c);
    init_idt_ثبت(idt, 0x8d, int_8d);
    init_idt_ثبت(idt, 0x8e, int_8e);
    init_idt_ثبت(idt, 0x8f, int_8f);

    init_idt_ثبت(idt, 0x90, int_90);
    init_idt_ثبت(idt, 0x91, int_91);
    init_idt_ثبت(idt, 0x92, int_92);
    init_idt_ثبت(idt, 0x93, int_93);
    init_idt_ثبت(idt, 0x94, int_94);
    init_idt_ثبت(idt, 0x95, int_95);
    init_idt_ثبت(idt, 0x96, int_96);
    init_idt_ثبت(idt, 0x97, int_97);
    init_idt_ثبت(idt, 0x98, int_98);
    init_idt_ثبت(idt, 0x99, int_99);
    init_idt_ثبت(idt, 0x9a, int_9a);
    init_idt_ثبت(idt, 0x9b, int_9b);
    init_idt_ثبت(idt, 0x9c, int_9c);
    init_idt_ثبت(idt, 0x9d, int_9d);
    init_idt_ثبت(idt, 0x9e, int_9e);
    init_idt_ثبت(idt, 0x9f, int_9f);

    init_idt_ثبت(idt, 0xa0, int_a0);
    init_idt_ثبت(idt, 0xa1, int_a1);
    init_idt_ثبت(idt, 0xa2, int_a2);
    init_idt_ثبت(idt, 0xa3, int_a3);
    init_idt_ثبت(idt, 0xa4, int_a4);
    init_idt_ثبت(idt, 0xa5, int_a5);
    init_idt_ثبت(idt, 0xa6, int_a6);
    init_idt_ثبت(idt, 0xa7, int_a7);
    init_idt_ثبت(idt, 0xa8, int_a8);
    init_idt_ثبت(idt, 0xa9, int_a9);
    init_idt_ثبت(idt, 0xaa, int_aa);
    init_idt_ثبت(idt, 0xab, int_ab);
    init_idt_ثبت(idt, 0xac, int_ac);
    init_idt_ثبت(idt, 0xad, int_ad);
    init_idt_ثبت(idt, 0xae, int_ae);
    init_idt_ثبت(idt, 0xaf, int_af);

    init_idt_ثبت(idt, 0xb0, int_b0);
    init_idt_ثبت(idt, 0xb1, int_b1);
    init_idt_ثبت(idt, 0xb2, int_b2);
    init_idt_ثبت(idt, 0xb3, int_b3);
    init_idt_ثبت(idt, 0xb4, int_b4);
    init_idt_ثبت(idt, 0xb5, int_b5);
    init_idt_ثبت(idt, 0xb6, int_b6);
    init_idt_ثبت(idt, 0xb7, int_b7);
    init_idt_ثبت(idt, 0xb8, int_b8);
    init_idt_ثبت(idt, 0xb9, int_b9);
    init_idt_ثبت(idt, 0xba, int_ba);
    init_idt_ثبت(idt, 0xbb, int_bb);
    init_idt_ثبت(idt, 0xbc, int_bc);
    init_idt_ثبت(idt, 0xbd, int_bd);
    init_idt_ثبت(idt, 0xbe, int_be);
    init_idt_ثبت(idt, 0xbf, int_bf);

    init_idt_ثبت(idt, 0xc0, int_c0);
    init_idt_ثبت(idt, 0xc1, int_c1);
    init_idt_ثبت(idt, 0xc2, int_c2);
    init_idt_ثبت(idt, 0xc3, int_c3);
    init_idt_ثبت(idt, 0xc4, int_c4);
    init_idt_ثبت(idt, 0xc5, int_c5);
    init_idt_ثبت(idt, 0xc6, int_c6);
    init_idt_ثبت(idt, 0xc7, int_c7);
    init_idt_ثبت(idt, 0xc8, int_c8);
    init_idt_ثبت(idt, 0xc9, int_c9);
    init_idt_ثبت(idt, 0xca, int_ca);
    init_idt_ثبت(idt, 0xcb, int_cb);
    init_idt_ثبت(idt, 0xcc, int_cc);
    init_idt_ثبت(idt, 0xcd, int_cd);
    init_idt_ثبت(idt, 0xce, int_ce);
    init_idt_ثبت(idt, 0xcf, int_cf);

    init_idt_ثبت(idt, 0xd0, int_d0);
    init_idt_ثبت(idt, 0xd1, int_d1);
    init_idt_ثبت(idt, 0xd2, int_d2);
    init_idt_ثبت(idt, 0xd3, int_d3);
    init_idt_ثبت(idt, 0xd4, int_d4);
    init_idt_ثبت(idt, 0xd5, int_d5);
    init_idt_ثبت(idt, 0xd6, int_d6);
    init_idt_ثبت(idt, 0xd7, int_d7);
    init_idt_ثبت(idt, 0xd8, int_d8);
    init_idt_ثبت(idt, 0xd9, int_d9);
    init_idt_ثبت(idt, 0xda, int_da);
    init_idt_ثبت(idt, 0xdb, int_db);
    init_idt_ثبت(idt, 0xdc, int_dc);
    init_idt_ثبت(idt, 0xdd, int_dd);
    init_idt_ثبت(idt, 0xde, int_de);
    init_idt_ثبت(idt, 0xdf, int_df);

    init_idt_ثبت(idt, 0xe0, int_e0);
    init_idt_ثبت(idt, 0xe1, int_e1);
    init_idt_ثبت(idt, 0xe2, int_e2);
    init_idt_ثبت(idt, 0xe3, int_e3);
    init_idt_ثبت(idt, 0xe4, int_e4);
    init_idt_ثبت(idt, 0xe5, int_e5);
    init_idt_ثبت(idt, 0xe6, int_e6);
    init_idt_ثبت(idt, 0xe7, int_e7);
    init_idt_ثبت(idt, 0xe8, int_e8);
    init_idt_ثبت(idt, 0xe9, int_e9);
    init_idt_ثبت(idt, 0xea, int_ea);
    init_idt_ثبت(idt, 0xeb, int_eb);
    init_idt_ثبت(idt, 0xec, int_ec);
    init_idt_ثبت(idt, 0xed, int_ed);
    init_idt_ثبت(idt, 0xee, int_ee);
    init_idt_ثبت(idt, 0xef, int_ef);

    init_idt_ثبت(idt, 0xf0, int_f0);
    init_idt_ثبت(idt, 0xf1, int_f1);
    init_idt_ثبت(idt, 0xf2, int_f2);
    init_idt_ثبت(idt, 0xf3, int_f3);
    init_idt_ثبت(idt, 0xf4, int_f4);
    init_idt_ثبت(idt, 0xf5, int_f5);
    init_idt_ثبت(idt, 0xf6, int_f6);
    init_idt_ثبت(idt, 0xf7, int_f7);
    init_idt_ثبت(idt, 0xf8, int_f8);
    init_idt_ثبت(idt, 0xf9, int_f9);
    init_idt_ثبت(idt, 0xfa, int_fa);
    init_idt_ثبت(idt, 0xfb, int_fb);
    init_idt_ثبت(idt, 0xfc, int_fc);
    init_idt_ثبت(idt, 0xfd, int_fd);
    init_idt_ثبت(idt, 0xfe, int_fe);
    init_idt_ثبت(idt, 0xff, int_ff);
}

BOOT_کوډ bool_t init_vm_state(فراغ)
{
    کلمه_t cache_کرښه_اندازه;
    x86_KScache_کرښه_اندازه_بټونه = get_Cache_کرښه_اندازه_بټونه();
    إذا (!x86_KScache_کرښه_اندازه_بټونه) {
        أرجع false;
    }

    cache_کرښه_اندازه = بټ(x86_KScache_کرښه_اندازه_بټونه);
    إذا (cache_کرښه_اندازه != L1_CACHE_کرښه_اندازه) {
        چاپ_کړه("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_کرښه_اندازه, cache_کرښه_اندازه);
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

BOOT_کوډ bool_t init_pat_msr(فراغ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    إذا ((x86_cpuid_edx(0x1, 0x0) & بټ(16)) == 0) {
        چاپ_کړه("PAT support not found\n");
        أرجع false;
    }
    pat_msr.ټکي[0] = x86_rdmsr_ټیټ(IA32_PAT_MSR);
    pat_msr.ټکي[1] = x86_rdmsr_لوړ(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_سېټ_pa0(pat_msr, IA32_PAT_MT_WRITE_شا);
    pat_msr = x86_pat_msr_سېټ_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_سېټ_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_سېټ_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_سېټ_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.ټکي[1]) << 32 | pat_msr.ټکي[0]);
    أرجع true;
}

BOOT_کوډ فراغ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_د_cap(it_ap_cap));
    ap->array[ASID_ټیټ(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_د_cap(it_vspace_cap));
    x86KSASIDTable[ASID_لوړ(IT_ASID)] = ap;
}

asid_map_t پیدا_کړه_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_لوړ(asid)];
    إذا (!poolPtr) {
        أرجع asid_map_asid_map_none_new();
    }

    أرجع poolPtr->array[ASID_ټیټ(asid)];
}

پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_VSpace_For_ASID(asid_t asid)
{
    پیدا_کړه_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = پیدا_کړه_Map_For_ASID(asid);
    إذا (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ریښه_new();

        ret.vspace_ریښه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    }

    ret.vspace_ریښه = (vspace_ریښه_t *)asid_map_asid_map_vspace_get_vspace_ریښه(asid_map);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    کلمه_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_ثبتوونکی(thread, Error);

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

lookupPTSlot_ret_t lookupPTSlot(vspace_ریښه_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_خونه;

    pd_خونه = lookupPDSlot(vspace, vptr);
    إذا (pd_خونه.حالت != EXCEPTION_NONE) {
        ret.pt_خونه = عدم;
        ret.حالت = pd_خونه.حالت;
        أرجع ret;
    }
    إذا ((pde_ptr_get_پاڼه_اندازه(pd_خونه.pd_خونه) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_خونه.pd_خونه)) {
        current_lookup_fault = lookup_fault_missing_capability_new(پاڼه_بټونه + PT_شاخص_بټونه);
        ret.pt_خونه = عدم;
        ret.حالت = EXCEPTION_LOOKUP_FAULT;
        أرجع ret;
    } وإلا {
        pte_t *pt;
        pte_t *pt_خونه;
        کلمه_t pt_شاخص;

        pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنسټ_پته(pd_خونه.pd_خونه));
        pt_شاخص = (vptr >> پاڼه_بټونه) & ماسک(PT_شاخص_بټونه);
        pt_خونه = pt + pt_شاخص;

        ret.pt_خونه = pt_خونه;
        ret.حالت = EXCEPTION_NONE;
        أرجع ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    إذا (cap_get_capType(cap) != cap_چوکاټ_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }
    إذا (unlikely(cap_چوکاټ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (!IS_ALIGNED(vptr, se_L4_IPCBuffer_اندازه_بټونه)) {
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

فراغ flush_جدول(vspace_ریښه_t *vspace, کلمه_t vptr, pte_t *pt, asid_t asid)
{
    کلمه_t i;
    cap_t        thread_ریښه;

    أكد(IS_ALIGNED(vptr, PT_شاخص_بټونه + پاڼه_بټونه));

    /* check if page table belongs to current address space */
    thread_ریښه = TCB_PTR_CTE_PTR(غوټه_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    لكل (i = 0; i < بټ(PT_شاخص_بټونه); i++) {
        إذا (pte_get_present(pt[i])) {
            إذا (config_سېټ(CONFIG_SUPPORT_PCID) || (is_Valid_Native_ریښه(thread_ریښه)
                                                    && (vspace_ریښه_t *)pptr_د_cap(thread_ریښه) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << پاڼه_بټونه), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


فراغ unmap_پاڼه(vm_پاڼه_اندازه_t پاڼه_اندازه, asid_t asid, vptr_t vptr, فراغ *pptr)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    بدل (پاڼه_اندازه) {
    حالة X86_Small_پاڼه:
        lu_ret = lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        إذا (!(pte_ptr_get_present(lu_ret.pt_خونه)
              && (pte_ptr_get_پاڼه_بنسټ_پته(lu_ret.pt_خونه)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *lu_ret.pt_خونه = makeUserPTEInvalid();
        اكسر;

    حالة X86_Large_پاڼه:
        pd_ret = lookupPDSlot(پیدا_کړه_ret.vspace_ریښه, vptr);
        إذا (pd_ret.حالت != EXCEPTION_NONE) {
            أرجع;
        }
        pde = pd_ret.pd_خونه;
        إذا (!(pde_ptr_get_پاڼه_اندازه(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_پاڼه_بنسټ_پته(pde)
                  == pptr_منزل_paddr(pptr)))) {
            أرجع;
        }
        *pde = makeUserPDEInvalid();
        اكسر;

    افتراضي:
        إذا (!ډېرپېښ_Unmap_پاڼه(پاڼه_اندازه, پیدا_کړه_ret.vspace_ریښه, vptr, pptr)) {
            أرجع;
        }
        اكسر;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(پیدا_کړه_ret.vspace_ریښه), 0));
}

فراغ unmap_پاڼه_جدول(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;
    lookupPDSlot_ret_t    lu_ret;

    پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
    إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    lu_ret = lookupPDSlot(پیدا_کړه_ret.vspace_ریښه, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        أرجع;
    }

    /* check if the PD actually refers to the PT */
    إذا (!(pde_ptr_get_پاڼه_اندازه(lu_ret.pd_خونه) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_خونه) &&
          (pde_pde_pt_ptr_get_pt_بنسټ_پته(lu_ret.pd_خونه) == pptr_منزل_paddr(pt)))) {
        أرجع;
    }

    flush_جدول(پیدا_کړه_ret.vspace_ریښه, vaddr, pt, asid);

    *lu_ret.pd_خونه = makeUserPDEInvalid();

    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(پیدا_کړه_ret.vspace_ریښه), asid,
                                     SMP_TERNARY(tlb_bitmap_get(پیدا_کړه_ret.vspace_ریښه), 0));
}

ساكن exception_t perform_X86_پاڼه_Invocation_Map_PTE(cap_t cap, cte_t *ct_خونه, pte_t *pt_خونه, pte_t pte,
                                                  vspace_ریښه_t *vspace)
{
    ct_خونه->cap = cap;
    *pt_خونه = pte;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_پاڼه_Invocation_Map_PDE(cap_t cap, cte_t *ct_خونه, pde_t *pd_خونه, pde_t pde,
                                                  vspace_ریښه_t *vspace)
{
    ct_خونه->cap = cap;
    *pd_خونه = pde;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(vspace), cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    أرجع EXCEPTION_NONE;
}


ساكن exception_t perform_X86_پاڼه_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{
    أكد(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap));
    أكد(cap_چوکاټ_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap)) {
        unmap_پاڼه(
            cap_چوکاټ_cap_get_cap_FSize(cap),
            cap_چوکاټ_cap_get_cap_FMapped_ASID(cap),
            cap_چوکاټ_cap_get_cap_FMapped_پته(cap),
            (فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_چوکاټ_cap_ptr_سېټ_cap_FMapped_پته(&ct_خونه->cap, 0);
    cap_چوکاټ_cap_ptr_سېټ_cap_FMapped_ASID(&ct_خونه->cap, asidInvalid);
    cap_چوکاټ_cap_ptr_سېټ_cap_FMap_Type(&ct_خونه->cap, X86_MappingNone);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_چوکاټ_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        بدل (cap_چوکاټ_cap_get_cap_FMap_Type(cap)) {
        حالة X86_MappingVSpace:
            أرجع perform_X86_پاڼه_Invocation_Unmap(cap, cte);
#که_تعریف_شوی CONFIG_IOMMU
        حالة X86_MappingIOSpace:
            أرجع performX86IOUnMapInvocation(cap, cte);
#د_که_پای
#که_تعریف_شوی CONFIG_VTX
        حالة X86_MappingEPT:
            أرجع performX86EPTPageInvocationUnmap(cap, cte);
#د_که_پای
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
    pte_t *pt_خونه;
};
تعريفنوع بنية create_mapping_pte_return create_mapping_pte_return_t;

ساكن create_mapping_pte_return_t create_خوندي_Mapping_Entries_PTE(paddr_t بنسټ, کلمه_t vaddr, vm_rights_t vmRights,
                                                                vm_ځانګړتیاوې_t attr,
                                                                vspace_ریښه_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        أرجع ret;
    }

    ret.pte = makeUserPTE(بنسټ, attr, vmRights);
    ret.pt_خونه = lu_ret.pt_خونه;
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}

بنية create_mapping_pde_return {
    exception_t حالت;
    pde_t pde;
    pde_t *pd_خونه;
};
تعريفنوع بنية create_mapping_pde_return create_mapping_pde_return_t;

ساكن create_mapping_pde_return_t create_خوندي_Mapping_Entries_PDE(paddr_t بنسټ, کلمه_t vaddr, vm_rights_t vmRights,
                                                                vm_ځانګړتیاوې_t attr,
                                                                vspace_ریښه_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    إذا (lu_ret.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        أرجع ret;
    }
    ret.pd_خونه = lu_ret.pd_خونه;

    /* check for existing page table */
    إذا ((pde_ptr_get_پاڼه_اندازه(ret.pd_خونه) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_خونه))) {
        current_syscall_error.type = se_L4_Delete_لومړی;
        ret.حالت = EXCEPTION_SYSCALL_ERROR;
        أرجع ret;
    }


    ret.pde = make_User_PDELarge_پاڼه(بنسټ, attr, vmRights);
    ret.حالت = EXCEPTION_NONE;
    أرجع ret;
}


exception_t decode_X86_چوکاټ_Invocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (invLabel) {
    حالة X86_پاڼه_Map: { /* Map */
        کلمه_t          vaddr;
        کلمه_t          vtop;
        کلمه_t          w_rights_ماسک;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_ریښه_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_ځانګړتیاوې_t vmAttr;
        vm_پاڼه_اندازه_t  چوکاټ_اندازه;
        asid_t          asid;

        إذا (اوږدتیا < 3 || current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        چوکاټ_اندازه = cap_چوکاټ_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, بفر);
        w_rights_ماسک = getSyscallArg(1, بفر);
        vmAttr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(2, بفر));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_چوکاټ_cap_get_cap_FVMRights(cap);

        إذا (!is_Valid_Native_ریښه(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_ریښه_t *)pptr_د_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            إذا (cap_چوکاټ_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکاټ_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (cap_چوکاټ_cap_get_cap_FMapped_پته(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        } وإلا {
            vtop = vaddr + بټ(پاڼه_بټونه_For_اندازه(چوکاټ_اندازه));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            إذا (vaddr > USER_سر || vtop > USER_سر) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_دلیل;
                current_syscall_error.invalid_دلیل_شمېره = 0;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

            پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
            إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_سرچینه = false;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }

            إذا (پیدا_کړه_ret.vspace_ریښه != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_شمېره = 1;

                أرجع EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = ماسک_VMRights(capVMRights, rights_From_کلمه(w_rights_ماسک));

        إذا (!checkVPAlignment(چوکاټ_اندازه, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_منزل_paddr((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap));

        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_ASID(cap, asid);
        cap = cap_چوکاټ_cap_سېټ_cap_FMapped_پته(cap, vaddr);
        cap = cap_چوکاټ_cap_سېټ_cap_FMap_Type(cap, X86_MappingVSpace);

        بدل (چوکاټ_اندازه) {
        /* PTE mappings */
        حالة X86_Small_پاڼه: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_خوندي_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_پاڼه_Invocation_Map_PTE(cap, cte, map_ret.pt_خونه, map_ret.pte, vspace);
        }

        /* PDE mappings */
        حالة X86_Large_پاڼه: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_خوندي_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            إذا (map_ret.حالت != EXCEPTION_NONE) {
                أرجع map_ret.حالت;
            }

            سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
            أرجع perform_X86_پاڼه_Invocation_Map_PDE(cap, cte, map_ret.pd_خونه, map_ret.pde, vspace);
        }

        افتراضي: {
            أرجع decode_X86_ډېرپېښ_Map_پاڼه(invLabel, چوکاټ_اندازه, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    حالة X86_پاڼه_Unmap: { /* Unmap */
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_چوکاټ_Invocation_Unmap(cap, cte);
    }

#که_تعریف_شوی CONFIG_IOMMU
    حالة X86_پاڼه_Map_IO: { /* MapIO */
        أرجع decodeX86IOMapInvocation(اوږدتیا, cte, cap, بفر);
    }
#د_که_پای

#که_تعریف_شوی CONFIG_VTX
    حالة X86_پاڼه_Map_EPT:
        أرجع decodeX86EPTPageMap(invLabel, اوږدتیا, cte, cap, بفر);
#د_که_پای

    حالة X86_پاڼه_Get_پته: {
        /* Return it in the first message register. */
        أكد(n_پیغام_Registers >= 1);

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_پاڼه_Get_پته((فراغ *)cap_چوکاټ_cap_get_cap_FBase_Ptr(cap), call);
    }

    افتراضي:
        current_syscall_error.type = seL4_IllegalOperation;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }
}

ساكن exception_t perform_X86_پاڼه_جدول_Invocation_Unmap(cap_t cap, cte_t *ct_خونه)
{

    إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap));
        unmap_پاڼه_جدول(
            cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
            cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap),
            pt
        );
        clear_حافظه((فراغ *)pt, cap_get_cap_اندازه_بټونه(cap));
    }
    cap_پاڼه_جدول_cap_ptr_سېټ_cap_PTIs_Mapped(&(ct_خونه->cap), 0);

    أرجع EXCEPTION_NONE;
}

ساكن exception_t perform_X86_پاڼه_جدول_Invocation_Map(cap_t cap, cte_t *ct_خونه, pde_t pde, pde_t *pd_خونه,
                                                    vspace_ریښه_t *ریښه)
{
    ct_خونه->cap = cap;
    *pd_خونه = pde;
    invalidate_پاڼه_Structure_Cache_ASID(pptr_منزل_paddr(ریښه), cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(ریښه), 0));
    أرجع EXCEPTION_NONE;
}

ساكن exception_t decode_X86_پاڼه_جدول_Invocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cte_t *cte, cap_t cap,
    کلمه_t *بفر
)
{
    کلمه_t          vaddr;
    vm_ځانګړتیاوې_t attr;
    lookupPDSlot_ret_t pd_خونه;
    cap_t           vspaceCap;
    vspace_ریښه_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    إذا (invLabel == X86_پاڼه_جدول_Unmap) {
        إذا (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_لومړی;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع perform_X86_پاڼه_جدول_Invocation_Unmap(cap, cte);
    }

    إذا (invLabel != X86_پاڼه_جدول_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_پیغام;
        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (cap_پاڼه_جدول_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, بفر) & (~ماسک(PT_شاخص_بټونه + پاڼه_بټونه));
    attr = vm_ځانګړتیاوې_From_کلمه(getSyscallArg(1, بفر));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    إذا (!is_Valid_Native_ریښه(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_شمېره = 1;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ریښه_t *)pptr_د_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    إذا (vaddr > USER_سر) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_دلیل;
        current_syscall_error.invalid_دلیل_شمېره = 0;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    {
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret;

        پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        إذا (پیدا_کړه_ret.حالت != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (پیدا_کړه_ret.vspace_ریښه != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_خونه = lookupPDSlot(vspace, vaddr);
    إذا (pd_خونه.حالت != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_سرچینه = false;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    إذا (((pde_ptr_get_پاڼه_اندازه(pd_خونه.pd_خونه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_خونه.pd_خونه)) ||
        ((pde_ptr_get_پاڼه_اندازه(pd_خونه.pd_خونه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_خونه.pd_خونه))) {
        current_syscall_error.type = se_L4_Delete_لومړی;

        أرجع EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_منزل_paddr(PTE_PTR(cap_پاڼه_جدول_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_جدول(paddr, attr);

    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTIs_Mapped(cap, 1);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_ASID(cap, asid);
    cap = cap_پاڼه_جدول_cap_سېټ_cap_PTMapped_پته(cap, vaddr);

    سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
    أرجع perform_X86_پاڼه_جدول_Invocation_Map(cap, cte, pde, pd_خونه.pd_خونه, vspace);
}

exception_t decodeX86MMUInvocation(
    کلمه_t invLabel,
    کلمه_t اوږدتیا,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    کلمه_t *بفر
)
{
    بدل (cap_get_capType(cap)) {

    حالة cap_چوکاټ_cap:
        أرجع decode_X86_چوکاټ_Invocation(invLabel, اوږدتیا, cte, cap, call, بفر);

    حالة cap_پاڼه_جدول_cap:
        أرجع decode_X86_پاڼه_جدول_Invocation(invLabel, اوږدتیا, cte, cap, بفر);

    حالة cap_asid_control_cap: {
        کلمه_t     i;
        asid_t           asid_بنسټ;
        کلمه_t           شاخص;
        کلمه_t           ژوروالی;
        cap_t            untyped;
        cap_t            ریښه;
        cte_t           *پلار_خونه;
        cte_t           *dest_خونه;
        lookup_خونه_ret_t lu_ret;
        فراغ            *چوکاټ;
        exception_t      حالت;

        إذا (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (اوږدتیا < 2 || current_extra_caps.excaprefs[0] == عدم
            || current_extra_caps.excaprefs[1] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        شاخص = getSyscallArg(0, بفر);
        ژوروالی = getSyscallArg(1, بفر);
        پلار_خونه = current_extra_caps.excaprefs[0];
        untyped = پلار_خونه->cap;
        ریښه = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        لكل (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        إذا (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid_بنسټ = i << asid_ټیټ_بټونه;


        إذا (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_بلاک_اندازه(untyped) != se_L4_ASIDPool_بټونه ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        حالت = ensureNoChildren(پلار_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        چوکاټ = کلمه_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_موخه_خونه(ریښه, شاخص, ژوروالی);
        إذا (lu_ret.حالت != EXCEPTION_NONE) {
            أرجع lu_ret.حالت;
        }
        dest_خونه = lu_ret.خونه;

        حالت = ensure_تشه_خونه(dest_خونه);
        إذا (حالت != EXCEPTION_NONE) {
            أرجع حالت;
        }

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDControlInvocation(چوکاټ, dest_خونه, پلار_خونه, asid_بنسټ);
    }

    حالة cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_خونه;
        asid_pool_t *pool;
        کلمه_t i;
        asid_t       asid;

        إذا (invLabel != X86_ASIDPool_ټاکل) {
            current_syscall_error.type = seL4_IllegalOperation;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }
        إذا (current_extra_caps.excaprefs[0] == عدم) {
            current_syscall_error.type = se_L4_Truncated_پیغام;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_خونه = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_خونه->cap;

        إذا (!(is_VTable_ریښه(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 1;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_لوړ(cap_asid_pool_cap_get_capASIDBase(cap))];
        إذا (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_سرچینه = false;
            current_lookup_fault = lookup_fault_invalid_ریښه_new();
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        إذا (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_شمېره = 0;
            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        لكل (i = 0; i < بټ(asid_ټیټ_بټونه) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        إذا (i == بټ(asid_ټیټ_بټونه)) {
            current_syscall_error.type = se_L4_Delete_لومړی;

            أرجع EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        سېټ_Thread_State(غوټه_STATE(ksCurThread), ThreadState_Restart);
        أرجع performASIDPoolInvocation(asid, pool, vspace_Cap_خونه);
    }

    افتراضي:
        أرجع decode_X86_ډېرپېښ_MMUInvocation(invLabel, اوږدتیا, cptr, cte, cap, call, بفر);
    }
}
