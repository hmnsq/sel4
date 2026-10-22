/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <types.h>
#अन्तर्भाव <arch/machine/gic_v3.h>

#परिभाषय IRQ_समुच्चयः_ALL 0xffffffff

#परिभाषय RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#परिभाषय GICR_प्रत्येकम्_CORE_आकार  (0x20000)
/* Assume 8 cores */
#परिभाषय GICR_आकार           (0x100000)

#परिभाषय GIC_DEADLINE_MS 2
#परिभाषय GIC_REG_विस्तारः   32

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
#परिभाषय ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#अन्यथा
#परिभाषय ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#यदि_अन्तः

#परिभाषय ICC_SGI1_R_INTID_सरणम्          (24)
#परिभाषय ICC_SGI1_R_AFF1_सरणम्           (16)
#परिभाषय ICC_SGI1_R_AFF2_सरणम्           (32)
#परिभाषय ICC_SGI1_R_AFF3_सरणम्           (48)
#परिभाषय ICC_SGI1_R_RS_सरणम्             (44)
#परिभाषय ICC_SGI1R_IRM_BIT              (40)
#परिभाषय ICC_SGI1_R_CPUTARGETLIST_आवरणम्   0xffff

चञ्चल संरचना gic_dist_map *ध्रुव gic_dist = (चञ्चल संरचना gic_dist_map *)(GICD_PPTR);
चञ्चल शून्य *ध्रुव gicr_आधारः = (चञ्चल uint8_t *)(GICR_PPTR);

word_t सक्रिय_irq[CONFIG_MAX_अङ्कः_NODES];
चञ्चल संरचना gic_rdist_map *gic_rdist_map[CONFIG_MAX_अङ्कः_NODES] = { 0 };
चञ्चल संरचना gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_अङ्कः_NODES] = { 0 };

#यदि_परिभाषितम् CONFIG_ARCH_AARCH64
#परिभाषय MPIDR_AFF0(x) (x & 0xff)
#परिभाषय MPIDR_AFF1(x) ((x >> 8) & 0xff)
#परिभाषय MPIDR_AFF2(x) ((x >> 16) & 0xff)
#परिभाषय MPIDR_AFF3(x) ((x >> 32) & 0xff)
#अन्यथा
#परिभाषय MPIDR_AFF0(x) (x & 0xff)
#परिभाषय MPIDR_AFF1(x) ((x >> 8) & 0xff)
#परिभाषय MPIDR_AFF2(x) ((x >> 16) & 0xff)
#परिभाषय MPIDR_AFF3(x) (0)
#यदि_अन्तः
#परिभाषय MPIDR_MT(x)   (x & BIT(24))
#परिभाषय MPIDR_AFF_आवरणम्(x) (x & 0xff00ffffff)

स्थिर word_t mpidr_map[CONFIG_MAX_अङ्कः_NODES];

स्थिर अन्तःस्थ word_t get_mpidr(word_t core_id)
{
    प्रत्यागम mpidr_map[core_id];
}

स्थिर अन्तःस्थ word_t get_current_mpidr(शून्य)
{
    word_t core_id = CURRENT_CPU_सूचकांक();
    प्रत्यागम get_mpidr(core_id);
}

स्थिर अन्तःस्थ uint64_t mpidr_गन्तव्यम्_gic_affinity(शून्य)
{
    word_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    प्रत्यागम affinity;
}

स्थिर अन्तःस्थ uint64_t sgir_word_from_तर्काः(word_t irq, word_t target)
{
    uint64_t t = target; /* make sure shifts below are on 64 bit */
    प्रत्यागम (uint64_t) irq << ICC_SGI1_R_INTID_सरणम्
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_सरणम् // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_सरणम् // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_सरणम् // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_सरणम्; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
स्थिर uint32_t gicv3_do_wait_for_rwp(चञ्चल uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t प्रतीक्षा = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    यदि (!(val & GICD_CTLR_RWP)) {
        प्रत्यागम 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_प्रत्येकम्_MS);

    यावत् (प्रतीक्षा) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        यदि (gpt_cnt_tval >= gpt_cnt_ciel) {
            प्रारूप_लिख("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            प्रतीक्षा = false;

        } अन्यथा यदि (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            प्रतीक्षा = false;
        }
    }
    प्रत्यागम ret;
}

स्थिर शून्य gicv3_dist_wait_for_rwp(शून्य)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

स्थिर शून्य gicv3_redist_wait_for_rwp(शून्य)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_सूचकांक()]->ctlr);
}

स्थिर शून्य gicv3_enable_sre(शून्य)
{
    word_t val;

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL2, val);
#यदि_अन्तः

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_WORD(ICC_SRE_EL1, val);
    isb();
}


BOOT_संहिता स्थिर शून्य dist_init(शून्य)
{
    word_t i;
    uint32_t type;
    अचिह्नित पूर्णाङ्क nr_पङ्क्तयः;
    uint64_t affinity;
    uint32_t प्राथमिकता;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_पङ्क्तयः = GIC_REG_विस्तारः * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    प्रति (i = SPI_START; i < nr_पङ्क्तयः; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    प्राथमिकता = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    प्रति (i = SPI_START; i < nr_पङ्क्तयः; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = प्राथमिकता;
    }
    /* Disable and clear all global interrupts */
    प्रति (i = SPI_START; i < nr_पङ्क्तयः; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_समुच्चयः_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_समुच्चयः_ALL;
    }

    /* group 1 for non-secure */
    यदि (config_समुच्चयः(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        प्रति (i = SPI_START; i < nr_पङ्क्तयः; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_समुच्चयः_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_गन्तव्यम्_gic_affinity();
    प्रति (i = SPI_START; i < nr_पङ्क्तयः; i++) {
        gic_dist->iroutern[i - SPI_START] = affinity;
    }

}

BOOT_संहिता स्थिर uint32_t gicr_enable_rdist(पूर्णाङ्क core_id)
{
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    bool_t प्रतीक्षा = true;
    uint32_t val;
    uint64_t gpt_cnt_tval = 0;
    uint64_t gpt_cnt_ciel;
    uint32_t ret = 0;

    val = gic_rdist_map[core_id]->waker;
    val &= ~GICR_WAKER_ProcessorSleep;
    gic_rdist_map[core_id]->waker = val;

    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_प्रत्येकम्_MS);

    यावत् (प्रतीक्षा) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        यदि (gpt_cnt_tval >= gpt_cnt_ciel) {
            प्रारूप_लिख("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            प्रतीक्षा = false;

        } अन्यथा यदि (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            प्रतीक्षा = false;
        }
    }
    प्रत्यागम ret;
}

BOOT_संहिता स्थिर शून्य gicr_locate_interface(शून्य)
{
    word_t विस्थापनम्;
    पूर्णाङ्क core_id = CURRENT_CPU_सूचकांक();
    word_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    प्रति (विस्थापनम् = 0; विस्थापनम् < GICR_आकार; विस्थापनम् += GICR_प्रत्येकम्_CORE_आकार) {

        uint64_t typer = ((संरचना gic_rdist_map *)((word_t)gicr_आधारः + विस्थापनम्))->typer;
        यदि ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            word_t gicr = (word_t)gicr_आधारः + विस्थापनम्;
            यदि (gic_rdist_map[core_id] != रिक्त || gic_rdist_sgi_ppi_map[core_id] != रिक्त) {
                प्रारूप_लिख("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == रिक्त ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == रिक्त ? (शून्य *)gic_rdist_map[core_id] : (शून्य *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (शून्य *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (शून्य *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            यदि (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                पूर्णाङ्क ret = gicr_enable_rdist(core_id);
                यदि (ret == 1) {
                    halt();
                }
            }
            भङ्ग;
        }
    }
    यदि (विस्थापनम् >= GICR_आकार) {
        प्रारूप_लिख("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (पूर्णाङ्क)MPIDR_AFF3(mpidr), (पूर्णाङ्क)MPIDR_AFF2(mpidr),
               (पूर्णाङ्क)MPIDR_AFF1(mpidr), (पूर्णाङ्क)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_संहिता स्थिर शून्य gicr_init(शून्य)
{
    पूर्णाङ्क i;
    uint32_t प्राथमिकता;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    प्राथमिकता = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    प्रति (i = 0; i < SPI_START; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->ipriorityrn[i / 4] = प्राथमिकता;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->isenabler0 = 0x0000ffff;
    यदि (config_समुच्चयः(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->igroupr0 = IRQ_समुच्चयः_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_सूचकांक()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_संहिता स्थिर शून्य cpu_iface_init(शून्य)
{
    word_t icc_ctlr = 0;

    /* Enable system registers */
    gicv3_enable_sre();

    /* No priority grouping: ICC_BPR1_EL1 */
    SYSTEM_WRITE_WORD(ICC_BPR1_EL1, 0);

    /* Set priority mask register: ICC_PMR_EL1 */
    SYSTEM_WRITE_WORD(ICC_PMR_EL1, DEFAULT_PMR_मानम्);

    /* EOI drops priority of the interrupt, deactivation happens separately: ICC_CTLR_EL1 */
    SYSTEM_READ_WORD(ICC_CTLR_EL1, icc_ctlr);
    icc_ctlr |= GICC_CTLR_EL1_EOImode_drop;
    SYSTEM_WRITE_WORD(ICC_CTLR_EL1, icc_ctlr);

    /* Enable Group1 interrupts: ICC_IGRPEN1_EL1 */
    SYSTEM_WRITE_WORD(ICC_IGRPEN1_EL1, 1);

    /* Sync at once at the end of cpu interface configuration */
    isb();
}

शून्य समुच्चयः_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    word_t hw_irq = IRQT_गन्तव्यम्_IRQ(irq);
    word_t core = IRQT_गन्तव्यम्_CORE(irq);
    यदि (HW_IRQ_IS_SGI(hw_irq)) {
        प्रत्यागम;
    }
    पूर्णाङ्क word = hw_irq >> 4;
    पूर्णाङ्क bit = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    यदि (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } अन्यथा {
        icfgr = gic_dist->icfgrn[word];
    }

    यदि (trigger) {
        icfgr |= (2 << bit);
    } अन्यथा {
        icfgr &= ~(3 << bit);
    }

    यदि (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } अन्यथा {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[word] = icfgr;
    }

    प्रत्यागम;
}

BOOT_संहिता शून्य initIRQController(शून्य)
{
    dist_init();
}

BOOT_संहिता शून्य cpu_initLocalIRQController(शून्य)
{
    word_t mpidr = 0;
    SYSTEM_READ_WORD(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_सूचकांक()] = mpidr;
    सक्रिय_irq[CURRENT_CPU_सूचकांक()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(word_t target)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    प्रत्यागम target <= UINT32_MAX;
}

शून्य plat_sendSGI(word_t irq, word_t target)
{
    uint64_t sgi1r_आधारः = sgir_word_from_तर्काः(irq, target);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_आधारः);
    isb();
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
#परिभाषय MPIDR_MT(x)   (x & BIT(24))

शून्य ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    uint64_t sgi1r_आधारः = ((word_t) IRQT_गन्तव्यम्_IRQ(irq)) << ICC_SGI1_R_INTID_सरणम्;
    word_t sgi1r[CONFIG_MAX_अङ्कः_NODES];
    word_t चरमः_aff1 = 0;

    प्रति (word_t i = 0; i < CONFIG_MAX_अङ्कः_NODES; i++) {
        sgi1r[i] = 0;
        यदि (cpuTargetList & BIT(i)) {
            word_t mpidr = mpidr_map[i];
            word_t aff1 = MPIDR_AFF1(mpidr);
            word_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            प्रतिज्ञा(aff1 >= 0 && aff1 < CONFIG_MAX_अङ्कः_NODES);
            sgi1r[aff1] |= sgi1r_आधारः | (aff1 << ICC_SGI1_R_AFF1_सरणम्) | (1 << aff0);
            यदि (aff1 > चरमः_aff1) {
                चरमः_aff1 = aff1;
            }
        }
    }
    प्रति (word_t i = 0; i <= चरमः_aff1; i++) {
        यदि (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

शून्य समुच्चयः_IRQTarget(irq_t irq, seL4_Word target)
{
    यदि (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        प्रत्यागम;
    }

    word_t hw_irq = IRQT_गन्तव्यम्_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_START] = MPIDR_AFF_आवरणम्(mpidr_map[target]);
}

#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

word_t gic_vcpu_अङ्कः_list_regs;

#यदि_अन्तः /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
