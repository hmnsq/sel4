/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <types.h>
#ضمن <arch/machine/gic_v3.h>

#وصف_ڪريو IRQ_سيٽ_ALL 0xffffffff

#وصف_ڪريو RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#وصف_ڪريو GICR_في_CORE_سائيز  (0x20000)
/* Assume 8 cores */
#وصف_ڪريو GICR_سائيز           (0x100000)

#وصف_ڪريو GIC_DEADLINE_MS 2
#وصف_ڪريو GIC_REG_ويڪر   32

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
#وصف_ڪريو ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#وإلا
#وصف_ڪريو ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#جيڪڏهن_پورو

#وصف_ڪريو ICC_SGI1_R_INTID_سرڪايو          (24)
#وصف_ڪريو ICC_SGI1_R_AFF1_سرڪايو           (16)
#وصف_ڪريو ICC_SGI1_R_AFF2_سرڪايو           (32)
#وصف_ڪريو ICC_SGI1_R_AFF3_سرڪايو           (48)
#وصف_ڪريو ICC_SGI1_R_RS_سرڪايو             (44)
#وصف_ڪريو ICC_SGI1_R_IRM_ٻٽو_انگ              (40)
#وصف_ڪريو ICC_SGI1_R_CPUTARGETLIST_نقاب   0xffff

متطاير بنية gic_dist_map *ثابت gic_dist = (متطاير بنية gic_dist_map *)(GICD_PPTR);
متطاير فراغ *ثابت gicr_بنياد = (متطاير uint8_t *)(GICR_PPTR);

لفظ_t سرگرم_irq[CONFIG_MAX_انگ_NODES];
متطاير بنية gic_rdist_map *gic_rdist_map[CONFIG_MAX_انگ_NODES] = { 0 };
متطاير بنية gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_انگ_NODES] = { 0 };

#جيڪڏهن_وصف_ٿيل CONFIG_ARCH_AARCH64
#وصف_ڪريو MPIDR_AFF0(x) (x & 0xff)
#وصف_ڪريو MPIDR_AFF1(x) ((x >> 8) & 0xff)
#وصف_ڪريو MPIDR_AFF2(x) ((x >> 16) & 0xff)
#وصف_ڪريو MPIDR_AFF3(x) ((x >> 32) & 0xff)
#وإلا
#وصف_ڪريو MPIDR_AFF0(x) (x & 0xff)
#وصف_ڪريو MPIDR_AFF1(x) ((x >> 8) & 0xff)
#وصف_ڪريو MPIDR_AFF2(x) ((x >> 16) & 0xff)
#وصف_ڪريو MPIDR_AFF3(x) (0)
#جيڪڏهن_پورو
#وصف_ڪريو MPIDR_MT(x)   (x & ٻٽو_انگ(24))
#وصف_ڪريو MPIDR_AFF_نقاب(x) (x & 0xff00ffffff)

ساكن لفظ_t mpidr_map[CONFIG_MAX_انگ_NODES];

ساكن مضمن لفظ_t get_mpidr(لفظ_t core_id)
{
    أرجع mpidr_map[core_id];
}

ساكن مضمن لفظ_t get_current_mpidr(فراغ)
{
    لفظ_t core_id = CURRENT_CPU_اشاريو();
    أرجع get_mpidr(core_id);
}

ساكن مضمن uint64_t mpidr_منزل_gic_affinity(فراغ)
{
    لفظ_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    أرجع affinity;
}

ساكن مضمن uint64_t sgir_لفظ_from_دليلون(لفظ_t irq, لفظ_t هدف)
{
    uint64_t t = هدف; /* make sure shifts below are on 64 bit */
    أرجع (uint64_t) irq << ICC_SGI1_R_INTID_سرڪايو
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_سرڪايو // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_سرڪايو // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_سرڪايو // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_سرڪايو; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
ساكن uint32_t gicv3_do_wait_for_rwp(متطاير uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t انتظار = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    إذا (!(val & GICD_CTLR_RWP)) {
        أرجع 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_في_MS);

    طالما (انتظار) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        إذا (gpt_cnt_tval >= gpt_cnt_ciel) {
            لکو("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            انتظار = false;

        } وإلا إذا (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            انتظار = false;
        }
    }
    أرجع ret;
}

ساكن فراغ gicv3_dist_wait_for_rwp(فراغ)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

ساكن فراغ gicv3_redist_wait_for_rwp(فراغ)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_اشاريو()]->ctlr);
}

ساكن فراغ gicv3_enable_sre(فراغ)
{
    لفظ_t val;

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_لفظ(ICC_SRE_EL2, val);
#جيڪڏهن_پورو

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_لفظ(ICC_SRE_EL1, val);
    isb();
}


BOOT_ڪوڊ ساكن فراغ dist_init(فراغ)
{
    لفظ_t i;
    uint32_t type;
    بنانشان صحيح nr_سٽون;
    uint64_t affinity;
    uint32_t ترجيح;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_سٽون = GIC_REG_ويڪر * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    لكل (i = SPI_شروعات; i < nr_سٽون; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    ترجيح = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    لكل (i = SPI_شروعات; i < nr_سٽون; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = ترجيح;
    }
    /* Disable and clear all global interrupts */
    لكل (i = SPI_شروعات; i < nr_سٽون; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_سيٽ_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_سيٽ_ALL;
    }

    /* group 1 for non-secure */
    إذا (config_سيٽ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        لكل (i = SPI_شروعات; i < nr_سٽون; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_سيٽ_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_منزل_gic_affinity();
    لكل (i = SPI_شروعات; i < nr_سٽون; i++) {
        gic_dist->iroutern[i - SPI_شروعات] = affinity;
    }

}

BOOT_ڪوڊ ساكن uint32_t gicr_enable_rdist(صحيح core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t انتظار = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_في_MS);

    طالما (انتظار) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        إذا (gpt_cnt_tval >= gpt_cnt_ciel) {
            لکو("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            انتظار = false;

        } وإلا إذا (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            انتظار = false;
        }
    }
    أرجع ret;
}

BOOT_ڪوڊ ساكن فراغ gicr_locate_interface(فراغ)
{
    لفظ_t وٿي;
    صحيح core_id = CURRENT_CPU_اشاريو();
    لفظ_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    لكل (وٿي = 0; وٿي < GICR_سائيز; وٿي += GICR_في_CORE_سائيز) {

        uint64_t typer = ((بنية gic_rdist_map *)((لفظ_t)gicr_بنياد + وٿي))->typer;
        إذا ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            لفظ_t gicr = (لفظ_t)gicr_بنياد + وٿي;
            إذا (gic_rdist_map[core_id] != عدم || gic_rdist_sgi_ppi_map[core_id] != عدم) {
                لکو("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == عدم ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == عدم ? (فراغ *)gic_rdist_map[core_id] : (فراغ *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (فراغ *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (فراغ *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            إذا (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                صحيح ret = gicr_enable_rdist(core_id);
                إذا (ret == 1) {
                    halt();
                }
            }
            اكسر;
        }
    }
    إذا (وٿي >= GICR_سائيز) {
        لکو("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (صحيح)MPIDR_AFF3(mpidr), (صحيح)MPIDR_AFF2(mpidr),
               (صحيح)MPIDR_AFF1(mpidr), (صحيح)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_ڪوڊ ساكن فراغ gicr_init(فراغ)
{
    صحيح i;
    uint32_t ترجيح;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    ترجيح = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    لكل (i = 0; i < SPI_شروعات; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->ipriorityrn[i / 4] = ترجيح;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->isenabler0 = 0x0000ffff;
    إذا (config_سيٽ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->igroupr0 = IRQ_سيٽ_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاريو()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_ڪوڊ ساكن فراغ cpu_iface_init(فراغ)
{
    لفظ_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_لفظ(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_لفظ(ICC_PMR_EL1, DEFAULT_PMR_قدر);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_لفظ(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_لفظ(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_لفظ(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

فراغ سيٽ_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    لفظ_t hw_irq = IRQT_منزل_IRQ(irq);
    لفظ_t core = IRQT_منزل_CORE(irq);
    إذا (HW_IRQ_IS_SGI(hw_irq)) {
        أرجع;
    }
    صحيح لفظ_2 = hw_irq >> 4;
    صحيح ٻٽو_انگ_2 = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    إذا (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } وإلا {
        icfgr = gic_dist->icfgrn[لفظ_2];
    }

    إذا (trigger) {
        icfgr |= (2 << ٻٽو_انگ_2);
    } وإلا {
        icfgr &= ~(3 << ٻٽو_انگ_2);
    }

    إذا (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } وإلا {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[لفظ_2] = icfgr;
    }

    أرجع;
}

BOOT_ڪوڊ فراغ initIRQController(فراغ)
{
    dist_init();
}

BOOT_ڪوڊ فراغ cpu_initLocalIRQController(فراغ)
{
    لفظ_t mpidr = 0;
    SYSTEM_READ_لفظ(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_اشاريو()] = mpidr;
    سرگرم_irq[CURRENT_CPU_اشاريو()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(لفظ_t هدف)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    أرجع هدف <= UINT32_MAX;
}

فراغ plat_sendSGI(لفظ_t irq, لفظ_t هدف)
{
    uint64_t sgi1r_بنياد = sgir_لفظ_from_دليلون(irq, هدف);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_بنياد);
    isb();
}

#جيڪڏهن_وصف_ٿيل ENABLE_SMP_SUPPORT
#وصف_ڪريو MPIDR_MT(x)   (x & ٻٽو_انگ(24))

فراغ ipi_send_هدف(irq_t irq, لفظ_t cpu_هدف_List)
{
    uint64_t sgi1r_بنياد = ((لفظ_t) IRQT_منزل_IRQ(irq)) << ICC_SGI1_R_INTID_سرڪايو;
    لفظ_t sgi1r[CONFIG_MAX_انگ_NODES];
    لفظ_t آخري_aff1 = 0;

    لكل (لفظ_t i = 0; i < CONFIG_MAX_انگ_NODES; i++) {
        sgi1r[i] = 0;
        إذا (cpu_هدف_List & ٻٽو_انگ(i)) {
            لفظ_t mpidr = mpidr_map[i];
            لفظ_t aff1 = MPIDR_AFF1(mpidr);
            لفظ_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            أكد(aff1 >= 0 && aff1 < CONFIG_MAX_انگ_NODES);
            sgi1r[aff1] |= sgi1r_بنياد | (aff1 << ICC_SGI1_R_AFF1_سرڪايو) | (1 << aff0);
            إذا (aff1 > آخري_aff1) {
                آخري_aff1 = aff1;
            }
        }
    }
    لكل (لفظ_t i = 0; i <= آخري_aff1; i++) {
        إذا (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

فراغ سيٽ_IRQTarget(irq_t irq, se_L4_لفظ هدف)
{
    إذا (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        أرجع;
    }

    لفظ_t hw_irq = IRQT_منزل_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_شروعات] = MPIDR_AFF_نقاب(mpidr_map[هدف]);
}

#جيڪڏهن_پورو /* ENABLE_SMP_SUPPORT */

#جيڪڏهن_وصف_ٿيل CONFIG_ARM_HYPERVISOR_SUPPORT

لفظ_t gic_vcpu_انگ_list_regs;

#جيڪڏهن_پورو /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
