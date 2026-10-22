/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <types.h>
#அந்தர்பாவ <arch/machine/gic_v3.h>

#define IRQ_SET_ALL 0xffffffff

#define RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#define GICR_PER_CORE_SIZE  (0x20000)
/* Assume 8 cores */
#define GICR_SIZE           (0x100000)

#define GIC_DEADLINE_MS 2
#define GIC_REG_WIDTH   32

#ifdef CONFIG_ARCH_AARCH64
#define ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#இல்லையெனில்
#define ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#endif

#define ICC_SGI1R_INTID_SHIFT          (24)
#define ICC_SGI1R_AFF1_SHIFT           (16)
#define ICC_SGI1R_AFF2_SHIFT           (32)
#define ICC_SGI1R_AFF3_SHIFT           (48)
#define ICC_SGI1R_RS_SHIFT             (44)
#define ICC_SGI1R_IRM_BIT              (40)
#define ICC_SGI1R_CPUTARGETLIST_MASK   0xffff

நிலையற்ற கட்டமைப்பு gic_dist_map *மாறிலி gic_dist = (நிலையற்ற கட்டமைப்பு gic_dist_map *)(GICD_PPTR);
நிலையற்ற வெறுமை *மாறிலி gicr_base = (நிலையற்ற uint8_t *)(GICR_PPTR);

word_t active_irq[CONFIG_MAX_இலக்கம்_NODES];
நிலையற்ற கட்டமைப்பு gic_rdist_map *gic_rdist_map[CONFIG_MAX_இலக்கம்_NODES] = { 0 };
நிலையற்ற கட்டமைப்பு gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_இலக்கம்_NODES] = { 0 };

#ifdef CONFIG_ARCH_AARCH64
#define MPIDR_AFF0(x) (x & 0xff)
#define MPIDR_AFF1(x) ((x >> 8) & 0xff)
#define MPIDR_AFF2(x) ((x >> 16) & 0xff)
#define MPIDR_AFF3(x) ((x >> 32) & 0xff)
#இல்லையெனில்
#define MPIDR_AFF0(x) (x & 0xff)
#define MPIDR_AFF1(x) ((x >> 8) & 0xff)
#define MPIDR_AFF2(x) ((x >> 16) & 0xff)
#define MPIDR_AFF3(x) (0)
#endif
#define MPIDR_MT(x)   (x & BIT(24))
#define MPIDR_AFF_MASK(x) (x & 0xff00ffffff)

நிலையான word_t mpidr_map[CONFIG_MAX_இலக்கம்_NODES];

நிலையான உள்வரி word_t get_mpidr(word_t core_id)
{
    திரும்பு mpidr_map[core_id];
}

நிலையான உள்வரி word_t get_current_mpidr(வெறுமை)
{
    word_t core_id = CURRENT_CPU_INDEX();
    திரும்பு get_mpidr(core_id);
}

நிலையான உள்வரி uint64_t mpidr_to_gic_affinity(வெறுமை)
{
    word_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    திரும்பு affinity;
}

நிலையான உள்வரி uint64_t sgir_word_from_args(word_t irq, word_t target)
{
    uint64_t t = target; /* make sure shifts below are on 64 bit */
    திரும்பு (uint64_t) irq << ICC_SGI1R_INTID_SHIFT
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1R_RS_SHIFT // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1R_AFF1_SHIFT // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1R_AFF2_SHIFT // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1R_AFF3_SHIFT; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
நிலையான uint32_t gicv3_do_wait_for_rwp(நிலையற்ற uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t waiting = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    எனில் (!(val & GICD_CTLR_RWP)) {
        திரும்பு 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_PER_MS);

    போது (waiting) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        எனில் (gpt_cnt_tval >= gpt_cnt_ciel) {
            printf("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            waiting = false;

        } இல்லையெனில் எனில் (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            waiting = false;
        }
    }
    திரும்பு ret;
}

நிலையான வெறுமை gicv3_dist_wait_for_rwp(வெறுமை)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

நிலையான வெறுமை gicv3_redist_wait_for_rwp(வெறுமை)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_INDEX()]->ctlr);
}

நிலையான வெறுமை gicv3_enable_sre(வெறுமை)
{
    word_t val;

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL2, val);
#endif

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL1, val);
    isb();
}


BOOT_CODE நிலையான வெறுமை dist_init(வெறுமை)
{
    word_t i;
    uint32_t type;
    unsigned முழு nr_வரிகள்;
    uint64_t affinity;
    uint32_t priority;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_வரிகள் = GIC_REG_WIDTH * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    க்கு (i = SPI_START; i < nr_வரிகள்; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    priority = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    க்கு (i = SPI_START; i < nr_வரிகள்; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = priority;
    }
    /* Disable and clear all global interrupts */
    க்கு (i = SPI_START; i < nr_வரிகள்; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_SET_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_SET_ALL;
    }

    /* group 1 for non-secure */
    எனில் (config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        க்கு (i = SPI_START; i < nr_வரிகள்; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_SET_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_to_gic_affinity();
    க்கு (i = SPI_START; i < nr_வரிகள்; i++) {
        gic_dist->iroutern[i - SPI_START] = affinity;
    }

}

BOOT_CODE நிலையான uint32_t gicr_enable_rdist(முழு core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t waiting = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_PER_MS);

    போது (waiting) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        எனில் (gpt_cnt_tval >= gpt_cnt_ciel) {
            printf("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            waiting = false;

        } இல்லையெனில் எனில் (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            waiting = false;
        }
    }
    திரும்பு ret;
}

BOOT_CODE நிலையான வெறுமை gicr_locate_interface(வெறுமை)
{
    word_t offset;
    முழு core_id = CURRENT_CPU_INDEX();
    word_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    க்கு (offset = 0; offset < GICR_SIZE; offset += GICR_PER_CORE_SIZE) {

        uint64_t typer = ((கட்டமைப்பு gic_rdist_map *)((word_t)gicr_base + offset))->typer;
        எனில் ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            word_t gicr = (word_t)gicr_base + offset;
            எனில் (gic_rdist_map[core_id] != வெற்று || gic_rdist_sgi_ppi_map[core_id] != வெற்று) {
                printf("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == வெற்று ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == வெற்று ? (வெறுமை *)gic_rdist_map[core_id] : (வெறுமை *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (வெறுமை *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (வெறுமை *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            எனில் (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                முழு ret = gicr_enable_rdist(core_id);
                எனில் (ret == 1) {
                    halt();
                }
            }
            நிறுத்து;
        }
    }
    எனில் (offset >= GICR_SIZE) {
        printf("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (முழு)MPIDR_AFF3(mpidr), (முழு)MPIDR_AFF2(mpidr),
               (முழு)MPIDR_AFF1(mpidr), (முழு)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_CODE நிலையான வெறுமை gicr_init(வெறுமை)
{
    முழு i;
    uint32_t priority;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    priority = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    க்கு (i = 0; i < SPI_START; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->ipriorityrn[i / 4] = priority;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->isenabler0 = 0x0000ffff;
    எனில் (config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->igroupr0 = IRQ_SET_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_INDEX()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_CODE நிலையான வெறுமை cpu_iface_init(வெறுமை)
{
    word_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_WORD(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_WORD(ICC_PMR_EL1, DEFAULT_PMR_மதிப்பு);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_WORD(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_WORD(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_WORD(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

வெறுமை setIRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    word_t hw_irq = IRQT_TO_IRQ(irq);
    word_t core = IRQT_TO_CORE(irq);
    எனில் (HW_IRQ_IS_SGI(hw_irq)) {
        திரும்பு;
    }
    முழு word = hw_irq >> 4;
    முழு bit = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    எனில் (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } இல்லையெனில் {
        icfgr = gic_dist->icfgrn[word];
    }

    எனில் (trigger) {
        icfgr |= (2 << bit);
    } இல்லையெனில் {
        icfgr &= ~(3 << bit);
    }

    எனில் (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } இல்லையெனில் {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[word] = icfgr;
    }

    திரும்பு;
}

BOOT_CODE வெறுமை initIRQController(வெறுமை)
{
    dist_init();
}

BOOT_CODE வெறுமை cpu_initLocalIRQController(வெறுமை)
{
    word_t mpidr = 0;
    SYSTEM_READ_WORD(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_INDEX()] = mpidr;
    active_irq[CURRENT_CPU_INDEX()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(word_t target)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    திரும்பு target <= UINT32_MAX;
}

வெறுமை plat_sendSGI(word_t irq, word_t target)
{
    uint64_t sgi1r_base = sgir_word_from_args(irq, target);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_base);
    isb();
}

#ifdef ENABLE_SMP_SUPPORT
#define MPIDR_MT(x)   (x & BIT(24))

வெறுமை ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    uint64_t sgi1r_base = ((word_t) IRQT_TO_IRQ(irq)) << ICC_SGI1R_INTID_SHIFT;
    word_t sgi1r[CONFIG_MAX_இலக்கம்_NODES];
    word_t last_aff1 = 0;

    க்கு (word_t i = 0; i < CONFIG_MAX_இலக்கம்_NODES; i++) {
        sgi1r[i] = 0;
        எனில் (cpuTargetList & BIT(i)) {
            word_t mpidr = mpidr_map[i];
            word_t aff1 = MPIDR_AFF1(mpidr);
            word_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            ப்ரதிஜ்ஞா(aff1 >= 0 && aff1 < CONFIG_MAX_இலக்கம்_NODES);
            sgi1r[aff1] |= sgi1r_base | (aff1 << ICC_SGI1R_AFF1_SHIFT) | (1 << aff0);
            எனில் (aff1 > last_aff1) {
                last_aff1 = aff1;
            }
        }
    }
    க்கு (word_t i = 0; i <= last_aff1; i++) {
        எனில் (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

வெறுமை setIRQTarget(irq_t irq, seL4_Word target)
{
    எனில் (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        திரும்பு;
    }

    word_t hw_irq = IRQT_TO_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_START] = MPIDR_AFF_MASK(mpidr_map[target]);
}

#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

word_t gic_vcpu_இலக்கம்_list_regs;

#endif /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
