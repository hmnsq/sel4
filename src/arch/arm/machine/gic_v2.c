/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <arch/machine/gic_v2.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਸ਼ਾਨਾ_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਸ਼ਾਨਾ_CPU0_ALLINT   ਨਿਸ਼ਾਨਾ_CPU_ALLINT(ਦੋਆਧਾਰੀ_ਅੰਕ(0))

/* Use this to forward interrupts to all CPUs when debugging */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਨਿਸ਼ਾਨਾ_CPU_ALL_ALLINT   ਨਿਸ਼ਾਨਾ_CPU_ALLINT(0xff)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IRQ_ਸੈੱਟ_ALL 0xffffffff

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GIC_V2_DISTRIBUTOR_PPTR
#ਗਲਤੀ GIC_V2_DISTRIBUTOR_PPTR must be defined ਲਈ ਆਭਾਸੀ ਸਮ੍ਰਿਤੀ access ਮੰਜ਼ਿਲ the gic distributer
#ਨਹੀਂਤਾਂ  /* GIC_DISTRIBUTOR_PPTR */
ਅਸਥਿਰ ਬਣਤਰ gic_dist_map *ਅਚਲ gic_dist =
    (ਅਸਥਿਰ ਬਣਤਰ gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#ਜੇ_ਅੰਤ /* GIC_DISTRIBUTOR_PPTR */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GIC_V2_CONTROLLER_PPTR
#ਗਲਤੀ GIC_V2_CONTROLLER_PPTR must be defined ਲਈ ਆਭਾਸੀ ਸਮ੍ਰਿਤੀ access ਮੰਜ਼ਿਲ the gic cpu interface
#ਨਹੀਂਤਾਂ  /* GIC_CONTROLLER_PPTR */
ਅਸਥਿਰ ਬਣਤਰ gic_cpu_iface_map *ਅਚਲ gic_cpuiface =
    (ਅਸਥਿਰ ਬਣਤਰ gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#ਜੇ_ਅੰਤ /* GIC_CONTROLLER_PPTR */

ਸ਼ਬਦ_t ਸਰਗਰਮ_irq[CONFIG_MAX_ਅੰਕ_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_ਕੋਡ ਸਥਿਰ uint8_t infer_cpu_gic_id(ਪੂਰਨ nirqs)
{
    ਸ਼ਬਦ_t i;
    uint32_t ਨਿਸ਼ਾਨਾ = 0;
    ਲਈ (i = 0; i < nirqs; i += 4) {
        ਨਿਸ਼ਾਨਾ = gic_dist->targets[i >> 2];
        ਨਿਸ਼ਾਨਾ |= ਨਿਸ਼ਾਨਾ >> 16;
        ਨਿਸ਼ਾਨਾ |= ਨਿਸ਼ਾਨਾ >> 8;
        ਜੇ (ਨਿਸ਼ਾਨਾ) {
            ਰੋਕੋ;
        }
    }
    ਜੇ (!ਨਿਸ਼ਾਨਾ) {
        ਲਿਖੋ("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        ਨਿਸ਼ਾਨਾ = ਦੋਆਧਾਰੀ_ਅੰਕ(0);
    }
    ਵਾਪਸ ਨਿਸ਼ਾਨਾ & 0xff;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ dist_init(ਖਾਲੀ)
{
    ਸ਼ਬਦ_t i;
    ਪੂਰਨ nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    ਲਈ (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_ਸੈੱਟ_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_ਸੈੱਟ_ALL;
    }

    /* reset interrupts priority */
    ਲਈ (i = 32; i < nirqs; i += 4) {
        ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->ਤਰਜੀਹ[i >> 2] = 0x80808080;
        } ਨਹੀਂਤਾਂ {
            gic_dist->ਤਰਜੀਹ[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t ਨਿਸ਼ਾਨਾ = infer_cpu_gic_id(nirqs);
    ਲਈ (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = ਨਿਸ਼ਾਨਾ_CPU_ALLINT(ਨਿਸ਼ਾਨਾ);
    }

    /* level-triggered, 1-N */
    ਲਈ (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    ਲਈ (i = 0; i < nirqs; i += 32) {
        ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_ਸੈੱਟ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } ਨਹੀਂਤਾਂ {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ cpu_iface_init(ਖਾਲੀ)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_ਸੈੱਟ_ALL;
    gic_dist->pending_clr[0] = IRQ_ਸੈੱਟ_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    ਜੇ (config_ਸੈੱਟ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_ਸੈੱਟ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->ਤਰਜੀਹ[0] = 0x80808080;
    } ਨਹੀਂਤਾਂ {
        gic_dist->security[0] = 0;
        gic_dist->ਤਰਜੀਹ[0] = 0x0;
    }

    /* clear any software generated interrupts */
    ਲਈ (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_ਸੈੱਟ_ALL;
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
    ਜਦੋਂ ((i & IRQ_ਨਕਾਬ) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

ਖਾਲੀ ਸੈੱਟ_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    ਸ਼ਬਦ_t ਸੂਚਕਾਂਕ = IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) / 16u;
    ਸ਼ਬਦ_t ਵਿੱਥ = (IRQT_ਮੰਜ਼ਿਲ_IRQ(irq) % 16u) * 2;
    ਜੇ (trigger) {
        /* set the bit */
        gic_dist->config[ਸੂਚਕਾਂਕ] |= ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਥ + 1);
    } ਨਹੀਂਤਾਂ {
        gic_dist->config[ਸੂਚਕਾਂਕ] &= ~ਦੋਆਧਾਰੀ_ਅੰਕ(ਵਿੱਥ + 1);
    }
}

BOOT_ਕੋਡ ਖਾਲੀ initIRQController(ਖਾਲੀ)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    ਪ੍ਰਤਿਜ੍ਞਾ(INT_STATE_ARRAY_ਮਾਪ < IRQT_ਮੰਜ਼ਿਲ_IRQ(irqInvalid));
    dist_init();
}

BOOT_ਕੋਡ ਖਾਲੀ cpu_initLocalIRQController(ਖਾਲੀ)
{
    ਸਰਗਰਮ_irq[CURRENT_CPU_ਸੂਚਕਾਂਕ()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    ਵਾਪਸ ਨਿਸ਼ਾਨਾ <= GIC_SGI_ਅੰਕ_TARGETS - 1;
}

ਖਾਲੀ plat_sendSGI(ਸ਼ਬਦ_t irq, ਸ਼ਬਦ_t ਨਿਸ਼ਾਨਾ)
{
    gic_dist->sgi_control = (ਦੋਆਧਾਰੀ_ਅੰਕ(ਨਿਸ਼ਾਨਾ) << (GICD_SGIR_CPUTARGETLIST_ਖਿਸਕਾਓ)) | (irq << GICD_SGIR_SGIINTID_ਖਿਸਕਾਓ);
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ENABLE_SMP_SUPPORT
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
ਖਾਲੀ ipi_send_ਨਿਸ਼ਾਨਾ(irq_t irq, ਸ਼ਬਦ_t cpu_ਨਿਸ਼ਾਨਾ_List)
{
    ਜੇ (config_ਸੈੱਟ(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpu_ਨਿਸ਼ਾਨਾ_List = ((cpu_ਨਿਸ਼ਾਨਾ_List & 0xf) << 4) | ((cpu_ਨਿਸ਼ਾਨਾ_List & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpu_ਨਿਸ਼ਾਨਾ_List << (GICD_SGIR_CPUTARGETLIST_ਖਿਸਕਾਓ)) | (IRQT_ਮੰਜ਼ਿਲ_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_ਖਿਸਕਾਓ);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
ਖਾਲੀ ਸੈੱਟ_IRQTarget(irq_t irq, se_L4_ਸ਼ਬਦ ਨਿਸ਼ਾਨਾ)
{
    uint8_t ਨਿਸ਼ਾਨਾ_List = 1 << ਨਿਸ਼ਾਨਾ;
    uint8_t *targets = (ਖਾਲੀ *)(gic_dist->targets);
    ਸ਼ਬਦ_t hwIRQ = IRQT_ਮੰਜ਼ਿਲ_IRQ(irq);

    /* Return early if PPI */
    ਜੇ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        ਵਾਪਸ;
    }
    targets[hwIRQ] = ਨਿਸ਼ਾਨਾ_List;
}
#ਜੇ_ਅੰਤ /* ENABLE_SMP_SUPPORT */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CONFIG_ARM_HYPERVISOR_SUPPORT

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GIC_V2_VCPUCTRL_PPTR
#ਗਲਤੀ GIC_V2_VCPUCTRL_PPTR must be defined ਲਈ ਆਭਾਸੀ ਸਮ੍ਰਿਤੀ access ਮੰਜ਼ਿਲ the gic ਆਭਾਸੀ cpu interface control
#ਨਹੀਂਤਾਂ  /* GIC_PL400_GICVCPUCTRL_PPTR */
ਅਸਥਿਰ ਬਣਤਰ gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (ਅਸਥਿਰ ਬਣਤਰ gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#ਜੇ_ਅੰਤ /* GIC_PL400_GICVCPUCTRL_PPTR */

ਸ਼ਬਦ_t gic_vcpu_ਅੰਕ_list_regs;

#ਜੇ_ਅੰਤ /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
