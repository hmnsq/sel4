/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <machine/capdl.h>
#ضمن <arch/machine/capdl.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP];
}

#جے_تعریف_شدہ CONFIG_PRINTING

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ obj_چوکھٹا_print_attrs(paddr_t paddr);
ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level);
ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt);
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں);

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکھو("(asid_high: 0x%lx)\n", ASID_بلند(asid));
}

فراغ print_ipc_بفر_تھاں(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
    لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);

    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, لبھو_ret.vspace_جڑ);
}

ساكن فراغ riscv_cap_pt_print_slots(pte_t *اتلا_Pt_تھاں, لفظ_t pt_اشاریہ, صحيح level)
{
    pte_t *pt;
    إذا (level == CONFIG_PT_پدھراں) {
        لکھو("%p_pd {\n", اتلا_Pt_تھاں);
        pt = اتلا_Pt_تھاں;
    } وإلا {
        لکھو("pt_%p_%04lu {\n", اتلا_Pt_تھاں, pt_اشاریہ);
        pt = getPPtrFromHWPTE(اتلا_Pt_تھاں);
    }
    level -= 1;

    لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_ورقہ_بٹ;

    /* - 1 to avoid overflowing */
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ) - 1; i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_تھاں_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_تھاں = pt + pt_تھاں_اشاریہ;
        إذا (pte_ptr_get_valid(pt_تھاں)) {
            إذا (level) { /* pt */
                لکھو("0x%lx: pt_%p_%04lu\n", pt_تھاں_اشاریہ, pt_تھاں, pt_تھاں_اشاریہ);
            } وإلا { /*چوکھٹا*/
                لکھو("0x%lx: frame_%p_%04lu", pt_تھاں_اشاریہ, pt_تھاں, pt_تھاں_اشاریہ);
                cap_چوکھٹا_print_attrs_pt(pt_تھاں);
            }
        }
    }
    لکھو("}\n"); /* lvl1pt/pt */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ) - 1; i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_تھاں_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_تھاں = pt + pt_تھاں_اشاریہ;
        إذا (pte_ptr_get_valid(pt_تھاں)) {
            إذا (level) { /* pt */
                riscv_cap_pt_print_slots(pt_تھاں, pt_تھاں_اشاریہ, level);
            }
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pte_t *lvl1pt = PTE_PTR(pptr_دا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        riscv_cap_pt_print_slots(lvl1pt, 0, CONFIG_PT_پدھراں);
    }
}

ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں)
{
    لکھو("(");

    /* rights */
    إذا (pte_ptr_get_read(pt_تھاں)) {
        لکھو("R");
    }

    إذا (pte_ptr_get_write(pt_تھاں)) {
        لکھو("W");
    }

    إذا (pte_ptr_get_execute(pt_تھاں)) {
        لکھو("X");
    }

    /* cacheable not supported yet */

    لکھو(")\n");
}

ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, pte_t *lvl1pt)
{
    lookupPTSlot_ret_t lu_ret = lookupPTSlot(lvl1pt, vptr);
    أكد(lu_ret.pt_بٹ_بایاں == se_L4_ورقہ_بٹ);
    لفظ_t تھاں = ((vptr >> lu_ret.pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));

    لکھو("frame_%p_%04lu ", lu_ret.pt_تھاں, تھاں);
    cap_چوکھٹا_print_attrs_pt(lu_ret.pt_تھاں);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_جدول_cap: {
        asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap);

        لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * CONFIG_PT_پدھراں + se_L4_ورقہ_بٹ;
        لفظ_t تھاں = ((vptr >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        إذا (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPTSlot(لبھو_ret.vspace_جڑ, vptr).pt_تھاں, تھاں, (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pt_%p_%04lu\n", lookupPTSlot(لبھو_ret.vspace_جڑ, vptr).pt_تھاں, تھاں);
        }
        اكسر;
    }
    حالة cap_asid_control_cap: {
        /* only one in the system */
        لکھو("asid_control\n");
        اكسر;
    }
    حالة cap_چوکھٹا_cap: {
        vptr_t vptr = cap_چوکھٹا_cap_get_cap_FMapped_پتہ(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(cap_چوکھٹا_cap_get_cap_FMapped_ASID(cap));

        أكد(لبھو_ret.حالت == EXCEPTION_NONE);
        cap_چوکھٹا_print_attrs_vptr(vptr, لبھو_ret.vspace_جڑ);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        لکھو("[unknown cap %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_چوکھٹا_print_attrs(paddr_t paddr)
{
    لکھو("(4k, paddr: 0x%p)\n", (فراغ *)paddr);
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکھٹا_cap:
    حالة cap_ورقہ_جدول_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
    /* riscv specific caps */
    /* nothing */
    افتراضي: {
        لکھو("[unknown object %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ riscv_obj_pt_print_slots(pte_t *lvl1pt, pte_t *pt, صحيح level)
{
    لفظ_t pt_بٹ_بایاں = PT_اشاریہ_بٹ * level + se_L4_ورقہ_بٹ;

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(pt_بٹ_بایاں + PT_اشاریہ_بٹ); i += (1 << (pt_بٹ_بایاں))) {
        لفظ_t pt_اشاریہ = ((i >> pt_بٹ_بایاں) & نقاب(PT_اشاریہ_بٹ));
        pte_t *pt_تھاں = pt + pt_اشاریہ;
        إذا (pte_ptr_get_valid(pt_تھاں)) {
            إذا (level) { /* pt */
                لکھو("pt_%p_%04lu = pt\n", pt_تھاں, pt_اشاریہ);
                riscv_obj_pt_print_slots(lvl1pt, getPPtrFromHWPTE(pt_تھاں), level - 1);
            } وإلا { /*چوکھٹا*/
                paddr_t paddr = pte_ptr_get_ppn(pt_تھاں);
                لکھو("frame_%p_%04lu = frame ", pt_تھاں, pt_اشاریہ);
                obj_چوکھٹا_print_attrs(paddr);
            }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_Valid_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        pte_t *lvl1pt = PTE_PTR(pptr_دا_cap(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        لکھو("%p_pd = pt\n", lvl1pt);
        riscv_obj_pt_print_slots(lvl1pt, lvl1pt, CONFIG_PT_پدھراں - 1);
    }
}

#جے_انت /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکھو("arch riscv\n");
    لکھو("objects {\n");
#جے_تعریف_شدہ CONFIG_PRINTING
    print_objects();
#جے_انت
    لکھو("}\n");

    لکھو("caps {\n");

    /* reset the seen list */
    reset_تکیا_list();

#جے_تعریف_شدہ CONFIG_PRINTING
    print_caps();
    لکھو("}\n");

    obj_irq_print_maps();
#جے_انت /* CONFIG_PRINTING */
}

#جے_انت /* CONFIG_DEBUG_BUILD */
