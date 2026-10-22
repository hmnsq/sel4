/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <kernel/thread.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/registerset.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <model/statedata.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <object/domain.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/boot.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/kernel/vspace.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <hardware.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS ਸਥਿਰ region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
ਬਾਹਰੀ ਅੱਖਰ ki_boot_ਸਿਰਾ[1];
BOOT_ਕੋਡ p_region_t get_p_reg_kernel_img_boot(ਖਾਲੀ)
{
    ਵਾਪਸ (p_region_t) {
        .ਸ਼ੁਰੂ = kpptr_ਮੰਜ਼ਿਲ_paddr((ਅਚਲ ਖਾਲੀ *)KERNEL_ELF_ਅਧਾਰ),
        .ਸਿਰਾ   = kpptr_ਮੰਜ਼ਿਲ_paddr(ki_boot_ਸਿਰਾ)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_ਕੋਡ p_region_t get_p_reg_kernel_img(ਖਾਲੀ)
{
    ਵਾਪਸ (p_region_t) {
        .ਸ਼ੁਰੂ = kpptr_ਮੰਜ਼ਿਲ_paddr((ਅਚਲ ਖਾਲੀ *)KERNEL_ELF_ਅਧਾਰ),
        .ਸਿਰਾ   = kpptr_ਮੰਜ਼ਿਲ_paddr((ਅਚਲ ਖਾਲੀ *)KERNEL_ELF_ਸਿਖਰ)
    };
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ ਮਿਲਾਓ_regions(ਖਾਲੀ)
{
    /* Walk through reserved regions and see if any can be merged */
    ਸ਼ਬਦ_t ਸਿਖਰ = 0;
    ਲਈ (ਸ਼ਬਦ_t i = 1; i < ndks_boot.resv_ਗਿਣਤੀ; i++) {
        ਜੇ (ndks_boot.reserved[ਸਿਖਰ].ਸਿਰਾ == ndks_boot.reserved[i].ਸ਼ੁਰੂ) {
            /* extend earlier region */
            ndks_boot.reserved[ਸਿਖਰ].ਸਿਰਾ = ndks_boot.reserved[i].ਸਿਰਾ;
        } ਨਹੀਂਤਾਂ {
            /* add region */
            ਸਿਖਰ++;
            ndks_boot.reserved[ਸਿਖਰ] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_ਗਿਣਤੀ = ndks_boot.resv_ਗਿਣਤੀ > 0 ? ਸਿਖਰ + 1 : 0;
}

BOOT_ਕੋਡ bool_t reserve_region(p_region_t reg)
{
    ਸ਼ਬਦ_t i;
    ਪ੍ਰਤਿਜ੍ਞਾ(reg.ਸ਼ੁਰੂ <= reg.ਸਿਰਾ);
    ਜੇ (reg.ਸ਼ੁਰੂ == reg.ਸਿਰਾ) {
        ਵਾਪਸ true;
    }

    /* keep the regions in order */
    ਲਈ (i = 0; i < ndks_boot.resv_ਗਿਣਤੀ; i++) {
        /* Try and merge the region to an existing one, if possible */
        ਜੇ (ndks_boot.reserved[i].ਸ਼ੁਰੂ == reg.ਸਿਰਾ) {
            ndks_boot.reserved[i].ਸ਼ੁਰੂ = reg.ਸ਼ੁਰੂ;
            ਮਿਲਾਓ_regions();
            ਵਾਪਸ true;
        }
        ਜੇ (ndks_boot.reserved[i].ਸਿਰਾ == reg.ਸ਼ੁਰੂ) {
            ndks_boot.reserved[i].ਸਿਰਾ = reg.ਸਿਰਾ;
            ਮਿਲਾਓ_regions();
            ਵਾਪਸ true;
        }
        /* Otherwise figure out where it should go. */
        ਜੇ (ndks_boot.reserved[i].ਸ਼ੁਰੂ > reg.ਸਿਰਾ) {
            /* move regions down, making sure there's enough room */
            ਜੇ (ndks_boot.resv_ਗਿਣਤੀ + 1 >= MAX_ਅੰਕ_RESV_REG) {
                ਲਿਖੋ("Can't mark region 0x%"SEL4_PRIx_ਸ਼ਬਦ"-0x%"SEL4_PRIx_ਸ਼ਬਦ
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ, (ਪੂਰਨ)MAX_ਅੰਕ_RESV_REG);
                ਵਾਪਸ false;
            }
            ਲਈ (ਸ਼ਬਦ_t j = ndks_boot.resv_ਗਿਣਤੀ; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_ਗਿਣਤੀ++;
            ਵਾਪਸ true;
        }
    }

    ਜੇ (i + 1 == MAX_ਅੰਕ_RESV_REG) {
        ਲਿਖੋ("Can't mark region 0x%"SEL4_PRIx_ਸ਼ਬਦ"-0x%"SEL4_PRIx_ਸ਼ਬਦ
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ, (ਪੂਰਨ)MAX_ਅੰਕ_RESV_REG);
        ਵਾਪਸ false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_ਗਿਣਤੀ++;

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t ਪਾਓ_region(region_t reg)
{
    ਪ੍ਰਤਿਜ੍ਞਾ(reg.ਸ਼ੁਰੂ <= reg.ਸਿਰਾ);
    ਜੇ (is_reg_ਖਾਲੀ_ਥਾਵਾਂ(reg)) {
        ਵਾਪਸ true;
    }

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ARRAY_ਮਾਪ(ndks_boot.freemem); i++) {
        ਜੇ (is_reg_ਖਾਲੀ_ਥਾਵਾਂ(ndks_boot.freemem[i])) {
            reserve_region(pptr_ਮੰਜ਼ਿਲ_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            ਵਾਪਸ true;
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
    ਲਿਖੋ("no free memory slot left for [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MAX_ਅੰਕ_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    ਪ੍ਰਤਿਜ੍ਞਾ(0);

    ਵਾਪਸ false;
}

BOOT_ਕੋਡ ਸਥਿਰ pptr_t alloc_rootserver_obj(ਸ਼ਬਦ_t ਮਾਪ_ਬਿੱਟ, ਸ਼ਬਦ_t n)
{
    pptr_t allocated = rootserver_mem.ਸ਼ੁਰੂ;
    /* allocated memory must be aligned */
    ਪ੍ਰਤਿਜ੍ਞਾ(allocated % ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_ਬਿੱਟ) == 0);
    rootserver_mem.ਸ਼ੁਰੂ += (n * ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_ਬਿੱਟ));
    /* we must not have run out of memory */
    ਪ੍ਰਤਿਜ੍ਞਾ(rootserver_mem.ਸ਼ੁਰੂ <= rootserver_mem.ਸਿਰਾ);
    memzero((ਖਾਲੀ *) allocated, n * ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_ਬਿੱਟ));
    ਵਾਪਸ allocated;
}

BOOT_ਕੋਡ ਸਥਿਰ ਸ਼ਬਦ_t rootserver_max_ਮਾਪ_ਬਿੱਟ(ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    ਸ਼ਬਦ_t cnode_ਮਾਪ_ਬਿੱਟ = CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ + se_L4_ਥਾਂ_ਬਿੱਟ;
    ਸ਼ਬਦ_t max = MAX(cnode_ਮਾਪ_ਬਿੱਟ, se_L4_VSpace_ਬਿੱਟ);
    ਵਾਪਸ MAX(max, extra_bi_ਮਾਪ_ਬਿੱਟ);
}

BOOT_ਕੋਡ ਸਥਿਰ ਸ਼ਬਦ_t calculate_rootserver_ਮਾਪ(v_region_t it_v_reg, ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    /* work out how much memory we need for root server objects */
    ਸ਼ਬਦ_t ਮਾਪ = ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ + se_L4_ਥਾਂ_ਬਿੱਟ);
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(seL4_TCBBits); // root thread tcb
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਬਿੱਟ); // ipc buf
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ); // boot info
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ASIDPool_ਬਿੱਟ);
    ਮਾਪ += extra_bi_ਮਾਪ_ਬਿੱਟ > 0 ? ਦੋਆਧਾਰੀ_ਅੰਕ(extra_bi_ਮਾਪ_ਬਿੱਟ) : 0;
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_VSpace_ਬਿੱਟ); // root vspace
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਮਾਪ += ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_Min_Sched_Context_ਬਿੱਟ); // root sched context
#ਜੇ_ਅੰਤ
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    ਵਾਪਸ ਮਾਪ + arch_get_n_paging(it_v_reg) * ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ maybe_alloc_extra_bi(ਸ਼ਬਦ_t cmp_ਮਾਪ_ਬਿੱਟ, ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    ਜੇ (extra_bi_ਮਾਪ_ਬਿੱਟ >= cmp_ਮਾਪ_ਬਿੱਟ && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_ਮਾਪ_ਬਿੱਟ, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ create_rootserver_objects(pptr_t ਸ਼ੁਰੂ, v_region_t it_v_reg,
                                                ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    ਸ਼ਬਦ_t cnode_ਮਾਪ_ਬਿੱਟ = CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ + se_L4_ਥਾਂ_ਬਿੱਟ;
    ਸ਼ਬਦ_t max = rootserver_max_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ_ਬਿੱਟ);

    ਸ਼ਬਦ_t ਮਾਪ = calculate_rootserver_ਮਾਪ(it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
    rootserver_mem.ਸ਼ੁਰੂ = ਸ਼ੁਰੂ;
    rootserver_mem.ਸਿਰਾ = ਸ਼ੁਰੂ + ਮਾਪ;

    maybe_alloc_extra_bi(max, extra_bi_ਮਾਪ_ਬਿੱਟ);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#ਜੇ (CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ + se_L4_ਥਾਂ_ਬਿੱਟ) > se_L4_VSpace_ਬਿੱਟ
    rootserver.cnode = alloc_rootserver_obj(cnode_ਮਾਪ_ਬਿੱਟ, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_ਬਿੱਟ, extra_bi_ਮਾਪ_ਬਿੱਟ);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_ਬਿੱਟ, 1);
#ਨਹੀਂਤਾਂ
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_ਬਿੱਟ, 1);
    maybe_alloc_extra_bi(cnode_ਮਾਪ_ਬਿੱਟ, extra_bi_ਮਾਪ_ਬਿੱਟ);
    rootserver.cnode = alloc_rootserver_obj(cnode_ਮਾਪ_ਬਿੱਟ, 1);
#ਜੇ_ਅੰਤ

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_ਪੰਨਾ_ਬਿੱਟ, extra_bi_ਮਾਪ_ਬਿੱਟ);
    compile_assert(invalid_se_L4_ASIDPool_ਬਿੱਟ, se_L4_ASIDPool_ਬਿੱਟ == se_L4_ਪੰਨਾ_ਬਿੱਟ);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_ਬਿੱਟ, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_ਪੰਨਾ_ਬਿੱਟ, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ, se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ == se_L4_ਪੰਨਾ_ਬਿੱਟ);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#ਜੇ seL4_TCBBits >= se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#ਜੇ_ਅੰਤ

    /* paging structures are 4k on every arch except aarch32 (1k) */
    ਸ਼ਬਦ_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.ਸ਼ੁਰੂ = alloc_rootserver_obj(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ, n);
    rootserver.paging.ਸਿਰਾ = rootserver.paging.ਸ਼ੁਰੂ + n * ਦੋਆਧਾਰੀ_ਅੰਕ(se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);

    /* for most archs, TCBs are smaller than page tables */
#ਜੇ seL4_TCBBits < se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_ਬਿੱਟ, 1);
#ਜੇ_ਅੰਤ
    /* we should have allocated all our memory */
    ਪ੍ਰਤਿਜ੍ਞਾ(rootserver_mem.ਸ਼ੁਰੂ == rootserver_mem.ਸਿਰਾ);
}

BOOT_ਕੋਡ ਖਾਲੀ write_ਥਾਂ(ਥਾਂ_ptr_t ਥਾਂ_ptr, cap_t cap)
{
    ਥਾਂ_ptr->cap = cap;

    ਥਾਂ_ptr->cteMDBNode = nullMDBNode;
    mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_Revocable(&ਥਾਂ_ptr->cteMDBNode, true);
    mdb_ਗੰਢ_ptr_ਸੈੱਟ_mdb_ਪਹਿਲਾ_Badged(&ਥਾਂ_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(ਜੜ੍ਹ_cnode_ਮਾਪ_valid,
               CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ < 32 - se_L4_ਥਾਂ_ਬਿੱਟ &&
               ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ) >= se_L4_ਅੰਕ_Initial_Caps &&
               CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ >= (se_L4_ਪੰਨਾ_ਬਿੱਟ - se_L4_ਥਾਂ_ਬਿੱਟ))

BOOT_ਕੋਡ cap_t
create_ਜੜ੍ਹ_cnode(ਖਾਲੀ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ, /* radix */
                    ਸ਼ਬਦ_ਬਿੱਟ - CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_ਥਾਂ(ਥਾਂ_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    ਵਾਪਸ cap;
}

/* Check domain scheduler assumptions. */
compile_assert(ਅੰਕ_domains_valid,
               CONFIG_ਅੰਕ_DOMAINS >= 1 && CONFIG_ਅੰਕ_DOMAINS <= 256)
compile_assert(ਅੰਕ_priorities_valid,
               CONFIG_ਅੰਕ_PRIORITIES >= 1 && CONFIG_ਅੰਕ_PRIORITIES <= 256)

BOOT_ਕੋਡ ਖਾਲੀ
create_domain_cap(cap_t ਜੜ੍ਹ_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapDomain), cap);
}

BOOT_ਕੋਡ cap_t create_ipcbuf_ਚੌਖਟਾ_cap(cap_t ਜੜ੍ਹ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)rootserver.ipc_buf, ਪੰਨਾ_ਬਿੱਟ);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_ਚੌਖਟਾ_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    ਵਾਪਸ cap;
}

BOOT_ਕੋਡ ਖਾਲੀ create_bi_ਚੌਖਟਾ_cap(cap_t ਜੜ੍ਹ_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_ਚੌਖਟਾ_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), se_L4_Cap_Boot_Info_ਚੌਖਟਾ), cap);
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
BOOT_ਕੋਡ ਸ਼ਬਦ_t calculate_extra_bi_ਮਾਪ_ਬਿੱਟ(ਸ਼ਬਦ_t extra_ਮਾਪ)
{
    ਜੇ (extra_ਮਾਪ == 0) {
        ਵਾਪਸ 0;
    }

    ਸ਼ਬਦ_t clzl_ret = clzl(ਗੇੜਾ_ਉੱਪਰ(extra_ਮਾਪ, se_L4_ਪੰਨਾ_ਬਿੱਟ));
    ਸ਼ਬਦ_t msb = se_L4_ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    ਜੇ (extra_ਮਾਪ > ਦੋਆਧਾਰੀ_ਅੰਕ(msb)) {
        msb++;
    }
    ਵਾਪਸ msb;
}

BOOT_ਕੋਡ ਖਾਲੀ populate_bi_ਚੌਖਟਾ(ਗੰਢ_id_t ਗੰਢ_id, ਸ਼ਬਦ_t ਅੰਕ_nodes,
                                 vptr_t ipcbuf_vptr, ਸ਼ਬਦ_t extra_bi_ਮਾਪ)
{
    /* clear boot info memory */
    clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)rootserver.boot_info, se_L4_Boot_Info_ਚੌਖਟਾ_ਬਿੱਟ);
    ਜੇ (extra_bi_ਮਾਪ) {
        clear_ਸਮ੍ਰਿਤੀ((ਖਾਲੀ *)rootserver.extra_bi,
                    calculate_extra_bi_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->ਗੰਢ_ID = ਗੰਢ_id;
    bi->ਅੰਕ_Nodes = ਅੰਕ_nodes;
    bi->ਅੰਕ_IOPTLevels = 0;
    bi->ipc_ਬਫਰ = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_ਮਾਪ_ਬਿੱਟ = CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_ਮਾਪ;

    ndks_boot.bi_ਚੌਖਟਾ = bi;
    ndks_boot.ਥਾਂ_pos_cur = se_L4_ਅੰਕ_Initial_Caps;
}

BOOT_ਕੋਡ bool_t provide_cap(cap_t ਜੜ੍ਹ_cnode_cap, cap_t cap)
{
    ਜੇ (ndks_boot.ਥਾਂ_pos_cur >= ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("ERROR: can't add another cap, all %"SEL4_PRIu_ਸ਼ਬਦ
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ));
        ਵਾਪਸ false;
    }
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), ndks_boot.ਥਾਂ_pos_cur), cap);
    ndks_boot.ਥਾਂ_pos_cur++;
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ create_ਚੌਖਟੇ_ਦਾ_region_ret_t create_ਚੌਖਟੇ_ਦਾ_region(
    cap_t    ਜੜ੍ਹ_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_ਵਿੱਥ
)
{
    pptr_t     f;
    cap_t      ਚੌਖਟਾ_cap;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before;
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_after;

    ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;

    ਲਈ (f = reg.ਸ਼ੁਰੂ; f < reg.ਸਿਰਾ; f += ਦੋਆਧਾਰੀ_ਅੰਕ(ਪੰਨਾ_ਬਿੱਟ)) {
        ਜੇ (do_map) {
            ਚੌਖਟਾ_cap = create_mapped_it_ਚੌਖਟਾ_cap(pd_cap, f, pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)(f - pv_ਵਿੱਥ)), IT_ASID, false, true);
        } ਨਹੀਂਤਾਂ {
            ਚੌਖਟਾ_cap = create_unmapped_it_ਚੌਖਟਾ_cap(f, false);
        }
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, ਚੌਖਟਾ_cap)) {
            ਵਾਪਸ (create_ਚੌਖਟੇ_ਦਾ_region_ret_t) {
                .region  = S_REG_ਖਾਲੀ_ਥਾਵਾਂ,
                .success = false
            };
        }
    }

    ਥਾਂ_pos_after = ndks_boot.ਥਾਂ_pos_cur;

    ਵਾਪਸ (create_ਚੌਖਟੇ_ਦਾ_region_ret_t) {
        .region = (se_L4_ਥਾਂ_Region) {
            .ਸ਼ੁਰੂ = ਥਾਂ_pos_before,
            .ਸਿਰਾ   = ਥਾਂ_pos_after
        },
        .success = true
    };
}

BOOT_ਕੋਡ cap_t create_it_asid_pool(cap_t ਜੜ੍ਹ_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_ਉੱਚਾ(IT_ASID) << asid_ਨੀਵਾਂ_ਬਿੱਟ, rootserver.asid_pool);
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_ਥਾਂ(
        ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    ਵਾਪਸ ap_cap;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_ਕੋਡ bool_t init_sched_control(cap_t ਜੜ੍ਹ_cnode_cap, ਸ਼ਬਦ_t ਅੰਕ_nodes)
{
    se_L4_ਥਾਂ_Pos ਥਾਂ_pos_before = ndks_boot.ਥਾਂ_pos_cur;

    /* create a sched control cap for each core */
    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < ਅੰਕ_nodes; i++) {
        ਜੇ (!provide_cap(ਜੜ੍ਹ_cnode_cap, cap_sched_control_cap_new(i))) {
            ਲਿਖੋ("can't init sched_control for node %u, provide_cap() failed\n", i);
            ਵਾਪਸ false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_ਚੌਖਟਾ->schedcontrol = (se_L4_ਥਾਂ_Region) {
        .ਸ਼ੁਰੂ = ਥਾਂ_pos_before,
        .ਸਿਰਾ = ndks_boot.ਥਾਂ_pos_cur
    };

    ਵਾਪਸ true;
}
#ਜੇ_ਅੰਤ

BOOT_ਕੋਡ ਖਾਲੀ create_idle_thread(ਖਾਲੀ)
{
    pptr_t pptr;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < CONFIG_MAX_ਅੰਕ_NODES; i++) {
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        ਗੰਢ_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_ਵਿੱਥ);
        configureIdleThread(ਗੰਢ_STATE_ON_CORE(ksIdleThread, i));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
        ਸੈੱਟ_Thread_Name(ਗੰਢ_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#ਜੇ_ਅੰਤ
        SMP_COND_STATEMENT(ਗੰਢ_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
        configure_sched_context(ਗੰਢ_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(ਗੰਢ_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        ਗੰਢ_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
    }
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */
}

BOOT_ਕੋਡ tcb_t *create_initial_thread(cap_t ਜੜ੍ਹ_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_ਇੰਦਰਾਜ, vptr_t bi_ਚੌਖਟਾ_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_ਵਿੱਥ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    tcb->tcb_ਸਮਾਂ_ਟੁਕੜਾ = CONFIG_ਸਮਾਂ_ਟੁਕੜਾ;
#ਜੇ_ਅੰਤ

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    ਜੇ (dc_ret.ਸਥਿਤੀ != EXCEPTION_NONE) {
        ਲਿਖੋ("Failed to derive copy of IPC Buffer\n");
        ਵਾਪਸ ਨਿਰਾ;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_ਪਾਓ(
        ਜੜ੍ਹ_cnode_cap,
        ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadCNode),
        ਥਾਂ_PTR(rootserver.tcb, tcbCTable)
    );
    cte_ਪਾਓ(
        it_pd_cap,
        ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadVSpace),
        ਥਾਂ_PTR(rootserver.tcb, tcbVTable)
    );
    cte_ਪਾਓ(
        dc_ret.cap,
        ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadIPCBuffer),
        ਥਾਂ_PTR(rootserver.tcb, tcb_ਬਫਰ)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    ਸੈੱਟ_ਰਜਿਸਟਰ(tcb, cap_ਰਜਿਸਟਰ, bi_ਚੌਖਟਾ_vptr);
    ਸੈੱਟ_ਅਗਲਾ_PC(tcb, ui_v_ਇੰਦਰਾਜ);

    /* initialise TCB */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_ਮੰਜ਼ਿਲ_Ticks(CONFIG_BOOT_THREAD_ਸਮਾਂ_ਟੁਕੜਾ * US_ਵਿੱਚ_MS));
#ਜੇ_ਅੰਤ

    tcb->tcb_ਤਰਜੀਹ = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਤਿਆਰੀ_ਜਵਾਬ_Master(tcb);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_Thread_State(tcb, ThreadState_Running);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#ਜੇ_ਅੰਤ

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadTCB), cap);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_ਬਿੱਟ);
    write_ਥਾਂ(ਥਾਂ_PTR(pptr_ਦਾ_cap(ਜੜ੍ਹ_cnode_cap), seL4_CapInitThreadSC), cap);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    ਸੈੱਟ_Thread_Name(tcb, "rootserver");
#ਜੇ_ਅੰਤ

    ਵਾਪਸ tcb;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_ਕੋਡ ਸਥਿਰ bool_t hypervisor_present(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    ਜੇ ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        ਵਾਪਸ true;
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ false;
}

BOOT_ਕੋਡ ਖਾਲੀ clock_sync_test(ਖਾਲੀ)
{
    ticks_t t, t0;
    ticks_t margin = us_ਮੰਜ਼ਿਲ_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    ਪ੍ਰਤਿਜ੍ਞਾ(getCurrentCPUIndex() != 0);
    t = ਗੰਢ_STATE_ON_CORE(ks_Cur_ਸਮਾਂ, 0);
    ਕਰੋ {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = ਗੰਢ_STATE_ON_CORE(ks_Cur_ਸਮਾਂ, 0);
    } ਜਦੋਂ (t0 == t);
    t = get_Current_ਸਮਾਂ();
    ਲਿਖੋ("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (ਪੂਰਨ)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    ਜੇ (hypervisor_present()) {
        ਲਿਖੋ("clock_sync_test[%d]: disabled, detected running as VM\n", (ਪੂਰਨ)getCurrentCPUIndex());
    } ਨਹੀਂਤਾਂ {
        ਪ੍ਰਤਿਜ੍ਞਾ(t0 <= margin + t && t <= t0 + margin);
    }
}
#ਜੇ_ਅੰਤ

BOOT_ਕੋਡ ਖਾਲੀ init_core_state(tcb_t *scheduler_action)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_HAVE_FPU
    ਗੰਢ_STATE(ksCurFPUOwner) = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    ਗੰਢ_STATE(ksDebugTCBs) = ਨਿਰਾ;
    ਜੇ (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_ਚੁਣੋ_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(ਗੰਢ_STATE(ksIdleThread));
#ਜੇ_ਅੰਤ
    ਗੰਢ_STATE(ksSchedulerAction) = scheduler_action;
    ਗੰਢ_STATE(ksCurThread) = ਗੰਢ_STATE(ksIdleThread);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_KERNEL_MCS
    ਗੰਢ_STATE(ksCurSC) = ਗੰਢ_STATE(ksCurThread->tcbSchedContext);
    ਗੰਢ_STATE(ksConsumed) = 0;
    ਗੰਢ_STATE(ksReprogram) = true;
    ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰ) = ਨਿਰਾ;
    ਗੰਢ_STATE(ks_Release_ਕਤਾਰ.ਸਿਰਾ) = ਨਿਰਾ;
    ਗੰਢ_STATE(ks_Cur_ਸਮਾਂ) = get_Current_ਸਮਾਂ();
#ਜੇ_ਅੰਤ
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_ਸਮਾਂ = DSCHED_MAX_DURATION;
    ks_Dom_ਸਮਾਂ_ਸਾਰਣੀ[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_ਕੋਡ ਸਥਿਰ bool_t pptr_ਵਿੱਚ_kernel_ਖਿੜਕੀ(pptr_t pptr)
{
    ਵਾਪਸ pptr >= PPTR_ਅਧਾਰ && pptr < PPTR_ਸਿਖਰ;
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
BOOT_ਕੋਡ ਸਥਿਰ bool_t provide_untyped_cap(
    cap_t      ਜੜ੍ਹ_cnode_cap,
    bool_t     device_ਸਮ੍ਰਿਤੀ_2,
    pptr_t     pptr,
    ਸ਼ਬਦ_t     ਮਾਪ_ਬਿੱਟ,
    se_L4_ਥਾਂ_Pos ਪਹਿਲਾ_untyped_ਥਾਂ
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    ਜੇ (ਮਾਪ_ਬਿੱਟ > se_L4_Max_Untyped_ਬਿੱਟ || ਮਾਪ_ਬਿੱਟ < se_L4_Min_Untyped_ਬਿੱਟ) {
        ਲਿਖੋ("Kernel init: Invalid untyped size %"SEL4_PRIu_ਸ਼ਬਦ"\n", ਮਾਪ_ਬਿੱਟ);
        ਵਾਪਸ false;
    }

    /* All cap ptrs must be aligned to object size */
    ਜੇ (!IS_ALIGNED(pptr, ਮਾਪ_ਬਿੱਟ)) {
        ਲਿਖੋ("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_ਸ਼ਬਦ")\n", (ਖਾਲੀ *)pptr, ਮਾਪ_ਬਿੱਟ);
        ਵਾਪਸ false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    ਜੇ (!device_ਸਮ੍ਰਿਤੀ_2 && !pptr_ਵਿੱਚ_kernel_ਖਿੜਕੀ(pptr)) {
        ਲਿਖੋ("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (ਖਾਲੀ *)pptr);
        ਵਾਪਸ false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    ਜੇ (!device_ਸਮ੍ਰਿਤੀ_2 && !pptr_ਵਿੱਚ_kernel_ਖਿੜਕੀ(pptr + ਨਕਾਬ(ਮਾਪ_ਬਿੱਟ))) {
        ਲਿਖੋ("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_ਸ਼ਬਦ")\n",
               (ਖਾਲੀ *)pptr, ਮਾਪ_ਬਿੱਟ);
        ਵਾਪਸ false;
    }

    ਸ਼ਬਦ_t i = ndks_boot.ਥਾਂ_pos_cur - ਪਹਿਲਾ_untyped_ਥਾਂ;
    ਜੇ (i < CONFIG_MAX_ਅੰਕ_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_ਚੌਖਟਾ->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_ਮੰਜ਼ਿਲ_paddr((ਖਾਲੀ *)pptr),
            .ਮਾਪ_ਬਿੱਟ_2 = ਮਾਪ_ਬਿੱਟ,
            .isDevice = device_ਸਮ੍ਰਿਤੀ_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_ਸੂਚਕਾਂਕ(ਮਾਪ_ਬਿੱਟ),
                                     device_ਸਮ੍ਰਿਤੀ_2, ਮਾਪ_ਬਿੱਟ, pptr);
        ret = provide_cap(ਜੜ੍ਹ_cnode_cap, ut_cap);
    } ਨਹੀਂਤਾਂ {
        ਲਿਖੋ("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    ਵਾਪਸ ret;
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
BOOT_ਕੋਡ ਸਥਿਰ bool_t create_untypeds_for_region(
    cap_t      ਜੜ੍ਹ_cnode_cap,
    bool_t     device_ਸਮ੍ਰਿਤੀ_2,
    region_t   reg,
    se_L4_ਥਾਂ_Pos ਪਹਿਲਾ_untyped_ਥਾਂ
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    ਜਦੋਂ (!is_reg_ਖਾਲੀ_ਥਾਵਾਂ(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਮਾਪ_ਬਿੱਟ = se_L4_ਸ਼ਬਦ_ਬਿੱਟ - 1 - clzl(reg.ਸਿਰਾ - reg.ਸ਼ੁਰੂ);
        /* The size can't exceed the largest possible untyped size. */
        ਜੇ (ਮਾਪ_ਬਿੱਟ > se_L4_Max_Untyped_ਬਿੱਟ) {
            ਮਾਪ_ਬਿੱਟ = se_L4_Max_Untyped_ਬਿੱਟ;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        ਜੇ (0 != reg.ਸ਼ੁਰੂ) {
            ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ align_ਬਿੱਟ = ctzl(reg.ਸ਼ੁਰੂ);
            ਜੇ (ਮਾਪ_ਬਿੱਟ > align_ਬਿੱਟ) {
                ਮਾਪ_ਬਿੱਟ = align_ਬਿੱਟ;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        ਜੇ (ਮਾਪ_ਬਿੱਟ >= se_L4_Min_Untyped_ਬਿੱਟ) {
            ਜੇ (!provide_untyped_cap(ਜੜ੍ਹ_cnode_cap, device_ਸਮ੍ਰਿਤੀ_2, reg.ਸ਼ੁਰੂ, ਮਾਪ_ਬਿੱਟ, ਪਹਿਲਾ_untyped_ਥਾਂ)) {
                ਵਾਪਸ false;
            }
        }
        reg.ਸ਼ੁਰੂ += ਦੋਆਧਾਰੀ_ਅੰਕ(ਮਾਪ_ਬਿੱਟ);
    }
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ bool_t create_untypeds(cap_t ਜੜ੍ਹ_cnode_cap)
{
    se_L4_ਥਾਂ_Pos ਪਹਿਲਾ_untyped_ਥਾਂ = ndks_boot.ਥਾਂ_pos_cur;

    paddr_t ਸ਼ੁਰੂ = 0;
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ndks_boot.resv_ਗਿਣਤੀ; i++) {
        ਜੇ (ਸ਼ੁਰੂ < ndks_boot.reserved[i].ਸ਼ੁਰੂ) {
            region_t reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg((p_region_t) {
                ਸ਼ੁਰੂ, ndks_boot.reserved[i].ਸ਼ੁਰੂ
            });
            ਜੇ (!create_untypeds_for_region(ਜੜ੍ਹ_cnode_cap, true, reg, ਪਹਿਲਾ_untyped_ਥਾਂ)) {
                ਲਿਖੋ("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") failed\n",
                       (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)i, reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ);
                ਵਾਪਸ false;
            }
        }

        ਸ਼ੁਰੂ = ndks_boot.reserved[i].ਸਿਰਾ;
    }

    ਜੇ (ਸ਼ੁਰੂ <= CONFIG_PADDR_USER_DEVICE_ਸਿਖਰ - 1) {
        region_t reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            ਸ਼ੁਰੂ, (paddr_t) CONFIG_PADDR_USER_DEVICE_ਸਿਖਰ
        });

        ਜੇ (!create_untypeds_for_region(ਜੜ੍ਹ_cnode_cap, true, reg, ਪਹਿਲਾ_untyped_ਥਾਂ)) {
            ਲਿਖੋ("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") failed\n",
                   reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ);
            ਵਾਪਸ false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_ਮੰਜ਼ਿਲ_pptr_reg(get_p_reg_kernel_img_boot());
    ਜੇ (!create_untypeds_for_region(ਜੜ੍ਹ_cnode_cap, false, boot_mem_reuse_reg, ਪਹਿਲਾ_untyped_ਥਾਂ)) {
        ਲਿਖੋ("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ"] failed\n",
               boot_mem_reuse_reg.ਸ਼ੁਰੂ, boot_mem_reuse_reg.ਸਿਰਾ);
        ਵਾਪਸ false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ARRAY_ਮਾਪ(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_ਖਾਲੀ_ਥਾਵਾਂ;
        ਜੇ (!create_untypeds_for_region(ਜੜ੍ਹ_cnode_cap, false, reg, ਪਹਿਲਾ_untyped_ਥਾਂ)) {
            ਲਿਖੋ("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") failed\n",
                   (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)i, reg.ਸ਼ੁਰੂ, reg.ਸਿਰਾ);
            ਵਾਪਸ false;
        }
    }

    ndks_boot.bi_ਚੌਖਟਾ->untyped = (se_L4_ਥਾਂ_Region) {
        .ਸ਼ੁਰੂ = ਪਹਿਲਾ_untyped_ਥਾਂ,
        .ਸਿਰਾ   = ndks_boot.ਥਾਂ_pos_cur
    };

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਖਾਲੀ bi_finalise(ਖਾਲੀ)
{

    ਜੇ (rootserver.paging.ਸ਼ੁਰੂ != rootserver.paging.ਸਿਰਾ) {
        ਲਿਖੋ("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.ਸਿਰਾ - rootserver.paging.ਸ਼ੁਰੂ) >> se_L4_ਪੰਨਾ_ਸਾਰਣੀ_ਬਿੱਟ);
    }

    ndks_boot.bi_ਚੌਖਟਾ->ਖਾਲੀ_ਥਾਵਾਂ = (se_L4_ਥਾਂ_Region) {
        .ਸ਼ੁਰੂ = ndks_boot.ਥਾਂ_pos_cur,
        .ਸਿਰਾ   = ਦੋਆਧਾਰੀ_ਅੰਕ(CONFIG_ਜੜ੍ਹ_CNODE_ਮਾਪ_ਬਿੱਟ)
    };
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t check_ਉਪਲਬਧ_ਸਮ੍ਰਿਤੀ(ਸ਼ਬਦ_t n_ਉਪਲਬਧ,
                                               ਅਚਲ p_region_t *ਉਪਲਬਧ)
{
    /* The system configuration is broken if no region is available. */
    ਜੇ (0 == n_ਉਪਲਬਧ) {
        ਲਿਖੋ("ERROR: no memory regions available\n");
        ਵਾਪਸ false;
    }

    ਲਿਖੋ("Available phys memory regions: %"SEL4_PRIu_ਸ਼ਬਦ"\n", n_ਉਪਲਬਧ);
    /* Force ordering and exclusivity of available regions. */
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < n_ਉਪਲਬਧ; i++) {
        ਅਚਲ p_region_t *r = &ਉਪਲਬਧ[i];
        ਲਿਖੋ("  [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ")\n", r->ਸ਼ੁਰੂ, r->ਸਿਰਾ);

        /* Available regions must not wrap */
        ਜੇ (r->ਸ਼ੁਰੂ > r->ਸਿਰਾ) {
            ਲਿਖੋ("ERROR: memory region %"SEL4_PRIu_ਸ਼ਬਦ" has start > end\n", i);
            ਵਾਪਸ false;
        }

        /* Available regions must be non-empty. */
        ਜੇ (r->ਸ਼ੁਰੂ == r->ਸਿਰਾ) {
            ਲਿਖੋ("ERROR: memory region %"SEL4_PRIu_ਸ਼ਬਦ" empty\n", i);
            ਵਾਪਸ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        ਜੇ ((i > 0) && (r->ਸ਼ੁਰੂ < ਉਪਲਬਧ[i - 1].ਸਿਰਾ)) {
            ਲਿਖੋ("ERROR: memory region %"SEL4_PRIu_ਸ਼ਬਦ" in wrong order\n", i);
            ਵਾਪਸ false;
        }
    }

    ਵਾਪਸ true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ਸਥਿਰ region_t avail_reg[MAX_ਅੰਕ_FREEMEM_REG];

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
BOOT_ਕੋਡ ਸਥਿਰ ਸ਼ਬਦ_t init_avail_reg(ਸ਼ਬਦ_t n_ਉਪਲਬਧ,
                                       ਅਚਲ p_region_t *ਉਪਲਬਧ)
{
    ਸ਼ਬਦ_t cnt = 0;
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < n_ਉਪਲਬਧ; i++) {
        ਅਚਲ p_region_t *r = &ਉਪਲਬਧ[i];

        ਜੇ ((r->ਸ਼ੁਰੂ >= PADDR_ਸਿਖਰ) || (r->ਸਿਰਾ <= PADDR_ਅਧਾਰ)) {
            ਲਿਖੋ("  region [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->ਸ਼ੁਰੂ, r->ਸਿਰਾ);
        } ਨਹੀਂਤਾਂ {
            p_region_t usable_reg = {
                .ਸ਼ੁਰੂ = MAX(r->ਸ਼ੁਰੂ, PADDR_ਅਧਾਰ),
                .ਸਿਰਾ   = MIN(r->ਸਿਰਾ, PADDR_ਸਿਖਰ)
            };

            ਜੇ (cnt >= ARRAY_ਮਾਪ(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                ਲਿਖੋ("  WARNING: cannot use region [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ"),"
                       " avail_reg[] is full\n", r->ਸ਼ੁਰੂ, r->ਸਿਰਾ);
                ਪ੍ਰਤਿਜ੍ਞਾ(0);
            } ਨਹੀਂਤਾਂ {
                avail_reg[cnt] = paddr_ਮੰਜ਼ਿਲ_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    ਵਾਪਸ cnt;
}


BOOT_ਕੋਡ ਸਥਿਰ bool_t check_reserved_ਸਮ੍ਰਿਤੀ(ਸ਼ਬਦ_t n_reserved,
                                              ਅਚਲ region_t *reserved)
{
    ਲਿਖੋ("Reserved virt address space regions: %"SEL4_PRIu_ਸ਼ਬਦ"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    ਲਈ (ਸ਼ਬਦ_t i = 0; i < n_reserved; i++) {
        ਅਚਲ region_t *r = &reserved[i];
        ਲਿਖੋ("  [%"SEL4_PRIx_ਸ਼ਬਦ"..%"SEL4_PRIx_ਸ਼ਬਦ")\n", r->ਸ਼ੁਰੂ, r->ਸਿਰਾ);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        ਜੇ (r->ਸ਼ੁਰੂ > r->ਸਿਰਾ) {
            ਲਿਖੋ("ERROR: reserved region %"SEL4_PRIu_ਸ਼ਬਦ" has start > end\n", i);
            ਵਾਪਸ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        ਜੇ ((i > 0) && (r->ਸ਼ੁਰੂ < reserved[i - 1].ਸਿਰਾ)) {
            ਲਿਖੋ("ERROR: reserved region %"SEL4_PRIu_ਸ਼ਬਦ" in wrong order\n", i);
            ਵਾਪਸ false;
        }
    }

    ਵਾਪਸ true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_ਕੋਡ bool_t init_freemem(ਸ਼ਬਦ_t n_ਉਪਲਬਧ, ਅਚਲ p_region_t *ਉਪਲਬਧ,
                              ਸ਼ਬਦ_t n_reserved, ਅਚਲ region_t *reserved,
                              v_region_t it_v_reg, ਸ਼ਬਦ_t extra_bi_ਮਾਪ_ਬਿੱਟ)
{
    ਜੇ (!check_ਉਪਲਬਧ_ਸਮ੍ਰਿਤੀ(n_ਉਪਲਬਧ, ਉਪਲਬਧ)) {
        ਵਾਪਸ false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_ਉਪਲਬਧ = init_avail_reg(n_ਉਪਲਬਧ, ਉਪਲਬਧ);
    ਜੇ (0 == n_ਉਪਲਬਧ) {
        ਲਿਖੋ("ERROR: no available memory within the kernel window\n");
        ਵਾਪਸ false;
    }

    ਜੇ (!check_reserved_ਸਮ੍ਰਿਤੀ(n_reserved, reserved)) {
        ਵਾਪਸ false;
    }

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < ARRAY_ਮਾਪ(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_ਖਾਲੀ_ਥਾਵਾਂ;
    }

    ਸ਼ਬਦ_t a = 0;
    ਸ਼ਬਦ_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    ਜਦੋਂ (a < n_ਉਪਲਬਧ && r < n_reserved) {
        ਜੇ (reserved[r].ਸ਼ੁਰੂ == reserved[r].ਸਿਰਾ) {
            /* reserved region is empty - skip it */
            r++;
        } ਨਹੀਂਤਾਂ ਜੇ (avail_reg[a].ਸ਼ੁਰੂ >= avail_reg[a].ਸਿਰਾ) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } ਨਹੀਂਤਾਂ ਜੇ (reserved[r].ਸਿਰਾ <= avail_reg[a].ਸ਼ੁਰੂ) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_ਮੰਜ਼ਿਲ_paddr_reg(reserved[r]));
            r++;
        } ਨਹੀਂਤਾਂ ਜੇ (reserved[r].ਸ਼ੁਰੂ >= avail_reg[a].ਸਿਰਾ) {
            /* the reserved region is above the available region - take the whole thing */
            ਪਾਓ_region(avail_reg[a]);
            a++;
        } ਨਹੀਂਤਾਂ {
            /* the reserved region overlaps with the available region */
            ਜੇ (reserved[r].ਸ਼ੁਰੂ <= avail_reg[a].ਸ਼ੁਰੂ) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].ਸ਼ੁਰੂ = MIN(avail_reg[a].ਸਿਰਾ, reserved[r].ਸਿਰਾ);
                /* do not increment reserved index here - there could be more overlapping regions */
            } ਨਹੀਂਤਾਂ {
                ਪ੍ਰਤਿਜ੍ਞਾ(reserved[r].ਸ਼ੁਰੂ < avail_reg[a].ਸਿਰਾ);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.ਸਿਰਾ = reserved[r].ਸ਼ੁਰੂ;
                ਪਾਓ_region(m);
                ਜੇ (avail_reg[a].ਸਿਰਾ > reserved[r].ਸਿਰਾ) {
                    avail_reg[a].ਸ਼ੁਰੂ = reserved[r].ਸਿਰਾ;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } ਨਹੀਂਤਾਂ {
                    a++;
                }
            }
        }
    }

    ਲਈ (; r < n_reserved; r++) {
        ਜੇ (reserved[r].ਸ਼ੁਰੂ < reserved[r].ਸਿਰਾ) {
            reserve_region(pptr_ਮੰਜ਼ਿਲ_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    ਲਈ (; a < n_ਉਪਲਬਧ; a++) {
        ਜੇ (avail_reg[a].ਸ਼ੁਰੂ < avail_reg[a].ਸਿਰਾ) {
            ਪਾਓ_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    ਪੂਰਨ i = ARRAY_ਮਾਪ(ndks_boot.freemem) - 1;
    ਜੇ (!is_reg_ਖਾਲੀ_ਥਾਵਾਂ(ndks_boot.freemem[i])) {
        ਲਿਖੋ("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)MAX_ਅੰਕ_FREEMEM_REG);
        ਵਾਪਸ false;
    }
    /* skip any empty regions */
    ਲਈ (; i >= 0 && is_reg_ਖਾਲੀ_ਥਾਵਾਂ(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    ਸ਼ਬਦ_t ਮਾਪ = calculate_rootserver_ਮਾਪ(it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
    ਸ਼ਬਦ_t max = rootserver_max_ਮਾਪ_ਬਿੱਟ(extra_bi_ਮਾਪ_ਬਿੱਟ);
    ਲਈ (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        ਪ੍ਰਤਿਜ੍ਞਾ(i < ARRAY_ਮਾਪ(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        ਪ੍ਰਤਿਜ੍ਞਾ(is_reg_ਖਾਲੀ_ਥਾਵਾਂ(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_ਸੂਚਕਾਂਕ = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_ਸ਼ੁਰੂ = ndks_boot.freemem[i].ਸਿਰਾ - ਮਾਪ;
        pptr_t ਸ਼ੁਰੂ = ਗੇੜਾ_ਥੱਲੇ(unaligned_ਸ਼ੁਰੂ, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        ਜੇ (unaligned_ਸ਼ੁਰੂ <= ndks_boot.freemem[i].ਸਿਰਾ
            && ਸ਼ੁਰੂ >= ndks_boot.freemem[i].ਸ਼ੁਰੂ) {
            create_rootserver_objects(ਸ਼ੁਰੂ, it_v_reg, extra_bi_ਮਾਪ_ਬਿੱਟ);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[ਖਾਲੀ_ਥਾਵਾਂ_ਸੂਚਕਾਂਕ] = (region_t) {
                .ਸ਼ੁਰੂ = ਸ਼ੁਰੂ + ਮਾਪ,
                .ਸਿਰਾ = ndks_boot.freemem[i].ਸਿਰਾ
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].ਸਿਰਾ = ਸ਼ੁਰੂ;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            ਵਾਪਸ true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[ਖਾਲੀ_ਥਾਵਾਂ_ਸੂਚਕਾਂਕ] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_ਖਾਲੀ_ਥਾਵਾਂ;
    }

    /* We didn't find a big enough region. */
    ਲਿਖੋ("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_ਸ਼ਬਦ"\n", max);
    ਵਾਪਸ false;
}
