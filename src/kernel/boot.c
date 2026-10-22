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
خارجي حرف ki_boot_پڇاڙي[1];
BOOT_ڪوڊ p_region_t get_p_reg_kernel_img_boot(فراغ)
{
    أرجع (p_region_t) {
        .شروعات = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_بنياد),
        .پڇاڙي   = kpptr_منزل_paddr(ki_boot_پڇاڙي)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_ڪوڊ p_region_t get_p_reg_kernel_img(فراغ)
{
    أرجع (p_region_t) {
        .شروعات = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_بنياد),
        .پڇاڙي   = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_چوٽي)
    };
}

BOOT_ڪوڊ ساكن فراغ ملايو_regions(فراغ)
{
    /* Walk through reserved regions and see if any can be merged */
    لفظ_t چوٽي = 0;
    لكل (لفظ_t i = 1; i < ndks_boot.resv_ڳڻپ; i++) {
        إذا (ndks_boot.reserved[چوٽي].پڇاڙي == ndks_boot.reserved[i].شروعات) {
            /* extend earlier region */
            ndks_boot.reserved[چوٽي].پڇاڙي = ndks_boot.reserved[i].پڇاڙي;
        } وإلا {
            /* add region */
            چوٽي++;
            ndks_boot.reserved[چوٽي] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_ڳڻپ = ndks_boot.resv_ڳڻپ > 0 ? چوٽي + 1 : 0;
}

BOOT_ڪوڊ bool_t reserve_region(p_region_t reg)
{
    لفظ_t i;
    أكد(reg.شروعات <= reg.پڇاڙي);
    إذا (reg.شروعات == reg.پڇاڙي) {
        أرجع true;
    }

    /* keep the regions in order */
    لكل (i = 0; i < ndks_boot.resv_ڳڻپ; i++) {
        /* Try and merge the region to an existing one, if possible */
        إذا (ndks_boot.reserved[i].شروعات == reg.پڇاڙي) {
            ndks_boot.reserved[i].شروعات = reg.شروعات;
            ملايو_regions();
            أرجع true;
        }
        إذا (ndks_boot.reserved[i].پڇاڙي == reg.شروعات) {
            ndks_boot.reserved[i].پڇاڙي = reg.پڇاڙي;
            ملايو_regions();
            أرجع true;
        }
        /* Otherwise figure out where it should go. */
        إذا (ndks_boot.reserved[i].شروعات > reg.پڇاڙي) {
            /* move regions down, making sure there's enough room */
            إذا (ndks_boot.resv_ڳڻپ + 1 >= MAX_انگ_RESV_REG) {
                لکو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.شروعات, reg.پڇاڙي, (صحيح)MAX_انگ_RESV_REG);
                أرجع false;
            }
            لكل (لفظ_t j = ndks_boot.resv_ڳڻپ; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_ڳڻپ++;
            أرجع true;
        }
    }

    إذا (i + 1 == MAX_انگ_RESV_REG) {
        لکو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.شروعات, reg.پڇاڙي, (صحيح)MAX_انگ_RESV_REG);
        أرجع false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_ڳڻپ++;

    أرجع true;
}

BOOT_ڪوڊ ساكن bool_t وجهو_region(region_t reg)
{
    أكد(reg.شروعات <= reg.پڇاڙي);
    إذا (is_reg_خالي_جايون(reg)) {
        أرجع true;
    }

    لكل (لفظ_t i = 0; i < ARRAY_سائيز(ndks_boot.freemem); i++) {
        إذا (is_reg_خالي_جايون(ndks_boot.freemem[i])) {
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
    لکو("no free memory slot left for [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.شروعات, reg.پڇاڙي, (بنانشان صحيح)MAX_انگ_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    أكد(0);

    أرجع false;
}

BOOT_ڪوڊ ساكن pptr_t alloc_rootserver_obj(لفظ_t سائيز_بٽ, لفظ_t n)
{
    pptr_t allocated = rootserver_mem.شروعات;
    /* allocated memory must be aligned */
    أكد(allocated % ٻٽو_انگ(سائيز_بٽ) == 0);
    rootserver_mem.شروعات += (n * ٻٽو_انگ(سائيز_بٽ));
    /* we must not have run out of memory */
    أكد(rootserver_mem.شروعات <= rootserver_mem.پڇاڙي);
    memzero((فراغ *) allocated, n * ٻٽو_انگ(سائيز_بٽ));
    أرجع allocated;
}

BOOT_ڪوڊ ساكن لفظ_t rootserver_max_سائيز_بٽ(لفظ_t extra_bi_سائيز_بٽ)
{
    لفظ_t cnode_سائيز_بٽ = CONFIG_پاڙ_CNODE_سائيز_بٽ + se_L4_جڳهه_بٽ;
    لفظ_t max = MAX(cnode_سائيز_بٽ, se_L4_VSpace_بٽ);
    أرجع MAX(max, extra_bi_سائيز_بٽ);
}

BOOT_ڪوڊ ساكن لفظ_t calculate_rootserver_سائيز(v_region_t it_v_reg, لفظ_t extra_bi_سائيز_بٽ)
{
    /* work out how much memory we need for root server objects */
    لفظ_t سائيز = ٻٽو_انگ(CONFIG_پاڙ_CNODE_سائيز_بٽ + se_L4_جڳهه_بٽ);
    سائيز += ٻٽو_انگ(seL4_TCBBits); // root thread tcb
    سائيز += ٻٽو_انگ(se_L4_صفحو_بٽ); // ipc buf
    سائيز += ٻٽو_انگ(se_L4_Boot_Info_چوکٺو_بٽ); // boot info
    سائيز += ٻٽو_انگ(se_L4_ASIDPool_بٽ);
    سائيز += extra_bi_سائيز_بٽ > 0 ? ٻٽو_انگ(extra_bi_سائيز_بٽ) : 0;
    سائيز += ٻٽو_انگ(se_L4_VSpace_بٽ); // root vspace
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    سائيز += ٻٽو_انگ(se_L4_Min_Sched_Context_بٽ); // root sched context
#جيڪڏهن_پورو
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    أرجع سائيز + arch_get_n_paging(it_v_reg) * ٻٽو_انگ(se_L4_صفحو_جدول_بٽ);
}

BOOT_ڪوڊ ساكن فراغ maybe_alloc_extra_bi(لفظ_t cmp_سائيز_بٽ, لفظ_t extra_bi_سائيز_بٽ)
{
    إذا (extra_bi_سائيز_بٽ >= cmp_سائيز_بٽ && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_سائيز_بٽ, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_ڪوڊ ساكن فراغ create_rootserver_objects(pptr_t شروعات, v_region_t it_v_reg,
                                                لفظ_t extra_bi_سائيز_بٽ)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    لفظ_t cnode_سائيز_بٽ = CONFIG_پاڙ_CNODE_سائيز_بٽ + se_L4_جڳهه_بٽ;
    لفظ_t max = rootserver_max_سائيز_بٽ(extra_bi_سائيز_بٽ);

    لفظ_t سائيز = calculate_rootserver_سائيز(it_v_reg, extra_bi_سائيز_بٽ);
    rootserver_mem.شروعات = شروعات;
    rootserver_mem.پڇاڙي = شروعات + سائيز;

    maybe_alloc_extra_bi(max, extra_bi_سائيز_بٽ);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#إذا (CONFIG_پاڙ_CNODE_سائيز_بٽ + se_L4_جڳهه_بٽ) > se_L4_VSpace_بٽ
    rootserver.cnode = alloc_rootserver_obj(cnode_سائيز_بٽ, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_بٽ, extra_bi_سائيز_بٽ);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٽ, 1);
#وإلا
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٽ, 1);
    maybe_alloc_extra_bi(cnode_سائيز_بٽ, extra_bi_سائيز_بٽ);
    rootserver.cnode = alloc_rootserver_obj(cnode_سائيز_بٽ, 1);
#جيڪڏهن_پورو

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_صفحو_بٽ, extra_bi_سائيز_بٽ);
    compile_assert(invalid_se_L4_ASIDPool_بٽ, se_L4_ASIDPool_بٽ == se_L4_صفحو_بٽ);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_بٽ, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_صفحو_بٽ, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_چوکٺو_بٽ, se_L4_Boot_Info_چوکٺو_بٽ == se_L4_صفحو_بٽ);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_چوکٺو_بٽ, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#إذا seL4_TCBBits >= se_L4_صفحو_جدول_بٽ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#جيڪڏهن_پورو

    /* paging structures are 4k on every arch except aarch32 (1k) */
    لفظ_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.شروعات = alloc_rootserver_obj(se_L4_صفحو_جدول_بٽ, n);
    rootserver.paging.پڇاڙي = rootserver.paging.شروعات + n * ٻٽو_انگ(se_L4_صفحو_جدول_بٽ);

    /* for most archs, TCBs are smaller than page tables */
#إذا seL4_TCBBits < se_L4_صفحو_جدول_بٽ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_بٽ, 1);
#جيڪڏهن_پورو
    /* we should have allocated all our memory */
    أكد(rootserver_mem.شروعات == rootserver_mem.پڇاڙي);
}

BOOT_ڪوڊ فراغ write_جڳهه(جڳهه_ptr_t جڳهه_ptr, cap_t cap)
{
    جڳهه_ptr->cap = cap;

    جڳهه_ptr->cteMDBNode = nullMDBNode;
    mdb_ڳنڍ_ptr_سيٽ_mdb_Revocable(&جڳهه_ptr->cteMDBNode, true);
    mdb_ڳنڍ_ptr_سيٽ_mdb_پهريون_Badged(&جڳهه_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(پاڙ_cnode_سائيز_valid,
               CONFIG_پاڙ_CNODE_سائيز_بٽ < 32 - se_L4_جڳهه_بٽ &&
               ٻٽو_انگ(CONFIG_پاڙ_CNODE_سائيز_بٽ) >= se_L4_انگ_Initial_Caps &&
               CONFIG_پاڙ_CNODE_سائيز_بٽ >= (se_L4_صفحو_بٽ - se_L4_جڳهه_بٽ))

BOOT_ڪوڊ cap_t
create_پاڙ_cnode(فراغ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_پاڙ_CNODE_سائيز_بٽ, /* radix */
                    لفظ_بٽ - CONFIG_پاڙ_CNODE_سائيز_بٽ, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_جڳهه(جڳهه_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    أرجع cap;
}

/* Check domain scheduler assumptions. */
compile_assert(انگ_domains_valid,
               CONFIG_انگ_DOMAINS >= 1 && CONFIG_انگ_DOMAINS <= 256)
compile_assert(انگ_priorities_valid,
               CONFIG_انگ_PRIORITIES >= 1 && CONFIG_انگ_PRIORITIES <= 256)

BOOT_ڪوڊ فراغ
create_domain_cap(cap_t پاڙ_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapDomain), cap);
}

BOOT_ڪوڊ cap_t create_ipcbuf_چوکٺو_cap(cap_t پاڙ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_يادگيري((فراغ *)rootserver.ipc_buf, صفحو_بٽ);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکٺو_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    أرجع cap;
}

BOOT_ڪوڊ فراغ create_bi_چوکٺو_cap(cap_t پاڙ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکٺو_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), se_L4_Cap_Boot_Info_چوکٺو), cap);
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
BOOT_ڪوڊ لفظ_t calculate_extra_bi_سائيز_بٽ(لفظ_t extra_سائيز)
{
    إذا (extra_سائيز == 0) {
        أرجع 0;
    }

    لفظ_t clzl_ret = clzl(ڦيرو_مٿي(extra_سائيز, se_L4_صفحو_بٽ));
    لفظ_t msb = se_L4_لفظ_بٽ - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    إذا (extra_سائيز > ٻٽو_انگ(msb)) {
        msb++;
    }
    أرجع msb;
}

BOOT_ڪوڊ فراغ populate_bi_چوکٺو(ڳنڍ_id_t ڳنڍ_id, لفظ_t انگ_nodes,
                                 vptr_t ipcbuf_vptr, لفظ_t extra_bi_سائيز)
{
    /* clear boot info memory */
    clear_يادگيري((فراغ *)rootserver.boot_info, se_L4_Boot_Info_چوکٺو_بٽ);
    إذا (extra_bi_سائيز) {
        clear_يادگيري((فراغ *)rootserver.extra_bi,
                    calculate_extra_bi_سائيز_بٽ(extra_bi_سائيز));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->ڳنڍ_ID = ڳنڍ_id;
    bi->انگ_Nodes = انگ_nodes;
    bi->انگ_IOPTLevels = 0;
    bi->ipc_بفر = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_سائيز_بٽ = CONFIG_پاڙ_CNODE_سائيز_بٽ;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_سائيز;

    ndks_boot.bi_چوکٺو = bi;
    ndks_boot.جڳهه_pos_cur = se_L4_انگ_Initial_Caps;
}

BOOT_ڪوڊ bool_t provide_cap(cap_t پاڙ_cnode_cap, cap_t cap)
{
    إذا (ndks_boot.جڳهه_pos_cur >= ٻٽو_انگ(CONFIG_پاڙ_CNODE_سائيز_بٽ)) {
        لکو("ERROR: can't add another cap, all %"SEL4_PRIu_لفظ
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               ٻٽو_انگ(CONFIG_پاڙ_CNODE_سائيز_بٽ));
        أرجع false;
    }
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), ndks_boot.جڳهه_pos_cur), cap);
    ndks_boot.جڳهه_pos_cur++;
    أرجع true;
}

BOOT_ڪوڊ create_چوکٺا_جو_region_ret_t create_چوکٺا_جو_region(
    cap_t    پاڙ_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_وٿي
)
{
    pptr_t     f;
    cap_t      چوکٺو_cap;
    se_L4_جڳهه_Pos جڳهه_pos_before;
    se_L4_جڳهه_Pos جڳهه_pos_after;

    جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;

    لكل (f = reg.شروعات; f < reg.پڇاڙي; f += ٻٽو_انگ(صفحو_بٽ)) {
        إذا (do_map) {
            چوکٺو_cap = create_mapped_it_چوکٺو_cap(pd_cap, f, pptr_منزل_paddr((فراغ *)(f - pv_وٿي)), IT_ASID, false, true);
        } وإلا {
            چوکٺو_cap = create_unmapped_it_چوکٺو_cap(f, false);
        }
        إذا (!provide_cap(پاڙ_cnode_cap, چوکٺو_cap)) {
            أرجع (create_چوکٺا_جو_region_ret_t) {
                .region  = S_REG_خالي_جايون,
                .success = false
            };
        }
    }

    جڳهه_pos_after = ndks_boot.جڳهه_pos_cur;

    أرجع (create_چوکٺا_جو_region_ret_t) {
        .region = (se_L4_جڳهه_Region) {
            .شروعات = جڳهه_pos_before,
            .پڇاڙي   = جڳهه_pos_after
        },
        .success = true
    };
}

BOOT_ڪوڊ cap_t create_it_asid_pool(cap_t پاڙ_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_مٿاهون(IT_ASID) << asid_هيٺيون_بٽ, rootserver.asid_pool);
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_جڳهه(
        جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    أرجع ap_cap;
}

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
BOOT_ڪوڊ ساكن فراغ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_ڪوڊ bool_t init_sched_control(cap_t پاڙ_cnode_cap, لفظ_t انگ_nodes)
{
    se_L4_جڳهه_Pos جڳهه_pos_before = ndks_boot.جڳهه_pos_cur;

    /* create a sched control cap for each core */
    لكل (بنانشان صحيح i = 0; i < انگ_nodes; i++) {
        إذا (!provide_cap(پاڙ_cnode_cap, cap_sched_control_cap_new(i))) {
            لکو("can't init sched_control for node %u, provide_cap() failed\n", i);
            أرجع false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_چوکٺو->schedcontrol = (se_L4_جڳهه_Region) {
        .شروعات = جڳهه_pos_before,
        .پڇاڙي = ndks_boot.جڳهه_pos_cur
    };

    أرجع true;
}
#جيڪڏهن_پورو

BOOT_ڪوڊ فراغ create_idle_thread(فراغ)
{
    pptr_t pptr;

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    لكل (بنانشان صحيح i = 0; i < CONFIG_MAX_انگ_NODES; i++) {
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        ڳنڍ_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_وٿي);
        configureIdleThread(ڳنڍ_STATE_ON_CORE(ksIdleThread, i));
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
        سيٽ_Thread_Name(ڳنڍ_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#جيڪڏهن_پورو
        SMP_COND_STATEMENT(ڳنڍ_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
        configure_sched_context(ڳنڍ_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(ڳنڍ_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        ڳنڍ_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
    }
#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */
}

BOOT_ڪوڊ tcb_t *create_initial_thread(cap_t پاڙ_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_داخلا, vptr_t bi_چوکٺو_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_وٿي);
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    tcb->tcb_وقت_ٽڪرو = CONFIG_وقت_ٽڪرو;
#جيڪڏهن_پورو

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        لکو("Failed to derive copy of IPC Buffer\n");
        أرجع عدم;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_وجهو(
        پاڙ_cnode_cap,
        جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadCNode),
        جڳهه_PTR(rootserver.tcb, tcbCTable)
    );
    cte_وجهو(
        it_pd_cap,
        جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadVSpace),
        جڳهه_PTR(rootserver.tcb, tcbVTable)
    );
    cte_وجهو(
        dc_ret.cap,
        جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadIPCBuffer),
        جڳهه_PTR(rootserver.tcb, tcb_بفر)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    سيٽ_رجسٽر(tcb, cap_رجسٽر, bi_چوکٺو_vptr);
    سيٽ_ايندڙ_PC(tcb, ui_v_داخلا);

    /* initialise TCB */
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_منزل_Ticks(CONFIG_BOOT_THREAD_وقت_ٽڪرو * US_اندريون_MS));
#جيڪڏهن_پورو

    tcb->tcb_ترجيح = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    تياري_جواب_Master(tcb);
#جيڪڏهن_پورو
    سيٽ_Thread_State(tcb, ThreadState_Running);

#جيڪڏهن_وصف_نه_ٿيل CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#جيڪڏهن_پورو

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadTCB), cap);

#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_بٽ);
    write_جڳهه(جڳهه_PTR(pptr_جو_cap(پاڙ_cnode_cap), seL4_CapInitThreadSC), cap);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    سيٽ_Thread_Name(tcb, "rootserver");
#جيڪڏهن_پورو

    أرجع tcb;
}

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_ڪوڊ ساكن bool_t hypervisor_present(فراغ)
{
#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    إذا ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        أرجع true;
    }
#جيڪڏهن_پورو
    أرجع false;
}

BOOT_ڪوڊ فراغ clock_sync_test(فراغ)
{
    ticks_t t, t0;
    ticks_t margin = us_منزل_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    أكد(getCurrentCPUIndex() != 0);
    t = ڳنڍ_STATE_ON_CORE(ks_Cur_وقت, 0);
    افعل {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = ڳنڍ_STATE_ON_CORE(ks_Cur_وقت, 0);
    } طالما (t0 == t);
    t = get_Current_وقت();
    لکو("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (صحيح)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    إذا (hypervisor_present()) {
        لکو("clock_sync_test[%d]: disabled, detected running as VM\n", (صحيح)getCurrentCPUIndex());
    } وإلا {
        أكد(t0 <= margin + t && t <= t0 + margin);
    }
}
#جيڪڏهن_پورو

BOOT_ڪوڊ فراغ init_core_state(tcb_t *scheduler_action)
{
#جيڪڏهن_وصف_ٿيل CONFIG_HAVE_FPU
    ڳنڍ_STATE(ksCurFPUOwner) = عدم;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    ڳنڍ_STATE(ksDebugTCBs) = عدم;
    إذا (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_چونڊيو_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(ڳنڍ_STATE(ksIdleThread));
#جيڪڏهن_پورو
    ڳنڍ_STATE(ksSchedulerAction) = scheduler_action;
    ڳنڍ_STATE(ksCurThread) = ڳنڍ_STATE(ksIdleThread);
#جيڪڏهن_وصف_ٿيل CONFIG_KERNEL_MCS
    ڳنڍ_STATE(ksCurSC) = ڳنڍ_STATE(ksCurThread->tcbSchedContext);
    ڳنڍ_STATE(ksConsumed) = 0;
    ڳنڍ_STATE(ksReprogram) = true;
    ڳنڍ_STATE(ks_Release_قطار.سر) = عدم;
    ڳنڍ_STATE(ks_Release_قطار.پڇاڙي) = عدم;
    ڳنڍ_STATE(ks_Cur_وقت) = get_Current_وقت();
#جيڪڏهن_پورو
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_وقت = DSCHED_MAX_DURATION;
    ks_Dom_وقت_جدول[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_ڪوڊ ساكن bool_t pptr_اندريون_kernel_دري(pptr_t pptr)
{
    أرجع pptr >= PPTR_بنياد && pptr < PPTR_چوٽي;
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
BOOT_ڪوڊ ساكن bool_t provide_untyped_cap(
    cap_t      پاڙ_cnode_cap,
    bool_t     device_يادگيري_2,
    pptr_t     pptr,
    لفظ_t     سائيز_بٽ,
    se_L4_جڳهه_Pos پهريون_untyped_جڳهه
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    إذا (سائيز_بٽ > se_L4_Max_Untyped_بٽ || سائيز_بٽ < se_L4_Min_Untyped_بٽ) {
        لکو("Kernel init: Invalid untyped size %"SEL4_PRIu_لفظ"\n", سائيز_بٽ);
        أرجع false;
    }

    /* All cap ptrs must be aligned to object size */
    إذا (!IS_ALIGNED(pptr, سائيز_بٽ)) {
        لکو("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_لفظ")\n", (فراغ *)pptr, سائيز_بٽ);
        أرجع false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    إذا (!device_يادگيري_2 && !pptr_اندريون_kernel_دري(pptr)) {
        لکو("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (فراغ *)pptr);
        أرجع false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    إذا (!device_يادگيري_2 && !pptr_اندريون_kernel_دري(pptr + نقاب(سائيز_بٽ))) {
        لکو("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_لفظ")\n",
               (فراغ *)pptr, سائيز_بٽ);
        أرجع false;
    }

    لفظ_t i = ndks_boot.جڳهه_pos_cur - پهريون_untyped_جڳهه;
    إذا (i < CONFIG_MAX_انگ_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_چوکٺو->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_منزل_paddr((فراغ *)pptr),
            .سائيز_بٽ_2 = سائيز_بٽ,
            .isDevice = device_يادگيري_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_اشاريو(سائيز_بٽ),
                                     device_يادگيري_2, سائيز_بٽ, pptr);
        ret = provide_cap(پاڙ_cnode_cap, ut_cap);
    } وإلا {
        لکو("Kernel init: Too many untyped regions for boot info\n");
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
BOOT_ڪوڊ ساكن bool_t create_untypeds_for_region(
    cap_t      پاڙ_cnode_cap,
    bool_t     device_يادگيري_2,
    region_t   reg,
    se_L4_جڳهه_Pos پهريون_untyped_جڳهه
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    طالما (!is_reg_خالي_جايون(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        بنانشان صحيح سائيز_بٽ = se_L4_لفظ_بٽ - 1 - clzl(reg.پڇاڙي - reg.شروعات);
        /* The size can't exceed the largest possible untyped size. */
        إذا (سائيز_بٽ > se_L4_Max_Untyped_بٽ) {
            سائيز_بٽ = se_L4_Max_Untyped_بٽ;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        إذا (0 != reg.شروعات) {
            بنانشان صحيح align_بٽ = ctzl(reg.شروعات);
            إذا (سائيز_بٽ > align_بٽ) {
                سائيز_بٽ = align_بٽ;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        إذا (سائيز_بٽ >= se_L4_Min_Untyped_بٽ) {
            إذا (!provide_untyped_cap(پاڙ_cnode_cap, device_يادگيري_2, reg.شروعات, سائيز_بٽ, پهريون_untyped_جڳهه)) {
                أرجع false;
            }
        }
        reg.شروعات += ٻٽو_انگ(سائيز_بٽ);
    }
    أرجع true;
}

BOOT_ڪوڊ bool_t create_untypeds(cap_t پاڙ_cnode_cap)
{
    se_L4_جڳهه_Pos پهريون_untyped_جڳهه = ndks_boot.جڳهه_pos_cur;

    paddr_t شروعات = 0;
    لكل (لفظ_t i = 0; i < ndks_boot.resv_ڳڻپ; i++) {
        إذا (شروعات < ndks_boot.reserved[i].شروعات) {
            region_t reg = paddr_منزل_pptr_reg((p_region_t) {
                شروعات, ndks_boot.reserved[i].شروعات
            });
            إذا (!create_untypeds_for_region(پاڙ_cnode_cap, true, reg, پهريون_untyped_جڳهه)) {
                لکو("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                       (بنانشان صحيح)i, reg.شروعات, reg.پڇاڙي);
                أرجع false;
            }
        }

        شروعات = ndks_boot.reserved[i].پڇاڙي;
    }

    إذا (شروعات <= CONFIG_PADDR_USER_DEVICE_چوٽي - 1) {
        region_t reg = paddr_منزل_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            شروعات, (paddr_t) CONFIG_PADDR_USER_DEVICE_چوٽي
        });

        إذا (!create_untypeds_for_region(پاڙ_cnode_cap, true, reg, پهريون_untyped_جڳهه)) {
            لکو("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   reg.شروعات, reg.پڇاڙي);
            أرجع false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_منزل_pptr_reg(get_p_reg_kernel_img_boot());
    إذا (!create_untypeds_for_region(پاڙ_cnode_cap, false, boot_mem_reuse_reg, پهريون_untyped_جڳهه)) {
        لکو("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"] failed\n",
               boot_mem_reuse_reg.شروعات, boot_mem_reuse_reg.پڇاڙي);
        أرجع false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    لكل (لفظ_t i = 0; i < ARRAY_سائيز(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_خالي_جايون;
        إذا (!create_untypeds_for_region(پاڙ_cnode_cap, false, reg, پهريون_untyped_جڳهه)) {
            لکو("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   (بنانشان صحيح)i, reg.شروعات, reg.پڇاڙي);
            أرجع false;
        }
    }

    ndks_boot.bi_چوکٺو->untyped = (se_L4_جڳهه_Region) {
        .شروعات = پهريون_untyped_جڳهه,
        .پڇاڙي   = ndks_boot.جڳهه_pos_cur
    };

    أرجع true;
}

BOOT_ڪوڊ فراغ bi_finalise(فراغ)
{

    إذا (rootserver.paging.شروعات != rootserver.paging.پڇاڙي) {
        لکو("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.پڇاڙي - rootserver.paging.شروعات) >> se_L4_صفحو_جدول_بٽ);
    }

    ndks_boot.bi_چوکٺو->خالي_جايون = (se_L4_جڳهه_Region) {
        .شروعات = ndks_boot.جڳهه_pos_cur,
        .پڇاڙي   = ٻٽو_انگ(CONFIG_پاڙ_CNODE_سائيز_بٽ)
    };
}

BOOT_ڪوڊ ساكن bool_t check_موجود_يادگيري(لفظ_t n_موجود,
                                               ثابت p_region_t *موجود)
{
    /* The system configuration is broken if no region is available. */
    إذا (0 == n_موجود) {
        لکو("ERROR: no memory regions available\n");
        أرجع false;
    }

    لکو("Available phys memory regions: %"SEL4_PRIu_لفظ"\n", n_موجود);
    /* Force ordering and exclusivity of available regions. */
    لكل (لفظ_t i = 0; i < n_موجود; i++) {
        ثابت p_region_t *r = &موجود[i];
        لکو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->شروعات, r->پڇاڙي);

        /* Available regions must not wrap */
        إذا (r->شروعات > r->پڇاڙي) {
            لکو("ERROR: memory region %"SEL4_PRIu_لفظ" has start > end\n", i);
            أرجع false;
        }

        /* Available regions must be non-empty. */
        إذا (r->شروعات == r->پڇاڙي) {
            لکو("ERROR: memory region %"SEL4_PRIu_لفظ" empty\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        إذا ((i > 0) && (r->شروعات < موجود[i - 1].پڇاڙي)) {
            لکو("ERROR: memory region %"SEL4_PRIu_لفظ" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ساكن region_t avail_reg[MAX_انگ_FREEMEM_REG];

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
BOOT_ڪوڊ ساكن لفظ_t init_avail_reg(لفظ_t n_موجود,
                                       ثابت p_region_t *موجود)
{
    لفظ_t cnt = 0;
    لكل (لفظ_t i = 0; i < n_موجود; i++) {
        ثابت p_region_t *r = &موجود[i];

        إذا ((r->شروعات >= PADDR_چوٽي) || (r->پڇاڙي <= PADDR_بنياد)) {
            لکو("  region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->شروعات, r->پڇاڙي);
        } وإلا {
            p_region_t usable_reg = {
                .شروعات = MAX(r->شروعات, PADDR_بنياد),
                .پڇاڙي   = MIN(r->پڇاڙي, PADDR_چوٽي)
            };

            إذا (cnt >= ARRAY_سائيز(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                لکو("  WARNING: cannot use region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
                       " avail_reg[] is full\n", r->شروعات, r->پڇاڙي);
                أكد(0);
            } وإلا {
                avail_reg[cnt] = paddr_منزل_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    أرجع cnt;
}


BOOT_ڪوڊ ساكن bool_t check_reserved_يادگيري(لفظ_t n_reserved,
                                              ثابت region_t *reserved)
{
    لکو("Reserved virt address space regions: %"SEL4_PRIu_لفظ"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    لكل (لفظ_t i = 0; i < n_reserved; i++) {
        ثابت region_t *r = &reserved[i];
        لکو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->شروعات, r->پڇاڙي);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        إذا (r->شروعات > r->پڇاڙي) {
            لکو("ERROR: reserved region %"SEL4_PRIu_لفظ" has start > end\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        إذا ((i > 0) && (r->شروعات < reserved[i - 1].پڇاڙي)) {
            لکو("ERROR: reserved region %"SEL4_PRIu_لفظ" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_ڪوڊ bool_t init_freemem(لفظ_t n_موجود, ثابت p_region_t *موجود,
                              لفظ_t n_reserved, ثابت region_t *reserved,
                              v_region_t it_v_reg, لفظ_t extra_bi_سائيز_بٽ)
{
    إذا (!check_موجود_يادگيري(n_موجود, موجود)) {
        أرجع false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_موجود = init_avail_reg(n_موجود, موجود);
    إذا (0 == n_موجود) {
        لکو("ERROR: no available memory within the kernel window\n");
        أرجع false;
    }

    إذا (!check_reserved_يادگيري(n_reserved, reserved)) {
        أرجع false;
    }

    لكل (لفظ_t i = 0; i < ARRAY_سائيز(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_خالي_جايون;
    }

    لفظ_t a = 0;
    لفظ_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    طالما (a < n_موجود && r < n_reserved) {
        إذا (reserved[r].شروعات == reserved[r].پڇاڙي) {
            /* reserved region is empty - skip it */
            r++;
        } وإلا إذا (avail_reg[a].شروعات >= avail_reg[a].پڇاڙي) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } وإلا إذا (reserved[r].پڇاڙي <= avail_reg[a].شروعات) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
            r++;
        } وإلا إذا (reserved[r].شروعات >= avail_reg[a].پڇاڙي) {
            /* the reserved region is above the available region - take the whole thing */
            وجهو_region(avail_reg[a]);
            a++;
        } وإلا {
            /* the reserved region overlaps with the available region */
            إذا (reserved[r].شروعات <= avail_reg[a].شروعات) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].شروعات = MIN(avail_reg[a].پڇاڙي, reserved[r].پڇاڙي);
                /* do not increment reserved index here - there could be more overlapping regions */
            } وإلا {
                أكد(reserved[r].شروعات < avail_reg[a].پڇاڙي);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.پڇاڙي = reserved[r].شروعات;
                وجهو_region(m);
                إذا (avail_reg[a].پڇاڙي > reserved[r].پڇاڙي) {
                    avail_reg[a].شروعات = reserved[r].پڇاڙي;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } وإلا {
                    a++;
                }
            }
        }
    }

    لكل (; r < n_reserved; r++) {
        إذا (reserved[r].شروعات < reserved[r].پڇاڙي) {
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    لكل (; a < n_موجود; a++) {
        إذا (avail_reg[a].شروعات < avail_reg[a].پڇاڙي) {
            وجهو_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    صحيح i = ARRAY_سائيز(ndks_boot.freemem) - 1;
    إذا (!is_reg_خالي_جايون(ndks_boot.freemem[i])) {
        لکو("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (بنانشان صحيح)MAX_انگ_FREEMEM_REG);
        أرجع false;
    }
    /* skip any empty regions */
    لكل (; i >= 0 && is_reg_خالي_جايون(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    لفظ_t سائيز = calculate_rootserver_سائيز(it_v_reg, extra_bi_سائيز_بٽ);
    لفظ_t max = rootserver_max_سائيز_بٽ(extra_bi_سائيز_بٽ);
    لكل (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        أكد(i < ARRAY_سائيز(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        أكد(is_reg_خالي_جايون(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        صحيح خالي_جايون_اشاريو = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_شروعات = ndks_boot.freemem[i].پڇاڙي - سائيز;
        pptr_t شروعات = ڦيرو_هيٺ_ڏانهن(unaligned_شروعات, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        إذا (unaligned_شروعات <= ndks_boot.freemem[i].پڇاڙي
            && شروعات >= ndks_boot.freemem[i].شروعات) {
            create_rootserver_objects(شروعات, it_v_reg, extra_bi_سائيز_بٽ);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[خالي_جايون_اشاريو] = (region_t) {
                .شروعات = شروعات + سائيز,
                .پڇاڙي = ndks_boot.freemem[i].پڇاڙي
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].پڇاڙي = شروعات;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            أرجع true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[خالي_جايون_اشاريو] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_خالي_جايون;
    }

    /* We didn't find a big enough region. */
    لکو("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_لفظ"\n", max);
    أرجع false;
}
