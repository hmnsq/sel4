/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#جے_تعریف_شدہ CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

لفظ_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[SP_EL0];
}

#جے_تعریف_شدہ CONFIG_PRINTING

ساكن فراغ obj_چوکھٹا_print_attrs(vm_ورقہ_ناپ_t چوکھٹا_ناپ, paddr_t چوکھٹا_ادھار);
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں);
ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN);
ساكن فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace);

ساكن فراغ cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace_جڑ_2);

ساكن فراغ arm64_obj_pt_print_slots(pte_t *pd_تھاں);
ساكن فراغ arm64_obj_pd_print_slots(pte_t *pud_تھاں);
ساكن فراغ arm64_obj_pud_print_slots(فراغ *pgd_تھاں_or_vspace);

ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_تھاں, vptr_t vptr);
ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_تھاں, vptr_t vptr);
ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_تھاں_or_vspace, vptr_t vptr);

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
ساكن فراغ cap_چوکھٹا_print_attrs_pt(pte_t *pt_تھاں)
{
    cap_چوکھٹا_print_attrs_impl(pte_pte_ورقہ_ptr_get_SH(pt_تھاں),
                               pte_pte_ورقہ_ptr_get_AP(pt_تھاں),
                               pte_pte_ورقہ_ptr_get_UXN(pt_تھاں));
}

ساكن فراغ cap_چوکھٹا_print_attrs_impl(لفظ_t SH, لفظ_t AP, لفظ_t NXN)
{
    لکھو("(");

    /* rights */
    بدل (AP) {
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة 0b00:
        اكسر;
    حالة 0b01:
        لکھو("R");
        اكسر;
    حالة 0b10:
        لکھو("W");
        اكسر;
    حالة 0b11:
        لکھو("RW");
        اكسر;
#وإلا
    حالة 0b00:
        اكسر;
    حالة 0b01:
        لکھو("RW");
    حالة 0b10:
        اكسر;
    حالة 0b11:
        لکھو("R");
#جے_انت /* CONFIG_ARM_HYPERVISOR_SUPPORT */
    افتراضي:
        اكسر;
    }

    إذا (!NXN) {
        لکھو("X");
    }

    /* Only has effect if SMP enabled */
    إذا (SH != SMP_TERNARY(SMP_SHARE, 0)) {
        لکھو(", uncached");
    }

    لکھو(")\n");
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکھٹا_print_attrs_vptr_2(لفظ_t vptr, vspace_جڑ_t *vspace)
{
    lookupPTSlot_ret_t ret = lookupPTSlot(vspace, vptr);

    /* Check that the returned slot is a page. */
    إذا (!pte_ptr_get_valid(ret.pt_تھاں) ||
        (pte_pte_جدول_ptr_get_present(ret.pt_تھاں) && ret.pt_بٹ_بایاں > ورقہ_بٹ)) {
        أكد(0);
    }

    لفظ_t جدول_اشاریہ;
    بدل (ret.pt_بٹ_بایاں) {

    حالة ARMHuge_ورقہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1));
        اكسر;
    حالة ARMLarge_ورقہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2));
        اكسر;
    حالة ARMSmall_ورقہ:
        جدول_اشاریہ = GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(3));
        اكسر;
    افتراضي:
        أكد(0);

    }
    لکھو("frame_%p_%04lu ", ret.pt_تھاں, جدول_اشاریہ);
    cap_چوکھٹا_print_attrs_pt(ret.pt_تھاں);
}

فراغ cap_چوکھٹا_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    cap_چوکھٹا_print_attrs_vptr_2(vptr, VSPACE_PTR(pptr_دا_cap(vspace)));
}

/*
 * print object slots
 */
ساكن فراغ arm64_cap_pt_print_slots(pte_t *pd_تھاں, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pd_تھاں));
    لکھو("pt_%p_%04lu {\n", pd_تھاں, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(2)));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i ++) {
        pte_t *pt_تھاں = pt + i;

        إذا (pte_4k_ورقہ_ptr_get_present(pt_تھاں)) {
            // print pte entries
            لکھو("0x%lx: frame_%p_%04lu", i, pt_تھاں, i);
            cap_چوکھٹا_print_attrs_pt(pt_تھاں);
        }
    }
    لکھو("}\n"); /* pt */
}

ساكن فراغ arm64_cap_pd_print_slots(pte_t *pud_تھاں, vptr_t vptr)
{
    لکھو("pd_%p_%04lu {\n", pud_تھاں, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(1)));
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pud_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_تھاں = pd + i;

        بدل (pte_ptr_get_pte_type(pd_تھاں)) {

        حالة pte_pte_ورقہ:
            لکھو("0x%lx: frame_%p_%04lu", i, pd_تھاں, i);
            cap_چوکھٹا_print_attrs_pt(pd_تھاں);
            اكسر;

        حالة pte_pte_جدول:
            لکھو("0x%lx: pt_%p_%04lu\n", i, pd_تھاں, i);
            اكسر;
        }
    }

    لکھو("}\n"); /* pd */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_تھاں = pd + i;
        إذا (pte_ptr_get_pte_type(pd_تھاں) == pte_pte_جدول) {
            arm64_cap_pt_print_slots(pd_تھاں, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(2))));
        }
    }
}

ساكن فراغ arm64_cap_pud_print_slots(فراغ *pgd_تھاں_or_vspace, vptr_t vptr)
{
#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
    pte_t *pud = pgd_تھاں_or_vspace;
    لفظ_t اشاریہ_بٹ = se_L4_VSpace_اشاریہ_بٹ;
    لکھو("%p_pd {\n", pgd_تھاں_or_vspace);
#وإلا
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pgd_تھاں_or_vspace));
    لفظ_t اشاریہ_بٹ = se_L4_ورقہ_جدول_اشاریہ_بٹ;
    لکھو("pud_%p_%04lu {\n", pgd_تھاں_or_vspace, GET_UPT_اشاریہ(vptr, ULVL_FRM_ARM_PT_LVL(0)));
#جے_انت

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_تھاں = pud + i;
        إذا (pte_ptr_get_pte_type(pud_تھاں) == pte_pte_جدول) {
            لکھو("0x%lx: pd_%p_%04lu\n", i, pud_تھاں, i);
        }
    }

    لکھو("}\n"); /* pgd/pud */

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_تھاں = pud + GET_UPT_اشاریہ(i, ULVL_FRM_ARM_PT_LVL(1));
        إذا (pte_ptr_get_pte_type(pud_تھاں) == pte_pte_جدول) {
            arm64_cap_pd_print_slots(pud_تھاں, vptr + (i * GET_ULVL_PGSIZE(ULVL_FRM_ARM_PT_LVL(1))));
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    إذا (is_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_جڑ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
        * ARM hyp uses 3 level translation rather than the usual 4 level.
        * levels: PGD -> UPD -> PD -> PT
        */
#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
        arm64_cap_pud_print_slots(vspace, 0);
#وإلا
        لکھو("%p_pd {\n", vspace);
        لكل (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_تھاں = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_تھاں)) {
                لکھو("0x%lx: pud_%p_%04lu\n", i, pt_تھاں, i);
            }
        }
        لکھو("}\n"); /* pd */

        لكل (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_تھاں = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_تھاں)) {
                arm64_cap_pud_print_slots(pt_تھاں, i * GET_ULVL_PGSIZE(0));
            }
        }
#جے_انت
    }
}

فراغ print_ipc_بفر_تھاں(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکھو("ipc_buffer_slot: ");
    cap_چوکھٹا_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

فراغ print_cap_arch(cap_t cap)
{

    بدل (cap_get_capType(cap)) {
    حالة cap_ورقہ_جدول_cap: {
        asid_t asid = cap_ورقہ_جدول_cap_get_cap_PTMapped_ASID(cap);
        vptr_t vptr = cap_ورقہ_جدول_cap_get_cap_PTMapped_پتہ(cap);
        pte_t *نشانہ_pt = PT_PTR(cap_ورقہ_جدول_cap_get_cap_PTBase_Ptr(cap));

        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        pte_t *pt_تھاں = عدم;
        pte_t *pt = (pte_t *)لبھو_ret.vspace_جڑ;
        لفظ_t level;
        لكل (level = 0; level < UPT_پدھراں - 1 && pt != نشانہ_pt; level++) {
            pt_تھاں = pt + GET_UPT_اشاریہ(vptr, level);
            إذا (unlikely(!pte_pte_جدول_ptr_get_present(pt_تھاں))) {
                /* couldn't find it */
                اكسر;
            }
            pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pt_تھاں));
        }
        إذا (pt != نشانہ_pt) {
            /* didn't find it */
            اكسر;
        }


        إذا (asid) {
            لکھو("pt_%p_%04lu (asid: %lu)\n",
                   نشانہ_pt, GET_UPT_اشاریہ(vptr, level), (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("pt_%p_%04lu\n", نشانہ_pt, GET_UPT_اشاریہ(vptr, level));
        }
        اكسر;
    }
    حالة cap_vspace_cap: {
        asid_t asid = cap_vspace_cap_get_capVSMappedASID(cap);
        لبھو_VSpace_For_ASID_ret_t لبھو_ret = لبھو_VSpace_For_ASID(asid);
        إذا (asid) {
            لکھو("%p_pd (asid: %lu)\n",
                   لبھو_ret.vspace_جڑ, (لما بنانشانی صحيح)asid);
        } وإلا {
            لکھو("%p_pd\n", لبھو_ret.vspace_جڑ);
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
        cap_چوکھٹا_print_attrs_vptr_2(vptr, لبھو_ret.vspace_جڑ);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت

        /* ARM specific caps */
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown cap %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    حالة cap_چوکھٹا_cap:
    حالة cap_ورقہ_جدول_cap:
    حالة cap_vspace_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکھو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    حالة cap_vcpu_cap: {
        لکھو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جے_انت
        /* ARM specific objects */
#جے_تعریف_شدہ CONFIG_TK1_SMMU
    حالة cap_io_space_cap: {
        لکھو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capModuleID(cap));
        arm_obj_iospace_print_attrs(cap);
        اكسر;
    }
#جے_انت
    افتراضي: {
        لکھو("[unknown object %lu]\n", (لما بنانشانی صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ obj_چوکھٹا_print_attrs(vm_ورقہ_ناپ_t چوکھٹا_ناپ, paddr_t چوکھٹا_ادھار)
{
    لکھو("(");

    /* VM size */
    بدل (چوکھٹا_ناپ) {
    حالة ARMHuge_ورقہ:
        لکھو("1G");
        اكسر;
    حالة ARMLarge_ورقہ:
        لکھو("2M");
        اكسر;
    حالة ARMSmall_ورقہ:
        لکھو("4k");
        اكسر;
    }

    لکھو(", paddr: 0x%p)\n", (فراغ *)چوکھٹا_ادھار);
}

فراغ arm64_obj_pt_print_slots(pte_t *pd_تھاں)
{
    pte_t *pt = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pd_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pt_تھاں = pt + i;

        إذا (pte_4k_ورقہ_ptr_get_present(pt_تھاں)) {
            لکھو("frame_%p_%04lu = frame ", pt_تھاں, i);
            obj_چوکھٹا_print_attrs(ARMSmall_ورقہ, pte_ورقہ_ptr_get_ورقہ_ادھار_پتہ(pt_تھاں));
        }
    }
}

فراغ arm64_obj_pd_print_slots(pte_t *pud_تھاں)
{
    pte_t *pd = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pud_تھاں));

    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(PT_اشاریہ_بٹ); i++) {
        pte_t *pd_تھاں = pd + i;

        إذا (pte_ptr_get_pte_type(pd_تھاں) == pte_pte_ورقہ) {
            لکھو("frame_%p_%04lu = frame ", pd_تھاں, i);
            obj_چوکھٹا_print_attrs(ARMLarge_ورقہ, pte_ورقہ_ptr_get_ورقہ_ادھار_پتہ(pd_تھاں));
        }

        إذا (pte_ptr_get_pte_type(pd_تھاں) == pte_pte_جدول) {
            لکھو("pt_%p_%04lu = pt\n", pd_تھاں, i);
            arm64_obj_pt_print_slots(pd_تھاں);
        }
    }
}

فراغ arm64_obj_pud_print_slots(فراغ *pgd_تھاں_or_vspace)
{
    pte_t *pud = paddr_منزل_pptr(pte_pte_جدول_ptr_get_pt_ادھار_پتہ(pgd_تھاں_or_vspace));
#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
    لفظ_t اشاریہ_بٹ = se_L4_VSpace_اشاریہ_بٹ;
#وإلا
    لفظ_t اشاریہ_بٹ = se_L4_ورقہ_جدول_اشاریہ_بٹ;
#جے_انت
    لكل (لفظ_t i = 0; i < ثنائی_ہندسہ(اشاریہ_بٹ); i++) {
        pte_t *pud_تھاں = pud + i;

        بدل (pte_ptr_get_pte_type(pud_تھاں)) {
        حالة pte_pte_ورقہ:
            لکھو("frame_%p_%04lu = frame ", pud_تھاں, i);
            obj_چوکھٹا_print_attrs(ARMHuge_ورقہ, pte_ورقہ_ptr_get_ورقہ_ادھار_پتہ(pud_تھاں));
            اكسر;

        حالة pte_pte_جدول: {
            لکھو("pd_%p_%04lu = pd\n", pud_تھاں, i);
            arm64_obj_pd_print_slots(pud_تھاں);

        }
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    إذا (is_VTable_جڑ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        جوڑو_منزل_تکیا(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        vspace_جڑ_t *vspace = VSPACE_PTR(cap_vspace_cap_get_capVSBasePtr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));

        /*
         * ARM hyp uses 3 level translation rather than the usual 4 level.
         * levels: PGD -> PUD -> PD -> PT
         */
#جے_تعریف_شدہ AARCH64_VSPACE_S2_شروع_L1
        لکھو("%p_pd = pud\n", vspace);
        arm64_obj_pud_print_slots(vspace);
#وإلا
        لکھو("%p_pd = pgd\n", vspace);
        لكل (لفظ_t i = 0; i < PT_اشاریہ_بٹ; i++) {
            pte_t *pt_تھاں = vspace + i;
            إذا (pte_pte_جدول_ptr_get_present(pt_تھاں)) {
                لکھو("pud_%p_%04lu = pud\n", pt_تھاں, i);
                arm64_obj_pud_print_slots(pt_تھاں);
            }
        }
#جے_انت
    }
}

#جے_انت /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکھو("arch aarch64\n");
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
