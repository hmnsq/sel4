/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#اگر_تعریف_شده CONFIG_PRINTING

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ obj_قاب_print_attrs(paddr_t paddr);
ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level);
ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, pte_t *lvl1pt);
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه);

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    چاپ_کن("(asid_high: 0x%lx)\n", ASID_بالا(asid));
}

فراغ print_ipc_میانگیر_جایگاه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);

    چاپ_کن("ipc_buffer_slot: ");
    cap_قاب_print_attrs_vptr(vptr, پیدا_کن_ret.vspace_ریشه);
}

ساكن فراغ riscv_cap_pt_print_slots(pte_t *فوقانی_Pt_جایگاه, کلمه_t pt_شاخص, صحيح level)
{
    pte_t *pt;
    إذا (level == CONFIG_PT_سطوح) {
        چاپ_کن("%p_pd {\n", فوقانی_Pt_جایگاه);
        pt = فوقانی_Pt_جایگاه;
    } وإلا {
        چاپ_کن("pt_%p_%04lu {\n", فوقانی_Pt_جایگاه, pt_شاخص);
        pt = getPPtrFromHWPTE(فوقانی_Pt_جایگاه);
    }
    level -= 1;

    کلمه_t pt_بیتها_چپ = PT_شاخص_بیتها * level + se_L4_صفحه_بیتها;

    /* - 1 to avoid overflowing */
    لكل (کلمه_t i = 0; i < بیت(pt_بیتها_چپ + PT_شاخص_بیتها) - 1; i += (1 << (pt_بیتها_چپ))) {
        کلمه_t pt_جایگاه_شاخص = ((i >> pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
        pte_t *pt_جایگاه = pt + pt_جایگاه_شاخص;
        إذا (pte_ptr_get_valid(pt_جایگاه)) {
            إذا (level) { /* pt */
                چاپ_کن("0x%lx: pt_%p_%04lu\n", pt_جایگاه_شاخص, pt_جایگاه, pt_جایگاه_شاخص);
            } وإلا { /*قاب*/
                چاپ_کن("0x%lx: frame_%p_%04lu", pt_جایگاه_شاخص, pt_جایگاه, pt_جایگاه_شاخص);
                cap_قاب_print_attrs_pt(pt_جایگاه);
            }
        }
    }
    چاپ_کن("}\n"); /* lvl1pt/pt */

    لكل (کلمه_t i = 0; i < بیت(pt_بیتها_چپ + PT_شاخص_بیتها) - 1; i += (1 << (pt_بیتها_چپ))) {
        کلمه_t pt_جایگاه_شاخص = ((i >> pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
        pte_t *pt_جایگاه = pt + pt_جایگاه_شاخص;
        إذا (pte_ptr_get_valid(pt_جایگاه)) {
            إذا (level) { /* pt */
                riscv_cap_pt_print_slots(pt_جایگاه, pt_جایگاه_شاخص, level);
            }
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_از_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_سطوح);
    }
}

ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه)
{
    چاپ_کن("(");

    /* rights */
    إذا (pte_ptr_get_read(pt_جایگاه)) {
        چاپ_کن("R");
    }

    إذا (pte_ptr_get_write(pt_جایگاه)) {
        چاپ_کن("W");
    }

    إذا (pte_ptr_get_execute(pt_جایگاه)) {
        چاپ_کن("X");
    }

    /* cacheable not supported yet */

    چاپ_کن(")\n");
}

ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_بیتها_چپ == se_L4_صفحه_بیتها);
    کلمه_t جایگاه = ((vptr >> lu_ret.pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));

    چاپ_کن("frame_%p_%04lu ", lu_ret.pt_جایگاه, جایگاه);
    cap_قاب_print_attrs_pt(lu_ret.pt_جایگاه);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحه_جدول_cap: {
        asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap);

        کلمه_t pt_بیتها_چپ = PT_شاخص_بیتها * CONFIG_PT_سطوح + se_L4_صفحه_بیتها;
        کلمه_t جایگاه = ((vptr >> pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
        إذا (asid) {
            چاپ_کن("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vptr).pt_جایگاه, جایگاه, (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pt_%p_%04lu\n", lookupPTSlot(پیدا_کن_ret.vspace_ریشه, vptr).pt_جایگاه, جایگاه);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        چاپ_کن("asid_control\n");
        اكسر;
    }
    حالة cap_قاب_cap: {
        vptr_t vptr = cap_قاب_cap_get_cap_FMapped_آدرس(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(cap_قاب_cap_get_cap_FMapped_ASID(cap));

        أكد(پیدا_کن_ret.وضعیت == EXCEPTION_NONE);
        cap_قاب_print_attrs_vptr(vptr, پیدا_کن_ret.vspace_ریشه);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        چاپ_کن("[unknown cap %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_قاب_print_attrs(paddr_t paddr)
{
    چاپ_کن("(4k, paddr: 0x%p)\n", (فراغ *)paddr);
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_قاب_cap:
    حالة cap_صفحه_جدول_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        چاپ_کن("[unknown object %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level)
{
    کلمه_t pt_بیتها_چپ = PT_شاخص_بیتها * level + se_L4_صفحه_بیتها;

    لكل (کلمه_t i = 0; i < بیت(pt_بیتها_چپ + PT_شاخص_بیتها); i += (1 << (pt_بیتها_چپ))) {
        کلمه_t pt_شاخص = ((i >> pt_بیتها_چپ) & ماسک(PT_شاخص_بیتها));
        pte_t *pt_جایگاه = pt + pt_شاخص;
        إذا (pte_ptr_get_valid(pt_جایگاه)) {
            إذا (level) { /* pt */
                چاپ_کن("pt_%p_%04lu = pt\n", pt_جایگاه, pt_شاخص);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_جایگاه), level - 1);
            } وإلا { /*قاب*/
                paddr_t paddr = pte_ptr_get_ppn(pt_جایگاه);
                چاپ_کن("frame_%p_%04lu = frame ", pt_جایگاه, pt_شاخص);
                obj_قاب_print_attrs(paddr);
            }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_از_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        چاپ_کن("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_سطوح - 1);
    }
}

#پایان_اگر /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کن("arch riscv\n");
    چاپ_کن("objects {\n");
#اگر_تعریف_شده CONFIG_PRINTING
    print_objects();
#پایان_اگر
    چاپ_کن("}\n");

    چاپ_کن("caps {\n");

    /* reset the seen list */
    reset_دیده_list();

#اگر_تعریف_شده CONFIG_PRINTING
    print_caps();
    چاپ_کن("}\n");

    obj_irq_print_maps();
#پایان_اگر /* CONFIG_PRINTING */
}

#پایان_اگر /* CONFIG_DEBUG_BUILD */
