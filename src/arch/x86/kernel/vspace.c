/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <api/syscall.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <arch/api/invocation.h>
#అన్తర్భావ <arch/kernel/tlb_bitmap.h>
#అన్తర్భావ <mode/kernel/tlb.h>
#అన్తర్భావ <mode/kernel/vspace.h>

స్థిర exception_t perform_పుట_Get_చిరునామా(శూన్య *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = కణుపు_STATE(ksCurThread);
    ఒకవేళ (call) {
        మాట_t *ipc_బఫరు = lookupIPCBuffer(true, thread);
        సమితి_రిజిస్టరు(thread, badge_రిజిస్టరు, 0);
        గుర్తులేని పూర్ణాంకం నిడివి = సమితి_MR(thread, ipc_బఫరు, 0, capFBasePtr);
        సమితి_రిజిస్టరు(thread, సందేశం_Info_రిజిస్టరు, మాట_From_సందేశం_Info(
                        se_L4_సందేశం_Info_new(0, 0, 0, నిడివి)));
    }
    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Running);
    తిరిగి EXCEPTION_NONE;
}

శూన్య deleteASIDPool(asid_t asid_పునాది, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా(IS_ALIGNED(asid_పునాది, asid_దిగువ_బిట్లు));

    ఒకవేళ (x86KSASIDTable[ASID_ఎగువ(asid_పునాది)] == pool) {
        కొరకు (గుర్తులేని పూర్ణాంకం స్థానభ్రంశం = 0; స్థానభ్రంశం < బిట్టు(asid_దిగువ_బిట్లు); స్థానభ్రంశం++) {
            asid_map_t asid_map = pool->array[స్థానభ్రంశం];
            ఒకవేళ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_మూలం_t *vspace = (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map);
                hwASIDInvalidate(asid_పునాది + స్థానభ్రంశం, vspace);
            }
        }
        x86KSASIDTable[ASID_ఎగువ(asid_పునాది)] = ఖాళీ;
        సమితి_VMRoot(కణుపు_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(శూన్య *చట్రం, cte_t *స్థానం, cte_t *మాతృ, asid_t asid_పునాది)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_సమితి_cap_Free_సూచీ(&(మాతృ->cap),
                                         MAX_FREE_సూచీ(cap_untyped_cap_get_cap_ఖండం_కొలత(మాతృ->cap)));

    memzero(చట్రం, బిట్టు(పుట_బిట్లు_For_కొలత(X86_Small_పుట)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_చొప్పించు(
        cap_asid_pool_cap_new(
            asid_పునాది,          /* capASIDBase  */
            మాట_REF(చట్రం)     /* capASIDPool  */
        ),
        మాతృ,
        స్థానం
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ప్రతిజ్ఞా((asid_పునాది & ముసుగు(asid_దిగువ_బిట్లు)) == 0);
    x86KSASIDTable[ASID_ఎగువ(asid_పునాది)] = (asid_pool_t *)చట్రం;

    తిరిగి EXCEPTION_NONE;
}

శూన్య deleteASID(asid_t asid, vspace_మూలం_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (poolPtr != ఖాళీ) {
        asid_map_t asid_map = poolPtr->array[ASID_దిగువ(asid)];
        ఒకవేళ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_దిగువ(asid)] = asid_map_asid_map_none_new();
            సమితి_VMRoot(కణుపు_STATE(ksCurThread));
        }
    }
}

మాట_t *శుద్ధ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    మాట_t      w_బఫరు_Ptr;
    cap_t       బఫరు_Cap;
    vm_rights_t vm_rights;

    w_బఫరు_Ptr = thread->tcbIPCBuffer;
    బఫరు_Cap = TCB_PTR_CTE_PTR(thread, tcb_బఫరు)->cap;

    ఒకవేళ (cap_get_capType(బఫరు_Cap) != cap_చట్రం_cap) {
        తిరిగి ఖాళీ;
    }
    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(బఫరు_Cap))) {
        తిరిగి ఖాళీ;
    }

    vm_rights = cap_చట్రం_cap_get_cap_FVMRights(బఫరు_Cap);
    ఒకవేళ (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        మాట_t పునాది_Ptr, పుట_బిట్లు_2;

        పునాది_Ptr = cap_చట్రం_cap_get_cap_FBase_Ptr(బఫరు_Cap);
        పుట_బిట్లు_2 = పుట_బిట్లు_For_కొలత(cap_చట్రం_cap_get_cap_FSize(బఫరు_Cap));
        తిరిగి (మాట_t *)(పునాది_Ptr + (w_బఫరు_Ptr & ముసుగు(పుట_బిట్లు_2)));
    } లేకపోతే {
        తిరిగి ఖాళీ;
    }
}

bool_t CONST is_Valid_VTable_మూలం(cap_t cap)
{
    తిరిగి is_Valid_Native_మూలం(cap);
}


BOOT_సంకేతం bool_t map_kernel_కిటికీ_devices(pte_t *pt, uint32_t అంకె_ioapic, paddr_t *ioapic_paddrs, uint32_t అంకె_drhu,
                                           paddr_t *drhu_list)
{
    మాట_t idx = (KDEV_పునాది & ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు;
    paddr_t phys;
    pte_t pte;
    గుర్తులేని పూర్ణాంకం i;
    /* map kernel devices: APIC */
    phys = apic_get_పునాది_paddr();
    ఒకవేళ (!phys) {
        తిరిగి false;
    }
    ఒకవేళ (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        తిరిగి false;
    }
    pte = x86_make_device_pte(phys);

    ప్రతిజ్ఞా(idx == (PPTR_APIC & ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు);
    pt[idx] = pte;
    idx++;
    కొరకు (i = 0; i < అంకె_ioapic; i++) {
        phys = ioapic_paddrs[i];
        ఒకవేళ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            తిరిగి false;
        }
        pte = x86_make_device_pte(phys);
        ప్రతిజ్ఞా(idx == ((PPTR_IOAPIC_ప్రారంభం + i * బిట్టు(పుట_బిట్లు)) & ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు);
        pt[idx] = pte;
        idx++;
        ఒకవేళ (idx == బిట్టు(PT_సూచీ_బిట్లు)) {
            తిరిగి false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    కొరకు (; i < CONFIG_MAX_అంకె_IOAPIC; i++) {
        pte = x86_make_ఖాళీలు_pte();
        ప్రతిజ్ఞా(idx == ((PPTR_IOAPIC_ప్రారంభం + i * బిట్టు(పుట_బిట్లు)) & ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    కొరకు (i = 0; i < అంకె_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        ఒకవేళ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            తిరిగి false;
        }
        pte = x86_make_device_pte(phys);

        ప్రతిజ్ఞా(idx == ((PPTR_DRHU_ప్రారంభం + i * బిట్టు(పుట_బిట్లు)) & ముసుగు(LARGE_పుట_బిట్లు)) >> పుట_బిట్లు);
        pt[idx] = pte;
        idx++;
        ఒకవేళ (idx == బిట్టు(PT_సూచీ_బిట్లు)) {
            తిరిగి false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    అయితే (idx < బిట్టు(PT_సూచీ_బిట్లు)) {
        pte = x86_make_ఖాళీలు_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    ప్రతిజ్ఞా(idx == బిట్టు(PT_సూచీ_బిట్లు));
    తిరిగి true;
}

BOOT_సంకేతం స్థిర శూన్య init_idt(idt_నమోదు_t *idt)
{
    init_idt_నమోదు(idt, 0x00, int_00);
    init_idt_నమోదు(idt, 0x01, int_01);
    init_idt_నమోదు(idt, 0x02, int_02);
    init_idt_నమోదు(idt, 0x03, int_03);
    init_idt_నమోదు(idt, 0x04, int_04);
    init_idt_నమోదు(idt, 0x05, int_05);
    init_idt_నమోదు(idt, 0x06, int_06);
    init_idt_నమోదు(idt, 0x07, int_07);
    init_idt_నమోదు(idt, 0x08, int_08);
    init_idt_నమోదు(idt, 0x09, int_09);
    init_idt_నమోదు(idt, 0x0a, int_0a);
    init_idt_నమోదు(idt, 0x0b, int_0b);
    init_idt_నమోదు(idt, 0x0c, int_0c);
    init_idt_నమోదు(idt, 0x0d, int_0d);
    init_idt_నమోదు(idt, 0x0e, int_0e);
    init_idt_నమోదు(idt, 0x0f, int_0f);

    init_idt_నమోదు(idt, 0x10, int_10);
    init_idt_నమోదు(idt, 0x11, int_11);
    init_idt_నమోదు(idt, 0x12, int_12);
    init_idt_నమోదు(idt, 0x13, int_13);
    init_idt_నమోదు(idt, 0x14, int_14);
    init_idt_నమోదు(idt, 0x15, int_15);
    init_idt_నమోదు(idt, 0x16, int_16);
    init_idt_నమోదు(idt, 0x17, int_17);
    init_idt_నమోదు(idt, 0x18, int_18);
    init_idt_నమోదు(idt, 0x19, int_19);
    init_idt_నమోదు(idt, 0x1a, int_1a);
    init_idt_నమోదు(idt, 0x1b, int_1b);
    init_idt_నమోదు(idt, 0x1c, int_1c);
    init_idt_నమోదు(idt, 0x1d, int_1d);
    init_idt_నమోదు(idt, 0x1e, int_1e);
    init_idt_నమోదు(idt, 0x1f, int_1f);

    init_idt_నమోదు(idt, 0x20, int_20);
    init_idt_నమోదు(idt, 0x21, int_21);
    init_idt_నమోదు(idt, 0x22, int_22);
    init_idt_నమోదు(idt, 0x23, int_23);
    init_idt_నమోదు(idt, 0x24, int_24);
    init_idt_నమోదు(idt, 0x25, int_25);
    init_idt_నమోదు(idt, 0x26, int_26);
    init_idt_నమోదు(idt, 0x27, int_27);
    init_idt_నమోదు(idt, 0x28, int_28);
    init_idt_నమోదు(idt, 0x29, int_29);
    init_idt_నమోదు(idt, 0x2a, int_2a);
    init_idt_నమోదు(idt, 0x2b, int_2b);
    init_idt_నమోదు(idt, 0x2c, int_2c);
    init_idt_నమోదు(idt, 0x2d, int_2d);
    init_idt_నమోదు(idt, 0x2e, int_2e);
    init_idt_నమోదు(idt, 0x2f, int_2f);

    init_idt_నమోదు(idt, 0x30, int_30);
    init_idt_నమోదు(idt, 0x31, int_31);
    init_idt_నమోదు(idt, 0x32, int_32);
    init_idt_నమోదు(idt, 0x33, int_33);
    init_idt_నమోదు(idt, 0x34, int_34);
    init_idt_నమోదు(idt, 0x35, int_35);
    init_idt_నమోదు(idt, 0x36, int_36);
    init_idt_నమోదు(idt, 0x37, int_37);
    init_idt_నమోదు(idt, 0x38, int_38);
    init_idt_నమోదు(idt, 0x39, int_39);
    init_idt_నమోదు(idt, 0x3a, int_3a);
    init_idt_నమోదు(idt, 0x3b, int_3b);
    init_idt_నమోదు(idt, 0x3c, int_3c);
    init_idt_నమోదు(idt, 0x3d, int_3d);
    init_idt_నమోదు(idt, 0x3e, int_3e);
    init_idt_నమోదు(idt, 0x3f, int_3f);

    init_idt_నమోదు(idt, 0x40, int_40);
    init_idt_నమోదు(idt, 0x41, int_41);
    init_idt_నమోదు(idt, 0x42, int_42);
    init_idt_నమోదు(idt, 0x43, int_43);
    init_idt_నమోదు(idt, 0x44, int_44);
    init_idt_నమోదు(idt, 0x45, int_45);
    init_idt_నమోదు(idt, 0x46, int_46);
    init_idt_నమోదు(idt, 0x47, int_47);
    init_idt_నమోదు(idt, 0x48, int_48);
    init_idt_నమోదు(idt, 0x49, int_49);
    init_idt_నమోదు(idt, 0x4a, int_4a);
    init_idt_నమోదు(idt, 0x4b, int_4b);
    init_idt_నమోదు(idt, 0x4c, int_4c);
    init_idt_నమోదు(idt, 0x4d, int_4d);
    init_idt_నమోదు(idt, 0x4e, int_4e);
    init_idt_నమోదు(idt, 0x4f, int_4f);

    init_idt_నమోదు(idt, 0x50, int_50);
    init_idt_నమోదు(idt, 0x51, int_51);
    init_idt_నమోదు(idt, 0x52, int_52);
    init_idt_నమోదు(idt, 0x53, int_53);
    init_idt_నమోదు(idt, 0x54, int_54);
    init_idt_నమోదు(idt, 0x55, int_55);
    init_idt_నమోదు(idt, 0x56, int_56);
    init_idt_నమోదు(idt, 0x57, int_57);
    init_idt_నమోదు(idt, 0x58, int_58);
    init_idt_నమోదు(idt, 0x59, int_59);
    init_idt_నమోదు(idt, 0x5a, int_5a);
    init_idt_నమోదు(idt, 0x5b, int_5b);
    init_idt_నమోదు(idt, 0x5c, int_5c);
    init_idt_నమోదు(idt, 0x5d, int_5d);
    init_idt_నమోదు(idt, 0x5e, int_5e);
    init_idt_నమోదు(idt, 0x5f, int_5f);

    init_idt_నమోదు(idt, 0x60, int_60);
    init_idt_నమోదు(idt, 0x61, int_61);
    init_idt_నమోదు(idt, 0x62, int_62);
    init_idt_నమోదు(idt, 0x63, int_63);
    init_idt_నమోదు(idt, 0x64, int_64);
    init_idt_నమోదు(idt, 0x65, int_65);
    init_idt_నమోదు(idt, 0x66, int_66);
    init_idt_నమోదు(idt, 0x67, int_67);
    init_idt_నమోదు(idt, 0x68, int_68);
    init_idt_నమోదు(idt, 0x69, int_69);
    init_idt_నమోదు(idt, 0x6a, int_6a);
    init_idt_నమోదు(idt, 0x6b, int_6b);
    init_idt_నమోదు(idt, 0x6c, int_6c);
    init_idt_నమోదు(idt, 0x6d, int_6d);
    init_idt_నమోదు(idt, 0x6e, int_6e);
    init_idt_నమోదు(idt, 0x6f, int_6f);

    init_idt_నమోదు(idt, 0x70, int_70);
    init_idt_నమోదు(idt, 0x71, int_71);
    init_idt_నమోదు(idt, 0x72, int_72);
    init_idt_నమోదు(idt, 0x73, int_73);
    init_idt_నమోదు(idt, 0x74, int_74);
    init_idt_నమోదు(idt, 0x75, int_75);
    init_idt_నమోదు(idt, 0x76, int_76);
    init_idt_నమోదు(idt, 0x77, int_77);
    init_idt_నమోదు(idt, 0x78, int_78);
    init_idt_నమోదు(idt, 0x79, int_79);
    init_idt_నమోదు(idt, 0x7a, int_7a);
    init_idt_నమోదు(idt, 0x7b, int_7b);
    init_idt_నమోదు(idt, 0x7c, int_7c);
    init_idt_నమోదు(idt, 0x7d, int_7d);
    init_idt_నమోదు(idt, 0x7e, int_7e);
    init_idt_నమోదు(idt, 0x7f, int_7f);

    init_idt_నమోదు(idt, 0x80, int_80);
    init_idt_నమోదు(idt, 0x81, int_81);
    init_idt_నమోదు(idt, 0x82, int_82);
    init_idt_నమోదు(idt, 0x83, int_83);
    init_idt_నమోదు(idt, 0x84, int_84);
    init_idt_నమోదు(idt, 0x85, int_85);
    init_idt_నమోదు(idt, 0x86, int_86);
    init_idt_నమోదు(idt, 0x87, int_87);
    init_idt_నమోదు(idt, 0x88, int_88);
    init_idt_నమోదు(idt, 0x89, int_89);
    init_idt_నమోదు(idt, 0x8a, int_8a);
    init_idt_నమోదు(idt, 0x8b, int_8b);
    init_idt_నమోదు(idt, 0x8c, int_8c);
    init_idt_నమోదు(idt, 0x8d, int_8d);
    init_idt_నమోదు(idt, 0x8e, int_8e);
    init_idt_నమోదు(idt, 0x8f, int_8f);

    init_idt_నమోదు(idt, 0x90, int_90);
    init_idt_నమోదు(idt, 0x91, int_91);
    init_idt_నమోదు(idt, 0x92, int_92);
    init_idt_నమోదు(idt, 0x93, int_93);
    init_idt_నమోదు(idt, 0x94, int_94);
    init_idt_నమోదు(idt, 0x95, int_95);
    init_idt_నమోదు(idt, 0x96, int_96);
    init_idt_నమోదు(idt, 0x97, int_97);
    init_idt_నమోదు(idt, 0x98, int_98);
    init_idt_నమోదు(idt, 0x99, int_99);
    init_idt_నమోదు(idt, 0x9a, int_9a);
    init_idt_నమోదు(idt, 0x9b, int_9b);
    init_idt_నమోదు(idt, 0x9c, int_9c);
    init_idt_నమోదు(idt, 0x9d, int_9d);
    init_idt_నమోదు(idt, 0x9e, int_9e);
    init_idt_నమోదు(idt, 0x9f, int_9f);

    init_idt_నమోదు(idt, 0xa0, int_a0);
    init_idt_నమోదు(idt, 0xa1, int_a1);
    init_idt_నమోదు(idt, 0xa2, int_a2);
    init_idt_నమోదు(idt, 0xa3, int_a3);
    init_idt_నమోదు(idt, 0xa4, int_a4);
    init_idt_నమోదు(idt, 0xa5, int_a5);
    init_idt_నమోదు(idt, 0xa6, int_a6);
    init_idt_నమోదు(idt, 0xa7, int_a7);
    init_idt_నమోదు(idt, 0xa8, int_a8);
    init_idt_నమోదు(idt, 0xa9, int_a9);
    init_idt_నమోదు(idt, 0xaa, int_aa);
    init_idt_నమోదు(idt, 0xab, int_ab);
    init_idt_నమోదు(idt, 0xac, int_ac);
    init_idt_నమోదు(idt, 0xad, int_ad);
    init_idt_నమోదు(idt, 0xae, int_ae);
    init_idt_నమోదు(idt, 0xaf, int_af);

    init_idt_నమోదు(idt, 0xb0, int_b0);
    init_idt_నమోదు(idt, 0xb1, int_b1);
    init_idt_నమోదు(idt, 0xb2, int_b2);
    init_idt_నమోదు(idt, 0xb3, int_b3);
    init_idt_నమోదు(idt, 0xb4, int_b4);
    init_idt_నమోదు(idt, 0xb5, int_b5);
    init_idt_నమోదు(idt, 0xb6, int_b6);
    init_idt_నమోదు(idt, 0xb7, int_b7);
    init_idt_నమోదు(idt, 0xb8, int_b8);
    init_idt_నమోదు(idt, 0xb9, int_b9);
    init_idt_నమోదు(idt, 0xba, int_ba);
    init_idt_నమోదు(idt, 0xbb, int_bb);
    init_idt_నమోదు(idt, 0xbc, int_bc);
    init_idt_నమోదు(idt, 0xbd, int_bd);
    init_idt_నమోదు(idt, 0xbe, int_be);
    init_idt_నమోదు(idt, 0xbf, int_bf);

    init_idt_నమోదు(idt, 0xc0, int_c0);
    init_idt_నమోదు(idt, 0xc1, int_c1);
    init_idt_నమోదు(idt, 0xc2, int_c2);
    init_idt_నమోదు(idt, 0xc3, int_c3);
    init_idt_నమోదు(idt, 0xc4, int_c4);
    init_idt_నమోదు(idt, 0xc5, int_c5);
    init_idt_నమోదు(idt, 0xc6, int_c6);
    init_idt_నమోదు(idt, 0xc7, int_c7);
    init_idt_నమోదు(idt, 0xc8, int_c8);
    init_idt_నమోదు(idt, 0xc9, int_c9);
    init_idt_నమోదు(idt, 0xca, int_ca);
    init_idt_నమోదు(idt, 0xcb, int_cb);
    init_idt_నమోదు(idt, 0xcc, int_cc);
    init_idt_నమోదు(idt, 0xcd, int_cd);
    init_idt_నమోదు(idt, 0xce, int_ce);
    init_idt_నమోదు(idt, 0xcf, int_cf);

    init_idt_నమోదు(idt, 0xd0, int_d0);
    init_idt_నమోదు(idt, 0xd1, int_d1);
    init_idt_నమోదు(idt, 0xd2, int_d2);
    init_idt_నమోదు(idt, 0xd3, int_d3);
    init_idt_నమోదు(idt, 0xd4, int_d4);
    init_idt_నమోదు(idt, 0xd5, int_d5);
    init_idt_నమోదు(idt, 0xd6, int_d6);
    init_idt_నమోదు(idt, 0xd7, int_d7);
    init_idt_నమోదు(idt, 0xd8, int_d8);
    init_idt_నమోదు(idt, 0xd9, int_d9);
    init_idt_నమోదు(idt, 0xda, int_da);
    init_idt_నమోదు(idt, 0xdb, int_db);
    init_idt_నమోదు(idt, 0xdc, int_dc);
    init_idt_నమోదు(idt, 0xdd, int_dd);
    init_idt_నమోదు(idt, 0xde, int_de);
    init_idt_నమోదు(idt, 0xdf, int_df);

    init_idt_నమోదు(idt, 0xe0, int_e0);
    init_idt_నమోదు(idt, 0xe1, int_e1);
    init_idt_నమోదు(idt, 0xe2, int_e2);
    init_idt_నమోదు(idt, 0xe3, int_e3);
    init_idt_నమోదు(idt, 0xe4, int_e4);
    init_idt_నమోదు(idt, 0xe5, int_e5);
    init_idt_నమోదు(idt, 0xe6, int_e6);
    init_idt_నమోదు(idt, 0xe7, int_e7);
    init_idt_నమోదు(idt, 0xe8, int_e8);
    init_idt_నమోదు(idt, 0xe9, int_e9);
    init_idt_నమోదు(idt, 0xea, int_ea);
    init_idt_నమోదు(idt, 0xeb, int_eb);
    init_idt_నమోదు(idt, 0xec, int_ec);
    init_idt_నమోదు(idt, 0xed, int_ed);
    init_idt_నమోదు(idt, 0xee, int_ee);
    init_idt_నమోదు(idt, 0xef, int_ef);

    init_idt_నమోదు(idt, 0xf0, int_f0);
    init_idt_నమోదు(idt, 0xf1, int_f1);
    init_idt_నమోదు(idt, 0xf2, int_f2);
    init_idt_నమోదు(idt, 0xf3, int_f3);
    init_idt_నమోదు(idt, 0xf4, int_f4);
    init_idt_నమోదు(idt, 0xf5, int_f5);
    init_idt_నమోదు(idt, 0xf6, int_f6);
    init_idt_నమోదు(idt, 0xf7, int_f7);
    init_idt_నమోదు(idt, 0xf8, int_f8);
    init_idt_నమోదు(idt, 0xf9, int_f9);
    init_idt_నమోదు(idt, 0xfa, int_fa);
    init_idt_నమోదు(idt, 0xfb, int_fb);
    init_idt_నమోదు(idt, 0xfc, int_fc);
    init_idt_నమోదు(idt, 0xfd, int_fd);
    init_idt_నమోదు(idt, 0xfe, int_fe);
    init_idt_నమోదు(idt, 0xff, int_ff);
}

BOOT_సంకేతం bool_t init_vm_state(శూన్య)
{
    మాట_t cache_పంక్తి_కొలత;
    x86_KScache_పంక్తి_కొలత_బిట్లు = get_Cache_పంక్తి_కొలత_బిట్లు();
    ఒకవేళ (!x86_KScache_పంక్తి_కొలత_బిట్లు) {
        తిరిగి false;
    }

    cache_పంక్తి_కొలత = బిట్టు(x86_KScache_పంక్తి_కొలత_బిట్లు);
    ఒకవేళ (cache_పంక్తి_కొలత != L1_CACHE_పంక్తి_కొలత) {
        రాయి("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_పంక్తి_కొలత, cache_పంక్తి_కొలత);
        SMP_COND_STATEMENT(తిరిగి false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    శూన్య *tss_ptr = &x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_సూచీ()].x86KSidt);
    తిరిగి true;
}

BOOT_సంకేతం bool_t init_pat_msr(శూన్య)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    ఒకవేళ ((x86_cpuid_edx(0x1, 0x0) & బిట్టు(16)) == 0) {
        రాయి("PAT support not found\n");
        తిరిగి false;
    }
    pat_msr.పదాలు[0] = x86_rdmsr_దిగువ(IA32_PAT_MSR);
    pat_msr.పదాలు[1] = x86_rdmsr_ఎగువ(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_సమితి_pa0(pat_msr, IA32_PAT_MT_WRITE_వెనుక);
    pat_msr = x86_pat_msr_సమితి_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_సమితి_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_సమితి_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_సమితి_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.పదాలు[1]) << 32 | pat_msr.పదాలు[0]);
    తిరిగి true;
}

BOOT_సంకేతం శూన్య write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_యొక్క_cap(it_ap_cap));
    ap->array[ASID_దిగువ(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_యొక్క_cap(it_vspace_cap));
    x86KSASIDTable[ASID_ఎగువ(IT_ASID)] = ap;
}

asid_map_t కనుగొను_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ఎగువ(asid)];
    ఒకవేళ (!poolPtr) {
        తిరిగి asid_map_asid_map_none_new();
    }

    తిరిగి poolPtr->array[ASID_దిగువ(asid)];
}

కనుగొను_VSpace_For_ASID_ret_t కనుగొను_VSpace_For_ASID(asid_t asid)
{
    కనుగొను_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = కనుగొను_Map_For_ASID(asid);
    ఒకవేళ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_మూలం_new();

        ret.vspace_మూలం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    }

    ret.vspace_మూలం = (vspace_మూలం_t *)asid_map_asid_map_vspace_get_vspace_మూలం(asid_map);
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    మాట_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_రిజిస్టరు(thread, Error);

    ఎంపిక (vm_faultType) {
    స్థితి X86_దత్తాంశం_Fault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        తిరిగి EXCEPTION_FAULT;

    స్థితి X86InstructionFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, true);
        తిరిగి EXCEPTION_FAULT;

    అప్రమేయ:
        fail("Invalid VM fault type");
    }
}

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMReadOnly:
        తిరిగి 0;

    స్థితి VMKernelOnly:
    స్థితి VMReadWrite:
        తిరిగి 1;

    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights)
{
    ఎంపిక (vm_rights) {
    స్థితి VMKernelOnly:
        తిరిగి 0;

    స్థితి VMReadOnly:
    స్థితి VMReadWrite:
        తిరిగి 1;

    అప్రమేయ:
        fail("Invalid VM rights");
    }
}

lookupPTSlot_ret_t lookupPTSlot(vspace_మూలం_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_స్థానం;

    pd_స్థానం = lookupPDSlot(vspace, vptr);
    ఒకవేళ (pd_స్థానం.స్థితి_2 != EXCEPTION_NONE) {
        ret.pt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = pd_స్థానం.స్థితి_2;
        తిరిగి ret;
    }
    ఒకవేళ ((pde_ptr_get_పుట_కొలత(pd_స్థానం.pd_స్థానం) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_స్థానం.pd_స్థానం)) {
        current_lookup_fault = lookup_fault_missing_capability_new(పుట_బిట్లు + PT_సూచీ_బిట్లు);
        ret.pt_స్థానం = ఖాళీ;
        ret.స్థితి_2 = EXCEPTION_LOOKUP_FAULT;
        తిరిగి ret;
    } లేకపోతే {
        pte_t *pt;
        pte_t *pt_స్థానం;
        మాట_t pt_సూచీ;

        pt = paddr_గమ్యం_pptr(pde_pde_pt_ptr_get_pt_పునాది_చిరునామా(pd_స్థానం.pd_స్థానం));
        pt_సూచీ = (vptr >> పుట_బిట్లు) & ముసుగు(PT_సూచీ_బిట్లు);
        pt_స్థానం = pt + pt_సూచీ;

        ret.pt_స్థానం = pt_స్థానం;
        ret.స్థితి_2 = EXCEPTION_NONE;
        తిరిగి ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ఒకవేళ (cap_get_capType(cap) != cap_చట్రం_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
    ఒకవేళ (unlikely(cap_చట్రం_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_కొలత_బిట్లు)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (పూర్ణాంకం)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    తిరిగి EXCEPTION_NONE;
}

vm_rights_t CONST ముసుగు_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ముసుగు)
{
    ఒకవేళ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        తిరిగి VMReadOnly;
    }
    ఒకవేళ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు)) {
        ఒకవేళ (!seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
            తిరిగి VMReadOnly;
        } లేకపోతే {
            తిరిగి VMReadWrite;
        }
    }
    ఒకవేళ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ముసుగు) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ముసుగు)) {
        userError("Attempted to make unsupported write only mapping");
    }
    తిరిగి VMKernelOnly;
}

శూన్య flush_పట్టిక(vspace_మూలం_t *vspace, మాట_t vptr, pte_t *pt, asid_t asid)
{
    మాట_t i;
    cap_t        thread_మూలం;

    ప్రతిజ్ఞా(IS_ALIGNED(vptr, PT_సూచీ_బిట్లు + పుట_బిట్లు));

    /* check if page table belongs to current address space */
    thread_మూలం = TCB_PTR_CTE_PTR(కణుపు_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    కొరకు (i = 0; i < బిట్టు(PT_సూచీ_బిట్లు); i++) {
        ఒకవేళ (pte_get_present(pt[i])) {
            ఒకవేళ (config_సమితి(CONFIG_SUPPORT_PCID) || (is_Valid_Native_మూలం(thread_మూలం)
                                                    && (vspace_మూలం_t *)pptr_యొక్క_cap(thread_మూలం) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << పుట_బిట్లు), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


శూన్య unmap_పుట(vm_పుట_కొలత_t పుట_కొలత, asid_t asid, vptr_t vptr, శూన్య *pptr)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    ఎంపిక (పుట_కొలత) {
    స్థితి X86_Small_పుట:
        lu_ret = lookupPTSlot(కనుగొను_ret.vspace_మూలం, vptr);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి;
        }
        ఒకవేళ (!(pte_ptr_get_present(lu_ret.pt_స్థానం)
              && (pte_ptr_get_పుట_పునాది_చిరునామా(lu_ret.pt_స్థానం)
                  == pptr_గమ్యం_paddr(pptr)))) {
            తిరిగి;
        }
        *lu_ret.pt_స్థానం = makeUserPTEInvalid();
        ఆపు;

    స్థితి X86_Large_పుట:
        pd_ret = lookupPDSlot(కనుగొను_ret.vspace_మూలం, vptr);
        ఒకవేళ (pd_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి;
        }
        pde = pd_ret.pd_స్థానం;
        ఒకవేళ (!(pde_ptr_get_పుట_కొలత(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_పుట_పునాది_చిరునామా(pde)
                  == pptr_గమ్యం_paddr(pptr)))) {
            తిరిగి;
        }
        *pde = makeUserPDEInvalid();
        ఆపు;

    అప్రమేయ:
        ఒకవేళ (!బహుళకం_Unmap_పుట(పుట_కొలత, కనుగొను_ret.vspace_మూలం, vptr, pptr)) {
            తిరిగి;
        }
        ఆపు;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(కనుగొను_ret.vspace_మూలం), 0));
}

శూన్య unmap_పుట_పట్టిక(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;
    lookupPDSlot_ret_t    lu_ret;

    కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
    ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    lu_ret = lookupPDSlot(కనుగొను_ret.vspace_మూలం, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        తిరిగి;
    }

    /* check if the PD actually refers to the PT */
    ఒకవేళ (!(pde_ptr_get_పుట_కొలత(lu_ret.pd_స్థానం) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_స్థానం) &&
          (pde_pde_pt_ptr_get_pt_పునాది_చిరునామా(lu_ret.pd_స్థానం) == pptr_గమ్యం_paddr(pt)))) {
        తిరిగి;
    }

    flush_పట్టిక(కనుగొను_ret.vspace_మూలం, vaddr, pt, asid);

    *lu_ret.pd_స్థానం = makeUserPDEInvalid();

    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(కనుగొను_ret.vspace_మూలం), asid,
                                     SMP_TERNARY(tlb_bitmap_get(కనుగొను_ret.vspace_మూలం), 0));
}

స్థిర exception_t perform_X86_పుట_Invocation_Map_PTE(cap_t cap, cte_t *ct_స్థానం, pte_t *pt_స్థానం, pte_t pte,
                                                  vspace_మూలం_t *vspace)
{
    ct_స్థానం->cap = cap;
    *pt_స్థానం = pte;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(vspace), cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X86_పుట_Invocation_Map_PDE(cap_t cap, cte_t *ct_స్థానం, pde_t *pd_స్థానం, pde_t pde,
                                                  vspace_మూలం_t *vspace)
{
    ct_స్థానం->cap = cap;
    *pd_స్థానం = pde;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(vspace), cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    తిరిగి EXCEPTION_NONE;
}


స్థిర exception_t perform_X86_పుట_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{
    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMapped_ASID(cap));
    ప్రతిజ్ఞా(cap_చట్రం_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap)) {
        unmap_పుట(
            cap_చట్రం_cap_get_cap_FSize(cap),
            cap_చట్రం_cap_get_cap_FMapped_ASID(cap),
            cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap),
            (శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_చట్రం_cap_ptr_సమితి_cap_FMapped_చిరునామా(&ct_స్థానం->cap, 0);
    cap_చట్రం_cap_ptr_సమితి_cap_FMapped_ASID(&ct_స్థానం->cap, asidInvalid);
    cap_చట్రం_cap_ptr_సమితి_cap_FMap_Type(&ct_స్థానం->cap, X86_MappingNone);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X86_చట్రం_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        ఎంపిక (cap_చట్రం_cap_get_cap_FMap_Type(cap)) {
        స్థితి X86_MappingVSpace:
            తిరిగి perform_X86_పుట_Invocation_Unmap(cap, cte);
#నిర్వచితమైతే CONFIG_IOMMU
        స్థితి X86_MappingIOSpace:
            తిరిగి performX86IOUnMapInvocation(cap, cte);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_VTX
        స్థితి X86_MappingEPT:
            తిరిగి performX86EPTPageInvocationUnmap(cap, cte);
#అయితే_ముగింపు
        స్థితి X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            ఆపు;
        }
    }

    తిరిగి EXCEPTION_NONE;
}

నిర్మాణం create_mapping_pte_return {
    exception_t స్థితి_2;
    pte_t pte;
    pte_t *pt_స్థానం;
};
రకనిర్వచనం నిర్మాణం create_mapping_pte_return create_mapping_pte_return_t;

స్థిర create_mapping_pte_return_t create_సురక్షిత_Mapping_Entries_PTE(paddr_t పునాది, మాట_t vaddr, vm_rights_t vmRights,
                                                                vm_లక్షణాలు_t attr,
                                                                vspace_మూలం_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        తిరిగి ret;
    }

    ret.pte = makeUserPTE(పునాది, attr, vmRights);
    ret.pt_స్థానం = lu_ret.pt_స్థానం;
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}

నిర్మాణం create_mapping_pde_return {
    exception_t స్థితి_2;
    pde_t pde;
    pde_t *pd_స్థానం;
};
రకనిర్వచనం నిర్మాణం create_mapping_pde_return create_mapping_pde_return_t;

స్థిర create_mapping_pde_return_t create_సురక్షిత_Mapping_Entries_PDE(paddr_t పునాది, మాట_t vaddr, vm_rights_t vmRights,
                                                                vm_లక్షణాలు_t attr,
                                                                vspace_మూలం_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        తిరిగి ret;
    }
    ret.pd_స్థానం = lu_ret.pd_స్థానం;

    /* check for existing page table */
    ఒకవేళ ((pde_ptr_get_పుట_కొలత(ret.pd_స్థానం) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_స్థానం))) {
        current_syscall_error.type = se_L4_Delete_మొదటి;
        ret.స్థితి_2 = EXCEPTION_SYSCALL_ERROR;
        తిరిగి ret;
    }


    ret.pde = make_User_PDELarge_పుట(పునాది, attr, vmRights);
    ret.స్థితి_2 = EXCEPTION_NONE;
    తిరిగి ret;
}


exception_t decode_X86_చట్రం_Invocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (invLabel) {
    స్థితి X86_పుట_Map: { /* Map */
        మాట_t          vaddr;
        మాట_t          vtop;
        మాట_t          w_rights_ముసుగు;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_మూలం_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_లక్షణాలు_t vmAttr;
        vm_పుట_కొలత_t  చట్రం_కొలత;
        asid_t          asid;

        ఒకవేళ (నిడివి < 3 || current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        చట్రం_కొలత = cap_చట్రం_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, బఫరు);
        w_rights_ముసుగు = getSyscallArg(1, బఫరు);
        vmAttr = vm_లక్షణాలు_From_మాట(getSyscallArg(2, బఫరు));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_చట్రం_cap_get_cap_FVMRights(cap);

        ఒకవేళ (!is_Valid_Native_మూలం(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_మూలం_t *)pptr_యొక్క_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            ఒకవేళ (cap_చట్రం_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            ఒకవేళ (cap_చట్రం_cap_get_cap_FMapped_చిరునామా(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        } లేకపోతే {
            vtop = vaddr + బిట్టు(పుట_బిట్లు_For_కొలత(చట్రం_కొలత));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            ఒకవేళ (vaddr > USER_శిఖరం || vtop > USER_శిఖరం) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_వాదం;
                current_syscall_error.invalid_వాదం_సంఖ్య = 0;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

            కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
            ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ఆధారం = false;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }

            ఒకవేళ (కనుగొను_ret.vspace_మూలం != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_సంఖ్య = 1;

                తిరిగి EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = ముసుగు_VMRights(capVMRights, rights_From_మాట(w_rights_ముసుగు));

        ఒకవేళ (!checkVPAlignment(చట్రం_కొలత, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_గమ్యం_paddr((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap));

        cap = cap_చట్రం_cap_సమితి_cap_FMapped_ASID(cap, asid);
        cap = cap_చట్రం_cap_సమితి_cap_FMapped_చిరునామా(cap, vaddr);
        cap = cap_చట్రం_cap_సమితి_cap_FMap_Type(cap, X86_MappingVSpace);

        ఎంపిక (చట్రం_కొలత) {
        /* PTE mappings */
        స్థితి X86_Small_పుట: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_సురక్షిత_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            ఒకవేళ (map_ret.స్థితి_2 != EXCEPTION_NONE) {
                తిరిగి map_ret.స్థితి_2;
            }

            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_X86_పుట_Invocation_Map_PTE(cap, cte, map_ret.pt_స్థానం, map_ret.pte, vspace);
        }

        /* PDE mappings */
        స్థితి X86_Large_పుట: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_సురక్షిత_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            ఒకవేళ (map_ret.స్థితి_2 != EXCEPTION_NONE) {
                తిరిగి map_ret.స్థితి_2;
            }

            సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
            తిరిగి perform_X86_పుట_Invocation_Map_PDE(cap, cte, map_ret.pd_స్థానం, map_ret.pde, vspace);
        }

        అప్రమేయ: {
            తిరిగి decode_X86_బహుళకం_Map_పుట(invLabel, చట్రం_కొలత, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    స్థితి X86_పుట_Unmap: { /* Unmap */
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_X86_చట్రం_Invocation_Unmap(cap, cte);
    }

#నిర్వచితమైతే CONFIG_IOMMU
    స్థితి X86_పుట_Map_IO: { /* MapIO */
        తిరిగి decodeX86IOMapInvocation(నిడివి, cte, cap, బఫరు);
    }
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_VTX
    స్థితి X86_పుట_Map_EPT:
        తిరిగి decodeX86EPTPageMap(invLabel, నిడివి, cte, cap, బఫరు);
#అయితే_ముగింపు

    స్థితి X86_పుట_Get_చిరునామా: {
        /* Return it in the first message register. */
        ప్రతిజ్ఞా(n_సందేశం_Registers >= 1);

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_పుట_Get_చిరునామా((శూన్య *)cap_చట్రం_cap_get_cap_FBase_Ptr(cap), call);
    }

    అప్రమేయ:
        current_syscall_error.type = seL4_IllegalOperation;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }
}

స్థిర exception_t perform_X86_పుట_పట్టిక_Invocation_Unmap(cap_t cap, cte_t *ct_స్థానం)
{

    ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap));
        unmap_పుట_పట్టిక(
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
            cap_పుట_పట్టిక_cap_get_cap_PTMapped_చిరునామా(cap),
            pt
        );
        clear_జ్ఞాపకం((శూన్య *)pt, cap_get_cap_కొలత_బిట్లు(cap));
    }
    cap_పుట_పట్టిక_cap_ptr_సమితి_cap_PTIs_Mapped(&(ct_స్థానం->cap), 0);

    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t perform_X86_పుట_పట్టిక_Invocation_Map(cap_t cap, cte_t *ct_స్థానం, pde_t pde, pde_t *pd_స్థానం,
                                                    vspace_మూలం_t *మూలం)
{
    ct_స్థానం->cap = cap;
    *pd_స్థానం = pde;
    invalidate_పుట_Structure_Cache_ASID(pptr_గమ్యం_paddr(మూలం), cap_పుట_పట్టిక_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(మూలం), 0));
    తిరిగి EXCEPTION_NONE;
}

స్థిర exception_t decode_X86_పుట_పట్టిక_Invocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cte_t *cte, cap_t cap,
    మాట_t *బఫరు
)
{
    మాట_t          vaddr;
    vm_లక్షణాలు_t attr;
    lookupPDSlot_ret_t pd_స్థానం;
    cap_t           vspaceCap;
    vspace_మూలం_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    ఒకవేళ (invLabel == X86_పుట_పట్టిక_Unmap) {
        ఒకవేళ (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_మొదటి;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి perform_X86_పుట_పట్టిక_Invocation_Unmap(cap, cte);
    }

    ఒకవేళ (invLabel != X86_పుట_పట్టిక_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_సందేశం;
        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (cap_పుట_పట్టిక_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, బఫరు) & (~ముసుగు(PT_సూచీ_బిట్లు + పుట_బిట్లు));
    attr = vm_లక్షణాలు_From_మాట(getSyscallArg(1, బఫరు));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ఒకవేళ (!is_Valid_Native_మూలం(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_సంఖ్య = 1;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_మూలం_t *)pptr_యొక్క_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ఒకవేళ (vaddr > USER_శిఖరం) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_వాదం;
        current_syscall_error.invalid_వాదం_సంఖ్య = 0;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    {
        కనుగొను_VSpace_For_ASID_ret_t కనుగొను_ret;

        కనుగొను_ret = కనుగొను_VSpace_For_ASID(asid);
        ఒకవేళ (కనుగొను_ret.స్థితి_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (కనుగొను_ret.vspace_మూలం != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_స్థానం = lookupPDSlot(vspace, vaddr);
    ఒకవేళ (pd_స్థానం.స్థితి_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ఆధారం = false;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    ఒకవేళ (((pde_ptr_get_పుట_కొలత(pd_స్థానం.pd_స్థానం) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_స్థానం.pd_స్థానం)) ||
        ((pde_ptr_get_పుట_కొలత(pd_స్థానం.pd_స్థానం) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_స్థానం.pd_స్థానం))) {
        current_syscall_error.type = se_L4_Delete_మొదటి;

        తిరిగి EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_గమ్యం_paddr(PTE_PTR(cap_పుట_పట్టిక_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_పట్టిక(paddr, attr);

    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTIs_Mapped(cap, 1);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_ASID(cap, asid);
    cap = cap_పుట_పట్టిక_cap_సమితి_cap_PTMapped_చిరునామా(cap, vaddr);

    సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
    తిరిగి perform_X86_పుట_పట్టిక_Invocation_Map(cap, cte, pde, pd_స్థానం.pd_స్థానం, vspace);
}

exception_t decodeX86MMUInvocation(
    మాట_t invLabel,
    మాట_t నిడివి,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    మాట_t *బఫరు
)
{
    ఎంపిక (cap_get_capType(cap)) {

    స్థితి cap_చట్రం_cap:
        తిరిగి decode_X86_చట్రం_Invocation(invLabel, నిడివి, cte, cap, call, బఫరు);

    స్థితి cap_పుట_పట్టిక_cap:
        తిరిగి decode_X86_పుట_పట్టిక_Invocation(invLabel, నిడివి, cte, cap, బఫరు);

    స్థితి cap_asid_control_cap: {
        మాట_t     i;
        asid_t           asid_పునాది;
        మాట_t           సూచీ;
        మాట_t           లోతు;
        cap_t            untyped;
        cap_t            మూలం;
        cte_t           *మాతృ_స్థానం;
        cte_t           *dest_స్థానం;
        lookup_స్థానం_ret_t lu_ret;
        శూన్య            *చట్రం;
        exception_t      స్థితి_2;

        ఒకవేళ (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (నిడివి < 2 || current_extra_caps.excaprefs[0] == ఖాళీ
            || current_extra_caps.excaprefs[1] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        సూచీ = getSyscallArg(0, బఫరు);
        లోతు = getSyscallArg(1, బఫరు);
        మాతృ_స్థానం = current_extra_caps.excaprefs[0];
        untyped = మాతృ_స్థానం->cap;
        మూలం = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        కొరకు (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        ఒకవేళ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid_పునాది = i << asid_దిగువ_బిట్లు;


        ఒకవేళ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ఖండం_కొలత(untyped) != se_L4_ASIDPool_బిట్లు ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        స్థితి_2 = ensureNoChildren(మాతృ_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        చట్రం = మాట_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_లక్ష్యం_స్థానం(మూలం, సూచీ, లోతు);
        ఒకవేళ (lu_ret.స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి lu_ret.స్థితి_2;
        }
        dest_స్థానం = lu_ret.స్థానం;

        స్థితి_2 = ensure_ఖాళీలు_స్థానం(dest_స్థానం);
        ఒకవేళ (స్థితి_2 != EXCEPTION_NONE) {
            తిరిగి స్థితి_2;
        }

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDControlInvocation(చట్రం, dest_స్థానం, మాతృ_స్థానం, asid_పునాది);
    }

    స్థితి cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_స్థానం;
        asid_pool_t *pool;
        మాట_t i;
        asid_t       asid;

        ఒకవేళ (invLabel != X86_ASIDPool_కేటాయించు) {
            current_syscall_error.type = seL4_IllegalOperation;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }
        ఒకవేళ (current_extra_caps.excaprefs[0] == ఖాళీ) {
            current_syscall_error.type = se_L4_Truncated_సందేశం;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_స్థానం = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_స్థానం->cap;

        ఒకవేళ (!(is_VTable_మూలం(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 1;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_ఎగువ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ఒకవేళ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ఆధారం = false;
            current_lookup_fault = lookup_fault_invalid_మూలం_new();
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        ఒకవేళ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_సంఖ్య = 0;
            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        కొరకు (i = 0; i < బిట్టు(asid_దిగువ_బిట్లు) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        ఒకవేళ (i == బిట్టు(asid_దిగువ_బిట్లు)) {
            current_syscall_error.type = se_L4_Delete_మొదటి;

            తిరిగి EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        సమితి_Thread_State(కణుపు_STATE(ksCurThread), ThreadState_Restart);
        తిరిగి performASIDPoolInvocation(asid, pool, vspace_Cap_స్థానం);
    }

    అప్రమేయ:
        తిరిగి decode_X86_బహుళకం_MMUInvocation(invLabel, నిడివి, cptr, cte, cap, call, బఫరు);
    }
}
