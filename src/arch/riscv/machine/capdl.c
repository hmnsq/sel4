/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#که_تعریف_شوی CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#که_تعریف_شوی CONFIG_PRINTING

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ obj_چوکاټ_print_attrs(paddr_t paddr);
ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level);
ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, pte_t *lvl1pt);
ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه);

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    چاپ_کړه("(asid_high: 0x%lx)\n", ASID_لوړ(asid));
}

فراغ print_ipc_بفر_خونه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);

    چاپ_کړه("ipc_buffer_slot: ");
    cap_چوکاټ_print_attrs_vptr(vptr, پیدا_کړه_ret.vspace_ریښه);
}

ساكن فراغ riscv_cap_pt_print_slots(pte_t *پاسنی_Pt_خونه, کلمه_t pt_شاخص, صحيح level)
{
    pte_t *pt;
    إذا (level == CONFIG_PT_پوړونه) {
        چاپ_کړه("%p_pd {\n", پاسنی_Pt_خونه);
        pt = پاسنی_Pt_خونه;
    } وإلا {
        چاپ_کړه("pt_%p_%04lu {\n", پاسنی_Pt_خونه, pt_شاخص);
        pt = getPPtrFromHWPTE(پاسنی_Pt_خونه);
    }
    level -= 1;

    کلمه_t pt_بټونه_کیڼ = PT_شاخص_بټونه * level + se_L4_پاڼه_بټونه;

    /* - 1 to avoid overflowing */
    لكل (کلمه_t i = 0; i < بټ(pt_بټونه_کیڼ + PT_شاخص_بټونه) - 1; i += (1 << (pt_بټونه_کیڼ))) {
        کلمه_t pt_خونه_شاخص = ((i >> pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
        pte_t *pt_خونه = pt + pt_خونه_شاخص;
        إذا (pte_ptr_get_valid(pt_خونه)) {
            إذا (level) { /* pt */
                چاپ_کړه("0x%lx: pt_%p_%04lu\n", pt_خونه_شاخص, pt_خونه, pt_خونه_شاخص);
            } وإلا { /*چوکاټ*/
                چاپ_کړه("0x%lx: frame_%p_%04lu", pt_خونه_شاخص, pt_خونه, pt_خونه_شاخص);
                cap_چوکاټ_print_attrs_pt(pt_خونه);
            }
        }
    }
    چاپ_کړه("}\n"); /* lvl1pt/pt */

    لكل (کلمه_t i = 0; i < بټ(pt_بټونه_کیڼ + PT_شاخص_بټونه) - 1; i += (1 << (pt_بټونه_کیڼ))) {
        کلمه_t pt_خونه_شاخص = ((i >> pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
        pte_t *pt_خونه = pt + pt_خونه_شاخص;
        إذا (pte_ptr_get_valid(pt_خونه)) {
            إذا (level) { /* pt */
                riscv_cap_pt_print_slots(pt_خونه, pt_خونه_شاخص, level);
            }
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_د_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_پوړونه);
    }
}

ساكن فراغ cap_چوکاټ_print_attrs_pt(pte_t *pt_خونه)
{
    چاپ_کړه("(");

    /* rights */
    إذا (pte_ptr_get_read(pt_خونه)) {
        چاپ_کړه("R");
    }

    إذا (pte_ptr_get_write(pt_خونه)) {
        چاپ_کړه("W");
    }

    إذا (pte_ptr_get_execute(pt_خونه)) {
        چاپ_کړه("X");
    }

    /* cacheable not supported yet */

    چاپ_کړه(")\n");
}

ساكن فراغ cap_چوکاټ_print_attrs_vptr(کلمه_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_بټونه_کیڼ == se_L4_پاڼه_بټونه);
    کلمه_t خونه = ((vptr >> lu_ret.pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));

    چاپ_کړه("frame_%p_%04lu ", lu_ret.pt_خونه, خونه);
    cap_چوکاټ_print_attrs_pt(lu_ret.pt_خونه);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_پاڼه_جدول_cap: {
        asid_t asid = cap_پاڼه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(asid);
        vptr_t vptr = cap_پاڼه_جدول_cap_get_cap_PTMapped_پته(cap);

        کلمه_t pt_بټونه_کیڼ = PT_شاخص_بټونه * CONFIG_PT_پوړونه + se_L4_پاڼه_بټونه;
        کلمه_t خونه = ((vptr >> pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
        إذا (asid) {
            چاپ_کړه("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pt_خونه, خونه, (اوږد بینښه صحيح)asid);
        } وإلا {
            چاپ_کړه("pt_%p_%04lu\n", lookupPTSlot(پیدا_کړه_ret.vspace_ریښه, vptr).pt_خونه, خونه);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کړه("asid_control\n");
        اكسر;
    }
    حالة cap_چوکاټ_cap: {
        vptr_t vptr = cap_چوکاټ_cap_get_cap_FMapped_پته(cap);
        پیدا_کړه_VSpace_For_ASID_ret_t پیدا_کړه_ret = پیدا_کړه_VSpace_For_ASID(cap_چوکاټ_cap_get_cap_FMapped_ASID(cap));

        أكد(پیدا_کړه_ret.حالت == EXCEPTION_NONE);
        cap_چوکاټ_print_attrs_vptr(vptr, پیدا_کړه_ret.vspace_ریښه);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        چاپ_کړه("[unknown cap %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکاټ_print_attrs(paddr_t paddr)
{
    چاپ_کړه("(4k, paddr: 0x%p)\n", (فراغ *)paddr);
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکاټ_cap:
    حالة cap_پاڼه_جدول_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کړه("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        چاپ_کړه("[unknown object %lu]\n", (اوږد بینښه صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level)
{
    کلمه_t pt_بټونه_کیڼ = PT_شاخص_بټونه * level + se_L4_پاڼه_بټونه;

    لكل (کلمه_t i = 0; i < بټ(pt_بټونه_کیڼ + PT_شاخص_بټونه); i += (1 << (pt_بټونه_کیڼ))) {
        کلمه_t pt_شاخص = ((i >> pt_بټونه_کیڼ) & ماسک(PT_شاخص_بټونه));
        pte_t *pt_خونه = pt + pt_شاخص;
        إذا (pte_ptr_get_valid(pt_خونه)) {
            إذا (level) { /* pt */
                چاپ_کړه("pt_%p_%04lu = pt\n", pt_خونه, pt_شاخص);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_خونه), level - 1);
            } وإلا { /*چوکاټ*/
                paddr_t paddr = pte_ptr_get_ppn(pt_خونه);
                چاپ_کړه("frame_%p_%04lu = frame ", pt_خونه, pt_شاخص);
                obj_چوکاټ_print_attrs(paddr);
            }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریښه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کړه_منزل_کتل_شوی(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_د_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        چاپ_کړه("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_پوړونه - 1);
    }
}

#د_که_پای /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کړه("arch riscv\n");
    چاپ_کړه("objects {\n");
#که_تعریف_شوی CONFIG_PRINTING
    print_objects();
#د_که_پای
    چاپ_کړه("}\n");

    چاپ_کړه("caps {\n");

    /* reset the seen list */
    reset_کتل_شوی_list();

#که_تعریف_شوی CONFIG_PRINTING
    print_caps();
    چاپ_کړه("}\n");

    obj_irq_print_maps();
#د_که_پای /* CONFIG_PRINTING */
}

#د_که_پای /* CONFIG_DEBUG_BUILD */
