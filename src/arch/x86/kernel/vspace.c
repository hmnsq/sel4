/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <api/syscall.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/api/invocation.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/tlb_bitmap.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/kernel/tlb.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <mode/kernel/vspace.h>

ਸਥਿਰ exception_t perform_ਪੰਨਾ_Get_ਪਤਾ(ਖਾਲੀ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ਗੰਢ_STATE(ksCurThread);
    ਜੇ (call) {
        ਸ਼ਬਦ_t *ipc_ਬਫਰ = lookupIPCBuffer(true, thread);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, badge_ਰਜਿਸਟਰ, 0);
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਲੰਮਾਈ = ਸੈੱਟ_MR(thread, ipc_ਬਫਰ, 0, capFBasePtr);
        ਸੈੱਟ_ਰਜਿਸਟਰ(thread, ਸੁਨੇਹਾ_Info_ਰਜਿਸਟਰ, ਸ਼ਬਦ_From_ਸੁਨੇਹਾ_Info(
                        se_L4_ਸੁਨੇਹਾ_Info_new(0, 0, 0, ਲੰਮਾਈ)));
    }
    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Running);
    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ deleteASIDPool(asid_t asid_ਅਧਾਰ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(asid_ਅਧਾਰ, asid_ਨੀਵਾਂ_ਬਿੱਟ));

    ਜੇ (x86KSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] == pool) {
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਵਿੱਥ = 0; ਵਿੱਥ < ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ); ਵਿੱਥ++) {
            asid_map_t asid_map = pool->array[ਵਿੱਥ];
            ਜੇ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_ਜੜ੍ਹ_t *vspace = (vspace_ਜੜ੍ਹ_t *)asid_map_asid_map_vspace_get_vspace_ਜੜ੍ਹ(asid_map);
                hwASIDInvalidate(asid_ਅਧਾਰ + ਵਿੱਥ, vspace);
            }
        }
        x86KSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = ਨਿਰਾ;
        ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(ਖਾਲੀ *ਚੌਖਟਾ, cte_t *ਥਾਂ, cte_t *ਮਾਪਾ, asid_t asid_ਅਧਾਰ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ਸੈੱਟ_cap_Free_ਸੂਚਕਾਂਕ(&(ਮਾਪਾ->cap),
                                         MAX_FREE_ਸੂਚਕਾਂਕ(cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(ਮਾਪਾ->cap)));

    memzero(ਚੌਖਟਾ, ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(X86_Small_ਪੰਨਾ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ਪਾਓ(
        cap_asid_pool_cap_new(
            asid_ਅਧਾਰ,          /* capASIDBase  */
            ਸ਼ਬਦ_REF(ਚੌਖਟਾ)     /* capASIDPool  */
        ),
        ਮਾਪਾ,
        ਥਾਂ
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ਪ੍ਰਤਿਜ੍ਞਾ((asid_ਅਧਾਰ & ਨਕਾਬ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) == 0);
    x86KSASIDTable[ASID_ਉੱਚਾ(asid_ਅਧਾਰ)] = (asid_pool_t *)ਚੌਖਟਾ;

    ਵਾਪਸ EXCEPTION_NONE;
}

ਖਾਲੀ deleteASID(asid_t asid, vspace_ਜੜ੍ਹ_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (poolPtr != ਨਿਰਾ) {
        asid_map_t asid_map = poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
        ਜੇ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ਜੜ੍ਹ_t *)asid_map_asid_map_vspace_get_vspace_ਜੜ੍ਹ(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_ਨੀਵਾਂ(asid)] = asid_map_asid_map_none_new();
            ਸੈੱਟ_VMRoot(ਗੰਢ_STATE(ksCurThread));
        }
    }
}

ਸ਼ਬਦ_t *ਸ਼ੁੱਧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ਸ਼ਬਦ_t      w_ਬਫਰ_Ptr;
    cap_t       ਬਫਰ_Cap;
    vm_rights_t vm_rights;

    w_ਬਫਰ_Ptr = thread->tcbIPCBuffer;
    ਬਫਰ_Cap = TCB_PTR_CTE_PTR(thread, tcb_ਬਫਰ)->cap;

    ਜੇ (cap_get_capType(ਬਫਰ_Cap) != cap_ਚੌਖਟਾ_cap) {
        ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(ਬਫਰ_Cap))) {
        ਵਾਪਸ ਨਿਰਾ;
    }

    vm_rights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(ਬਫਰ_Cap);
    ਜੇ (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        ਸ਼ਬਦ_t ਅਧਾਰ_Ptr, ਪੰਨਾ_ਬਿੱਟ_2;

        ਅਧਾਰ_Ptr = cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(ਬਫਰ_Cap);
        ਪੰਨਾ_ਬਿੱਟ_2 = ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(cap_ਚੌਖਟਾ_cap_get_cap_FSize(ਬਫਰ_Cap));
        ਵਾਪਸ (ਸ਼ਬਦ_t *)(ਅਧਾਰ_Ptr + (w_ਬਫਰ_Ptr & ਨਕਾਬ(ਪੰਨਾ_ਬਿੱਟ_2)));
    } ਨਹੀਂਤਾਂ {
        ਵਾਪਸ ਨਿਰਾ;
    }
}

bool_t CONST is_Valid_VTable_ਜੜ੍ਹ(cap_t cap)
{
    ਵਾਪਸ is_Valid_Native_ਜੜ੍ਹ(cap);
}


BOOT_ਕੋਡ bool_t map_kernel_ਖਿੜਕੀ_devices(pte_t *pt, uint32_t ਅੰਕ_ioapic, paddr_t *ioapic_paddrs, uint32_t ਅੰਕ_drhu,
                                           paddr_t *drhu_list)
{
    ਸ਼ਬਦ_t idx = (KDEV_ਅਧਾਰ & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ;
    paddr_t phys;
    pte_t pte;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i;
    /* map kernel devices: APIC */
    phys = apic_get_ਅਧਾਰ_paddr();
    ਜੇ (!phys) {
        ਵਾਪਸ false;
    }
    ਜੇ (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        ਵਾਪਸ false;
    }
    pte = x86_make_device_pte(phys);

    ਪ੍ਰਤਿਜ੍ਞਾ(idx == (PPTR_APIC & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ);
    pt[idx] = pte;
    idx++;
    ਲਈ (i = 0; i < ਅੰਕ_ioapic; i++) {
        phys = ioapic_paddrs[i];
        ਜੇ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            ਵਾਪਸ false;
        }
        pte = x86_make_device_pte(phys);
        ਪ੍ਰਤਿਜ੍ਞਾ(idx == ((PPTR_IOAPIC_ਸ਼ੁਰੂ + i * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ);
        pt[idx] = pte;
        idx++;
        ਜੇ (idx == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)) {
            ਵਾਪਸ false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    ਲਈ (; i < CONFIG_MAX_ਅੰਕ_IOAPIC; i++) {
        pte = x86_make_ਖਾਲੀ_ਥਾਵਾਂ_pte();
        ਪ੍ਰਤਿਜ੍ਞਾ(idx == ((PPTR_IOAPIC_ਸ਼ੁਰੂ + i * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    ਲਈ (i = 0; i < ਅੰਕ_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        ਜੇ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            ਵਾਪਸ false;
        }
        pte = x86_make_device_pte(phys);

        ਪ੍ਰਤਿਜ੍ਞਾ(idx == ((PPTR_DRHU_ਸ਼ੁਰੂ + i * ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ)) >> ਪੰਨਾ_ਬਿੱਟ);
        pt[idx] = pte;
        idx++;
        ਜੇ (idx == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)) {
            ਵਾਪਸ false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    ਜਦੋਂ (idx < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ)) {
        pte = x86_make_ਖਾਲੀ_ਥਾਵਾਂ_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    ਪ੍ਰਤਿਜ੍ਞਾ(idx == ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ init_idt(idt_ਇੰਦਰਾਜ_t *idt)
{
    init_idt_ਇੰਦਰਾਜ(idt, 0x00, int_00);
    init_idt_ਇੰਦਰਾਜ(idt, 0x01, int_01);
    init_idt_ਇੰਦਰਾਜ(idt, 0x02, int_02);
    init_idt_ਇੰਦਰਾਜ(idt, 0x03, int_03);
    init_idt_ਇੰਦਰਾਜ(idt, 0x04, int_04);
    init_idt_ਇੰਦਰਾਜ(idt, 0x05, int_05);
    init_idt_ਇੰਦਰਾਜ(idt, 0x06, int_06);
    init_idt_ਇੰਦਰਾਜ(idt, 0x07, int_07);
    init_idt_ਇੰਦਰਾਜ(idt, 0x08, int_08);
    init_idt_ਇੰਦਰਾਜ(idt, 0x09, int_09);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0a, int_0a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0b, int_0b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0c, int_0c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0d, int_0d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0e, int_0e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x0f, int_0f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x10, int_10);
    init_idt_ਇੰਦਰਾਜ(idt, 0x11, int_11);
    init_idt_ਇੰਦਰਾਜ(idt, 0x12, int_12);
    init_idt_ਇੰਦਰਾਜ(idt, 0x13, int_13);
    init_idt_ਇੰਦਰਾਜ(idt, 0x14, int_14);
    init_idt_ਇੰਦਰਾਜ(idt, 0x15, int_15);
    init_idt_ਇੰਦਰਾਜ(idt, 0x16, int_16);
    init_idt_ਇੰਦਰਾਜ(idt, 0x17, int_17);
    init_idt_ਇੰਦਰਾਜ(idt, 0x18, int_18);
    init_idt_ਇੰਦਰਾਜ(idt, 0x19, int_19);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1a, int_1a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1b, int_1b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1c, int_1c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1d, int_1d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1e, int_1e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x1f, int_1f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x20, int_20);
    init_idt_ਇੰਦਰਾਜ(idt, 0x21, int_21);
    init_idt_ਇੰਦਰਾਜ(idt, 0x22, int_22);
    init_idt_ਇੰਦਰਾਜ(idt, 0x23, int_23);
    init_idt_ਇੰਦਰਾਜ(idt, 0x24, int_24);
    init_idt_ਇੰਦਰਾਜ(idt, 0x25, int_25);
    init_idt_ਇੰਦਰਾਜ(idt, 0x26, int_26);
    init_idt_ਇੰਦਰਾਜ(idt, 0x27, int_27);
    init_idt_ਇੰਦਰਾਜ(idt, 0x28, int_28);
    init_idt_ਇੰਦਰਾਜ(idt, 0x29, int_29);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2a, int_2a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2b, int_2b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2c, int_2c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2d, int_2d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2e, int_2e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x2f, int_2f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x30, int_30);
    init_idt_ਇੰਦਰਾਜ(idt, 0x31, int_31);
    init_idt_ਇੰਦਰਾਜ(idt, 0x32, int_32);
    init_idt_ਇੰਦਰਾਜ(idt, 0x33, int_33);
    init_idt_ਇੰਦਰਾਜ(idt, 0x34, int_34);
    init_idt_ਇੰਦਰਾਜ(idt, 0x35, int_35);
    init_idt_ਇੰਦਰਾਜ(idt, 0x36, int_36);
    init_idt_ਇੰਦਰਾਜ(idt, 0x37, int_37);
    init_idt_ਇੰਦਰਾਜ(idt, 0x38, int_38);
    init_idt_ਇੰਦਰਾਜ(idt, 0x39, int_39);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3a, int_3a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3b, int_3b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3c, int_3c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3d, int_3d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3e, int_3e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x3f, int_3f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x40, int_40);
    init_idt_ਇੰਦਰਾਜ(idt, 0x41, int_41);
    init_idt_ਇੰਦਰਾਜ(idt, 0x42, int_42);
    init_idt_ਇੰਦਰਾਜ(idt, 0x43, int_43);
    init_idt_ਇੰਦਰਾਜ(idt, 0x44, int_44);
    init_idt_ਇੰਦਰਾਜ(idt, 0x45, int_45);
    init_idt_ਇੰਦਰਾਜ(idt, 0x46, int_46);
    init_idt_ਇੰਦਰਾਜ(idt, 0x47, int_47);
    init_idt_ਇੰਦਰਾਜ(idt, 0x48, int_48);
    init_idt_ਇੰਦਰਾਜ(idt, 0x49, int_49);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4a, int_4a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4b, int_4b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4c, int_4c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4d, int_4d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4e, int_4e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x4f, int_4f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x50, int_50);
    init_idt_ਇੰਦਰਾਜ(idt, 0x51, int_51);
    init_idt_ਇੰਦਰਾਜ(idt, 0x52, int_52);
    init_idt_ਇੰਦਰਾਜ(idt, 0x53, int_53);
    init_idt_ਇੰਦਰਾਜ(idt, 0x54, int_54);
    init_idt_ਇੰਦਰਾਜ(idt, 0x55, int_55);
    init_idt_ਇੰਦਰਾਜ(idt, 0x56, int_56);
    init_idt_ਇੰਦਰਾਜ(idt, 0x57, int_57);
    init_idt_ਇੰਦਰਾਜ(idt, 0x58, int_58);
    init_idt_ਇੰਦਰਾਜ(idt, 0x59, int_59);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5a, int_5a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5b, int_5b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5c, int_5c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5d, int_5d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5e, int_5e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x5f, int_5f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x60, int_60);
    init_idt_ਇੰਦਰਾਜ(idt, 0x61, int_61);
    init_idt_ਇੰਦਰਾਜ(idt, 0x62, int_62);
    init_idt_ਇੰਦਰਾਜ(idt, 0x63, int_63);
    init_idt_ਇੰਦਰਾਜ(idt, 0x64, int_64);
    init_idt_ਇੰਦਰਾਜ(idt, 0x65, int_65);
    init_idt_ਇੰਦਰਾਜ(idt, 0x66, int_66);
    init_idt_ਇੰਦਰਾਜ(idt, 0x67, int_67);
    init_idt_ਇੰਦਰਾਜ(idt, 0x68, int_68);
    init_idt_ਇੰਦਰਾਜ(idt, 0x69, int_69);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6a, int_6a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6b, int_6b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6c, int_6c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6d, int_6d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6e, int_6e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x6f, int_6f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x70, int_70);
    init_idt_ਇੰਦਰਾਜ(idt, 0x71, int_71);
    init_idt_ਇੰਦਰਾਜ(idt, 0x72, int_72);
    init_idt_ਇੰਦਰਾਜ(idt, 0x73, int_73);
    init_idt_ਇੰਦਰਾਜ(idt, 0x74, int_74);
    init_idt_ਇੰਦਰਾਜ(idt, 0x75, int_75);
    init_idt_ਇੰਦਰਾਜ(idt, 0x76, int_76);
    init_idt_ਇੰਦਰਾਜ(idt, 0x77, int_77);
    init_idt_ਇੰਦਰਾਜ(idt, 0x78, int_78);
    init_idt_ਇੰਦਰਾਜ(idt, 0x79, int_79);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7a, int_7a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7b, int_7b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7c, int_7c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7d, int_7d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7e, int_7e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x7f, int_7f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x80, int_80);
    init_idt_ਇੰਦਰਾਜ(idt, 0x81, int_81);
    init_idt_ਇੰਦਰਾਜ(idt, 0x82, int_82);
    init_idt_ਇੰਦਰਾਜ(idt, 0x83, int_83);
    init_idt_ਇੰਦਰਾਜ(idt, 0x84, int_84);
    init_idt_ਇੰਦਰਾਜ(idt, 0x85, int_85);
    init_idt_ਇੰਦਰਾਜ(idt, 0x86, int_86);
    init_idt_ਇੰਦਰਾਜ(idt, 0x87, int_87);
    init_idt_ਇੰਦਰਾਜ(idt, 0x88, int_88);
    init_idt_ਇੰਦਰਾਜ(idt, 0x89, int_89);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8a, int_8a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8b, int_8b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8c, int_8c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8d, int_8d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8e, int_8e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x8f, int_8f);

    init_idt_ਇੰਦਰਾਜ(idt, 0x90, int_90);
    init_idt_ਇੰਦਰਾਜ(idt, 0x91, int_91);
    init_idt_ਇੰਦਰਾਜ(idt, 0x92, int_92);
    init_idt_ਇੰਦਰਾਜ(idt, 0x93, int_93);
    init_idt_ਇੰਦਰਾਜ(idt, 0x94, int_94);
    init_idt_ਇੰਦਰਾਜ(idt, 0x95, int_95);
    init_idt_ਇੰਦਰਾਜ(idt, 0x96, int_96);
    init_idt_ਇੰਦਰਾਜ(idt, 0x97, int_97);
    init_idt_ਇੰਦਰਾਜ(idt, 0x98, int_98);
    init_idt_ਇੰਦਰਾਜ(idt, 0x99, int_99);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9a, int_9a);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9b, int_9b);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9c, int_9c);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9d, int_9d);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9e, int_9e);
    init_idt_ਇੰਦਰਾਜ(idt, 0x9f, int_9f);

    init_idt_ਇੰਦਰਾਜ(idt, 0xa0, int_a0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa1, int_a1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa2, int_a2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa3, int_a3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa4, int_a4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa5, int_a5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa6, int_a6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa7, int_a7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa8, int_a8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xa9, int_a9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xaa, int_aa);
    init_idt_ਇੰਦਰਾਜ(idt, 0xab, int_ab);
    init_idt_ਇੰਦਰਾਜ(idt, 0xac, int_ac);
    init_idt_ਇੰਦਰਾਜ(idt, 0xad, int_ad);
    init_idt_ਇੰਦਰਾਜ(idt, 0xae, int_ae);
    init_idt_ਇੰਦਰਾਜ(idt, 0xaf, int_af);

    init_idt_ਇੰਦਰਾਜ(idt, 0xb0, int_b0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb1, int_b1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb2, int_b2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb3, int_b3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb4, int_b4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb5, int_b5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb6, int_b6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb7, int_b7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb8, int_b8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xb9, int_b9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xba, int_ba);
    init_idt_ਇੰਦਰਾਜ(idt, 0xbb, int_bb);
    init_idt_ਇੰਦਰਾਜ(idt, 0xbc, int_bc);
    init_idt_ਇੰਦਰਾਜ(idt, 0xbd, int_bd);
    init_idt_ਇੰਦਰਾਜ(idt, 0xbe, int_be);
    init_idt_ਇੰਦਰਾਜ(idt, 0xbf, int_bf);

    init_idt_ਇੰਦਰਾਜ(idt, 0xc0, int_c0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc1, int_c1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc2, int_c2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc3, int_c3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc4, int_c4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc5, int_c5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc6, int_c6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc7, int_c7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc8, int_c8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xc9, int_c9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xca, int_ca);
    init_idt_ਇੰਦਰਾਜ(idt, 0xcb, int_cb);
    init_idt_ਇੰਦਰਾਜ(idt, 0xcc, int_cc);
    init_idt_ਇੰਦਰਾਜ(idt, 0xcd, int_cd);
    init_idt_ਇੰਦਰਾਜ(idt, 0xce, int_ce);
    init_idt_ਇੰਦਰਾਜ(idt, 0xcf, int_cf);

    init_idt_ਇੰਦਰਾਜ(idt, 0xd0, int_d0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd1, int_d1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd2, int_d2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd3, int_d3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd4, int_d4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd5, int_d5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd6, int_d6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd7, int_d7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd8, int_d8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xd9, int_d9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xda, int_da);
    init_idt_ਇੰਦਰਾਜ(idt, 0xdb, int_db);
    init_idt_ਇੰਦਰਾਜ(idt, 0xdc, int_dc);
    init_idt_ਇੰਦਰਾਜ(idt, 0xdd, int_dd);
    init_idt_ਇੰਦਰਾਜ(idt, 0xde, int_de);
    init_idt_ਇੰਦਰਾਜ(idt, 0xdf, int_df);

    init_idt_ਇੰਦਰਾਜ(idt, 0xe0, int_e0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe1, int_e1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe2, int_e2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe3, int_e3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe4, int_e4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe5, int_e5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe6, int_e6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe7, int_e7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe8, int_e8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xe9, int_e9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xea, int_ea);
    init_idt_ਇੰਦਰਾਜ(idt, 0xeb, int_eb);
    init_idt_ਇੰਦਰਾਜ(idt, 0xec, int_ec);
    init_idt_ਇੰਦਰਾਜ(idt, 0xed, int_ed);
    init_idt_ਇੰਦਰਾਜ(idt, 0xee, int_ee);
    init_idt_ਇੰਦਰਾਜ(idt, 0xef, int_ef);

    init_idt_ਇੰਦਰਾਜ(idt, 0xf0, int_f0);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf1, int_f1);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf2, int_f2);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf3, int_f3);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf4, int_f4);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf5, int_f5);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf6, int_f6);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf7, int_f7);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf8, int_f8);
    init_idt_ਇੰਦਰਾਜ(idt, 0xf9, int_f9);
    init_idt_ਇੰਦਰਾਜ(idt, 0xfa, int_fa);
    init_idt_ਇੰਦਰਾਜ(idt, 0xfb, int_fb);
    init_idt_ਇੰਦਰਾਜ(idt, 0xfc, int_fc);
    init_idt_ਇੰਦਰਾਜ(idt, 0xfd, int_fd);
    init_idt_ਇੰਦਰਾਜ(idt, 0xfe, int_fe);
    init_idt_ਇੰਦਰਾਜ(idt, 0xff, int_ff);
}

BOOT_ਕੋਡ bool_t init_vm_state(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t cache_ਸਤਰ_ਮਾਪ;
    x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ = get_Cache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ();
    ਜੇ (!x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ) {
        ਵਾਪਸ false;
    }

    cache_ਸਤਰ_ਮਾਪ = ਦੋਆਧਾਰੀ_ਅੰਕ(x86_KScache_ਸਤਰ_ਮਾਪ_ਬਿੱਟ);
    ਜੇ (cache_ਸਤਰ_ਮਾਪ != L1_CACHE_ਸਤਰ_ਮਾਪ) {
        ਲਿਖੋ("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_ਸਤਰ_ਮਾਪ, cache_ਸਤਰ_ਮਾਪ);
        SMP_COND_STATEMENT(ਵਾਪਸ false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    ਖਾਲੀ *tss_ptr = &x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_ਸੂਚਕਾਂਕ()].x86KSidt);
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ bool_t init_pat_msr(ਖਾਲੀ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    ਜੇ ((x86_cpuid_edx(0x1, 0x0) & ਦੋਆਧਾਰੀ_ਅੰਕ(16)) == 0) {
        ਲਿਖੋ("PAT support not found\n");
        ਵਾਪਸ false;
    }
    pat_msr.ਸ਼ਬਦ[0] = x86_rdmsr_ਨੀਵਾਂ(IA32_PAT_MSR);
    pat_msr.ਸ਼ਬਦ[1] = x86_rdmsr_ਉੱਚਾ(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_ਸੈੱਟ_pa0(pat_msr, IA32_PAT_MT_WRITE_ਪਿੱਠ);
    pat_msr = x86_pat_msr_ਸੈੱਟ_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_ਸੈੱਟ_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_ਸੈੱਟ_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_ਸੈੱਟ_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.ਸ਼ਬਦ[1]) << 32 | pat_msr.ਸ਼ਬਦ[0]);
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਖਾਲੀ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ਦਾ_cap(it_ap_cap));
    ap->array[ASID_ਨੀਵਾਂ(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_ਦਾ_cap(it_vspace_cap));
    x86KSASIDTable[ASID_ਉੱਚਾ(IT_ASID)] = ap;
}

asid_map_t ਲੱਭੋ_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ਉੱਚਾ(asid)];
    ਜੇ (!poolPtr) {
        ਵਾਪਸ asid_map_asid_map_none_new();
    }

    ਵਾਪਸ poolPtr->array[ASID_ਨੀਵਾਂ(asid)];
}

ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_VSpace_For_ASID(asid_t asid)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ਲੱਭੋ_Map_For_ASID(asid);
    ਜੇ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();

        ret.vspace_ਜੜ੍ਹ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    }

    ret.vspace_ਜੜ੍ਹ = (vspace_ਜੜ੍ਹ_t *)asid_map_asid_map_vspace_get_vspace_ਜੜ੍ਹ(asid_map);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ਸ਼ਬਦ_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_ਰਜਿਸਟਰ(thread, Error);

    ਚੋਣ (vm_faultType) {
    ਹਾਲਤ X86_ਅੰਕੜੇ_Fault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ਵਾਪਸ EXCEPTION_FAULT;

    ਹਾਲਤ X86InstructionFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, true);
        ਵਾਪਸ EXCEPTION_FAULT;

    ਮੂਲ:
        fail("Invalid VM fault type");
    }
}

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMReadOnly:
        ਵਾਪਸ 0;

    ਹਾਲਤ VMKernelOnly:
    ਹਾਲਤ VMReadWrite:
        ਵਾਪਸ 1;

    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights)
{
    ਚੋਣ (vm_rights) {
    ਹਾਲਤ VMKernelOnly:
        ਵਾਪਸ 0;

    ਹਾਲਤ VMReadOnly:
    ਹਾਲਤ VMReadWrite:
        ਵਾਪਸ 1;

    ਮੂਲ:
        fail("Invalid VM rights");
    }
}

lookupPTSlot_ret_t lookupPTSlot(vspace_ਜੜ੍ਹ_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_ਥਾਂ;

    pd_ਥਾਂ = lookupPDSlot(vspace, vptr);
    ਜੇ (pd_ਥਾਂ.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ret.pt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = pd_ਥਾਂ.ਸਥਿਤੀ;
        ਵਾਪਸ ret;
    }
    ਜੇ ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ.pd_ਥਾਂ) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_ਥਾਂ.pd_ਥਾਂ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(ਪੰਨਾ_ਬਿੱਟ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
        ret.pt_ਥਾਂ = ਨਿਰਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_LOOKUP_FAULT;
        ਵਾਪਸ ret;
    } ਨਹੀਂਤਾਂ {
        pte_t *pt;
        pte_t *pt_ਥਾਂ;
        ਸ਼ਬਦ_t pt_ਸੂਚਕਾਂਕ;

        pt = paddr_ਮੰਜ਼ਿਲ_pptr(pde_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ.pd_ਥਾਂ));
        pt_ਸੂਚਕਾਂਕ = (vptr >> ਪੰਨਾ_ਬਿੱਟ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ);
        pt_ਥਾਂ = pt + pt_ਸੂਚਕਾਂਕ;

        ret.pt_ਥਾਂ = pt_ਥਾਂ;
        ret.ਸਥਿਤੀ = EXCEPTION_NONE;
        ਵਾਪਸ ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ਜੇ (cap_get_capType(cap) != cap_ਚੌਖਟਾ_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
    ਜੇ (unlikely(cap_ਚੌਖਟਾ_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ਮਾਪ_ਬਿੱਟ)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (ਪੂਰਨ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

vm_rights_t CONST ਨਕਾਬ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ਨਕਾਬ)
{
    ਜੇ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਵਾਪਸ VMReadOnly;
    }
    ਜੇ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ)) {
        ਜੇ (!seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
            ਵਾਪਸ VMReadOnly;
        } ਨਹੀਂਤਾਂ {
            ਵਾਪਸ VMReadWrite;
        }
    }
    ਜੇ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ਨਕਾਬ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ਨਕਾਬ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ਵਾਪਸ VMKernelOnly;
}

ਖਾਲੀ flush_ਸਾਰਣੀ(vspace_ਜੜ੍ਹ_t *vspace, ਸ਼ਬਦ_t vptr, pte_t *pt, asid_t asid)
{
    ਸ਼ਬਦ_t i;
    cap_t        thread_ਜੜ੍ਹ;

    ਪ੍ਰਤਿਜ੍ਞਾ(IS_ALIGNED(vptr, PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ));

    /* check if page table belongs to current address space */
    thread_ਜੜ੍ਹ = TCB_PTR_CTE_PTR(ਗੰਢ_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    ਲਈ (i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        ਜੇ (pte_get_present(pt[i])) {
            ਜੇ (config_ਸੈੱਟ(CONFIG_SUPPORT_PCID) || (is_Valid_Native_ਜੜ੍ਹ(thread_ਜੜ੍ਹ)
                                                    && (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(thread_ਜੜ੍ਹ) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << ਪੰਨਾ_ਬਿੱਟ), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


ਖਾਲੀ unmap_ਪੰਨਾ(vm_ਪੰਨਾ_ਮਾਪ_t ਪੰਨਾ_ਮਾਪ, asid_t asid, vptr_t vptr, ਖਾਲੀ *pptr)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    ਚੋਣ (ਪੰਨਾ_ਮਾਪ) {
    ਹਾਲਤ X86_Small_ਪੰਨਾ:
        lu_ret = lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ;
        }
        ਜੇ (!(pte_ptr_get_present(lu_ret.pt_ਥਾਂ)
              && (pte_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(lu_ret.pt_ਥਾਂ)
                  == pptr_ਮੰਜ਼ਿਲ_paddr(pptr)))) {
            ਵਾਪਸ;
        }
        *lu_ret.pt_ਥਾਂ = makeUserPTEInvalid();
        ਰੋਕੋ;

    ਹਾਲਤ X86_Large_ਪੰਨਾ:
        pd_ret = lookupPDSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr);
        ਜੇ (pd_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ;
        }
        pde = pd_ret.pd_ਥਾਂ;
        ਜੇ (!(pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pde)
                  == pptr_ਮੰਜ਼ਿਲ_paddr(pptr)))) {
            ਵਾਪਸ;
        }
        *pde = makeUserPDEInvalid();
        ਰੋਕੋ;

    ਮੂਲ:
        ਜੇ (!ਬਹੁਲਕ_Unmap_ਪੰਨਾ(ਪੰਨਾ_ਮਾਪ, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr, pptr)) {
            ਵਾਪਸ;
        }
        ਰੋਕੋ;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ), 0));
}

ਖਾਲੀ unmap_ਪੰਨਾ_ਸਾਰਣੀ(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;
    lookupPDSlot_ret_t    lu_ret;

    ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
    ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    lu_ret = lookupPDSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਵਾਪਸ;
    }

    /* check if the PD actually refers to the PT */
    ਜੇ (!(pde_ptr_get_ਪੰਨਾ_ਮਾਪ(lu_ret.pd_ਥਾਂ) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_ਥਾਂ) &&
          (pde_pde_pt_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(lu_ret.pd_ਥਾਂ) == pptr_ਮੰਜ਼ਿਲ_paddr(pt)))) {
        ਵਾਪਸ;
    }

    flush_ਸਾਰਣੀ(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vaddr, pt, asid);

    *lu_ret.pd_ਥਾਂ = makeUserPDEInvalid();

    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ), 0));
}

ਸਥਿਰ exception_t perform_X86_ਪੰਨਾ_Invocation_Map_PTE(cap_t cap, cte_t *ct_ਥਾਂ, pte_t *pt_ਥਾਂ, pte_t pte,
                                                  vspace_ਜੜ੍ਹ_t *vspace)
{
    ct_ਥਾਂ->cap = cap;
    *pt_ਥਾਂ = pte;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(vspace), cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X86_ਪੰਨਾ_Invocation_Map_PDE(cap_t cap, cte_t *ct_ਥਾਂ, pde_t *pd_ਥਾਂ, pde_t pde,
                                                  vspace_ਜੜ੍ਹ_t *vspace)
{
    ct_ਥਾਂ->cap = cap;
    *pd_ਥਾਂ = pde;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(vspace), cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ਵਾਪਸ EXCEPTION_NONE;
}


ਸਥਿਰ exception_t perform_X86_ਪੰਨਾ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap));
    ਪ੍ਰਤਿਜ੍ਞਾ(cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap)) {
        unmap_ਪੰਨਾ(
            cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap),
            cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap),
            cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap),
            (ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMapped_ਪਤਾ(&ct_ਥਾਂ->cap, 0);
    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMapped_ASID(&ct_ਥਾਂ->cap, asidInvalid);
    cap_ਚੌਖਟਾ_cap_ptr_ਸੈੱਟ_cap_FMap_Type(&ct_ਥਾਂ->cap, X86_MappingNone);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X86_ਚੌਖਟਾ_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        ਚੋਣ (cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap)) {
        ਹਾਲਤ X86_MappingVSpace:
            ਵਾਪਸ perform_X86_ਪੰਨਾ_Invocation_Unmap(cap, cte);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
        ਹਾਲਤ X86_MappingIOSpace:
            ਵਾਪਸ performX86IOUnMapInvocation(cap, cte);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
        ਹਾਲਤ X86_MappingEPT:
            ਵਾਪਸ performX86EPTPageInvocationUnmap(cap, cte);
#ਜੇ_ਅੰਤ
        ਹਾਲਤ X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            ਰੋਕੋ;
        }
    }

    ਵਾਪਸ EXCEPTION_NONE;
}

ਬਣਤਰ create_mapping_pte_return {
    exception_t ਸਥਿਤੀ;
    pte_t pte;
    pte_t *pt_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ create_mapping_pte_return create_mapping_pte_return_t;

ਸਥਿਰ create_mapping_pte_return_t create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PTE(paddr_t ਅਧਾਰ, ਸ਼ਬਦ_t vaddr, vm_rights_t vmRights,
                                                                vm_ਗੁਣ_t attr,
                                                                vspace_ਜੜ੍ਹ_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        ਵਾਪਸ ret;
    }

    ret.pte = makeUserPTE(ਅਧਾਰ, attr, vmRights);
    ret.pt_ਥਾਂ = lu_ret.pt_ਥਾਂ;
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}

ਬਣਤਰ create_mapping_pde_return {
    exception_t ਸਥਿਤੀ;
    pde_t pde;
    pde_t *pd_ਥਾਂ;
};
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ create_mapping_pde_return create_mapping_pde_return_t;

ਸਥਿਰ create_mapping_pde_return_t create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDE(paddr_t ਅਧਾਰ, ਸ਼ਬਦ_t vaddr, vm_rights_t vmRights,
                                                                vm_ਗੁਣ_t attr,
                                                                vspace_ਜੜ੍ਹ_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        ਵਾਪਸ ret;
    }
    ret.pd_ਥਾਂ = lu_ret.pd_ਥਾਂ;

    /* check for existing page table */
    ਜੇ ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(ret.pd_ਥਾਂ) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_ਥਾਂ))) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;
        ret.ਸਥਿਤੀ = EXCEPTION_SYSCALL_ERROR;
        ਵਾਪਸ ret;
    }


    ret.pde = make_User_PDELarge_ਪੰਨਾ(ਅਧਾਰ, attr, vmRights);
    ret.ਸਥਿਤੀ = EXCEPTION_NONE;
    ਵਾਪਸ ret;
}


exception_t decode_X86_ਚੌਖਟਾ_Invocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (invLabel) {
    ਹਾਲਤ X86_ਪੰਨਾ_Map: { /* Map */
        ਸ਼ਬਦ_t          vaddr;
        ਸ਼ਬਦ_t          vtop;
        ਸ਼ਬਦ_t          w_rights_ਨਕਾਬ;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_ਜੜ੍ਹ_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_ਗੁਣ_t vmAttr;
        vm_ਪੰਨਾ_ਮਾਪ_t  ਚੌਖਟਾ_ਮਾਪ;
        asid_t          asid;

        ਜੇ (ਲੰਮਾਈ < 3 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਚੌਖਟਾ_ਮਾਪ = cap_ਚੌਖਟਾ_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, ਬਫਰ);
        w_rights_ਨਕਾਬ = getSyscallArg(1, ਬਫਰ);
        vmAttr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(2, ਬਫਰ));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_ਚੌਖਟਾ_cap_get_cap_FVMRights(cap);

        ਜੇ (!is_Valid_Native_ਜੜ੍ਹ(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            ਜੇ (cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        } ਨਹੀਂਤਾਂ {
            vtop = vaddr + ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ_For_ਮਾਪ(ਚੌਖਟਾ_ਮਾਪ));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            ਜੇ (vaddr > USER_ਸਿਖਰ || vtop > USER_ਸਿਖਰ) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
                current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

            ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
            ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }

            ਜੇ (ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

                ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = ਨਕਾਬ_VMRights(capVMRights, rights_From_ਸ਼ਬਦ(w_rights_ਨਕਾਬ));

        ਜੇ (!checkVPAlignment(ਚੌਖਟਾ_ਮਾਪ, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap));

        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ASID(cap, asid);
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMapped_ਪਤਾ(cap, vaddr);
        cap = cap_ਚੌਖਟਾ_cap_ਸੈੱਟ_cap_FMap_Type(cap, X86_MappingVSpace);

        ਚੋਣ (ਚੌਖਟਾ_ਮਾਪ) {
        /* PTE mappings */
        ਹਾਲਤ X86_Small_ਪੰਨਾ: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            ਜੇ (map_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                ਵਾਪਸ map_ret.ਸਥਿਤੀ;
            }

            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_X86_ਪੰਨਾ_Invocation_Map_PTE(cap, cte, map_ret.pt_ਥਾਂ, map_ret.pte, vspace);
        }

        /* PDE mappings */
        ਹਾਲਤ X86_Large_ਪੰਨਾ: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_ਸੁਰੱਖਿਅਤ_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            ਜੇ (map_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
                ਵਾਪਸ map_ret.ਸਥਿਤੀ;
            }

            ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
            ਵਾਪਸ perform_X86_ਪੰਨਾ_Invocation_Map_PDE(cap, cte, map_ret.pd_ਥਾਂ, map_ret.pde, vspace);
        }

        ਮੂਲ: {
            ਵਾਪਸ decode_X86_ਬਹੁਲਕ_Map_ਪੰਨਾ(invLabel, ਚੌਖਟਾ_ਮਾਪ, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਹਾਲਤ X86_ਪੰਨਾ_Unmap: { /* Unmap */
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_X86_ਚੌਖਟਾ_Invocation_Unmap(cap, cte);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_IOMMU
    ਹਾਲਤ X86_ਪੰਨਾ_Map_IO: { /* MapIO */
        ਵਾਪਸ decodeX86IOMapInvocation(ਲੰਮਾਈ, cte, cap, ਬਫਰ);
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_VTX
    ਹਾਲਤ X86_ਪੰਨਾ_Map_EPT:
        ਵਾਪਸ decodeX86EPTPageMap(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);
#ਜੇ_ਅੰਤ

    ਹਾਲਤ X86_ਪੰਨਾ_Get_ਪਤਾ: {
        /* Return it in the first message register. */
        ਪ੍ਰਤਿਜ੍ਞਾ(n_ਸੁਨੇਹਾ_Registers >= 1);

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_ਪੰਨਾ_Get_ਪਤਾ((ਖਾਲੀ *)cap_ਚੌਖਟਾ_cap_get_cap_FBase_Ptr(cap), call);
    }

    ਮੂਲ:
        current_syscall_error.type = seL4_IllegalOperation;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }
}

ਸਥਿਰ exception_t perform_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap_t cap, cte_t *ct_ਥਾਂ)
{

    ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ਪੰਨਾ_ਸਾਰਣੀ(
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
            cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap),
            pt
        );
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)pt, cap_get_cap_ਮਾਪ_ਬਿੱਟ(cap));
    }
    cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ptr_ਸੈੱਟ_cap_PTIs_Mapped(&(ct_ਥਾਂ->cap), 0);

    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t perform_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap_t cap, cte_t *ct_ਥਾਂ, pde_t pde, pde_t *pd_ਥਾਂ,
                                                    vspace_ਜੜ੍ਹ_t *ਜੜ੍ਹ)
{
    ct_ਥਾਂ->cap = cap;
    *pd_ਥਾਂ = pde;
    invalidate_ਪੰਨਾ_Structure_Cache_ASID(pptr_ਮੰਜ਼ਿਲ_paddr(ਜੜ੍ਹ), cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(ਜੜ੍ਹ), 0));
    ਵਾਪਸ EXCEPTION_NONE;
}

ਸਥਿਰ exception_t decode_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cte_t *cte, cap_t cap,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਸ਼ਬਦ_t          vaddr;
    vm_ਗੁਣ_t attr;
    lookupPDSlot_ret_t pd_ਥਾਂ;
    cap_t           vspaceCap;
    vspace_ਜੜ੍ਹ_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    ਜੇ (invLabel == X86_ਪੰਨਾ_ਸਾਰਣੀ_Unmap) {
        ਜੇ (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ਪਹਿਲਾ;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ perform_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Unmap(cap, cte);
    }

    ਜੇ (invLabel != X86_ਪੰਨਾ_ਸਾਰਣੀ_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ਬਫਰ) & (~ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ + ਪੰਨਾ_ਬਿੱਟ));
    attr = vm_ਗੁਣ_From_ਸ਼ਬਦ(getSyscallArg(1, ਬਫਰ));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ਜੇ (!is_Valid_Native_ਜੜ੍ਹ(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ਜੜ੍ਹ_t *)pptr_ਦਾ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ਜੇ (vaddr > USER_ਸਿਖਰ) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ਦਲੀਲ;
        current_syscall_error.invalid_ਦਲੀਲ_ਸੰਖਿਆ = 0;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    {
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret;

        ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (ਲੱਭੋ_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_ਥਾਂ = lookupPDSlot(vspace, vaddr);
    ਜੇ (pd_ਥਾਂ.ਸਥਿਤੀ != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    ਜੇ (((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ.pd_ਥਾਂ) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_ਥਾਂ.pd_ਥਾਂ)) ||
        ((pde_ptr_get_ਪੰਨਾ_ਮਾਪ(pd_ਥਾਂ.pd_ਥਾਂ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ਥਾਂ.pd_ਥਾਂ))) {
        current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

        ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ਮੰਜ਼ਿਲ_paddr(PTE_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_ਸਾਰਣੀ(paddr, attr);

    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_ਸੈੱਟ_cap_PTMapped_ਪਤਾ(cap, vaddr);

    ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
    ਵਾਪਸ perform_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation_Map(cap, cte, pde, pd_ਥਾਂ.pd_ਥਾਂ, vspace);
}

exception_t decodeX86MMUInvocation(
    ਸ਼ਬਦ_t invLabel,
    ਸ਼ਬਦ_t ਲੰਮਾਈ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ਸ਼ਬਦ_t *ਬਫਰ
)
{
    ਚੋਣ (cap_get_capType(cap)) {

    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
        ਵਾਪਸ decode_X86_ਚੌਖਟਾ_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, call, ਬਫਰ);

    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        ਵਾਪਸ decode_X86_ਪੰਨਾ_ਸਾਰਣੀ_Invocation(invLabel, ਲੰਮਾਈ, cte, cap, ਬਫਰ);

    ਹਾਲਤ cap_asid_control_cap: {
        ਸ਼ਬਦ_t     i;
        asid_t           asid_ਅਧਾਰ;
        ਸ਼ਬਦ_t           ਸੂਚਕਾਂਕ;
        ਸ਼ਬਦ_t           ਡੂੰਘਾਈ;
        cap_t            untyped;
        cap_t            ਜੜ੍ਹ;
        cte_t           *ਮਾਪਾ_ਥਾਂ;
        cte_t           *dest_ਥਾਂ;
        lookup_ਥਾਂ_ret_t lu_ret;
        ਖਾਲੀ            *ਚੌਖਟਾ;
        exception_t      ਸਥਿਤੀ;

        ਜੇ (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (ਲੰਮਾਈ < 2 || current_extra_caps.excaprefs[0] == ਨਿਰਾ
            || current_extra_caps.excaprefs[1] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸੂਚਕਾਂਕ = getSyscallArg(0, ਬਫਰ);
        ਡੂੰਘਾਈ = getSyscallArg(1, ਬਫਰ);
        ਮਾਪਾ_ਥਾਂ = current_extra_caps.excaprefs[0];
        untyped = ਮਾਪਾ_ਥਾਂ->cap;
        ਜੜ੍ਹ = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ਲਈ (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        ਜੇ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ਅਧਾਰ = i << asid_ਨੀਵਾਂ_ਬਿੱਟ;


        ਜੇ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ਖੰਡ_ਮਾਪ(untyped) != se_L4_ASIDPool_ਬਿੱਟ ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਸਥਿਤੀ = ensureNoChildren(ਮਾਪਾ_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਚੌਖਟਾ = ਸ਼ਬਦ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ਨਿਸ਼ਾਨਾ_ਥਾਂ(ਜੜ੍ਹ, ਸੂਚਕਾਂਕ, ਡੂੰਘਾਈ);
        ਜੇ (lu_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ lu_ret.ਸਥਿਤੀ;
        }
        dest_ਥਾਂ = lu_ret.ਥਾਂ;

        ਸਥਿਤੀ = ensure_ਖਾਲੀ_ਥਾਵਾਂ_ਥਾਂ(dest_ਥਾਂ);
        ਜੇ (ਸਥਿਤੀ != EXCEPTION_NONE) {
            ਵਾਪਸ ਸਥਿਤੀ;
        }

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDControlInvocation(ਚੌਖਟਾ, dest_ਥਾਂ, ਮਾਪਾ_ਥਾਂ, asid_ਅਧਾਰ);
    }

    ਹਾਲਤ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_ਥਾਂ;
        asid_pool_t *pool;
        ਸ਼ਬਦ_t i;
        asid_t       asid;

        ਜੇ (invLabel != X86_ASIDPool_ਸੌਂਪੋ) {
            current_syscall_error.type = seL4_IllegalOperation;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }
        ਜੇ (current_extra_caps.excaprefs[0] == ਨਿਰਾ) {
            current_syscall_error.type = se_L4_Truncated_ਸੁਨੇਹਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ਥਾਂ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ਥਾਂ->cap;

        ਜੇ (!(is_VTable_ਜੜ੍ਹ(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 1;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_ਉੱਚਾ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ਜੇ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ਸਰੋਤ = false;
            current_lookup_fault = lookup_fault_invalid_ਜੜ੍ਹ_new();
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        ਜੇ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ਸੰਖਿਆ = 0;
            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ਲਈ (i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        ਜੇ (i == ਦੋਆਧਾਰੀ_ਅੰਕ(asid_ਨੀਵਾਂ_ਬਿੱਟ)) {
            current_syscall_error.type = se_L4_Delete_ਪਹਿਲਾ;

            ਵਾਪਸ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ਸੈੱਟ_Thread_State(ਗੰਢ_STATE(ksCurThread), ThreadState_Restart);
        ਵਾਪਸ performASIDPoolInvocation(asid, pool, vspace_Cap_ਥਾਂ);
    }

    ਮੂਲ:
        ਵਾਪਸ decode_X86_ਬਹੁਲਕ_MMUInvocation(invLabel, ਲੰਮਾਈ, cptr, cte, cap, call, ਬਫਰ);
    }
}
