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
خارجي حرف ki_boot_سرا[1];
BOOT_کوڈ p_region_t get_p_reg_kernel_img_boot(فراغ)
{
    أرجع (p_region_t) {
        .شروع = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_ادھار),
        .سرا   = kpptr_منزل_paddr(ki_boot_سرا)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_کوڈ p_region_t get_p_reg_kernel_img(فراغ)
{
    أرجع (p_region_t) {
        .شروع = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_ادھار),
        .سرا   = kpptr_منزل_paddr((ثابت فراغ *)KERNEL_ELF_چوٹی)
    };
}

BOOT_کوڈ ساكن فراغ ملاؤ_regions(فراغ)
{
    /* Walk through reserved regions and see if any can be merged */
    لفظ_t چوٹی = 0;
    لكل (لفظ_t i = 1; i < ndks_boot.resv_گنتی; i++) {
        إذا (ndks_boot.reserved[چوٹی].سرا == ndks_boot.reserved[i].شروع) {
            /* extend earlier region */
            ndks_boot.reserved[چوٹی].سرا = ndks_boot.reserved[i].سرا;
        } وإلا {
            /* add region */
            چوٹی++;
            ndks_boot.reserved[چوٹی] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_گنتی = ndks_boot.resv_گنتی > 0 ? چوٹی + 1 : 0;
}

BOOT_کوڈ bool_t reserve_region(p_region_t reg)
{
    لفظ_t i;
    أكد(reg.شروع <= reg.سرا);
    إذا (reg.شروع == reg.سرا) {
        أرجع true;
    }

    /* keep the regions in order */
    لكل (i = 0; i < ndks_boot.resv_گنتی; i++) {
        /* Try and merge the region to an existing one, if possible */
        إذا (ndks_boot.reserved[i].شروع == reg.سرا) {
            ndks_boot.reserved[i].شروع = reg.شروع;
            ملاؤ_regions();
            أرجع true;
        }
        إذا (ndks_boot.reserved[i].سرا == reg.شروع) {
            ndks_boot.reserved[i].سرا = reg.سرا;
            ملاؤ_regions();
            أرجع true;
        }
        /* Otherwise figure out where it should go. */
        إذا (ndks_boot.reserved[i].شروع > reg.سرا) {
            /* move regions down, making sure there's enough room */
            إذا (ndks_boot.resv_گنتی + 1 >= MAX_عدد_RESV_REG) {
                لکھو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.شروع, reg.سرا, (صحيح)MAX_عدد_RESV_REG);
                أرجع false;
            }
            لكل (لفظ_t j = ndks_boot.resv_گنتی; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_گنتی++;
            أرجع true;
        }
    }

    إذا (i + 1 == MAX_عدد_RESV_REG) {
        لکھو("Can't mark region 0x%"SEL4_PRIx_لفظ"-0x%"SEL4_PRIx_لفظ
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.شروع, reg.سرا, (صحيح)MAX_عدد_RESV_REG);
        أرجع false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_گنتی++;

    أرجع true;
}

BOOT_کوڈ ساكن bool_t داخل_کرو_region(region_t reg)
{
    أكد(reg.شروع <= reg.سرا);
    إذا (is_reg_خالی_تھاواں(reg)) {
        أرجع true;
    }

    لكل (لفظ_t i = 0; i < ARRAY_ناپ(ndks_boot.freemem); i++) {
        إذا (is_reg_خالی_تھاواں(ndks_boot.freemem[i])) {
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
    لکھو("no free memory slot left for [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.شروع, reg.سرا, (بنانشانی صحيح)MAX_عدد_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    أكد(0);

    أرجع false;
}

BOOT_کوڈ ساكن pptr_t alloc_rootserver_obj(لفظ_t ناپ_بٹ, لفظ_t n)
{
    pptr_t allocated = rootserver_mem.شروع;
    /* allocated memory must be aligned */
    أكد(allocated % ثنائی_ہندسہ(ناپ_بٹ) == 0);
    rootserver_mem.شروع += (n * ثنائی_ہندسہ(ناپ_بٹ));
    /* we must not have run out of memory */
    أكد(rootserver_mem.شروع <= rootserver_mem.سرا);
    memzero((فراغ *) allocated, n * ثنائی_ہندسہ(ناپ_بٹ));
    أرجع allocated;
}

BOOT_کوڈ ساكن لفظ_t rootserver_max_ناپ_بٹ(لفظ_t extra_bi_ناپ_بٹ)
{
    لفظ_t cnode_ناپ_بٹ = CONFIG_جڑ_CNODE_ناپ_بٹ + se_L4_تھاں_بٹ;
    لفظ_t max = MAX(cnode_ناپ_بٹ, se_L4_VSpace_بٹ);
    أرجع MAX(max, extra_bi_ناپ_بٹ);
}

BOOT_کوڈ ساكن لفظ_t calculate_rootserver_ناپ(v_region_t it_v_reg, لفظ_t extra_bi_ناپ_بٹ)
{
    /* work out how much memory we need for root server objects */
    لفظ_t ناپ = ثنائی_ہندسہ(CONFIG_جڑ_CNODE_ناپ_بٹ + se_L4_تھاں_بٹ);
    ناپ += ثنائی_ہندسہ(seL4_TCBBits); // root thread tcb
    ناپ += ثنائی_ہندسہ(se_L4_ورقہ_بٹ); // ipc buf
    ناپ += ثنائی_ہندسہ(se_L4_Boot_Info_چوکھٹا_بٹ); // boot info
    ناپ += ثنائی_ہندسہ(se_L4_ASIDPool_بٹ);
    ناپ += extra_bi_ناپ_بٹ > 0 ? ثنائی_ہندسہ(extra_bi_ناپ_بٹ) : 0;
    ناپ += ثنائی_ہندسہ(se_L4_VSpace_بٹ); // root vspace
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    ناپ += ثنائی_ہندسہ(se_L4_Min_Sched_Context_بٹ); // root sched context
#جے_انت
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    أرجع ناپ + arch_get_n_paging(it_v_reg) * ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ);
}

BOOT_کوڈ ساكن فراغ maybe_alloc_extra_bi(لفظ_t cmp_ناپ_بٹ, لفظ_t extra_bi_ناپ_بٹ)
{
    إذا (extra_bi_ناپ_بٹ >= cmp_ناپ_بٹ && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_ناپ_بٹ, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_کوڈ ساكن فراغ create_rootserver_objects(pptr_t شروع, v_region_t it_v_reg,
                                                لفظ_t extra_bi_ناپ_بٹ)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    لفظ_t cnode_ناپ_بٹ = CONFIG_جڑ_CNODE_ناپ_بٹ + se_L4_تھاں_بٹ;
    لفظ_t max = rootserver_max_ناپ_بٹ(extra_bi_ناپ_بٹ);

    لفظ_t ناپ = calculate_rootserver_ناپ(it_v_reg, extra_bi_ناپ_بٹ);
    rootserver_mem.شروع = شروع;
    rootserver_mem.سرا = شروع + ناپ;

    maybe_alloc_extra_bi(max, extra_bi_ناپ_بٹ);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#إذا (CONFIG_جڑ_CNODE_ناپ_بٹ + se_L4_تھاں_بٹ) > se_L4_VSpace_بٹ
    rootserver.cnode = alloc_rootserver_obj(cnode_ناپ_بٹ, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_بٹ, extra_bi_ناپ_بٹ);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٹ, 1);
#وإلا
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_بٹ, 1);
    maybe_alloc_extra_bi(cnode_ناپ_بٹ, extra_bi_ناپ_بٹ);
    rootserver.cnode = alloc_rootserver_obj(cnode_ناپ_بٹ, 1);
#جے_انت

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_ورقہ_بٹ, extra_bi_ناپ_بٹ);
    compile_assert(invalid_se_L4_ASIDPool_بٹ, se_L4_ASIDPool_بٹ == se_L4_ورقہ_بٹ);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_بٹ, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_ورقہ_بٹ, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_چوکھٹا_بٹ, se_L4_Boot_Info_چوکھٹا_بٹ == se_L4_ورقہ_بٹ);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_چوکھٹا_بٹ, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#إذا seL4_TCBBits >= se_L4_ورقہ_جدول_بٹ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#جے_انت

    /* paging structures are 4k on every arch except aarch32 (1k) */
    لفظ_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.شروع = alloc_rootserver_obj(se_L4_ورقہ_جدول_بٹ, n);
    rootserver.paging.سرا = rootserver.paging.شروع + n * ثنائی_ہندسہ(se_L4_ورقہ_جدول_بٹ);

    /* for most archs, TCBs are smaller than page tables */
#إذا seL4_TCBBits < se_L4_ورقہ_جدول_بٹ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#جے_انت

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_بٹ, 1);
#جے_انت
    /* we should have allocated all our memory */
    أكد(rootserver_mem.شروع == rootserver_mem.سرا);
}

BOOT_کوڈ فراغ write_تھاں(تھاں_ptr_t تھاں_ptr, cap_t cap)
{
    تھاں_ptr->cap = cap;

    تھاں_ptr->cteMDBNode = nullMDBNode;
    mdb_گرہ_ptr_سیٹ_mdb_Revocable(&تھاں_ptr->cteMDBNode, true);
    mdb_گرہ_ptr_سیٹ_mdb_پہلا_Badged(&تھاں_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(جڑ_cnode_ناپ_valid,
               CONFIG_جڑ_CNODE_ناپ_بٹ < 32 - se_L4_تھاں_بٹ &&
               ثنائی_ہندسہ(CONFIG_جڑ_CNODE_ناپ_بٹ) >= se_L4_عدد_Initial_Caps &&
               CONFIG_جڑ_CNODE_ناپ_بٹ >= (se_L4_ورقہ_بٹ - se_L4_تھاں_بٹ))

BOOT_کوڈ cap_t
create_جڑ_cnode(فراغ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_جڑ_CNODE_ناپ_بٹ, /* radix */
                    لفظ_بٹ - CONFIG_جڑ_CNODE_ناپ_بٹ, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_تھاں(تھاں_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    أرجع cap;
}

/* Check domain scheduler assumptions. */
compile_assert(عدد_domains_valid,
               CONFIG_عدد_DOMAINS >= 1 && CONFIG_عدد_DOMAINS <= 256)
compile_assert(عدد_priorities_valid,
               CONFIG_عدد_PRIORITIES >= 1 && CONFIG_عدد_PRIORITIES <= 256)

BOOT_کوڈ فراغ
create_domain_cap(cap_t جڑ_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapDomain), cap);
}

BOOT_کوڈ cap_t create_ipcbuf_چوکھٹا_cap(cap_t جڑ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_حافظہ((فراغ *)rootserver.ipc_buf, ورقہ_بٹ);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکھٹا_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    أرجع cap;
}

BOOT_کوڈ فراغ create_bi_چوکھٹا_cap(cap_t جڑ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_چوکھٹا_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), se_L4_Cap_Boot_Info_چوکھٹا), cap);
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
BOOT_کوڈ لفظ_t calculate_extra_bi_ناپ_بٹ(لفظ_t extra_ناپ)
{
    إذا (extra_ناپ == 0) {
        أرجع 0;
    }

    لفظ_t clzl_ret = clzl(گیڑا_اتے(extra_ناپ, se_L4_ورقہ_بٹ));
    لفظ_t msb = se_L4_لفظ_بٹ - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    إذا (extra_ناپ > ثنائی_ہندسہ(msb)) {
        msb++;
    }
    أرجع msb;
}

BOOT_کوڈ فراغ populate_bi_چوکھٹا(گرہ_id_t گرہ_id, لفظ_t عدد_nodes,
                                 vptr_t ipcbuf_vptr, لفظ_t extra_bi_ناپ)
{
    /* clear boot info memory */
    clear_حافظہ((فراغ *)rootserver.boot_info, se_L4_Boot_Info_چوکھٹا_بٹ);
    إذا (extra_bi_ناپ) {
        clear_حافظہ((فراغ *)rootserver.extra_bi,
                    calculate_extra_bi_ناپ_بٹ(extra_bi_ناپ));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->گرہ_ID = گرہ_id;
    bi->عدد_Nodes = عدد_nodes;
    bi->عدد_IOPTLevels = 0;
    bi->ipc_بفر = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_ناپ_بٹ = CONFIG_جڑ_CNODE_ناپ_بٹ;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_ناپ;

    ndks_boot.bi_چوکھٹا = bi;
    ndks_boot.تھاں_pos_cur = se_L4_عدد_Initial_Caps;
}

BOOT_کوڈ bool_t provide_cap(cap_t جڑ_cnode_cap, cap_t cap)
{
    إذا (ndks_boot.تھاں_pos_cur >= ثنائی_ہندسہ(CONFIG_جڑ_CNODE_ناپ_بٹ)) {
        لکھو("ERROR: can't add another cap, all %"SEL4_PRIu_لفظ
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               ثنائی_ہندسہ(CONFIG_جڑ_CNODE_ناپ_بٹ));
        أرجع false;
    }
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), ndks_boot.تھاں_pos_cur), cap);
    ndks_boot.تھاں_pos_cur++;
    أرجع true;
}

BOOT_کوڈ create_چوکھٹے_دا_region_ret_t create_چوکھٹے_دا_region(
    cap_t    جڑ_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_وتھ
)
{
    pptr_t     f;
    cap_t      چوکھٹا_cap;
    se_L4_تھاں_Pos تھاں_pos_before;
    se_L4_تھاں_Pos تھاں_pos_after;

    تھاں_pos_before = ndks_boot.تھاں_pos_cur;

    لكل (f = reg.شروع; f < reg.سرا; f += ثنائی_ہندسہ(ورقہ_بٹ)) {
        إذا (do_map) {
            چوکھٹا_cap = create_mapped_it_چوکھٹا_cap(pd_cap, f, pptr_منزل_paddr((فراغ *)(f - pv_وتھ)), IT_ASID, false, true);
        } وإلا {
            چوکھٹا_cap = create_unmapped_it_چوکھٹا_cap(f, false);
        }
        إذا (!provide_cap(جڑ_cnode_cap, چوکھٹا_cap)) {
            أرجع (create_چوکھٹے_دا_region_ret_t) {
                .region  = S_REG_خالی_تھاواں,
                .success = false
            };
        }
    }

    تھاں_pos_after = ndks_boot.تھاں_pos_cur;

    أرجع (create_چوکھٹے_دا_region_ret_t) {
        .region = (se_L4_تھاں_Region) {
            .شروع = تھاں_pos_before,
            .سرا   = تھاں_pos_after
        },
        .success = true
    };
}

BOOT_کوڈ cap_t create_it_asid_pool(cap_t جڑ_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_بلند(IT_ASID) << asid_کم_بٹ, rootserver.asid_pool);
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_تھاں(
        تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    أرجع ap_cap;
}

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
BOOT_کوڈ ساكن فراغ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_کوڈ bool_t init_sched_control(cap_t جڑ_cnode_cap, لفظ_t عدد_nodes)
{
    se_L4_تھاں_Pos تھاں_pos_before = ndks_boot.تھاں_pos_cur;

    /* create a sched control cap for each core */
    لكل (بنانشانی صحيح i = 0; i < عدد_nodes; i++) {
        إذا (!provide_cap(جڑ_cnode_cap, cap_sched_control_cap_new(i))) {
            لکھو("can't init sched_control for node %u, provide_cap() failed\n", i);
            أرجع false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_چوکھٹا->schedcontrol = (se_L4_تھاں_Region) {
        .شروع = تھاں_pos_before,
        .سرا = ndks_boot.تھاں_pos_cur
    };

    أرجع true;
}
#جے_انت

BOOT_کوڈ فراغ create_idle_thread(فراغ)
{
    pptr_t pptr;

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    لكل (بنانشانی صحيح i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
#جے_انت /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        گرہ_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_وتھ);
        configureIdleThread(گرہ_STATE_ON_CORE(ksIdleThread, i));
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
        سیٹ_Thread_Name(گرہ_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#جے_انت
        SMP_COND_STATEMENT(گرہ_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
        configure_sched_context(گرہ_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(گرہ_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        گرہ_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#جے_انت
#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
    }
#جے_انت /* ENABLE_SMP_SUPPORT */
}

BOOT_کوڈ tcb_t *create_initial_thread(cap_t جڑ_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_اندراج, vptr_t bi_چوکھٹا_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_وتھ);
#جے_بنا_تعریف CONFIG_KERNEL_MCS
    tcb->tcb_ویلا_ٹکڑا = CONFIG_ویلا_ٹکڑا;
#جے_انت

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    إذا (dc_ret.حالت != EXCEPTION_NONE) {
        لکھو("Failed to derive copy of IPC Buffer\n");
        أرجع عدم;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_داخل_کرو(
        جڑ_cnode_cap,
        تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadCNode),
        تھاں_PTR(rootserver.tcb, tcbCTable)
    );
    cte_داخل_کرو(
        it_pd_cap,
        تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadVSpace),
        تھاں_PTR(rootserver.tcb, tcbVTable)
    );
    cte_داخل_کرو(
        dc_ret.cap,
        تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadIPCBuffer),
        تھاں_PTR(rootserver.tcb, tcb_بفر)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    سیٹ_رجسٹر(tcb, cap_رجسٹر, bi_چوکھٹا_vptr);
    سیٹ_اگلا_PC(tcb, ui_v_اندراج);

    /* initialise TCB */
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_منزل_Ticks(CONFIG_BOOT_THREAD_ویلا_ٹکڑا * US_وچ_MS));
#جے_انت

    tcb->tcb_ترجیح = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#جے_بنا_تعریف CONFIG_KERNEL_MCS
    تیاری_جواب_Master(tcb);
#جے_انت
    سیٹ_Thread_State(tcb, ThreadState_Running);

#جے_بنا_تعریف CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#جے_انت

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadTCB), cap);

#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_بٹ);
    write_تھاں(تھاں_PTR(pptr_دا_cap(جڑ_cnode_cap), seL4_CapInitThreadSC), cap);
#جے_انت
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    سیٹ_Thread_Name(tcb, "rootserver");
#جے_انت

    أرجع tcb;
}

#جے_تعریف_شدہ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_کوڈ ساكن bool_t hypervisor_present(فراغ)
{
#جے_تعریف_شدہ CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    إذا ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        أرجع true;
    }
#جے_انت
    أرجع false;
}

BOOT_کوڈ فراغ clock_sync_test(فراغ)
{
    ticks_t t, t0;
    ticks_t margin = us_منزل_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    أكد(getCurrentCPUIndex() != 0);
    t = گرہ_STATE_ON_CORE(ks_Cur_ویلا, 0);
    افعل {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = گرہ_STATE_ON_CORE(ks_Cur_ویلا, 0);
    } طالما (t0 == t);
    t = get_Current_ویلا();
    لکھو("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (صحيح)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    إذا (hypervisor_present()) {
        لکھو("clock_sync_test[%d]: disabled, detected running as VM\n", (صحيح)getCurrentCPUIndex());
    } وإلا {
        أكد(t0 <= margin + t && t <= t0 + margin);
    }
}
#جے_انت

BOOT_کوڈ فراغ init_core_state(tcb_t *scheduler_action)
{
#جے_تعریف_شدہ CONFIG_HAVE_FPU
    گرہ_STATE(ksCurFPUOwner) = عدم;
#جے_انت
#جے_تعریف_شدہ CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    گرہ_STATE(ksDebugTCBs) = عدم;
    إذا (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_چنو_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(گرہ_STATE(ksIdleThread));
#جے_انت
    گرہ_STATE(ksSchedulerAction) = scheduler_action;
    گرہ_STATE(ksCurThread) = گرہ_STATE(ksIdleThread);
#جے_تعریف_شدہ CONFIG_KERNEL_MCS
    گرہ_STATE(ksCurSC) = گرہ_STATE(ksCurThread->tcbSchedContext);
    گرہ_STATE(ksConsumed) = 0;
    گرہ_STATE(ksReprogram) = true;
    گرہ_STATE(ks_Release_قطار.سر) = عدم;
    گرہ_STATE(ks_Release_قطار.سرا) = عدم;
    گرہ_STATE(ks_Cur_ویلا) = get_Current_ویلا();
#جے_انت
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_ویلا = DSCHED_MAX_DURATION;
    ks_Dom_ویلا_سارنی[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_کوڈ ساكن bool_t pptr_وچ_kernel_کھڑکی(pptr_t pptr)
{
    أرجع pptr >= PPTR_ادھار && pptr < PPTR_چوٹی;
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
BOOT_کوڈ ساكن bool_t provide_untyped_cap(
    cap_t      جڑ_cnode_cap,
    bool_t     device_حافظہ_2,
    pptr_t     pptr,
    لفظ_t     ناپ_بٹ,
    se_L4_تھاں_Pos پہلا_untyped_تھاں
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    إذا (ناپ_بٹ > se_L4_Max_Untyped_بٹ || ناپ_بٹ < se_L4_Min_Untyped_بٹ) {
        لکھو("Kernel init: Invalid untyped size %"SEL4_PRIu_لفظ"\n", ناپ_بٹ);
        أرجع false;
    }

    /* All cap ptrs must be aligned to object size */
    إذا (!IS_ALIGNED(pptr, ناپ_بٹ)) {
        لکھو("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_لفظ")\n", (فراغ *)pptr, ناپ_بٹ);
        أرجع false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    إذا (!device_حافظہ_2 && !pptr_وچ_kernel_کھڑکی(pptr)) {
        لکھو("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (فراغ *)pptr);
        أرجع false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    إذا (!device_حافظہ_2 && !pptr_وچ_kernel_کھڑکی(pptr + نقاب(ناپ_بٹ))) {
        لکھو("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_لفظ")\n",
               (فراغ *)pptr, ناپ_بٹ);
        أرجع false;
    }

    لفظ_t i = ndks_boot.تھاں_pos_cur - پہلا_untyped_تھاں;
    إذا (i < CONFIG_MAX_عدد_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_چوکھٹا->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_منزل_paddr((فراغ *)pptr),
            .ناپ_بٹ_2 = ناپ_بٹ,
            .isDevice = device_حافظہ_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_اشاریہ(ناپ_بٹ),
                                     device_حافظہ_2, ناپ_بٹ, pptr);
        ret = provide_cap(جڑ_cnode_cap, ut_cap);
    } وإلا {
        لکھو("Kernel init: Too many untyped regions for boot info\n");
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
BOOT_کوڈ ساكن bool_t create_untypeds_for_region(
    cap_t      جڑ_cnode_cap,
    bool_t     device_حافظہ_2,
    region_t   reg,
    se_L4_تھاں_Pos پہلا_untyped_تھاں
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    طالما (!is_reg_خالی_تھاواں(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        بنانشانی صحيح ناپ_بٹ = se_L4_لفظ_بٹ - 1 - clzl(reg.سرا - reg.شروع);
        /* The size can't exceed the largest possible untyped size. */
        إذا (ناپ_بٹ > se_L4_Max_Untyped_بٹ) {
            ناپ_بٹ = se_L4_Max_Untyped_بٹ;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        إذا (0 != reg.شروع) {
            بنانشانی صحيح align_بٹ = ctzl(reg.شروع);
            إذا (ناپ_بٹ > align_بٹ) {
                ناپ_بٹ = align_بٹ;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        إذا (ناپ_بٹ >= se_L4_Min_Untyped_بٹ) {
            إذا (!provide_untyped_cap(جڑ_cnode_cap, device_حافظہ_2, reg.شروع, ناپ_بٹ, پہلا_untyped_تھاں)) {
                أرجع false;
            }
        }
        reg.شروع += ثنائی_ہندسہ(ناپ_بٹ);
    }
    أرجع true;
}

BOOT_کوڈ bool_t create_untypeds(cap_t جڑ_cnode_cap)
{
    se_L4_تھاں_Pos پہلا_untyped_تھاں = ndks_boot.تھاں_pos_cur;

    paddr_t شروع = 0;
    لكل (لفظ_t i = 0; i < ndks_boot.resv_گنتی; i++) {
        إذا (شروع < ndks_boot.reserved[i].شروع) {
            region_t reg = paddr_منزل_pptr_reg((p_region_t) {
                شروع, ndks_boot.reserved[i].شروع
            });
            إذا (!create_untypeds_for_region(جڑ_cnode_cap, true, reg, پہلا_untyped_تھاں)) {
                لکھو("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                       (بنانشانی صحيح)i, reg.شروع, reg.سرا);
                أرجع false;
            }
        }

        شروع = ndks_boot.reserved[i].سرا;
    }

    إذا (شروع <= CONFIG_PADDR_USER_DEVICE_چوٹی - 1) {
        region_t reg = paddr_منزل_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            شروع, (paddr_t) CONFIG_PADDR_USER_DEVICE_چوٹی
        });

        إذا (!create_untypeds_for_region(جڑ_cnode_cap, true, reg, پہلا_untyped_تھاں)) {
            لکھو("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   reg.شروع, reg.سرا);
            أرجع false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_منزل_pptr_reg(get_p_reg_kernel_img_boot());
    إذا (!create_untypeds_for_region(جڑ_cnode_cap, false, boot_mem_reuse_reg, پہلا_untyped_تھاں)) {
        لکھو("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"] failed\n",
               boot_mem_reuse_reg.شروع, boot_mem_reuse_reg.سرا);
        أرجع false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    لكل (لفظ_t i = 0; i < ARRAY_ناپ(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_خالی_تھاواں;
        إذا (!create_untypeds_for_region(جڑ_cnode_cap, false, reg, پہلا_untyped_تھاں)) {
            لکھو("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") failed\n",
                   (بنانشانی صحيح)i, reg.شروع, reg.سرا);
            أرجع false;
        }
    }

    ndks_boot.bi_چوکھٹا->untyped = (se_L4_تھاں_Region) {
        .شروع = پہلا_untyped_تھاں,
        .سرا   = ndks_boot.تھاں_pos_cur
    };

    أرجع true;
}

BOOT_کوڈ فراغ bi_finalise(فراغ)
{

    إذا (rootserver.paging.شروع != rootserver.paging.سرا) {
        لکھو("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.سرا - rootserver.paging.شروع) >> se_L4_ورقہ_جدول_بٹ);
    }

    ndks_boot.bi_چوکھٹا->خالی_تھاواں = (se_L4_تھاں_Region) {
        .شروع = ndks_boot.تھاں_pos_cur,
        .سرا   = ثنائی_ہندسہ(CONFIG_جڑ_CNODE_ناپ_بٹ)
    };
}

BOOT_کوڈ ساكن bool_t check_دستیاب_حافظہ(لفظ_t n_دستیاب,
                                               ثابت p_region_t *دستیاب)
{
    /* The system configuration is broken if no region is available. */
    إذا (0 == n_دستیاب) {
        لکھو("ERROR: no memory regions available\n");
        أرجع false;
    }

    لکھو("Available phys memory regions: %"SEL4_PRIu_لفظ"\n", n_دستیاب);
    /* Force ordering and exclusivity of available regions. */
    لكل (لفظ_t i = 0; i < n_دستیاب; i++) {
        ثابت p_region_t *r = &دستیاب[i];
        لکھو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->شروع, r->سرا);

        /* Available regions must not wrap */
        إذا (r->شروع > r->سرا) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" has start > end\n", i);
            أرجع false;
        }

        /* Available regions must be non-empty. */
        إذا (r->شروع == r->سرا) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" empty\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        إذا ((i > 0) && (r->شروع < دستیاب[i - 1].سرا)) {
            لکھو("ERROR: memory region %"SEL4_PRIu_لفظ" in wrong order\n", i);
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
BOOT_کوڈ ساكن لفظ_t init_avail_reg(لفظ_t n_دستیاب,
                                       ثابت p_region_t *دستیاب)
{
    لفظ_t cnt = 0;
    لكل (لفظ_t i = 0; i < n_دستیاب; i++) {
        ثابت p_region_t *r = &دستیاب[i];

        إذا ((r->شروع >= PADDR_چوٹی) || (r->سرا <= PADDR_ادھار)) {
            لکھو("  region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->شروع, r->سرا);
        } وإلا {
            p_region_t usable_reg = {
                .شروع = MAX(r->شروع, PADDR_ادھار),
                .سرا   = MIN(r->سرا, PADDR_چوٹی)
            };

            إذا (cnt >= ARRAY_ناپ(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                لکھو("  WARNING: cannot use region [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ"),"
                       " avail_reg[] is full\n", r->شروع, r->سرا);
                أكد(0);
            } وإلا {
                avail_reg[cnt] = paddr_منزل_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    أرجع cnt;
}


BOOT_کوڈ ساكن bool_t check_reserved_حافظہ(لفظ_t n_reserved,
                                              ثابت region_t *reserved)
{
    لکھو("Reserved virt address space regions: %"SEL4_PRIu_لفظ"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    لكل (لفظ_t i = 0; i < n_reserved; i++) {
        ثابت region_t *r = &reserved[i];
        لکھو("  [%"SEL4_PRIx_لفظ"..%"SEL4_PRIx_لفظ")\n", r->شروع, r->سرا);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        إذا (r->شروع > r->سرا) {
            لکھو("ERROR: reserved region %"SEL4_PRIu_لفظ" has start > end\n", i);
            أرجع false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        إذا ((i > 0) && (r->شروع < reserved[i - 1].سرا)) {
            لکھو("ERROR: reserved region %"SEL4_PRIu_لفظ" in wrong order\n", i);
            أرجع false;
        }
    }

    أرجع true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_کوڈ bool_t init_freemem(لفظ_t n_دستیاب, ثابت p_region_t *دستیاب,
                              لفظ_t n_reserved, ثابت region_t *reserved,
                              v_region_t it_v_reg, لفظ_t extra_bi_ناپ_بٹ)
{
    إذا (!check_دستیاب_حافظہ(n_دستیاب, دستیاب)) {
        أرجع false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_دستیاب = init_avail_reg(n_دستیاب, دستیاب);
    إذا (0 == n_دستیاب) {
        لکھو("ERROR: no available memory within the kernel window\n");
        أرجع false;
    }

    إذا (!check_reserved_حافظہ(n_reserved, reserved)) {
        أرجع false;
    }

    لكل (لفظ_t i = 0; i < ARRAY_ناپ(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_خالی_تھاواں;
    }

    لفظ_t a = 0;
    لفظ_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    طالما (a < n_دستیاب && r < n_reserved) {
        إذا (reserved[r].شروع == reserved[r].سرا) {
            /* reserved region is empty - skip it */
            r++;
        } وإلا إذا (avail_reg[a].شروع >= avail_reg[a].سرا) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } وإلا إذا (reserved[r].سرا <= avail_reg[a].شروع) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
            r++;
        } وإلا إذا (reserved[r].شروع >= avail_reg[a].سرا) {
            /* the reserved region is above the available region - take the whole thing */
            داخل_کرو_region(avail_reg[a]);
            a++;
        } وإلا {
            /* the reserved region overlaps with the available region */
            إذا (reserved[r].شروع <= avail_reg[a].شروع) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].شروع = MIN(avail_reg[a].سرا, reserved[r].سرا);
                /* do not increment reserved index here - there could be more overlapping regions */
            } وإلا {
                أكد(reserved[r].شروع < avail_reg[a].سرا);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.سرا = reserved[r].شروع;
                داخل_کرو_region(m);
                إذا (avail_reg[a].سرا > reserved[r].سرا) {
                    avail_reg[a].شروع = reserved[r].سرا;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } وإلا {
                    a++;
                }
            }
        }
    }

    لكل (; r < n_reserved; r++) {
        إذا (reserved[r].شروع < reserved[r].سرا) {
            reserve_region(pptr_منزل_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    لكل (; a < n_دستیاب; a++) {
        إذا (avail_reg[a].شروع < avail_reg[a].سرا) {
            داخل_کرو_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    صحيح i = ARRAY_ناپ(ndks_boot.freemem) - 1;
    إذا (!is_reg_خالی_تھاواں(ndks_boot.freemem[i])) {
        لکھو("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (بنانشانی صحيح)MAX_عدد_FREEMEM_REG);
        أرجع false;
    }
    /* skip any empty regions */
    لكل (; i >= 0 && is_reg_خالی_تھاواں(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    لفظ_t ناپ = calculate_rootserver_ناپ(it_v_reg, extra_bi_ناپ_بٹ);
    لفظ_t max = rootserver_max_ناپ_بٹ(extra_bi_ناپ_بٹ);
    لكل (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        أكد(i < ARRAY_ناپ(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        أكد(is_reg_خالی_تھاواں(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        صحيح خالی_تھاواں_اشاریہ = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_شروع = ndks_boot.freemem[i].سرا - ناپ;
        pptr_t شروع = گیڑا_تھلے(unaligned_شروع, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        إذا (unaligned_شروع <= ndks_boot.freemem[i].سرا
            && شروع >= ndks_boot.freemem[i].شروع) {
            create_rootserver_objects(شروع, it_v_reg, extra_bi_ناپ_بٹ);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[خالی_تھاواں_اشاریہ] = (region_t) {
                .شروع = شروع + ناپ,
                .سرا = ndks_boot.freemem[i].سرا
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].سرا = شروع;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            أرجع true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[خالی_تھاواں_اشاریہ] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_خالی_تھاواں;
    }

    /* We didn't find a big enough region. */
    لکھو("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_لفظ"\n", max);
    أرجع false;
}
