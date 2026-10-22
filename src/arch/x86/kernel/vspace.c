/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <api/syscall.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <arch/api/invocation.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/tlb_bitmap.h>
#ಅನ್ತರ್ಭಾವ <mode/kernel/tlb.h>
#ಅನ್ತರ್ಭಾವ <mode/kernel/vspace.h>

ಸ್ಥಿರ exception_t perform_ಪುಟ_Get_ವಿಳಾಸ(ಶೂನ್ಯ *vbase_ptr, bool_t call)
{
    /* Get the physical address of this frame. */
    paddr_t capFBasePtr;
    capFBasePtr = addrFromPPtr(vbase_ptr);

    tcb_t *thread;
    thread = ಗಂಟು_STATE(ksCurThread);
    ಆದರೆ (call) {
        ಶಬ್ದ_t *ipc_ಬಫರ್ = lookupIPCBuffer(true, thread);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, badge_ರಿಜಿಸ್ಟರ್, 0);
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಉದ್ದಳತೆ = ಗಣ_MR(thread, ipc_ಬಫರ್, 0, capFBasePtr);
        ಗಣ_ರಿಜಿಸ್ಟರ್(thread, ಸಂದೇಶ_Info_ರಿಜಿಸ್ಟರ್, ಶಬ್ದ_From_ಸಂದೇಶ_Info(
                        se_L4_ಸಂದೇಶ_Info_new(0, 0, 0, ಉದ್ದಳತೆ)));
    }
    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Running);
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ deleteASIDPool(asid_t asid_ತಳ, asid_pool_t *pool)
{
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(asid_ತಳ, asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು));

    ಆದರೆ (x86KSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] == pool) {
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು); ಸ್ಥಾನಾಂತರ++) {
            asid_map_t asid_map = pool->array[ಸ್ಥಾನಾಂತರ];
            ಆದರೆ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace) {
                vspace_ಬೇರು_t *vspace = (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map);
                hwASIDInvalidate(asid_ತಳ + ಸ್ಥಾನಾಂತರ, vspace);
            }
        }
        x86KSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = ಖಾಲಿ;
        ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
    }
}

exception_t performASIDControlInvocation(ಶೂನ್ಯ *ಚೌಕಟ್ಟು, cte_t *ಸ್ಥಾನ, cte_t *ಪೋಷಕ, asid_t asid_ತಳ)
{
    /** AUXUPD: "(True, typ_region_bytes (ptr_val \<acute>frame) 12)" */
    /** GHOSTUPD: "(True, gs_clear_region (ptr_val \<acute>frame) 12)" */
    cap_untyped_cap_ptr_ಗಣ_cap_Free_ಸೂಚ್ಯಂಕ(&(ಪೋಷಕ->cap),
                                         MAX_FREE_ಸೂಚ್ಯಂಕ(cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(ಪೋಷಕ->cap)));

    memzero(ಚೌಕಟ್ಟು, ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(X86_Small_ಪುಟ)));
    /** AUXUPD: "(True, ptr_retyps 1 (Ptr (ptr_val \<acute>frame) :: asid_pool_C ptr))" */

    cte_ಸೇರಿಸು(
        cap_asid_pool_cap_new(
            asid_ತಳ,          /* capASIDBase  */
            ಶಬ್ದ_REF(ಚೌಕಟ್ಟು)     /* capASIDPool  */
        ),
        ಪೋಷಕ,
        ಸ್ಥಾನ
    );
    /* Haskell error: "ASID pool's base must be aligned" */
    ಪ್ರತಿಜ್ಞಾ((asid_ತಳ & ಮುಖವಾಡ(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) == 0);
    x86KSASIDTable[ASID_ಮೇಲಿನ(asid_ತಳ)] = (asid_pool_t *)ಚೌಕಟ್ಟು;

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ deleteASID(asid_t asid, vspace_ಬೇರು_t *vspace)
{
    asid_pool_t *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (poolPtr != ಖಾಲಿ) {
        asid_map_t asid_map = poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
        ಆದರೆ (asid_map_get_type(asid_map) == asid_map_asid_map_vspace &&
            (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map) == vspace) {
            hwASIDInvalidate(asid, vspace);
            poolPtr->array[ASID_ಕೆಳಗಿನ(asid)] = asid_map_asid_map_none_new();
            ಗಣ_VMRoot(ಗಂಟು_STATE(ksCurThread));
        }
    }
}

ಶಬ್ದ_t *ಶುದ್ಧ lookupIPCBuffer(bool_t isReceiver, tcb_t *thread)
{
    ಶಬ್ದ_t      w_ಬಫರ್_Ptr;
    cap_t       ಬಫರ್_Cap;
    vm_rights_t vm_rights;

    w_ಬಫರ್_Ptr = thread->tcbIPCBuffer;
    ಬಫರ್_Cap = TCB_PTR_CTE_PTR(thread, tcb_ಬಫರ್)->cap;

    ಆದರೆ (cap_get_capType(ಬಫರ್_Cap) != cap_ಚೌಕಟ್ಟು_cap) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(ಬಫರ್_Cap))) {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    vm_rights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(ಬಫರ್_Cap);
    ಆದರೆ (vm_rights == VMReadWrite || (!isReceiver && vm_rights == VMReadOnly)) {
        ಶಬ್ದ_t ತಳ_Ptr, ಪುಟ_ಬಿಟ್ಟುಗಳು_2;

        ತಳ_Ptr = cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(ಬಫರ್_Cap);
        ಪುಟ_ಬಿಟ್ಟುಗಳು_2 = ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(ಬಫರ್_Cap));
        ಹಿಂತಿರುಗಿ (ಶಬ್ದ_t *)(ತಳ_Ptr + (w_ಬಫರ್_Ptr & ಮುಖವಾಡ(ಪುಟ_ಬಿಟ್ಟುಗಳು_2)));
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
}

bool_t CONST is_Valid_VTable_ಬೇರು(cap_t cap)
{
    ಹಿಂತಿರುಗಿ is_Valid_Native_ಬೇರು(cap);
}


BOOT_ಸಂಕೇತ bool_t map_kernel_ಕಿಟಕಿ_devices(pte_t *pt, uint32_t ಅಂಕೆ_ioapic, paddr_t *ioapic_paddrs, uint32_t ಅಂಕೆ_drhu,
                                           paddr_t *drhu_list)
{
    ಶಬ್ದ_t idx = (KDEV_ತಳ & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು;
    paddr_t phys;
    pte_t pte;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i;
    /* map kernel devices: APIC */
    phys = apic_get_ತಳ_paddr();
    ಆದರೆ (!phys) {
        ಹಿಂತಿರುಗಿ false;
    }
    ಆದರೆ (!reserve_region((p_region_t) {
    phys, phys + 0x1000
})) {
        ಹಿಂತಿರುಗಿ false;
    }
    pte = x86_make_device_pte(phys);

    ಪ್ರತಿಜ್ಞಾ(idx == (PPTR_APIC & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು);
    pt[idx] = pte;
    idx++;
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_ioapic; i++) {
        phys = ioapic_paddrs[i];
        ಆದರೆ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            ಹಿಂತಿರುಗಿ false;
        }
        pte = x86_make_device_pte(phys);
        ಪ್ರತಿಜ್ಞಾ(idx == ((PPTR_IOAPIC_ಆರಂಭ + i * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು);
        pt[idx] = pte;
        idx++;
        ಆದರೆ (idx == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)) {
            ಹಿಂತಿರುಗಿ false;
        }
    }
    /* put in null mappings for any extra IOAPICs */
    ಗಾಗಿ (; i < CONFIG_MAX_ಅಂಕೆ_IOAPIC; i++) {
        pte = x86_make_ಖಾಲಿಗಳು_pte();
        ಪ್ರತಿಜ್ಞಾ(idx == ((PPTR_IOAPIC_ಆರಂಭ + i * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು);
        pt[idx] = pte;
        idx++;
    }

    /* map kernel devices: IOMMUs */
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_drhu; i++) {
        phys = (paddr_t)drhu_list[i];
        ಆದರೆ (!reserve_region((p_region_t) {
        phys, phys + 0x1000
    })) {
            ಹಿಂತಿರುಗಿ false;
        }
        pte = x86_make_device_pte(phys);

        ಪ್ರತಿಜ್ಞಾ(idx == ((PPTR_DRHU_ಆರಂಭ + i * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು)) >> ಪುಟ_ಬಿಟ್ಟುಗಳು);
        pt[idx] = pte;
        idx++;
        ಆದರೆ (idx == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)) {
            ಹಿಂತಿರುಗಿ false;
        }
    }

    /* mark unused kernel-device pages as 'not present' */
    ಆಗ (idx < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು)) {
        pte = x86_make_ಖಾಲಿಗಳು_pte();
        pt[idx] = pte;
        idx++;
    }

    /* Check we haven't added too many kernel-device mappings.*/
    ಪ್ರತಿಜ್ಞಾ(idx == ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು));
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ init_idt(idt_ನಮೂದು_t *idt)
{
    init_idt_ನಮೂದು(idt, 0x00, int_00);
    init_idt_ನಮೂದು(idt, 0x01, int_01);
    init_idt_ನಮೂದು(idt, 0x02, int_02);
    init_idt_ನಮೂದು(idt, 0x03, int_03);
    init_idt_ನಮೂದು(idt, 0x04, int_04);
    init_idt_ನಮೂದು(idt, 0x05, int_05);
    init_idt_ನಮೂದು(idt, 0x06, int_06);
    init_idt_ನಮೂದು(idt, 0x07, int_07);
    init_idt_ನಮೂದು(idt, 0x08, int_08);
    init_idt_ನಮೂದು(idt, 0x09, int_09);
    init_idt_ನಮೂದು(idt, 0x0a, int_0a);
    init_idt_ನಮೂದು(idt, 0x0b, int_0b);
    init_idt_ನಮೂದು(idt, 0x0c, int_0c);
    init_idt_ನಮೂದು(idt, 0x0d, int_0d);
    init_idt_ನಮೂದು(idt, 0x0e, int_0e);
    init_idt_ನಮೂದು(idt, 0x0f, int_0f);

    init_idt_ನಮೂದು(idt, 0x10, int_10);
    init_idt_ನಮೂದು(idt, 0x11, int_11);
    init_idt_ನಮೂದು(idt, 0x12, int_12);
    init_idt_ನಮೂದು(idt, 0x13, int_13);
    init_idt_ನಮೂದು(idt, 0x14, int_14);
    init_idt_ನಮೂದು(idt, 0x15, int_15);
    init_idt_ನಮೂದು(idt, 0x16, int_16);
    init_idt_ನಮೂದು(idt, 0x17, int_17);
    init_idt_ನಮೂದು(idt, 0x18, int_18);
    init_idt_ನಮೂದು(idt, 0x19, int_19);
    init_idt_ನಮೂದು(idt, 0x1a, int_1a);
    init_idt_ನಮೂದು(idt, 0x1b, int_1b);
    init_idt_ನಮೂದು(idt, 0x1c, int_1c);
    init_idt_ನಮೂದು(idt, 0x1d, int_1d);
    init_idt_ನಮೂದು(idt, 0x1e, int_1e);
    init_idt_ನಮೂದು(idt, 0x1f, int_1f);

    init_idt_ನಮೂದು(idt, 0x20, int_20);
    init_idt_ನಮೂದು(idt, 0x21, int_21);
    init_idt_ನಮೂದು(idt, 0x22, int_22);
    init_idt_ನಮೂದು(idt, 0x23, int_23);
    init_idt_ನಮೂದು(idt, 0x24, int_24);
    init_idt_ನಮೂದು(idt, 0x25, int_25);
    init_idt_ನಮೂದು(idt, 0x26, int_26);
    init_idt_ನಮೂದು(idt, 0x27, int_27);
    init_idt_ನಮೂದು(idt, 0x28, int_28);
    init_idt_ನಮೂದು(idt, 0x29, int_29);
    init_idt_ನಮೂದು(idt, 0x2a, int_2a);
    init_idt_ನಮೂದು(idt, 0x2b, int_2b);
    init_idt_ನಮೂದು(idt, 0x2c, int_2c);
    init_idt_ನಮೂದು(idt, 0x2d, int_2d);
    init_idt_ನಮೂದು(idt, 0x2e, int_2e);
    init_idt_ನಮೂದು(idt, 0x2f, int_2f);

    init_idt_ನಮೂದು(idt, 0x30, int_30);
    init_idt_ನಮೂದು(idt, 0x31, int_31);
    init_idt_ನಮೂದು(idt, 0x32, int_32);
    init_idt_ನಮೂದು(idt, 0x33, int_33);
    init_idt_ನಮೂದು(idt, 0x34, int_34);
    init_idt_ನಮೂದು(idt, 0x35, int_35);
    init_idt_ನಮೂದು(idt, 0x36, int_36);
    init_idt_ನಮೂದು(idt, 0x37, int_37);
    init_idt_ನಮೂದು(idt, 0x38, int_38);
    init_idt_ನಮೂದು(idt, 0x39, int_39);
    init_idt_ನಮೂದು(idt, 0x3a, int_3a);
    init_idt_ನಮೂದು(idt, 0x3b, int_3b);
    init_idt_ನಮೂದು(idt, 0x3c, int_3c);
    init_idt_ನಮೂದು(idt, 0x3d, int_3d);
    init_idt_ನಮೂದು(idt, 0x3e, int_3e);
    init_idt_ನಮೂದು(idt, 0x3f, int_3f);

    init_idt_ನಮೂದು(idt, 0x40, int_40);
    init_idt_ನಮೂದು(idt, 0x41, int_41);
    init_idt_ನಮೂದು(idt, 0x42, int_42);
    init_idt_ನಮೂದು(idt, 0x43, int_43);
    init_idt_ನಮೂದು(idt, 0x44, int_44);
    init_idt_ನಮೂದು(idt, 0x45, int_45);
    init_idt_ನಮೂದು(idt, 0x46, int_46);
    init_idt_ನಮೂದು(idt, 0x47, int_47);
    init_idt_ನಮೂದು(idt, 0x48, int_48);
    init_idt_ನಮೂದು(idt, 0x49, int_49);
    init_idt_ನಮೂದು(idt, 0x4a, int_4a);
    init_idt_ನಮೂದು(idt, 0x4b, int_4b);
    init_idt_ನಮೂದು(idt, 0x4c, int_4c);
    init_idt_ನಮೂದು(idt, 0x4d, int_4d);
    init_idt_ನಮೂದು(idt, 0x4e, int_4e);
    init_idt_ನಮೂದು(idt, 0x4f, int_4f);

    init_idt_ನಮೂದು(idt, 0x50, int_50);
    init_idt_ನಮೂದು(idt, 0x51, int_51);
    init_idt_ನಮೂದು(idt, 0x52, int_52);
    init_idt_ನಮೂದು(idt, 0x53, int_53);
    init_idt_ನಮೂದು(idt, 0x54, int_54);
    init_idt_ನಮೂದು(idt, 0x55, int_55);
    init_idt_ನಮೂದು(idt, 0x56, int_56);
    init_idt_ನಮೂದು(idt, 0x57, int_57);
    init_idt_ನಮೂದು(idt, 0x58, int_58);
    init_idt_ನಮೂದು(idt, 0x59, int_59);
    init_idt_ನಮೂದು(idt, 0x5a, int_5a);
    init_idt_ನಮೂದು(idt, 0x5b, int_5b);
    init_idt_ನಮೂದು(idt, 0x5c, int_5c);
    init_idt_ನಮೂದು(idt, 0x5d, int_5d);
    init_idt_ನಮೂದು(idt, 0x5e, int_5e);
    init_idt_ನಮೂದು(idt, 0x5f, int_5f);

    init_idt_ನಮೂದು(idt, 0x60, int_60);
    init_idt_ನಮೂದು(idt, 0x61, int_61);
    init_idt_ನಮೂದು(idt, 0x62, int_62);
    init_idt_ನಮೂದು(idt, 0x63, int_63);
    init_idt_ನಮೂದು(idt, 0x64, int_64);
    init_idt_ನಮೂದು(idt, 0x65, int_65);
    init_idt_ನಮೂದು(idt, 0x66, int_66);
    init_idt_ನಮೂದು(idt, 0x67, int_67);
    init_idt_ನಮೂದು(idt, 0x68, int_68);
    init_idt_ನಮೂದು(idt, 0x69, int_69);
    init_idt_ನಮೂದು(idt, 0x6a, int_6a);
    init_idt_ನಮೂದು(idt, 0x6b, int_6b);
    init_idt_ನಮೂದು(idt, 0x6c, int_6c);
    init_idt_ನಮೂದು(idt, 0x6d, int_6d);
    init_idt_ನಮೂದು(idt, 0x6e, int_6e);
    init_idt_ನಮೂದು(idt, 0x6f, int_6f);

    init_idt_ನಮೂದು(idt, 0x70, int_70);
    init_idt_ನಮೂದು(idt, 0x71, int_71);
    init_idt_ನಮೂದು(idt, 0x72, int_72);
    init_idt_ನಮೂದು(idt, 0x73, int_73);
    init_idt_ನಮೂದು(idt, 0x74, int_74);
    init_idt_ನಮೂದು(idt, 0x75, int_75);
    init_idt_ನಮೂದು(idt, 0x76, int_76);
    init_idt_ನಮೂದು(idt, 0x77, int_77);
    init_idt_ನಮೂದು(idt, 0x78, int_78);
    init_idt_ನಮೂದು(idt, 0x79, int_79);
    init_idt_ನಮೂದು(idt, 0x7a, int_7a);
    init_idt_ನಮೂದು(idt, 0x7b, int_7b);
    init_idt_ನಮೂದು(idt, 0x7c, int_7c);
    init_idt_ನಮೂದು(idt, 0x7d, int_7d);
    init_idt_ನಮೂದು(idt, 0x7e, int_7e);
    init_idt_ನಮೂದು(idt, 0x7f, int_7f);

    init_idt_ನಮೂದು(idt, 0x80, int_80);
    init_idt_ನಮೂದು(idt, 0x81, int_81);
    init_idt_ನಮೂದು(idt, 0x82, int_82);
    init_idt_ನಮೂದು(idt, 0x83, int_83);
    init_idt_ನಮೂದು(idt, 0x84, int_84);
    init_idt_ನಮೂದು(idt, 0x85, int_85);
    init_idt_ನಮೂದು(idt, 0x86, int_86);
    init_idt_ನಮೂದು(idt, 0x87, int_87);
    init_idt_ನಮೂದು(idt, 0x88, int_88);
    init_idt_ನಮೂದು(idt, 0x89, int_89);
    init_idt_ನಮೂದು(idt, 0x8a, int_8a);
    init_idt_ನಮೂದು(idt, 0x8b, int_8b);
    init_idt_ನಮೂದು(idt, 0x8c, int_8c);
    init_idt_ನಮೂದು(idt, 0x8d, int_8d);
    init_idt_ನಮೂದು(idt, 0x8e, int_8e);
    init_idt_ನಮೂದು(idt, 0x8f, int_8f);

    init_idt_ನಮೂದು(idt, 0x90, int_90);
    init_idt_ನಮೂದು(idt, 0x91, int_91);
    init_idt_ನಮೂದು(idt, 0x92, int_92);
    init_idt_ನಮೂದು(idt, 0x93, int_93);
    init_idt_ನಮೂದು(idt, 0x94, int_94);
    init_idt_ನಮೂದು(idt, 0x95, int_95);
    init_idt_ನಮೂದು(idt, 0x96, int_96);
    init_idt_ನಮೂದು(idt, 0x97, int_97);
    init_idt_ನಮೂದು(idt, 0x98, int_98);
    init_idt_ನಮೂದು(idt, 0x99, int_99);
    init_idt_ನಮೂದು(idt, 0x9a, int_9a);
    init_idt_ನಮೂದು(idt, 0x9b, int_9b);
    init_idt_ನಮೂದು(idt, 0x9c, int_9c);
    init_idt_ನಮೂದು(idt, 0x9d, int_9d);
    init_idt_ನಮೂದು(idt, 0x9e, int_9e);
    init_idt_ನಮೂದು(idt, 0x9f, int_9f);

    init_idt_ನಮೂದು(idt, 0xa0, int_a0);
    init_idt_ನಮೂದು(idt, 0xa1, int_a1);
    init_idt_ನಮೂದು(idt, 0xa2, int_a2);
    init_idt_ನಮೂದು(idt, 0xa3, int_a3);
    init_idt_ನಮೂದು(idt, 0xa4, int_a4);
    init_idt_ನಮೂದು(idt, 0xa5, int_a5);
    init_idt_ನಮೂದು(idt, 0xa6, int_a6);
    init_idt_ನಮೂದು(idt, 0xa7, int_a7);
    init_idt_ನಮೂದು(idt, 0xa8, int_a8);
    init_idt_ನಮೂದು(idt, 0xa9, int_a9);
    init_idt_ನಮೂದು(idt, 0xaa, int_aa);
    init_idt_ನಮೂದು(idt, 0xab, int_ab);
    init_idt_ನಮೂದು(idt, 0xac, int_ac);
    init_idt_ನಮೂದು(idt, 0xad, int_ad);
    init_idt_ನಮೂದು(idt, 0xae, int_ae);
    init_idt_ನಮೂದು(idt, 0xaf, int_af);

    init_idt_ನಮೂದು(idt, 0xb0, int_b0);
    init_idt_ನಮೂದು(idt, 0xb1, int_b1);
    init_idt_ನಮೂದು(idt, 0xb2, int_b2);
    init_idt_ನಮೂದು(idt, 0xb3, int_b3);
    init_idt_ನಮೂದು(idt, 0xb4, int_b4);
    init_idt_ನಮೂದು(idt, 0xb5, int_b5);
    init_idt_ನಮೂದು(idt, 0xb6, int_b6);
    init_idt_ನಮೂದು(idt, 0xb7, int_b7);
    init_idt_ನಮೂದು(idt, 0xb8, int_b8);
    init_idt_ನಮೂದು(idt, 0xb9, int_b9);
    init_idt_ನಮೂದು(idt, 0xba, int_ba);
    init_idt_ನಮೂದು(idt, 0xbb, int_bb);
    init_idt_ನಮೂದು(idt, 0xbc, int_bc);
    init_idt_ನಮೂದು(idt, 0xbd, int_bd);
    init_idt_ನಮೂದು(idt, 0xbe, int_be);
    init_idt_ನಮೂದು(idt, 0xbf, int_bf);

    init_idt_ನಮೂದು(idt, 0xc0, int_c0);
    init_idt_ನಮೂದು(idt, 0xc1, int_c1);
    init_idt_ನಮೂದು(idt, 0xc2, int_c2);
    init_idt_ನಮೂದು(idt, 0xc3, int_c3);
    init_idt_ನಮೂದು(idt, 0xc4, int_c4);
    init_idt_ನಮೂದು(idt, 0xc5, int_c5);
    init_idt_ನಮೂದು(idt, 0xc6, int_c6);
    init_idt_ನಮೂದು(idt, 0xc7, int_c7);
    init_idt_ನಮೂದು(idt, 0xc8, int_c8);
    init_idt_ನಮೂದು(idt, 0xc9, int_c9);
    init_idt_ನಮೂದು(idt, 0xca, int_ca);
    init_idt_ನಮೂದು(idt, 0xcb, int_cb);
    init_idt_ನಮೂದು(idt, 0xcc, int_cc);
    init_idt_ನಮೂದು(idt, 0xcd, int_cd);
    init_idt_ನಮೂದು(idt, 0xce, int_ce);
    init_idt_ನಮೂದು(idt, 0xcf, int_cf);

    init_idt_ನಮೂದು(idt, 0xd0, int_d0);
    init_idt_ನಮೂದು(idt, 0xd1, int_d1);
    init_idt_ನಮೂದು(idt, 0xd2, int_d2);
    init_idt_ನಮೂದು(idt, 0xd3, int_d3);
    init_idt_ನಮೂದು(idt, 0xd4, int_d4);
    init_idt_ನಮೂದು(idt, 0xd5, int_d5);
    init_idt_ನಮೂದು(idt, 0xd6, int_d6);
    init_idt_ನಮೂದು(idt, 0xd7, int_d7);
    init_idt_ನಮೂದು(idt, 0xd8, int_d8);
    init_idt_ನಮೂದು(idt, 0xd9, int_d9);
    init_idt_ನಮೂದು(idt, 0xda, int_da);
    init_idt_ನಮೂದು(idt, 0xdb, int_db);
    init_idt_ನಮೂದು(idt, 0xdc, int_dc);
    init_idt_ನಮೂದು(idt, 0xdd, int_dd);
    init_idt_ನಮೂದು(idt, 0xde, int_de);
    init_idt_ನಮೂದು(idt, 0xdf, int_df);

    init_idt_ನಮೂದು(idt, 0xe0, int_e0);
    init_idt_ನಮೂದು(idt, 0xe1, int_e1);
    init_idt_ನಮೂದು(idt, 0xe2, int_e2);
    init_idt_ನಮೂದು(idt, 0xe3, int_e3);
    init_idt_ನಮೂದು(idt, 0xe4, int_e4);
    init_idt_ನಮೂದು(idt, 0xe5, int_e5);
    init_idt_ನಮೂದು(idt, 0xe6, int_e6);
    init_idt_ನಮೂದು(idt, 0xe7, int_e7);
    init_idt_ನಮೂದು(idt, 0xe8, int_e8);
    init_idt_ನಮೂದು(idt, 0xe9, int_e9);
    init_idt_ನಮೂದು(idt, 0xea, int_ea);
    init_idt_ನಮೂದು(idt, 0xeb, int_eb);
    init_idt_ನಮೂದು(idt, 0xec, int_ec);
    init_idt_ನಮೂದು(idt, 0xed, int_ed);
    init_idt_ನಮೂದು(idt, 0xee, int_ee);
    init_idt_ನಮೂದು(idt, 0xef, int_ef);

    init_idt_ನಮೂದು(idt, 0xf0, int_f0);
    init_idt_ನಮೂದು(idt, 0xf1, int_f1);
    init_idt_ನಮೂದು(idt, 0xf2, int_f2);
    init_idt_ನಮೂದು(idt, 0xf3, int_f3);
    init_idt_ನಮೂದು(idt, 0xf4, int_f4);
    init_idt_ನಮೂದು(idt, 0xf5, int_f5);
    init_idt_ನಮೂದು(idt, 0xf6, int_f6);
    init_idt_ನಮೂದು(idt, 0xf7, int_f7);
    init_idt_ನಮೂದು(idt, 0xf8, int_f8);
    init_idt_ನಮೂದು(idt, 0xf9, int_f9);
    init_idt_ನಮೂದು(idt, 0xfa, int_fa);
    init_idt_ನಮೂದು(idt, 0xfb, int_fb);
    init_idt_ನಮೂದು(idt, 0xfc, int_fc);
    init_idt_ನಮೂದು(idt, 0xfd, int_fd);
    init_idt_ನಮೂದು(idt, 0xfe, int_fe);
    init_idt_ನಮೂದು(idt, 0xff, int_ff);
}

BOOT_ಸಂಕೇತ bool_t init_vm_state(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t cache_ಸಾಲು_ಗಾತ್ರ;
    x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = get_Cache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು();
    ಆದರೆ (!x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) {
        ಹಿಂತಿರುಗಿ false;
    }

    cache_ಸಾಲು_ಗಾತ್ರ = ಬಿಟ್(x86_KScache_ಸಾಲು_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    ಆದರೆ (cache_ಸಾಲು_ಗಾತ್ರ != L1_CACHE_ಸಾಲು_ಗಾತ್ರ) {
        ಬರೆ("Configured cache line size is %d but detected size is %lu\n",
               L1_CACHE_ಸಾಲು_ಗಾತ್ರ, cache_ಸಾಲು_ಗಾತ್ರ);
        SMP_COND_STATEMENT(ಹಿಂತಿರುಗಿ false);
    }

    /*
     * Work around -Waddress-of-packed-member. TSS is the first thing
     * in the struct and so it's safe to take its address.
     */
    ಶೂನ್ಯ *tss_ptr = &x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KStss.tss;
    init_tss(tss_ptr);
    init_gdt(x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSgdt, tss_ptr);
    init_idt(x86KSGlobalState[CURRENT_CPU_ಸೂಚ್ಯಂಕ()].x86KSidt);
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ bool_t init_pat_msr(ಶೂನ್ಯ)
{
    x86_pat_msr_t pat_msr;
    /* First verify PAT is supported by the machine.
     *      See section 11.12.1 of Volume 3 of the Intel manual */
    ಆದರೆ ((x86_cpuid_edx(0x1, 0x0) & ಬಿಟ್(16)) == 0) {
        ಬರೆ("PAT support not found\n");
        ಹಿಂತಿರುಗಿ false;
    }
    pat_msr.ಪದಗಳು[0] = x86_rdmsr_ಕೆಳಗಿನ(IA32_PAT_MSR);
    pat_msr.ಪದಗಳು[1] = x86_rdmsr_ಮೇಲಿನ(IA32_PAT_MSR);
    /* Set up the PAT MSR to the Intel defaults, just in case
     * they have been changed but a bootloader somewhere along the way */
    pat_msr = x86_pat_msr_ಗಣ_pa0(pat_msr, IA32_PAT_MT_WRITE_ಹಿಂಭಾಗ);
    pat_msr = x86_pat_msr_ಗಣ_pa1(pat_msr, IA32_PAT_MT_WRITE_THROUGH);
    pat_msr = x86_pat_msr_ಗಣ_pa2(pat_msr, IA32_PAT_MT_UNCACHED);
    pat_msr = x86_pat_msr_ಗಣ_pa3(pat_msr, IA32_PAT_MT_UNCACHEABLE);
    /* Add the WriteCombining cache type to the PAT */
    pat_msr = x86_pat_msr_ಗಣ_pa4(pat_msr, IA32_PAT_MT_WRITE_COMBINING);
    x86_wrmsr(IA32_PAT_MSR, ((uint64_t)pat_msr.ಪದಗಳು[1]) << 32 | pat_msr.ಪದಗಳು[0]);
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ write_it_asid_pool(cap_t it_ap_cap, cap_t it_vspace_cap)
{
    asid_pool_t *ap = ASID_POOL_PTR(pptr_ದ_cap(it_ap_cap));
    ap->array[ASID_ಕೆಳಗಿನ(IT_ASID)] = asid_map_asid_map_vspace_new(pptr_ದ_cap(it_vspace_cap));
    x86KSASIDTable[ASID_ಮೇಲಿನ(IT_ASID)] = ap;
}

asid_map_t ಕಂಡುಹಿಡಿ_Map_For_ASID(asid_t asid)
{
    asid_pool_t        *poolPtr;

    poolPtr = x86KSASIDTable[ASID_ಮೇಲಿನ(asid)];
    ಆದರೆ (!poolPtr) {
        ಹಿಂತಿರುಗಿ asid_map_asid_map_none_new();
    }

    ಹಿಂತಿರುಗಿ poolPtr->array[ASID_ಕೆಳಗಿನ(asid)];
}

ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid_t asid)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ret;
    asid_map_t asid_map;

    asid_map = ಕಂಡುಹಿಡಿ_Map_For_ASID(asid);
    ಆದರೆ (asid_map_get_type(asid_map) != asid_map_asid_map_vspace) {
        current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();

        ret.vspace_ಬೇರು = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.vspace_ಬೇರು = (vspace_ಬೇರು_t *)asid_map_asid_map_vspace_get_vspace_ಬೇರು(asid_map);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

exception_t handleVMFault(tcb_t *thread, vm_fault_type_t vm_faultType)
{
    ಶಬ್ದ_t addr;
    uint32_t fault;

    addr = getFaultAddr();
    fault = get_ರಿಜಿಸ್ಟರ್(thread, Error);

    ಆಯ್ಕೆ (vm_faultType) {
    ಸ್ಥಿತಿ X86_ದತ್ತಾಂಶ_Fault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, false);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;

    ಸ್ಥಿತಿ X86InstructionFault:
        current_fault = seL4_Fault_VMFault_new(addr, fault, true);
        ಹಿಂತಿರುಗಿ EXCEPTION_FAULT;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM fault type");
    }
}

uint32_t CONST WritableFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMReadOnly:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ VMKernelOnly:
    ಸ್ಥಿತಿ VMReadWrite:
        ಹಿಂತಿರುಗಿ 1;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

uint32_t CONST SuperUserFromVMRights(vm_rights_t vm_rights)
{
    ಆಯ್ಕೆ (vm_rights) {
    ಸ್ಥಿತಿ VMKernelOnly:
        ಹಿಂತಿರುಗಿ 0;

    ಸ್ಥಿತಿ VMReadOnly:
    ಸ್ಥಿತಿ VMReadWrite:
        ಹಿಂತಿರುಗಿ 1;

    ಪೂರ್ವನಿಯೋಜಿತ:
        fail("Invalid VM rights");
    }
}

lookupPTSlot_ret_t lookupPTSlot(vspace_ಬೇರು_t *vspace, vptr_t vptr)
{
    lookupPTSlot_ret_t ret;
    lookupPDSlot_ret_t pd_ಸ್ಥಾನ;

    pd_ಸ್ಥಾನ = lookupPDSlot(vspace, vptr);
    ಆದರೆ (pd_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ret.pt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = pd_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2;
        ಹಿಂತಿರುಗಿ ret;
    }
    ಆದರೆ ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ) != pde_pde_pt) ||
        !pde_pde_pt_ptr_get_present(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ)) {
        current_lookup_fault = lookup_fault_missing_capability_new(ಪುಟ_ಬಿಟ್ಟುಗಳು + PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
        ret.pt_ಸ್ಥಾನ = ಖಾಲಿ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_LOOKUP_FAULT;
        ಹಿಂತಿರುಗಿ ret;
    } ಇಲ್ಲದಿದ್ದರೆ {
        pte_t *pt;
        pte_t *pt_ಸ್ಥಾನ;
        ಶಬ್ದ_t pt_ಸೂಚ್ಯಂಕ;

        pt = paddr_ಗಮ್ಯ_pptr(pde_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ));
        pt_ಸೂಚ್ಯಂಕ = (vptr >> ಪುಟ_ಬಿಟ್ಟುಗಳು) & ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು);
        pt_ಸ್ಥಾನ = pt + pt_ಸೂಚ್ಯಂಕ;

        ret.pt_ಸ್ಥಾನ = pt_ಸ್ಥಾನ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
        ಹಿಂತಿರುಗಿ ret;
    }
}

exception_t checkValidIPCBuffer(vptr_t vptr, cap_t cap)
{
    ಆದರೆ (cap_get_capType(cap) != cap_ಚೌಕಟ್ಟು_cap) {
        userError("IPC Buffer is an invalid cap.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (unlikely(cap_ಚೌಕಟ್ಟು_cap_get_cap_FIs_Device(cap))) {
        userError("Specifying a device frame as an IPC buffer is not permitted.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (!IS_ALIGNED(vptr, se_L4_IPCBuffer_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        userError("IPC Buffer vaddr 0x%x is not aligned.", (ಪೂರ್ಣಾಂಕ)vptr);
        current_syscall_error.type = seL4_AlignmentError;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

vm_rights_t CONST ಮುಖವಾಡ_VMRights(vm_rights_t vm_rights, seL4_CapRights_t cap_rights_ಮುಖವಾಡ)
{
    ಆದರೆ (vm_rights == VMReadOnly && seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಹಿಂತಿರುಗಿ VMReadOnly;
    }
    ಆದರೆ (vm_rights == VMReadWrite && seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ)) {
        ಆದರೆ (!seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
            ಹಿಂತಿರುಗಿ VMReadOnly;
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಹಿಂತಿರುಗಿ VMReadWrite;
        }
    }
    ಆದರೆ (vm_rights == VMReadWrite &&
        !seL4_CapRights_get_capAllowRead(cap_rights_ಮುಖವಾಡ) &&
        seL4_CapRights_get_capAllowWrite(cap_rights_ಮುಖವಾಡ)) {
        userError("Attempted to make unsupported write only mapping");
    }
    ಹಿಂತಿರುಗಿ VMKernelOnly;
}

ಶೂನ್ಯ flush_ಕೋಷ್ಟಕ(vspace_ಬೇರು_t *vspace, ಶಬ್ದ_t vptr, pte_t *pt, asid_t asid)
{
    ಶಬ್ದ_t i;
    cap_t        thread_ಬೇರು;

    ಪ್ರತಿಜ್ಞಾ(IS_ALIGNED(vptr, PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು));

    /* check if page table belongs to current address space */
    thread_ಬೇರು = TCB_PTR_CTE_PTR(ಗಂಟು_STATE(ksCurThread), tcbVTable)->cap;
    /* find valid mappings */
    ಗಾಗಿ (i = 0; i < ಬಿಟ್(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು); i++) {
        ಆದರೆ (pte_get_present(pt[i])) {
            ಆದರೆ (config_ಗಣ(CONFIG_SUPPORT_PCID) || (is_Valid_Native_ಬೇರು(thread_ಬೇರು)
                                                    && (vspace_ಬೇರು_t *)pptr_ದ_cap(thread_ಬೇರು) == vspace)) {
                invalidateTranslationSingleASID(vptr + (i << ಪುಟ_ಬಿಟ್ಟುಗಳು), asid,
                                                SMP_TERNARY(tlb_bitmap_get(vspace), 0));
            }
        }
    }
}


ಶೂನ್ಯ unmap_ಪುಟ(vm_ಪುಟ_ಗಾತ್ರ_t ಪುಟ_ಗಾತ್ರ, asid_t asid, vptr_t vptr, ಶೂನ್ಯ *pptr)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupPTSlot_ret_t  lu_ret;
    lookupPDSlot_ret_t  pd_ret;
    pde_t               *pde;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    ಆಯ್ಕೆ (ಪುಟ_ಗಾತ್ರ) {
    ಸ್ಥಿತಿ X86_Small_ಪುಟ:
        lu_ret = lookupPTSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ;
        }
        ಆದರೆ (!(pte_ptr_get_present(lu_ret.pt_ಸ್ಥಾನ)
              && (pte_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(lu_ret.pt_ಸ್ಥಾನ)
                  == pptr_ಗಮ್ಯ_paddr(pptr)))) {
            ಹಿಂತಿರುಗಿ;
        }
        *lu_ret.pt_ಸ್ಥಾನ = makeUserPTEInvalid();
        ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ X86_Large_ಪುಟ:
        pd_ret = lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr);
        ಆದರೆ (pd_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ;
        }
        pde = pd_ret.pd_ಸ್ಥಾನ;
        ಆದರೆ (!(pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pde) == pde_pde_large
              && pde_pde_large_ptr_get_present(pde)
              && (pde_pde_large_ptr_get_ಪುಟ_ತಳ_ವಿಳಾಸ(pde)
                  == pptr_ಗಮ್ಯ_paddr(pptr)))) {
            ಹಿಂತಿರುಗಿ;
        }
        *pde = makeUserPDEInvalid();
        ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಆದರೆ (!ಬಹುಲಕ_Unmap_ಪುಟ(ಪುಟ_ಗಾತ್ರ, ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vptr, pptr)) {
            ಹಿಂತಿರುಗಿ;
        }
        ನಿಲ್ಲಿಸು;
    }

    invalidateTranslationSingleASID(vptr, asid,
                                    SMP_TERNARY(tlb_bitmap_get(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು), 0));
}

ಶೂನ್ಯ unmap_ಪುಟ_ಕೋಷ್ಟಕ(asid_t asid, vptr_t vaddr, pte_t *pt)
{
    ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;
    lookupPDSlot_ret_t    lu_ret;

    ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
    ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    lu_ret = lookupPDSlot(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಹಿಂತಿರುಗಿ;
    }

    /* check if the PD actually refers to the PT */
    ಆದರೆ (!(pde_ptr_get_ಪುಟ_ಗಾತ್ರ(lu_ret.pd_ಸ್ಥಾನ) == pde_pde_pt &&
          pde_pde_pt_ptr_get_present(lu_ret.pd_ಸ್ಥಾನ) &&
          (pde_pde_pt_ptr_get_pt_ತಳ_ವಿಳಾಸ(lu_ret.pd_ಸ್ಥಾನ) == pptr_ಗಮ್ಯ_paddr(pt)))) {
        ಹಿಂತಿರುಗಿ;
    }

    flush_ಕೋಷ್ಟಕ(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು, vaddr, pt, asid);

    *lu_ret.pd_ಸ್ಥಾನ = makeUserPDEInvalid();

    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು), asid,
                                     SMP_TERNARY(tlb_bitmap_get(ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು), 0));
}

ಸ್ಥಿರ exception_t perform_X86_ಪುಟ_Invocation_Map_PTE(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pte_t *pt_ಸ್ಥಾನ, pte_t pte,
                                                  vspace_ಬೇರು_t *vspace)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pt_ಸ್ಥಾನ = pte;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(vspace), cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X86_ಪುಟ_Invocation_Map_PDE(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pde_t *pd_ಸ್ಥಾನ, pde_t pde,
                                                  vspace_ಬೇರು_t *vspace)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pd_ಸ್ಥಾನ = pde;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(vspace), cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(vspace), 0));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}


ಸ್ಥಿರ exception_t perform_X86_ಪುಟ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{
    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap));
    ಪ್ರತಿಜ್ಞಾ(cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap) == X86_MappingVSpace);
    // We have this `if` for something we just asserted to be true for simplicity of verification
    // This has no performance implications as when this function is inlined this `if` will be
    // inside an identical `if` and will therefore be elided
    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap)) {
        unmap_ಪುಟ(
            cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap),
            cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap),
            cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap),
            (ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap)
        );
    }

    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMapped_ವಿಳಾಸ(&ct_ಸ್ಥಾನ->cap, 0);
    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMapped_ASID(&ct_ಸ್ಥಾನ->cap, asidInvalid);
    cap_ಚೌಕಟ್ಟು_cap_ptr_ಗಣ_cap_FMap_Type(&ct_ಸ್ಥಾನ->cap, X86_MappingNone);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X86_ಚೌಕಟ್ಟು_Invocation_Unmap(cap_t cap, cte_t *cte)
{
    ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
        ಆಯ್ಕೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap)) {
        ಸ್ಥಿತಿ X86_MappingVSpace:
            ಹಿಂತಿರುಗಿ perform_X86_ಪುಟ_Invocation_Unmap(cap, cte);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
        ಸ್ಥಿತಿ X86_MappingIOSpace:
            ಹಿಂತಿರುಗಿ performX86IOUnMapInvocation(cap, cte);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
        ಸ್ಥಿತಿ X86_MappingEPT:
            ಹಿಂತಿರುಗಿ performX86EPTPageInvocationUnmap(cap, cte);
#ಆದರೆ_ಅಂತ್ಯ
        ಸ್ಥಿತಿ X86_MappingNone:
            fail("Mapped frame cap was not mapped");
            ನಿಲ್ಲಿಸು;
        }
    }

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ರಚನೆ create_mapping_pte_return {
    exception_t ಸ್ಥಿತಿ_2;
    pte_t pte;
    pte_t *pt_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ create_mapping_pte_return create_mapping_pte_return_t;

ಸ್ಥಿರ create_mapping_pte_return_t create_ಸುರಕ್ಷಿತ_Mapping_Entries_PTE(paddr_t ತಳ, ಶಬ್ದ_t vaddr, vm_rights_t vmRights,
                                                                vm_ಗುಣಲಕ್ಷಣಗಳು_t attr,
                                                                vspace_ಬೇರು_t *vspace)
{
    create_mapping_pte_return_t ret;
    lookupPTSlot_ret_t          lu_ret;

    lu_ret = lookupPTSlot(vspace, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPTSlot */
        ಹಿಂತಿರುಗಿ ret;
    }

    ret.pte = makeUserPTE(ತಳ, attr, vmRights);
    ret.pt_ಸ್ಥಾನ = lu_ret.pt_ಸ್ಥಾನ;
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}

ರಚನೆ create_mapping_pde_return {
    exception_t ಸ್ಥಿತಿ_2;
    pde_t pde;
    pde_t *pd_ಸ್ಥಾನ;
};
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ create_mapping_pde_return create_mapping_pde_return_t;

ಸ್ಥಿರ create_mapping_pde_return_t create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDE(paddr_t ತಳ, ಶಬ್ದ_t vaddr, vm_rights_t vmRights,
                                                                vm_ಗುಣಲಕ್ಷಣಗಳು_t attr,
                                                                vspace_ಬೇರು_t *vspace)
{
    create_mapping_pde_return_t ret;
    lookupPDSlot_ret_t          lu_ret;

    lu_ret = lookupPDSlot(vspace, vaddr);
    ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        /* current_lookup_fault will have been set by lookupPDSlot */
        ಹಿಂತಿರುಗಿ ret;
    }
    ret.pd_ಸ್ಥಾನ = lu_ret.pd_ಸ್ಥಾನ;

    /* check for existing page table */
    ಆದರೆ ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(ret.pd_ಸ್ಥಾನ) == pde_pde_pt) &&
        (pde_pde_pt_ptr_get_present(ret.pd_ಸ್ಥಾನ))) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;
        ret.ಸ್ಥಿತಿ_2 = EXCEPTION_SYSCALL_ERROR;
        ಹಿಂತಿರುಗಿ ret;
    }


    ret.pde = make_User_PDELarge_ಪುಟ(ತಳ, attr, vmRights);
    ret.ಸ್ಥಿತಿ_2 = EXCEPTION_NONE;
    ಹಿಂತಿರುಗಿ ret;
}


exception_t decode_X86_ಚೌಕಟ್ಟು_Invocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (invLabel) {
    ಸ್ಥಿತಿ X86_ಪುಟ_Map: { /* Map */
        ಶಬ್ದ_t          vaddr;
        ಶಬ್ದ_t          vtop;
        ಶಬ್ದ_t          w_rights_ಮುಖವಾಡ;
        paddr_t         paddr;
        cap_t           vspaceCap;
        vspace_ಬೇರು_t  *vspace;
        vm_rights_t     capVMRights;
        vm_rights_t     vmRights;
        vm_ಗುಣಲಕ್ಷಣಗಳು_t vmAttr;
        vm_ಪುಟ_ಗಾತ್ರ_t  ಚೌಕಟ್ಟು_ಗಾತ್ರ;
        asid_t          asid;

        ಆದರೆ (ಉದ್ದಳತೆ < 3 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಚೌಕಟ್ಟು_ಗಾತ್ರ = cap_ಚೌಕಟ್ಟು_cap_get_cap_FSize(cap);
        vaddr = getSyscallArg(0, ಬಫರ್);
        w_rights_ಮುಖವಾಡ = getSyscallArg(1, ಬಫರ್);
        vmAttr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(2, ಬಫರ್));
        vspaceCap = current_extra_caps.excaprefs[0]->cap;

        capVMRights = cap_ಚೌಕಟ್ಟು_cap_get_cap_FVMRights(cap);

        ಆದರೆ (!is_Valid_Native_ಬೇರು(vspaceCap)) {
            userError("X86FrameMap: Attempting to map frame into invalid page directory cap.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        vspace = (vspace_ಬೇರು_t *)pptr_ದ_cap(vspaceCap);
        asid = cap_get_capMappedASID(vspaceCap);

        ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asidInvalid) {
            ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ASID(cap) != asid) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMap_Type(cap) != X86_MappingVSpace) {
                userError("X86Frame: Attempting to remap frame with different mapping type");
                current_syscall_error.type = seL4_IllegalOperation;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            ಆದರೆ (cap_ಚೌಕಟ್ಟು_cap_get_cap_FMapped_ವಿಳಾಸ(cap) != vaddr) {
                userError("X86Frame: Attempting to map frame into multiple addresses");
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        } ಇಲ್ಲದಿದ್ದರೆ {
            vtop = vaddr + ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು_For_ಗಾತ್ರ(ಚೌಕಟ್ಟು_ಗಾತ್ರ));

            /* check vaddr and vtop against USER_TOP to catch case where vaddr + frame_size wrapped around */
            ಆದರೆ (vaddr > USER_ತುದಿ || vtop > USER_ತುದಿ) {
                userError("X86Frame: Mapping address too high.");
                current_syscall_error.type = se_L4_Invalid_ವಾದ;
                current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        {
            ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

            ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
            ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                current_syscall_error.type = seL4_FailedLookup;
                current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }

            ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace) {
                current_syscall_error.type = seL4_InvalidCapability;
                current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

                ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
            }
        }

        vmRights = ಮುಖವಾಡ_VMRights(capVMRights, rights_From_ಶಬ್ದ(w_rights_ಮುಖವಾಡ));

        ಆದರೆ (!checkVPAlignment(ಚೌಕಟ್ಟು_ಗಾತ್ರ, vaddr)) {
            current_syscall_error.type = seL4_AlignmentError;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        paddr = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap));

        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ASID(cap, asid);
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMapped_ವಿಳಾಸ(cap, vaddr);
        cap = cap_ಚೌಕಟ್ಟು_cap_ಗಣ_cap_FMap_Type(cap, X86_MappingVSpace);

        ಆಯ್ಕೆ (ಚೌಕಟ್ಟು_ಗಾತ್ರ) {
        /* PTE mappings */
        ಸ್ಥಿತಿ X86_Small_ಪುಟ: {
            create_mapping_pte_return_t map_ret;

            map_ret = create_ಸುರಕ್ಷಿತ_Mapping_Entries_PTE(paddr, vaddr, vmRights, vmAttr, vspace);
            ಆದರೆ (map_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                ಹಿಂತಿರುಗಿ map_ret.ಸ್ಥಿತಿ_2;
            }

            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_X86_ಪುಟ_Invocation_Map_PTE(cap, cte, map_ret.pt_ಸ್ಥಾನ, map_ret.pte, vspace);
        }

        /* PDE mappings */
        ಸ್ಥಿತಿ X86_Large_ಪುಟ: {
            create_mapping_pde_return_t map_ret;

            map_ret = create_ಸುರಕ್ಷಿತ_Mapping_Entries_PDE(paddr, vaddr, vmRights, vmAttr, vspace);
            ಆದರೆ (map_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
                ಹಿಂತಿರುಗಿ map_ret.ಸ್ಥಿತಿ_2;
            }

            ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
            ಹಿಂತಿರುಗಿ perform_X86_ಪುಟ_Invocation_Map_PDE(cap, cte, map_ret.pd_ಸ್ಥಾನ, map_ret.pde, vspace);
        }

        ಪೂರ್ವನಿಯೋಜಿತ: {
            ಹಿಂತಿರುಗಿ decode_X86_ಬಹುಲಕ_Map_ಪುಟ(invLabel, ಚೌಕಟ್ಟು_ಗಾತ್ರ, cte, cap, vspace, vaddr, paddr, vmRights, vmAttr);
        }
        }

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಸ್ಥಿತಿ X86_ಪುಟ_Unmap: { /* Unmap */
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_X86_ಚೌಕಟ್ಟು_Invocation_Unmap(cap, cte);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_IOMMU
    ಸ್ಥಿತಿ X86_ಪುಟ_Map_IO: { /* MapIO */
        ಹಿಂತಿರುಗಿ decodeX86IOMapInvocation(ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_VTX
    ಸ್ಥಿತಿ X86_ಪುಟ_Map_EPT:
        ಹಿಂತಿರುಗಿ decodeX86EPTPageMap(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿತಿ X86_ಪುಟ_Get_ವಿಳಾಸ: {
        /* Return it in the first message register. */
        ಪ್ರತಿಜ್ಞಾ(n_ಸಂದೇಶ_Registers >= 1);

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_ಪುಟ_Get_ವಿಳಾಸ((ಶೂನ್ಯ *)cap_ಚೌಕಟ್ಟು_cap_get_cap_FBase_Ptr(cap), call);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        current_syscall_error.type = seL4_IllegalOperation;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
}

ಸ್ಥಿರ exception_t perform_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap_t cap, cte_t *ct_ಸ್ಥಾನ)
{

    ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
        pte_t *pt = PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap));
        unmap_ಪುಟ_ಕೋಷ್ಟಕ(
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
            cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ವಿಳಾಸ(cap),
            pt
        );
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)pt, cap_get_cap_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(cap));
    }
    cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ptr_ಗಣ_cap_PTIs_Mapped(&(ct_ಸ್ಥಾನ->cap), 0);

    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t perform_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap_t cap, cte_t *ct_ಸ್ಥಾನ, pde_t pde, pde_t *pd_ಸ್ಥಾನ,
                                                    vspace_ಬೇರು_t *ಬೇರು)
{
    ct_ಸ್ಥಾನ->cap = cap;
    *pd_ಸ್ಥಾನ = pde;
    invalidate_ಪುಟ_Structure_Cache_ASID(pptr_ಗಮ್ಯ_paddr(ಬೇರು), cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTMapped_ASID(cap),
                                     SMP_TERNARY(tlb_bitmap_get(ಬೇರು), 0));
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಸ್ಥಿರ exception_t decode_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cte_t *cte, cap_t cap,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಶಬ್ದ_t          vaddr;
    vm_ಗುಣಲಕ್ಷಣಗಳು_t attr;
    lookupPDSlot_ret_t pd_ಸ್ಥಾನ;
    cap_t           vspaceCap;
    vspace_ಬೇರು_t  *vspace;
    pde_t           pde;
    paddr_t         paddr;
    asid_t          asid;

    ಆದರೆ (invLabel == X86_ಪುಟ_ಕೋಷ್ಟಕ_Unmap) {
        ಆದರೆ (! isFinalCapability(cte)) {
            current_syscall_error.type = se_L4_Revoke_ಮೊದಲ;
            userError("X86PageTable: Cannot unmap if more than one cap exists.");
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ perform_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Unmap(cap, cte);
    }

    ಆದರೆ (invLabel != X86_ಪುಟ_ಕೋಷ್ಟಕ_Map) {
        userError("X86PageTable: Illegal operation.");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
        userError("X86PageTable: Truncated message.");
        current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTIs_Mapped(cap)) {
        userError("X86PageTable: Page table is already mapped to a page directory.");
        current_syscall_error.type =
            seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vaddr = getSyscallArg(0, ಬಫರ್) & (~ಮುಖವಾಡ(PT_ಸೂಚ್ಯಂಕ_ಬಿಟ್ಟುಗಳು + ಪುಟ_ಬಿಟ್ಟುಗಳು));
    attr = vm_ಗುಣಲಕ್ಷಣಗಳು_From_ಶಬ್ದ(getSyscallArg(1, ಬಫರ್));
    vspaceCap = current_extra_caps.excaprefs[0]->cap;

    ಆದರೆ (!is_Valid_Native_ಬೇರು(vspaceCap)) {
        current_syscall_error.type = seL4_InvalidCapability;
        current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    vspace = (vspace_ಬೇರು_t *)pptr_ದ_cap(vspaceCap);
    asid = cap_get_capMappedASID(vspaceCap);

    ಆದರೆ (vaddr > USER_ತುದಿ) {
        userError("X86PageTable: Mapping address too high.");
        current_syscall_error.type = se_L4_Invalid_ವಾದ;
        current_syscall_error.invalid_ವಾದ_ಸಂಖ್ಯೆ = 0;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    {
        ಕಂಡುಹಿಡಿ_VSpace_For_ASID_ret_t ಕಂಡುಹಿಡಿ_ret;

        ಕಂಡುಹಿಡಿ_ret = ಕಂಡುಹಿಡಿ_VSpace_For_ASID(asid);
        ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (ಕಂಡುಹಿಡಿ_ret.vspace_ಬೇರು != vspace) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
    }

    pd_ಸ್ಥಾನ = lookupPDSlot(vspace, vaddr);
    ಆದರೆ (pd_ಸ್ಥಾನ.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        current_syscall_error.type = seL4_FailedLookup;
        current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ)) ||
        ((pde_ptr_get_ಪುಟ_ಗಾತ್ರ(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ))) {
        current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    paddr = pptr_ಗಮ್ಯ_paddr(PTE_PTR(cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_get_cap_PTBase_Ptr(cap)));
    pde = make_User_PDEPage_ಕೋಷ್ಟಕ(paddr, attr);

    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTIs_Mapped(cap, 1);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ASID(cap, asid);
    cap = cap_ಪುಟ_ಕೋಷ್ಟಕ_cap_ಗಣ_cap_PTMapped_ವಿಳಾಸ(cap, vaddr);

    ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
    ಹಿಂತಿರುಗಿ perform_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation_Map(cap, cte, pde, pd_ಸ್ಥಾನ.pd_ಸ್ಥಾನ, vspace);
}

exception_t decodeX86MMUInvocation(
    ಶಬ್ದ_t invLabel,
    ಶಬ್ದ_t ಉದ್ದಳತೆ,
    cptr_t cptr,
    cte_t *cte,
    cap_t cap,
    bool_t call,
    ಶಬ್ದ_t *ಬಫರ್
)
{
    ಆಯ್ಕೆ (cap_get_capType(cap)) {

    ಸ್ಥಿತಿ cap_ಚೌಕಟ್ಟು_cap:
        ಹಿಂತಿರುಗಿ decode_X86_ಚೌಕಟ್ಟು_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, call, ಬಫರ್);

    ಸ್ಥಿತಿ cap_ಪುಟ_ಕೋಷ್ಟಕ_cap:
        ಹಿಂತಿರುಗಿ decode_X86_ಪುಟ_ಕೋಷ್ಟಕ_Invocation(invLabel, ಉದ್ದಳತೆ, cte, cap, ಬಫರ್);

    ಸ್ಥಿತಿ cap_asid_control_cap: {
        ಶಬ್ದ_t     i;
        asid_t           asid_ತಳ;
        ಶಬ್ದ_t           ಸೂಚ್ಯಂಕ;
        ಶಬ್ದ_t           ಆಳ;
        cap_t            untyped;
        cap_t            ಬೇರು;
        cte_t           *ಪೋಷಕ_ಸ್ಥಾನ;
        cte_t           *dest_ಸ್ಥಾನ;
        lookup_ಸ್ಥಾನ_ret_t lu_ret;
        ಶೂನ್ಯ            *ಚೌಕಟ್ಟು;
        exception_t      ಸ್ಥಿತಿ_2;

        ಆದರೆ (invLabel != X86ASIDControlMakePool) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (ಉದ್ದಳತೆ < 2 || current_extra_caps.excaprefs[0] == ಖಾಲಿ
            || current_extra_caps.excaprefs[1] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸೂಚ್ಯಂಕ = getSyscallArg(0, ಬಫರ್);
        ಆಳ = getSyscallArg(1, ಬಫರ್);
        ಪೋಷಕ_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        untyped = ಪೋಷಕ_ಸ್ಥಾನ->cap;
        ಬೇರು = current_extra_caps.excaprefs[1]->cap;

        /* Find first free pool */
        ಗಾಗಿ (i = 0; i < nASIDPools && x86KSASIDTable[i]; i++);

        ಆದರೆ (i == nASIDPools) {
            userError("ASIDControlMakePool: No unallocated pools found.");
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid_ತಳ = i << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು;


        ಆದರೆ (cap_get_capType(untyped) != cap_untyped_cap ||
            cap_untyped_cap_get_cap_ಖಂಡ_ಗಾತ್ರ(untyped) != se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು ||
            cap_untyped_cap_get_capIsDevice(untyped)) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಸ್ಥಿತಿ_2 = ensureNoChildren(ಪೋಷಕ_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಚೌಕಟ್ಟು = ಶಬ್ದ_PTR(cap_untyped_cap_get_capPtr(untyped));

        lu_ret = lookup_ಗುರಿ_ಸ್ಥಾನ(ಬೇರು, ಸೂಚ್ಯಂಕ, ಆಳ);
        ಆದರೆ (lu_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ lu_ret.ಸ್ಥಿತಿ_2;
        }
        dest_ಸ್ಥಾನ = lu_ret.ಸ್ಥಾನ;

        ಸ್ಥಿತಿ_2 = ensure_ಖಾಲಿಗಳು_ಸ್ಥಾನ(dest_ಸ್ಥಾನ);
        ಆದರೆ (ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
            ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
        }

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDControlInvocation(ಚೌಕಟ್ಟು, dest_ಸ್ಥಾನ, ಪೋಷಕ_ಸ್ಥಾನ, asid_ತಳ);
    }

    ಸ್ಥಿತಿ cap_asid_pool_cap: {
        cap_t        vspaceCap;
        cte_t       *vspace_Cap_ಸ್ಥಾನ;
        asid_pool_t *pool;
        ಶಬ್ದ_t i;
        asid_t       asid;

        ಆದರೆ (invLabel != X86_ASIDPool_ನಿಯೋಜಿಸು) {
            current_syscall_error.type = seL4_IllegalOperation;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }
        ಆದರೆ (current_extra_caps.excaprefs[0] == ಖಾಲಿ) {
            current_syscall_error.type = se_L4_Truncated_ಸಂದೇಶ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        vspace_Cap_ಸ್ಥಾನ = current_extra_caps.excaprefs[0];
        vspaceCap = vspace_Cap_ಸ್ಥಾನ->cap;

        ಆದರೆ (!(is_VTable_ಬೇರು(vspaceCap) || VTX_TERNARY(cap_get_capType(vspaceCap) == cap_ept_pml4_cap, 0))
            || cap_get_capMappedASID(vspaceCap) != asidInvalid) {
            userError("X86ASIDPool: Invalid vspace root.");
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 1;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        pool = x86KSASIDTable[ASID_ಮೇಲಿನ(cap_asid_pool_cap_get_capASIDBase(cap))];
        ಆದರೆ (!pool) {
            current_syscall_error.type = seL4_FailedLookup;
            current_syscall_error.failed_Lookup_Was_ಮೂಲ = false;
            current_lookup_fault = lookup_fault_invalid_ಬೇರು_new();
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        ಆದರೆ (pool != ASID_POOL_PTR(cap_asid_pool_cap_get_capASIDPool(cap))) {
            current_syscall_error.type = seL4_InvalidCapability;
            current_syscall_error.invalid_Cap_ಸಂಖ್ಯೆ = 0;
            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        /* Find first free ASID */
        asid = cap_asid_pool_cap_get_capASIDBase(cap);
        ಗಾಗಿ (i = 0; i < ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು) && (asid + i == 0
                                             || asid_map_get_type(pool->array[i]) != asid_map_asid_map_none); i++);

        ಆದರೆ (i == ಬಿಟ್(asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು)) {
            current_syscall_error.type = se_L4_Delete_ಮೊದಲ;

            ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
        }

        asid += i;

        ಗಣ_Thread_State(ಗಂಟು_STATE(ksCurThread), ThreadState_Restart);
        ಹಿಂತಿರುಗಿ performASIDPoolInvocation(asid, pool, vspace_Cap_ಸ್ಥಾನ);
    }

    ಪೂರ್ವನಿಯೋಜಿತ:
        ಹಿಂತಿರುಗಿ decode_X86_ಬಹುಲಕ_MMUInvocation(invLabel, ಉದ್ದಳತೆ, cptr, cte, cap, call, ಬಫರ್);
    }
}
