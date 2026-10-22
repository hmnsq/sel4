/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

ساكن فراغ obj_چوکٺو_print_attrs(vm_صفحو_سائيز_t چوکٺو_سائيز, paddr_t چوکٺو_بنياد);
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه);
ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN);
ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, cap_t vspace);

ساكن فراغ cap_چوکٺو_print_attrs_vptr_2(لفظ_t vptr, vspace_پاڙ_t *vspace_پاڙ_2);

ساكن فراغ arm64_obj_pt_print_slots(pte_t *pd_جڳهه);
ساكن فراغ arm64_obj_pd_print_slots(pte_t *pud_جڳهه);
ساكن فراغ arm64_obj_pud_print_slots(فراغ *pgd_جڳهه_or_vspace);

ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_جڳهه, vptr_t vptr);
ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_جڳهه, vptr_t vptr);
ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_جڳهه_or_vspace, vptr_t vptr);

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
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه)
{
    cap_چوکٺو_print_attrs_impl(pte_pte_صفحو_ptr_get_SH(pt_جڳهه),
                               pte_pte_صفحو_ptr_get_AP(pt_جڳهه),
                               pte_pte_صفحو_ptr_get_UXN(pt_جڳهه));
}

ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN)
{
    لکو("(");

    /* rights */
    بدل (AP) {
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة 0b00:
        اكسر;
    حالة 0b01:
        لکو("R");
        اكسر;
    حالة 0b10:
        لکو("W");
        اكسر;
    حالة 0b11:
        لکو("RW");
        اكسر;
#وإلا
    حالة 0b00:
        اكسر;
    حالة 0b01:
        لکو("RW");
    حالة 0b10:
        اكسر;
    حالة 0b11:
        لکو("R");
#جيڪڏهن_پورو /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        اكسر;
    }

    إذا (!NXN) {
        لکو("X");
    }

    /* Only has effect if SMP enabled */
    إذا (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        لکو(", uncached");
    }

    لکو(")\n");
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکٺو_print_attrs_vptr_2(لفظ_t vptr, vspace_پاڙ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(ret.pt_جڳهه) ||
        (pte_pte_جدول_ptr_get_present(ret.pt_جڳهه) && ret.pt_بٽ_کاٻو > صفحو_بٽ)) {
        أكد(0);
    }

    لفظ_t جدول_اشاريو;
    بدل (ret.pt_بٽ_کاٻو) {

    حالة ARMHuge_صفحو:
        جدول_اشاريو = GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(1));
        اكسر;
    حالة ARMLarge_صفحو:
        جدول_اشاريو = GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(2));
        اكسر;
    حالة ARMSmall_صفحو:
        جدول_اشاريو = GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(3));
        اكسر;
    افتراضي:
        أكد(0);

    }
    لکو("frame_%p_%04lu ", ret.pt_جڳهه, جدول_اشاريو);
    cap_چوکٺو_print_attrs_pt(ret.pt_جڳهه);
}

فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    cap_چوکٺو_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_جو_cap(vspace)));
}

/*
 * print object slots
 */
ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_جڳهه, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pd_جڳهه));
    لکو("pt_%p_%04lu {\n", pd_جڳهه, GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i ++) {
        pte_t *pt_جڳهه = pt + i;

        إذا (pte_4k_صفحو_ptr_get_present(pt_جڳهه)) {
            // print pte entries
            لکو("0x%lx: frame_%p_%04lu", i, pt_جڳهه, i);
            cap_چوکٺو_print_attrs_pt(pt_جڳهه);
        }
    }
    لکو("}\n"); /* pt */
}

ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_جڳهه, vptr_t vptr)
{
    لکو("pd_%p_%04lu {\n", pud_جڳهه, GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pud_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        pte_t *pd_جڳهه = pd + i;

        بدل (pte_ptr_get_pte_type(pd_جڳهه)) {

        حالة pte_pte_صفحو:
            لکو("0x%lx: frame_%p_%04lu", i, pd_جڳهه, i);
            cap_چوکٺو_print_attrs_pt(pd_جڳهه);
            اكسر;

        حالة pte_pte_جدول:
            لکو("0x%lx: pt_%p_%04lu\n", i, pd_جڳهه, i);
            اكسر;
        }
    }

    لکو("}\n"); /* pd */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        pte_t *pd_جڳهه = pd + i;
        إذا (pte_ptr_get_pte_type(pd_جڳهه) == pte_pte_جدول) {
            arm64_cap_pt_print_slots(pd_جڳهه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_جڳهه_or_vspace, vptr_t vptr)
{
#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    pte_t *pud = pgd_جڳهه_or_vspace;
    لفظ_t اشاريو_بٽ = se_L4_VSpace_اشاريو_بٽ;
    لکو("%p_pd {\n", pgd_جڳهه_or_vspace);
#وإلا
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pgd_جڳهه_or_vspace));
    لفظ_t اشاريو_بٽ = se_L4_صفحو_جدول_اشاريو_بٽ;
    لکو("pud_%p_%04lu {\n", pgd_جڳهه_or_vspace, GET_UPT_اشاريو(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#جيڪڏهن_پورو

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(اشاريو_بٽ); i++) {
        pte_t *pud_جڳهه = pud + i;
        إذا (pte_ptr_get_pte_type(pud_جڳهه) == pte_pte_جدول) {
            لکو("0x%lx: pd_%p_%04lu\n", i, pud_جڳهه, i);
        }
    }

    لکو("}\n"); /* pgd/pud */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(اشاريو_بٽ); i++) {
        pte_t *pud_جڳهه = pud + GET_UPT_اشاريو(i, ULVL_FRM_ARM_PT_LVL(1));
        إذا (pte_ptr_get_pte_type(pud_جڳهه) == pte_pte_جدول) {
            arm64_cap_pd_print_slots(pud_جڳهه, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_پاڙ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وإلا
        لکو("%p_pd {\n", vspace);
        لكل (لفظ_t i = 0; i < PT_اشاريو_بٽ; i++) {
            pte_t *pt_جڳهه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جڳهه)) {
                لکو("0x%lx: pud_%p_%04lu\n", i, pt_جڳهه, i);
            }
        }
        لکو("}\n"); /* pd */

        لكل (لفظ_t i = 0; i < PT_اشاريو_بٽ; i++) {
            pte_t *pt_جڳهه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جڳهه)) {
                arm64_cap_pud_print_slots(pt_جڳهه, i * GET_ULVL_PGSIZE(0));
            }
        }
#جيڪڏهن_پورو
    }
}

فراغ print_ipc_بفر_جڳهه(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکو("ipc_buffer_slot: ");
    cap_چوکٺو_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

فراغ print_cap_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_صفحو_جدول_cap: {
        asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap);
        pte_t *هدف_pt = PT_PTR(cap_صفحو_جدول_cap_get_cap_PTBase_Ptr(cap));

        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        pte_t *pt_جڳهه = عدم;
        pte_t *pt = (pte_t *)ڳوليو_ret.vspace_پاڙ;
        لفظ_t level;
        لكل (level = 0; level < UPT_سطحون - 1 && pt != هدف_pt; level++) {
            pt_جڳهه = pt + GET_UPT_اشاريو(vptr, level);
            إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_جڳهه))) {
                /* couldn't find it */
                اكسر;
            }
            pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pt_جڳهه));
        }
        إذا (pt != هدف_pt) {
            /* didn't find it */
            اكسر;
        }


        إذا (asid) {
            لکو("pt_%p_%04lu (asid: %lu)\n",
                   هدف_pt, GET_UPT_اشاريو(vptr, level), (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pt_%p_%04lu\n", هدف_pt, GET_UPT_اشاريو(vptr, level));
        }
        اكسر;
    }
    حالة cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        إذا (asid) {
            لکو("%p_pd (asid: %lu)\n",
                   ڳوليو_ret.vspace_پاڙ, (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("%p_pd\n", ڳوليو_ret.vspace_پاڙ);
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
        cap_چوکٺو_print_attrs_vptr_2(vptr, ڳوليو_ret.vspace_پاڙ);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو

        /* ARM specific caps */
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown cap %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکٺو_cap:
    حالة cap_صفحو_جدول_cap:
    حالة cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو
        /* ARM specific objects */
#جيڪڏهن_وصف_ٿيل CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown object %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ obj_چوکٺو_print_attrs(vm_صفحو_سائيز_t چوکٺو_سائيز, paddr_t چوکٺو_بنياد)
{
    لکو("(");

    /* VM size */
    بدل (چوکٺو_سائيز) {
    حالة ARMHuge_صفحو:
        لکو("1G");
        اكسر;
    حالة ARMLarge_صفحو:
        لکو("2M");
        اكسر;
    حالة ARMSmall_صفحو:
        لکو("4k");
        اكسر;
    }

    لکو(", paddr: 0x%p)\n", (فراغ *)چوکٺو_بنياد);
}

فراغ arm64_obj_pt_print_slots(pte_t *pd_جڳهه)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pd_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        pte_t *pt_جڳهه = pt + i;

        إذا (pte_4k_صفحو_ptr_get_present(pt_جڳهه)) {
            لکو("frame_%p_%04lu = frame ", pt_جڳهه, i);
            obj_چوکٺو_print_attrs(ARMSmall_صفحو, pte_صفحو_ptr_get_صفحو_بنياد_پتو(pt_جڳهه));
        }
    }
}

فراغ arm64_obj_pd_print_slots(pte_t *pud_جڳهه)
{
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pud_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_بٽ); i++) {
        pte_t *pd_جڳهه = pd + i;

        إذا (pte_ptr_get_pte_type(pd_جڳهه) == pte_pte_صفحو) {
            لکو("frame_%p_%04lu = frame ", pd_جڳهه, i);
            obj_چوکٺو_print_attrs(ARMLarge_صفحو, pte_صفحو_ptr_get_صفحو_بنياد_پتو(pd_جڳهه));
        }

        إذا (pte_ptr_get_pte_type(pd_جڳهه) == pte_pte_جدول) {
            لکو("pt_%p_%04lu = pt\n", pd_جڳهه, i);
            arm64_obj_pt_print_slots(pd_جڳهه);
        }
    }
}

فراغ arm64_obj_pud_print_slots(فراغ *pgd_جڳهه_or_vspace)
{
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_بنياد_پتو(pgd_جڳهه_or_vspace));
#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
    لفظ_t اشاريو_بٽ = se_L4_VSpace_اشاريو_بٽ;
#وإلا
    لفظ_t اشاريو_بٽ = se_L4_صفحو_جدول_اشاريو_بٽ;
#جيڪڏهن_پورو
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(اشاريو_بٽ); i++) {
        pte_t *pud_جڳهه = pud + i;

        بدل (pte_ptr_get_pte_type(pud_جڳهه)) {
        حالة pte_pte_صفحو:
            لکو("frame_%p_%04lu = frame ", pud_جڳهه, i);
            obj_چوکٺو_print_attrs(ARMHuge_صفحو, pte_صفحو_ptr_get_صفحو_بنياد_پتو(pud_جڳهه));
            اكسر;

        حالة pte_pte_جدول: {
            لکو("pd_%p_%04lu = pd\n", pud_جڳهه, i);
            arm64_obj_pd_print_slots(pud_جڳهه);

        }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_پاڙ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#جيڪڏهن_وصف_ٿيل AARCH64_VSPACE_S2_شروعات_L1
        لکو("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وإلا
        لکو("%p_pd = pgd\n", vspace);
        لكل (لفظ_t i = 0; i < PT_اشاريو_بٽ; i++) {
            pte_t *pt_جڳهه = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_جڳهه)) {
                لکو("pud_%p_%04lu = pud\n", pt_جڳهه, i);
                arm64_obj_pud_print_slots(pt_جڳهه);
            }
        }
#جيڪڏهن_پورو
    }
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکو("arch aarch64\n");
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
