/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/gic_v3.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IRQ_ਸੈੱਟ_ALL 0xffffffff

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GICR_ਪ੍ਰਤੀ_CORE_ਮਾਪ  (0x20000)
/* Assume 8 cores */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GICR_ਮਾਪ           (0x100000)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GIC_DEADLINE_MS 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GIC_REG_ਚੌੜਾਈ   32

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_INTID_ਖਿਸਕਾਓ          (24)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_AFF1_ਖਿਸਕਾਓ           (16)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_AFF2_ਖਿਸਕਾਓ           (32)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_AFF3_ਖਿਸਕਾਓ           (48)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_RS_ਖਿਸਕਾਓ             (44)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_IRM_ਦੋਆਧਾਰੀ_ਅੰਕ              (40)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ICC_SGI1_R_CPUTARGETLIST_ਨਕਾਬ   0xffff

ਅਸਥਿਰ ਬਣਤਰ gic_dist_map *ਅਚਲ gic_dist = (ਅਸਥਿਰ ਬਣਤਰ gic_dist_map *)(GICD_PPTR);
ਅਸਥਿਰ ਖਾਲੀ *ਅਚਲ gicr_ਅਧਾਰ = (ਅਸਥਿਰ uint8_t *)(GICR_PPTR);

ਸ਼ਬਦ_t ਸਰਗਰਮ_irq[CONFIG_MAX_ਅੰਕ_NODES];
ਅਸਥਿਰ ਬਣਤਰ gic_rdist_map *gic_rdist_map[CONFIG_MAX_ਅੰਕ_NODES] = { 0 };
ਅਸਥਿਰ ਬਣਤਰ gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_ਅੰਕ_NODES] = { 0 };

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARCH_AARCH64
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF0(x) (x & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF1(x) ((x >> 8) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF2(x) ((x >> 16) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF3(x) ((x >> 32) & 0xff)
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF0(x) (x & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF1(x) ((x >> 8) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF2(x) ((x >> 16) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF3(x) (0)
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_MT(x)   (x & ਦੋਆਧਾਰੀ_ਅੰਕ(24))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_AFF_ਨਕਾਬ(x) (x & 0xff00ffffff)

ਸਥਿਰ ਸ਼ਬਦ_t mpidr_map[CONFIG_MAX_ਅੰਕ_NODES];

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t get_mpidr(ਸ਼ਬਦ_t core_id)
{
    ਵਾਪਸ mpidr_map[core_id];
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਸ਼ਬਦ_t get_current_mpidr(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t core_id = CURRENT_CPU_ਸੂਚਕਾਂਕ();
    ਵਾਪਸ get_mpidr(core_id);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t mpidr_ਮੰਜ਼ਿਲ_gic_affinity(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    ਵਾਪਸ affinity;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t sgir_ਸ਼ਬਦ_from_ਦਲੀਲਾਂ(ਸ਼ਬਦ_t irq, ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    uint64_t t = ਨਿਸ਼ਾਨਾ; /* make sure shifts below are on 64 bit */
    ਵਾਪਸ (uint64_t) irq << ICC_SGI1_R_INTID_ਖਿਸਕਾਓ
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_ਖਿਸਕਾਓ // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_ਖਿਸਕਾਓ // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_ਖਿਸਕਾਓ // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_ਖਿਸਕਾਓ; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
ਸਥਿਰ uint32_t gicv3_do_wait_for_rwp(ਅਸਥਿਰ uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t ਉਡੀਕ = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    ਜੇ (!(val & GICD_CTLR_RWP)) {
        ਵਾਪਸ 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_ਪ੍ਰਤੀ_MS);

    ਜਦੋਂ (ਉਡੀਕ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        ਜੇ (gpt_cnt_tval >= gpt_cnt_ciel) {
            ਲਿਖੋ("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            ਉਡੀਕ = false;

        } ਨਹੀਂਤਾਂ ਜੇ (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            ਉਡੀਕ = false;
        }
    }
    ਵਾਪਸ ret;
}

ਸਥਿਰ ਖਾਲੀ gicv3_dist_wait_for_rwp(ਖਾਲੀ)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

ਸਥਿਰ ਖਾਲੀ gicv3_redist_wait_for_rwp(ਖਾਲੀ)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->ctlr);
}

ਸਥਿਰ ਖਾਲੀ gicv3_enable_sre(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t val;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_SRE_EL2, val);
#ਜੇ_ਅੰਤ

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_SRE_EL1, val);
    isb();
}


BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ dist_init(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t i;
    uint32_t type;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nr_ਸਤਰਾਂ;
    uint64_t affinity;
    uint32_t ਤਰਜੀਹ;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_ਸਤਰਾਂ = GIC_REG_ਚੌੜਾਈ * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    ਲਈ (i = SPI_ਸ਼ੁਰੂ; i < nr_ਸਤਰਾਂ; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    ਤਰਜੀਹ = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    ਲਈ (i = SPI_ਸ਼ੁਰੂ; i < nr_ਸਤਰਾਂ; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = ਤਰਜੀਹ;
    }
    /* Disable and clear all global interrupts */
    ਲਈ (i = SPI_ਸ਼ੁਰੂ; i < nr_ਸਤਰਾਂ; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_ਸੈੱਟ_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_ਸੈੱਟ_ALL;
    }

    /* group 1 for non-secure */
    ਜੇ (config_ਸੈੱਟ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        ਲਈ (i = SPI_ਸ਼ੁਰੂ; i < nr_ਸਤਰਾਂ; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_ਸੈੱਟ_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_ਮੰਜ਼ਿਲ_gic_affinity();
    ਲਈ (i = SPI_ਸ਼ੁਰੂ; i < nr_ਸਤਰਾਂ; i++) {
        gic_dist->iroutern[i - SPI_ਸ਼ੁਰੂ] = affinity;
    }

}

BOOT_ਕੋਡ ਸਥਿਰ uint32_t gicr_enable_rdist(ਪੂਰਨ core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t ਉਡੀਕ = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_ਪ੍ਰਤੀ_MS);

    ਜਦੋਂ (ਉਡੀਕ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        ਜੇ (gpt_cnt_tval >= gpt_cnt_ciel) {
            ਲਿਖੋ("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            ਉਡੀਕ = false;

        } ਨਹੀਂਤਾਂ ਜੇ (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            ਉਡੀਕ = false;
        }
    }
    ਵਾਪਸ ret;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ gicr_locate_interface(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t ਵਿੱਥ;
    ਪੂਰਨ core_id = CURRENT_CPU_ਸੂਚਕਾਂਕ();
    ਸ਼ਬਦ_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    ਲਈ (ਵਿੱਥ = 0; ਵਿੱਥ < GICR_ਮਾਪ; ਵਿੱਥ += GICR_ਪ੍ਰਤੀ_CORE_ਮਾਪ) {

        uint64_t typer = ((ਬਣਤਰ gic_rdist_map *)((ਸ਼ਬਦ_t)gicr_ਅਧਾਰ + ਵਿੱਥ))->typer;
        ਜੇ ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            ਸ਼ਬਦ_t gicr = (ਸ਼ਬਦ_t)gicr_ਅਧਾਰ + ਵਿੱਥ;
            ਜੇ (gic_rdist_map[core_id] != ਨਿਰਾ || gic_rdist_sgi_ppi_map[core_id] != ਨਿਰਾ) {
                ਲਿਖੋ("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == ਨਿਰਾ ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == ਨਿਰਾ ? (ਖਾਲੀ *)gic_rdist_map[core_id] : (ਖਾਲੀ *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (ਖਾਲੀ *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (ਖਾਲੀ *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            ਜੇ (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                ਪੂਰਨ ret = gicr_enable_rdist(core_id);
                ਜੇ (ret == 1) {
                    halt();
                }
            }
            ਰੋਕੋ;
        }
    }
    ਜੇ (ਵਿੱਥ >= GICR_ਮਾਪ) {
        ਲਿਖੋ("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (ਪੂਰਨ)MPIDR_AFF3(mpidr), (ਪੂਰਨ)MPIDR_AFF2(mpidr),
               (ਪੂਰਨ)MPIDR_AFF1(mpidr), (ਪੂਰਨ)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ gicr_init(ਖਾਲੀ)
{
    ਪੂਰਨ i;
    uint32_t ਤਰਜੀਹ;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    ਤਰਜੀਹ = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    ਲਈ (i = 0; i < SPI_ਸ਼ੁਰੂ; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->ipriorityrn[i / 4] = ਤਰਜੀਹ;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->isenabler0 = 0x0000ffff;
    ਜੇ (config_ਸੈੱਟ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->igroupr0 = IRQ_ਸੈੱਟ_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ cpu_iface_init(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_PMR_EL1, DEFAULT_PMR_ਮੁੱਲ);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_ਸ਼ਬਦ(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_ਸ਼ਬਦ(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

ਖਾਲੀ ਸੈੱਟ_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    ਸ਼ਬਦ_t hw_irq = IRQT_ਮੰਜ਼ਿਲ_IRQ(irq);
    ਸ਼ਬਦ_t core = IRQT_ਮੰਜ਼ਿਲ_CORE(irq);
    ਜੇ (HW_IRQ_IS_SGI(hw_irq)) {
        ਵਾਪਸ;
    }
    ਪੂਰਨ ਸ਼ਬਦ_2 = hw_irq >> 4;
    ਪੂਰਨ ਦੋਆਧਾਰੀ_ਅੰਕ_2 = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    ਜੇ (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } ਨਹੀਂਤਾਂ {
        icfgr = gic_dist->icfgrn[ਸ਼ਬਦ_2];
    }

    ਜੇ (trigger) {
        icfgr |= (2 << ਦੋਆਧਾਰੀ_ਅੰਕ_2);
    } ਨਹੀਂਤਾਂ {
        icfgr &= ~(3 << ਦੋਆਧਾਰੀ_ਅੰਕ_2);
    }

    ਜੇ (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } ਨਹੀਂਤਾਂ {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[ਸ਼ਬਦ_2] = icfgr;
    }

    ਵਾਪਸ;
}

BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    dist_init();
}

BOOT_ਕੋਡ ਖਾਲੀ cpu_initLocalIRQController(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t mpidr = 0;
    SYSTEM_READ_ਸ਼ਬਦ(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_ਸੂਚਕਾਂਕ()] = mpidr;
    ਸਰਗਰਮ_irq[CURRENT_CPU_ਸੂਚਕਾਂਕ()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    ਵਾਪਸ ਨਿਸ਼ਾਨਾ <= UINT32_MAX;
}

ਖਾਲੀ plat_sendSGI(ਸ਼ਬਦ_t irq, ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    uint64_t sgi1r_ਅਧਾਰ = sgir_ਸ਼ਬਦ_from_ਦਲੀਲਾਂ(irq, ਨਿਸ਼ਾਨਾ);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_ਅਧਾਰ);
    isb();
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MPIDR_MT(x)   (x & ਦੋਆਧਾਰੀ_ਅੰਕ(24))

ਖਾਲੀ ipi_send_ਨਿਸ਼ਾਨਾ(irq_t irq, ਸ਼ਬਦ_t cpu_ਨਿਸ਼ਾਨਾ_List)
{
    uint64_t sgi1r_ਅਧਾਰ = ((ਸ਼ਬਦ_t) IRQT_ਮੰਜ਼ਿਲ_IRQ(irq)) << ICC_SGI1_R_INTID_ਖਿਸਕਾਓ;
    ਸ਼ਬਦ_t sgi1r[CONFIG_MAX_ਅੰਕ_NODES];
    ਸ਼ਬਦ_t ਆਖਰੀ_aff1 = 0;

    ਲਈ (ਸ਼ਬਦ_t i = 0; i < CONFIG_MAX_ਅੰਕ_NODES; i++) {
        sgi1r[i] = 0;
        ਜੇ (cpu_ਨਿਸ਼ਾਨਾ_List & ਦੋਆਧਾਰੀ_ਅੰਕ(i)) {
            ਸ਼ਬਦ_t mpidr = mpidr_map[i];
            ਸ਼ਬਦ_t aff1 = MPIDR_AFF1(mpidr);
            ਸ਼ਬਦ_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            ਪ੍ਰਤਿਜ੍ਞਾ(aff1 >= 0 && aff1 < CONFIG_MAX_ਅੰਕ_NODES);
            sgi1r[aff1] |= sgi1r_ਅਧਾਰ | (aff1 << ICC_SGI1_R_AFF1_ਖਿਸਕਾਓ) | (1 << aff0);
            ਜੇ (aff1 > ਆਖਰੀ_aff1) {
                ਆਖਰੀ_aff1 = aff1;
            }
        }
    }
    ਲਈ (ਸ਼ਬਦ_t i = 0; i <= ਆਖਰੀ_aff1; i++) {
        ਜੇ (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

ਖਾਲੀ ਸੈੱਟ_IRQTarget(irq_t irq, se_L4_ਸ਼ਬਦ ਨਿਸ਼ਾਨਾ)
{
    ਜੇ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        ਵਾਪਸ;
    }

    ਸ਼ਬਦ_t hw_irq = IRQT_ਮੰਜ਼ਿਲ_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_ਸ਼ੁਰੂ] = MPIDR_AFF_ਨਕਾਬ(mpidr_map[ਨਿਸ਼ਾਨਾ]);
}

#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

ਸ਼ਬਦ_t gic_vcpu_ਅੰਕ_list_regs;

#ਜੇ_ਅੰਤ /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
