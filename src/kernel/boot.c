/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <kernel/thread.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <machine/registerset.h>
#ಅನ್ತರ್ಭಾವ <model/statedata.h>
#ಅನ್ತರ್ಭಾವ <object/domain.h>
#ಅನ್ತರ್ಭಾವ <arch/machine.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/boot.h>
#ಅನ್ತರ್ಭಾವ <arch/kernel/vspace.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <hardware.h>
#ಅನ್ತರ್ಭಾವ <util.h>

/* (node-local) state accessed only during bootstrapping */
BOOT_BSS ndks_boot_t ndks_boot;

BOOT_BSS rootserver_mem_t rootserver;
BOOT_BSS ಸ್ಥಿರ region_t rootserver_mem;

/* Returns the physical region of the kernel image boot part, which is the part
 * that is no longer needed once booting is finished. */
ಬಾಹ್ಯ ಅಕ್ಷರ ki_boot_ಕೊನೆ[1];
BOOT_ಸಂಕೇತ p_region_t get_p_reg_kernel_img_boot(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (p_region_t) {
        .ಆರಂಭ = kpptr_ಗಮ್ಯ_paddr((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)KERNEL_ELF_ತಳ),
        .ಕೊನೆ   = kpptr_ಗಮ್ಯ_paddr(ki_boot_ಕೊನೆ)
    };
}

/* Returns the physical region of the kernel image. */
BOOT_ಸಂಕೇತ p_region_t get_p_reg_kernel_img(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (p_region_t) {
        .ಆರಂಭ = kpptr_ಗಮ್ಯ_paddr((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)KERNEL_ELF_ತಳ),
        .ಕೊನೆ   = kpptr_ಗಮ್ಯ_paddr((ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *)KERNEL_ELF_ತುದಿ)
    };
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ ವಿಲೀನ_regions(ಶೂನ್ಯ)
{
    /* Walk through reserved regions and see if any can be merged */
    ಶಬ್ದ_t ತುದಿ = 0;
    ಗಾಗಿ (ಶಬ್ದ_t i = 1; i < ndks_boot.resv_ಎಣಿಕೆ; i++) {
        ಆದರೆ (ndks_boot.reserved[ತುದಿ].ಕೊನೆ == ndks_boot.reserved[i].ಆರಂಭ) {
            /* extend earlier region */
            ndks_boot.reserved[ತುದಿ].ಕೊನೆ = ndks_boot.reserved[i].ಕೊನೆ;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* add region */
            ತುದಿ++;
            ndks_boot.reserved[ತುದಿ] = ndks_boot.reserved[i];
        }
    }
    ndks_boot.resv_ಎಣಿಕೆ = ndks_boot.resv_ಎಣಿಕೆ > 0 ? ತುದಿ + 1 : 0;
}

BOOT_ಸಂಕೇತ bool_t reserve_region(p_region_t reg)
{
    ಶಬ್ದ_t i;
    ಪ್ರತಿಜ್ಞಾ(reg.ಆರಂಭ <= reg.ಕೊನೆ);
    ಆದರೆ (reg.ಆರಂಭ == reg.ಕೊನೆ) {
        ಹಿಂತಿರುಗಿ true;
    }

    /* keep the regions in order */
    ಗಾಗಿ (i = 0; i < ndks_boot.resv_ಎಣಿಕೆ; i++) {
        /* Try and merge the region to an existing one, if possible */
        ಆದರೆ (ndks_boot.reserved[i].ಆರಂಭ == reg.ಕೊನೆ) {
            ndks_boot.reserved[i].ಆರಂಭ = reg.ಆರಂಭ;
            ವಿಲೀನ_regions();
            ಹಿಂತಿರುಗಿ true;
        }
        ಆದರೆ (ndks_boot.reserved[i].ಕೊನೆ == reg.ಆರಂಭ) {
            ndks_boot.reserved[i].ಕೊನೆ = reg.ಕೊನೆ;
            ವಿಲೀನ_regions();
            ಹಿಂತಿರುಗಿ true;
        }
        /* Otherwise figure out where it should go. */
        ಆದರೆ (ndks_boot.reserved[i].ಆರಂಭ > reg.ಕೊನೆ) {
            /* move regions down, making sure there's enough room */
            ಆದರೆ (ndks_boot.resv_ಎಣಿಕೆ + 1 >= MAX_ಅಂಕೆ_RESV_REG) {
                ಬರೆ("Can't mark region 0x%"SEL4_PRIx_ಶಬ್ದ"-0x%"SEL4_PRIx_ಶಬ್ದ
                       " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
                       reg.ಆರಂಭ, reg.ಕೊನೆ, (ಪೂರ್ಣಾಂಕ)MAX_ಅಂಕೆ_RESV_REG);
                ಹಿಂತಿರುಗಿ false;
            }
            ಗಾಗಿ (ಶಬ್ದ_t j = ndks_boot.resv_ಎಣಿಕೆ; j > i; j--) {
                ndks_boot.reserved[j] = ndks_boot.reserved[j - 1];
            }
            /* insert the new region */
            ndks_boot.reserved[i] = reg;
            ndks_boot.resv_ಎಣಿಕೆ++;
            ಹಿಂತಿರುಗಿ true;
        }
    }

    ಆದರೆ (i + 1 == MAX_ಅಂಕೆ_RESV_REG) {
        ಬರೆ("Can't mark region 0x%"SEL4_PRIx_ಶಬ್ದ"-0x%"SEL4_PRIx_ಶಬ್ದ
               " as reserved, try increasing MAX_NUM_RESV_REG (currently %d)\n",
               reg.ಆರಂಭ, reg.ಕೊನೆ, (ಪೂರ್ಣಾಂಕ)MAX_ಅಂಕೆ_RESV_REG);
        ಹಿಂತಿರುಗಿ false;
    }

    ndks_boot.reserved[i] = reg;
    ndks_boot.resv_ಎಣಿಕೆ++;

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t ಸೇರಿಸು_region(region_t reg)
{
    ಪ್ರತಿಜ್ಞಾ(reg.ಆರಂಭ <= reg.ಕೊನೆ);
    ಆದರೆ (is_reg_ಖಾಲಿಗಳು(reg)) {
        ಹಿಂತಿರುಗಿ true;
    }

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ARRAY_ಗಾತ್ರ(ndks_boot.freemem); i++) {
        ಆದರೆ (is_reg_ಖಾಲಿಗಳು(ndks_boot.freemem[i])) {
            reserve_region(pptr_ಗಮ್ಯ_paddr_reg(reg));
            ndks_boot.freemem[i] = reg;
            ಹಿಂತಿರುಗಿ true;
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
    ಬರೆ("no free memory slot left for [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ"),"
           " consider increasing MAX_NUM_FREEMEM_REG (%u)\n",
           reg.ಆರಂಭ, reg.ಕೊನೆ, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MAX_ಅಂಕೆ_FREEMEM_REG);

    /* For debug builds we consider this a fatal error. Rationale is, that the
     * caller does not check the error code at the moment, but just ignores any
     * failures silently. */
    ಪ್ರತಿಜ್ಞಾ(0);

    ಹಿಂತಿರುಗಿ false;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ pptr_t alloc_rootserver_obj(ಶಬ್ದ_t ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, ಶಬ್ದ_t n)
{
    pptr_t allocated = rootserver_mem.ಆರಂಭ;
    /* allocated memory must be aligned */
    ಪ್ರತಿಜ್ಞಾ(allocated % ಬಿಟ್(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) == 0);
    rootserver_mem.ಆರಂಭ += (n * ಬಿಟ್(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು));
    /* we must not have run out of memory */
    ಪ್ರತಿಜ್ಞಾ(rootserver_mem.ಆರಂಭ <= rootserver_mem.ಕೊನೆ);
    memzero((ಶೂನ್ಯ *) allocated, n * ಬಿಟ್(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು));
    ಹಿಂತಿರುಗಿ allocated;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶಬ್ದ_t rootserver_max_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    ಶಬ್ದ_t cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು;
    ಶಬ್ದ_t max = MAX(cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, se_L4_VSpace_ಬಿಟ್ಟುಗಳು);
    ಹಿಂತಿರುಗಿ MAX(max, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶಬ್ದ_t calculate_rootserver_ಗಾತ್ರ(v_region_t it_v_reg, ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    /* work out how much memory we need for root server objects */
    ಶಬ್ದ_t ಗಾತ್ರ_2 = ಬಿಟ್(CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು);
    ಗಾತ್ರ_2 += ಬಿಟ್(seL4_TCBBits); // root thread tcb
    ಗಾತ್ರ_2 += ಬಿಟ್(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು); // ipc buf
    ಗಾತ್ರ_2 += ಬಿಟ್(se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು); // boot info
    ಗಾತ್ರ_2 += ಬಿಟ್(se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು);
    ಗಾತ್ರ_2 += extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > 0 ? ಬಿಟ್(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) : 0;
    ಗಾತ್ರ_2 += ಬಿಟ್(se_L4_VSpace_ಬಿಟ್ಟುಗಳು); // root vspace
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಾತ್ರ_2 += ಬಿಟ್(se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು); // root sched context
#ಆದರೆ_ಅಂತ್ಯ
    /* for all archs, seL4_PageTable Bits is the size of all non top-level paging structures */
    ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2 + arch_get_n_paging(it_v_reg) * ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ maybe_alloc_extra_bi(ಶಬ್ದ_t cmp_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    ಆದರೆ (extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು >= cmp_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು && rootserver.extra_bi == 0) {
        rootserver.extra_bi = alloc_rootserver_obj(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, 1);
    }
}

/* Create pptrs for all root server objects, starting at a give start address,
 * to cover the virtual memory region v_reg, and any extra boot info.
 */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ create_rootserver_objects(pptr_t ಆರಂಭ, v_region_t it_v_reg,
                                                ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    /* the largest object the PD, the root cnode, or the extra boot info */
    ಶಬ್ದ_t cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು;
    ಶಬ್ದ_t max = rootserver_max_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);

    ಶಬ್ದ_t ಗಾತ್ರ_2 = calculate_rootserver_ಗಾತ್ರ(it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    rootserver_mem.ಆರಂಭ = ಆರಂಭ;
    rootserver_mem.ಕೊನೆ = ಆರಂಭ + ಗಾತ್ರ_2;

    maybe_alloc_extra_bi(max, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);

    /* the root cnode is at least 4k, so it could be larger or smaller than a pd. */
#ಆದರೆ (CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು + se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು) > se_L4_VSpace_ಬಿಟ್ಟುಗಳು
    rootserver.cnode = alloc_rootserver_obj(cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, 1);
    maybe_alloc_extra_bi(se_L4_VSpace_ಬಿಟ್ಟುಗಳು, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_ಬಿಟ್ಟುಗಳು, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    rootserver.vspace = alloc_rootserver_obj(se_L4_VSpace_ಬಿಟ್ಟುಗಳು, 1);
    maybe_alloc_extra_bi(cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    rootserver.cnode = alloc_rootserver_obj(cnode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, 1);
#ಆದರೆ_ಅಂತ್ಯ

    /* at this point we are up to creating 4k objects - which is the min size of
     * extra_bi so this is the last chance to allocate it */
    maybe_alloc_extra_bi(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    compile_assert(invalid_se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು, se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    rootserver.asid_pool = alloc_rootserver_obj(se_L4_ASIDPool_ಬಿಟ್ಟುಗಳು, 1);
    rootserver.ipc_buf = alloc_rootserver_obj(se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು, 1);
    /* The boot info size must be at least one page. Due to the hard-coded order
     * of allocations used in the current implementation here, it can't be any
     * bigger.
     */
    compile_assert(invalid_se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು, se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು == se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    rootserver.boot_info = alloc_rootserver_obj(se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು, 1);

    /* TCBs on aarch32 can be larger than page tables in certain configs */
#ಆದರೆ seL4_TCBBits >= se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#ಆದರೆ_ಅಂತ್ಯ

    /* paging structures are 4k on every arch except aarch32 (1k) */
    ಶಬ್ದ_t n = arch_get_n_paging(it_v_reg);
    rootserver.paging.ಆರಂಭ = alloc_rootserver_obj(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು, n);
    rootserver.paging.ಕೊನೆ = rootserver.paging.ಆರಂಭ + n * ಬಿಟ್(se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);

    /* for most archs, TCBs are smaller than page tables */
#ಆದರೆ seL4_TCBBits < se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು
    rootserver.tcb = alloc_rootserver_obj(seL4_TCBBits, 1);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    rootserver.sc = alloc_rootserver_obj(se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು, 1);
#ಆದರೆ_ಅಂತ್ಯ
    /* we should have allocated all our memory */
    ಪ್ರತಿಜ್ಞಾ(rootserver_mem.ಆರಂಭ == rootserver_mem.ಕೊನೆ);
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ write_ಸ್ಥಾನ(ಸ್ಥಾನ_ptr_t ಸ್ಥಾನ_ptr, cap_t cap)
{
    ಸ್ಥಾನ_ptr->cap = cap;

    ಸ್ಥಾನ_ptr->cteMDBNode = nullMDBNode;
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_Revocable(&ಸ್ಥಾನ_ptr->cteMDBNode, true);
    mdb_ಗಂಟು_ptr_ಗಣ_mdb_ಮೊದಲ_Badged(&ಸ್ಥಾನ_ptr->cteMDBNode, true);
}

/* Our root CNode needs to be able to fit all the initial caps and not
 * cover all of memory.
 */
compile_assert(ಬೇರು_cnode_ಗಾತ್ರ_valid,
               CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು < 32 - se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು &&
               ಬಿಟ್(CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು) >= se_L4_ಅಂಕೆ_Initial_Caps &&
               CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು >= (se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು - se_L4_ಸ್ಥಾನ_ಬಿಟ್ಟುಗಳು))

BOOT_ಸಂಕೇತ cap_t
create_ಬೇರು_cnode(ಶೂನ್ಯ)
{
    cap_t cap = cap_cnode_cap_new(
                    CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, /* radix */
                    ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, /* guard size */
                    0, /* guard */
                    rootserver.cnode); /* pptr */

    /* write the root CNode cap into the root CNode */
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(rootserver.cnode, seL4_CapInitThreadCNode), cap);

    ಹಿಂತಿರುಗಿ cap;
}

/* Check domain scheduler assumptions. */
compile_assert(ಅಂಕೆ_domains_valid,
               CONFIG_ಅಂಕೆ_DOMAINS >= 1 && CONFIG_ಅಂಕೆ_DOMAINS <= 256)
compile_assert(ಅಂಕೆ_priorities_valid,
               CONFIG_ಅಂಕೆ_PRIORITIES >= 1 && CONFIG_ಅಂಕೆ_PRIORITIES <= 256)

BOOT_ಸಂಕೇತ ಶೂನ್ಯ
create_domain_cap(cap_t ಬೇರು_cnode_cap)
{
    cap_t cap = cap_domain_cap_new();
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapDomain), cap);
}

BOOT_ಸಂಕೇತ cap_t create_ipcbuf_ಚೌಕಟ್ಟು_cap(cap_t ಬೇರು_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)rootserver.ipc_buf, ಪುಟ_ಬಿಟ್ಟುಗಳು);

    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_ಚೌಕಟ್ಟು_cap(pd_cap, rootserver.ipc_buf, vptr, IT_ASID, false, false);
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadIPCBuffer), cap);

    ಹಿಂತಿರುಗಿ cap;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ create_bi_ಚೌಕಟ್ಟು_cap(cap_t ಬೇರು_cnode_cap, cap_t pd_cap, vptr_t vptr)
{
    /* create a cap of it and write it into the root CNode */
    cap_t cap = create_mapped_it_ಚೌಕಟ್ಟು_cap(pd_cap, rootserver.boot_info, vptr, IT_ASID, false, false);
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), se_L4_Cap_Boot_Info_ಚೌಕಟ್ಟು), cap);
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
BOOT_ಸಂಕೇತ ಶಬ್ದ_t calculate_extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(ಶಬ್ದ_t extra_ಗಾತ್ರ)
{
    ಆದರೆ (extra_ಗಾತ್ರ == 0) {
        ಹಿಂತಿರುಗಿ 0;
    }

    ಶಬ್ದ_t clzl_ret = clzl(ಸುತ್ತು_ಮೇಲೆ(extra_ಗಾತ್ರ, se_L4_ಪುಟ_ಬಿಟ್ಟುಗಳು));
    ಶಬ್ದ_t msb = se_L4_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl_ret;
    /* If region is bigger than a page, make sure we overallocate rather than
     * underallocate
     */
    ಆದರೆ (extra_ಗಾತ್ರ > ಬಿಟ್(msb)) {
        msb++;
    }
    ಹಿಂತಿರುಗಿ msb;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ populate_bi_ಚೌಕಟ್ಟು(ಗಂಟು_id_t ಗಂಟು_id, ಶಬ್ದ_t ಅಂಕೆ_nodes,
                                 vptr_t ipcbuf_vptr, ಶಬ್ದ_t extra_bi_ಗಾತ್ರ)
{
    /* clear boot info memory */
    clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)rootserver.boot_info, se_L4_Boot_Info_ಚೌಕಟ್ಟು_ಬಿಟ್ಟುಗಳು);
    ಆದರೆ (extra_bi_ಗಾತ್ರ) {
        clear_ಸ್ಮರಣೆ((ಶೂನ್ಯ *)rootserver.extra_bi,
                    calculate_extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ));
    }

    /* initialise bootinfo-related global state */
    seL4_BootInfo *bi = BI_PTR(rootserver.boot_info);
    bi->ಗಂಟು_ID = ಗಂಟು_id;
    bi->ಅಂಕೆ_Nodes = ಅಂಕೆ_nodes;
    bi->ಅಂಕೆ_IOPTLevels = 0;
    bi->ipc_ಬಫರ್ = (seL4_IPCBuffer *)ipcbuf_vptr;
    bi->init_Thread_CNode_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು;
    bi->initThreadDomain = 0;
    bi->extraLen = extra_bi_ಗಾತ್ರ;

    ndks_boot.bi_ಚೌಕಟ್ಟು = bi;
    ndks_boot.ಸ್ಥಾನ_pos_cur = se_L4_ಅಂಕೆ_Initial_Caps;
}

BOOT_ಸಂಕೇತ bool_t provide_cap(cap_t ಬೇರು_cnode_cap, cap_t cap)
{
    ಆದರೆ (ndks_boot.ಸ್ಥಾನ_pos_cur >= ಬಿಟ್(CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("ERROR: can't add another cap, all %"SEL4_PRIu_ಶಬ್ದ
               " (=2^CONFIG_ROOT_CNODE_SIZE_BITS) slots used\n",
               ಬಿಟ್(CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು));
        ಹಿಂತಿರುಗಿ false;
    }
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), ndks_boot.ಸ್ಥಾನ_pos_cur), cap);
    ndks_boot.ಸ್ಥಾನ_pos_cur++;
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t create_ಚೌಕಟ್ಟುಗಳು_ದ_region(
    cap_t    ಬೇರು_cnode_cap,
    cap_t    pd_cap,
    region_t reg,
    bool_t   do_map,
    sword_t  pv_ಸ್ಥಾನಾಂತರ
)
{
    pptr_t     f;
    cap_t      ಚೌಕಟ್ಟು_cap;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before;
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_after;

    ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;

    ಗಾಗಿ (f = reg.ಆರಂಭ; f < reg.ಕೊನೆ; f += ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) {
        ಆದರೆ (do_map) {
            ಚೌಕಟ್ಟು_cap = create_mapped_it_ಚೌಕಟ್ಟು_cap(pd_cap, f, pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)(f - pv_ಸ್ಥಾನಾಂತರ)), IT_ASID, false, true);
        } ಇಲ್ಲದಿದ್ದರೆ {
            ಚೌಕಟ್ಟು_cap = create_unmapped_it_ಚೌಕಟ್ಟು_cap(f, false);
        }
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, ಚೌಕಟ್ಟು_cap)) {
            ಹಿಂತಿರುಗಿ (create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t) {
                .region  = S_REG_ಖಾಲಿಗಳು,
                .success = false
            };
        }
    }

    ಸ್ಥಾನ_pos_after = ndks_boot.ಸ್ಥಾನ_pos_cur;

    ಹಿಂತಿರುಗಿ (create_ಚೌಕಟ್ಟುಗಳು_ದ_region_ret_t) {
        .region = (se_L4_ಸ್ಥಾನ_Region) {
            .ಆರಂಭ = ಸ್ಥಾನ_pos_before,
            .ಕೊನೆ   = ಸ್ಥಾನ_pos_after
        },
        .success = true
    };
}

BOOT_ಸಂಕೇತ cap_t create_it_asid_pool(cap_t ಬೇರು_cnode_cap)
{
    cap_t ap_cap = cap_asid_pool_cap_new(ASID_ಮೇಲಿನ(IT_ASID) << asid_ಕೆಳಗಿನ_ಬಿಟ್ಟುಗಳು, rootserver.asid_pool);
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadASIDPool), ap_cap);

    /* create ASID control cap */
    write_ಸ್ಥಾನ(
        ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapASIDControl),
        cap_asid_control_cap_new()
    );

    ಹಿಂತಿರುಗಿ ap_cap;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ configure_sched_context(tcb_t *tcb, sched_context_t *sc_pptr, ticks_t timeslice)
{
    tcb->tcbSchedContext = sc_pptr;
    refill_new(tcb->tcbSchedContext, MIN_REFILLS, timeslice, 0);
    tcb->tcbSchedContext->scTcb = tcb;
}

BOOT_ಸಂಕೇತ bool_t init_sched_control(cap_t ಬೇರು_cnode_cap, ಶಬ್ದ_t ಅಂಕೆ_nodes)
{
    se_L4_ಸ್ಥಾನ_Pos ಸ್ಥಾನ_pos_before = ndks_boot.ಸ್ಥಾನ_pos_cur;

    /* create a sched control cap for each core */
    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < ಅಂಕೆ_nodes; i++) {
        ಆದರೆ (!provide_cap(ಬೇರು_cnode_cap, cap_sched_control_cap_new(i))) {
            ಬರೆ("can't init sched_control for node %u, provide_cap() failed\n", i);
            ಹಿಂತಿರುಗಿ false;
        }
    }

    /* update boot info with slot region for sched control caps */
    ndks_boot.bi_ಚೌಕಟ್ಟು->schedcontrol = (se_L4_ಸ್ಥಾನ_Region) {
        .ಆರಂಭ = ಸ್ಥಾನ_pos_before,
        .ಕೊನೆ = ndks_boot.ಸ್ಥಾನ_pos_cur
    };

    ಹಿಂತಿರುಗಿ true;
}
#ಆದರೆ_ಅಂತ್ಯ

BOOT_ಸಂಕೇತ ಶೂನ್ಯ create_idle_thread(ಶೂನ್ಯ)
{
    pptr_t pptr;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
        pptr = (pptr_t) &ksIdleThreadTCB[SMP_TERNARY(i, 0)];
        ಗಂಟು_STATE_ON_CORE(ksIdleThread, i) = TCB_PTR(pptr + TCB_ಸ್ಥಾನಾಂತರ);
        configureIdleThread(ಗಂಟು_STATE_ON_CORE(ksIdleThread, i));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
        ಗಣ_Thread_Name(ಗಂಟು_STATE_ON_CORE(ksIdleThread, i), "idle_thread");
#ಆದರೆ_ಅಂತ್ಯ
        SMP_COND_STATEMENT(ಗಂಟು_STATE_ON_CORE(ksIdleThread, i)->tcbAffinity = i);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
        configure_sched_context(ಗಂಟು_STATE_ON_CORE(ksIdleThread, i), SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]),
                                MAX_PERIOD_TICKS);
        SMP_COND_STATEMENT(ಗಂಟು_STATE_ON_CORE(ksIdleThread, i)->tcbSchedContext->scCore = i;)
        ಗಂಟು_STATE_ON_CORE(ksIdleSC, i) = SC_PTR(&ksIdleThreadSC[SMP_TERNARY(i, 0)]);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
    }
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */
}

BOOT_ಸಂಕೇತ tcb_t *create_initial_thread(cap_t ಬೇರು_cnode_cap, cap_t it_pd_cap, vptr_t ui_v_ನಮೂದು, vptr_t bi_ಚೌಕಟ್ಟು_vptr,
                                       vptr_t ipcbuf_vptr, cap_t ipcbuf_cap)
{
    tcb_t *tcb = TCB_PTR(rootserver.tcb + TCB_ಸ್ಥಾನಾಂತರ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    tcb->tcb_ಸಮಯ_ತುಂಡು = CONFIG_ಸಮಯ_ತುಂಡು;
#ಆದರೆ_ಅಂತ್ಯ

    Arch_initContext(&tcb->tcbArch.tcbContext);

    /* derive a copy of the IPC buffer cap for inserting */
    deriveCap_ret_t dc_ret = deriveCap(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadIPCBuffer), ipcbuf_cap);
    ಆದರೆ (dc_ret.ಸ್ಥಿತಿ_2 != EXCEPTION_NONE) {
        ಬರೆ("Failed to derive copy of IPC Buffer\n");
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* initialise TCB (corresponds directly to abstract specification) */
    cte_ಸೇರಿಸು(
        ಬೇರು_cnode_cap,
        ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadCNode),
        ಸ್ಥಾನ_PTR(rootserver.tcb, tcbCTable)
    );
    cte_ಸೇರಿಸು(
        it_pd_cap,
        ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadVSpace),
        ಸ್ಥಾನ_PTR(rootserver.tcb, tcbVTable)
    );
    cte_ಸೇರಿಸು(
        dc_ret.cap,
        ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadIPCBuffer),
        ಸ್ಥಾನ_PTR(rootserver.tcb, tcb_ಬಫರ್)
    );
    tcb->tcbIPCBuffer = ipcbuf_vptr;

    ಗಣ_ರಿಜಿಸ್ಟರ್(tcb, cap_ರಿಜಿಸ್ಟರ್, bi_ಚೌಕಟ್ಟು_vptr);
    ಗಣ_ಮುಂದಿನ_PC(tcb, ui_v_ನಮೂದು);

    /* initialise TCB */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    configure_sched_context(tcb, SC_PTR(rootserver.sc), us_ಗಮ್ಯ_Ticks(CONFIG_BOOT_THREAD_ಸಮಯ_ತುಂಡು * US_ಒಳ_MS));
#ಆದರೆ_ಅಂತ್ಯ

    tcb->tcb_ಆದ್ಯತೆ = seL4_MaxPrio;
    tcb->tcbMCP = seL4_MaxPrio;
    tcb->tcbDomain = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    ಸಿದ್ಧತೆ_ಉತ್ತರ_Master(tcb);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_Thread_State(tcb, ThreadState_Running);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_KERNEL_MCS
    SMP_COND_STATEMENT(tcb->tcbAffinity = 0);
#ಆದರೆ_ಅಂತ್ಯ

    /* create initial thread's TCB cap */
    cap_t cap = cap_thread_cap_new(TCB_REF(tcb));
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadTCB), cap);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    cap = cap_sched_context_cap_new(SC_REF(tcb->tcbSchedContext), se_L4_Min_Sched_Context_ಬಿಟ್ಟುಗಳು);
    write_ಸ್ಥಾನ(ಸ್ಥಾನ_PTR(pptr_ದ_cap(ಬೇರು_cnode_cap), seL4_CapInitThreadSC), cap);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    ಗಣ_Thread_Name(tcb, "rootserver");
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ tcb;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_CLOCK_SYNC_TEST_ON_BOOT
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t hypervisor_present(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_X86
    uint32_t ebx = x86_cpuid_ebx(KVM_CPUID_SIGNATURE, 0);
    uint32_t ecx = x86_cpuid_ecx(KVM_CPUID_SIGNATURE, 0);
    uint32_t edx = x86_cpuid_edx(KVM_CPUID_SIGNATURE, 0);

    ಆದರೆ ((ebx == CPUID_KVM_EBX && ecx == CPUID_KVM_ECX && edx == CPUID_KVM_EDX)
        || (ebx == CPUID_TCG_EBX && ecx == CPUID_TCG_ECX && edx == CPUID_TCG_EDX)) {
        ಹಿಂತಿರುಗಿ true;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ false;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ clock_sync_test(ಶೂನ್ಯ)
{
    ticks_t t, t0;
    ticks_t margin = us_ಗಮ್ಯ_Ticks(CLOCK_SYNC_DELTA) + getTimerPrecision();

    ಪ್ರತಿಜ್ಞಾ(getCurrentCPUIndex() != 0);
    t = ಗಂಟು_STATE_ON_CORE(ks_Cur_ಸಮಯ, 0);
    ಮಾಡು {
        /* perform a memory acquire to get new values of ksCurTime */
        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        t0 = ಗಂಟು_STATE_ON_CORE(ks_Cur_ಸಮಯ, 0);
    } ಆಗ (t0 == t);
    t = get_Current_ಸಮಯ();
    ಬರೆ("clock_sync_test[%d]: t0 = %"PRIu64", t = %"PRIu64", td = %"PRIi64"\n",
           (ಪೂರ್ಣಾಂಕ)getCurrentCPUIndex(), t0, t, t - t0);
    /*
     * The test does not consistently work if we are in a virtual machine (e.g
     * within QEMU) because the measurement cannot distinguish between
     * interrupted clock reads and out-of-sync clocks.
     */
    ಆದರೆ (hypervisor_present()) {
        ಬರೆ("clock_sync_test[%d]: disabled, detected running as VM\n", (ಪೂರ್ಣಾಂಕ)getCurrentCPUIndex());
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಪ್ರತಿಜ್ಞಾ(t0 <= margin + t && t <= t0 + margin);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

BOOT_ಸಂಕೇತ ಶೂನ್ಯ init_core_state(tcb_t *scheduler_action)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_HAVE_FPU
    ಗಂಟು_STATE(ksCurFPUOwner) = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DEBUG_BUILD
    /* add initial threads to the debug queue */
    ಗಂಟು_STATE(ksDebugTCBs) = ಖಾಲಿ;
    ಆದರೆ (scheduler_action != SchedulerAction_ResumeCurrentThread &&
        scheduler_action != Scheduler_Action_ಆರಿಸು_New_Thread) {
        tcbDebugAppend(scheduler_action);
    }
    tcbDebugAppend(ಗಂಟು_STATE(ksIdleThread));
#ಆದರೆ_ಅಂತ್ಯ
    ಗಂಟು_STATE(ksSchedulerAction) = scheduler_action;
    ಗಂಟು_STATE(ksCurThread) = ಗಂಟು_STATE(ksIdleThread);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_KERNEL_MCS
    ಗಂಟು_STATE(ksCurSC) = ಗಂಟು_STATE(ksCurThread->tcbSchedContext);
    ಗಂಟು_STATE(ksConsumed) = 0;
    ಗಂಟು_STATE(ksReprogram) = true;
    ಗಂಟು_STATE(ks_Release_ಸರತಿ.ತಲೆ) = ಖಾಲಿ;
    ಗಂಟು_STATE(ks_Release_ಸರತಿ.ಕೊನೆ) = ಖಾಲಿ;
    ಗಂಟು_STATE(ks_Cur_ಸಮಯ) = get_Current_ಸಮಯ();
#ಆದರೆ_ಅಂತ್ಯ
    /* No need for NODE_STATE() as there is no SMP support for domains */
    ksCurDomain = 0;
    ks_Domain_ಸಮಯ = DSCHED_MAX_DURATION;
    ks_Dom_ವೇಳಾಪಟ್ಟಿ[0] = dschedule_make(0, DSCHED_MAX_DURATION);
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
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t pptr_ಒಳ_kernel_ಕಿಟಕಿ(pptr_t pptr)
{
    ಹಿಂತಿರುಗಿ pptr >= PPTR_ತಳ && pptr < PPTR_ತುದಿ;
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
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t provide_untyped_cap(
    cap_t      ಬೇರು_cnode_cap,
    bool_t     device_ಸ್ಮರಣೆ_2,
    pptr_t     pptr,
    ಶಬ್ದ_t     ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು,
    se_L4_ಸ್ಥಾನ_Pos ಮೊದಲ_untyped_ಸ್ಥಾನ
)
{
    bool_t ret;
    cap_t ut_cap;

    /* Since we are in boot code, we can do extensive error checking and
       return failure if anything unexpected happens. */

    /* Bounds check for size parameter */
    ಆದರೆ (ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > se_L4_Max_Untyped_ಬಿಟ್ಟುಗಳು || ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು < se_L4_Min_Untyped_ಬಿಟ್ಟುಗಳು) {
        ಬರೆ("Kernel init: Invalid untyped size %"SEL4_PRIu_ಶಬ್ದ"\n", ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        ಹಿಂತಿರುಗಿ false;
    }

    /* All cap ptrs must be aligned to object size */
    ಆದರೆ (!IS_ALIGNED(pptr, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)) {
        ಬರೆ("Kernel init: Unaligned untyped pptr %p (alignment %"SEL4_PRIu_ಶಬ್ದ")\n", (ಶೂನ್ಯ *)pptr, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        ಹಿಂತಿರುಗಿ false;
    }

    /* All cap ptrs apart from device untypeds must be in the kernel window. */
    ಆದರೆ (!device_ಸ್ಮರಣೆ_2 && !pptr_ಒಳ_kernel_ಕಿಟಕಿ(pptr)) {
        ಬರೆ("Kernel init: Non-device untyped pptr %p outside kernel window\n",
               (ಶೂನ್ಯ *)pptr);
        ಹಿಂತಿರುಗಿ false;
    }

    /* Check that the end of the region is also in the kernel window, so we don't
       need to assume that the kernel window is aligned up to potentially
       seL4_MaxUntypedBits. */
    ಆದರೆ (!device_ಸ್ಮರಣೆ_2 && !pptr_ಒಳ_kernel_ಕಿಟಕಿ(pptr + ಮುಖವಾಡ(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು))) {
        ಬರೆ("Kernel init: End of non-device untyped at %p outside kernel window (size %"SEL4_PRIu_ಶಬ್ದ")\n",
               (ಶೂನ್ಯ *)pptr, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
        ಹಿಂತಿರುಗಿ false;
    }

    ಶಬ್ದ_t i = ndks_boot.ಸ್ಥಾನ_pos_cur - ಮೊದಲ_untyped_ಸ್ಥಾನ;
    ಆದರೆ (i < CONFIG_MAX_ಅಂಕೆ_BOOTINFO_UNTYPED_CAPS) {
        ndks_boot.bi_ಚೌಕಟ್ಟು->untypedList[i] = (seL4_UntypedDesc) {
            .paddr    = pptr_ಗಮ್ಯ_paddr((ಶೂನ್ಯ *)pptr),
            .ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು_2 = ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು,
            .isDevice = device_ಸ್ಮರಣೆ_2,
            .padding  = {0}
        };
        ut_cap = cap_untyped_cap_new(MAX_FREE_ಸೂಚ್ಯಂಕ(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು),
                                     device_ಸ್ಮರಣೆ_2, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, pptr);
        ret = provide_cap(ಬೇರು_cnode_cap, ut_cap);
    } ಇಲ್ಲದಿದ್ದರೆ {
        ಬರೆ("Kernel init: Too many untyped regions for boot info\n");
        ret = true;
    }
    ಹಿಂತಿರುಗಿ ret;
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
BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t create_untypeds_for_region(
    cap_t      ಬೇರು_cnode_cap,
    bool_t     device_ಸ್ಮರಣೆ_2,
    region_t   reg,
    se_L4_ಸ್ಥಾನ_Pos ಮೊದಲ_untyped_ಸ್ಥಾನ
)
{
    /* This code works with regions that wrap (where end < start), because the loop cuts up the
       region into size-aligned chunks, one for each cap. Memory chunks that are size-aligned cannot
       themselves overflow, so they satisfy alignment, size, and overflow conditions. The region
       [0..end) is not necessarily part of the kernel window (depending on the value of PPTR_BASE).
       This is fine for device untypeds. For normal untypeds, the region is assumed to be fully in
       the kernel window. This is not checked here. */
    ಆಗ (!is_reg_ಖಾಲಿಗಳು(reg)) {

        /* Calculate the bit size of the region. This is also correct for end < start: it will
           return the correct size of the set [start..-1] union [0..end). This will then be too
           large for alignment, so the code further down will reduce the size. */
        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = se_L4_ಶಬ್ದ_ಬಿಟ್ಟುಗಳು - 1 - clzl(reg.ಕೊನೆ - reg.ಆರಂಭ);
        /* The size can't exceed the largest possible untyped size. */
        ಆದರೆ (ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > se_L4_Max_Untyped_ಬಿಟ್ಟುಗಳು) {
            ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = se_L4_Max_Untyped_ಬಿಟ್ಟುಗಳು;
        }
        /* The start address 0 satisfies any alignment needs, otherwise ensure
         * the region's bit size does not exceed the alignment of the region.
         */
        ಆದರೆ (0 != reg.ಆರಂಭ) {
            ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ align_ಬಿಟ್ಟುಗಳು = ctzl(reg.ಆರಂಭ);
            ಆದರೆ (ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು > align_ಬಿಟ್ಟುಗಳು) {
                ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು = align_ಬಿಟ್ಟುಗಳು;
            }
        }
        /* Provide an untyped capability for the region only if it is large
         * enough to be retyped into objects later. Otherwise the region can't
         * be used anyway.
         */
        ಆದರೆ (ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು >= se_L4_Min_Untyped_ಬಿಟ್ಟುಗಳು) {
            ಆದರೆ (!provide_untyped_cap(ಬೇರು_cnode_cap, device_ಸ್ಮರಣೆ_2, reg.ಆರಂಭ, ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು, ಮೊದಲ_untyped_ಸ್ಥಾನ)) {
                ಹಿಂತಿರುಗಿ false;
            }
        }
        reg.ಆರಂಭ += ಬಿಟ್(ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    }
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ bool_t create_untypeds(cap_t ಬೇರು_cnode_cap)
{
    se_L4_ಸ್ಥಾನ_Pos ಮೊದಲ_untyped_ಸ್ಥಾನ = ndks_boot.ಸ್ಥಾನ_pos_cur;

    paddr_t ಆರಂಭ = 0;
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ndks_boot.resv_ಎಣಿಕೆ; i++) {
        ಆದರೆ (ಆರಂಭ < ndks_boot.reserved[i].ಆರಂಭ) {
            region_t reg = paddr_ಗಮ್ಯ_pptr_reg((p_region_t) {
                ಆರಂಭ, ndks_boot.reserved[i].ಆರಂಭ
            });
            ಆದರೆ (!create_untypeds_for_region(ಬೇರು_cnode_cap, true, reg, ಮೊದಲ_untyped_ಸ್ಥಾನ)) {
                ಬರೆ("ERROR: creation of untypeds for device region #%u at"
                       " [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") failed\n",
                       (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)i, reg.ಆರಂಭ, reg.ಕೊನೆ);
                ಹಿಂತಿರುಗಿ false;
            }
        }

        ಆರಂಭ = ndks_boot.reserved[i].ಕೊನೆ;
    }

    ಆದರೆ (ಆರಂಭ <= CONFIG_PADDR_USER_DEVICE_ತುದಿ - 1) {
        region_t reg = paddr_ಗಮ್ಯ_pptr_reg((p_region_t) {
            /* CONFIG_PADDR_USER_DEVICE_TOP cast to paddr_t can be 0.
             * create_untypeds_for_region() can deal with that correctly. */
            ಆರಂಭ, (paddr_t) CONFIG_PADDR_USER_DEVICE_ತುದಿ
        });

        ಆದರೆ (!create_untypeds_for_region(ಬೇರು_cnode_cap, true, reg, ಮೊದಲ_untyped_ಸ್ಥಾನ)) {
            ಬರೆ("ERROR: creation of untypeds for top device region"
                   " [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") failed\n",
                   reg.ಆರಂಭ, reg.ಕೊನೆ);
            ಹಿಂತಿರುಗಿ false;
        }
    }

    /* There is a part of the kernel (code/data) that is only needed for the
     * boot process. We can create UT objects for these frames, so the memory
     * can be reused.
     */
    region_t boot_mem_reuse_reg = paddr_ಗಮ್ಯ_pptr_reg(get_p_reg_kernel_img_boot());
    ಆದರೆ (!create_untypeds_for_region(ಬೇರು_cnode_cap, false, boot_mem_reuse_reg, ಮೊದಲ_untyped_ಸ್ಥಾನ)) {
        ಬರೆ("ERROR: creation of untypeds for recycled boot memory"
               " [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ"] failed\n",
               boot_mem_reuse_reg.ಆರಂಭ, boot_mem_reuse_reg.ಕೊನೆ);
        ಹಿಂತಿರುಗಿ false;
    }

    /* convert remaining freemem into UT objects and provide the caps */
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ARRAY_ಗಾತ್ರ(ndks_boot.freemem); i++) {
        region_t reg = ndks_boot.freemem[i];
        ndks_boot.freemem[i] = REG_ಖಾಲಿಗಳು;
        ಆದರೆ (!create_untypeds_for_region(ಬೇರು_cnode_cap, false, reg, ಮೊದಲ_untyped_ಸ್ಥಾನ)) {
            ಬರೆ("ERROR: creation of untypeds for free memory region #%u at"
                   " [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") failed\n",
                   (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)i, reg.ಆರಂಭ, reg.ಕೊನೆ);
            ಹಿಂತಿರುಗಿ false;
        }
    }

    ndks_boot.bi_ಚೌಕಟ್ಟು->untyped = (se_L4_ಸ್ಥಾನ_Region) {
        .ಆರಂಭ = ಮೊದಲ_untyped_ಸ್ಥಾನ,
        .ಕೊನೆ   = ndks_boot.ಸ್ಥಾನ_pos_cur
    };

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ bi_finalise(ಶೂನ್ಯ)
{

    ಆದರೆ (rootserver.paging.ಆರಂಭ != rootserver.paging.ಕೊನೆ) {
        ಬರೆ("WARNING: internal book keeping error. Less pagetables allocated than predicted: "
               "%ld page tables allocated but not used.\n", (rootserver.paging.ಕೊನೆ - rootserver.paging.ಆರಂಭ) >> se_L4_ಪುಟ_ಕೋಷ್ಟಕ_ಬಿಟ್ಟುಗಳು);
    }

    ndks_boot.bi_ಚೌಕಟ್ಟು->ಖಾಲಿಗಳು = (se_L4_ಸ್ಥಾನ_Region) {
        .ಆರಂಭ = ndks_boot.ಸ್ಥಾನ_pos_cur,
        .ಕೊನೆ   = ಬಿಟ್(CONFIG_ಬೇರು_CNODE_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
    };
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t check_ಲಭ್ಯ_ಸ್ಮರಣೆ(ಶಬ್ದ_t n_ಲಭ್ಯ,
                                               ಸ್ಥಿರಾಂಕ p_region_t *ಲಭ್ಯ)
{
    /* The system configuration is broken if no region is available. */
    ಆದರೆ (0 == n_ಲಭ್ಯ) {
        ಬರೆ("ERROR: no memory regions available\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಬರೆ("Available phys memory regions: %"SEL4_PRIu_ಶಬ್ದ"\n", n_ಲಭ್ಯ);
    /* Force ordering and exclusivity of available regions. */
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < n_ಲಭ್ಯ; i++) {
        ಸ್ಥಿರಾಂಕ p_region_t *r = &ಲಭ್ಯ[i];
        ಬರೆ("  [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ")\n", r->ಆರಂಭ, r->ಕೊನೆ);

        /* Available regions must not wrap */
        ಆದರೆ (r->ಆರಂಭ > r->ಕೊನೆ) {
            ಬರೆ("ERROR: memory region %"SEL4_PRIu_ಶಬ್ದ" has start > end\n", i);
            ಹಿಂತಿರುಗಿ false;
        }

        /* Available regions must be non-empty. */
        ಆದರೆ (r->ಆರಂಭ == r->ಕೊನೆ) {
            ಬರೆ("ERROR: memory region %"SEL4_PRIu_ಶಬ್ದ" empty\n", i);
            ಹಿಂತಿರುಗಿ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
         * so the '==' case is fine. Directly adjacent regions are allowed.
         */
        ಆದರೆ ((i > 0) && (r->ಆರಂಭ < ಲಭ್ಯ[i - 1].ಕೊನೆ)) {
            ಬರೆ("ERROR: memory region %"SEL4_PRIu_ಶಬ್ದ" in wrong order\n", i);
            ಹಿಂತಿರುಗಿ false;
        }
    }

    ಹಿಂತಿರುಗಿ true;
}

/* We can't declare arrays on the stack, so this is space for init_avail_reg()
 * below to populate and init_freemem() to consume. */
BOOT_BSS ಸ್ಥಿರ region_t avail_reg[MAX_ಅಂಕೆ_FREEMEM_REG];

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
BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶಬ್ದ_t init_avail_reg(ಶಬ್ದ_t n_ಲಭ್ಯ,
                                       ಸ್ಥಿರಾಂಕ p_region_t *ಲಭ್ಯ)
{
    ಶಬ್ದ_t cnt = 0;
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < n_ಲಭ್ಯ; i++) {
        ಸ್ಥಿರಾಂಕ p_region_t *r = &ಲಭ್ಯ[i];

        ಆದರೆ ((r->ಆರಂಭ >= PADDR_ತುದಿ) || (r->ಕೊನೆ <= PADDR_ತಳ)) {
            ಬರೆ("  region [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ") is outside the kernel"
                   " window, only potentially available as device untypeds\n", r->ಆರಂಭ, r->ಕೊನೆ);
        } ಇಲ್ಲದಿದ್ದರೆ {
            p_region_t usable_reg = {
                .ಆರಂಭ = MAX(r->ಆರಂಭ, PADDR_ತಳ),
                .ಕೊನೆ   = MIN(r->ಕೊನೆ, PADDR_ತುದಿ)
            };

            ಆದರೆ (cnt >= ARRAY_ಗಾತ್ರ(avail_reg)) {
                /* No space left in the array, seems MAX_NUM_FREEMEM_REG should
                 * be increased. Debug builds raise an assert here because this
                 * is likely a porting issue that should be looked into. Release
                 * builds will continue booting, but can't use this memory
                 * region. This might still be sufficient to run.
                 */
                ಬರೆ("  WARNING: cannot use region [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ"),"
                       " avail_reg[] is full\n", r->ಆರಂಭ, r->ಕೊನೆ);
                ಪ್ರತಿಜ್ಞಾ(0);
            } ಇಲ್ಲದಿದ್ದರೆ {
                avail_reg[cnt] = paddr_ಗಮ್ಯ_pptr_reg(usable_reg);
                cnt++;
            }
        }
    }
    ಹಿಂತಿರುಗಿ cnt;
}


BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t check_reserved_ಸ್ಮರಣೆ(ಶಬ್ದ_t n_reserved,
                                              ಸ್ಥಿರಾಂಕ region_t *reserved)
{
    ಬರೆ("Reserved virt address space regions: %"SEL4_PRIu_ಶಬ್ದ"\n",
           n_reserved);
    /* Force ordering and exclusivity of reserved regions. */
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < n_reserved; i++) {
        ಸ್ಥಿರಾಂಕ region_t *r = &reserved[i];
        ಬರೆ("  [%"SEL4_PRIx_ಶಬ್ದ"..%"SEL4_PRIx_ಶಬ್ದ")\n", r->ಆರಂಭ, r->ಕೊನೆ);

        /* Reserved regions must be sane, the size is allowed to be zero. */
        ಆದರೆ (r->ಆರಂಭ > r->ಕೊನೆ) {
            ಬರೆ("ERROR: reserved region %"SEL4_PRIu_ಶಬ್ದ" has start > end\n", i);
            ಹಿಂತಿರುಗಿ false;
        }

        /* Regions must be ordered and must not overlap. Regions are [start..end),
           so the == case is fine. Directly adjacent regions are allowed. */
        ಆದರೆ ((i > 0) && (r->ಆರಂಭ < reserved[i - 1].ಕೊನೆ)) {
            ಬರೆ("ERROR: reserved region %"SEL4_PRIu_ಶಬ್ದ" in wrong order\n", i);
            ಹಿಂತಿರುಗಿ false;
        }
    }

    ಹಿಂತಿರುಗಿ true;
}

/**
 * Dynamically initialise the available memory on the platform.
 * A region represents an area of memory.
 */
BOOT_ಸಂಕೇತ bool_t init_freemem(ಶಬ್ದ_t n_ಲಭ್ಯ, ಸ್ಥಿರಾಂಕ p_region_t *ಲಭ್ಯ,
                              ಶಬ್ದ_t n_reserved, ಸ್ಥಿರಾಂಕ region_t *reserved,
                              v_region_t it_v_reg, ಶಬ್ದ_t extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು)
{
    ಆದರೆ (!check_ಲಭ್ಯ_ಸ್ಮರಣೆ(n_ಲಭ್ಯ, ಲಭ್ಯ)) {
        ಹಿಂತಿರುಗಿ false;
    }

    /* After here, avail_reg[0..n_available) are all regions within the kernel window. */
    n_ಲಭ್ಯ = init_avail_reg(n_ಲಭ್ಯ, ಲಭ್ಯ);
    ಆದರೆ (0 == n_ಲಭ್ಯ) {
        ಬರೆ("ERROR: no available memory within the kernel window\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (!check_reserved_ಸ್ಮರಣೆ(n_reserved, reserved)) {
        ಹಿಂತಿರುಗಿ false;
    }

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < ARRAY_ಗಾತ್ರ(ndks_boot.freemem); i++) {
        ndks_boot.freemem[i] = REG_ಖಾಲಿಗಳು;
    }

    ಶಬ್ದ_t a = 0;
    ಶಬ್ದ_t r = 0;
    /* Now iterate through the available regions, removing any reserved regions. */
    ಆಗ (a < n_ಲಭ್ಯ && r < n_reserved) {
        ಆದರೆ (reserved[r].ಆರಂಭ == reserved[r].ಕೊನೆ) {
            /* reserved region is empty - skip it */
            r++;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (avail_reg[a].ಆರಂಭ >= avail_reg[a].ಕೊನೆ) {
            /* skip the entire region - it's empty now after trimming */
            a++;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reserved[r].ಕೊನೆ <= avail_reg[a].ಆರಂಭ) {
            /* the reserved region is below the available region - skip it */
            reserve_region(pptr_ಗಮ್ಯ_paddr_reg(reserved[r]));
            r++;
        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (reserved[r].ಆರಂಭ >= avail_reg[a].ಕೊನೆ) {
            /* the reserved region is above the available region - take the whole thing */
            ಸೇರಿಸು_region(avail_reg[a]);
            a++;
        } ಇಲ್ಲದಿದ್ದರೆ {
            /* the reserved region overlaps with the available region */
            ಆದರೆ (reserved[r].ಆರಂಭ <= avail_reg[a].ಆರಂಭ) {
                /* the region overlaps with the start of the available region.
                 * trim start of the available region */
                avail_reg[a].ಆರಂಭ = MIN(avail_reg[a].ಕೊನೆ, reserved[r].ಕೊನೆ);
                /* do not increment reserved index here - there could be more overlapping regions */
            } ಇಲ್ಲದಿದ್ದರೆ {
                ಪ್ರತಿಜ್ಞಾ(reserved[r].ಆರಂಭ < avail_reg[a].ಕೊನೆ);
                /* take the first chunk of the available region and move
                 * the start to the end of the reserved region */
                region_t m = avail_reg[a];
                m.ಕೊನೆ = reserved[r].ಆರಂಭ;
                ಸೇರಿಸು_region(m);
                ಆದರೆ (avail_reg[a].ಕೊನೆ > reserved[r].ಕೊನೆ) {
                    avail_reg[a].ಆರಂಭ = reserved[r].ಕೊನೆ;
                    /* we could increment reserved index here, but it's more consistent with the
                     * other overlapping case if we don't */
                } ಇಲ್ಲದಿದ್ದರೆ {
                    a++;
                }
            }
        }
    }

    ಗಾಗಿ (; r < n_reserved; r++) {
        ಆದರೆ (reserved[r].ಆರಂಭ < reserved[r].ಕೊನೆ) {
            reserve_region(pptr_ಗಮ್ಯ_paddr_reg(reserved[r]));
        }
    }

    /* no more reserved regions - add the rest */
    ಗಾಗಿ (; a < n_ಲಭ್ಯ; a++) {
        ಆದರೆ (avail_reg[a].ಆರಂಭ < avail_reg[a].ಕೊನೆ) {
            ಸೇರಿಸು_region(avail_reg[a]);
        }
    }

    /* now try to fit the root server objects into a region */
    ಪೂರ್ಣಾಂಕ i = ARRAY_ಗಾತ್ರ(ndks_boot.freemem) - 1;
    ಆದರೆ (!is_reg_ಖಾಲಿಗಳು(ndks_boot.freemem[i])) {
        ಬರೆ("ERROR: insufficient MAX_NUM_FREEMEM_REG (%u)\n",
               (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)MAX_ಅಂಕೆ_FREEMEM_REG);
        ಹಿಂತಿರುಗಿ false;
    }
    /* skip any empty regions */
    ಗಾಗಿ (; i >= 0 && is_reg_ಖಾಲಿಗಳು(ndks_boot.freemem[i]); i--);

    /* try to grab the last available p region to create the root server objects
     * from. If possible, retain any left over memory as an extra p region */
    ಶಬ್ದ_t ಗಾತ್ರ_2 = calculate_rootserver_ಗಾತ್ರ(it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    ಶಬ್ದ_t max = rootserver_max_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು(extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
    ಗಾಗಿ (; i >= 0; i--) {
        /* Invariant: both i and (i + 1) are valid indices in ndks_boot.freemem. */
        ಪ್ರತಿಜ್ಞಾ(i < ARRAY_ಗಾತ್ರ(ndks_boot.freemem) - 1);
        /* Invariant; the region at index i is the current candidate.
         * Invariant: regions 0 up to (i - 1), if any, are additional candidates.
         * Invariant: region (i + 1) is empty. */
        ಪ್ರತಿಜ್ಞಾ(is_reg_ಖಾಲಿಗಳು(ndks_boot.freemem[i + 1]));
        /* Invariant: regions above (i + 1), if any, are empty or too small to use.
         * Invariant: all non-empty regions are ordered, disjoint and unallocated. */

        /* We make a fresh variable to index the known-empty region, because the
         * SimplExportAndRefine verification test has poor support for array
         * indices that are sums of variables and small constants. */
        ಪೂರ್ಣಾಂಕ ಖಾಲಿಗಳು_ಸೂಚ್ಯಂಕ = i + 1;

        /* Try to take the top-most suitably sized and aligned chunk. */
        pptr_t unaligned_ಆರಂಭ = ndks_boot.freemem[i].ಕೊನೆ - ಗಾತ್ರ_2;
        pptr_t ಆರಂಭ = ಸುತ್ತು_ಕೆಳಕ್ಕೆ(unaligned_ಆರಂಭ, max);
        /* if unaligned_start didn't underflow, and start fits in the region,
         * then we've found a region that fits the root server objects. */
        ಆದರೆ (unaligned_ಆರಂಭ <= ndks_boot.freemem[i].ಕೊನೆ
            && ಆರಂಭ >= ndks_boot.freemem[i].ಆರಂಭ) {
            create_rootserver_objects(ಆರಂಭ, it_v_reg, extra_bi_ಗಾತ್ರ_ಬಿಟ್ಟುಗಳು);
            /* There may be leftovers before and after the memory we used. */
            /* Shuffle the after leftover up to the empty slot (i + 1). */
            ndks_boot.freemem[ಖಾಲಿಗಳು_ಸೂಚ್ಯಂಕ] = (region_t) {
                .ಆರಂಭ = ಆರಂಭ + ಗಾತ್ರ_2,
                .ಕೊನೆ = ndks_boot.freemem[i].ಕೊನೆ
            };
            /* Leave the before leftover in current slot i. */
            ndks_boot.freemem[i].ಕೊನೆ = ಆರಂಭ;
            /* Regions i and (i + 1) are now well defined, ordered, disjoint,
             * and unallocated, so we can return successfully. */
            ಹಿಂತಿರುಗಿ true;
        }
        /* Region i isn't big enough, so shuffle it up to slot (i + 1),
         * which we know is unused. */
        ndks_boot.freemem[ಖಾಲಿಗಳು_ಸೂಚ್ಯಂಕ] = ndks_boot.freemem[i];
        /* Now region i is unused, so make it empty to reestablish the invariant
         * for the next iteration (when it will be slot i + 1). */
        ndks_boot.freemem[i] = REG_ಖಾಲಿಗಳು;
    }

    /* We didn't find a big enough region. */
    ಬರೆ("ERROR: no free memory region is big enough for root server "
           "objects, need size/alignment of 2^%"SEL4_PRIu_ಶಬ್ದ"\n", max);
    ಹಿಂತಿರುಗಿ false;
}
