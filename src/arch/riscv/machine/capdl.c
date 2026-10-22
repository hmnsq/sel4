/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر_تعریف_شدہ CONFIG_DEBUG_BUILD

#شامل <machine/capdl.h>
#شامل <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    واپس tcb->tcbArch.tcbContext.registers[SP];
}

#اگر_تعریف_شدہ CONFIG_PRINTING

ساکن خالی obj_asidpool_print_attrs(cap_t asid_cap);
ساکن خالی obj_چوکھٹا_print_attrs(paddr_t paddr);
ساکن خالی riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحیح level);
ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt);
ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ);

ساکن خالی obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکھو("(asid_high: 0x%lx)\n", ASID_بلند(asid));
}

خالی print_ipc_بفر_خانہ(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);

    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, ڈھونڈو_ret.vspace_جڑ);
}

ساکن خالی riscv_cap_pt_print_slots(pte_t *بالائی_Pt_خانہ, لفظ_t pt_اشاریہ, صحیح level)
{
    pte_t *pt;
    اگر (level == CONFIG_PT_سطحیں) {
        لکھو("%p_pd {\n", بالائی_Pt_خانہ);
        pt = بالائی_Pt_خانہ;
    } ورنہ {
        لکھو("pt_%p_%04lu {\n", بالائی_Pt_خانہ, pt_اشاریہ);
        pt = getPPtrFromHWPTE(بالائی_Pt_خانہ);
    }
    level -= 1;

    لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_صفحہ_بٹ;

    /* - 1 to avoid overflowing */
    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ) - 1; i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_خانہ_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_خانہ = pt + pt_خانہ_اشاریہ;
        اگر (pte_ptr_get_valid(pt_خانہ)) {
            اگر (level) { /* pt */
                لکھو("0x%lx: pt_%p_%04lu\n", pt_خانہ_اشاریہ, pt_خانہ, pt_خانہ_اشاریہ);
            } ورنہ { /*چوکھٹا*/
                لکھو("0x%lx: frame_%p_%04lu", pt_خانہ_اشاریہ, pt_خانہ, pt_خانہ_اشاریہ);
                cap_چوکھٹا_print_attrs_pt(pt_خانہ);
            }
        }
    }
    لکھو("}\n"); /* lvl1pt/pt */

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ) - 1; i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_خانہ_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_خانہ = pt + pt_خانہ_اشاریہ;
        اگر (pte_ptr_get_valid(pt_خانہ)) {
            اگر (level) { /* pt */
                riscv_cap_pt_print_slots(pt_خانہ, pt_خانہ_اشاریہ, level);
            }
        }
    }
}

خالی obj_vtable_print_slots(tcb_t *tcb)
{
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_کا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_سطحیں);
    }
}

ساکن خالی cap_چوکھٹا_print_attrs_pt(pte_t *pt_خانہ)
{
    لکھو("(");

    /* rights */
    اگر (pte_ptr_get_read(pt_خانہ)) {
        لکھو("R");
    }

    اگر (pte_ptr_get_write(pt_خانہ)) {
        لکھو("W");
    }

    اگر (pte_ptr_get_execute(pt_خانہ)) {
        لکھو("X");
    }

    /* cacheable not supported yet */

    لکھو(")\n");
}

ساکن خالی cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_بٹ_بایاں == se_L4_صفحہ_بٹ);
    لفظ_t خانہ = ((vptr >> lu_ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));

    لکھو("frame_%p_%04lu ", lu_ret.pt_خانہ, خانہ);
    cap_چوکھٹا_print_attrs_pt(lu_ret.pt_خانہ);
}

خالی print_cap_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_صفحہ_جدول_cap: {
        asid_t asid = cap_صفحہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحہ_جدول_cap_get_cap_PTMapped_پتہ(cap);

        لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * CONFIG_PT_سطحیں + se_L4_صفحہ_بٹ;
        لفظ_t خانہ = ((vptr >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        اگر (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pt_خانہ, خانہ, (طویل غیرعلامتی صحیح)asid);
        } ورنہ {
            لکھو("pt_%p_%04lu\n", lookupPTSlot(ڈھونڈو_ret.vspace_جڑ, vptr).pt_خانہ, خانہ);
        }
        توڑو;
    }
    حالت cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        توڑو;
    }
    حالت cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        ڈھونڈو_VSpace_For_ASID_ret_t ڈھونڈو_ret = ڈھونڈو_VSpace_For_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));

        أكد(ڈھونڈو_ret.حالت_2 == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, ڈھونڈو_ret.vspace_جڑ);
        توڑو;
    }
    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        توڑو;
    }
    /* riscv specific caps */
    /* nothing */
    طےشدہ: {
        لکھو("[unknown cap %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

ساکن خالی obj_چوکھٹا_print_attrs(paddr_t paddr)
{
    لکھو("(4k, paddr: 0x%p)\n", (خالی *)paddr);
}

خالی print_object_arch(cap_t cap)
{
    انتخاب (cap_get_capType(cap)) {
    حالت cap_چوکھٹا_cap:
    حالت cap_صفحہ_جدول_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        توڑو;

    حالت cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (خالی *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        توڑو;
    }
    /* riscv specific caps */
    /* nothing */
    طےشدہ: {
        لکھو("[unknown object %lu]\n", (طویل غیرعلامتی صحیح)cap_get_capType(cap));
        توڑو;
    }
    }
}

ساکن خالی riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحیح level)
{
    لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_صفحہ_بٹ;

    برائے (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ); i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_خانہ = pt + pt_اشاریہ;
        اگر (pte_ptr_get_valid(pt_خانہ)) {
            اگر (level) { /* pt */
                لکھو("pt_%p_%04lu = pt\n", pt_خانہ, pt_اشاریہ);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_خانہ), level - 1);
            } ورنہ { /*چوکھٹا*/
                paddr_t paddr = pte_ptr_get_ppn(pt_خانہ);
                لکھو("frame_%p_%04lu = frame ", pt_خانہ, pt_اشاریہ);
                obj_چوکھٹا_print_attrs(paddr);
            }
        }
    }
}

خالی obj_tcb_print_vtable(tcb_t *tcb)
{
    اگر (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_مقصد_نظر_آیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_کا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        لکھو("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_سطحیں - 1);
    }
}

#اگر_ختم /* CONFIG_PRINTING */

خالی debug_capDL(خالی)
{
    لکھو("arch riscv\n");
    لکھو("objects {\n");
#اگر_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#اگر_ختم
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_نظر_آیا_list();

#اگر_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#اگر_ختم /* CONFIG_PRINTING */
}

#اگر_ختم /* CONFIG_DEBUG_BUILD */
