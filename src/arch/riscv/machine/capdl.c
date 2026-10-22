/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ obj_چوکٺو_print_attrs(paddr_t paddr);
ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level);
ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt);
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه);

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکو("(asid_high: 0x%lx)\n", ASID_مٿاهون(asid));
}

فراغ print_ipc_بفر_جڳهه(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);

    لکو("ipc_buffer_slot: ");
    cap_چوکٺو_print_attrs_vptr(vptr, ڳوليو_ret.vspace_پاڙ);
}

ساكن فراغ riscv_cap_pt_print_slots(pte_t *مٿانهون_Pt_جڳهه, لفظ_t pt_اشاريو, صحيح level)
{
    pte_t *pt;
    إذا (level == CONFIG_PT_سطحون) {
        لکو("%p_pd {\n", مٿانهون_Pt_جڳهه);
        pt = مٿانهون_Pt_جڳهه;
    } وإلا {
        لکو("pt_%p_%04lu {\n", مٿانهون_Pt_جڳهه, pt_اشاريو);
        pt = getPPtrFromHWPTE(مٿانهون_Pt_جڳهه);
    }
    level -= 1;

    لفظ_t pt_بٽ_کاٻو = PT_اشاريو_بٽ * level + se_L4_صفحو_بٽ;

    /* - 1 to avoid overflowing */
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(pt_بٽ_کاٻو + PT_اشاريو_بٽ) - 1; i += (1 << (pt_بٽ_کاٻو))) {
        لفظ_t pt_جڳهه_اشاريو = ((i >> pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
        pte_t *pt_جڳهه = pt + pt_جڳهه_اشاريو;
        إذا (pte_ptr_get_valid(pt_جڳهه)) {
            إذا (level) { /* pt */
                لکو("0x%lx: pt_%p_%04lu\n", pt_جڳهه_اشاريو, pt_جڳهه, pt_جڳهه_اشاريو);
            } وإلا { /*چوکٺو*/
                لکو("0x%lx: frame_%p_%04lu", pt_جڳهه_اشاريو, pt_جڳهه, pt_جڳهه_اشاريو);
                cap_چوکٺو_print_attrs_pt(pt_جڳهه);
            }
        }
    }
    لکو("}\n"); /* lvl1pt/pt */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(pt_بٽ_کاٻو + PT_اشاريو_بٽ) - 1; i += (1 << (pt_بٽ_کاٻو))) {
        لفظ_t pt_جڳهه_اشاريو = ((i >> pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
        pte_t *pt_جڳهه = pt + pt_جڳهه_اشاريو;
        إذا (pte_ptr_get_valid(pt_جڳهه)) {
            إذا (level) { /* pt */
                riscv_cap_pt_print_slots(pt_جڳهه, pt_جڳهه_اشاريو, level);
            }
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_جو_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_سطحون);
    }
}

ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه)
{
    لکو("(");

    /* rights */
    إذا (pte_ptr_get_read(pt_جڳهه)) {
        لکو("R");
    }

    إذا (pte_ptr_get_write(pt_جڳهه)) {
        لکو("W");
    }

    إذا (pte_ptr_get_execute(pt_جڳهه)) {
        لکو("X");
    }

    /* cacheable not supported yet */

    لکو(")\n");
}

ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_بٽ_کاٻو == se_L4_صفحو_بٽ);
    لفظ_t جڳهه = ((vptr >> lu_ret.pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));

    لکو("frame_%p_%04lu ", lu_ret.pt_جڳهه, جڳهه);
    cap_چوکٺو_print_attrs_pt(lu_ret.pt_جڳهه);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_جدول_cap: {
        asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap);

        لفظ_t pt_بٽ_کاٻو = PT_اشاريو_بٽ * CONFIG_PT_سطحون + se_L4_صفحو_بٽ;
        لفظ_t جڳهه = ((vptr >> pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
        إذا (asid) {
            لکو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vptr).pt_جڳهه, جڳهه, (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pt_%p_%04lu\n", lookupPTSlot(ڳوليو_ret.vspace_پاڙ, vptr).pt_جڳهه, جڳهه);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        لکو("asid_control\n");
        اكسر;
    }
    حالة cap_چوکٺو_cap: {
        vptr_t vptr = cap_چوکٺو_cap_get_cap_FMapped_پتو(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(cap_چوکٺو_cap_get_cap_FMapped_ASID(cap));

        أكد(ڳوليو_ret.حالت == EXCEPTION_NONE);
        cap_چوکٺو_print_attrs_vptr(vptr, ڳوليو_ret.vspace_پاڙ);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        لکو("[unknown cap %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکٺو_print_attrs(paddr_t paddr)
{
    لکو("(4k, paddr: 0x%p)\n", (فراغ *)paddr);
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکٺو_cap:
    حالة cap_صفحو_جدول_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        لکو("[unknown object %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level)
{
    لفظ_t pt_بٽ_کاٻو = PT_اشاريو_بٽ * level + se_L4_صفحو_بٽ;

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(pt_بٽ_کاٻو + PT_اشاريو_بٽ); i += (1 << (pt_بٽ_کاٻو))) {
        لفظ_t pt_اشاريو = ((i >> pt_بٽ_کاٻو) & نقاب(PT_اشاريو_بٽ));
        pte_t *pt_جڳهه = pt + pt_اشاريو;
        إذا (pte_ptr_get_valid(pt_جڳهه)) {
            إذا (level) { /* pt */
                لکو("pt_%p_%04lu = pt\n", pt_جڳهه, pt_اشاريو);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_جڳهه), level - 1);
            } وإلا { /*چوکٺو*/
                paddr_t paddr = pte_ptr_get_ppn(pt_جڳهه);
                لکو("frame_%p_%04lu = frame ", pt_جڳهه, pt_اشاريو);
                obj_چوکٺو_print_attrs(paddr);
            }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_جو_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        لکو("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_سطحون - 1);
    }
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکو("arch riscv\n");
    لکو("objects {\n");
#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    print_objects();
#جيڪڏهن_پورو
    لکو("}\n");

    لکو("caps {\n");

    /* reset the seen list */
    reset_ڏٺل_list();

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING
    print_caps();
    لکو("}\n");

    obj_irq_print_maps();
#جيڪڏهن_پورو /* CONFIG_PRINTING */
}

#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
