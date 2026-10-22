/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <assert.h>
#అన్తర్భావ <kernel/boot.h>
#అన్తర్భావ <kernel/thread.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <machine/registerset.h>
#అన్తర్భావ <model/statedata.h>
#అన్తర్భావ <object/domain.h>
#అన్తర్భావ <arch/machine.h>
#అన్తర్భావ <arch/kernel/boot.h>
#అన్తర్భావ <arch/kernel/vspace.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <hardware.h>
#అన్తర్భావ <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS స్థిర region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
బాహ్య అక్షరం ki_boot_చివర[1];
BOOT_సంకేతం p_region_t get_p_reg_kernel_img_boot(శూన్య)
{
    తిరిగి (p_region_t) {
        .ప్రారంభం = kpptr_గమ్యం_paddr((స్థిరరాశి శూన్య *)KERNEL_ELF_పునాది),
        .చివర   = kpptr_గమ్యం_paddr(ki_boot_చివర)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_సంకేతం p_region_t get_p_reg_kernel_img(శూన్య)
{
    తిరిగి (p_region_t) {
        .ప్రారంభం = kpptr_గమ్యం_paddr((స్థిరరాశి శూన్య *)KERNEL_ELF_పునాది),
        .చివర   = kpptr_గమ్యం_paddr((స్థిరరాశి శూన్య *)KERNEL_ELF_శిఖరం)
    };
}

BOOT_సంకేతం స్థిర శూన్య విలీనం_regions(శూన్య)
{
    /* Walk through reserved regions and see if any can be merged */
    మాట_t శిఖరం = 0;
    కొరకు (మాట_t i = 1; i < ndks_boot.resv_లెక్క; i++) {
        ఒకవేళ (ndks_boot.reserved[శిఖరం].చివర == ndks_boot.reserved[i].ప్రారంభం) {
            /* extend earlier region */
            ndks_boot.reserved[శిఖరం].చివర = ndks_boot.reserved[i].చివర;
        } లేకపోతే {
            /* add region */
            శిఖరం++;
            ndks_boot.reserved[శిఖరం] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_లెక్క = ndks_boot.resv_లెక్క > 0 ? శిఖరం + 1 : 0;
}

BOOT_సంకేతం bool_t reserve_region(p_region_t reg)
{
    మాట_t i;
    ప్రతిజ్ఞా(reg.ప్రారంభం <= reg.చివర);
    ఒకవేళ (reg.ప్రారంభం == reg.చివర) {
        తిరిగి true;
    }

    /* keep the regions in order */
    కొరకు (i = 0; i < ndks_boot.resv_లెక్క; i++) {
        /* Try and merge the region to an existing one, if possible */
        ఒకవేళ (ndks_boot.reserved[i].ప్రారంభం == reg.చివర) {
            ndks_boot.reserved[i].ప్రారంభం = reg.ప్రారంభం;
            విలీనం_regions();
            తిరిగి true;
        }
        ఒకవేళ (ndks_boot.reserved[i].చివర == reg.ప్రారంభం) {
            ndks_boot.reserved[i].చివర = reg.చివర;
            విలీనం_regions();
            తిరిగి true;
        }
        /* Otherwise figure out where it should go. */
        ఒకవేళ (ndks_boot.reserved[i].ప్రారంభం > reg.చివర) {
            /* move regions down, making sure there's enough room */
            ఒకవేళ (ndks_boot.resv_లెక్క + 1 >= MAX_అంకె_RESV_REG) {
                రాయి("Can't mark region 0x%"SEL4_PRIx_మాట"-0x%"SEL4_PRIx_మాట
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.ప్రారంభం, reg.చివర, (పూర్ణాంకం)MAX_అంకె_RESV_REG);
                తిరిగి false;
            }
            కొరకు (మాట_t j = ndks_boot.resv_లెక్క; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_లెక్క++;
            తిరిగి true;
        }
    }

    ఒకవేళ (i + 1 == MAX_అంకె_RESV_REG) {
        రాయి("Can't mark region 0x%"SEL4_PRIx_మాట"-0x%"SEL4_PRIx_మాట
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.ప్రారంభం, reg.చివర, (పూర్ణాంకం)MAX_అంకె_RESV_REG);
        తిరిగి false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_లెక్క++;

    తిరిగి true;
}

BOOT_సంకేతం స్థిర bool_t చొప్పించు_region(region_t reg)
{
    ప్రతిజ్ఞా(reg.ప్రారంభం <= reg.చివర);
    ఒకవేళ (is_reg_ఖాళీలు(reg)) {
        తిరిగి true;
    }

    కొరకు (మాట_t i = 0; i < ARRAY_కొలత(ndks_boot.freemem); i++) {
        ఒకవేళ (is_reg_ఖాళీలు(ndks_boot.freemem[i])) {
            reserve_region(pptr_గమ్యం_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            తిరిగి true;
        }
    }

    /* We don't know if a platform or architecture picked MAX_NUM_FREEMEM_REG
     * arbitrarily or carefully calculated it to be big enough. Running out of
     * slots here is not really fatal, eventually memory allocation may fail
     * if there is not enough free memory. However, allocations should never
     * blindly assume to work, some error handling must always be in place even
     * if the environment has been crafted carefully to support them. Thus, we
     * don't stop the boot process here, but return an error. The caller should
     * decide how bad this is.
     */
    రాయి("no free memory slot left for [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.ప్రారంభం, reg.చివర, (గుర్తులేని పూర్ణాంకం)MAX_అంకె_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    ప్రతిజ్ఞా(0);

    తిరిగి false;
}

BOOT_సంకేతం స్థిర pptr_t alloc_rootserver_obj(మాట_t కొలత_బిట్లు, మాట_t n)
{
    pptr_t allocated = rootserver_mem.ప్రారంభం;
    /* allocated memory must be aligned */
    ప్రతిజ్ఞా(allocated % బిట్టు(కొలత_బిట్లు) == 0);
    rootserver_mem.ప్రారంభం += (n * బిట్టు(కొలత_బిట్లు));
    /* we must not have run out of memory */
    ప్రతిజ్ఞా(rootserver_mem.ప్రారంభం <= rootserver_mem.చివర);
    memzero((శూన్య *) allocated, n * బిట్టు(కొలత_బిట్లు));
    తిరిగి allocated;
}

BOOT_సంకేతం స్థిర మాట_t rootserver_max_కొలత_బిట్లు(మాట_t extra_bi_కొలత_బిట్లు)
{
    మాట_t cnode_కొలత_బిట్లు = CONFIG_మూలం_CNODE_కొలత_బిట్లు + se_L4_స్థానం_బిట్లు;
    మాట_t max = MAX(cnode_కొలత_బిట్లు, se_L4_VSpace_బిట్లు);
    తిరిగి MAX(max, extra_bi_కొలత_బిట్లు);
}

BOOT_సంకేతం స్థిర మాట_t calculate_rootserver_కొలత(v_region_t it_v_reg, మాట_t extra_bi_కొలత_బిట్లు)
{
    /* work out how much memory we need for root server objects */
    మాట_t కొలత = బిట్టు(CONFIG_మూలం_CNODE_కొలత_బిట్లు + se_L4_స్థానం_బిట్లు);
    కొలత += బిట్టు(seL4_TCBBits); // root thread tcb
    కొలత += బిట్టు(se_L4_పుట_బిట్లు); // ipc buf
    కొలత += బిట్టు(se_L4_Boot_Info_చట్రం_బిట్లు); // boot info
    కొలత += బిట్టు(se_L4_ASIDPool_బిట్లు);
    కొలత += extra_bi_కొలత_బిట్లు > 0 ? బిట్టు(extra_bi_కొలత_బిట్లు) : 0;
    కొలత += బిట్టు(se_L4_VSpace_బిట్లు); // root vspace
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కొలత += బిట్టు(se_L4_Min_Sched_Context_బిట్లు); // root sched context
#అయితే_ముగింపు
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    తిరిగి కొలత + arch_get_n_paging(it_v_reg) * బిట్టు(se_L4_పుట_పట్టిక_బిట్లు);
}

BOOT_సంకేతం స్థిర శూన్య maybe_alloc_extra_bi(మాట_t cmp_కొలత_బిట్లు, మాట_t extra_bi_కొలత_బిట్లు)
{
    ఒకవేళ (extra_bi_కొలత_బిట్లు >= cmp_కొలత_బిట్లు && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_కొలత_బిట్లు, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_సంకేతం స్థిర శూన్య create_rootserver_objects(pptr_t ప్రారంభం, v_region_t it_v_reg,
                                                మాట_t extra_bi_కొలత_బిట్లు)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    మాట_t cnode_కొలత_బిట్లు = CONFIG_మూలం_CNODE_కొలత_బిట్లు + se_L4_స్థానం_బిట్లు;
    మాట_t max = rootserver_max_కొలత_బిట్లు(extra_bi_కొలత_బిట్లు);

    మాట_t కొలత = calculate_rootserver_కొలత(it_v_reg, extra_bi_కొలత_బిట్లు);
    rootserver_mem.ప్రారంభం = ప్రారంభం;
    rootserver_mem.చివర = ప్రారంభం + కొలత;

    maybe_alloc_extra_bi(max, extra_bi_కొలత_బిట్లు);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#ఒకవేళ (CONFIG_మూలం_CNODE_కొలత_బిట్లు + se_L4_స్థానం_బిట్లు) > se_L4_VSpace_బిట్లు
    rootserver.cnode = alloc_rootserver_obj(cnode_కొలత_బిట్లు, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_బిట్లు, extra_bi_కొలత_బిట్లు);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_బిట్లు, 1);
#లేకపోతే
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_బిట్లు, 1);
    maybe_alloc_extra_bi(cnode_కొలత_బిట్లు, extra_bi_కొలత_బిట్లు);
    rootserver.cnode = alloc_rootserver_obj(cnode_కొలత_బిట్లు, 1);
#అయితే_ముగింపు

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_పుట_బిట్లు, extra_bi_కొలత_బిట్లు);
    compile_assert(invalid_se_L4_ASIDPool_బిట్లు, se_L4_ASIDPool_బిట్లు == se_L4_పుట_బిట్లు);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_బిట్లు, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_పుట_బిట్లు, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_చట్రం_బిట్లు, se_L4_Boot_Info_చట్రం_బిట్లు == se_L4_పుట_బిట్లు);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_చట్రం_బిట్లు, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#ఒకవేళ seL4_TCBBits >= se_L4_పుట_పట్టిక_బిట్లు
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#అయితే_ముగింపు

    /* paging structures are 4k on every arch except aarch32 (1k) */
    మాట_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.ప్రారంభం = alloc_rootserver_obj(se_L4_పుట_పట్టిక_బిట్లు, n);
    rootserver.paging.చివర = rootserver.paging.ప్రారంభం + n * బిట్టు(se_L4_పుట_పట్టిక_బిట్లు);

    /* for most archs, TCBs are smaller than page tables */
#ఒకవేళ seL4_TCBBits < se_L4_పుట_పట్టిక_బిట్లు
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#అయితే_ముగింపు

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_బిట్లు, 1);
#అయితే_ముగింపు
    /* we should have allocated all our memory */
    ప్రతిజ్ఞా(rootserver_mem.ప్రారంభం == rootserver_mem.చివర);
}

BOOT_సంకేతం శూన్య write_స్థానం(స్థానం_ptr_t స్థానం_ptr, cap_t cap)
{
    స్థానం_ptr->cap = cap;

    స్థానం_ptr->cteMDBNode = nullMDBNode;
    mdb_కణుపు_ptr_సమితి_mdb_Revocable(&స్థానం_ptr->cteMDBNode, true);
    mdb_కణుపు_ptr_సమితి_mdb_మొదటి_Badged(&స్థానం_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(మూలం_cnode_కొలత_valid,
               CONFIG_మూలం_CNODE_కొలత_బిట్లు < 32 - se_L4_స్థానం_బిట్లు &&
               బిట్టు(CONFIG_మూలం_CNODE_కొలత_బిట్లు) >= se_L4_అంకె_Initial_Caps &&
               CONFIG_మూలం_CNODE_కొలత_బిట్లు >= (se_L4_పుట_బిట్లు - se_L4_స్థానం_బిట్లు))

BOOT_సంకేతం cap_t
create_మూలం_cnode(శూన్య)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_మూలం_CNODE_కొలత_బిట్లు, /* radix */
                    మాట_బిట్లు - CONFIG_మూలం_CNODE_కొలత_బిట్లు, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_స్థానం(స్థానం_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    తిరిగి cap;
}

/* Check domain scheduler assumptions. */
compile_assert(అంకె_domains_valid,
               CONFIG_అంకె_DOMAINS >= 1 && CONFIG_అంకె_DOMAINS <= 256)
compile_assert(అంకె_priorities_valid,
               CONFIG_అంకె_PRIORITIES >= 1 && CONFIG_అంకె_PRIORITIES <= 256)

BOOT_సంకేతం శూన్య
create_domain_cap(cap_t మూలం_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapDomain), cap);
}

BOOT_సంకేతం cap_t create_ipcbuf_చట్రం_cap(cap_t మూలం_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_జ్ఞాపకం((శూన్య *)rootserver.ipc_buf, పుట_బిట్లు);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_చట్రం_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    తిరిగి cap;
}

BOOT_సంకేతం శూన్య create_bi_చట్రం_cap(cap_t మూలం_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_చట్రం_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), se_L4_Cap_Boot_Info_చట్రం), cap);
}

/**
 * the size_bits we return is 0 for extra_size = 0
 * and if it is non-zero the bits are always >= seL4_PageBits
 * this is relied on in a few places, and gives us code of the form
 *
 *     extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0
 *
 * which handles the 0-size case.
 */
BOOT_సంకేతం మాట_t calculate_extra_bi_కొలత_బిట్లు(మాట_t extra_కొలత)
{
    ఒకవేళ (extra_కొలత == 0) {
        తిరిగి 0;
    }

    మాట_t clzl_ret = clzl(విడత_పైకి(extra_కొలత, se_L4_పుట_బిట్లు));
    మాట_t msb = se_L4_మాట_బిట్లు - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    ఒకవేళ (extra_కొలత > బిట్టు(msb)) {
        msb++;
    }
    తిరిగి msb;
}

BOOT_సంకేతం శూన్య populate_bi_చట్రం(కణుపు_id_t కణుపు_id, మాట_t అంకె_nodes,
                                 vptr_t ipcbuf_vptr, మాట_t extra_bi_కొలత)
{
    /* clear boot info memory */
    clear_జ్ఞాపకం((శూన్య *)rootserver.boot_info, se_L4_Boot_Info_చట్రం_బిట్లు);
    ఒకవేళ (extra_bi_కొలత) {
        clear_జ్ఞాపకం((శూన్య *)rootserver.extra_bi,
                    calculate_extra_bi_కొలత_బిట్లు(extra_bi_కొలత));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->కణుపు_ID = కణుపు_id;
    bi->అంకె_Nodes = అంకె_nodes;
    bi->అంకె_IOPTLevels = 0;
    bi->ipc_బఫరు = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_కొలత_బిట్లు = CONFIG_మూలం_CNODE_కొలత_బిట్లు;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_కొలత;

    ndks_boot.bi_చట్రం = bi;
    ndks_boot.స్థానం_pos_cur = se_L4_అంకె_Initial_Caps;
}

BOOT_సంకేతం bool_t provide_cap(cap_t మూలం_cnode_cap, cap_t cap)
{
    ఒకవేళ (ndks_boot.స్థానం_pos_cur >= బిట్టు(CONFIG_మూలం_CNODE_కొలత_బిట్లు)) {
        రాయి("ERROR: can't add another cap, all %"SEL4_PRIu_మాట
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               బిట్టు(CONFIG_మూలం_CNODE_కొలత_బిట్లు));
        తిరిగి false;
    }
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), ndks_boot.స్థానం_pos_cur), cap);
    ndks_boot.స్థానం_pos_cur++;
    తిరిగి true;
}

BOOT_సంకేతం create_చట్రాలు_యొక్క_region_ret_t create_చట్రాలు_యొక్క_region(
    cap_t    మూలం_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_స్థానభ్రంశం
)
{
    pptr_t     f;
    cap_t      చట్రం_cap;
    se_L4_స్థానం_Pos స్థానం_pos_before;
    se_L4_స్థానం_Pos స్థానం_pos_after;

    స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;

    కొరకు (f = reg.ప్రారంభం; f < reg.చివర; f += బిట్టు(పుట_బిట్లు)) {
        ఒకవేళ (do_map) {
            చట్రం_cap = create_mapped_it_చట్రం_cap(pd_cap, f, pptr_గమ్యం_paddr((శూన్య *)(f - pv_స్థానభ్రంశం)), IT_ASID, false, true);
        } లేకపోతే {
            చట్రం_cap = create_unmapped_it_చట్రం_cap(f, false);
        }
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, చట్రం_cap)) {
            తిరిగి (create_చట్రాలు_యొక్క_region_ret_t) {
                .region  = S_REG_ఖాళీలు,
                .success = false
            };
        }
    }

    స్థానం_pos_after = ndks_boot.స్థానం_pos_cur;

    తిరిగి (create_చట్రాలు_యొక్క_region_ret_t) {
        .region = (se_L4_స్థానం_Region) {
            .ప్రారంభం = స్థానం_pos_before,
            .చివర   = స్థానం_pos_after
        },
        .success = true
    };
}

BOOT_సంకేతం cap_t create_it_asid_pool(cap_t మూలం_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_ఎగువ(IT_ASID) << asid_దిగువ_బిట్లు, rootserver.asid_pool);
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_స్థానం(
        స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    తిరిగి ap_cap;
}

#నిర్వచితమైతే CONFIG_KERNEL_MCS
BOOT_సంకేతం స్థిర శూన్య configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_సంకేతం bool_t init_sched_control(cap_t మూలం_cnode_cap, మాట_t అంకె_nodes)
{
    se_L4_స్థానం_Pos స్థానం_pos_before = ndks_boot.స్థానం_pos_cur;

    /* create a sched control cap for each core */
    కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < అంకె_nodes; i++) {
        ఒకవేళ (!provide_cap(మూలం_cnode_cap, cap_sched_control_cap_new(i))) {
            రాయి("can't init sched_control for node %u, provide_cap() failed\n", i);
            తిరిగి false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_చట్రం->schedcontrol = (se_L4_స్థానం_Region) {
        .ప్రారంభం = స్థానం_pos_before,
        .చివర = ndks_boot.స్థానం_pos_cur
    };

    తిరిగి true;
}
#అయితే_ముగింపు

BOOT_సంకేతం శూన్య create_idle_thread(శూన్య)
{
    pptr_t pptr;

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < CONFIG_MAX_అంకె_NODES; i++) {
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        కణుపు_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_స్థానభ్రంశం);
        configureIdleThread(కణుపు_STATE_ON_CORE(ksIdleThread, i));
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
        సమితి_Thread_Name(కణుపు_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#అయితే_ముగింపు
        SMP_COND_STATEMENT(కణుపు_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
        configure_sched_context(కణుపు_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(కణుపు_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        కణుపు_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#అయితే_ముగింపు
#నిర్వచితమైతే ENABLE_SMP_SUPPORT
    }
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */
}

BOOT_సంకేతం tcb_t *create_initial_thread(cap_t మూలం_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_నమోదు, vptr_t bi_చట్రం_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_స్థానభ్రంశం);
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    tcb->tcb_సమయం_ముక్క = CONFIG_సమయం_ముక్క;
#అయితే_ముగింపు

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    ఒకవేళ (dc_ret.స్థితి_2 != EXCEPTION_NONE) {
        రాయి("Failed to derive copy of IPC Buffer\n");
        తిరిగి ఖాళీ;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_చొప్పించు(
        మూలం_cnode_cap,
        స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadCNode),
        స్థానం_PTR(rootserver.tcb, tcbCTable)
    );
    cte_చొప్పించు(
        it_pd_cap,
        స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadVSpace),
        స్థానం_PTR(rootserver.tcb, tcbVTable)
    );
    cte_చొప్పించు(
        dc_ret.cap,
        స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadIPCBuffer),
        స్థానం_PTR(rootserver.tcb, tcb_బఫరు)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    సమితి_రిజిస్టరు(tcb, cap_రిజిస్టరు, bi_చట్రం_vptr);
    సమితి_తదుపరి_PC(tcb, ui_v_నమోదు);

    /* initialise TCB */
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_గమ్యం_Ticks(CONFIG_BOOT_THREAD_సమయం_ముక్క * US_లోపల_MS));
#అయితే_ముగింపు

    tcb->tcb_ప్రాధాన్యత = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    అమరిక_జవాబు_Master(tcb);
#అయితే_ముగింపు
    సమితి_Thread_State(tcb, ThreadState_Running);

#నిర్వచితం_కాకపోతే CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#అయితే_ముగింపు

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadTCB), cap);

#నిర్వచితమైతే CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_బిట్లు);
    write_స్థానం(స్థానం_PTR(pptr_యొక్క_cap(మూలం_cnode_cap), seL4_CapInitThreadSC), cap);
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    సమితి_Thread_Name(tcb, "rootserver");
#అయితే_ముగింపు

    తిరిగి tcb;
}

#నిర్వచితమైతే ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_సంకేతం స్థిర bool_t hypervisor_present(శూన్య)
{
#నిర్వచితమైతే CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    ఒకవేళ ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        తిరిగి true;
    }
#అయితే_ముగింపు
    తిరిగి false;
}

BOOT_సంకేతం శూన్య clock_sync_test(శూన్య)
{
    ticks_t t, t0;
    ticks_t margin = us_గమ్యం_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    ప్రతిజ్ఞా(getCurrentCPUIndex() != 0);
    t = కణుపు_STATE_ON_CORE(ks_Cur_సమయం, 0);
    చేయి {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = కణుపు_STATE_ON_CORE(ks_Cur_సమయం, 0);
    } అయితే (t0 == t);
    t = get_Current_సమయం();
    రాయి("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (పూర్ణాంకం)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    ఒకవేళ (hypervisor_present()) {
        రాయి("clock_sync_test[%d]: disabled, detected running as VM\n", (పూర్ణాంకం)getCurrentCPUIndex());
    } లేకపోతే {
        ప్రతిజ్ఞా(t0 <= margin + t && t <= t0 + margin);
    }
}
#అయితే_ముగింపు

BOOT_సంకేతం శూన్య init_core_state(tcb_t *scheduler_action)
{
#నిర్వచితమైతే CONFIG_HAVE_FPU
    కణుపు_STATE(ksCurFPUOwner) = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    కణుపు_STATE(ksDebugTCBs) = ఖాళీ;
    ఒకవేళ (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_ఎంచుకో_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(కణుపు_STATE(ksIdleThread));
#అయితే_ముగింపు
    కణుపు_STATE(ksSchedulerAction) = scheduler_action;
    కణుపు_STATE(ksCurThread) = కణుపు_STATE(ksIdleThread);
#నిర్వచితమైతే CONFIG_KERNEL_MCS
    కణుపు_STATE(ksCurSC) = కణుపు_STATE(ksCurThread->tcbSchedContext);
    కణుపు_STATE(ksConsumed) = 0;
    కణుపు_STATE(ksReprogram) = true;
    కణుపు_STATE(ks_Release_వరుస.తల) = ఖాళీ;
    కణుపు_STATE(ks_Release_వరుస.చివర) = ఖాళీ;
    కణుపు_STATE(ks_Cur_సమయం) = get_Current_సమయం();
#అయితే_ముగింపు
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_సమయం = DSCHED_MAX_DURATION;
    ks_Dom_కాలపట్టిక[0] = dschedule_make(0, DSCHED_MAX_DURATION);
}

/**
 * Sanity check if a kernel-virtual pointer is in the kernel window that maps
 * physical memory.
 *
 * This check is necessary, but not sufficient, because it only checks for the
 * pointer interval, not for any potential holes in the memory window.
 *
 * @param pptr the pointer to check
 * @return false if the pointer is definitely not in the kernel window, true
 *         otherwise.
 */
BOOT_సంకేతం స్థిర bool_t pptr_లోపల_kernel_కిటికీ(pptr_t pptr)
{
    తిరిగి pptr >= PPTR_పునాది && pptr < PPTR_శిఖరం;
}

/**
 * Create an untyped cap, store it in a cnode and mark it in boot info.
 *
 * The function can fail if basic sanity checks fail, or if there is no space in
 * boot info or cnode to store the cap.
 *
 * @param root_cnode_cap cap to the cnode to store the untyped cap in
 * @param device_memory true if the cap to create is a device untyped
 * @param pptr the kernel-virtual address of the untyped
 * @param size_bits the size of the untyped in bits
 * @param first_untyped_slot next available slot in the boot info structure
 * @return true on success, false on failure
 */
BOOT_సంకేతం స్థిర bool_t provide_untyped_cap(
    cap_t      మూలం_cnode_cap,
    bool_t     device_జ్ఞాపకం_2,
    pptr_t     pptr,
    మాట_t     కొలత_బిట్లు,
    se_L4_స్థానం_Pos మొదటి_untyped_స్థానం
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    ఒకవేళ (కొలత_బిట్లు > se_L4_Max_Untyped_బిట్లు || కొలత_బిట్లు < se_L4_Min_Untyped_బిట్లు) {
        రాయి("Kernel init: Invalid untyped size %"SEL4_PRIu_మాట"\n", కొలత_బిట్లు);
        తిరిగి false;
    }

    /* All cap ptrs must be aligned to object size */
    ఒకవేళ (!IS_ALIGNED(pptr, కొలత_బిట్లు)) {
        రాయి("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_మాట")\n", (శూన్య *)pptr, కొలత_బిట్లు);
        తిరిగి false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    ఒకవేళ (!device_జ్ఞాపకం_2 && !pptr_లోపల_kernel_కిటికీ(pptr)) {
        రాయి("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (శూన్య *)pptr);
        తిరిగి false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    ఒకవేళ (!device_జ్ఞాపకం_2 && !pptr_లోపల_kernel_కిటికీ(pptr + ముసుగు(కొలత_బిట్లు))) {
        రాయి("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_మాట")\n",
               (శూన్య *)pptr, కొలత_బిట్లు);
        తిరిగి false;
    }

    మాట_t i = ndks_boot.స్థానం_pos_cur - మొదటి_untyped_స్థానం;
    ఒకవేళ (i < CONFIG_MAX_అంకె_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_చట్రం->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_గమ్యం_paddr((శూన్య *)pptr),
            .కొలత_బిట్లు_2 = కొలత_బిట్లు,
            .isDevice = device_జ్ఞాపకం_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_సూచీ(కొలత_బిట్లు),
                                     device_జ్ఞాపకం_2, కొలత_బిట్లు, pptr);
        ret = provide_cap(మూలం_cnode_cap, ut_cap);
    } లేకపోతే {
        రాయి("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    తిరిగి ret;
}

/**
 * Create untyped caps for a region of kernel-virtual memory.
 *
 * Takes care of alignment, size and potentially wrapping memory regions. It is fine to provide a
 * region with end < start if the memory is device memory.
 *
 * If the region start is not aligned to seL4_MinUntypedBits, the part up to the next aligned
 * address will be ignored and is lost, because it is too small to create kernel objects in.
 *
 * @param root_cnode_cap Cap to the CNode to store the untypeds in.
 * @param device_memory  Whether the region is device memory.
 * @param reg Region of kernel-virtual memory. May wrap around.
 * @param first_untyped_slot First available untyped boot info slot.
 * @return true on success, false on failure.
 */
BOOT_సంకేతం స్థిర bool_t create_untypeds_for_region(
    cap_t      మూలం_cnode_cap,
    bool_t     device_జ్ఞాపకం_2,
    region_t   reg,
    se_L4_స్థానం_Pos మొదటి_untyped_స్థానం
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    అయితే (!is_reg_ఖాళీలు(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        గుర్తులేని పూర్ణాంకం కొలత_బిట్లు = se_L4_మాట_బిట్లు - 1 - clzl(reg.చివర - reg.ప్రారంభం);
        /* The size can't exceed the largest possible untyped size. */
        ఒకవేళ (కొలత_బిట్లు > se_L4_Max_Untyped_బిట్లు) {
            కొలత_బిట్లు = se_L4_Max_Untyped_బిట్లు;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        ఒకవేళ (0 != reg.ప్రారంభం) {
            గుర్తులేని పూర్ణాంకం align_బిట్లు = ctzl(reg.ప్రారంభం);
            ఒకవేళ (కొలత_బిట్లు > align_బిట్లు) {
                కొలత_బిట్లు = align_బిట్లు;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        ఒకవేళ (కొలత_బిట్లు >= se_L4_Min_Untyped_బిట్లు) {
            ఒకవేళ (!provide_untyped_cap(మూలం_cnode_cap, device_జ్ఞాపకం_2, reg.ప్రారంభం, కొలత_బిట్లు, మొదటి_untyped_స్థానం)) {
                తిరిగి false;
            }
        }
        reg.ప్రారంభం += బిట్టు(కొలత_బిట్లు);
    }
    తిరిగి true;
}

BOOT_సంకేతం bool_t create_untypeds(cap_t మూలం_cnode_cap)
{
    se_L4_స్థానం_Pos మొదటి_untyped_స్థానం = ndks_boot.స్థానం_pos_cur;

    paddr_t ప్రారంభం = 0;
    కొరకు (మాట_t i = 0; i < ndks_boot.resv_లెక్క; i++) {
        ఒకవేళ (ప్రారంభం < ndks_boot.reserved[i].ప్రారంభం) {
            region_t reg = paddr_గమ్యం_pptr_reg((p_region_t) {
                ప్రారంభం, ndks_boot.reserved[i].ప్రారంభం
            });
            ఒకవేళ (!create_untypeds_for_region(మూలం_cnode_cap, true, reg, మొదటి_untyped_స్థానం)) {
                రాయి("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") failed\n",
                       (గుర్తులేని పూర్ణాంకం)i, reg.ప్రారంభం, reg.చివర);
                తిరిగి false;
            }
        }

        ప్రారంభం = ndks_boot.reserved[i].చివర;
    }

    ఒకవేళ (ప్రారంభం <= CONFIG_PADDR_USER_DEVICE_శిఖరం - 1) {
        region_t reg = paddr_గమ్యం_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            ప్రారంభం, (paddr_t) CONFIG_PADDR_USER_DEVICE_శిఖరం
        });

        ఒకవేళ (!create_untypeds_for_region(మూలం_cnode_cap, true, reg, మొదటి_untyped_స్థానం)) {
            రాయి("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") failed\n",
                   reg.ప్రారంభం, reg.చివర);
            తిరిగి false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_గమ్యం_pptr_reg(get_p_reg_kernel_img_boot());
    ఒకవేళ (!create_untypeds_for_region(మూలం_cnode_cap, false, boot_mem_reuse_reg, మొదటి_untyped_స్థానం)) {
        రాయి("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట"] failed\n",
               boot_mem_reuse_reg.ప్రారంభం, boot_mem_reuse_reg.చివర);
        తిరిగి false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    కొరకు (మాట_t i = 0; i < ARRAY_కొలత(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_ఖాళీలు;
        ఒకవేళ (!create_untypeds_for_region(మూలం_cnode_cap, false, reg, మొదటి_untyped_స్థానం)) {
            రాయి("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") failed\n",
                   (గుర్తులేని పూర్ణాంకం)i, reg.ప్రారంభం, reg.చివర);
            తిరిగి false;
        }
    }

    ndks_boot.bi_చట్రం->untyped = (se_L4_స్థానం_Region) {
        .ప్రారంభం = మొదటి_untyped_స్థానం,
        .చివర   = ndks_boot.స్థానం_pos_cur
    };

    తిరిగి true;
}

BOOT_సంకేతం శూన్య bi_finalise(శూన్య)
{

    ఒకవేళ (rootserver.paging.ప్రారంభం != rootserver.paging.చివర) {
        రాయి("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.చివర - rootserver.paging.ప్రారంభం) >> se_L4_పుట_పట్టిక_బిట్లు);
    }

    ndks_boot.bi_చట్రం->ఖాళీలు = (se_L4_స్థానం_Region) {
        .ప్రారంభం = ndks_boot.స్థానం_pos_cur,
        .చివర   = బిట్టు(CONFIG_మూలం_CNODE_కొలత_బిట్లు)
    };
}

BOOT_సంకేతం స్థిర bool_t check_అందుబాటు_జ్ఞాపకం(మాట_t n_అందుబాటు,
                                               స్థిరరాశి p_region_t *అందుబాటు)
{
    /* The system configuration is broken if no region is available. */
    ఒకవేళ (0 == n_అందుబాటు) {
        రాయి("ERROR: no memory regions available\n");
        తిరిగి false;
    }

    రాయి("Available phys memory regions: %"SEL4_PRIu_మాట"\n", n_అందుబాటు);
    /* Force ordering and exclusivity of available regions. */
    కొరకు (మాట_t i = 0; i < n_అందుబాటు; i++) {
        స్థిరరాశి p_region_t *r = &అందుబాటు[i];
        రాయి("  [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట")\n", r->ప్రారంభం, r->చివర);

        /* Available regions must not wrap */
        ఒకవేళ (r->ప్రారంభం > r->చివర) {
            రాయి("ERROR: memory region %"SEL4_PRIu_మాట" has start > end\n", i);
            తిరిగి false;
        }

        /* Available regions must be non-empty. */
        ఒకవేళ (r->ప్రారంభం == r->చివర) {
            రాయి("ERROR: memory region %"SEL4_PRIu_మాట" empty\n", i);
            తిరిగి false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        ఒకవేళ ((i > 0) && (r->ప్రారంభం < అందుబాటు[i - 1].చివర)) {
            రాయి("ERROR: memory region %"SEL4_PRIu_మాట" in wrong order\n", i);
            తిరిగి false;
        }
    }

    తిరిగి true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS స్థిర region_t avail_reg[MAX_అంకె_FREEMEM_REG];

/**
 * Initialise avail_reg with the parts of the available memory regions that are
 * in the kernel window, converted to kernel pointers. Only this memory can be
 * used for kernel objects. On 32-bit architectures it is not uncommon for
 * regions to be completely or partially outside of the kernel window, because
 * the available address space is small. Memory outside of the kernel window is
 * discarded here, but it may be made available as device untyped memory instead
 * if it is not also in a reserved region. Regions must have passed
 * check_available_memory(). Returns the number of entries populated in
 * avail_reg.
 */
BOOT_సంకేతం స్థిర మాట_t init_avail_reg(మాట_t n_అందుబాటు,
                                       స్థిరరాశి p_region_t *అందుబాటు)
{
    మాట_t cnt = 0;
    కొరకు (మాట_t i = 0; i < n_అందుబాటు; i++) {
        స్థిరరాశి p_region_t *r = &అందుబాటు[i];

        ఒకవేళ ((r->ప్రారంభం >= PADDR_శిఖరం) || (r->చివర <= PADDR_పునాది)) {
            రాయి("  region [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->ప్రారంభం, r->చివర);
        } లేకపోతే {
            p_region_t usable_reg = {
                .ప్రారంభం = MAX(r->ప్రారంభం, PADDR_పునాది),
                .చివర   = MIN(r->చివర, PADDR_శిఖరం)
            };

            ఒకవేళ (cnt >= ARRAY_కొలత(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                రాయి("  WARNING: cannot use region [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట"),"
                       " avail_reg[] is full\n", r->ప్రారంభం, r->చివర);
                ప్రతిజ్ఞా(0);
            } లేకపోతే {
                avail_reg[cnt] = paddr_గమ్యం_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    తిరిగి cnt;
}


BOOT_సంకేతం స్థిర bool_t check_reserved_జ్ఞాపకం(మాట_t n_reserved,
                                              స్థిరరాశి region_t *reserved)
{
    రాయి("Reserved virt address space regions: %"SEL4_PRIu_మాట"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    కొరకు (మాట_t i = 0; i < n_reserved; i++) {
        స్థిరరాశి region_t *r = &reserved[i];
        రాయి("  [%"SEL4_PRIx_మాట"..%"SEL4_PRIx_మాట")\n", r->ప్రారంభం, r->చివర);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        ఒకవేళ (r->ప్రారంభం > r->చివర) {
            రాయి("ERROR: reserved region %"SEL4_PRIu_మాట" has start > end\n", i);
            తిరిగి false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        ఒకవేళ ((i > 0) && (r->ప్రారంభం < reserved[i - 1].చివర)) {
            రాయి("ERROR: reserved region %"SEL4_PRIu_మాట" in wrong order\n", i);
            తిరిగి false;
        }
    }

    తిరిగి true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_సంకేతం bool_t init_freemem(మాట_t n_అందుబాటు, స్థిరరాశి p_region_t *అందుబాటు,
                              మాట_t n_reserved, స్థిరరాశి region_t *reserved,
                              v_region_t it_v_reg, మాట_t extra_bi_కొలత_బిట్లు)
{
    ఒకవేళ (!check_అందుబాటు_జ్ఞాపకం(n_అందుబాటు, అందుబాటు)) {
        తిరిగి false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_అందుబాటు = init_avail_reg(n_అందుబాటు, అందుబాటు);
    ఒకవేళ (0 == n_అందుబాటు) {
        రాయి("ERROR: no available memory within the kernel window\n");
        తిరిగి false;
    }

    ఒకవేళ (!check_reserved_జ్ఞాపకం(n_reserved, reserved)) {
        తిరిగి false;
    }

    కొరకు (మాట_t i = 0; i < ARRAY_కొలత(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_ఖాళీలు;
    }

    మాట_t a = 0;
    మాట_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    అయితే (a < n_అందుబాటు && r < n_reserved) {
        ఒకవేళ (reserved[r].ప్రారంభం == reserved[r].చివర) {
            /* reserved region is empty - skip it */
            r++;
        } లేకపోతే ఒకవేళ (avail_reg[a].ప్రారంభం >= avail_reg[a].చివర) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } లేకపోతే ఒకవేళ (reserved[r].చివర <= avail_reg[a].ప్రారంభం) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_గమ్యం_paddr_reg(reserved[r]));
            r++;
        } లేకపోతే ఒకవేళ (reserved[r].ప్రారంభం >= avail_reg[a].చివర) {
            /* the reserved region is above the available region - take the whole thing */
            చొప్పించు_region(avail_reg[a]);
            a++;
        } లేకపోతే {
            /* the reserved region overlaps with the available region */
            ఒకవేళ (reserved[r].ప్రారంభం <= avail_reg[a].ప్రారంభం) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].ప్రారంభం = MIN(avail_reg[a].చివర, reserved[r].చివర);
                /* do not increment reserved index here - there could be more overlapping regions */
            } లేకపోతే {
                ప్రతిజ్ఞా(reserved[r].ప్రారంభం < avail_reg[a].చివర);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.చివర = reserved[r].ప్రారంభం;
                చొప్పించు_region(m);
                ఒకవేళ (avail_reg[a].చివర > reserved[r].చివర) {
                    avail_reg[a].ప్రారంభం = reserved[r].చివర;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } లేకపోతే {
                    a++;
                }
            }
        }
    }

    కొరకు (; r < n_reserved; r++) {
        ఒకవేళ (reserved[r].ప్రారంభం < reserved[r].చివర) {
            reserve_region(pptr_గమ్యం_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    కొరకు (; a < n_అందుబాటు; a++) {
        ఒకవేళ (avail_reg[a].ప్రారంభం < avail_reg[a].చివర) {
            చొప్పించు_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    పూర్ణాంకం i = ARRAY_కొలత(ndks_boot.freemem) - 1;
    ఒకవేళ (!is_reg_ఖాళీలు(ndks_boot.freemem[i])) {
        రాయి("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (గుర్తులేని పూర్ణాంకం)MAX_అంకె_FREEMEM_REG);
        తిరిగి false;
    }
    /* skip any empty regions */
    కొరకు (; i >= 0 && is_reg_ఖాళీలు(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    మాట_t కొలత = calculate_rootserver_కొలత(it_v_reg, extra_bi_కొలత_బిట్లు);
    మాట_t max = rootserver_max_కొలత_బిట్లు(extra_bi_కొలత_బిట్లు);
    కొరకు (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        ప్రతిజ్ఞా(i < ARRAY_కొలత(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        ప్రతిజ్ఞా(is_reg_ఖాళీలు(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        పూర్ణాంకం ఖాళీలు_సూచీ = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_ప్రారంభం = ndks_boot.freemem[i].చివర - కొలత;
        pptr_t ప్రారంభం = విడత_కిందికి(unaligned_ప్రారంభం, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        ఒకవేళ (unaligned_ప్రారంభం <= ndks_boot.freemem[i].చివర
            && ప్రారంభం >= ndks_boot.freemem[i].ప్రారంభం) {
            create_rootserver_objects(ప్రారంభం, it_v_reg, extra_bi_కొలత_బిట్లు);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[ఖాళీలు_సూచీ] = (region_t) {
                .ప్రారంభం = ప్రారంభం + కొలత,
                .చివర = ndks_boot.freemem[i].చివర
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].చివర = ప్రారంభం;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            తిరిగి true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[ఖాళీలు_సూచీ] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_ఖాళీలు;
    }

    /* We didn't find a big enough region. */
    రాయి("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_మాట"\n", max);
    తిరిగి false;
}
