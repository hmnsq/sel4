/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <types.h>
#అన్తర్భావ <arch/machine/gic_v3.h>

#నిర్వచించు IRQ_సమితి_ALL 0xffffffff

#నిర్వచించు RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#నిర్వచించు GICR_చొప్పున_CORE_కొలత  (0x20000)
/* Assume 8 cores */
#నిర్వచించు GICR_కొలత           (0x100000)

#నిర్వచించు GIC_DEADLINE_MS 2
#నిర్వచించు GIC_REG_వెడల్పు   32

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
#నిర్వచించు ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#లేకపోతే
#నిర్వచించు ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#అయితే_ముగింపు

#నిర్వచించు ICC_SGI1_R_INTID_జరుపు          (24)
#నిర్వచించు ICC_SGI1_R_AFF1_జరుపు           (16)
#నిర్వచించు ICC_SGI1_R_AFF2_జరుపు           (32)
#నిర్వచించు ICC_SGI1_R_AFF3_జరుపు           (48)
#నిర్వచించు ICC_SGI1_R_RS_జరుపు             (44)
#నిర్వచించు ICC_SGI1_R_IRM_బిట్టు              (40)
#నిర్వచించు ICC_SGI1_R_CPUTARGETLIST_ముసుగు   0xffff

అస్థిర నిర్మాణం gic_dist_map *స్థిరరాశి gic_dist = (అస్థిర నిర్మాణం gic_dist_map *)(GICD_PPTR);
అస్థిర శూన్య *స్థిరరాశి gicr_పునాది = (అస్థిర uint8_t *)(GICR_PPTR);

మాట_t చురుకైన_irq[CONFIG_MAX_అంకె_NODES];
అస్థిర నిర్మాణం gic_rdist_map *gic_rdist_map[CONFIG_MAX_అంకె_NODES] = { 0 };
అస్థిర నిర్మాణం gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_అంకె_NODES] = { 0 };

#నిర్వచితమైతే CONFIG_ARCH_AARCH64
#నిర్వచించు MPIDR_AFF0(x) (x & 0xff)
#నిర్వచించు MPIDR_AFF1(x) ((x >> 8) & 0xff)
#నిర్వచించు MPIDR_AFF2(x) ((x >> 16) & 0xff)
#నిర్వచించు MPIDR_AFF3(x) ((x >> 32) & 0xff)
#లేకపోతే
#నిర్వచించు MPIDR_AFF0(x) (x & 0xff)
#నిర్వచించు MPIDR_AFF1(x) ((x >> 8) & 0xff)
#నిర్వచించు MPIDR_AFF2(x) ((x >> 16) & 0xff)
#నిర్వచించు MPIDR_AFF3(x) (0)
#అయితే_ముగింపు
#నిర్వచించు MPIDR_MT(x)   (x & బిట్టు(24))
#నిర్వచించు MPIDR_AFF_ముసుగు(x) (x & 0xff00ffffff)

స్థిర మాట_t mpidr_map[CONFIG_MAX_అంకె_NODES];

స్థిర అంతఃవరుస మాట_t get_mpidr(మాట_t core_id)
{
    తిరిగి mpidr_map[core_id];
}

స్థిర అంతఃవరుస మాట_t get_current_mpidr(శూన్య)
{
    మాట_t core_id = CURRENT_CPU_సూచీ();
    తిరిగి get_mpidr(core_id);
}

స్థిర అంతఃవరుస uint64_t mpidr_గమ్యం_gic_affinity(శూన్య)
{
    మాట_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    తిరిగి affinity;
}

స్థిర అంతఃవరుస uint64_t sgir_మాట_from_వాదనలు(మాట_t irq, మాట_t లక్ష్యం)
{
    uint64_t t = లక్ష్యం; /* make sure shifts below are on 64 bit */
    తిరిగి (uint64_t) irq << ICC_SGI1_R_INTID_జరుపు
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_జరుపు // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_జరుపు // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_జరుపు // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_జరుపు; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
స్థిర uint32_t gicv3_do_wait_for_rwp(అస్థిర uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t నిరీక్షణ = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    ఒకవేళ (!(val & GICD_CTLR_RWP)) {
        తిరిగి 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_చొప్పున_MS);

    అయితే (నిరీక్షణ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        ఒకవేళ (gpt_cnt_tval >= gpt_cnt_ciel) {
            రాయి("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            నిరీక్షణ = false;

        } లేకపోతే ఒకవేళ (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            నిరీక్షణ = false;
        }
    }
    తిరిగి ret;
}

స్థిర శూన్య gicv3_dist_wait_for_rwp(శూన్య)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

స్థిర శూన్య gicv3_redist_wait_for_rwp(శూన్య)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_సూచీ()]->ctlr);
}

స్థిర శూన్య gicv3_enable_sre(శూన్య)
{
    మాట_t val;

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_మాట(ICC_SRE_EL2, val);
#అయితే_ముగింపు

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_మాట(ICC_SRE_EL1, val);
    isb();
}


BOOT_సంకేతం స్థిర శూన్య dist_init(శూన్య)
{
    మాట_t i;
    uint32_t type;
    గుర్తులేని పూర్ణాంకం nr_పంక్తులు;
    uint64_t affinity;
    uint32_t ప్రాధాన్యత;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_పంక్తులు = GIC_REG_వెడల్పు * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    కొరకు (i = SPI_ప్రారంభం; i < nr_పంక్తులు; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    ప్రాధాన్యత = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    కొరకు (i = SPI_ప్రారంభం; i < nr_పంక్తులు; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = ప్రాధాన్యత;
    }
    /* Disable and clear all global interrupts */
    కొరకు (i = SPI_ప్రారంభం; i < nr_పంక్తులు; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_సమితి_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_సమితి_ALL;
    }

    /* group 1 for non-secure */
    ఒకవేళ (config_సమితి(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        కొరకు (i = SPI_ప్రారంభం; i < nr_పంక్తులు; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_సమితి_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_గమ్యం_gic_affinity();
    కొరకు (i = SPI_ప్రారంభం; i < nr_పంక్తులు; i++) {
        gic_dist->iroutern[i - SPI_ప్రారంభం] = affinity;
    }

}

BOOT_సంకేతం స్థిర uint32_t gicr_enable_rdist(పూర్ణాంకం core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t నిరీక్షణ = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_చొప్పున_MS);

    అయితే (నిరీక్షణ) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        ఒకవేళ (gpt_cnt_tval >= gpt_cnt_ciel) {
            రాయి("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            నిరీక్షణ = false;

        } లేకపోతే ఒకవేళ (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            నిరీక్షణ = false;
        }
    }
    తిరిగి ret;
}

BOOT_సంకేతం స్థిర శూన్య gicr_locate_interface(శూన్య)
{
    మాట_t స్థానభ్రంశం;
    పూర్ణాంకం core_id = CURRENT_CPU_సూచీ();
    మాట_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    కొరకు (స్థానభ్రంశం = 0; స్థానభ్రంశం < GICR_కొలత; స్థానభ్రంశం += GICR_చొప్పున_CORE_కొలత) {

        uint64_t typer = ((నిర్మాణం gic_rdist_map *)((మాట_t)gicr_పునాది + స్థానభ్రంశం))->typer;
        ఒకవేళ ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            మాట_t gicr = (మాట_t)gicr_పునాది + స్థానభ్రంశం;
            ఒకవేళ (gic_rdist_map[core_id] != ఖాళీ || gic_rdist_sgi_ppi_map[core_id] != ఖాళీ) {
                రాయి("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == ఖాళీ ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == ఖాళీ ? (శూన్య *)gic_rdist_map[core_id] : (శూన్య *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (శూన్య *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (శూన్య *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            ఒకవేళ (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                పూర్ణాంకం ret = gicr_enable_rdist(core_id);
                ఒకవేళ (ret == 1) {
                    halt();
                }
            }
            ఆపు;
        }
    }
    ఒకవేళ (స్థానభ్రంశం >= GICR_కొలత) {
        రాయి("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (పూర్ణాంకం)MPIDR_AFF3(mpidr), (పూర్ణాంకం)MPIDR_AFF2(mpidr),
               (పూర్ణాంకం)MPIDR_AFF1(mpidr), (పూర్ణాంకం)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_సంకేతం స్థిర శూన్య gicr_init(శూన్య)
{
    పూర్ణాంకం i;
    uint32_t ప్రాధాన్యత;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    ప్రాధాన్యత = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    కొరకు (i = 0; i < SPI_ప్రారంభం; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->ipriorityrn[i / 4] = ప్రాధాన్యత;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->isenabler0 = 0x0000ffff;
    ఒకవేళ (config_సమితి(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->igroupr0 = IRQ_సమితి_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_సూచీ()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_సంకేతం స్థిర శూన్య cpu_iface_init(శూన్య)
{
    మాట_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_మాట(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_మాట(ICC_PMR_EL1, DEFAULT_PMR_విలువ);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_మాట(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_మాట(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_మాట(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

శూన్య సమితి_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    మాట_t hw_irq = IRQT_గమ్యం_IRQ(irq);
    మాట_t core = IRQT_గమ్యం_CORE(irq);
    ఒకవేళ (HW_IRQ_IS_SGI(hw_irq)) {
        తిరిగి;
    }
    పూర్ణాంకం మాట = hw_irq >> 4;
    పూర్ణాంకం బిట్టు_2 = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    ఒకవేళ (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } లేకపోతే {
        icfgr = gic_dist->icfgrn[మాట];
    }

    ఒకవేళ (trigger) {
        icfgr |= (2 << బిట్టు_2);
    } లేకపోతే {
        icfgr &= ~(3 << బిట్టు_2);
    }

    ఒకవేళ (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } లేకపోతే {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[మాట] = icfgr;
    }

    తిరిగి;
}

BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    dist_init();
}

BOOT_సంకేతం శూన్య cpu_initLocalIRQController(శూన్య)
{
    మాట_t mpidr = 0;
    SYSTEM_READ_మాట(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_సూచీ()] = mpidr;
    చురుకైన_irq[CURRENT_CPU_సూచీ()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(మాట_t లక్ష్యం)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    తిరిగి లక్ష్యం <= UINT32_MAX;
}

శూన్య plat_sendSGI(మాట_t irq, మాట_t లక్ష్యం)
{
    uint64_t sgi1r_పునాది = sgir_మాట_from_వాదనలు(irq, లక్ష్యం);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_పునాది);
    isb();
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
#నిర్వచించు MPIDR_MT(x)   (x & బిట్టు(24))

శూన్య ipi_send_లక్ష్యం(irq_t irq, మాట_t cpu_లక్ష్యం_List)
{
    uint64_t sgi1r_పునాది = ((మాట_t) IRQT_గమ్యం_IRQ(irq)) << ICC_SGI1_R_INTID_జరుపు;
    మాట_t sgi1r[CONFIG_MAX_అంకె_NODES];
    మాట_t చివరి_aff1 = 0;

    కొరకు (మాట_t i = 0; i < CONFIG_MAX_అంకె_NODES; i++) {
        sgi1r[i] = 0;
        ఒకవేళ (cpu_లక్ష్యం_List & బిట్టు(i)) {
            మాట_t mpidr = mpidr_map[i];
            మాట_t aff1 = MPIDR_AFF1(mpidr);
            మాట_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            ప్రతిజ్ఞా(aff1 >= 0 && aff1 < CONFIG_MAX_అంకె_NODES);
            sgi1r[aff1] |= sgi1r_పునాది | (aff1 << ICC_SGI1_R_AFF1_జరుపు) | (1 << aff0);
            ఒకవేళ (aff1 > చివరి_aff1) {
                చివరి_aff1 = aff1;
            }
        }
    }
    కొరకు (మాట_t i = 0; i <= చివరి_aff1; i++) {
        ఒకవేళ (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

శూన్య సమితి_IRQTarget(irq_t irq, se_L4_మాట లక్ష్యం)
{
    ఒకవేళ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        తిరిగి;
    }

    మాట_t hw_irq = IRQT_గమ్యం_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_ప్రారంభం] = MPIDR_AFF_ముసుగు(mpidr_map[లక్ష్యం]);
}

#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

మాట_t gic_vcpu_అంకె_list_regs;

#అయితే_ముగింపు /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
