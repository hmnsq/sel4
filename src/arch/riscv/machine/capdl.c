/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD

#ਅਨ੍ਤਰ੍ਭਾਵ <machine/capdl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/capdl.h>

ਸ਼ਬਦ_t get_tcb_sp(tcb_t *tcb)
{
    ਵਾਪਸ tcb->tcbArch.tcbContext.registers[SP];
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_PRINTING

ਸਥਿਰ ਖਾਲੀ obj_asidpool_print_attrs(cap_t asid_cap);
ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(paddr_t paddr);
ਸਥਿਰ ਖਾਲੀ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ਪੂਰਨ level);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, pte_t *lvl1pt);
ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ);

ਸਥਿਰ ਖਾਲੀ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    ਲਿਖੋ("(asid_high: 0x%lx)\n", ASID_ਉੱਚਾ(asid));
}

ਖਾਲੀ print_ipc_ਬਫਰ_ਥਾਂ(tcb_t *tcb)
{
    ਸ਼ਬਦ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);

    ਲਿਖੋ("ipc_buffer_slot: ");
    cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
}

ਸਥਿਰ ਖਾਲੀ riscv_cap_pt_print_slots(pte_t *ਉੱਤਲਾ_Pt_ਥਾਂ, ਸ਼ਬਦ_t pt_ਸੂਚਕਾਂਕ, ਪੂਰਨ level)
{
    pte_t *pt;
    ਜੇ (level == CONFIG_PT_ਪੱਧਰ) {
        ਲਿਖੋ("%p_pd {\n", ਉੱਤਲਾ_Pt_ਥਾਂ);
        pt = ਉੱਤਲਾ_Pt_ਥਾਂ;
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("pt_%p_%04lu {\n", ਉੱਤਲਾ_Pt_ਥਾਂ, pt_ਸੂਚਕਾਂਕ);
        pt = getPPtrFromHWPTE(ਉੱਤਲਾ_Pt_ਥਾਂ);
    }
    level -= 1;

    ਸ਼ਬਦ_t pt_ਬਿੱਟ_ਖੱਬਾ = PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * level + se_L4_ਪੰਨਾ_ਬਿੱਟ;

    /* - 1 to avoid overflowing */
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(pt_ਬਿੱਟ_ਖੱਬਾ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1; i += (1 << (pt_ਬਿੱਟ_ਖੱਬਾ))) {
        ਸ਼ਬਦ_t pt_ਥਾਂ_ਸੂਚਕਾਂਕ = ((i >> pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
        pte_t *pt_ਥਾਂ = pt + pt_ਥਾਂ_ਸੂਚਕਾਂਕ;
        ਜੇ (pte_ptr_get_valid(pt_ਥਾਂ)) {
            ਜੇ (level) { /* pt */
                ਲਿਖੋ("0x%lx: pt_%p_%04lu\n", pt_ਥਾਂ_ਸੂਚਕਾਂਕ, pt_ਥਾਂ, pt_ਥਾਂ_ਸੂਚਕਾਂਕ);
            } ਨਹੀਂਤਾਂ { /*ਚੌਖਟਾ*/
                ਲਿਖੋ("0x%lx: frame_%p_%04lu", pt_ਥਾਂ_ਸੂਚਕਾਂਕ, pt_ਥਾਂ, pt_ਥਾਂ_ਸੂਚਕਾਂਕ);
                cap_ਚੌਖਟਾ_print_attrs_pt(pt_ਥਾਂ);
            }
        }
    }
    ਲਿਖੋ("}\n"); /* lvl1pt/pt */

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(pt_ਬਿੱਟ_ਖੱਬਾ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ) - 1; i += (1 << (pt_ਬਿੱਟ_ਖੱਬਾ))) {
        ਸ਼ਬਦ_t pt_ਥਾਂ_ਸੂਚਕਾਂਕ = ((i >> pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
        pte_t *pt_ਥਾਂ = pt + pt_ਥਾਂ_ਸੂਚਕਾਂਕ;
        ਜੇ (pte_ptr_get_valid(pt_ਥਾਂ)) {
            ਜੇ (level) { /* pt */
                riscv_cap_pt_print_slots(pt_ਥਾਂ, pt_ਥਾਂ_ਸੂਚਕਾਂਕ, level);
            }
        }
    }
}

ਖਾਲੀ obj_vtable_print_slots(tcb_t *tcb)
{
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_ਦਾ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_ਪੱਧਰ);
    }
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_pt(pte_t *pt_ਥਾਂ)
{
    ਲਿਖੋ("(");

    /* rights */
    ਜੇ (pte_ptr_get_read(pt_ਥਾਂ)) {
        ਲਿਖੋ("R");
    }

    ਜੇ (pte_ptr_get_write(pt_ਥਾਂ)) {
        ਲਿਖੋ("W");
    }

    ਜੇ (pte_ptr_get_execute(pt_ਥਾਂ)) {
        ਲਿਖੋ("X");
    }

    /* cacheable not supported yet */

    ਲਿਖੋ(")\n");
}

ਸਥਿਰ ਖਾਲੀ cap_ਚੌਖਟਾ_print_attrs_vptr(ਸ਼ਬਦ_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    ਪ੍ਰਤਿਜ੍ਞਾ(lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ == se_L4_ਪੰਨਾ_ਬਿੱਟ);
    ਸ਼ਬਦ_t ਥਾਂ = ((vptr >> lu_ret.pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));

    ਲਿਖੋ("frame_%p_%04lu ", lu_ret.pt_ਥਾਂ, ਥਾਂ);
    cap_ਚੌਖਟਾ_print_attrs_pt(lu_ret.pt_ਥਾਂ);
}

ਖਾਲੀ print_cap_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap: {
        asid_t asid = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ASID(cap);
        ਲੱਭੋ_VSpace_For_ASID_ret_t ਲੱਭੋ_ret = ਲੱਭੋ_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ਪੰਨਾ_ਸਾਰਣੀ_cap_get_cap_PTMapped_ਪਤਾ(cap);

        ਸ਼ਬਦ_t pt_ਬਿੱਟ_ਖੱਬਾ = PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * CONFIG_PT_ਪੱਧਰ + se_L4_ਪੰਨਾ_ਬਿੱਟ;
        ਸ਼ਬਦ_t ਥਾਂ = ((vptr >> pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
        ਜੇ (asid) {
            ਲਿਖੋ("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pt_ਥਾਂ, ਥਾਂ, (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)asid);
        } ਨਹੀਂਤਾਂ {
            ਲਿਖੋ("pt_%p_%04lu\n", lookupPTSlot(ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ, vptr).pt_ਥਾਂ, ਥਾਂ);
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
        cap_ਚੌਖਟਾ_print_attrs_vptr(vptr, ਲੱਭੋ_ret.vspace_ਜੜ੍ਹ);
        ਰੋਕੋ;
    }
    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool\n", (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        ਰੋਕੋ;
    }
    /* riscv specific caps */
    /* nothing */
    ਮੂਲ: {
        ਲਿਖੋ("[unknown cap %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਸਥਿਰ ਖਾਲੀ obj_ਚੌਖਟਾ_print_attrs(paddr_t paddr)
{
    ਲਿਖੋ("(4k, paddr: 0x%p)\n", (ਖਾਲੀ *)paddr);
}

ਖਾਲੀ print_object_arch(cap_t cap)
{
    ਚੋਣ (cap_get_capType(cap)) {
    ਹਾਲਤ cap_ਚੌਖਟਾ_cap:
    ਹਾਲਤ cap_ਪੰਨਾ_ਸਾਰਣੀ_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        ਰੋਕੋ;

    ਹਾਲਤ cap_asid_pool_cap: {
        ਲਿਖੋ("%p_asid_pool = asid_pool ",
               (ਖਾਲੀ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        ਰੋਕੋ;
    }
    /* riscv specific caps */
    /* nothing */
    ਮੂਲ: {
        ਲਿਖੋ("[unknown object %lu]\n", (ਲੰਮਾ ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)cap_get_capType(cap));
        ਰੋਕੋ;
    }
    }
}

ਸਥਿਰ ਖਾਲੀ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, ਪੂਰਨ level)
{
    ਸ਼ਬਦ_t pt_ਬਿੱਟ_ਖੱਬਾ = PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ * level + se_L4_ਪੰਨਾ_ਬਿੱਟ;

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ਦੋਆਧਾਰੀ_ਅੰਕ(pt_ਬਿੱਟ_ਖੱਬਾ + PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ); i += (1 << (pt_ਬਿੱਟ_ਖੱਬਾ))) {
        ਸ਼ਬਦ_t pt_ਸੂਚਕਾਂਕ = ((i >> pt_ਬਿੱਟ_ਖੱਬਾ) & ਨਕਾਬ(PT_ਸੂਚਕਾਂਕ_ਬਿੱਟ));
        pte_t *pt_ਥਾਂ = pt + pt_ਸੂਚਕਾਂਕ;
        ਜੇ (pte_ptr_get_valid(pt_ਥਾਂ)) {
            ਜੇ (level) { /* pt */
                ਲਿਖੋ("pt_%p_%04lu = pt\n", pt_ਥਾਂ, pt_ਸੂਚਕਾਂਕ);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_ਥਾਂ), level - 1);
            } ਨਹੀਂਤਾਂ { /*ਚੌਖਟਾ*/
                paddr_t paddr = pte_ptr_get_ppn(pt_ਥਾਂ);
                ਲਿਖੋ("frame_%p_%04lu = frame ", pt_ਥਾਂ, pt_ਸੂਚਕਾਂਕ);
                obj_ਚੌਖਟਾ_print_attrs(paddr);
            }
        }
    }
}

ਖਾਲੀ obj_tcb_print_vtable(tcb_t *tcb)
{
    ਜੇ (is_Valid_VTable_ਜੜ੍ਹ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਵੇਖਿਆ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_ਦਾ_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        ਲਿਖੋ("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_ਪੱਧਰ - 1);
    }
}

#ਜੇ_ਅੰਤ /* CONFIG_PRINTING */

ਖਾਲੀ debug_capDL(ਖਾਲੀ)
{
    ਲਿਖੋ("arch riscv\n");
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
