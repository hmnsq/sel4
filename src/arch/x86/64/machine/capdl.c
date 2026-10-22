/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#اگر_تعریف_شده CONFIG_DEBUG_BUILD

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>

کلمه_t get_tcb_sp(tcb_t *tcb)
{
    أرجع tcb->tcbArch.tcbContext.registers[RSP];
}

#اگر_تعریف_شده CONFIG_PRINTING

ساكن فراغ obj_قاب_print_attrs(paddr_t paddr, کلمه_t صفحه_اندازه);
ساكن فراغ cap_قاب_print_attrs_vptr_2(کلمه_t vptr, vspace_ریشه_t *vspace);
ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, cap_t vspace);
ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ cap_قاب_print_attrs_pdpt(pdpte_t *pdpt_جایگاه);
ساكن فراغ cap_قاب_print_attrs_pd(pde_t *pd_جایگاه);
ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه);
ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t super_user, کلمه_t read_write, کلمه_t cache_disabled, کلمه_t xd);

/* use when only have access to pte of frames */
ساكن فراغ cap_قاب_print_attrs_pdpt(pdpte_t *pdpt_جایگاه)
{
    cap_قاب_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_جایگاه),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_جایگاه),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_جایگاه),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_جایگاه));
}
ساكن فراغ cap_قاب_print_attrs_pd(pde_t *pd_جایگاه)
{
    cap_قاب_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_جایگاه),
                               pde_pde_large_ptr_get_read_write(pd_جایگاه),
                               pde_pde_large_ptr_get_cache_disabled(pd_جایگاه),
                               pde_pde_large_ptr_get_xd(pd_جایگاه));
}

ساكن فراغ cap_قاب_print_attrs_pt(pte_t *pt_جایگاه)
{
    cap_قاب_print_attrs_impl(pte_ptr_get_super_user(pt_جایگاه),
                               pte_ptr_get_read_write(pt_جایگاه),
                               pte_ptr_get_cache_disabled(pt_جایگاه),
                               pte_ptr_get_xd(pt_جایگاه));
}

ساكن فراغ cap_قاب_print_attrs_impl(کلمه_t super_user, کلمه_t read_write, کلمه_t cache_disabled, کلمه_t xd)
{
    چاپ_کن("(");

    /* rights */
    إذا (read_write) {
        چاپ_کن("RW");
    } وإلا إذا (super_user) {
        چاپ_کن("R");
    }

    إذا (!xd) {
        چاپ_کن("X");
    }

    /* asid, mapping */

    إذا (cache_disabled) {
        چاپ_کن(", uncached");
    }

    چاپ_کن(")\n");
}

ساكن فراغ obj_قاب_print_attrs(paddr_t paddr, کلمه_t صفحه_اندازه)
{
    چاپ_کن("(");

    /* VM size */
    بدل (صفحه_اندازه) {
    حالة se_L4_Huge_صفحه_بیتها:
        چاپ_کن("1G");
        اكسر;
    حالة se_L4_Large_صفحه_بیتها:
        چاپ_کن("2M");
        اكسر;
    حالة se_L4_صفحه_بیتها:
        چاپ_کن("4k");
        اكسر;
    }

    چاپ_کن(", paddr: 0x%p)\n", (فراغ *)paddr);
}

ساكن فراغ x86_64_obj_pt_print_slots(pde_t *pd_جایگاه)
{
    paddr_t paddr;
    کلمه_t صفحه_اندازه;
    pte_t *pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_پایه_آدرس(pd_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_جابه_جایی + PT_شاخص_بیتها); i += (1UL << PT_شاخص_جابه_جایی)) {
        pte_t *pt_جایگاه = pt + GET_PT_شاخص(i);

        إذا (pte_ptr_get_present(pt_جایگاه)) {
            paddr = pte_ptr_get_صفحه_پایه_آدرس(pt_جایگاه);
            صفحه_اندازه = se_L4_صفحه_بیتها;
            چاپ_کن("frame_%p_%04lu = frame ", pt_جایگاه, GET_PT_شاخص(i));
            obj_قاب_print_attrs(paddr, صفحه_اندازه);
        }
    }
}

ساكن فراغ x86_64_obj_pd_print_slots(pdpte_t *pdpt_جایگاه)
{
    paddr_t paddr;
    کلمه_t صفحه_اندازه;
    pde_t *pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(pdpt_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PD_شاخص_جابه_جایی + PD_شاخص_بیتها); i += (1UL << PD_شاخص_جابه_جایی)) {
        pde_t *pd_جایگاه = pd + GET_PD_شاخص(i);

        إذا ((pde_ptr_get_صفحه_اندازه(pd_جایگاه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جایگاه)) {
            paddr = pde_pde_large_ptr_get_صفحه_پایه_آدرس(pd_جایگاه);
            صفحه_اندازه = se_L4_Large_صفحه_بیتها;

            چاپ_کن("frame_%p_%04lu = frame ", pd_جایگاه, GET_PD_شاخص(i));
            obj_قاب_print_attrs(paddr, صفحه_اندازه);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_جایگاه)) {
            چاپ_کن("pt_%p_%04lu = pt\n", pd_جایگاه, GET_PD_شاخص(i));
            x86_64_obj_pt_print_slots(pd_جایگاه);
        }
    }
}

ساكن فراغ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_جایگاه)
{
    paddr_t paddr;
    کلمه_t صفحه_اندازه;
    pdpte_t *pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه));

    لكل (کلمه_t i = 0; i < بیت(PDPT_شاخص_جابه_جایی + PDPT_شاخص_بیتها); i += (1UL << PDPT_شاخص_جابه_جایی)) {
        pdpte_t *pdpt_جایگاه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_جایگاه)) {
            paddr = pdpte_pdpte_1g_ptr_get_صفحه_پایه_آدرس(pdpt_جایگاه);
            صفحه_اندازه = se_L4_Huge_صفحه_بیتها;

            چاپ_کن("frame_%p_%04lu = frame ", pdpt_جایگاه, GET_PDPT_شاخص(i));
            obj_قاب_print_attrs(paddr, صفحه_اندازه);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_جایگاه)) {
            چاپ_کن("pd_%p_%04lu = pd\n", pdpt_جایگاه, GET_PDPT_شاخص(i));
            x86_64_obj_pd_print_slots(pdpt_جایگاه);
        }
    }
}

ساكن فراغ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    لكل (کلمه_t i = 0; i < بیت(PML4_شاخص_جابه_جایی + PML4_شاخص_بیتها); i += (1UL << PML4_شاخص_جابه_جایی)) {
        pml4e_t *pml4_جایگاه = lookup_PML4_جایگاه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جایگاه)) {
            چاپ_کن("pdpt_%p_%04lu = pdpt\n", pml4_جایگاه, GET_PML4_شاخص(i));
            x86_64_obj_pdpt_print_slots(pml4_جایگاه);
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_پایه_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        چاپ_کن("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_قاب_print_attrs_vptr_2(کلمه_t vptr, vspace_ریشه_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_جایگاه = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_جایگاه;
    lookupPDSlot_ret_t pd_جایگاه;

    إذا (pdpt_جایگاه.وضعیت == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه.pdpt_جایگاه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_جایگاه.pdpt_جایگاه)) {
        چاپ_کن("frame_%p_%04lu ", pdpt_جایگاه.pdpt_جایگاه, GET_PDPT_شاخص(vptr));
        cap_قاب_print_attrs_pdpt(pdpt_جایگاه.pdpt_جایگاه);
    } وإلا {
        pd_جایگاه = lookupPDSlot(vspace, vptr);
        إذا (pd_جایگاه.وضعیت == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحه_اندازه(pd_جایگاه.pd_جایگاه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_جایگاه.pd_جایگاه))) {
            چاپ_کن("frame_%p_%04lu ", pd_جایگاه.pd_جایگاه, GET_PD_شاخص(vptr));
            cap_قاب_print_attrs_pd(pd_جایگاه.pd_جایگاه);
        } وإلا {
            pt_جایگاه = lookupPTSlot(vspace, vptr);
            أكد(pt_جایگاه.وضعیت == EXCEPTION_NONE && pte_ptr_get_present(pt_جایگاه.pt_جایگاه));
            چاپ_کن("frame_%p_%04lu ", pt_جایگاه.pt_جایگاه, GET_PT_شاخص(vptr));
            cap_قاب_print_attrs_pt(pt_جایگاه.pt_جایگاه);
        }
    }
}

ساكن فراغ cap_قاب_print_attrs_vptr(کلمه_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
    cap_قاب_print_attrs_vptr_2(vptr, پیدا_کن_ret.vspace_ریشه);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific caps */
    حالة cap_صفحه_جدول_cap: {
        asid_t asid = cap_صفحه_جدول_cap_get_cap_PTMapped_ASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحه_جدول_cap_get_cap_PTMapped_آدرس(cap);
        إذا (asid) {
            چاپ_کن("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(پیدا_کن_ret.vspace_ریشه, vptr).pd_جایگاه, GET_PD_شاخص(vptr), (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pt_%p_%04lu\n", lookupPDSlot(پیدا_کن_ret.vspace_ریشه, vptr).pd_جایگاه, GET_PD_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_صفحه_directory_cap: {
        asid_t asid = cap_صفحه_directory_cap_get_cap_PDMapped_ASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحه_directory_cap_get_cap_PDMapped_آدرس(cap);
        إذا (asid) {
            چاپ_کن("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(پیدا_کن_ret.vspace_ریشه, vptr).pdpt_جایگاه, GET_PDPT_شاخص(vptr), (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pd_%p_%04lu\n", lookupPDPTSlot(پیدا_کن_ret.vspace_ریشه, vptr).pdpt_جایگاه, GET_PDPT_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_آدرس(cap);
        إذا (asid) {
            چاپ_کن("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_جایگاه(پیدا_کن_ret.vspace_ریشه, vptr), GET_PML4_شاخص(vptr), (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("pdpt_%p_%04lu\n", lookup_PML4_جایگاه(پیدا_کن_ret.vspace_ریشه, vptr), GET_PML4_شاخص(vptr));
        }
        اكسر;
    }
    حالة cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
        پیدا_کن_VSpace_For_ASID_ret_t پیدا_کن_ret = پیدا_کن_VSpace_For_ASID(asid);
        إذا (asid) {
            چاپ_کن("%p_pd (asid: %lu)\n",
                   پیدا_کن_ret.vspace_ریشه, (دراز بیعلامت صحيح)asid);
        } وإلا {
            چاپ_کن("%p_pd\n", پیدا_کن_ret.vspace_ریشه);
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
        cap_قاب_print_attrs_vptr_2(vptr, پیدا_کن_ret.vspace_ریشه);
        اكسر;
    }
    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool\n", (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        اكسر;
    }
#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر
    /* X86 specific caps */
    حالة cap_io_پورت_cap: {
        چاپ_کن("%p%p_io_port\n", (فراغ *)cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap),
               (فراغ *)cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap));
        اكسر;
    }
#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        اكسر;
    }
    حالة cap_io_صفحه_جدول_cap: {
        چاپ_کن("%p_iopt\n", (فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown cap %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    چاپ_کن("(asid_high: 0x%lx)\n", ASID_بالا(asid));
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific objects */
    حالة cap_قاب_cap:
    حالة cap_صفحه_جدول_cap:
    حالة cap_صفحه_directory_cap:
    حالة cap_pdpt_cap:
    حالة cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        چاپ_کن("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#اگر_تعریف_شده CONFIG_VTX
    حالة cap_vcpu_cap: {
        چاپ_کن("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#پایان_اگر
    /* X86 specific caps */
    حالة cap_io_پورت_cap: {
        چاپ_کن("%p%p_io_port = io_port ",
               (فراغ *)cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(cap),
               (فراغ *)cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(cap));
        x86_obj_ioports_print_attrs(cap);
        اكسر;
    }
#اگر_تعریف_شده CONFIG_IOMMU
    حالة cap_io_space_cap: {
        چاپ_کن("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        اكسر;
    }
    حالة cap_io_صفحه_جدول_cap: {
        چاپ_کن("%p_iopt = iopt ", (فراغ *)cap_io_صفحه_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        اكسر;
    }
#پایان_اگر
    افتراضي: {
        چاپ_کن("[unknown object %lu]\n", (دراز بیعلامت صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_ipc_میانگیر_جایگاه(tcb_t *tcb)
{
    کلمه_t vptr = tcb->tcbIPCBuffer;
    چاپ_کن("ipc_buffer_slot: ");
    cap_قاب_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ساكن فراغ x86_64_cap_pt_print_slots(pde_t *pd_جایگاه, vptr_t vptr)
{
    pte_t *pt = paddr_مقصد_pptr(pde_pde_pt_ptr_get_pt_پایه_آدرس(pd_جایگاه));
    چاپ_کن("pt_%p_%04lu {\n", pd_جایگاه, GET_PD_شاخص(vptr));

    لكل (کلمه_t i = 0; i < بیت(PT_شاخص_جابه_جایی + PT_شاخص_بیتها); i += (1UL << PT_شاخص_جابه_جایی)) {
        pte_t *pt_جایگاه = pt + GET_PT_شاخص(i);

        إذا (pte_ptr_get_present(pt_جایگاه)) {
            چاپ_کن("0x%lx: frame_%p_%04lu ", GET_PT_شاخص(i), pt_جایگاه, GET_PT_شاخص(i));
            cap_قاب_print_attrs_pt(pt_جایگاه);
        }
    }
    چاپ_کن("}\n"); /* pt */
}

ساكن فراغ x86_64_cap_pd_print_slots(pdpte_t *pdpt_جایگاه, vptr_t vptr)
{
    pde_t *pd = paddr_مقصد_pptr(pdpte_pdpte_pd_ptr_get_pd_پایه_آدرس(pdpt_جایگاه));
    چاپ_کن("pd_%p_%04lu {\n", pdpt_جایگاه, GET_PDPT_شاخص(vptr));

    لكل (کلمه_t i = 0; i < بیت(PD_شاخص_جابه_جایی + PD_شاخص_بیتها); i += (1UL << PD_شاخص_جابه_جایی)) {
        pde_t *pd_جایگاه = pd + GET_PD_شاخص(i);

        إذا ((pde_ptr_get_صفحه_اندازه(pd_جایگاه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جایگاه)) {
            چاپ_کن("0x%lx: frame_%p_%04lu ", GET_PD_شاخص(i), pd_جایگاه, GET_PD_شاخص(i));
            cap_قاب_print_attrs_pd(pd_جایگاه);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_جایگاه)) {
            چاپ_کن("0x%lx: pt_%p_%04lu\n", GET_PD_شاخص(i), pd_جایگاه, GET_PD_شاخص(i));
        }
    }
    چاپ_کن("}\n"); /* pd */

    لكل (کلمه_t i = 0; i < بیت(PD_شاخص_جابه_جایی + PD_شاخص_بیتها); i += (1UL << PD_شاخص_جابه_جایی)) {
        pde_t *pd_جایگاه = pd + GET_PD_شاخص(i);
        إذا ((pde_ptr_get_صفحه_اندازه(pd_جایگاه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_جایگاه)) {
            x86_64_cap_pt_print_slots(pd_جایگاه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_جایگاه, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_مقصد_pptr(pml4e_ptr_get_pdpt_پایه_آدرس(pml4_جایگاه));

    چاپ_کن("pdpt_%p_%04lu {\n", pml4_جایگاه, GET_PML4_شاخص(vptr));
    لكل (کلمه_t i = 0; i < بیت(PDPT_شاخص_جابه_جایی + PDPT_شاخص_بیتها); i += (1UL << PDPT_شاخص_جابه_جایی)) {
        pdpte_t *pdpt_جایگاه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_جایگاه)) {
            چاپ_کن("0x%lx: frame_%p_%04lu ", GET_PDPT_شاخص(i), pdpt_جایگاه, GET_PDPT_شاخص(i));
            cap_قاب_print_attrs_pdpt(pdpt_جایگاه);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_جایگاه)) {
            چاپ_کن("0x%lx: pd_%p_%04lu\n", GET_PDPT_شاخص(i), pdpt_جایگاه, GET_PDPT_شاخص(i));
        }
    }
    چاپ_کن("}\n"); /* pdpt */

    لكل (کلمه_t i = 0; i < بیت(PDPT_شاخص_جابه_جایی + PDPT_شاخص_بیتها); i += (1UL << PDPT_شاخص_جابه_جایی)) {
        pdpte_t *pdpt_جایگاه = pdpt + GET_PDPT_شاخص(i);

        إذا (pdpte_ptr_get_صفحه_اندازه(pdpt_جایگاه) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_جایگاه)) {
            x86_64_cap_pd_print_slots(pdpt_جایگاه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    چاپ_کن("%p_pd {\n", pml4);
    لكل (کلمه_t i = 0; i < بیت(PML4_شاخص_جابه_جایی + PML4_شاخص_بیتها); i += (1UL << PML4_شاخص_جابه_جایی)) {
        pml4e_t *pml4_جایگاه = lookup_PML4_جایگاه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جایگاه)) {
            چاپ_کن("0x%lx: pdpt_%p_%04lu\n", GET_PML4_شاخص(i), pml4_جایگاه, GET_PML4_شاخص(i));
        }
    }
    چاپ_کن("}\n"); /* pml4 */

    لكل (کلمه_t i = 0; i < بیت(PML4_شاخص_جابه_جایی + PML4_شاخص_بیتها); i += (1UL << PML4_شاخص_جابه_جایی)) {
        pml4e_t *pml4_جایگاه = lookup_PML4_جایگاه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جایگاه)) {
            x86_64_cap_pdpt_print_slots(pml4_جایگاه, i);
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_ریشه(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_پایه_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جمع_کن_مقصد_دیده(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#پایان_اگر /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    چاپ_کن("arch x86_64\n");
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
#پایان_اگر
}

#پایان_اگر /* CONFIG_DEBUG_BUILD */
