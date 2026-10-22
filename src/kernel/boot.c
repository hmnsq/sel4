/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <assert.h>
#ضمن <kernel/boot.h>
#ضمن <kernel/thread.h>
#ضمن <machine/io.h>
#ضمن <machine/registerset.h>
#ضمن <model/statedata.h>
#ضمن <object/domain.h>
#ضمن <arch/machine.h>
#ضمن <arch/kernel/boot.h>
#ضمن <arch/kernel/vspace.h>
#ضمن <linker.h>
#ضمن <hardware.h>
#ضمن <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS ساكن region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
خارجي حرف ki_boot_پای_ټکی[1];
BOOT_کوډ p_region_t get_p_reg_kernel_img_boot(فراغ)
{
    أرجع (p_region_t) {
        .پیل = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_بنسټ),
        .پای_ټکی   = kpptr_منزل_paddr(ki_boot_پای_ټکی)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_کوډ p_region_t get_p_reg_kernel_img(فراغ)
{
    أرجع (p_region_t) {
        .پیل = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_بنسټ),
        .پای_ټکی   = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_سر)
    };
}

BOOT_کوډ ساكن فراغ یوځای_regions(فراغ)
{
    /* Walk through reserved regions and see if any can be merged */
    کلمه_t سر_2 = 0;
    لكل (کلمه_t i = 1; i < ndks_boot.resv_شمېر; i++) {
        إذا (ndks_boot.reserved[سر_2].پای_ټکی == ndks_boot.reserved[i].پیل) {
            /* extend earlier region */
            ndks_boot.reserved[سر_2].پای_ټکی = ndks_boot.reserved[i].پای_ټکی;
        } وإلا {
            /* add region */
            سر_2++;
            ndks_boot.reserved[سر_2] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_شمېر = ndks_boot.resv_شمېر > 0 ? سر_2 + 1 : 0;
}

BOOT_کوډ bool_t reserve_region(p_region_t reg)
{
    کلمه_t i;
    أكد(reg.پیل <= reg.پای_ټکی);
    إذا (reg.پیل == reg.پای_ټکی) {
        أرجع true;
    }

    /* keep the regions in order */
    لكل (i = 0; i < ndks_boot.resv_شمېر; i++) {
        /* Try and merge the region to an existing one, if possible */
        إذا (ndks_boot.reserved[i].پیل == reg.پای_ټکی) {
            ndks_boot.reserved[i].پیل = reg.پیل;
            یوځای_regions();
            أرجع true;
        }
        إذا (ndks_boot.reserved[i].پای_ټکی == reg.پیل) {
            ndks_boot.reserved[i].پای_ټکی = reg.پای_ټکی;
            یوځای_regions();
            أرجع true;
        }
        /* Otherwise figure out where it should go. */
        إذا (ndks_boot.reserved[i].پیل > reg.پای_ټکی) {
            /* move regions down, making sure there's enough room */
            إذا (ndks_boot.resv_شمېر + 1 >= MAX_عدد_RESV_REG) {
                چاپ_کړه("Can't mark region 0x%"SEL4_PRIx_کلمه"-0x%"SEL4_PRIx_کلمه
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.پیل, reg.پای_ټکی, (صحيح)MAX_عدد_RESV_REG);
                أرجع false;
            }
            لكل (کلمه_t j = ndks_boot.resv_شمېر; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_شمېر++;
            أرجع true;
        }
    }

    إذا (i + 1 == MAX_عدد_RESV_REG) {
        چاپ_کړه("Can't mark region 0x%"SEL4_PRIx_کلمه"-0x%"SEL4_PRIx_کلمه
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.پیل, reg.پای_ټکی, (صحيح)MAX_عدد_RESV_REG);
        أرجع false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_شمېر++;

    أرجع true;
}

BOOT_کوډ ساكن bool_t ورننباسه_region(region_t reg)
{
    أكد(reg.پیل <= reg.پای_ټکی);
    إذا (is_reg_تشه(reg)) {
        أرجع true;
    }

    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        إذا (is_reg_تشه(ndks_boot.freemem[i])) {
            reserve_region(pptr_منزل_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            أرجع true;
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
    چاپ_کړه("no free memory slot left for [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.پیل, reg.پای_ټکی, (بینښه صحيح)MAX_عدد_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    أكد(0);

    أرجع false;
}

BOOT_کوډ ساكن pptr_t alloc_rootserver_obj(کلمه_t اندازه_بټونه, کلمه_t n)
{
    pptr_t allocated = rootserver_mem.پیل;
    /* allocated memory must be aligned */
    أكد(allocated % بټ(اندازه_بټونه) == 0);
    rootserver_mem.پیل += (n * بټ(اندازه_بټونه));
    /* we must not have run out of memory */
    أكد(rootserver_mem.پیل <= rootserver_mem.پای_ټکی);
    memzero((فراغ *) allocated, n * بټ(اندازه_بټونه));
    أرجع allocated;
}

BOOT_کوډ ساكن کلمه_t rootserver_max_اندازه_بټونه(کلمه_t extra_bi_اندازه_بټونه)
{
    کلمه_t cnode_اندازه_بټونه = CONFIG_ریښه_CNODE_اندازه_بټونه + se_L4_خونه_بټونه;
    کلمه_t max = MAX(cnode_اندازه_بټونه, se_L4_VSpace_بټونه);
    أرجع MAX(max, extra_bi_اندازه_بټونه);
}

BOOT_کوډ ساكن کلمه_t calculate_rootserver_اندازه(v_region_t it_v_reg, کلمه_t extra_bi_اندازه_بټونه)
{
    /* work out how much memory we need for root server objects */
    کلمه_t اندازه = بټ(CONFIG_ریښه_CNODE_اندازه_بټونه + se_L4_خونه_بټونه);
    اندازه += بټ(seL4_TCBBits); // root thread tcb
    اندازه += بټ(se_L4_پاڼه_بټونه); // ipc buf
    اندازه += بټ(se_L4_Boot_Info_چوکاټ_بټونه); // boot info
    اندازه += بټ(se_L4_ASIDPool_بټونه);
    اندازه += extra_bi_اندازه_بټونه > 0 ? بټ(extra_bi_اندازه_بټونه) : 0;
    اندازه += بټ(se_L4_VSpace_بټونه); // root vspace
#که_تعریف_شوی CONFIG_KERNEL_MCS
    اندازه += بټ(se_L4_Min_Sched_Context_بټونه); // root sched context
#د_که_پای
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    أرجع اندازه + arch_get_n_paging(it_v_reg) * بټ(se_L4_پاڼه_جدول_بټونه);
}

BOOT_کوډ ساكن فراغ maybe_alloc_extra_bi(کلمه_t cmp_اندازه_بټونه, کلمه_t extra_bi_اندازه_بټونه)
{
    إذا (extra_bi_اندازه_بټونه >= cmp_اندازه_بټونه && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_اندازه_بټونه, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_کوډ ساكن فراغ create_rootserver_objects(pptr_t پیل, v_region_t it_v_reg,
                                                کلمه_t extra_bi_اندازه_بټونه)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    کلمه_t cnode_اندازه_بټونه = CONFIG_ریښه_CNODE_اندازه_بټونه + se_L4_خونه_بټونه;
    کلمه_t max = rootserver_max_اندازه_بټونه(extra_bi_اندازه_بټونه);

    کلمه_t اندازه = calculate_rootserver_اندازه(it_v_reg, extra_bi_اندازه_بټونه);
    rootserver_mem.پیل = پیل;
    rootserver_mem.پای_ټکی = پیل + اندازه;

    maybe_alloc_extra_bi(max, extra_bi_اندازه_بټونه);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#إذا (CONFIG_ریښه_CNODE_اندازه_بټونه + se_L4_خونه_بټونه) > se_L4_VSpace_بټونه
    rootserver.cnode = alloc_rootserver_obj(cnode_اندازه_بټونه, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_بټونه, extra_bi_اندازه_بټونه);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بټونه, 1);
#وإلا
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بټونه, 1);
    maybe_alloc_extra_bi(cnode_اندازه_بټونه, extra_bi_اندازه_بټونه);
    rootserver.cnode = alloc_rootserver_obj(cnode_اندازه_بټونه, 1);
#د_که_پای

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_پاڼه_بټونه, extra_bi_اندازه_بټونه);
    compile_assert(invalid_se_L4_ASIDPool_بټونه, se_L4_ASIDPool_بټونه == se_L4_پاڼه_بټونه);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_بټونه, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_پاڼه_بټونه, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_چوکاټ_بټونه, se_L4_Boot_Info_چوکاټ_بټونه == se_L4_پاڼه_بټونه);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_چوکاټ_بټونه, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#إذا seL4_TCBBits >= se_L4_پاڼه_جدول_بټونه
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#د_که_پای

    /* paging structures are 4k on every arch except aarch32 (1k) */
    کلمه_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.پیل = alloc_rootserver_obj(se_L4_پاڼه_جدول_بټونه, n);
    rootserver.paging.پای_ټکی = rootserver.paging.پیل + n * بټ(se_L4_پاڼه_جدول_بټونه);

    /* for most archs, TCBs are smaller than page tables */
#إذا seL4_TCBBits < se_L4_پاڼه_جدول_بټونه
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#د_که_پای

#که_تعریف_شوی CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_بټونه, 1);
#د_که_پای
    /* we should have allocated all our memory */
    أكد(rootserver_mem.پیل == rootserver_mem.پای_ټکی);
}

BOOT_کوډ فراغ write_خونه(خونه_ptr_t خونه_ptr, cap_t cap)
{
    خونه_ptr->cap = cap;

    خونه_ptr->cteMDBNode = nullMDBNode;
    mdb_غوټه_ptr_سېټ_mdb_Revocable(&خونه_ptr->cteMDBNode, true);
    mdb_غوټه_ptr_سېټ_mdb_لومړی_Badged(&خونه_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(ریښه_cnode_اندازه_valid,
               CONFIG_ریښه_CNODE_اندازه_بټونه < 32 - se_L4_خونه_بټونه &&
               بټ(CONFIG_ریښه_CNODE_اندازه_بټونه) >= se_L4_عدد_Initial_Caps &&
               CONFIG_ریښه_CNODE_اندازه_بټونه >= (se_L4_پاڼه_بټونه - se_L4_خونه_بټونه))

BOOT_کوډ cap_t
create_ریښه_cnode(فراغ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_ریښه_CNODE_اندازه_بټونه, /* radix */
                    کلمه_بټونه - CONFIG_ریښه_CNODE_اندازه_بټونه, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_خونه(خونه_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    أرجع cap;
}

/* Check domain scheduler assumptions. */
compile_assert(عدد_domains_valid,
               CONFIG_عدد_DOMAINS >= 1 && CONFIG_عدد_DOMAINS <= 256)
compile_assert(عدد_priorities_valid,
               CONFIG_عدد_PRIORITIES >= 1 && CONFIG_عدد_PRIORITIES <= 256)

BOOT_کوډ فراغ
create_domain_cap(cap_t ریښه_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapDomain), cap);
}

BOOT_کوډ cap_t create_ipcbuf_چوکاټ_cap(cap_t ریښه_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_حافظه((فراغ *)rootserver.ipc_buf, پاڼه_بټونه);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکاټ_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    أرجع cap;
}

BOOT_کوډ فراغ create_bi_چوکاټ_cap(cap_t ریښه_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکاټ_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), se_L4_Cap_Boot_Info_چوکاټ), cap);
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
BOOT_کوډ کلمه_t calculate_extra_bi_اندازه_بټونه(کلمه_t extra_اندازه)
{
    إذا (extra_اندازه == 0) {
        أرجع 0;
    }

    کلمه_t clzl_ret = clzl(پړاو_پورته(extra_اندازه, se_L4_پاڼه_بټونه));
    کلمه_t msb = se_L4_کلمه_بټونه - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    إذا (extra_اندازه > بټ(msb)) {
        msb++;
    }
    أرجع msb;
}

BOOT_کوډ فراغ populate_bi_چوکاټ(غوټه_id_t غوټه_id, کلمه_t عدد_nodes,
                                 vptr_t ipcbuf_vptr, کلمه_t extra_bi_اندازه)
{
    /* clear boot info memory */
    clear_حافظه((فراغ *)rootserver.boot_info, se_L4_Boot_Info_چوکاټ_بټونه);
    إذا (extra_bi_اندازه) {
        clear_حافظه((فراغ *)rootserver.extra_bi,
                    calculate_extra_bi_اندازه_بټونه(extra_bi_اندازه));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->غوټه_ID = غوټه_id;
    bi->عدد_Nodes = عدد_nodes;
    bi->عدد_IOPTLevels = 0;
    bi->ipc_بفر = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_اندازه_بټونه = CONFIG_ریښه_CNODE_اندازه_بټونه;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_اندازه;

    ndks_boot.bi_چوکاټ = bi;
    ndks_boot.خونه_pos_cur = se_L4_عدد_Initial_Caps;
}

BOOT_کوډ bool_t provide_cap(cap_t ریښه_cnode_cap, cap_t cap)
{
    إذا (ndks_boot.خونه_pos_cur >= بټ(CONFIG_ریښه_CNODE_اندازه_بټونه)) {
        چاپ_کړه("ERROR: can't add another cap, all %"SEL4_PRIu_کلمه
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               بټ(CONFIG_ریښه_CNODE_اندازه_بټونه));
        أرجع false;
    }
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), ndks_boot.خونه_pos_cur), cap);
    ndks_boot.خونه_pos_cur++;
    أرجع true;
}

BOOT_کوډ create_چوکاټونه_د_region_ret_t create_چوکاټونه_د_region(
    cap_t    ریښه_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_دننه_واټن
)
{
    pptr_t     f;
    cap_t      چوکاټ_cap;
    se_L4_خونه_Pos خونه_pos_before;
    se_L4_خونه_Pos خونه_pos_after;

    خونه_pos_before = ndks_boot.خونه_pos_cur;

    لكل (f = reg.پیل; f < reg.پای_ټکی; f += بټ(پاڼه_بټونه)) {
        إذا (do_map) {
            چوکاټ_cap = create_mapped_it_چوکاټ_cap(pd_cap, f, pptr_منزل_paddr((فراغ *)(f - pv_دننه_واټن)), IT_ASID, false, true);
        } وإلا {
            چوکاټ_cap = create_unmapped_it_چوکاټ_cap(f, false);
        }
        إذا (!provide_cap(ریښه_cnode_cap, چوکاټ_cap)) {
            أرجع (create_چوکاټونه_د_region_ret_t) {
                .region  = S_REG_تشه,
                .success = false
            };
        }
    }

    خونه_pos_after = ndks_boot.خونه_pos_cur;

    أرجع (create_چوکاټونه_د_region_ret_t) {
        .region = (se_L4_خونه_Region) {
            .پیل = خونه_pos_before,
            .پای_ټکی   = خونه_pos_after
        },
        .success = true
    };
}

BOOT_کوډ cap_t create_it_asid_pool(cap_t ریښه_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_لوړ(IT_ASID) << asid_ټیټ_بټونه, rootserver.asid_pool);
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_خونه(
        خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    أرجع ap_cap;
}

#که_تعریف_شوی CONFIG_KERNEL_MCS
BOOT_کوډ ساكن فراغ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_کوډ bool_t init_sched_control(cap_t ریښه_cnode_cap, کلمه_t عدد_nodes)
{
    se_L4_خونه_Pos خونه_pos_before = ndks_boot.خونه_pos_cur;

    /* create a sched control cap for each core */
    لكل (بینښه صحيح i = 0; i < عدد_nodes; i++) {
        إذا (!provide_cap(ریښه_cnode_cap, cap_sched_control_cap_new(i))) {
            چاپ_کړه("can't init sched_control for node %u, provide_cap() failed\n", i);
            أرجع false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_چوکاټ->schedcontrol = (se_L4_خونه_Region) {
        .پیل = خونه_pos_before,
        .پای_ټکی = ndks_boot.خونه_pos_cur
    };

    أرجع true;
}
#د_که_پای

BOOT_کوډ فراغ create_idle_thread(فراغ)
{
    pptr_t pptr;

#که_تعریف_شوی ENABLE_SMP_SUPPORT
    لكل (بینښه صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
#د_که_پای /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        غوټه_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_دننه_واټن);
        configureIdleThread(غوټه_STATE_ON_CORE(ksIdleThread, i));
#که_تعریف_شوی CONFIG_DEBUG_BUILD
        سېټ_Thread_Name(غوټه_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#د_که_پای
        SMP_COND_STATEMENT(غوټه_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#که_تعریف_شوی CONFIG_KERNEL_MCS
        configure_sched_context(غوټه_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(غوټه_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        غوټه_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#د_که_پای
#که_تعریف_شوی ENABLE_SMP_SUPPORT
    }
#د_که_پای /* ENABLE_SMP_SUPPORT */
}

BOOT_کوډ tcb_t *create_initial_thread(cap_t ریښه_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_ثبت, vptr_t bi_چوکاټ_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_دننه_واټن);
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    tcb->tcb_وخت_ټوټه = CONFIG_وخت_ټوټه;
#د_که_پای

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        چاپ_کړه("Failed to derive copy of IPC Buffer\n");
        أرجع عدم;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_ورننباسه(
        ریښه_cnode_cap,
        خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadCNode),
        خونه_PTR(rootserver.tcb, tcbCTable)
    );
    cte_ورننباسه(
        it_pd_cap,
        خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadVSpace),
        خونه_PTR(rootserver.tcb, tcbVTable)
    );
    cte_ورننباسه(
        dc_ret.cap,
        خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadIPCBuffer),
        خونه_PTR(rootserver.tcb, tcb_بفر)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    سېټ_ثبتوونکی(tcb, cap_ثبتوونکی, bi_چوکاټ_vptr);
    سېټ_بل_PC(tcb, ui_v_ثبت);

    /* initialise TCB */
#که_تعریف_شوی CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_منزل_Ticks(CONFIG_BOOT_THREAD_وخت_ټوټه * US_دننه_MS));
#د_که_پای

    tcb->tcb_لومړیتوب = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    تیاری_ځواب_Master(tcb);
#د_که_پای
    سېټ_Thread_State(tcb, ThreadState_Running);

#که_نه_تعریف_شوی CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#د_که_پای

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadTCB), cap);

#که_تعریف_شوی CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_بټونه);
    write_خونه(خونه_PTR(pptr_د_cap(ریښه_cnode_cap), seL4_CapInitThreadSC), cap);
#د_که_پای
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    سېټ_Thread_Name(tcb, "rootserver");
#د_که_پای

    أرجع tcb;
}

#که_تعریف_شوی ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_کوډ ساكن bool_t hypervisor_present(فراغ)
{
#که_تعریف_شوی CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    إذا ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        أرجع true;
    }
#د_که_پای
    أرجع false;
}

BOOT_کوډ فراغ clock_sync_test(فراغ)
{
    ticks_t t, t0;
    ticks_t margin = us_منزل_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    أكد(getCurrentCPUIndex() != 0);
    t = غوټه_STATE_ON_CORE(ks_Cur_وخت, 0);
    افعل {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = غوټه_STATE_ON_CORE(ks_Cur_وخت, 0);
    } طالما (t0 == t);
    t = get_Current_وخت();
    چاپ_کړه("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (صحيح)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    إذا (hypervisor_present()) {
        چاپ_کړه("clock_sync_test[%d]: disabled, detected running as VM\n", (صحيح)getCurrentCPUIndex());
    } وإلا {
        أكد(t0 <= margin + t && t <= t0 + margin);
    }
}
#د_که_پای

BOOT_کوډ فراغ init_core_state(tcb_t *scheduler_action)
{
#که_تعریف_شوی CONFIG_HAVE_FPU
    غوټه_STATE(ksCurFPUOwner) = عدم;
#د_که_پای
#که_تعریف_شوی CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    غوټه_STATE(ksDebugTCBs) = عدم;
    إذا (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_ټاکنه_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(غوټه_STATE(ksIdleThread));
#د_که_پای
    غوټه_STATE(ksSchedulerAction) = scheduler_action;
    غوټه_STATE(ksCurThread) = غوټه_STATE(ksIdleThread);
#که_تعریف_شوی CONFIG_KERNEL_MCS
    غوټه_STATE(ksCurSC) = غوټه_STATE(ksCurThread->tcbSchedContext);
    غوټه_STATE(ksConsumed) = 0;
    غوټه_STATE(ksReprogram) = true;
    غوټه_STATE(ks_Release_کتار.سر) = عدم;
    غوټه_STATE(ks_Release_کتار.پای_ټکی) = عدم;
    غوټه_STATE(ks_Cur_وخت) = get_Current_وخت();
#د_که_پای
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_وخت = DSCHED_MAX_DURATION;
    ks_Dom_مهالوېش[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_کوډ ساكن bool_t pptr_دننه_kernel_کړکۍ(pptr_t pptr)
{
    أرجع pptr >= PPTR_بنسټ && pptr < PPTR_سر;
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
BOOT_کوډ ساكن bool_t provide_untyped_cap(
    cap_t      ریښه_cnode_cap,
    bool_t     device_حافظه_2,
    pptr_t     pptr,
    کلمه_t     اندازه_بټونه,
    se_L4_خونه_Pos لومړی_untyped_خونه
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    إذا (اندازه_بټونه > se_L4_Max_Untyped_بټونه || اندازه_بټونه < se_L4_Min_Untyped_بټونه) {
        چاپ_کړه("Kernel init: Invalid untyped size %"SEL4_PRIu_کلمه"\n", اندازه_بټونه);
        أرجع false;
    }

    /* All cap ptrs must be aligned to object size */
    إذا (!IS_ALIGNED(pptr, اندازه_بټونه)) {
        چاپ_کړه("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_کلمه")\n", (فراغ *)pptr, اندازه_بټونه);
        أرجع false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    إذا (!device_حافظه_2 && !pptr_دننه_kernel_کړکۍ(pptr)) {
        چاپ_کړه("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (فراغ *)pptr);
        أرجع false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    إذا (!device_حافظه_2 && !pptr_دننه_kernel_کړکۍ(pptr + ماسک(اندازه_بټونه))) {
        چاپ_کړه("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_کلمه")\n",
               (فراغ *)pptr, اندازه_بټونه);
        أرجع false;
    }

    کلمه_t i = ndks_boot.خونه_pos_cur - لومړی_untyped_خونه;
    إذا (i < CONFIG_MAX_عدد_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_چوکاټ->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_منزل_paddr((فراغ *)pptr),
            .اندازه_بټونه_2 = اندازه_بټونه,
            .isDevice = device_حافظه_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_شاخص(اندازه_بټونه),
                                     device_حافظه_2, اندازه_بټونه, pptr);
        ret = provide_cap(ریښه_cnode_cap, ut_cap);
    } وإلا {
        چاپ_کړه("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    أرجع ret;
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
BOOT_کوډ ساكن bool_t create_untypeds_for_region(
    cap_t      ریښه_cnode_cap,
    bool_t     device_حافظه_2,
    region_t   reg,
    se_L4_خونه_Pos لومړی_untyped_خونه
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    طالما (!is_reg_تشه(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        بینښه صحيح اندازه_بټونه = se_L4_کلمه_بټونه - 1 - clzl(reg.پای_ټکی - reg.پیل);
        /* The size can't exceed the largest possible untyped size. */
        إذا (اندازه_بټونه > se_L4_Max_Untyped_بټونه) {
            اندازه_بټونه = se_L4_Max_Untyped_بټونه;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        إذا (0 != reg.پیل) {
            بینښه صحيح align_بټونه = ctzl(reg.پیل);
            إذا (اندازه_بټونه > align_بټونه) {
                اندازه_بټونه = align_بټونه;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        إذا (اندازه_بټونه >= se_L4_Min_Untyped_بټونه) {
            إذا (!provide_untyped_cap(ریښه_cnode_cap, device_حافظه_2, reg.پیل, اندازه_بټونه, لومړی_untyped_خونه)) {
                أرجع false;
            }
        }
        reg.پیل += بټ(اندازه_بټونه);
    }
    أرجع true;
}

BOOT_کوډ bool_t create_untypeds(cap_t ریښه_cnode_cap)
{
    se_L4_خونه_Pos لومړی_untyped_خونه = ndks_boot.خونه_pos_cur;

    paddr_t پیل = 0;
    لكل (کلمه_t i = 0; i < ndks_boot.resv_شمېر; i++) {
        إذا (پیل < ndks_boot.reserved[i].پیل) {
            region_t reg = paddr_منزل_pptr_reg((p_region_t) {
                پیل, ndks_boot.reserved[i].پیل
            });
            إذا (!create_untypeds_for_region(ریښه_cnode_cap, true, reg, لومړی_untyped_خونه)) {
                چاپ_کړه("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                       (بینښه صحيح)i, reg.پیل, reg.پای_ټکی);
                أرجع false;
            }
        }

        پیل = ndks_boot.reserved[i].پای_ټکی;
    }

    إذا (پیل <= CONFIG_PADDR_USER_DEVICE_سر - 1) {
        region_t reg = paddr_منزل_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            پیل, (paddr_t) CONFIG_PADDR_USER_DEVICE_سر
        });

        إذا (!create_untypeds_for_region(ریښه_cnode_cap, true, reg, لومړی_untyped_خونه)) {
            چاپ_کړه("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                   reg.پیل, reg.پای_ټکی);
            أرجع false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_منزل_pptr_reg(get_p_reg_kernel_img_boot());
    إذا (!create_untypeds_for_region(ریښه_cnode_cap, false, boot_mem_reuse_reg, لومړی_untyped_خونه)) {
        چاپ_کړه("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"] failed\n",
               boot_mem_reuse_reg.پیل, boot_mem_reuse_reg.پای_ټکی);
        أرجع false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_تشه;
        إذا (!create_untypeds_for_region(ریښه_cnode_cap, false, reg, لومړی_untyped_خونه)) {
            چاپ_کړه("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") failed\n",
                   (بینښه صحيح)i, reg.پیل, reg.پای_ټکی);
            أرجع false;
        }
    }

    ndks_boot.bi_چوکاټ->untyped = (se_L4_خونه_Region) {
        .پیل = لومړی_untyped_خونه,
        .پای_ټکی   = ndks_boot.خونه_pos_cur
    };

    أرجع true;
}

BOOT_کوډ فراغ bi_finalise(فراغ)
{

    إذا (rootserver.paging.پیل != rootserver.paging.پای_ټکی) {
        چاپ_کړه("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.پای_ټکی - rootserver.paging.پیل) >> se_L4_پاڼه_جدول_بټونه);
    }

    ndks_boot.bi_چوکاټ->تشه = (se_L4_خونه_Region) {
        .پیل = ndks_boot.خونه_pos_cur,
        .پای_ټکی   = بټ(CONFIG_ریښه_CNODE_اندازه_بټونه)
    };
}

BOOT_کوډ ساكن bool_t check_شته_حافظه(کلمه_t n_شته,
                                               ثابت p_region_t *شته)
{
    /* The system configuration is broken if no region is available. */
    إذا (0 == n_شته) {
        چاپ_کړه("ERROR: no memory regions available\n");
        أرجع false;
    }

    چاپ_کړه("Available phys memory regions: %"SEL4_PRIu_کلمه"\n", n_شته);
    /* Force ordering and exclusivity of available regions. */
    لكل (کلمه_t i = 0; i < n_شته; i++) {
        ثابت p_region_t *r = &شته[i];
        چاپ_کړه("  [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")\n", r->پیل, r->پای_ټکی);

        /* Available regions must not wrap */
        إذا (r->پیل > r->پای_ټکی) {
            چاپ_کړه("ERROR: memory region %"SEL4_PRIu_کلمه" has start > end\n", i);
            أرجع false;
        }

        /* Available regions must be non-empty. */
        إذا (r->پیل == r->پای_ټکی) {
            چاپ_کړه("ERROR: memory region %"SEL4_PRIu_کلمه" empty\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        إذا ((i > 0) && (r->پیل < شته[i - 1].پای_ټکی)) {
            چاپ_کړه("ERROR: memory region %"SEL4_PRIu_کلمه" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ساكن region_t avail_reg[MAX_عدد_FREEMEM_REG];

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
BOOT_کوډ ساكن کلمه_t init_avail_reg(کلمه_t n_شته,
                                       ثابت p_region_t *شته)
{
    کلمه_t cnt = 0;
    لكل (کلمه_t i = 0; i < n_شته; i++) {
        ثابت p_region_t *r = &شته[i];

        إذا ((r->پیل >= PADDR_سر) || (r->پای_ټکی <= PADDR_بنسټ)) {
            چاپ_کړه("  region [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->پیل, r->پای_ټکی);
        } وإلا {
            p_region_t usable_reg = {
                .پیل = MAX(r->پیل, PADDR_بنسټ),
                .پای_ټکی   = MIN(r->پای_ټکی, PADDR_سر)
            };

            إذا (cnt >= ARRAY_اندازه(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                چاپ_کړه("  WARNING: cannot use region [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه"),"
                       " avail_reg[] is full\n", r->پیل, r->پای_ټکی);
                أكد(0);
            } وإلا {
                avail_reg[cnt] = paddr_منزل_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    أرجع cnt;
}


BOOT_کوډ ساكن bool_t check_reserved_حافظه(کلمه_t n_reserved,
                                              ثابت region_t *reserved)
{
    چاپ_کړه("Reserved virt address space regions: %"SEL4_PRIu_کلمه"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    لكل (کلمه_t i = 0; i < n_reserved; i++) {
        ثابت region_t *r = &reserved[i];
        چاپ_کړه("  [%"SEL4_PRIx_کلمه"..%"SEL4_PRIx_کلمه")\n", r->پیل, r->پای_ټکی);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        إذا (r->پیل > r->پای_ټکی) {
            چاپ_کړه("ERROR: reserved region %"SEL4_PRIu_کلمه" has start > end\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        إذا ((i > 0) && (r->پیل < reserved[i - 1].پای_ټکی)) {
            چاپ_کړه("ERROR: reserved region %"SEL4_PRIu_کلمه" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_کوډ bool_t init_freemem(کلمه_t n_شته, ثابت p_region_t *شته,
                              کلمه_t n_reserved, ثابت region_t *reserved,
                              v_region_t it_v_reg, کلمه_t extra_bi_اندازه_بټونه)
{
    إذا (!check_شته_حافظه(n_شته, شته)) {
        أرجع false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_شته = init_avail_reg(n_شته, شته);
    إذا (0 == n_شته) {
        چاپ_کړه("ERROR: no available memory within the kernel window\n");
        أرجع false;
    }

    إذا (!check_reserved_حافظه(n_reserved, reserved)) {
        أرجع false;
    }

    لكل (کلمه_t i = 0; i < ARRAY_اندازه(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_تشه;
    }

    کلمه_t a = 0;
    کلمه_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    طالما (a < n_شته && r < n_reserved) {
        إذا (reserved[r].پیل == reserved[r].پای_ټکی) {
            /* reserved region is empty - skip it */
            r++;
        } وإلا إذا (avail_reg[a].پیل >= avail_reg[a].پای_ټکی) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } وإلا إذا (reserved[r].پای_ټکی <= avail_reg[a].پیل) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
            r++;
        } وإلا إذا (reserved[r].پیل >= avail_reg[a].پای_ټکی) {
            /* the reserved region is above the available region - take the whole thing */
            ورننباسه_region(avail_reg[a]);
            a++;
        } وإلا {
            /* the reserved region overlaps with the available region */
            إذا (reserved[r].پیل <= avail_reg[a].پیل) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].پیل = MIN(avail_reg[a].پای_ټکی, reserved[r].پای_ټکی);
                /* do not increment reserved index here - there could be more overlapping regions */
            } وإلا {
                أكد(reserved[r].پیل < avail_reg[a].پای_ټکی);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.پای_ټکی = reserved[r].پیل;
                ورننباسه_region(m);
                إذا (avail_reg[a].پای_ټکی > reserved[r].پای_ټکی) {
                    avail_reg[a].پیل = reserved[r].پای_ټکی;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } وإلا {
                    a++;
                }
            }
        }
    }

    لكل (; r < n_reserved; r++) {
        إذا (reserved[r].پیل < reserved[r].پای_ټکی) {
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    لكل (; a < n_شته; a++) {
        إذا (avail_reg[a].پیل < avail_reg[a].پای_ټکی) {
            ورننباسه_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    صحيح i = ARRAY_اندازه(ndks_boot.freemem) - 1;
    إذا (!is_reg_تشه(ndks_boot.freemem[i])) {
        چاپ_کړه("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (بینښه صحيح)MAX_عدد_FREEMEM_REG);
        أرجع false;
    }
    /* skip any empty regions */
    لكل (; i >= 0 && is_reg_تشه(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    کلمه_t اندازه = calculate_rootserver_اندازه(it_v_reg, extra_bi_اندازه_بټونه);
    کلمه_t max = rootserver_max_اندازه_بټونه(extra_bi_اندازه_بټونه);
    لكل (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        أكد(i < ARRAY_اندازه(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        أكد(is_reg_تشه(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        صحيح تشه_شاخص = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_پیل = ndks_boot.freemem[i].پای_ټکی - اندازه;
        pptr_t پیل = پړاو_ښکته(unaligned_پیل, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        إذا (unaligned_پیل <= ndks_boot.freemem[i].پای_ټکی
            && پیل >= ndks_boot.freemem[i].پیل) {
            create_rootserver_objects(پیل, it_v_reg, extra_bi_اندازه_بټونه);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[تشه_شاخص] = (region_t) {
                .پیل = پیل + اندازه,
                .پای_ټکی = ndks_boot.freemem[i].پای_ټکی
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].پای_ټکی = پیل;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            أرجع true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[تشه_شاخص] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_تشه;
    }

    /* We didn't find a big enough region. */
    چاپ_کړه("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_کلمه"\n", max);
    أرجع false;
}
