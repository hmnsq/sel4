/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <kernel/boot.h>
#അന്തര്ഭാവ <kernel/thread.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <machine/registerset.h>
#അന്തര്ഭാവ <model/statedata.h>
#അന്തര്ഭാവ <object/domain.h>
#അന്തര്ഭാവ <arch/machine.h>
#അന്തര്ഭാവ <arch/kernel/boot.h>
#അന്തര്ഭാവ <arch/kernel/vspace.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <hardware.h>
#അന്തര്ഭാവ <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS സ്ഥിതം region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
ബാഹ്യ അക്ഷരം ki_boot_end[1];
BOOT_CODE p_region_t get_p_reg_kernel_img_boot(ശൂന്യം)
{
    തിരികെ (p_region_t) {
        .start = kpptr_to_paddr((സ്ഥിരം ശൂന്യം *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr(ki_boot_end)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_CODE p_region_t get_p_reg_kernel_img(ശൂന്യം)
{
    തിരികെ (p_region_t) {
        .start = kpptr_to_paddr((സ്ഥിരം ശൂന്യം *)KERNEL_ELF_BASE),
        .end   = kpptr_to_paddr((സ്ഥിരം ശൂന്യം *)KERNEL_ELF_മുകൾ)
    };
}

BOOT_CODE സ്ഥിതം ശൂന്യം merge_regions(ശൂന്യം)
{
    /* Walk through reserved regions and see if any can be merged */
    word_t മുകൾ = 0;
    വേണ്ടി (word_t i = 1; i < ndks_boot.resv_എണ്ണം; i++) {
        എങ്കിൽ (ndks_boot.reserved[മുകൾ].end == ndks_boot.reserved[i].start) {
            /* extend earlier region */
            ndks_boot.reserved[മുകൾ].end = ndks_boot.reserved[i].end;
        } അല്ലെങ്കിൽ {
            /* add region */
            മുകൾ++;
            ndks_boot.reserved[മുകൾ] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_എണ്ണം = ndks_boot.resv_എണ്ണം > 0 ? മുകൾ + 1 : 0;
}

BOOT_CODE bool_t reserve_region(p_region_t reg)
{
    word_t i;
    പ്രതിജ്ഞാ(reg.start <= reg.end);
    എങ്കിൽ (reg.start == reg.end) {
        തിരികെ true;
    }

    /* keep the regions in order */
    വേണ്ടി (i = 0; i < ndks_boot.resv_എണ്ണം; i++) {
        /* Try and merge the region to an existing one, if possible */
        എങ്കിൽ (ndks_boot.reserved[i].start == reg.end) {
            ndks_boot.reserved[i].start = reg.start;
            merge_regions();
            തിരികെ true;
        }
        എങ്കിൽ (ndks_boot.reserved[i].end == reg.start) {
            ndks_boot.reserved[i].end = reg.end;
            merge_regions();
            തിരികെ true;
        }
        /* Otherwise figure out where it should go. */
        എങ്കിൽ (ndks_boot.reserved[i].start > reg.end) {
            /* move regions down, making sure there's enough room */
            എങ്കിൽ (ndks_boot.resv_എണ്ണം + 1 >= MAX_അക്കം_RESV_REG) {
                printf("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.start, reg.end, (പൂർണ്ണസംഖ്യ)MAX_അക്കം_RESV_REG);
                തിരികെ false;
            }
            വേണ്ടി (word_t j = ndks_boot.resv_എണ്ണം; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_എണ്ണം++;
            തിരികെ true;
        }
    }

    എങ്കിൽ (i + 1 == MAX_അക്കം_RESV_REG) {
        printf("Can't mark region 0x%"SEL4_PRIx_word"-0x%"SEL4_PRIx_word
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.start, reg.end, (പൂർണ്ണസംഖ്യ)MAX_അക്കം_RESV_REG);
        തിരികെ false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_എണ്ണം++;

    തിരികെ true;
}

BOOT_CODE സ്ഥിതം bool_t തിരുകുക_region(region_t reg)
{
    പ്രതിജ്ഞാ(reg.start <= reg.end);
    എങ്കിൽ (is_reg_empty(reg)) {
        തിരികെ true;
    }

    വേണ്ടി (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        എങ്കിൽ (is_reg_empty(ndks_boot.freemem[i])) {
            reserve_region(pptr_to_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            തിരികെ true;
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
    printf("no free memory slot left for [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.start, reg.end, (unsigned പൂർണ്ണസംഖ്യ)MAX_അക്കം_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    പ്രതിജ്ഞാ(0);

    തിരികെ false;
}

BOOT_CODE സ്ഥിതം pptr_t alloc_rootserver_obj(word_t size_bits, word_t n)
{
    pptr_t allocated = rootserver_mem.start;
    /* allocated memory must be aligned */
    പ്രതിജ്ഞാ(allocated % BIT(size_bits) == 0);
    rootserver_mem.start += (n * BIT(size_bits));
    /* we must not have run out of memory */
    പ്രതിജ്ഞാ(rootserver_mem.start <= rootserver_mem.end);
    memzero((ശൂന്യം *) allocated, n * BIT(size_bits));
    തിരികെ allocated;
}

BOOT_CODE സ്ഥിതം word_t rootserver_max_size_bits(word_t extra_bi_size_bits)
{
    word_t cnode_size_bits = CONFIG_വേര്_CNODE_SIZE_BITS + seL4_SlotBits;
    word_t max = MAX(cnode_size_bits, seL4_VSpaceBits);
    തിരികെ MAX(max, extra_bi_size_bits);
}

BOOT_CODE സ്ഥിതം word_t calculate_rootserver_size(v_region_t it_v_reg, word_t extra_bi_size_bits)
{
    /* work out how much memory we need for root server objects */
    word_t size = BIT(CONFIG_വേര്_CNODE_SIZE_BITS + seL4_SlotBits);
    size += BIT(seL4_TCBBits); // root thread tcb
    size += BIT(seL4_PageBits); // ipc buf
    size += BIT(seL4_BootInfoFrameBits); // boot info
    size += BIT(seL4_ASIDPoolBits);
    size += extra_bi_size_bits > 0 ? BIT(extra_bi_size_bits) : 0;
    size += BIT(seL4_VSpaceBits); // root vspace
#ifdef CONFIG_KERNEL_MCS
    size += BIT(seL4_MinSchedContextBits); // root sched context
#endif
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    തിരികെ size + arch_get_n_paging(it_v_reg) * BIT(seL4_PageTableBits);
}

BOOT_CODE സ്ഥിതം ശൂന്യം maybe_alloc_extra_bi(word_t cmp_size_bits, word_t extra_bi_size_bits)
{
    എങ്കിൽ (extra_bi_size_bits >= cmp_size_bits && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_size_bits, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_CODE സ്ഥിതം ശൂന്യം create_rootserver_objects(pptr_t start, v_region_t it_v_reg,
                                                word_t extra_bi_size_bits)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    word_t cnode_size_bits = CONFIG_വേര്_CNODE_SIZE_BITS + seL4_SlotBits;
    word_t max = rootserver_max_size_bits(extra_bi_size_bits);

    word_t size = calculate_rootserver_size(it_v_reg, extra_bi_size_bits);
    rootserver_mem.start = start;
    rootserver_mem.end = start + size;

    maybe_alloc_extra_bi(max, extra_bi_size_bits);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#എങ്കിൽ (CONFIG_വേര്_CNODE_SIZE_BITS + seL4_SlotBits) > seL4_VSpaceBits
    rootserver.cnode = alloc_rootserver_obj(cnode_size_bits, 1);
    maybe_alloc_extra_bi(seL4_VSpaceBits, extra_bi_size_bits);
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
#അല്ലെങ്കിൽ
    rootserver.vspace = alloc_rootserver_obj(seL4_VSpaceBits, 1);
    maybe_alloc_extra_bi(cnode_size_bits, extra_bi_size_bits);
    rootserver.cnode = alloc_rootserver_obj(cnode_size_bits, 1);
#endif

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(seL4_PageBits, extra_bi_size_bits);
    compile_assert(invalid_seL4_ASIDPoolBits, seL4_ASIDPoolBits == seL4_PageBits);
    rootserver.asid_pool = alloc_rootserver_obj(seL4_ASIDPoolBits, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(seL4_PageBits, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_seL4_BootInfoFrameBits, seL4_BootInfoFrameBits == seL4_PageBits);
    rootserver.boot_info = alloc_rootserver_obj(seL4_BootInfoFrameBits, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#എങ്കിൽ seL4_TCBBits >= seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#endif

    /* paging structures are 4k on every arch except aarch32 (1k) */
    word_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.start = alloc_rootserver_obj(seL4_PageTableBits, n);
    rootserver.paging.end = rootserver.paging.start + n * BIT(seL4_PageTableBits);

    /* for most archs, TCBs are smaller than page tables */
#എങ്കിൽ seL4_TCBBits < seL4_PageTableBits
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#endif

#ifdef CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(seL4_MinSchedContextBits, 1);
#endif
    /* we should have allocated all our memory */
    പ്രതിജ്ഞാ(rootserver_mem.start == rootserver_mem.end);
}

BOOT_CODE ശൂന്യം write_slot(slot_ptr_t slot_ptr, cap_t cap)
{
    slot_ptr->cap = cap;

    slot_ptr->cteMDBNode = nullMDBNode;
    mdb_കണ്ണി_ptr_set_mdb_Revocable(&slot_ptr->cteMDBNode, true);
    mdb_കണ്ണി_ptr_set_mdb_First_Badged(&slot_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(വേര്_cnode_size_valid,
               CONFIG_വേര്_CNODE_SIZE_BITS < 32 - seL4_SlotBits &&
               BIT(CONFIG_വേര്_CNODE_SIZE_BITS) >= se_L4_അക്കം_Initial_Caps &&
               CONFIG_വേര്_CNODE_SIZE_BITS >= (seL4_PageBits - seL4_SlotBits))

BOOT_CODE cap_t
create_വേര്_cnode(ശൂന്യം)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_വേര്_CNODE_SIZE_BITS, /* radix */
                    wordBits - CONFIG_വേര്_CNODE_SIZE_BITS, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_slot(SLOT_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    തിരികെ cap;
}

/* Check domain scheduler assumptions. */
compile_assert(അക്കം_domains_valid,
               CONFIG_അക്കം_DOMAINS >= 1 && CONFIG_അക്കം_DOMAINS <= 256)
compile_assert(അക്കം_priorities_valid,
               CONFIG_അക്കം_PRIORITIES >= 1 && CONFIG_അക്കം_PRIORITIES <= 256)

BOOT_CODE ശൂന്യം
create_domain_cap(cap_t വേര്_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapDomain), cap);
}

BOOT_CODE cap_t create_ipcbuf_frame_cap(cap_t വേര്_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clearMemory((ശൂന്യം *)rootserver.ipc_buf, PAGE_BITS);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    തിരികെ cap;
}

BOOT_CODE ശൂന്യം create_bi_frame_cap(cap_t വേര്_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_frame_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapBootInfoFrame), cap);
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
BOOT_CODE word_t calculate_extra_bi_size_bits(word_t extra_size)
{
    എങ്കിൽ (extra_size == 0) {
        തിരികെ 0;
    }

    word_t clzl_ret = clzl(ROUND_UP(extra_size, seL4_PageBits));
    word_t msb = seL4_WordBits - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    എങ്കിൽ (extra_size > BIT(msb)) {
        msb++;
    }
    തിരികെ msb;
}

BOOT_CODE ശൂന്യം populate_bi_frame(കണ്ണി_id_t കണ്ണി_id, word_t അക്കം_nodes,
                                 vptr_t ipcbuf_vptr, word_t extra_bi_size)
{
    /* clear boot info memory */
    clearMemory((ശൂന്യം *)rootserver.boot_info, seL4_BootInfoFrameBits);
    എങ്കിൽ (extra_bi_size) {
        clearMemory((ശൂന്യം *)rootserver.extra_bi,
                    calculate_extra_bi_size_bits(extra_bi_size));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->കണ്ണി_ID = കണ്ണി_id;
    bi->അക്കം_Nodes = അക്കം_nodes;
    bi->അക്കം_IOPTLevels = 0;
    bi->ipcBuffer = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->initThreadCNodeSizeBits = CONFIG_വേര്_CNODE_SIZE_BITS;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_size;

    ndks_boot.bi_frame = bi;
    ndks_boot.slot_pos_cur = se_L4_അക്കം_Initial_Caps;
}

BOOT_CODE bool_t provide_cap(cap_t വേര്_cnode_cap, cap_t cap)
{
    എങ്കിൽ (ndks_boot.slot_pos_cur >= BIT(CONFIG_വേര്_CNODE_SIZE_BITS)) {
        printf("ERROR: can't add another cap, all %"SEL4_PRIu_word
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               BIT(CONFIG_വേര്_CNODE_SIZE_BITS));
        തിരികെ false;
    }
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), ndks_boot.slot_pos_cur), cap);
    ndks_boot.slot_pos_cur++;
    തിരികെ true;
}

BOOT_CODE create_frames_of_region_ret_t create_frames_of_region(
    cap_t    വേര്_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_offset
)
{
    pptr_t     f;
    cap_t      frame_cap;
    seL4_SlotPos slot_pos_before;
    seL4_SlotPos slot_pos_after;

    slot_pos_before = ndks_boot.slot_pos_cur;

    വേണ്ടി (f = reg.start; f < reg.end; f += BIT(PAGE_BITS)) {
        എങ്കിൽ (do_map) {
            frame_cap = create_mapped_it_frame_cap(pd_cap, f, pptr_to_paddr((ശൂന്യം *)(f - pv_offset)), IT_ASID, false, true);
        } അല്ലെങ്കിൽ {
            frame_cap = create_unmapped_it_frame_cap(f, false);
        }
        എങ്കിൽ (!provide_cap(വേര്_cnode_cap, frame_cap)) {
            തിരികെ (create_frames_of_region_ret_t) {
                .region  = S_REG_EMPTY,
                .success = false
            };
        }
    }

    slot_pos_after = ndks_boot.slot_pos_cur;

    തിരികെ (create_frames_of_region_ret_t) {
        .region = (seL4_SlotRegion) {
            .start = slot_pos_before,
            .end   = slot_pos_after
        },
        .success = true
    };
}

BOOT_CODE cap_t create_it_asid_pool(cap_t വേര്_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_മുകളിലെ(IT_ASID) << asid_താഴെയുള്ള_Bits, rootserver.asid_pool);
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_slot(
        SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    തിരികെ ap_cap;
}

#ifdef CONFIG_KERNEL_MCS
BOOT_CODE സ്ഥിതം ശൂന്യം configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_CODE bool_t init_sched_control(cap_t വേര്_cnode_cap, word_t അക്കം_nodes)
{
    seL4_SlotPos slot_pos_before = ndks_boot.slot_pos_cur;

    /* create a sched control cap for each core */
    വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < അക്കം_nodes; i++) {
        എങ്കിൽ (!provide_cap(വേര്_cnode_cap, cap_sched_control_cap_new(i))) {
            printf("can't init sched_control for node %u, provide_cap() failed\n", i);
            തിരികെ false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_frame->schedcontrol = (seL4_SlotRegion) {
        .start = slot_pos_before,
        .end = ndks_boot.slot_pos_cur
    };

    തിരികെ true;
}
#endif

BOOT_CODE ശൂന്യം create_idle_thread(ശൂന്യം)
{
    pptr_t pptr;

#ifdef ENABLE_SMP_SUPPORT
    വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < CONFIG_MAX_അക്കം_NODES; i++) {
#endif /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        കണ്ണി_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_OFFSET);
        configureIdleThread(കണ്ണി_STATE_ON_CORE(ksIdleThread, i));
#ifdef CONFIG_DEBUG_BUILD
        setThreadName(കണ്ണി_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#endif
        SMP_COND_STATEMENT(കണ്ണി_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#ifdef CONFIG_KERNEL_MCS
        configure_sched_context(കണ്ണി_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(കണ്ണി_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        കണ്ണി_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#endif
#ifdef ENABLE_SMP_SUPPORT
    }
#endif /* ENABLE_SMP_SUPPORT */
}

BOOT_CODE tcb_t *create_initial_thread(cap_t വേര്_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_entry, vptr_t bi_frame_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_OFFSET);
#ifndef CONFIG_KERNEL_MCS
    tcb->tcbTimeSlice = CONFIG_TIME_SLICE;
#endif

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    എങ്കിൽ (dc_ret.status != EXCEPTION_NONE) {
        printf("Failed to derive copy of IPC Buffer\n");
        തിരികെ ശൂന്യ;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_തിരുകുക(
        വേര്_cnode_cap,
        SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadCNode),
        SLOT_PTR(rootserver.tcb, tcbCTable)
    );
    cte_തിരുകുക(
        it_pd_cap,
        SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadVSpace),
        SLOT_PTR(rootserver.tcb, tcbVTable)
    );
    cte_തിരുകുക(
        dc_ret.cap,
        SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadIPCBuffer),
        SLOT_PTR(rootserver.tcb, tcbBuffer)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    setRegister(tcb, capRegister, bi_frame_vptr);
    setNextPC(tcb, ui_v_entry);

    /* initialise TCB */
#ifdef CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), usToTicks(CONFIG_BOOT_THREAD_TIME_SLICE * US_IN_MS));
#endif

    tcb->tcbPriority = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#ifndef CONFIG_KERNEL_MCS
    setupReplyMaster(tcb);
#endif
    setThreadState(tcb, ThreadState_Running);

#ifndef CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#endif

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadTCB), cap);

#ifdef CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), seL4_MinSchedContextBits);
    write_slot(SLOT_PTR(pptr_of_cap(വേര്_cnode_cap), seL4_CapInitThreadSC), cap);
#endif
#ifdef CONFIG_DEBUG_BUILD
    setThreadName(tcb, "rootserver");
#endif

    തിരികെ tcb;
}

#ifdef ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_CODE സ്ഥിതം bool_t hypervisor_present(ശൂന്യം)
{
#ifdef CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    എങ്കിൽ ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        തിരികെ true;
    }
#endif
    തിരികെ false;
}

BOOT_CODE ശൂന്യം clock_sync_test(ശൂന്യം)
{
    ticks_t t, t0;
    ticks_t margin = usToTicks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    പ്രതിജ്ഞാ(getCurrentCPUIndex() != 0);
    t = കണ്ണി_STATE_ON_CORE(ksCurTime, 0);
    ചെയ്യുക {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = കണ്ണി_STATE_ON_CORE(ksCurTime, 0);
    } ആകുമ്പോൾ (t0 == t);
    t = getCurrentTime();
    printf("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (പൂർണ്ണസംഖ്യ)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    എങ്കിൽ (hypervisor_present()) {
        printf("clock_sync_test[%d]: disabled, detected running as VM\n", (പൂർണ്ണസംഖ്യ)getCurrentCPUIndex());
    } അല്ലെങ്കിൽ {
        പ്രതിജ്ഞാ(t0 <= margin + t && t <= t0 + margin);
    }
}
#endif

BOOT_CODE ശൂന്യം init_core_state(tcb_t *scheduler_action)
{
#ifdef CONFIG_HAVE_FPU
    കണ്ണി_STATE(ksCurFPUOwner) = ശൂന്യ;
#endif
#ifdef CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    കണ്ണി_STATE(ksDebugTCBs) = ശൂന്യ;
    എങ്കിൽ (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != SchedulerAction_ChooseNewThread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(കണ്ണി_STATE(ksIdleThread));
#endif
    കണ്ണി_STATE(ksSchedulerAction) = scheduler_action;
    കണ്ണി_STATE(ksCurThread) = കണ്ണി_STATE(ksIdleThread);
#ifdef CONFIG_KERNEL_MCS
    കണ്ണി_STATE(ksCurSC) = കണ്ണി_STATE(ksCurThread->tcbSchedContext);
    കണ്ണി_STATE(ksConsumed) = 0;
    കണ്ണി_STATE(ksReprogram) = true;
    കണ്ണി_STATE(ksReleaseQueue.head) = ശൂന്യ;
    കണ്ണി_STATE(ksReleaseQueue.end) = ശൂന്യ;
    കണ്ണി_STATE(ksCurTime) = getCurrentTime();
#endif
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ksDomainTime = DSCHED_MAX_DURATION;
    ksDomSchedule[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_CODE സ്ഥിതം bool_t pptr_in_kernel_window(pptr_t pptr)
{
    തിരികെ pptr >= PPTR_BASE && pptr < PPTR_മുകൾ;
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
BOOT_CODE സ്ഥിതം bool_t provide_untyped_cap(
    cap_t      വേര്_cnode_cap,
    bool_t     device_memory,
    pptr_t     pptr,
    word_t     size_bits,
    seL4_SlotPos first_untyped_slot
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    എങ്കിൽ (size_bits > seL4_MaxUntypedBits || size_bits < seL4_MinUntypedBits) {
        printf("Kernel init: Invalid untyped size %"SEL4_PRIu_word"\n", size_bits);
        തിരികെ false;
    }

    /* All cap ptrs must be aligned to object size */
    എങ്കിൽ (!IS_ALIGNED(pptr, size_bits)) {
        printf("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_word")\n", (ശൂന്യം *)pptr, size_bits);
        തിരികെ false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    എങ്കിൽ (!device_memory && !pptr_in_kernel_window(pptr)) {
        printf("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (ശൂന്യം *)pptr);
        തിരികെ false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    എങ്കിൽ (!device_memory && !pptr_in_kernel_window(pptr + MASK(size_bits))) {
        printf("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_word")\n",
               (ശൂന്യം *)pptr, size_bits);
        തിരികെ false;
    }

    word_t i = ndks_boot.slot_pos_cur - first_untyped_slot;
    എങ്കിൽ (i < CONFIG_MAX_അക്കം_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_frame->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_to_paddr((ശൂന്യം *)pptr),
            .sizeBits = size_bits,
            .isDevice = device_memory,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_INDEX(size_bits),
                                     device_memory, size_bits, pptr);
        ret = provide_cap(വേര്_cnode_cap, ut_cap);
    } അല്ലെങ്കിൽ {
        printf("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    തിരികെ ret;
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
BOOT_CODE സ്ഥിതം bool_t create_untypeds_for_region(
    cap_t      വേര്_cnode_cap,
    bool_t     device_memory,
    region_t   reg,
    seL4_SlotPos first_untyped_slot
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    ആകുമ്പോൾ (!is_reg_empty(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        unsigned പൂർണ്ണസംഖ്യ size_bits = seL4_WordBits - 1 - clzl(reg.end - reg.start);
        /* The size can't exceed the largest possible untyped size. */
        എങ്കിൽ (size_bits > seL4_MaxUntypedBits) {
            size_bits = seL4_MaxUntypedBits;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        എങ്കിൽ (0 != reg.start) {
            unsigned പൂർണ്ണസംഖ്യ align_bits = ctzl(reg.start);
            എങ്കിൽ (size_bits > align_bits) {
                size_bits = align_bits;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        എങ്കിൽ (size_bits >= seL4_MinUntypedBits) {
            എങ്കിൽ (!provide_untyped_cap(വേര്_cnode_cap, device_memory, reg.start, size_bits, first_untyped_slot)) {
                തിരികെ false;
            }
        }
        reg.start += BIT(size_bits);
    }
    തിരികെ true;
}

BOOT_CODE bool_t create_untypeds(cap_t വേര്_cnode_cap)
{
    seL4_SlotPos first_untyped_slot = ndks_boot.slot_pos_cur;

    paddr_t start = 0;
    വേണ്ടി (word_t i = 0; i < ndks_boot.resv_എണ്ണം; i++) {
        എങ്കിൽ (start < ndks_boot.reserved[i].start) {
            region_t reg = paddr_to_pptr_reg((p_region_t) {
                start, ndks_boot.reserved[i].start
            });
            എങ്കിൽ (!create_untypeds_for_region(വേര്_cnode_cap, true, reg, first_untyped_slot)) {
                printf("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                       (unsigned പൂർണ്ണസംഖ്യ)i, reg.start, reg.end);
                തിരികെ false;
            }
        }

        start = ndks_boot.reserved[i].end;
    }

    എങ്കിൽ (start <= CONFIG_PADDR_USER_DEVICE_മുകൾ - 1) {
        region_t reg = paddr_to_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            start, (paddr_t) CONFIG_PADDR_USER_DEVICE_മുകൾ
        });

        എങ്കിൽ (!create_untypeds_for_region(വേര്_cnode_cap, true, reg, first_untyped_slot)) {
            printf("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   reg.start, reg.end);
            തിരികെ false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_to_pptr_reg(get_p_reg_kernel_img_boot());
    എങ്കിൽ (!create_untypeds_for_region(വേര്_cnode_cap, false, boot_mem_reuse_reg, first_untyped_slot)) {
        printf("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"] failed\n",
               boot_mem_reuse_reg.start, boot_mem_reuse_reg.end);
        തിരികെ false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    വേണ്ടി (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_EMPTY;
        എങ്കിൽ (!create_untypeds_for_region(വേര്_cnode_cap, false, reg, first_untyped_slot)) {
            printf("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") failed\n",
                   (unsigned പൂർണ്ണസംഖ്യ)i, reg.start, reg.end);
            തിരികെ false;
        }
    }

    ndks_boot.bi_frame->untyped = (seL4_SlotRegion) {
        .start = first_untyped_slot,
        .end   = ndks_boot.slot_pos_cur
    };

    തിരികെ true;
}

BOOT_CODE ശൂന്യം bi_finalise(ശൂന്യം)
{

    എങ്കിൽ (rootserver.paging.start != rootserver.paging.end) {
        printf("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.end - rootserver.paging.start) >> seL4_PageTableBits);
    }

    ndks_boot.bi_frame->empty = (seL4_SlotRegion) {
        .start = ndks_boot.slot_pos_cur,
        .end   = BIT(CONFIG_വേര്_CNODE_SIZE_BITS)
    };
}

BOOT_CODE സ്ഥിതം bool_t check_available_memory(word_t n_available,
                                               സ്ഥിരം p_region_t *available)
{
    /* The system configuration is broken if no region is available. */
    എങ്കിൽ (0 == n_available) {
        printf("ERROR: no memory regions available\n");
        തിരികെ false;
    }

    printf("Available phys memory regions: %"SEL4_PRIu_word"\n", n_available);
    /* Force ordering and exclusivity of available regions. */
    വേണ്ടി (word_t i = 0; i < n_available; i++) {
        സ്ഥിരം p_region_t *r = &available[i];
        printf("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Available regions must not wrap */
        എങ്കിൽ (r->start > r->end) {
            printf("ERROR: memory region %"SEL4_PRIu_word" has start > end\n", i);
            തിരികെ false;
        }

        /* Available regions must be non-empty. */
        എങ്കിൽ (r->start == r->end) {
            printf("ERROR: memory region %"SEL4_PRIu_word" empty\n", i);
            തിരികെ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        എങ്കിൽ ((i > 0) && (r->start < available[i - 1].end)) {
            printf("ERROR: memory region %"SEL4_PRIu_word" in wrong order\n", i);
            തിരികെ false;
        }
    }

    തിരികെ true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS സ്ഥിതം region_t avail_reg[MAX_അക്കം_FREEMEM_REG];

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
BOOT_CODE സ്ഥിതം word_t init_avail_reg(word_t n_available,
                                       സ്ഥിരം p_region_t *available)
{
    word_t cnt = 0;
    വേണ്ടി (word_t i = 0; i < n_available; i++) {
        സ്ഥിരം p_region_t *r = &available[i];

        എങ്കിൽ ((r->start >= PADDR_മുകൾ) || (r->end <= PADDR_BASE)) {
            printf("  region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->start, r->end);
        } അല്ലെങ്കിൽ {
            p_region_t usable_reg = {
                .start = MAX(r->start, PADDR_BASE),
                .end   = MIN(r->end, PADDR_മുകൾ)
            };

            എങ്കിൽ (cnt >= ARRAY_SIZE(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                printf("  WARNING: cannot use region [%"SEL4_PRIx_word"..%"SEL4_PRIx_word"),"
                       " avail_reg[] is full\n", r->start, r->end);
                പ്രതിജ്ഞാ(0);
            } അല്ലെങ്കിൽ {
                avail_reg[cnt] = paddr_to_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    തിരികെ cnt;
}


BOOT_CODE സ്ഥിതം bool_t check_reserved_memory(word_t n_reserved,
                                              സ്ഥിരം region_t *reserved)
{
    printf("Reserved virt address space regions: %"SEL4_PRIu_word"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    വേണ്ടി (word_t i = 0; i < n_reserved; i++) {
        സ്ഥിരം region_t *r = &reserved[i];
        printf("  [%"SEL4_PRIx_word"..%"SEL4_PRIx_word")\n", r->start, r->end);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        എങ്കിൽ (r->start > r->end) {
            printf("ERROR: reserved region %"SEL4_PRIu_word" has start > end\n", i);
            തിരികെ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        എങ്കിൽ ((i > 0) && (r->start < reserved[i - 1].end)) {
            printf("ERROR: reserved region %"SEL4_PRIu_word" in wrong order\n", i);
            തിരികെ false;
        }
    }

    തിരികെ true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_CODE bool_t init_freemem(word_t n_available, സ്ഥിരം p_region_t *available,
                              word_t n_reserved, സ്ഥിരം region_t *reserved,
                              v_region_t it_v_reg, word_t extra_bi_size_bits)
{
    എങ്കിൽ (!check_available_memory(n_available, available)) {
        തിരികെ false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_available = init_avail_reg(n_available, available);
    എങ്കിൽ (0 == n_available) {
        printf("ERROR: no available memory within the kernel window\n");
        തിരികെ false;
    }

    എങ്കിൽ (!check_reserved_memory(n_reserved, reserved)) {
        തിരികെ false;
    }

    വേണ്ടി (word_t i = 0; i < ARRAY_SIZE(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    word_t a = 0;
    word_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    ആകുമ്പോൾ (a < n_available && r < n_reserved) {
        എങ്കിൽ (reserved[r].start == reserved[r].end) {
            /* reserved region is empty - skip it */
            r++;
        } അല്ലെങ്കിൽ എങ്കിൽ (avail_reg[a].start >= avail_reg[a].end) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } അല്ലെങ്കിൽ എങ്കിൽ (reserved[r].end <= avail_reg[a].start) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_to_paddr_reg(reserved[r]));
            r++;
        } അല്ലെങ്കിൽ എങ്കിൽ (reserved[r].start >= avail_reg[a].end) {
            /* the reserved region is above the available region - take the whole thing */
            തിരുകുക_region(avail_reg[a]);
            a++;
        } അല്ലെങ്കിൽ {
            /* the reserved region overlaps with the available region */
            എങ്കിൽ (reserved[r].start <= avail_reg[a].start) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].start = MIN(avail_reg[a].end, reserved[r].end);
                /* do not increment reserved index here - there could be more overlapping regions */
            } അല്ലെങ്കിൽ {
                പ്രതിജ്ഞാ(reserved[r].start < avail_reg[a].end);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.end = reserved[r].start;
                തിരുകുക_region(m);
                എങ്കിൽ (avail_reg[a].end > reserved[r].end) {
                    avail_reg[a].start = reserved[r].end;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } അല്ലെങ്കിൽ {
                    a++;
                }
            }
        }
    }

    വേണ്ടി (; r < n_reserved; r++) {
        എങ്കിൽ (reserved[r].start < reserved[r].end) {
            reserve_region(pptr_to_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    വേണ്ടി (; a < n_available; a++) {
        എങ്കിൽ (avail_reg[a].start < avail_reg[a].end) {
            തിരുകുക_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    പൂർണ്ണസംഖ്യ i = ARRAY_SIZE(ndks_boot.freemem) - 1;
    എങ്കിൽ (!is_reg_empty(ndks_boot.freemem[i])) {
        printf("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (unsigned പൂർണ്ണസംഖ്യ)MAX_അക്കം_FREEMEM_REG);
        തിരികെ false;
    }
    /* skip any empty regions */
    വേണ്ടി (; i >= 0 && is_reg_empty(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    word_t size = calculate_rootserver_size(it_v_reg, extra_bi_size_bits);
    word_t max = rootserver_max_size_bits(extra_bi_size_bits);
    വേണ്ടി (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        പ്രതിജ്ഞാ(i < ARRAY_SIZE(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        പ്രതിജ്ഞാ(is_reg_empty(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        പൂർണ്ണസംഖ്യ empty_index = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_start = ndks_boot.freemem[i].end - size;
        pptr_t start = ROUND_DOWN(unaligned_start, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        എങ്കിൽ (unaligned_start <= ndks_boot.freemem[i].end
            && start >= ndks_boot.freemem[i].start) {
            create_rootserver_objects(start, it_v_reg, extra_bi_size_bits);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[empty_index] = (region_t) {
                .start = start + size,
                .end = ndks_boot.freemem[i].end
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].end = start;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            തിരികെ true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[empty_index] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_EMPTY;
    }

    /* We didn't find a big enough region. */
    printf("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_word"\n", max);
    തിരികെ false;
}
