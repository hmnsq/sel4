/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/gic_v2.h>

#تعریف_کرو نشانہ_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#تعریف_کرو نشانہ_CPU0_ALLINT   نشانہ_CPU_ALLINT(ثنائی_ہندسہ(0))

/* Use this to forward interrupts to all CPUs when debugging */
#تعریف_کرو نشانہ_CPU_ALL_ALLINT   نشانہ_CPU_ALLINT(0xff)

#تعریف_کرو IRQ_سیٹ_ALL 0xffffffff

#جے_بنا_تعریف GIC_V2_DISTRIBUTOR_PPTR
#غلطی GIC_V2_DISTRIBUTOR_PPTR must be defined لكل مجازی حافظہ access منزل the gic distributer
#وإلا  /* GIC_DISTRIBUTOR_PPTR */
متطاير بنية gic_dist_map *ثابت gic_dist =
    (متطاير بنية gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#جے_انت /* GIC_DISTRIBUTOR_PPTR */

#جے_بنا_تعریف GIC_V2_CONTROLLER_PPTR
#غلطی GIC_V2_CONTROLLER_PPTR must be defined لكل مجازی حافظہ access منزل the gic cpu interface
#وإلا  /* GIC_CONTROLLER_PPTR */
متطاير بنية gic_cpu_iface_map *ثابت gic_cpuiface =
    (متطاير بنية gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#جے_انت /* GIC_CONTROLLER_PPTR */

لفظ_t سرگرم_irq[CONFIG_MAX_عدد_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_کوڈ ساكن uint8_t infer_cpu_gic_id(صحيح nirqs)
{
    لفظ_t i;
    uint32_t نشانہ = 0;
    لكل (i = 0; i < nirqs; i += 4) {
        نشانہ = gic_dist->targets[i >> 2];
        نشانہ |= نشانہ >> 16;
        نشانہ |= نشانہ >> 8;
        إذا (نشانہ) {
            اكسر;
        }
    }
    إذا (!نشانہ) {
        لکھو("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        نشانہ = ثنائی_ہندسہ(0);
    }
    أرجع نشانہ & 0xff;
}

BOOT_کوڈ ساكن فراغ dist_init(فراغ)
{
    لفظ_t i;
    صحيح nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    لكل (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_سیٹ_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_سیٹ_ALL;
    }

    /* reset interrupts priority */
    لكل (i = 32; i < nirqs; i += 4) {
        إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->ترجیح[i >> 2] = 0x80808080;
        } وإلا {
            gic_dist->ترجیح[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t نشانہ = infer_cpu_gic_id(nirqs);
    لكل (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = نشانہ_CPU_ALLINT(نشانہ);
    }

    /* level-triggered, 1-N */
    لكل (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    لكل (i = 0; i < nirqs; i += 32) {
        إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_سیٹ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } وإلا {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_کوڈ ساكن فراغ cpu_iface_init(فراغ)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_سیٹ_ALL;
    gic_dist->pending_clr[0] = IRQ_سیٹ_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    إذا (config_سیٹ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_سیٹ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->ترجیح[0] = 0x80808080;
    } وإلا {
        gic_dist->security[0] = 0;
        gic_dist->ترجیح[0] = 0x0;
    }

    /* clear any software generated interrupts */
    لكل (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_سیٹ_ALL;
    }

    gic_cpuiface->icontrol = 0;
    /* the write to priority mask is ignored if the kernel is
     * in non-secure mode and the priority mask is already configured
     * by secure mode software. the elfloader should config the
     * interrupt routing properly to ensure that the hyp-mode kernel
     * can get interrupts
     */
    gic_cpuiface->pri_msk_c = 0x000000f0;
    gic_cpuiface->pb_c = 0x00000003;

    i = gic_cpuiface->int_ack;
    طالما ((i & IRQ_نقاب) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

فراغ سیٹ_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    لفظ_t اشاریہ = IRQT_منزل_IRQ(irq) / 16u;
    لفظ_t وتھ = (IRQT_منزل_IRQ(irq) % 16u) * 2;
    إذا (trigger) {
        /* set the bit */
        gic_dist->config[اشاریہ] |= ثنائی_ہندسہ(وتھ + 1);
    } وإلا {
        gic_dist->config[اشاریہ] &= ~ثنائی_ہندسہ(وتھ + 1);
    }
}

BOOT_کوڈ فراغ initIRQController(فراغ)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    أكد(INT_STATE_ARRAY_ناپ < IRQT_منزل_IRQ(irqInvalid));
    dist_init();
}

BOOT_کوڈ فراغ cpu_initLocalIRQController(فراغ)
{
    سرگرم_irq[CURRENT_CPU_اشاریہ()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(لفظ_t نشانہ)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    أرجع نشانہ <= GIC_SGI_عدد_TARGETS - 1;
}

فراغ plat_sendSGI(لفظ_t irq, لفظ_t نشانہ)
{
    gic_dist->sgi_control = (ثنائی_ہندسہ(نشانہ) << (GICD_SGIR_CPUTARGETLIST_کھسکاؤ)) | (irq << GICD_SGIR_SGIINTID_کھسکاؤ);
}

#جے_تعریف_شدہ ENABLE_SMP_SUPPORT
/*
* 25-24: target lister filter
* 0b00 - send the ipi to the CPU interfaces specified in the CPU target list
* 0b01 - send the ipi to all CPU interfaces except the cpu interface.
*        that requested the ipi
* 0b10 - send the ipi only to the CPU interface that requested the IPI.
* 0b11 - reserved
*.
* 23-16: CPU targets list
* each bit of CPU target list [7:0] refers to the corresponding CPU interface.
* 3-0:   SGIINTID
* software generated interrupt id, from 0 to 15...
*/
فراغ ipi_send_نشانہ(irq_t irq, لفظ_t cpu_نشانہ_List)
{
    إذا (config_سیٹ(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpu_نشانہ_List = ((cpu_نشانہ_List & 0xf) << 4) | ((cpu_نشانہ_List & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpu_نشانہ_List << (GICD_SGIR_CPUTARGETLIST_کھسکاؤ)) | (IRQT_منزل_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_کھسکاؤ);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
فراغ سیٹ_IRQTarget(irq_t irq, se_L4_لفظ نشانہ)
{
    uint8_t نشانہ_List = 1 << نشانہ;
    uint8_t *targets = (فراغ *)(gic_dist->targets);
    لفظ_t hwIRQ = IRQT_منزل_IRQ(irq);

    /* Return early if PPI */
    إذا (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        أرجع;
    }
    targets[hwIRQ] = نشانہ_List;
}
#جے_انت /* ENABLE_SMP_SUPPORT */

#جے_تعریف_شدہ CONFIG_ARM_HYPERVISOR_SUPPORT

#جے_بنا_تعریف GIC_V2_VCPUCTRL_PPTR
#غلطی GIC_V2_VCPUCTRL_PPTR must be defined لكل مجازی حافظہ access منزل the gic مجازی cpu interface control
#وإلا  /* GIC_PL400_GICVCPUCTRL_PPTR */
متطاير بنية gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (متطاير بنية gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#جے_انت /* GIC_PL400_GICVCPUCTRL_PPTR */

لفظ_t gic_vcpu_عدد_list_regs;

#جے_انت /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
