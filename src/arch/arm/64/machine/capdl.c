/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/cspace.h>

ਸ਼ਬਦ_t get_tcb_sp(tcb_t *tcb)
{
    ਵਾਪਸ tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ, paddr_t ਚੌਖਟਾ_ਅਧਾਰ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t SH, ਸ਼ਬਦ_t AP, ਸ਼ਬਦ_t NXN);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, cap_t vspace);

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr_2(ਸ਼ਬਦ_t vptr, vspace_ਜੜ੍ਹ_t *vspace_ਜੜ੍ਹ_2);

ਸਥਿਰ ਖਾਲੀ arm64_obj_pt_print_slots(pte_t *pd_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ arm64_obj_pd_print_slots(pte_t *pud_ਥਾਂ);
ਸਥਿਰ ਖਾਲੀ arm64_obj_pud_print_slots(ਖਾਲੀ *pgd_ਥਾਂ_or_vspace);

ਸਥਿਰ ਖਾਲੀ arm64_cap_pt_print_slots(pte_t *pd_ਥਾਂ, vptr_t vptr);
ਸਥਿਰ ਖਾਲੀ arm64_cap_pd_print_slots(pte_t *pud_ਥਾਂ, vptr_t vptr);
ਸਥਿਰ ਖਾਲੀ arm64_cap_pud_print_slots(ਖਾਲੀ *pgd_ਥਾਂ_or_vspace, vptr_t vptr);

/* Stage-1 access permissions:
 * AP[2:1]  higher EL        EL0
 *   00       rw            None
 *   01       rw            rw
 *   10       r             None
 *   11       r             r
 *
 * Stage-2 access permissions:
 * S2AP    Access from Nonsecure EL1 or Non-secure EL0
 *  00                      None
 *  01                      r
 *  10                      w
 *  11                      rw
 *
 *  For VMs or native seL4 applications, if hypervisor support
 *  is enabled, we use the S2AP. The kernel itself running in
 *  EL2 still uses the Stage-1 AP format.
 */
/* use when only have access to pte of frames */
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ)
{
    cap_ਚੌਖਟਾ_print_attrs_impl(pte_pte_ਪੰਨਾ_ptr_get_SH(pt_ਥਾਂ),
                               pte_pte_ਪੰਨਾ_ptr_get_AP(pt_ਥਾਂ),
                               pte_pte_ਪੰਨਾ_ptr_get_UXN(pt_ਥਾਂ));
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_impl(ਸ਼ਬਦ_t SH, ਸ਼ਬਦ_t AP, ਸ਼ਬਦ_t NXN)
{
    ਲਿਖੋ("(");

    /* rights */
    ਚੋਣ (AP) {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ 0b00:
        ਰੋਕੋ;
    ਹਾਲਤ 0b01:
        ਲਿਖੋ("R");
        ਰੋਕੋ;
    ਹਾਲਤ 0b10:
        ਲਿਖੋ("W");
        ਰੋਕੋ;
    ਹਾਲਤ 0b11:
        ਲਿਖੋ("RW");
        ਰੋਕੋ;
#ਨਹੀਂਤਾਂ
    ਹਾਲਤ 0b00:
        ਰੋਕੋ;
    ਹਾਲਤ 0b01:
        ਲਿਖੋ("RW");
    ਹਾਲਤ 0b10:
        ਰੋਕੋ;
    ਹਾਲਤ 0b11:
        ਲਿਖੋ("R");
#ਜੇ_ਅੰਤ /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    ਮੂਲ:
        ਰੋਕੋ;
    }

    ਜੇ (!NXN) {
        ਲਿਖੋ("X");
    }

    /* Only has effect if SMP enabled */
    ਜੇ (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        ਲਿਖੋ(", uncached");
    }

    ਲਿਖੋ(")\n");
}

/* use when only have access to vptr of frames */
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr_2(ਸ਼ਬਦ_t vptr, vspace_ਜੜ੍ਹ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    ਜੇ (!pte_ptr_get_valid(ret.pt_ਥਾਂ) ||
        (pte_pte_ਸਾਰਣੀ_ptr_get_present(ret.pt_ਥਾਂ) && ret.pt_ਬਿੱਟ_ਖੱਬਾ > ਪੰਨਾ_ਬਿੱਟ)) {
        ਪ੍ਰਤਿਜ੍ਞਾ(0);
    }

    ਸ਼ਬਦ_t ਸਾਰਣੀ_ਸੂਚਕਾਂਕ;
    ਚੋਣ (ret.pt_ਬਿੱਟ_ਖੱਬਾ) {

    ਹਾਲਤ ARMHuge_ਪੰਨਾ:
        ਸਾਰਣੀ_ਸੂਚਕਾਂਕ = GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(1));
        ਰੋਕੋ;
    ਹਾਲਤ ARMLarge_ਪੰਨਾ:
        ਸਾਰਣੀ_ਸੂਚਕਾਂਕ = GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(2));
        ਰੋਕੋ;
    ਹਾਲਤ ARMSmall_ਪੰਨਾ:
        ਸਾਰਣੀ_ਸੂਚਕਾਂਕ = GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(3));
        ਰੋਕੋ;
    ਮੂਲ:
        ਪ੍ਰਤਿਜ੍ਞਾ(0);

    }
    ਲਿਖੋ("frame_%p_%04lu ", ret.pt_ਥਾਂ, ਸਾਰਣੀ_ਸੂਚਕਾਂਕ);
    cap_ਚੌਖਟਾ_print_attrs_pt(ret.pt_ਥਾਂ);
}

ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, cap_t vspace)
{
    cap_ਚੌਖਟਾ_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_ਦਾ_cap(vspace)));
}

/*
 * print object slots
 */
ਸਥਿਰ ਖਾਲੀ arm64_cap_pt_print_slots(pte_t *pd_ਥਾਂ, vptr_t vptr)
{
    pte_t *pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ));
    ਲਿਖੋ("pt_%p_%04lu {\n", pd_ਥਾਂ, GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i ++) {
        pte_t *pt_ਥਾਂ = pt + i;

        ਜੇ (pte_4k_ਪੰਨਾ_ptr_get_present(pt_ਥਾਂ)) {
            // print pte entries
            ਲਿਖੋ("0x%lx: frame_%p_%04lu", i, pt_ਥਾਂ, i);
            cap_ਚੌਖਟਾ_print_attrs_pt(pt_ਥਾਂ);
        }
    }
    ਲਿਖੋ("}\n"); /* pt */
}

ਸਥਿਰ ਖਾਲੀ arm64_cap_pd_print_slots(pte_t *pud_ਥਾਂ, vptr_t vptr)
{
    ਲਿਖੋ("pd_%p_%04lu {\n", pud_ਥਾਂ, GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pud_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pd_ਥਾਂ = pd + i;

        ਚੋਣ (pte_ptr_get_pte_type(pd_ਥਾਂ)) {

        ਹਾਲਤ pte_pte_ਪੰਨਾ:
            ਲਿਖੋ("0x%lx: frame_%p_%04lu", i, pd_ਥਾਂ, i);
            cap_ਚੌਖਟਾ_print_attrs_pt(pd_ਥਾਂ);
            ਰੋਕੋ;

        ਹਾਲਤ pte_pte_ਸਾਰਣੀ:
            ਲਿਖੋ("0x%lx: pt_%p_%04lu\n", i, pd_ਥਾਂ, i);
            ਰੋਕੋ;
        }
    }

    ਲਿਖੋ("}\n"); /* pd */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pd_ਥਾਂ = pd + i;
        ਜੇ (pte_ptr_get_pte_type(pd_ਥਾਂ) == pte_pte_ਸਾਰਣੀ) {
            arm64_cap_pt_print_slots(pd_ਥਾਂ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ਸਥਿਰ ਖਾਲੀ arm64_cap_pud_print_slots(ਖਾਲੀ *pgd_ਥਾਂ_or_vspace, vptr_t vptr)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    pte_t *pud = pgd_ਥਾਂ_or_vspace;
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ_ਬਿੱਟ = se_L4_VSpace_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
    ਲਿਖੋ("%p_pd {\n", pgd_ਥਾਂ_or_vspace);
#ਨਹੀਂਤਾਂ
    pte_t *pud = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pgd_ਥਾਂ_or_vspace));
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ_ਬਿੱਟ = se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
    ਲਿਖੋ("pud_%p_%04lu {\n", pgd_ਥਾਂ_or_vspace, GET_UPT_ਸੂਚਕਾਂਕ(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#ਜੇ_ਅੰਤ

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pud_ਥਾਂ = pud + i;
        ਜੇ (pte_ptr_get_pte_type(pud_ਥਾਂ) == pte_pte_ਸਾਰਣੀ) {
            ਲਿਖੋ("0x%lx: pd_%p_%04lu\n", i, pud_ਥਾਂ, i);
        }
    }

    ਲਿਖੋ("}\n"); /* pgd/pud */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pud_ਥਾਂ = pud + GET_UPT_ਸੂਚਕਾਂਕ(i, ULVL_FRM_ARM_PT_LVL(1));
        ਜੇ (pte_ptr_get_pte_type(pud_ਥਾਂ) == pte_pte_ਸਾਰਣੀ) {
            arm64_cap_pd_print_slots(pud_ਥਾਂ, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

ਖਾਲੀ obj_vtable_print_slots(tcb_t *tcb)
{
    ਜੇ (is_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ਜੜ੍ਹ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
        arm64_cap_pud_print_slots(vspace, 0);
#ਨਹੀਂਤਾਂ
        ਲਿਖੋ("%p_pd {\n", vspace);
        ਲਈ (ਸ਼ਬਦ_t i = 0; i < PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ; i++) {
            pte_t *pt_ਥਾਂ = vspace + i;
            ਜੇ (pte_pte_ਸਾਰਣੀ_ptr_get_present(pt_ਥਾਂ)) {
                ਲਿਖੋ("0x%lx: pud_%p_%04lu\n", i, pt_ਥਾਂ, i);
            }
        }
        ਲਿਖੋ("}\n"); /* pd */

        ਲਈ (ਸ਼ਬਦ_t i = 0; i < PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ; i++) {
            pte_t *pt_ਥਾਂ = vspace + i;
            ਜੇ (pte_pte_ਸਾਰਣੀ_ptr_get_present(pt_ਥਾਂ)) {
                arm64_cap_pud_print_slots(pt_ਥਾਂ, i * GET_ULVL_PGSIZE(0));
            }
        }
#ਜੇ_ਅੰਤ
    }
}

ਖਾਲੀ print_ipc_ਬਫਰ_ਥਾਂ(tcb_t *tcb)
{
    ਸ਼ਬਦ_t vptr = tcb->tcbIPCBuffer;
    ਲਿਖੋ("ipc_buffer_slot: ");
    cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ਖਾਲੀ print_cap_arch(cap_t cap)
{

    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap);
        pte_t *ਨਿਸ਼ਾਨਾ_pt = PT_PTR(cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTBase_Ptr(cap));

        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        pte_t *pt_ਥਾਂ = ਨਿਰਾ;
        pte_t *pt = (pte_t *)ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ;
        ਸ਼ਬਦ_t level;
        ਲਈ (level = 0; level < UPT_ਪੱਧਰ - 1 && pt != ਨਿਸ਼ਾਨਾ_pt; level++) {
            pt_ਥਾਂ = pt + GET_UPT_ਸੂਚਕਾਂਕ(vptr, level);
            ਜੇ (unlikely(!pte_pte_ਸਾਰਣੀ_ptr_get_present(pt_ਥਾਂ))) {
                /* couldn't find it */
                ਰੋਕੋ;
            }
            pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pt_ਥਾਂ));
        }
        ਜੇ (pt != ਨਿਸ਼ਾਨਾ_pt) {
            /* didn't find it */
            ਰੋਕੋ;
        }


        ਜੇ (asid) {
            ਲਿਖੋ("pt_%p_%04lu (asid: %lu)\n",
                   ਨਿਸ਼ਾਨਾ_pt, GET_UPT_ਸੂਚਕਾਂਕ(vptr, level), (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pt_%p_%04lu\n", ਨਿਸ਼ਾਨਾ_pt, GET_UPT_ਸੂਚਕਾਂਕ(vptr, level));
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        ਜੇ (asid) {
            ਲਿਖੋ("%p_pd (asid: %lu)\n",
                   ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("%p_pd\n", ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
        }
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_control_cap: {
        /* only one in the system */
        ਲਿਖੋ("asid_control\n");
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap: {
        vptr_t vptr = cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ਪਤਾ(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(cap_ਚੌਖਟਾ_cap_get_cap_FMapped_ASID(cap));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਲੱਭੋ_ret.ਸਥਿਤੀ == EXCEPTION_NONE);
        cap_ਚੌਖਟਾ_print_attrs_vptr_2(vptr, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool\n", (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ

        /* ARM specific caps */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space\n", (ਖਾਲੀ *)cap_io_space_cap_get_capModuleID(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown cap %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ print_object_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
    ਹਾਲਤ cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool = asid_pool ",
               (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    ਹਾਲਤ cap_vcpu_cap: {
        ਲਿਖੋ("%p_vcpu = vcpu\n", (ਖਾਲੀ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
        /* ARM specific objects */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_TK1_SMMU
    ਹਾਲਤ cap_io_space_cap: {
        ਲਿਖੋ("%p_io_space = io_space ", (ਖਾਲੀ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        ਰੋਕੋ;
    }
#ਜੇ_ਅੰਤ
    ਮੂਲ: {
        ਲਿਖੋ("[unknown object %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(vm_ਪੰਨਾ_ਮਾਪ_t ਚੌਖਟਾ_ਮਾਪ, paddr_t ਚੌਖਟਾ_ਅਧਾਰ)
{
    ਲਿਖੋ("(");

    /* VM size */
    ਚੋਣ (ਚੌਖਟਾ_ਮਾਪ) {
    ਹਾਲਤ ARMHuge_ਪੰਨਾ:
        ਲਿਖੋ("1G");
        ਰੋਕੋ;
    ਹਾਲਤ ARMLarge_ਪੰਨਾ:
        ਲਿਖੋ("2M");
        ਰੋਕੋ;
    ਹਾਲਤ ARMSmall_ਪੰਨਾ:
        ਲਿਖੋ("4k");
        ਰੋਕੋ;
    }

    ਲਿਖੋ(", paddr: 0x%p)\n", (ਖਾਲੀ *)ਚੌਖਟਾ_ਅਧਾਰ);
}

ਖਾਲੀ arm64_obj_pt_print_slots(pte_t *pd_ਥਾਂ)
{
    pte_t *pt = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pt_ਥਾਂ = pt + i;

        ਜੇ (pte_4k_ਪੰਨਾ_ptr_get_present(pt_ਥਾਂ)) {
            ਲਿਖੋ("frame_%p_%04lu = frame ", pt_ਥਾਂ, i);
            obj_ਚੌਖਟਾ_print_attrs(ARMSmall_ਪੰਨਾ, pte_ਪੰਨਾ_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pt_ਥਾਂ));
        }
    }
}

ਖਾਲੀ arm64_obj_pd_print_slots(pte_t *pud_ਥਾਂ)
{
    pte_t *pd = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pud_ਥਾਂ));

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pd_ਥਾਂ = pd + i;

        ਜੇ (pte_ptr_get_pte_type(pd_ਥਾਂ) == pte_pte_ਪੰਨਾ) {
            ਲਿਖੋ("frame_%p_%04lu = frame ", pd_ਥਾਂ, i);
            obj_ਚੌਖਟਾ_print_attrs(ARMLarge_ਪੰਨਾ, pte_ਪੰਨਾ_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pd_ਥਾਂ));
        }

        ਜੇ (pte_ptr_get_pte_type(pd_ਥਾਂ) == pte_pte_ਸਾਰਣੀ) {
            ਲਿਖੋ("pt_%p_%04lu = pt\n", pd_ਥਾਂ, i);
            arm64_obj_pt_print_slots(pd_ਥਾਂ);
        }
    }
}

ਖਾਲੀ arm64_obj_pud_print_slots(ਖਾਲੀ *pgd_ਥਾਂ_or_vspace)
{
    pte_t *pud = paddr_ਮੰਜ਼ਿਲ_pptr(pte_pte_ਸਾਰਣੀ_ptr_get_pt_ਅਧਾਰ_ਪਤਾ(pgd_ਥਾਂ_or_vspace));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ_ਬਿੱਟ = se_L4_VSpace_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
#ਨਹੀਂਤਾਂ
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ_ਬਿੱਟ = se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਸੂਚਕਾਂਕ_ਬਿੱਟ;
#ਜੇ_ਅੰਤ
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(ਸੂਚਕਾਂਕ_ਬਿੱਟ); i++) {
        pte_t *pud_ਥਾਂ = pud + i;

        ਚੋਣ (pte_ptr_get_pte_type(pud_ਥਾਂ)) {
        ਹਾਲਤ pte_pte_ਪੰਨਾ:
            ਲਿਖੋ("frame_%p_%04lu = frame ", pud_ਥਾਂ, i);
            obj_ਚੌਖਟਾ_print_attrs(ARMHuge_ਪੰਨਾ, pte_ਪੰਨਾ_ptr_get_ਪੰਨਾ_ਅਧਾਰ_ਪਤਾ(pud_ਥਾਂ));
            ਰੋਕੋ;

        ਹਾਲਤ pte_pte_ਸਾਰਣੀ: {
            ਲਿਖੋ("pd_%p_%04lu = pd\n", pud_ਥਾਂ, i);
            arm64_obj_pd_print_slots(pud_ਥਾਂ);

        }
        }
    }
}

ਖਾਲੀ obj_tcb_print_vtable(tcb_t *tcb)
{
    ਜੇ (is_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_ਜੜ੍ਹ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AARCH64_VSPACE_S2_ਸ਼ੁਰੂ_L1
        ਲਿਖੋ("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#ਨਹੀਂਤਾਂ
        ਲਿਖੋ("%p_pd = pgd\n", vspace);
        ਲਈ (ਸ਼ਬਦ_t i = 0; i < PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ; i++) {
            pte_t *pt_ਥਾਂ = vspace + i;
            ਜੇ (pte_pte_ਸਾਰਣੀ_ptr_get_present(pt_ਥਾਂ)) {
                ਲਿਖੋ("pud_%p_%04lu = pud\n", pt_ਥਾਂ, i);
                arm64_obj_pud_print_slots(pt_ਥਾਂ);
            }
        }
#ਜੇ_ਅੰਤ
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

ਖਾਲੀ debug_capDL(ਖਾਲੀ)
{
    ਲਿਖੋ("arch aarch64\n");
    ਲਿਖੋ("objects {\n");
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_objects();
#ਜੇ_ਅੰਤ
    ਲਿਖੋ("}\n");

    ਲਿਖੋ("caps {\n");

    /* reset the seen list */
    reset_ਵੇਖਿਆ_list();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING
    print_caps();
    ਲਿਖੋ("}\n");

    obj_irq_print_maps();
#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */
}

#ਜੇ_ਅੰਤ /* CONFIG_DEBUG_BUILD */
