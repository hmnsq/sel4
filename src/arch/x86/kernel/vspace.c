/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <api/syscall.h>
#அந்தர்பாவ <machine/io.h>
#அந்தர்பாவ <kernel/boot.h>
#அந்தர்பாவ <model/statedata.h>
#அந்தர்பாவ <arch/kernel/vspace.h>
#அந்தர்பாவ <arch/api/invocation.h>
#அந்தர்பாவ <arch/kernel/tlb_bitmap.h>
#அந்தர்பாவ <mode/kernel/tlb.h>
#அந்தர்பாவ <mode/kernel/vspace.h>

நிலையான exception_t performPageGetAddress(வெறுமை *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = முனை_STATE(ksCurThread);
    எனில் (call) {
        word_t *ipcBuffer = lookupIPCBuffer(true, thread);
        setRegister(thread, badgeRegister, 0);
        unsigned முழு length = setMR(thread, ipcBuffer, 0, capFBasePtr);
        setRegister(thread, செய்தி_Info_Register, wordFromMessageInfo(
                        seL4_MessageInfo_new(0, 0, 0, length)));
    }
    setThreadState(முனை_STATE(ksCurThread), ThreadState_Running);
    திரும்பு EXCEPTION_NONE;
}

வெறுமை deleteASIDPool(asid_t asid_base, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ப்ரதிஜ்ஞா(IS_ALIGNED(asid_base, asid_கீழ்_Bits));

    எனில் (x86KSASIDTable[ASID_உயர்வு(asid_base)] == pool) {
        க்கு (unsigned முழு offset = 0; offset < BIT(asid_கீழ்_Bits); offset++) {
            asid_map_t asid_map = pool->array[offset];
            எனில் (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_வேர்_t *vspace = (vspace_வேர்_t *)asid_map_asid_map_vspace_get_vspace_வேர்(asid_map);
                hwASIDInvalidate(asid_base + offset, vspace);
            }
        }
        x86KSASIDTable[ASID_உயர்வு(asid_base)] = வெற்று;
        setVMRoot(முனை_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(வெறுமை *frame, cte_t *slot, cte_t *parent, asid_t asid_base)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_set_capFreeIndex(&(parent->cap),
                                         MAX_FREE_INDEX(cap_untyped_cap_get_capBlockSize(parent->cap)));

    memzero(frame, BIT(pageBitsForSize(X86_SmallPage)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_செருகு(
        cap_asid_pool_cap_new(
            asid_base,          /* capASIDBase  */
            WORD_REF(frame)     /* capASIDPool  */
        ),
        parent,
        slot
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ப்ரதிஜ்ஞா((asid_base & MASK(asid_கீழ்_Bits)) == 0);
    x86KSASIDTable[ASID_உயர்வு(asid_base)] = (asid_pool_t *)frame;

    திரும்பு EXCEPTION_NONE;
}

வெறுமை deleteASID(asid_t asid, vspace_வேர்_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_உயர்வு(asid)];
    எனில் (poolPtr != வெற்று) {
        asid_map_t asid_map = poolPtr->array[ASID_கீழ்(asid)];
        எனில் (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_வேர்_t *)asid_map_asid_map_vspace_get_vspace_வேர்(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_கீழ்(asid)] = asid_map_asid_map_none_new();
            setVMRoot(முனை_STATE(ksCurThread));
        }
    }
}

word_t *PURE lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    word_t      w_bufferPtr;
    cap_t       bufferCap;
    vm_rights_t vm_rights;

    w_bufferPtr = thread->tcbIPCBuffer;
    bufferCap = TCB_PTR_CTE_PTR(thread, tcbBuffer)->cap;

    எனில் (cap_get_capType(bufferCap) != cap_frame_cap) {
        திரும்பு வெற்று;
    }
    எனில் (unlikely(cap_frame_cap_get_capFIsDevice(bufferCap))) {
        திரும்பு வெற்று;
    }

    vm_rights = cap_frame_cap_get_capFVMRights(bufferCap);
    எனில் (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        word_t basePtr, pageBits;

        basePtr = cap_frame_cap_get_capFBasePtr(bufferCap);
        pageBits = pageBitsForSize(cap_frame_cap_get_capFSize(bufferCap));
        திரும்பு (word_t *)(basePtr + (w_bufferPtr & MASK(pageBits)));
    } இல்லையெனில் {
        திரும்பு வெற்று;
    }
}

bool_t CONST is_Valid_VTable_வேர்(cap_t cap)
{
    திரும்பு is_Valid_Native_வேர்(cap);
}


BOOT_CODE bool_t map_kernel_window_devices(pte_t *pt, uint32_t இலக்கம்_ioapic, paddr_t *ioapic_paddrs, uint32_t இலக்கம்_drhu,
                                           paddr_t *drhu_list)
{
    word_t idx = (KDEV_BASE & MASK(LARGE_PAGE_BITS)) >> PAGE_BITS;
    paddr_t phys;
    pte_t pte;
    unsigned முழு i;
    /* map kernel devices: APIC */
    phys = apic_get_base_paddr();
    எனில் (!phys) {
        திரும்பு false;
    }
    எனில் (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        திரும்பு false;
    }
    pte = x86_make_device_pte(phys);

    ப்ரதிஜ்ஞா(idx == (PPTR_APIC & MASK(LARGE_PAGE_BITS)) >> PAGE_BITS);
    pt[idx] = pte;
    idx++;
    க்கு (i = 0; i < இலக்கம்_ioapic; i++) {
        phys = ioapic_paddrs[i];
        எனில் (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            திரும்பு false;
        }
        pte = x86_make_device_pte(phys);
        ப்ரதிஜ்ஞா(idx == ((PPTR_IOAPIC_START + i * BIT(PAGE_BITS)) & MASK(LARGE_PAGE_BITS)) >> PAGE_BITS);
        pt[idx] = pte;
        idx++;
        எனில் (idx == BIT(PT_INDEX_BITS)) {
            திரும்பு false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    க்கு (; i < CONFIG_MAX_இலக்கம்_IOAPIC; i++) {
        pte = x86_make_empty_pte();
        ப்ரதிஜ்ஞா(idx == ((PPTR_IOAPIC_START + i * BIT(PAGE_BITS)) & MASK(LARGE_PAGE_BITS)) >> PAGE_BITS);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    க்கு (i = 0; i < இலக்கம்_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        எனில் (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            திரும்பு false;
        }
        pte = x86_make_device_pte(phys);

        ப்ரதிஜ்ஞா(idx == ((PPTR_DRHU_START + i * BIT(PAGE_BITS)) & MASK(LARGE_PAGE_BITS)) >> PAGE_BITS);
        pt[idx] = pte;
        idx++;
        எனில் (idx == BIT(PT_INDEX_BITS)) {
            திரும்பு false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    போது (idx < BIT(PT_INDEX_BITS)) {
        pte = x86_make_empty_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    ப்ரதிஜ்ஞா(idx == BIT(PT_INDEX_BITS));
    திரும்பு true;
}

BOOT_CODE நிலையான வெறுமை init_idt(idt_entry_t *idt)
{
    init_idt_entry(idt, 0x00, int_00);
    init_idt_entry(idt, 0x01, int_01);
    init_idt_entry(idt, 0x02, int_02);
    init_idt_entry(idt, 0x03, int_03);
    init_idt_entry(idt, 0x04, int_04);
    init_idt_entry(idt, 0x05, int_05);
    init_idt_entry(idt, 0x06, int_06);
    init_idt_entry(idt, 0x07, int_07);
    init_idt_entry(idt, 0x08, int_08);
    init_idt_entry(idt, 0x09, int_09);
    init_idt_entry(idt, 0x0a, int_0a);
    init_idt_entry(idt, 0x0b, int_0b);
    init_idt_entry(idt, 0x0c, int_0c);
    init_idt_entry(idt, 0x0d, int_0d);
    init_idt_entry(idt, 0x0e, int_0e);
    init_idt_entry(idt, 0x0f, int_0f);

    init_idt_entry(idt, 0x10, int_10);
    init_idt_entry(idt, 0x11, int_11);
    init_idt_entry(idt, 0x12, int_12);
    init_idt_entry(idt, 0x13, int_13);
    init_idt_entry(idt, 0x14, int_14);
    init_idt_entry(idt, 0x15, int_15);
    init_idt_entry(idt, 0x16, int_16);
    init_idt_entry(idt, 0x17, int_17);
    init_idt_entry(idt, 0x18, int_18);
    init_idt_entry(idt, 0x19, int_19);
    init_idt_entry(idt, 0x1a, int_1a);
    init_idt_entry(idt, 0x1b, int_1b);
    init_idt_entry(idt, 0x1c, int_1c);
    init_idt_entry(idt, 0x1d, int_1d);
    init_idt_entry(idt, 0x1e, int_1e);
    init_idt_entry(idt, 0x1f, int_1f);

    init_idt_entry(idt, 0x20, int_20);
    init_idt_entry(idt, 0x21, int_21);
    init_idt_entry(idt, 0x22, int_22);
    init_idt_entry(idt, 0x23, int_23);
    init_idt_entry(idt, 0x24, int_24);
    init_idt_entry(idt, 0x25, int_25);
    init_idt_entry(idt, 0x26, int_26);
    init_idt_entry(idt, 0x27, int_27);
    init_idt_entry(idt, 0x28, int_28);
    init_idt_entry(idt, 0x29, int_29);
    init_idt_entry(idt, 0x2a, int_2a);
    init_idt_entry(idt, 0x2b, int_2b);
    init_idt_entry(idt, 0x2c, int_2c);
    init_idt_entry(idt, 0x2d, int_2d);
    init_idt_entry(idt, 0x2e, int_2e);
    init_idt_entry(idt, 0x2f, int_2f);

    init_idt_entry(idt, 0x30, int_30);
    init_idt_entry(idt, 0x31, int_31);
    init_idt_entry(idt, 0x32, int_32);
    init_idt_entry(idt, 0x33, int_33);
    init_idt_entry(idt, 0x34, int_34);
    init_idt_entry(idt, 0x35, int_35);
    init_idt_entry(idt, 0x36, int_36);
    init_idt_entry(idt, 0x37, int_37);
    init_idt_entry(idt, 0x38, int_38);
    init_idt_entry(idt, 0x39, int_39);
    init_idt_entry(idt, 0x3a, int_3a);
    init_idt_entry(idt, 0x3b, int_3b);
    init_idt_entry(idt, 0x3c, int_3c);
    init_idt_entry(idt, 0x3d, int_3d);
    init_idt_entry(idt, 0x3e, int_3e);
    init_idt_entry(idt, 0x3f, int_3f);

    init_idt_entry(idt, 0x40, int_40);
    init_idt_entry(idt, 0x41, int_41);
    init_idt_entry(idt, 0x42, int_42);
    init_idt_entry(idt, 0x43, int_43);
    init_idt_entry(idt, 0x44, int_44);
    init_idt_entry(idt, 0x45, int_45);
    init_idt_entry(idt, 0x46, int_46);
    init_idt_entry(idt, 0x47, int_47);
    init_idt_entry(idt, 0x48, int_48);
    init_idt_entry(idt, 0x49, int_49);
    init_idt_entry(idt, 0x4a, int_4a);
    init_idt_entry(idt, 0x4b, int_4b);
    init_idt_entry(idt, 0x4c, int_4c);
    init_idt_entry(idt, 0x4d, int_4d);
    init_idt_entry(idt, 0x4e, int_4e);
    init_idt_entry(idt, 0x4f, int_4f);

    init_idt_entry(idt, 0x50, int_50);
    init_idt_entry(idt, 0x51, int_51);
    init_idt_entry(idt, 0x52, int_52);
    init_idt_entry(idt, 0x53, int_53);
    init_idt_entry(idt, 0x54, int_54);
    init_idt_entry(idt, 0x55, int_55);
    init_idt_entry(idt, 0x56, int_56);
    init_idt_entry(idt, 0x57, int_57);
    init_idt_entry(idt, 0x58, int_58);
    init_idt_entry(idt, 0x59, int_59);
    init_idt_entry(idt, 0x5a, int_5a);
    init_idt_entry(idt, 0x5b, int_5b);
    init_idt_entry(idt, 0x5c, int_5c);
    init_idt_entry(idt, 0x5d, int_5d);
    init_idt_entry(idt, 0x5e, int_5e);
    init_idt_entry(idt, 0x5f, int_5f);

    init_idt_entry(idt, 0x60, int_60);
    init_idt_entry(idt, 0x61, int_61);
    init_idt_entry(idt, 0x62, int_62);
    init_idt_entry(idt, 0x63, int_63);
    init_idt_entry(idt, 0x64, int_64);
    init_idt_entry(idt, 0x65, int_65);
    init_idt_entry(idt, 0x66, int_66);
    init_idt_entry(idt, 0x67, int_67);
    init_idt_entry(idt, 0x68, int_68);
    init_idt_entry(idt, 0x69, int_69);
    init_idt_entry(idt, 0x6a, int_6a);
    init_idt_entry(idt, 0x6b, int_6b);
    init_idt_entry(idt, 0x6c, int_6c);
    init_idt_entry(idt, 0x6d, int_6d);
    init_idt_entry(idt, 0x6e, int_6e);
    init_idt_entry(idt, 0x6f, int_6f);

    init_idt_entry(idt, 0x70, int_70);
    init_idt_entry(idt, 0x71, int_71);
    init_idt_entry(idt, 0x72, int_72);
    init_idt_entry(idt, 0x73, int_73);
    init_idt_entry(idt, 0x74, int_74);
    init_idt_entry(idt, 0x75, int_75);
    init_idt_entry(idt, 0x76, int_76);
    init_idt_entry(idt, 0x77, int_77);
    init_idt_entry(idt, 0x78, int_78);
    init_idt_entry(idt, 0x79, int_79);
    init_idt_entry(idt, 0x7a, int_7a);
    init_idt_entry(idt, 0x7b, int_7b);
    init_idt_entry(idt, 0x7c, int_7c);
    init_idt_entry(idt, 0x7d, int_7d);
    init_idt_entry(idt, 0x7e, int_7e);
    init_idt_entry(idt, 0x7f, int_7f);

    init_idt_entry(idt, 0x80, int_80);
    init_idt_entry(idt, 0x81, int_81);
    init_idt_entry(idt, 0x82, int_82);
    init_idt_entry(idt, 0x83, int_83);
    init_idt_entry(idt, 0x84, int_84);
    init_idt_entry(idt, 0x85, int_85);
    init_idt_entry(idt, 0x86, int_86);
    init_idt_entry(idt, 0x87, int_87);
    init_idt_entry(idt, 0x88, int_88);
    init_idt_entry(idt, 0x89, int_89);
    init_idt_entry(idt, 0x8a, int_8a);
    init_idt_entry(idt, 0x8b, int_8b);
    init_idt_entry(idt, 0x8c, int_8c);
    init_idt_entry(idt, 0x8d, int_8d);
    init_idt_entry(idt, 0x8e, int_8e);
    init_idt_entry(idt, 0x8f, int_8f);

    init_idt_entry(idt, 0x90, int_90);
    init_idt_entry(idt, 0x91, int_91);
    init_idt_entry(idt, 0x92, int_92);
    init_idt_entry(idt, 0x93, int_93);
    init_idt_entry(idt, 0x94, int_94);
    init_idt_entry(idt, 0x95, int_95);
    init_idt_entry(idt, 0x96, int_96);
    init_idt_entry(idt, 0x97, int_97);
    init_idt_entry(idt, 0x98, int_98);
    init_idt_entry(idt, 0x99, int_99);
    init_idt_entry(idt, 0x9a, int_9a);
    init_idt_entry(idt, 0x9b, int_9b);
    init_idt_entry(idt, 0x9c, int_9c);
    init_idt_entry(idt, 0x9d, int_9d);
    init_idt_entry(idt, 0x9e, int_9e);
    init_idt_entry(idt, 0x9f, int_9f);

    init_idt_entry(idt, 0xa0, int_a0);
    init_idt_entry(idt, 0xa1, int_a1);
    init_idt_entry(idt, 0xa2, int_a2);
    init_idt_entry(idt, 0xa3, int_a3);
    init_idt_entry(idt, 0xa4, int_a4);
    init_idt_entry(idt, 0xa5, int_a5);
    init_idt_entry(idt, 0xa6, int_a6);
    init_idt_entry(idt, 0xa7, int_a7);
    init_idt_entry(idt, 0xa8, int_a8);
    init_idt_entry(idt, 0xa9, int_a9);
    init_idt_entry(idt, 0xaa, int_aa);
    init_idt_entry(idt, 0xab, int_ab);
    init_idt_entry(idt, 0xac, int_ac);
    init_idt_entry(idt, 0xad, int_ad);
    init_idt_entry(idt, 0xae, int_ae);
    init_idt_entry(idt, 0xaf, int_af);

    init_idt_entry(idt, 0xb0, int_b0);
    init_idt_entry(idt, 0xb1, int_b1);
    init_idt_entry(idt, 0xb2, int_b2);
    init_idt_entry(idt, 0xb3, int_b3);
    init_idt_entry(idt, 0xb4, int_b4);
    init_idt_entry(idt, 0xb5, int_b5);
    init_idt_entry(idt, 0xb6, int_b6);
    init_idt_entry(idt, 0xb7, int_b7);
    init_idt_entry(idt, 0xb8, int_b8);
    init_idt_entry(idt, 0xb9, int_b9);
    init_idt_entry(idt, 0xba, int_ba);
    init_idt_entry(idt, 0xbb, int_bb);
    init_idt_entry(idt, 0xbc, int_bc);
    init_idt_entry(idt, 0xbd, int_bd);
    init_idt_entry(idt, 0xbe, int_be);
    init_idt_entry(idt, 0xbf, int_bf);

    init_idt_entry(idt, 0xc0, int_c0);
    init_idt_entry(idt, 0xc1, int_c1);
    init_idt_entry(idt, 0xc2, int_c2);
    init_idt_entry(idt, 0xc3, int_c3);
    init_idt_entry(idt, 0xc4, int_c4);
    init_idt_entry(idt, 0xc5, int_c5);
    init_idt_entry(idt, 0xc6, int_c6);
    init_idt_entry(idt, 0xc7, int_c7);
    init_idt_entry(idt, 0xc8, int_c8);
    init_idt_entry(idt, 0xc9, int_c9);
    init_idt_entry(idt, 0xca, int_ca);
    init_idt_entry(idt, 0xcb, int_cb);
    init_idt_entry(idt, 0xcc, int_cc);
    init_idt_entry(idt, 0xcd, int_cd);
    init_idt_entry(idt, 0xce, int_ce);
    init_idt_entry(idt, 0xcf, int_cf);

    init_idt_entry(idt, 0xd0, int_d0);
    init_idt_entry(idt, 0xd1, int_d1);
    init_idt_entry(idt, 0xd2, int_d2);
    init_idt_entry(idt, 0xd3, int_d3);
    init_idt_entry(idt, 0xd4, int_d4);
    init_idt_entry(idt, 0xd5, int_d5);
    init_idt_entry(idt, 0xd6, int_d6);
    init_idt_entry(idt, 0xd7, int_d7);
    init_idt_entry(idt, 0xd8, int_d8);
    init_idt_entry(idt, 0xd9, int_d9);
    init_idt_entry(idt, 0xda, int_da);
    init_idt_entry(idt, 0xdb, int_db);
    init_idt_entry(idt, 0xdc, int_dc);
    init_idt_entry(idt, 0xdd, int_dd);
    init_idt_entry(idt, 0xde, int_de);
    init_idt_entry(idt, 0xdf, int_df);

    init_idt_entry(idt, 0xe0, int_e0);
    init_idt_entry(idt, 0xe1, int_e1);
    init_idt_entry(idt, 0xe2, int_e2);
    init_idt_entry(idt, 0xe3, int_e3);
    init_idt_entry(idt, 0xe4, int_e4);
    init_idt_entry(idt, 0xe5, int_e5);
    init_idt_entry(idt, 0xe6, int_e6);
    init_idt_entry(idt, 0xe7, int_e7);
    init_idt_entry(idt, 0xe8, int_e8);
    init_idt_entry(idt, 0xe9, int_e9);
    init_idt_entry(idt, 0xea, int_ea);
    init_idt_entry(idt, 0xeb, int_eb);
    init_idt_entry(idt, 0xec, int_ec);
    init_idt_entry(idt, 0xed, int_ed);
    init_idt_entry(idt, 0xee, int_ee);
    init_idt_entry(idt, 0xef, int_ef);

    init_idt_entry(idt, 0xf0, int_f0);
    init_idt_entry(idt, 0xf1, int_f1);
    init_idt_entry(idt, 0xf2, int_f2);
    init_idt_entry(idt, 0xf3, int_f3);
    init_idt_entry(idt, 0xf4, int_f4);
    init_idt_entry(idt, 0xf5, int_f5);
    init_idt_entry(idt, 0xf6, int_f6);
    init_idt_entry(idt, 0xf7, int_f7);
    init_idt_entry(idt, 0xf8, int_f8);
    init_idt_entry(idt, 0xf9, int_f9);
    init_idt_entry(idt, 0xfa, int_fa);
    init_idt_entry(idt, 0xfb, int_fb);
    init_idt_entry(idt, 0xfc, int_fc);
    init_idt_entry(idt, 0xfd, int_fd);
    init_idt_entry(idt, 0xfe, int_fe);
    init_idt_entry(idt, 0xff, int_ff);
}

BOOT_CODE bool_t init_vm_state(வெறுமை)
{
    word_t cache_வரி_Size;
    x86_KScache_வரி_Size_Bits = get_Cache_வரி_Size_Bits();
    எனில் (!x86_KScache_வரி_Size_Bits) {
        திரும்பு false;
    }

    cache_வரி_Size = BIT(x86_KScache_வரி_Size_Bits);
    எனில் (cache_வரி_Size != L1_CACHE_வரி_SIZE) {
        printf("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_வரி_SIZE, cache_வரி_Size);
        SMP_COND_STATEMENT(திரும்பு false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    வெறுமை *tss_ptr = &x86KSGlobalState[CURRENT_CPU_INDEX()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_INDEX()].x86KSidt);
    திரும்பு true;
}

BOOT_CODE bool_t init_pat_msr(வெறுமை)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    எனில் ((x86_cpuid_edx(0x1, 0x0) & BIT(16)) == 0) {
        printf("PAT support not found\n");
        திரும்பு false;
    }
    pat_msr.சொற்கள்[0] = x86_rdmsr_கீழ்(IA32_PAT_MSR);
    pat_msr.சொற்கள்[1] = x86_rdmsr_உயர்வு(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_set_pa0(pat_msr, IA32_PAT_MT_WRITE_BACK);
    pat_msr = x86_pat_msr_set_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_set_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_set_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_set_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.சொற்கள்[1]) << 32 | pat_msr.சொற்கள்[0]);
    திரும்பு true;
}

BOOT_CODE வெறுமை write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_of_cap(it_ap_cap));
    ap->array[ASID_கீழ்(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_of_cap(it_vspace_cap));
    x86KSASIDTable[ASID_உயர்வு(IT_ASID)] = ap;
}

asid_map_t findMapForASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_உயர்வு(asid)];
    எனில் (!poolPtr) {
        திரும்பு asid_map_asid_map_none_new();
    }

    திரும்பு poolPtr->array[ASID_கீழ்(asid)];
}

findVSpaceForASID_ret_t findVSpaceForASID(asid_t asid)
{
    findVSpaceForASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = findMapForASID(asid);
    எனில் (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_வேர்_new();

        ret.vspace_வேர் = வெற்று;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        திரும்பு ret;
    }

    ret.vspace_வேர் = (vspace_வேர்_t *)asid_map_asid_map_vspace_get_vspace_வேர்(asid_map);
    ret.status = EXCEPTION_NONE;
    திரும்பு ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    word_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = getRegister(thread, Error);

    தேர்வு (vm_faultType) {
    நிலை X86DataFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        திரும்பு EXCEPTION_FAULT;

    நிலை X86InstructionFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, true);
        திரும்பு EXCEPTION_FAULT;

    இயல்பு:
        fail("Invalid VM fault type");
    }
}

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights)
{
    தேர்வு (vm_rights) {
    நிலை VMReadOnly:
        திரும்பு 0;

    நிலை VMKernelOnly:
    நிலை VMReadWrite:
        திரும்பு 1;

    இயல்பு:
        fail("Invalid VM rights");
    }
}

uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights)
{
    தேர்வு (vm_rights) {
    நிலை VMKernelOnly:
        திரும்பு 0;

    நிலை VMReadOnly:
    நிலை VMReadWrite:
        திரும்பு 1;

    இயல்பு:
        fail("Invalid VM rights");
    }
}

lookupPTSlot_ret_t lookupPTSlot(vspace_வேர்_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pdSlot;

    pdSlot = lookupPDSlot(vspace, vptr);
    எனில் (pdSlot.status != EXCEPTION_NONE) {
        ret.ptSlot = வெற்று;
        ret.status = pdSlot.status;
        திரும்பு ret;
    }
    எனில் ((pde_ptr_get_page_size(pdSlot.pdSlot) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pdSlot.pdSlot)) {
        current_lookup_fault = lookup_fault_missing_capability_new(PAGE_BITS + PT_INDEX_BITS);
        ret.ptSlot = வெற்று;
        ret.status = EXCEPTION_LOOKUP_FAULT;
        திரும்பு ret;
    } இல்லையெனில் {
        pte_t *pt;
        pte_t *ptSlot;
        word_t ptIndex;

        pt = paddr_to_pptr(pde_pde_pt_ptr_get_pt_base_address(pdSlot.pdSlot));
        ptIndex = (vptr >> PAGE_BITS) & MASK(PT_INDEX_BITS);
        ptSlot = pt + ptIndex;

        ret.ptSlot = ptSlot;
        ret.status = EXCEPTION_NONE;
        திரும்பு ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    எனில் (cap_get_capType(cap) != cap_frame_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
    எனில் (unlikely(cap_frame_cap_get_capFIsDevice(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (!IS_ALIGNED(vptr, seL4_IPCBufferSizeBits)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (முழு)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    திரும்பு EXCEPTION_NONE;
}

vm_rights_t CONST maskVMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_mask)
{
    எனில் (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        திரும்பு VMReadOnly;
    }
    எனில் (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_mask)) {
        எனில் (!seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
            திரும்பு VMReadOnly;
        } இல்லையெனில் {
            திரும்பு VMReadWrite;
        }
    }
    எனில் (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_mask) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_mask)) {
        userError("Attempted to make unsupported write only mapping");
    }
    திரும்பு VMKernelOnly;
}

வெறுமை flushTable(vspace_வேர்_t *vspace, word_t vptr, pte_t *pt, asid_t asid)
{
    word_t i;
    cap_t        thread_வேர்;

    ப்ரதிஜ்ஞா(IS_ALIGNED(vptr, PT_INDEX_BITS + PAGE_BITS));

    /* check if page table belongs to current address space */
    thread_வேர் = TCB_PTR_CTE_PTR(முனை_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    க்கு (i = 0; i < BIT(PT_INDEX_BITS); i++) {
        எனில் (pte_get_present(pt[i])) {
            எனில் (config_set(CONFIG_SUPPORT_PCID) || (is_Valid_Native_வேர்(thread_வேர்)
                                                    && (vspace_வேர்_t *)pptr_of_cap(thread_வேர்) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << PAGE_BITS), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


வெறுமை unmapPage(vm_page_size_t page_size, asid_t asid, vptr_t vptr, வெறுமை *pptr)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    find_ret = findVSpaceForASID(asid);
    எனில் (find_ret.status != EXCEPTION_NONE) {
        திரும்பு;
    }

    தேர்வு (page_size) {
    நிலை X86_SmallPage:
        lu_ret = lookupPTSlot(find_ret.vspace_வேர், vptr);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            திரும்பு;
        }
        எனில் (!(pte_ptr_get_present(lu_ret.ptSlot)
              && (pte_ptr_get_page_base_address(lu_ret.ptSlot)
                  == pptr_to_paddr(pptr)))) {
            திரும்பு;
        }
        *lu_ret.ptSlot = makeUserPTEInvalid();
        நிறுத்து;

    நிலை X86_LargePage:
        pd_ret = lookupPDSlot(find_ret.vspace_வேர், vptr);
        எனில் (pd_ret.status != EXCEPTION_NONE) {
            திரும்பு;
        }
        pde = pd_ret.pdSlot;
        எனில் (!(pde_ptr_get_page_size(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_page_base_address(pde)
                  == pptr_to_paddr(pptr)))) {
            திரும்பு;
        }
        *pde = makeUserPDEInvalid();
        நிறுத்து;

    இயல்பு:
        எனில் (!modeUnmapPage(page_size, find_ret.vspace_வேர், vptr, pptr)) {
            திரும்பு;
        }
        நிறுத்து;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(find_ret.vspace_வேர்), 0));
}

வெறுமை unmapPageTable(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    findVSpaceForASID_ret_t find_ret;
    lookupPDSlot_ret_t    lu_ret;

    find_ret = findVSpaceForASID(asid);
    எனில் (find_ret.status != EXCEPTION_NONE) {
        திரும்பு;
    }

    lu_ret = lookupPDSlot(find_ret.vspace_வேர், vaddr);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        திரும்பு;
    }

    /* check if the PD actually refers to the PT */
    எனில் (!(pde_ptr_get_page_size(lu_ret.pdSlot) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pdSlot) &&
          (pde_pde_pt_ptr_get_pt_base_address(lu_ret.pdSlot) == pptr_to_paddr(pt)))) {
        திரும்பு;
    }

    flushTable(find_ret.vspace_வேர், vaddr, pt, asid);

    *lu_ret.pdSlot = makeUserPDEInvalid();

    invalidatePageStructureCacheASID(pptr_to_paddr(find_ret.vspace_வேர்), asid,
                                     SMP_TERNARY(tlb_bitmap_get(find_ret.vspace_வேர்), 0));
}

நிலையான exception_t performX86PageInvocationMapPTE(cap_t cap, cte_t *ctSlot, pte_t *ptSlot, pte_t pte,
                                                  vspace_வேர்_t *vspace)
{
    ctSlot->cap = cap;
    *ptSlot = pte;
    invalidatePageStructureCacheASID(pptr_to_paddr(vspace), cap_frame_cap_get_capFMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t performX86PageInvocationMapPDE(cap_t cap, cte_t *ctSlot, pde_t *pdSlot, pde_t pde,
                                                  vspace_வேர்_t *vspace)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    invalidatePageStructureCacheASID(pptr_to_paddr(vspace), cap_frame_cap_get_capFMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    திரும்பு EXCEPTION_NONE;
}


நிலையான exception_t performX86PageInvocationUnmap(cap_t cap, cte_t *ctSlot)
{
    ப்ரதிஜ்ஞா(cap_frame_cap_get_capFMappedASID(cap));
    ப்ரதிஜ்ஞா(cap_frame_cap_get_capFMapType(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    எனில் (cap_frame_cap_get_capFMappedASID(cap)) {
        unmapPage(
            cap_frame_cap_get_capFSize(cap),
            cap_frame_cap_get_capFMappedASID(cap),
            cap_frame_cap_get_capFMappedAddress(cap),
            (வெறுமை *)cap_frame_cap_get_capFBasePtr(cap)
        );
    }

    cap_frame_cap_ptr_set_capFMappedAddress(&ctSlot->cap, 0);
    cap_frame_cap_ptr_set_capFMappedASID(&ctSlot->cap, asidInvalid);
    cap_frame_cap_ptr_set_capFMapType(&ctSlot->cap, X86_MappingNone);

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t performX86FrameInvocationUnmap(cap_t cap, cte_t *cte)
{
    எனில் (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
        தேர்வு (cap_frame_cap_get_capFMapType(cap)) {
        நிலை X86_MappingVSpace:
            திரும்பு performX86PageInvocationUnmap(cap, cte);
#ifdef CONFIG_IOMMU
        நிலை X86_MappingIOSpace:
            திரும்பு performX86IOUnMapInvocation(cap, cte);
#endif
#ifdef CONFIG_VTX
        நிலை X86_MappingEPT:
            திரும்பு performX86EPTPageInvocationUnmap(cap, cte);
#endif
        நிலை X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            நிறுத்து;
        }
    }

    திரும்பு EXCEPTION_NONE;
}

கட்டமைப்பு create_mapping_pte_return {
    exception_t status;
    pte_t pte;
    pte_t *ptSlot;
};
வகைவரை கட்டமைப்பு create_mapping_pte_return create_mapping_pte_return_t;

நிலையான create_mapping_pte_return_t createSafeMappingEntries_PTE(paddr_t base, word_t vaddr, vm_rights_t vmRights,
                                                                vm_attributes_t attr,
                                                                vspace_வேர்_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        திரும்பு ret;
    }

    ret.pte = makeUserPTE(base, attr, vmRights);
    ret.ptSlot = lu_ret.ptSlot;
    ret.status = EXCEPTION_NONE;
    திரும்பு ret;
}

கட்டமைப்பு create_mapping_pde_return {
    exception_t status;
    pde_t pde;
    pde_t *pdSlot;
};
வகைவரை கட்டமைப்பு create_mapping_pde_return create_mapping_pde_return_t;

நிலையான create_mapping_pde_return_t createSafeMappingEntries_PDE(paddr_t base, word_t vaddr, vm_rights_t vmRights,
                                                                vm_attributes_t attr,
                                                                vspace_வேர்_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    எனில் (lu_ret.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        திரும்பு ret;
    }
    ret.pdSlot = lu_ret.pdSlot;

    /* check for existing page table */
    எனில் ((pde_ptr_get_page_size(ret.pdSlot) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pdSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;
        ret.status = EXCEPTION_SYSCALL_ERROR;
        திரும்பு ret;
    }


    ret.pde = makeUserPDELargePage(base, attr, vmRights);
    ret.status = EXCEPTION_NONE;
    திரும்பு ret;
}


exception_t decodeX86FrameInvocation(
    word_t invLabel,
    word_t length,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    தேர்வு (invLabel) {
    நிலை X86PageMap: { /* Map */
        word_t          vaddr;
        word_t          vtop;
        word_t          w_rightsMask;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_வேர்_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_attributes_t vmAttr;
        vm_page_size_t  frameSize;
        asid_t          asid;

        எனில் (length < 3 || current_extra_caps.excaprefs[0] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        frameSize = cap_frame_cap_get_capFSize(cap);
        vaddr = getSyscallArg(0, buffer);
        w_rightsMask = getSyscallArg(1, buffer);
        vmAttr = vmAttributesFromWord(getSyscallArg(2, buffer));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_frame_cap_get_capFVMRights(cap);

        எனில் (!is_Valid_Native_வேர்(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 1;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_வேர்_t *)pptr_of_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        எனில் (cap_frame_cap_get_capFMappedASID(cap) != asidInvalid) {
            எனில் (cap_frame_cap_get_capFMappedASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_எண் = 1;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            எனில் (cap_frame_cap_get_capFMapType(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            எனில் (cap_frame_cap_get_capFMappedAddress(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_எண் = 0;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }
        } இல்லையெனில் {
            vtop = vaddr + BIT(pageBitsForSize(frameSize));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            எனில் (vaddr > USER_உச்சி || vtop > USER_உச்சி) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = seL4_InvalidArgument;
                current_syscall_error.invalid_Argument_எண் = 0;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            findVSpaceForASID_ret_t find_ret;

            find_ret = findVSpaceForASID(asid);
            எனில் (find_ret.status != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failedLookupWasSource = false;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }

            எனில் (find_ret.vspace_வேர் != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_எண் = 1;

                திரும்பு EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = maskVMRights(capVMRights, rightsFromWord(w_rightsMask));

        எனில் (!checkVPAlignment(frameSize, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_to_paddr((வெறுமை *)cap_frame_cap_get_capFBasePtr(cap));

        cap = cap_frame_cap_set_capFMappedASID(cap, asid);
        cap = cap_frame_cap_set_capFMappedAddress(cap, vaddr);
        cap = cap_frame_cap_set_capFMapType(cap, X86_MappingVSpace);

        தேர்வு (frameSize) {
        /* PTE mappings */
        நிலை X86_SmallPage: {
            create_mapping_pte_return_t map_ret;

            map_ret = createSafeMappingEntries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            எனில் (map_ret.status != EXCEPTION_NONE) {
                திரும்பு map_ret.status;
            }

            setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
            திரும்பு performX86PageInvocationMapPTE(cap, cte, map_ret.ptSlot, map_ret.pte, vspace);
        }

        /* PDE mappings */
        நிலை X86_LargePage: {
            create_mapping_pde_return_t map_ret;

            map_ret = createSafeMappingEntries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            எனில் (map_ret.status != EXCEPTION_NONE) {
                திரும்பு map_ret.status;
            }

            setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
            திரும்பு performX86PageInvocationMapPDE(cap, cte, map_ret.pdSlot, map_ret.pde, vspace);
        }

        இயல்பு: {
            திரும்பு decodeX86ModeMapPage(invLabel, frameSize, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    நிலை X86PageUnmap: { /* Unmap */
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performX86FrameInvocationUnmap(cap, cte);
    }

#ifdef CONFIG_IOMMU
    நிலை X86PageMapIO: { /* MapIO */
        திரும்பு decodeX86IOMapInvocation(length, cte, cap, buffer);
    }
#endif

#ifdef CONFIG_VTX
    நிலை X86PageMapEPT:
        திரும்பு decodeX86EPTPageMap(invLabel, length, cte, cap, buffer);
#endif

    நிலை X86PageGetAddress: {
        /* Return it in the first message register. */
        ப்ரதிஜ்ஞா(n_செய்தி_Registers >= 1);

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performPageGetAddress((வெறுமை *)cap_frame_cap_get_capFBasePtr(cap), call);
    }

    இயல்பு:
        current_syscall_error.type = seL4_IllegalOperation;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }
}

நிலையான exception_t performX86PageTableInvocationUnmap(cap_t cap, cte_t *ctSlot)
{

    எனில் (cap_page_table_cap_get_capPTIsMapped(cap)) {
        pte_t *pt = PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap));
        unmapPageTable(
            cap_page_table_cap_get_capPTMappedASID(cap),
            cap_page_table_cap_get_capPTMappedAddress(cap),
            pt
        );
        clearMemory((வெறுமை *)pt, cap_get_capSizeBits(cap));
    }
    cap_page_table_cap_ptr_set_capPTIsMapped(&(ctSlot->cap), 0);

    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t performX86PageTableInvocationMap(cap_t cap, cte_t *ctSlot, pde_t pde, pde_t *pdSlot,
                                                    vspace_வேர்_t *வேர்)
{
    ctSlot->cap = cap;
    *pdSlot = pde;
    invalidatePageStructureCacheASID(pptr_to_paddr(வேர்), cap_page_table_cap_get_capPTMappedASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(வேர்), 0));
    திரும்பு EXCEPTION_NONE;
}

நிலையான exception_t decodeX86PageTableInvocation(
    word_t invLabel,
    word_t length,
    cte_t *cte, cap_t cap,
    word_t *buffer
)
{
    word_t          vaddr;
    vm_attributes_t attr;
    lookupPDSlot_ret_t pdSlot;
    cap_t           vspaceCap;
    vspace_வேர்_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    எனில் (invLabel == X86PageTableUnmap) {
        எனில் (! isFinalCapability(cte)) {
            current_syscall_error.type = seL4_RevokeFirst;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performX86PageTableInvocationUnmap(cap, cte);
    }

    எனில் (invLabel != X86PageTableMap) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (length < 2 || current_extra_caps.excaprefs[0] == வெற்று) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = seL4_TruncatedMessage;
        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (cap_page_table_cap_get_capPTIsMapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 0;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, buffer) & (~MASK(PT_INDEX_BITS + PAGE_BITS));
    attr = vmAttributesFromWord(getSyscallArg(1, buffer));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    எனில் (!is_Valid_Native_வேர்(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_எண் = 1;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_வேர்_t *)pptr_of_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    எனில் (vaddr > USER_உச்சி) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = seL4_InvalidArgument;
        current_syscall_error.invalid_Argument_எண் = 0;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    {
        findVSpaceForASID_ret_t find_ret;

        find_ret = findVSpaceForASID(asid);
        எனில் (find_ret.status != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (find_ret.vspace_வேர் != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 1;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
    }

    pdSlot = lookupPDSlot(vspace, vaddr);
    எனில் (pdSlot.status != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failedLookupWasSource = false;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    எனில் (((pde_ptr_get_page_size(pdSlot.pdSlot) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pdSlot.pdSlot)) ||
        ((pde_ptr_get_page_size(pdSlot.pdSlot) == pde_pde_large) && pde_pde_large_ptr_get_present(pdSlot.pdSlot))) {
        current_syscall_error.type = seL4_DeleteFirst;

        திரும்பு EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_to_paddr(PTE_PTR(cap_page_table_cap_get_capPTBasePtr(cap)));
    pde = makeUserPDEPageTable(paddr, attr);

    cap = cap_page_table_cap_set_capPTIsMapped(cap, 1);
    cap = cap_page_table_cap_set_capPTMappedASID(cap, asid);
    cap = cap_page_table_cap_set_capPTMappedAddress(cap, vaddr);

    setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
    திரும்பு performX86PageTableInvocationMap(cap, cte, pde, pdSlot.pdSlot, vspace);
}

exception_t decodeX86MMUInvocation(
    word_t invLabel,
    word_t length,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    word_t *buffer
)
{
    தேர்வு (cap_get_capType(cap)) {

    நிலை cap_frame_cap:
        திரும்பு decodeX86FrameInvocation(invLabel, length, cte, cap, call, buffer);

    நிலை cap_page_table_cap:
        திரும்பு decodeX86PageTableInvocation(invLabel, length, cte, cap, buffer);

    நிலை cap_asid_control_cap: {
        word_t     i;
        asid_t           asid_base;
        word_t           index;
        word_t           depth;
        cap_t            untyped;
        cap_t            வேர்;
        cte_t           *parentSlot;
        cte_t           *destSlot;
        lookupSlot_ret_t lu_ret;
        வெறுமை            *frame;
        exception_t      status;

        எனில் (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (length < 2 || current_extra_caps.excaprefs[0] == வெற்று
            || current_extra_caps.excaprefs[1] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        index = getSyscallArg(0, buffer);
        depth = getSyscallArg(1, buffer);
        parentSlot = current_extra_caps.excaprefs[0];
        untyped = parentSlot->cap;
        வேர் = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        க்கு (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        எனில் (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = seL4_DeleteFirst;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        asid_base = i << asid_கீழ்_Bits;


        எனில் (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_capBlockSize(untyped) != seL4_ASIDPoolBits ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 1;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        status = ensureNoChildren(parentSlot);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        frame = WORD_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookupTargetSlot(வேர், index, depth);
        எனில் (lu_ret.status != EXCEPTION_NONE) {
            திரும்பு lu_ret.status;
        }
        destSlot = lu_ret.slot;

        status = ensureEmptySlot(destSlot);
        எனில் (status != EXCEPTION_NONE) {
            திரும்பு status;
        }

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performASIDControlInvocation(frame, destSlot, parentSlot, asid_base);
    }

    நிலை cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspaceCapSlot;
        asid_pool_t *pool;
        word_t i;
        asid_t       asid;

        எனில் (invLabel != X86ASIDPoolAssign) {
            current_syscall_error.type = seL4_IllegalOperation;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }
        எனில் (current_extra_caps.excaprefs[0] == வெற்று) {
            current_syscall_error.type = seL4_TruncatedMessage;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        vspaceCapSlot = current_extra_caps.excaprefs[0];
        vspaceCap = vspaceCapSlot->cap;

        எனில் (!(is_VTable_வேர்(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 1;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_உயர்வு(cap_asid_pool_cap_get_capASIDBase(cap))];
        எனில் (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failedLookupWasSource = false;
            current_lookup_fault = lookup_fault_invalid_வேர்_new();
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        எனில் (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_எண் = 0;
            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        க்கு (i = 0; i < BIT(asid_கீழ்_Bits) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        எனில் (i == BIT(asid_கீழ்_Bits)) {
            current_syscall_error.type = seL4_DeleteFirst;

            திரும்பு EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        setThreadState(முனை_STATE(ksCurThread), ThreadState_Restart);
        திரும்பு performASIDPoolInvocation(asid, pool, vspaceCapSlot);
    }

    இயல்பு:
        திரும்பு decodeX86ModeMMUInvocation(invLabel, length, cptr, cte, cap, call, buffer);
    }
}
