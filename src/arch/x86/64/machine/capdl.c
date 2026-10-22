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
    أرجع tcb->tcbArch.tcbContext.registers[RSP];
}

#جيڪڏهن_وصف_ٿيل CONFIG_PRINTING

ساكن فراغ obj_چوکٺو_print_attrs(paddr_t paddr, لفظ_t صفحو_سائيز);
ساكن فراغ cap_چوکٺو_print_attrs_vptr_2(لفظ_t vptr, vspace_پاڙ_t *vspace);
ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, cap_t vspace);
ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap);
ساكن فراغ cap_چوکٺو_print_attrs_pdpt(pdpte_t *pdpt_جڳهه);
ساكن فراغ cap_چوکٺو_print_attrs_pd(pde_t *pd_جڳهه);
ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه);
ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd);

/* use when only have access to pte of frames */
ساكن فراغ cap_چوکٺو_print_attrs_pdpt(pdpte_t *pdpt_جڳهه)
{
    cap_چوکٺو_print_attrs_impl(pdpte_pdpte_1g_ptr_get_super_user(pdpt_جڳهه),
                               pdpte_pdpte_1g_ptr_get_read_write(pdpt_جڳهه),
                               pdpte_pdpte_1g_ptr_get_cache_disabled(pdpt_جڳهه),
                               pdpte_pdpte_1g_ptr_get_xd(pdpt_جڳهه));
}
ساكن فراغ cap_چوکٺو_print_attrs_pd(pde_t *pd_جڳهه)
{
    cap_چوکٺو_print_attrs_impl(pde_pde_large_ptr_get_super_user(pd_جڳهه),
                               pde_pde_large_ptr_get_read_write(pd_جڳهه),
                               pde_pde_large_ptr_get_cache_disabled(pd_جڳهه),
                               pde_pde_large_ptr_get_xd(pd_جڳهه));
}

ساكن فراغ cap_چوکٺو_print_attrs_pt(pte_t *pt_جڳهه)
{
    cap_چوکٺو_print_attrs_impl(pte_ptr_get_super_user(pt_جڳهه),
                               pte_ptr_get_read_write(pt_جڳهه),
                               pte_ptr_get_cache_disabled(pt_جڳهه),
                               pte_ptr_get_xd(pt_جڳهه));
}

ساكن فراغ cap_چوکٺو_print_attrs_impl(لفظ_t super_user, لفظ_t read_write, لفظ_t cache_disabled, لفظ_t xd)
{
    لکو("(");

    /* rights */
    إذا (read_write) {
        لکو("RW");
    } وإلا إذا (super_user) {
        لکو("R");
    }

    إذا (!xd) {
        لکو("X");
    }

    /* asid, mapping */

    إذا (cache_disabled) {
        لکو(", uncached");
    }

    لکو(")\n");
}

ساكن فراغ obj_چوکٺو_print_attrs(paddr_t paddr, لفظ_t صفحو_سائيز)
{
    لکو("(");

    /* VM size */
    بدل (صفحو_سائيز) {
    حالة se_L4_Huge_صفحو_بٽ:
        لکو("1G");
        اكسر;
    حالة se_L4_Large_صفحو_بٽ:
        لکو("2M");
        اكسر;
    حالة se_L4_صفحو_بٽ:
        لکو("4k");
        اكسر;
    }

    لکو(", paddr: 0x%p)\n", (فراغ *)paddr);
}

ساكن فراغ x86_64_obj_pt_print_slots(pde_t *pd_جڳهه)
{
    paddr_t paddr;
    لفظ_t صفحو_سائيز;
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنياد_پتو(pd_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_وٿي + PT_اشاريو_بٽ); i += (1UL << PT_اشاريو_وٿي)) {
        pte_t *pt_جڳهه = pt + GET_PT_اشاريو(i);

        إذا (pte_ptr_get_present(pt_جڳهه)) {
            paddr = pte_ptr_get_صفحو_بنياد_پتو(pt_جڳهه);
            صفحو_سائيز = se_L4_صفحو_بٽ;
            لکو("frame_%p_%04lu = frame ", pt_جڳهه, GET_PT_اشاريو(i));
            obj_چوکٺو_print_attrs(paddr, صفحو_سائيز);
        }
    }
}

ساكن فراغ x86_64_obj_pd_print_slots(pdpte_t *pdpt_جڳهه)
{
    paddr_t paddr;
    لفظ_t صفحو_سائيز;
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(pdpt_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PD_اشاريو_وٿي + PD_اشاريو_بٽ); i += (1UL << PD_اشاريو_وٿي)) {
        pde_t *pd_جڳهه = pd + GET_PD_اشاريو(i);

        إذا ((pde_ptr_get_صفحو_سائيز(pd_جڳهه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جڳهه)) {
            paddr = pde_pde_large_ptr_get_صفحو_بنياد_پتو(pd_جڳهه);
            صفحو_سائيز = se_L4_Large_صفحو_بٽ;

            لکو("frame_%p_%04lu = frame ", pd_جڳهه, GET_PD_اشاريو(i));
            obj_چوکٺو_print_attrs(paddr, صفحو_سائيز);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_جڳهه)) {
            لکو("pt_%p_%04lu = pt\n", pd_جڳهه, GET_PD_اشاريو(i));
            x86_64_obj_pt_print_slots(pd_جڳهه);
        }
    }
}

ساكن فراغ x86_64_obj_pdpt_print_slots(pml4e_t *pml4_جڳهه)
{
    paddr_t paddr;
    لفظ_t صفحو_سائيز;
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4_جڳهه));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PDPT_اشاريو_وٿي + PDPT_اشاريو_بٽ); i += (1UL << PDPT_اشاريو_وٿي)) {
        pdpte_t *pdpt_جڳهه = pdpt + GET_PDPT_اشاريو(i);

        إذا (pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_جڳهه)) {
            paddr = pdpte_pdpte_1g_ptr_get_صفحو_بنياد_پتو(pdpt_جڳهه);
            صفحو_سائيز = se_L4_Huge_صفحو_بٽ;

            لکو("frame_%p_%04lu = frame ", pdpt_جڳهه, GET_PDPT_اشاريو(i));
            obj_چوکٺو_print_attrs(paddr, صفحو_سائيز);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_جڳهه)) {
            لکو("pd_%p_%04lu = pd\n", pdpt_جڳهه, GET_PDPT_اشاريو(i));
            x86_64_obj_pd_print_slots(pdpt_جڳهه);
        }
    }
}

ساكن فراغ x86_64_obj_pml4_print_slots(pml4e_t *pml4)
{
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PML4_اشاريو_وٿي + PML4_اشاريو_بٽ); i += (1UL << PML4_اشاريو_وٿي)) {
        pml4e_t *pml4_جڳهه = lookup_PML4_جڳهه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جڳهه)) {
            لکو("pdpt_%p_%04lu = pdpt\n", pml4_جڳهه, GET_PML4_اشاريو(i));
            x86_64_obj_pdpt_print_slots(pml4_جڳهه);
        }
    }
}

فراغ obj_tcb_print_vtable(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنياد_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        لکو("%p_pd = pml4\n", pml4);
        x86_64_obj_pml4_print_slots(pml4);
    }
}

/* use when only have access to vptr of frames */
ساكن فراغ cap_چوکٺو_print_attrs_vptr_2(لفظ_t vptr, vspace_پاڙ_t *vspace)
{
    lookupPDPTSlot_ret_t pdpt_جڳهه = lookupPDPTSlot(vspace, vptr);
    lookupPTSlot_ret_t pt_جڳهه;
    lookupPDSlot_ret_t pd_جڳهه;

    إذا (pdpt_جڳهه.حالت == EXCEPTION_NONE &&
        pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه.pdpt_جڳهه) == pdpte_pdpte_1g &&
        pdpte_pdpte_1g_ptr_get_present(pdpt_جڳهه.pdpt_جڳهه)) {
        لکو("frame_%p_%04lu ", pdpt_جڳهه.pdpt_جڳهه, GET_PDPT_اشاريو(vptr));
        cap_چوکٺو_print_attrs_pdpt(pdpt_جڳهه.pdpt_جڳهه);
    } وإلا {
        pd_جڳهه = lookupPDSlot(vspace, vptr);
        إذا (pd_جڳهه.حالت == EXCEPTION_NONE &&
            ((pde_ptr_get_صفحو_سائيز(pd_جڳهه.pd_جڳهه) == pde_pde_large) &&
             pde_pde_large_ptr_get_present(pd_جڳهه.pd_جڳهه))) {
            لکو("frame_%p_%04lu ", pd_جڳهه.pd_جڳهه, GET_PD_اشاريو(vptr));
            cap_چوکٺو_print_attrs_pd(pd_جڳهه.pd_جڳهه);
        } وإلا {
            pt_جڳهه = lookupPTSlot(vspace, vptr);
            أكد(pt_جڳهه.حالت == EXCEPTION_NONE && pte_ptr_get_present(pt_جڳهه.pt_جڳهه));
            لکو("frame_%p_%04lu ", pt_جڳهه.pt_جڳهه, GET_PT_اشاريو(vptr));
            cap_چوکٺو_print_attrs_pt(pt_جڳهه.pt_جڳهه);
        }
    }
}

ساكن فراغ cap_چوکٺو_print_attrs_vptr(لفظ_t vptr, cap_t vspace)
{
    asid_t asid = cap_pml4_cap_get_capPML4MappedASID(vspace);
    ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
    cap_چوکٺو_print_attrs_vptr_2(vptr, ڳوليو_ret.vspace_پاڙ);
}

فراغ print_cap_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific caps */
    حالة cap_صفحو_جدول_cap: {
        asid_t asid = cap_صفحو_جدول_cap_get_cap_PTMapped_ASID(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحو_جدول_cap_get_cap_PTMapped_پتو(cap);
        إذا (asid) {
            لکو("pt_%p_%04lu (asid: %lu)\n",
                   lookupPDSlot(ڳوليو_ret.vspace_پاڙ, vptr).pd_جڳهه, GET_PD_اشاريو(vptr), (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pt_%p_%04lu\n", lookupPDSlot(ڳوليو_ret.vspace_پاڙ, vptr).pd_جڳهه, GET_PD_اشاريو(vptr));
        }
        اكسر;
    }
    حالة cap_صفحو_directory_cap: {
        asid_t asid = cap_صفحو_directory_cap_get_cap_PDMapped_ASID(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_صفحو_directory_cap_get_cap_PDMapped_پتو(cap);
        إذا (asid) {
            لکو("pd_%p_%04lu (asid: %lu)\n",
                   lookupPDPTSlot(ڳوليو_ret.vspace_پاڙ, vptr).pdpt_جڳهه, GET_PDPT_اشاريو(vptr), (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pd_%p_%04lu\n", lookupPDPTSlot(ڳوليو_ret.vspace_پاڙ, vptr).pdpt_جڳهه, GET_PDPT_اشاريو(vptr));
        }
        اكسر;
    }
    حالة cap_pdpt_cap: {
        asid_t asid = cap_pdpt_cap_get_capPDPTMappedASID(cap);
        ڳوليو_VSpace_For_ASID_ret_t ڳوليو_ret = ڳوليو_VSpace_For_ASID(asid);
        vptr_t vptr = cap_pdpt_cap_get_cap_PDPTMapped_پتو(cap);
        إذا (asid) {
            لکو("pdpt_%p_%04lu (asid: %lu)\n",
                   lookup_PML4_جڳهه(ڳوليو_ret.vspace_پاڙ, vptr), GET_PML4_اشاريو(vptr), (ڊگهو بنانشان صحيح)asid);
        } وإلا {
            لکو("pdpt_%p_%04lu\n", lookup_PML4_جڳهه(ڳوليو_ret.vspace_پاڙ, vptr), GET_PML4_اشاريو(vptr));
        }
        اكسر;
    }
    حالة cap_pml4_cap: {
        asid_t asid = cap_pml4_cap_get_capPML4MappedASID(cap);
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
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    /* X86 specific caps */
    حالة cap_io_پورٽ_cap: {
        لکو("%p%p_io_port\n", (فراغ *)cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(cap),
               (فراغ *)cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(cap));
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space\n", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        اكسر;
    }
    حالة cap_io_صفحو_جدول_cap: {
        لکو("%p_iopt\n", (فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown cap %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

ساكن فراغ obj_asidpool_print_attrs(cap_t asid_cap)
{
    asid_t asid = cap_asid_pool_cap_get_capASIDBase(asid_cap);
    لکو("(asid_high: 0x%lx)\n", ASID_مٿاهون(asid));
}

فراغ print_object_arch(cap_t cap)
{
    بدل (cap_get_capType(cap)) {
    /* arch specific objects */
    حالة cap_چوکٺو_cap:
    حالة cap_صفحو_جدول_cap:
    حالة cap_صفحو_directory_cap:
    حالة cap_pdpt_cap:
    حالة cap_pml4_cap:
        /* don't need to deal with these objects since they get handled from vtable */
        اكسر;

    حالة cap_asid_pool_cap: {
        لکو("%p_asid_pool = asid_pool ",
               (فراغ *)cap_asid_pool_cap_get_capASIDPool(cap));
        obj_asidpool_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_VTX
    حالة cap_vcpu_cap: {
        لکو("%p_vcpu = vcpu\n", (فراغ *)cap_vcpu_cap_get_capVCPUPtr(cap));
        اكسر;
    }
#جيڪڏهن_پورو
    /* X86 specific caps */
    حالة cap_io_پورٽ_cap: {
        لکو("%p%p_io_port = io_port ",
               (فراغ *)cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(cap),
               (فراغ *)cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(cap));
        x86_obj_ioports_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU
    حالة cap_io_space_cap: {
        لکو("%p_io_space = io_space ", (فراغ *)cap_io_space_cap_get_capPCIDevice(cap));
        x86_obj_iospace_print_attrs(cap);
        اكسر;
    }
    حالة cap_io_صفحو_جدول_cap: {
        لکو("%p_iopt = iopt ", (فراغ *)cap_io_صفحو_جدول_cap_get_cap_IOPTBase_Ptr(cap));
        x86_obj_iopt_print_attrs(cap);
        اكسر;
    }
#جيڪڏهن_پورو
    افتراضي: {
        لکو("[unknown object %lu]\n", (ڊگهو بنانشان صحيح)cap_get_capType(cap));
        اكسر;
    }
    }
}

فراغ print_ipc_بفر_جڳهه(tcb_t *tcb)
{
    لفظ_t vptr = tcb->tcbIPCBuffer;
    لکو("ipc_buffer_slot: ");
    cap_چوکٺو_print_attrs_vptr(vptr, TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
}

ساكن فراغ x86_64_cap_pt_print_slots(pde_t *pd_جڳهه, vptr_t vptr)
{
    pte_t *pt = paddr_منزل_pptr(pde_pde_pt_ptr_get_pt_بنياد_پتو(pd_جڳهه));
    لکو("pt_%p_%04lu {\n", pd_جڳهه, GET_PD_اشاريو(vptr));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PT_اشاريو_وٿي + PT_اشاريو_بٽ); i += (1UL << PT_اشاريو_وٿي)) {
        pte_t *pt_جڳهه = pt + GET_PT_اشاريو(i);

        إذا (pte_ptr_get_present(pt_جڳهه)) {
            لکو("0x%lx: frame_%p_%04lu ", GET_PT_اشاريو(i), pt_جڳهه, GET_PT_اشاريو(i));
            cap_چوکٺو_print_attrs_pt(pt_جڳهه);
        }
    }
    لکو("}\n"); /* pt */
}

ساكن فراغ x86_64_cap_pd_print_slots(pdpte_t *pdpt_جڳهه, vptr_t vptr)
{
    pde_t *pd = paddr_منزل_pptr(pdpte_pdpte_pd_ptr_get_pd_بنياد_پتو(pdpt_جڳهه));
    لکو("pd_%p_%04lu {\n", pdpt_جڳهه, GET_PDPT_اشاريو(vptr));

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PD_اشاريو_وٿي + PD_اشاريو_بٽ); i += (1UL << PD_اشاريو_وٿي)) {
        pde_t *pd_جڳهه = pd + GET_PD_اشاريو(i);

        إذا ((pde_ptr_get_صفحو_سائيز(pd_جڳهه) == pde_pde_large) && pde_pde_large_ptr_get_present(pd_جڳهه)) {
            لکو("0x%lx: frame_%p_%04lu ", GET_PD_اشاريو(i), pd_جڳهه, GET_PD_اشاريو(i));
            cap_چوکٺو_print_attrs_pd(pd_جڳهه);

        } وإلا إذا (pde_pde_pt_ptr_get_present(pd_جڳهه)) {
            لکو("0x%lx: pt_%p_%04lu\n", GET_PD_اشاريو(i), pd_جڳهه, GET_PD_اشاريو(i));
        }
    }
    لکو("}\n"); /* pd */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PD_اشاريو_وٿي + PD_اشاريو_بٽ); i += (1UL << PD_اشاريو_وٿي)) {
        pde_t *pd_جڳهه = pd + GET_PD_اشاريو(i);
        إذا ((pde_ptr_get_صفحو_سائيز(pd_جڳهه) == pde_pde_pt) && pde_pde_pt_ptr_get_present(pd_جڳهه)) {
            x86_64_cap_pt_print_slots(pd_جڳهه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pdpt_print_slots(pml4e_t *pml4_جڳهه, vptr_t vptr)
{
    pdpte_t *pdpt = paddr_منزل_pptr(pml4e_ptr_get_pdpt_بنياد_پتو(pml4_جڳهه));

    لکو("pdpt_%p_%04lu {\n", pml4_جڳهه, GET_PML4_اشاريو(vptr));
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PDPT_اشاريو_وٿي + PDPT_اشاريو_بٽ); i += (1UL << PDPT_اشاريو_وٿي)) {
        pdpte_t *pdpt_جڳهه = pdpt + GET_PDPT_اشاريو(i);

        إذا (pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه) == pdpte_pdpte_1g &&
            pdpte_pdpte_1g_ptr_get_present(pdpt_جڳهه)) {
            لکو("0x%lx: frame_%p_%04lu ", GET_PDPT_اشاريو(i), pdpt_جڳهه, GET_PDPT_اشاريو(i));
            cap_چوکٺو_print_attrs_pdpt(pdpt_جڳهه);

        } وإلا إذا (pdpte_pdpte_pd_ptr_get_present(pdpt_جڳهه)) {
            لکو("0x%lx: pd_%p_%04lu\n", GET_PDPT_اشاريو(i), pdpt_جڳهه, GET_PDPT_اشاريو(i));
        }
    }
    لکو("}\n"); /* pdpt */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PDPT_اشاريو_وٿي + PDPT_اشاريو_بٽ); i += (1UL << PDPT_اشاريو_وٿي)) {
        pdpte_t *pdpt_جڳهه = pdpt + GET_PDPT_اشاريو(i);

        إذا (pdpte_ptr_get_صفحو_سائيز(pdpt_جڳهه) == pdpte_pdpte_pd && pdpte_pdpte_pd_ptr_get_present(pdpt_جڳهه)) {
            x86_64_cap_pd_print_slots(pdpt_جڳهه, i);
        }
    }
}

ساكن فراغ x86_64_cap_pml4_print_slots(pml4e_t *pml4)
{
    لکو("%p_pd {\n", pml4);
    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PML4_اشاريو_وٿي + PML4_اشاريو_بٽ); i += (1UL << PML4_اشاريو_وٿي)) {
        pml4e_t *pml4_جڳهه = lookup_PML4_جڳهه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جڳهه)) {
            لکو("0x%lx: pdpt_%p_%04lu\n", GET_PML4_اشاريو(i), pml4_جڳهه, GET_PML4_اشاريو(i));
        }
    }
    لکو("}\n"); /* pml4 */

    لكل (لفظ_t i = 0; i < ٻٽو_انگ(PML4_اشاريو_وٿي + PML4_اشاريو_بٽ); i += (1UL << PML4_اشاريو_وٿي)) {
        pml4e_t *pml4_جڳهه = lookup_PML4_جڳهه(pml4, i);
        إذا (pml4e_ptr_get_present(pml4_جڳهه)) {
            x86_64_cap_pdpt_print_slots(pml4_جڳهه, i);
        }
    }
}

فراغ obj_vtable_print_slots(tcb_t *tcb)
{
    /* levels: PML4 -> PDPT -> PD -> PT */
    إذا (is_Valid_VTable_پاڙ(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap) && !ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap)) {
        pml4e_t *pml4 = PML4E_PTR(cap_pml4_cap_get_cap_PML4_بنياد_Ptr(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap));
        جوڙيو_منزل_ڏٺل(TCB_PTR_CTE_PTR(tcb, tcbVTable)->cap);
        x86_64_cap_pml4_print_slots(pml4);
    }
}

#جيڪڏهن_پورو /* CONFIG_PRINTING */

فراغ debug_capDL(فراغ)
{
    لکو("arch x86_64\n");
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
#جيڪڏهن_پورو
}

#جيڪڏهن_پورو /* CONFIG_DEBUG_BUILD */
