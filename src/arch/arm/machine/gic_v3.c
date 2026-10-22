/*
 * Copyright 2019, DornerWorks
 * Copyright 2019, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>
#شامل <types.h>
#شامل <arch/machine/gic_v3.h>

#تعریف_کرو IRQ_سیٹ_ALL 0xffffffff

#تعریف_کرو RDIST_BANK_SZ 0x00010000
/* One GICR region and one GICR_SGI region */
#تعریف_کرو GICR_فی_CORE_سائز  (0x20000)
/* Assume 8 cores */
#تعریف_کرو GICR_سائز           (0x100000)

#تعریف_کرو GIC_DEADLINE_MS 2
#تعریف_کرو GIC_REG_چوڑائی   32

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
#تعریف_کرو ICC_SGI1R_EL1 "S3_0_C12_C11_5"
#ورنہ
#تعریف_کرو ICC_SGI1R_EL1 "p15, 0, %Q0, %R0, c12"
#اگر_ختم

#تعریف_کرو ICC_SGI1_R_INTID_سرکاؤ          (24)
#تعریف_کرو ICC_SGI1_R_AFF1_سرکاؤ           (16)
#تعریف_کرو ICC_SGI1_R_AFF2_سرکاؤ           (32)
#تعریف_کرو ICC_SGI1_R_AFF3_سرکاؤ           (48)
#تعریف_کرو ICC_SGI1_R_RS_سرکاؤ             (44)
#تعریف_کرو ICC_SGI1_R_IRM_ثنائی_ہندسہ              (40)
#تعریف_کرو ICC_SGI1_R_CPUTARGETLIST_نقاب   0xffff

متلون ساخت gic_dist_map *ثابت gic_dist = (متلون ساخت gic_dist_map *)(GICD_PPTR);
متلون خالی *ثابت gicr_بنیاد = (متلون uint8_t *)(GICR_PPTR);

لفظ_t فعال_irq[CONFIG_MAX_عدد_NODES];
متلون ساخت gic_rdist_map *gic_rdist_map[CONFIG_MAX_عدد_NODES] = { 0 };
متلون ساخت gic_rdist_sgi_ppi_map *gic_rdist_sgi_ppi_map[CONFIG_MAX_عدد_NODES] = { 0 };

#اگر_تعریف_شدہ CONFIG_ARCH_AARCH64
#تعریف_کرو MPIDR_AFF0(x) (x & 0xff)
#تعریف_کرو MPIDR_AFF1(x) ((x >> 8) & 0xff)
#تعریف_کرو MPIDR_AFF2(x) ((x >> 16) & 0xff)
#تعریف_کرو MPIDR_AFF3(x) ((x >> 32) & 0xff)
#ورنہ
#تعریف_کرو MPIDR_AFF0(x) (x & 0xff)
#تعریف_کرو MPIDR_AFF1(x) ((x >> 8) & 0xff)
#تعریف_کرو MPIDR_AFF2(x) ((x >> 16) & 0xff)
#تعریف_کرو MPIDR_AFF3(x) (0)
#اگر_ختم
#تعریف_کرو MPIDR_MT(x)   (x & ثنائی_ہندسہ(24))
#تعریف_کرو MPIDR_AFF_نقاب(x) (x & 0xff00ffffff)

ساکن لفظ_t mpidr_map[CONFIG_MAX_عدد_NODES];

ساکن اندرونی لفظ_t get_mpidr(لفظ_t core_id)
{
    واپس mpidr_map[core_id];
}

ساکن اندرونی لفظ_t get_current_mpidr(خالی)
{
    لفظ_t core_id = CURRENT_CPU_اشاریہ();
    واپس get_mpidr(core_id);
}

ساکن اندرونی uint64_t mpidr_مقصد_gic_affinity(خالی)
{
    لفظ_t mpidr = get_current_mpidr();
    uint64_t affinity = 0;
    affinity = (uint64_t)MPIDR_AFF3(mpidr) << 32 | MPIDR_AFF2(mpidr) << 16 |
               MPIDR_AFF1(mpidr) << 8  | MPIDR_AFF0(mpidr);
    واپس affinity;
}

ساکن اندرونی uint64_t sgir_لفظ_from_دلائل(لفظ_t irq, لفظ_t ہدف)
{
    uint64_t t = ہدف; /* make sure shifts below are on 64 bit */
    واپس (uint64_t) irq << ICC_SGI1_R_INTID_سرکاؤ
           | (1llu << (t & 0xf)) // AFF0 base
           | ((t >> 4)  & 0x0f) << ICC_SGI1_R_RS_سرکاؤ // AFF0 Range select
           | ((t >> 8)  & 0xff) << ICC_SGI1_R_AFF1_سرکاؤ // AFF1
           | ((t >> 16) & 0xff) << ICC_SGI1_R_AFF2_سرکاؤ // AFF2
           | ((t >> 24) & 0xff) << ICC_SGI1_R_AFF3_سرکاؤ; // AFF3
}

/* Wait for completion of a distributor change */
/** DONT_TRANSLATE */
ساکن uint32_t gicv3_do_wait_for_rwp(متلون uint32_t *ctlr_addr)
{
    uint32_t val;
    bool_t انتظار = true;
    uint32_t ret = 0;

    uint64_t gpt_cnt_tval = 0;
    uint32_t deadline_ms =  GIC_DEADLINE_MS;
    uint64_t gpt_cnt_ciel;

    /* Check the value before reading the generic timer */
    val = *ctlr_addr;
    اگر (!(val & GICD_CTLR_RWP)) {
        واپس 0;
    }
    SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_فی_MS);

    جب (انتظار) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = *ctlr_addr;

        اگر (gpt_cnt_tval >= gpt_cnt_ciel) {
            لکھو("GICV3 RWP Timeout after %u ms\n", deadline_ms);
            ret = 1;
            انتظار = false;

        } ورنہ اگر (!(val & GICD_CTLR_RWP)) {
            ret = 0;
            انتظار = false;
        }
    }
    واپس ret;
}

ساکن خالی gicv3_dist_wait_for_rwp(خالی)
{
    gicv3_do_wait_for_rwp(&gic_dist->ctlr);
}

ساکن خالی gicv3_redist_wait_for_rwp(خالی)
{
    gicv3_do_wait_for_rwp(&gic_rdist_map[CURRENT_CPU_اشاریہ()]->ctlr);
}

ساکن خالی gicv3_enable_sre(خالی)
{
    لفظ_t val;

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT
    /* Set SRE to enable register interface for GICv3 in EL2. Disable IRQ/FIQ legacy bypass.
     * Set ICC_SRE_EL2.Enable to 0, so EL1 accesses to ICC_SRE_EL1 will trap.
     *
     * SRE may be RAO/WI and DIB/DFB may be read-only aliases of ICC_SRE_EL3 on cores
     * with EL3. Writing to them is harmless.
     */
    val = GICC_SRE_EL2_SRE | GICC_SRE_EL2_DIB | GICC_SRE_EL2_DFB;
    SYSTEM_WRITE_لفظ(ICC_SRE_EL2, val);
#اگر_ختم

    /* Enable register interface for GICv3 in EL1. Disable IRQ/FIQ legacy bypass. */
    val = GICC_SRE_EL1_SRE | GICC_SRE_EL1_DIB | GICC_SRE_EL1_DFB;
    SYSTEM_WRITE_لفظ(ICC_SRE_EL1, val);
    isb();
}


BOOT_رمز ساکن خالی dist_init(خالی)
{
    لفظ_t i;
    uint32_t type;
    غیرعلامتی صحیح nr_سطریں;
    uint64_t affinity;
    uint32_t ترجیح;

    /* Disable GIC Distributor */
    gic_dist->ctlr = 0;
    gicv3_dist_wait_for_rwp();

    type = gic_dist->typer;

    nr_سطریں = GIC_REG_چوڑائی * ((type & GICD_TYPE_LINESNR) + 1);

    /* Assume level-triggered */
    برائے (i = SPI_آغاز; i < nr_سطریں; i += 16) {
        gic_dist->icfgrn[(i / 16)] = 0;
    }

    /* Default priority for global interrupts */
    ترجیح = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    برائے (i = SPI_آغاز; i < nr_سطریں; i += 4) {
        gic_dist->ipriorityrn[(i / 4)] = ترجیح;
    }
    /* Disable and clear all global interrupts */
    برائے (i = SPI_آغاز; i < nr_سطریں; i += 32) {
        gic_dist->icenablern[(i / 32)] = IRQ_سیٹ_ALL;
        gic_dist->icpendrn[(i / 32)] = IRQ_سیٹ_ALL;
    }

    /* group 1 for non-secure */
    اگر (config_سیٹ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        برائے (i = SPI_آغاز; i < nr_سطریں; i += 32) {
            gic_dist->igrouprn[(i / 32)] = IRQ_سیٹ_ALL;
        }
    }

    gic_dist->ctlr = GICD_CTLR_ARE_NS | GICD_CTLR_ENABLE_G1NS;
    gicv3_dist_wait_for_rwp();

    /* Route all global IRQs to this CPU */
    affinity = mpidr_مقصد_gic_affinity();
    برائے (i = SPI_آغاز; i < nr_سطریں; i++) {
        gic_dist->iroutern[i - SPI_آغاز] = affinity;
    }

}

BOOT_رمز ساکن uint32_t gicr_enable_rdist(صحیح core_id)
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
    gpt_cnt_ciel = gpt_cnt_tval + (deadline_ms * TICKS_فی_MS);

    جب (انتظار) {
        SYSTEM_READ_64(CNT_CT, gpt_cnt_tval);
        val = gic_rdist_map[core_id]->waker;

        اگر (gpt_cnt_tval >= gpt_cnt_ciel) {
            لکھو("GICv3: GICR_WAKER returned non-zero %x\n", val);
            ret = 1;
            انتظار = false;

        } ورنہ اگر (!(val & GICR_WAKER_ChildrenAsleep)) {
            ret = 0;
            انتظار = false;
        }
    }
    واپس ret;
}

BOOT_رمز ساکن خالی gicr_locate_interface(خالی)
{
    لفظ_t انحراف;
    صحیح core_id = CURRENT_CPU_اشاریہ();
    لفظ_t mpidr = get_current_mpidr();
    uint32_t val;

    /*
     * Iterate through all redistributor interfaces looking for one that matches
     * our mpidr.
     */
    برائے (انحراف = 0; انحراف < GICR_سائز; انحراف += GICR_فی_CORE_سائز) {

        uint64_t typer = ((ساخت gic_rdist_map *)((لفظ_t)gicr_بنیاد + انحراف))->typer;
        اگر ((typer >> 32) == ((MPIDR_AFF3(mpidr) << 24) |
                              (MPIDR_AFF2(mpidr) << 16) |
                              (MPIDR_AFF1(mpidr) <<  8) |
                              MPIDR_AFF0(mpidr))) {

            لفظ_t gicr = (لفظ_t)gicr_بنیاد + انحراف;
            اگر (gic_rdist_map[core_id] != تہی || gic_rdist_sgi_ppi_map[core_id] != تہی) {
                لکھو("GICv3: %s[%d] %p is not null\n",
                       gic_rdist_map[core_id] == تہی ? "gic_rdist_map" : "gic_rdist_sgi_ppi_map",
                       core_id,
                       gic_rdist_map[core_id] == تہی ? (خالی *)gic_rdist_map[core_id] : (خالی *)gic_rdist_sgi_ppi_map[core_id]);
                halt();
            }
            gic_rdist_map[core_id] = (خالی *)gicr;
            gic_rdist_sgi_ppi_map[core_id] = (خالی *)(gicr + RDIST_BANK_SZ);

            /*
             * GICR_WAKER should be Read-all-zeros in Non-secure world
             * and we expect redistributors to be already awoken by an earlier loader.
             * However if we get a value back then something is probably wrong.
             */
            val = gic_rdist_map[core_id]->waker;
            اگر (val & GICR_WAKER_ChildrenAsleep) {
                /* On QEMU, the redistributor may not be woken by an earlier
                 * loader, so we need to explicitly wake it here. */
                صحیح ret = gicr_enable_rdist(core_id);
                اگر (ret == 1) {
                    halt();
                }
            }
            توڑو;
        }
    }
    اگر (انحراف >= GICR_سائز) {
        لکھو("GICv3: GICR base for CPU %d %d %d %d (Logic ID %d) not found\n",
               (صحیح)MPIDR_AFF3(mpidr), (صحیح)MPIDR_AFF2(mpidr),
               (صحیح)MPIDR_AFF1(mpidr), (صحیح)MPIDR_AFF0(mpidr), core_id);
        halt();
    }


}

BOOT_رمز ساکن خالی gicr_init(خالی)
{
    صحیح i;
    uint32_t ترجیح;

    /* Find redistributor for this core. */
    gicr_locate_interface();

    /* Deactivate SGIs/PPIs */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->icactiver0 = ~0;

    /* Set priority on PPI and SGI interrupts */
    ترجیح = (GIC_PRI_IRQ << 24 | GIC_PRI_IRQ << 16 | GIC_PRI_IRQ << 8 |
                GIC_PRI_IRQ);
    برائے (i = 0; i < SPI_آغاز; i += 4) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->ipriorityrn[i / 4] = ترجیح;
    }

    /*
     * Disable all PPI interrupts, ensure all SGI interrupts are
     * enabled.
     */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->icenabler0 = 0xffff0000;
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->isenabler0 = 0x0000ffff;
    اگر (config_سیٹ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->igroupr0 = IRQ_سیٹ_ALL;
    }

    /* Set ICFGR1 for PPIs as level-triggered */
    gic_rdist_sgi_ppi_map[CURRENT_CPU_اشاریہ()]->icfgr1 = 0x0;

    gicv3_redist_wait_for_rwp();
}

BOOT_رمز ساکن خالی cpu_iface_init(خالی)
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

خالی سیٹ_IRQTrigger(irq_t irq, bool_t trigger)
{

    /* GICv3 has read-only GICR_ICFG0 for SGI with
     * default value 0xaaaaaaaa, and read-write GICR_ICFG1
     * for PPI with default 0x00000000.*/
    لفظ_t hw_irq = IRQT_مقصد_IRQ(irq);
    لفظ_t core = IRQT_مقصد_CORE(irq);
    اگر (HW_IRQ_IS_SGI(hw_irq)) {
        واپس;
    }
    صحیح لفظ = hw_irq >> 4;
    صحیح ثنائی_ہندسہ_2 = ((hw_irq & 0xf) * 2);
    uint32_t icfgr = 0;
    اگر (HW_IRQ_IS_PPI(hw_irq)) {
        icfgr = gic_rdist_sgi_ppi_map[core]->icfgr1;
    } ورنہ {
        icfgr = gic_dist->icfgrn[لفظ];
    }

    اگر (trigger) {
        icfgr |= (2 << ثنائی_ہندسہ_2);
    } ورنہ {
        icfgr &= ~(3 << ثنائی_ہندسہ_2);
    }

    اگر (HW_IRQ_IS_PPI(hw_irq)) {
        gic_rdist_sgi_ppi_map[core]->icfgr1 = icfgr;
    } ورنہ {
        /* Update GICD_ICFGR<n>. Note that the interrupt should
         * be disabled before changing the field, and this function
         * assumes the caller has disabled the interrupt. */
        gic_dist->icfgrn[لفظ] = icfgr;
    }

    واپس;
}

BOOT_رمز خالی initIRQController(خالی)
{
    dist_init();
}

BOOT_رمز خالی cpu_initLocalIRQController(خالی)
{
    لفظ_t mpidr = 0;
    SYSTEM_READ_لفظ(MPIDR, mpidr);

    mpidr_map[CURRENT_CPU_اشاریہ()] = mpidr;
    فعال_irq[CURRENT_CPU_اشاریہ()] = IRQ_NONE;

    gicr_init();
    cpu_iface_init();
}

bool_t plat_SGITargetValid(لفظ_t ہدف)
{
    /* Aff0+Aff1+Aff2+Aff3 values are not guaranteed to be contiguous
     * and the first core may have a non-zero affinity value. */
    واپس ہدف <= UINT32_MAX;
}

خالی plat_sendSGI(لفظ_t irq, لفظ_t ہدف)
{
    uint64_t sgi1r_بنیاد = sgir_لفظ_from_دلائل(irq, ہدف);
    SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r_بنیاد);
    isb();
}

#اگر_تعریف_شدہ ENABLE_SMP_SUPPORT
#تعریف_کرو MPIDR_MT(x)   (x & ثنائی_ہندسہ(24))

خالی ipi_send_ہدف(irq_t irq, لفظ_t cpu_ہدف_List)
{
    uint64_t sgi1r_بنیاد = ((لفظ_t) IRQT_مقصد_IRQ(irq)) << ICC_SGI1_R_INTID_سرکاؤ;
    لفظ_t sgi1r[CONFIG_MAX_عدد_NODES];
    لفظ_t آخری_aff1 = 0;

    برائے (لفظ_t i = 0; i < CONFIG_MAX_عدد_NODES; i++) {
        sgi1r[i] = 0;
        اگر (cpu_ہدف_List & ثنائی_ہندسہ(i)) {
            لفظ_t mpidr = mpidr_map[i];
            لفظ_t aff1 = MPIDR_AFF1(mpidr);
            لفظ_t aff0 = MPIDR_AFF0(mpidr);
            // AFF1 is assumed to be contiguous and less than CONFIG_MAX_NUM_NODES.
            // The targets are grouped by AFF1.
            أكد(aff1 >= 0 && aff1 < CONFIG_MAX_عدد_NODES);
            sgi1r[aff1] |= sgi1r_بنیاد | (aff1 << ICC_SGI1_R_AFF1_سرکاؤ) | (1 << aff0);
            اگر (aff1 > آخری_aff1) {
                آخری_aff1 = aff1;
            }
        }
    }
    برائے (لفظ_t i = 0; i <= آخری_aff1; i++) {
        اگر (sgi1r[i] != 0) {
            SYSTEM_WRITE_64(ICC_SGI1R_EL1, sgi1r[i]);
        }
    }
    isb();
}

خالی سیٹ_IRQTarget(irq_t irq, se_L4_لفظ ہدف)
{
    اگر (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        واپس;
    }

    لفظ_t hw_irq = IRQT_مقصد_IRQ(irq);
    gic_dist->iroutern[hw_irq - SPI_آغاز] = MPIDR_AFF_نقاب(mpidr_map[ہدف]);
}

#اگر_ختم /* ENABLE_SMP_SUPPORT */

#اگر_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

لفظ_t gic_vcpu_عدد_list_regs;

#اگر_ختم /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
