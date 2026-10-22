/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <types.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/gic_v3.h>

#ವ್ಯಾಖ್ಯಾನಿಸು IRQ_ಗಣ_ALL 0xffffffff

#ವ್ಯಾಖ್ಯಾನಿಸು RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#ವ್ಯಾಖ್ಯಾನಿಸು GICR_ಪ್ರತಿ_CORE_ಗಾತ್ರ  (0x20000)
/* Assume 8 cores */
#ವ್ಯಾಖ್ಯಾನಿಸು GICR_ಗಾತ್ರ           (0x100000)

#ವ್ಯಾಖ್ಯಾನಿಸು GIC_DEADLINE_MS 2
#ವ್ಯಾಖ್ಯಾನಿಸು GIC_REG_ಅಗಲ   32

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_INTID_ಸರಿಸು          (24)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_AFF1_ಸರಿಸು           (16)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_AFF2_ಸರಿಸು           (32)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_AFF3_ಸರಿಸು           (48)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_RS_ಸರಿಸು             (44)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_IRM_ಬಿಟ್              (40)
#ವ್ಯಾಖ್ಯಾನಿಸು ICC_SGI1_R_CPUTARGETLIST_ಮುಖವಾಡ   0xffff

ಅಸ್ಥಿರ ರಚನೆ gic_dist_map *ಸ್ಥಿರಾಂಕ gic_dist = (ಅಸ್ಥಿರ ರಚನೆ gic_dist_map *)(GICD_PPTR);
ಅಸ್ಥಿರ ಶೂನ್ಯ *ಸ್ಥಿರಾಂಕ gicr_ತಳ = (ಅಸ್ಥಿರ uint8_t *)(GICR_PPTR);

ಶಬ್ದ_t ಸಕ್ರಿಯ_irq[CONFIG_MAX_ಅಂಕೆ_NODES];
ಅಸ್ಥಿರ ರಚನೆ gic_rdist_map *gic_rdist_map[CONFIG_MAX_ಅಂಕೆ_NODES] = { 0 };
ಅಸ್ಥಿರ ರಚನೆ gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_ಅಂಕೆ_NODES] = { 0 };

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARCH_AARCH64
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF0(x) (x & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF1(x) ((x >> 8) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF2(x) ((x >> 16) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF3(x) ((x >> 32) & 0xff)
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF0(x) (x & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF1(x) ((x >> 8) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF2(x) ((x >> 16) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF3(x) (0)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_MT(x)   (x & ಬಿಟ್(24))
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_AFF_ಮುಖವಾಡ(x) (x & 0xff00ffffff)

ಸ್ಥಿರ ಶಬ್ದ_t mpidr_map[CONFIG_MAX_ಅಂಕೆ_NODES];

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t get_mpidr(ಶಬ್ದ_t core_id)
{
    ಹಿಂತಿರುಗಿ mpidr_map[core_id];
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶಬ್ದ_t get_current_mpidr(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t core_id = CURRENT_CPU_ಸೂಚ್ಯಂಕ();
    ಹಿಂತಿರುಗಿ get_mpidr(core_id);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t mpidr_ಗಮ್ಯ_gic_affinity(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    ಹಿಂತಿರುಗಿ affinity;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t sgir_ಶಬ್ದ_from_ವಾದಗಳು(ಶಬ್ದ_t irq, ಶಬ್ದ_t ಗುರಿ)
{
    uint64_t t = ಗುರಿ; /* make sure shifts below are on 64 bit */
    ಹಿಂತಿರುಗಿ (uint64_t) irq << ICC_SGI1_R_INTID_ಸರಿಸು
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_ಸರಿಸು // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_ಸರಿಸು // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_ಸರಿಸು // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_ಸರಿಸು; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
ಸ್ಥಿರ uint32_t gicv3_do_wait_for_rwp(ಅಸ್ಥಿರ uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t ಕಾಯುವಿಕೆ = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    ಆದರೆ (!(val & GICD_CTLR_RWP)) {
        ಹಿಂತಿರುಗಿ 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_ಪ್ರತಿ_MS);

    ಆಗ (ಕಾಯುವಿಕೆ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        ಆದರೆ (gpt_cnt_tval >= gpt_cnt_ciel) {
            ಬರೆ("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            ಕಾಯುವಿಕೆ = false;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            ಕಾಯುವಿಕೆ = false;
        }
    }
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಶೂನ್ಯ gicv3_dist_wait_for_rwp(ಶೂನ್ಯ)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

ಸ್ಥಿರ ಶೂನ್ಯ gicv3_redist_wait_for_rwp(ಶೂನ್ಯ)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->ctlr);
}

ಸ್ಥಿರ ಶೂನ್ಯ gicv3_enable_sre(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t val;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_ಶಬ್ದ(ICC_SRE_EL2, val);
#ಆದರೆ_ಅಂತ್ಯ

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_ಶಬ್ದ(ICC_SRE_EL1, val);
    isb();
}


BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ dist_init(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t i;
    uint32_t type;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nr_ಸಾಲುಗಳು;
    uint64_t affinity;
    uint32_t ಆದ್ಯತೆ;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_ಸಾಲುಗಳು = GIC_REG_ಅಗಲ * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    ಗಾಗಿ (i = SPI_ಆರಂಭ; i < nr_ಸಾಲುಗಳು; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    ಆದ್ಯತೆ = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    ಗಾಗಿ (i = SPI_ಆರಂಭ; i < nr_ಸಾಲುಗಳು; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = ಆದ್ಯತೆ;
    }
    /* Disable and clear all global interrupts */
    ಗಾಗಿ (i = SPI_ಆರಂಭ; i < nr_ಸಾಲುಗಳು; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_ಗಣ_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_ಗಣ_ALL;
    }

    /* group 1 for non-secure */
    ಆದರೆ (config_ಗಣ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        ಗಾಗಿ (i = SPI_ಆರಂಭ; i < nr_ಸಾಲುಗಳು; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_ಗಣ_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_ಗಮ್ಯ_gic_affinity();
    ಗಾಗಿ (i = SPI_ಆರಂಭ; i < nr_ಸಾಲುಗಳು; i++) {
        gic_dist->iroutern[i - SPI_ಆರಂಭ] = affinity;
    }

}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ uint32_t gicr_enable_rdist(ಪೂರ್ಣಾಂಕ core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t ಕಾಯುವಿಕೆ = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_ಪ್ರತಿ_MS);

    ಆಗ (ಕಾಯುವಿಕೆ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        ಆದರೆ (gpt_cnt_tval >= gpt_cnt_ciel) {
            ಬರೆ("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            ಕಾಯುವಿಕೆ = false;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            ಕಾಯುವಿಕೆ = false;
        }
    }
    ಹಿಂತಿರುಗಿ ret;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ gicr_locate_interface(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ;
    ಪೂರ್ಣಾಂಕ core_id = CURRENT_CPU_ಸೂಚ್ಯಂಕ();
    ಶಬ್ದ_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    ಗಾಗಿ (ಸ್ಥಾನಾಂತರ = 0; ಸ್ಥಾನಾಂತರ < GICR_ಗಾತ್ರ; ಸ್ಥಾನಾಂತರ += GICR_ಪ್ರತಿ_CORE_ಗಾತ್ರ) {

        uint64_t typer = ((ರಚನೆ gic_rdist_map *)((ಶಬ್ದ_t)gicr_ತಳ + ಸ್ಥಾನಾಂತರ))->typer;
        ಆದರೆ ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            ಶಬ್ದ_t gicr = (ಶಬ್ದ_t)gicr_ತಳ + ಸ್ಥಾನಾಂತರ;
            ಆದರೆ (gic_rdist_map[core_id] != ಖಾಲಿ || gic_rdist_sgi_ppi_map[core_id] != ಖಾಲಿ) {
                ಬರೆ("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == ಖಾಲಿ ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == ಖಾಲಿ ? (ಶೂನ್ಯ *)gic_rdist_map[core_id] : (ಶೂನ್ಯ *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (ಶೂನ್ಯ *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (ಶೂನ್ಯ *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            ಆದರೆ (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                ಪೂರ್ಣಾಂಕ ret = gicr_enable_rdist(core_id);
                ಆದರೆ (ret == 1) {
                    halt();
                }
            }
            ನಿಲ್ಲಿಸು;
        }
    }
    ಆದರೆ (ಸ್ಥಾನಾಂತರ >= GICR_ಗಾತ್ರ) {
        ಬರೆ("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (ಪೂರ್ಣಾಂಕ)MPIDR_AFF3(mpidr), (ಪೂರ್ಣಾಂಕ)MPIDR_AFF2(mpidr),
               (ಪೂರ್ಣಾಂಕ)MPIDR_AFF1(mpidr), (ಪೂರ್ಣಾಂಕ)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ gicr_init(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ i;
    uint32_t ಆದ್ಯತೆ;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    ಆದ್ಯತೆ = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    ಗಾಗಿ (i = 0; i < SPI_ಆರಂಭ; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->ipriorityrn[i / 4] = ಆದ್ಯತೆ;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->isenabler0 = 0x0000ffff;
    ಆದರೆ (config_ಗಣ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->igroupr0 = IRQ_ಗಣ_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ cpu_iface_init(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_ಶಬ್ದ(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_ಶಬ್ದ(ICC_PMR_EL1, DEFAULT_PMR_ಮೌಲ್ಯ);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_ಶಬ್ದ(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_ಶಬ್ದ(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_ಶಬ್ದ(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

ಶೂನ್ಯ ಗಣ_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    ಶಬ್ದ_t hw_irq = IRQT_ಗಮ್ಯ_IRQ(irq);
    ಶಬ್ದ_t core = IRQT_ಗಮ್ಯ_CORE(irq);
    ಆದರೆ (HW_IRQ_IS_SGI(hw_irq)) {
        ಹಿಂತಿರುಗಿ;
    }
    ಪೂರ್ಣಾಂಕ ಶಬ್ದ = hw_irq >> 4;
    ಪೂರ್ಣಾಂಕ ಬಿಟ್_2 = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    ಆದರೆ (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } ಇಲ್ಲದಿದ್ದರೆ {
        icfgr = gic_dist->icfgrn[ಶಬ್ದ];
    }

    ಆದರೆ (trigger) {
        icfgr |= (2 << ಬಿಟ್_2);
    } ಇಲ್ಲದಿದ್ದರೆ {
        icfgr &= ~(3 << ಬಿಟ್_2);
    }

    ಆದರೆ (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } ಇಲ್ಲದಿದ್ದರೆ {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[ಶಬ್ದ] = icfgr;
    }

    ಹಿಂತಿರುಗಿ;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    dist_init();
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t mpidr = 0;
    SYSTEM_READ_ಶಬ್ದ(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_ಸೂಚ್ಯಂಕ()] = mpidr;
    ಸಕ್ರಿಯ_irq[CURRENT_CPU_ಸೂಚ್ಯಂಕ()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(ಶಬ್ದ_t ಗುರಿ)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    ಹಿಂತಿರುಗಿ ಗುರಿ <= UINT32_MAX;
}

ಶೂನ್ಯ plat_sendSGI(ಶಬ್ದ_t irq, ಶಬ್ದ_t ಗುರಿ)
{
    uint64_t sgi1r_ತಳ = sgir_ಶಬ್ದ_from_ವಾದಗಳು(irq, ಗುರಿ);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_ತಳ);
    isb();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
#ವ್ಯಾಖ್ಯಾನಿಸು MPIDR_MT(x)   (x & ಬಿಟ್(24))

ಶೂನ್ಯ ipi_send_ಗುರಿ(irq_t irq, ಶಬ್ದ_t cpu_ಗುರಿ_List)
{
    uint64_t sgi1r_ತಳ = ((ಶಬ್ದ_t) IRQT_ಗಮ್ಯ_IRQ(irq)) << ICC_SGI1_R_INTID_ಸರಿಸು;
    ಶಬ್ದ_t sgi1r[CONFIG_MAX_ಅಂಕೆ_NODES];
    ಶಬ್ದ_t ಕೊನೆಯ_aff1 = 0;

    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i < CONFIG_MAX_ಅಂಕೆ_NODES; i++) {
        sgi1r[i] = 0;
        ಆದರೆ (cpu_ಗುರಿ_List & ಬಿಟ್(i)) {
            ಶಬ್ದ_t mpidr = mpidr_map[i];
            ಶಬ್ದ_t aff1 = MPIDR_AFF1(mpidr);
            ಶಬ್ದ_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            ಪ್ರತಿಜ್ಞಾ(aff1 >= 0 && aff1 < CONFIG_MAX_ಅಂಕೆ_NODES);
            sgi1r[aff1] |= sgi1r_ತಳ | (aff1 << ICC_SGI1_R_AFF1_ಸರಿಸು) | (1 << aff0);
            ಆದರೆ (aff1 > ಕೊನೆಯ_aff1) {
                ಕೊನೆಯ_aff1 = aff1;
            }
        }
    }
    ಗಾಗಿ (ಶಬ್ದ_t i = 0; i <= ಕೊನೆಯ_aff1; i++) {
        ಆದರೆ (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

ಶೂನ್ಯ ಗಣ_IRQTarget(irq_t irq, se_L4_ಶಬ್ದ ಗುರಿ)
{
    ಆದರೆ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        ಹಿಂತಿರುಗಿ;
    }

    ಶಬ್ದ_t hw_irq = IRQT_ಗಮ್ಯ_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_ಆರಂಭ] = MPIDR_AFF_ಮುಖವಾಡ(mpidr_map[ಗುರಿ]);
}

#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

ಶಬ್ದ_t gic_vcpu_ಅಂಕೆ_list_regs;

#ಆದರೆ_ಅಂತ್ಯ /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
