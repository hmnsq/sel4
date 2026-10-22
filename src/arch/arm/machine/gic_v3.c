/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <types.h>
#समावेश <arch/machine/gic_v3.h>

#घोषणा IRQ_SET_ALL 0xffffffff

#घोषणा RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#घोषणा GICR_PER_CORE_आकार  (0x20000)
/* Assume 8 cores */
#घोषणा GICR_आकार           (0x100000)

#घोषणा GIC_DEADLINE_MS 2
#घोषणा GIC_REG_WIDTH   32

#अगर_घोषित CONFIG_ARCH_AARCH64
#घोषणा ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#अन्यथा
#घोषणा ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#पूर्ण_अगर

#घोषणा ICC_SGI1R_INTID_SHIFT          (24)
#घोषणा ICC_SGI1R_AFF1_SHIFT           (16)
#घोषणा ICC_SGI1R_AFF2_SHIFT           (32)
#घोषणा ICC_SGI1R_AFF3_SHIFT           (48)
#घोषणा ICC_SGI1R_RS_SHIFT             (44)
#घोषणा ICC_SGI1R_IRM_BIT              (40)
#घोषणा ICC_SGI1R_CPUTARGETLIST_MASK   0xffff

अस्थिर काष्ठा gic_dist_map *स्थिर gic_dist = (अस्थिर काष्ठा gic_dist_map *)(GICD_PPTR);
अस्थिर व्योम *स्थिर gicr_base = (अस्थिर uint8_t *)(GICR_PPTR);

word_t active_irq[CONFIG_MAX_अंक_NODES];
अस्थिर काष्ठा gic_rdist_map *gic_rdist_map[CONFIG_MAX_अंक_NODES] = { 0 };
अस्थिर काष्ठा gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_अंक_NODES] = { 0 };

#अगर_घोषित CONFIG_ARCH_AARCH64
#घोषणा MPIDR_AFF0(x) (x & 0xff)
#घोषणा MPIDR_AFF1(x) ((x >> 8) & 0xff)
#घोषणा MPIDR_AFF2(x) ((x >> 16) & 0xff)
#घोषणा MPIDR_AFF3(x) ((x >> 32) & 0xff)
#अन्यथा
#घोषणा MPIDR_AFF0(x) (x & 0xff)
#घोषणा MPIDR_AFF1(x) ((x >> 8) & 0xff)
#घोषणा MPIDR_AFF2(x) ((x >> 16) & 0xff)
#घोषणा MPIDR_AFF3(x) (0)
#पूर्ण_अगर
#घोषणा MPIDR_MT(x)   (x & BIT(24))
#घोषणा MPIDR_AFF_MASK(x) (x & 0xff00ffffff)

अटल word_t mpidr_map[CONFIG_MAX_अंक_NODES];

अटल अंतरभूत word_t get_mpidr(word_t core_id)
{
    वापस mpidr_map[core_id];
}

अटल अंतरभूत word_t get_current_mpidr(व्योम)
{
    word_t core_id = CURRENT_CPU_सूचकांक();
    वापस get_mpidr(core_id);
}

अटल अंतरभूत uint64_t mpidr_to_gic_affinity(व्योम)
{
    word_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    वापस affinity;
}

अटल अंतरभूत uint64_t sgir_word_from_args(word_t irq, word_t target)
{
    uint64_t t = target; /* make sure shifts below are on 64 bit */
    वापस (uint64_t) irq << ICC_SGI1R_INTID_SHIFT
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1R_RS_SHIFT // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1R_AFF1_SHIFT // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1R_AFF2_SHIFT // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1R_AFF3_SHIFT; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
अटल uint32_t gicv3_do_wait_for_rwp(अस्थिर uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t waiting = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    अगर (!(val & GICD_CTLR_RWP)) {
        वापस 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_PER_MS);

    जबतक (waiting) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        अगर (gpt_cnt_tval >= gpt_cnt_ciel) {
            म_लिखो("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            waiting = false;

        } अन्यथा अगर (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            waiting = false;
        }
    }
    वापस ret;
}

अटल व्योम gicv3_dist_wait_for_rwp(व्योम)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

अटल व्योम gicv3_redist_wait_for_rwp(व्योम)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_सूचकांक()]->ctlr);
}

अटल व्योम gicv3_enable_sre(व्योम)
{
    word_t val;

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL2, val);
#पूर्ण_अगर

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL1, val);
    isb();
}


BOOT_CODE अटल व्योम dist_init(व्योम)
{
    word_t i;
    uint32_t type;
    अचिन्हित पूर्णांक nr_पंक्तियाँ;
    uint64_t affinity;
    uint32_t priority;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_पंक्तियाँ = GIC_REG_WIDTH * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    क्रम (i = SPI_START; i < nr_पंक्तियाँ; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    priority = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    क्रम (i = SPI_START; i < nr_पंक्तियाँ; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = priority;
    }
    /* Disable and clear all global interrupts */
    क्रम (i = SPI_START; i < nr_पंक्तियाँ; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_SET_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_SET_ALL;
    }

    /* group 1 for non-secure */
    अगर (config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        क्रम (i = SPI_START; i < nr_पंक्तियाँ; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_SET_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_to_gic_affinity();
    क्रम (i = SPI_START; i < nr_पंक्तियाँ; i++) {
        gic_dist->iroutern[i - SPI_START] = affinity;
    }

}

BOOT_CODE अटल uint32_t gicr_enable_rdist(पूर्णांक core_id)
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

    जबतक (waiting) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        अगर (gpt_cnt_tval >= gpt_cnt_ciel) {
            म_लिखो("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            waiting = false;

        } अन्यथा अगर (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            waiting = false;
        }
    }
    वापस ret;
}

BOOT_CODE अटल व्योम gicr_locate_interface(व्योम)
{
    word_t offset;
    पूर्णांक core_id = CURRENT_CPU_सूचकांक();
    word_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    क्रम (offset = 0; offset < GICR_आकार; offset += GICR_PER_CORE_आकार) {

        uint64_t typer = ((काष्ठा gic_rdist_map *)((word_t)gicr_base + offset))->typer;
        अगर ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            word_t gicr = (word_t)gicr_base + offset;
            अगर (gic_rdist_map[core_id] != शून्य || gic_rdist_sgi_ppi_map[core_id] != शून्य) {
                म_लिखो("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == शून्य ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == शून्य ? (व्योम *)gic_rdist_map[core_id] : (व्योम *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (व्योम *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (व्योम *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            अगर (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                पूर्णांक ret = gicr_enable_rdist(core_id);
                अगर (ret == 1) {
                    halt();
                }
            }
            अवरोध;
        }
    }
    अगर (offset >= GICR_आकार) {
        म_लिखो("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (पूर्णांक)MPIDR_AFF3(mpidr), (पूर्णांक)MPIDR_AFF2(mpidr),
               (पूर्णांक)MPIDR_AFF1(mpidr), (पूर्णांक)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_CODE अटल व्योम gicr_init(व्योम)
{
    पूर्णांक i;
    uint32_t priority;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    priority = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    क्रम (i = 0; i < SPI_START; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->ipriorityrn[i / 4] = priority;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->isenabler0 = 0x0000ffff;
    अगर (config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->igroupr0 = IRQ_SET_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_CODE अटल व्योम cpu_iface_init(व्योम)
{
    word_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_WORD(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_WORD(ICC_PMR_EL1, DEFAULT_PMR_मूल्य);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_WORD(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_WORD(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_WORD(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

व्योम setIRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    word_t hw_irq = IRQT_TO_IRQ(irq);
    word_t core = IRQT_TO_CORE(irq);
    अगर (HW_IRQ_IS_SGI(hw_irq)) {
        वापस;
    }
    पूर्णांक word = hw_irq >> 4;
    पूर्णांक bit = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    अगर (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } अन्यथा {
        icfgr = gic_dist->icfgrn[word];
    }

    अगर (trigger) {
        icfgr |= (2 << bit);
    } अन्यथा {
        icfgr &= ~(3 << bit);
    }

    अगर (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } अन्यथा {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[word] = icfgr;
    }

    वापस;
}

BOOT_CODE व्योम initIRQController(व्योम)
{
    dist_init();
}

BOOT_CODE व्योम cpu_initLocalIRQController(व्योम)
{
    word_t mpidr = 0;
    SYSTEM_READ_WORD(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_सूचकांक()] = mpidr;
    active_irq[CURRENT_CPU_सूचकांक()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(word_t target)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    वापस target <= UINT32_MAX;
}

व्योम plat_sendSGI(word_t irq, word_t target)
{
    uint64_t sgi1r_base = sgir_word_from_args(irq, target);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_base);
    isb();
}

#अगर_घोषित ENABLE_SMP_SUPPORT
#घोषणा MPIDR_MT(x)   (x & BIT(24))

व्योम ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    uint64_t sgi1r_base = ((word_t) IRQT_TO_IRQ(irq)) << ICC_SGI1R_INTID_SHIFT;
    word_t sgi1r[CONFIG_MAX_अंक_NODES];
    word_t last_aff1 = 0;

    क्रम (word_t i = 0; i < CONFIG_MAX_अंक_NODES; i++) {
        sgi1r[i] = 0;
        अगर (cpuTargetList & BIT(i)) {
            word_t mpidr = mpidr_map[i];
            word_t aff1 = MPIDR_AFF1(mpidr);
            word_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            निश्चित(aff1 >= 0 && aff1 < CONFIG_MAX_अंक_NODES);
            sgi1r[aff1] |= sgi1r_base | (aff1 << ICC_SGI1R_AFF1_SHIFT) | (1 << aff0);
            अगर (aff1 > last_aff1) {
                last_aff1 = aff1;
            }
        }
    }
    क्रम (word_t i = 0; i <= last_aff1; i++) {
        अगर (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

व्योम setIRQTarget(irq_t irq, seL4_Word target)
{
    अगर (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        वापस;
    }

    word_t hw_irq = IRQT_TO_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_START] = MPIDR_AFF_MASK(mpidr_map[target]);
}

#पूर्ण_अगर /* ENABLE_SMP_SUPPORT */

#अगर_घोषित CONFIG_ARM_HYPERVISOR_SUPPORT

word_t gic_vcpu_अंक_list_regs;

#पूर्ण_अगर /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
