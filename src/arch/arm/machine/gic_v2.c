/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <arch/machine/gic_v2.h>

#నిర్వచించు లక్ష్యం_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#నిర్వచించు లక్ష్యం_CPU0_ALLINT   లక్ష్యం_CPU_ALLINT(బిట్టు(0))

/* Use this to forward interrupts to all CPUs when debugging */
#నిర్వచించు లక్ష్యం_CPU_ALL_ALLINT   లక్ష్యం_CPU_ALLINT(0xff)

#నిర్వచించు IRQ_సమితి_ALL 0xffffffff

#నిర్వచితం_కాకపోతే GIC_V2_DISTRIBUTOR_PPTR
#దోషం GIC_V2_DISTRIBUTOR_PPTR must be defined కొరకు మాయా జ్ఞాపకం access గమ్యం the gic distributer
#లేకపోతే  /* GIC_DISTRIBUTOR_PPTR */
అస్థిర నిర్మాణం gic_dist_map *స్థిరరాశి gic_dist =
    (అస్థిర నిర్మాణం gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#అయితే_ముగింపు /* GIC_DISTRIBUTOR_PPTR */

#నిర్వచితం_కాకపోతే GIC_V2_CONTROLLER_PPTR
#దోషం GIC_V2_CONTROLLER_PPTR must be defined కొరకు మాయా జ్ఞాపకం access గమ్యం the gic cpu interface
#లేకపోతే  /* GIC_CONTROLLER_PPTR */
అస్థిర నిర్మాణం gic_cpu_iface_map *స్థిరరాశి gic_cpuiface =
    (అస్థిర నిర్మాణం gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#అయితే_ముగింపు /* GIC_CONTROLLER_PPTR */

మాట_t చురుకైన_irq[CONFIG_MAX_అంకె_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_సంకేతం స్థిర uint8_t infer_cpu_gic_id(పూర్ణాంకం nirqs)
{
    మాట_t i;
    uint32_t లక్ష్యం = 0;
    కొరకు (i = 0; i < nirqs; i += 4) {
        లక్ష్యం = gic_dist->targets[i >> 2];
        లక్ష్యం |= లక్ష్యం >> 16;
        లక్ష్యం |= లక్ష్యం >> 8;
        ఒకవేళ (లక్ష్యం) {
            ఆపు;
        }
    }
    ఒకవేళ (!లక్ష్యం) {
        రాయి("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        లక్ష్యం = బిట్టు(0);
    }
    తిరిగి లక్ష్యం & 0xff;
}

BOOT_సంకేతం స్థిర శూన్య dist_init(శూన్య)
{
    మాట_t i;
    పూర్ణాంకం nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    కొరకు (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_సమితి_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_సమితి_ALL;
    }

    /* reset interrupts priority */
    కొరకు (i = 32; i < nirqs; i += 4) {
        ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->ప్రాధాన్యత[i >> 2] = 0x80808080;
        } లేకపోతే {
            gic_dist->ప్రాధాన్యత[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t లక్ష్యం = infer_cpu_gic_id(nirqs);
    కొరకు (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = లక్ష్యం_CPU_ALLINT(లక్ష్యం);
    }

    /* level-triggered, 1-N */
    కొరకు (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    కొరకు (i = 0; i < nirqs; i += 32) {
        ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_సమితి(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } లేకపోతే {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_సంకేతం స్థిర శూన్య cpu_iface_init(శూన్య)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_సమితి_ALL;
    gic_dist->pending_clr[0] = IRQ_సమితి_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    ఒకవేళ (config_సమితి(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_సమితి(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->ప్రాధాన్యత[0] = 0x80808080;
    } లేకపోతే {
        gic_dist->security[0] = 0;
        gic_dist->ప్రాధాన్యత[0] = 0x0;
    }

    /* clear any software generated interrupts */
    కొరకు (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_సమితి_ALL;
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
    అయితే ((i & IRQ_ముసుగు) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

శూన్య సమితి_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    మాట_t సూచీ = IRQT_గమ్యం_IRQ(irq) / 16u;
    మాట_t స్థానభ్రంశం = (IRQT_గమ్యం_IRQ(irq) % 16u) * 2;
    ఒకవేళ (trigger) {
        /* set the bit */
        gic_dist->config[సూచీ] |= బిట్టు(స్థానభ్రంశం + 1);
    } లేకపోతే {
        gic_dist->config[సూచీ] &= ~బిట్టు(స్థానభ్రంశం + 1);
    }
}

BOOT_సంకేతం శూన్య initIRQController(శూన్య)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    ప్రతిజ్ఞా(INT_STATE_ARRAY_కొలత < IRQT_గమ్యం_IRQ(irqInvalid));
    dist_init();
}

BOOT_సంకేతం శూన్య cpu_initLocalIRQController(శూన్య)
{
    చురుకైన_irq[CURRENT_CPU_సూచీ()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(మాట_t లక్ష్యం)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    తిరిగి లక్ష్యం <= GIC_SGI_అంకె_TARGETS - 1;
}

శూన్య plat_sendSGI(మాట_t irq, మాట_t లక్ష్యం)
{
    gic_dist->sgi_control = (బిట్టు(లక్ష్యం) << (GICD_SGIR_CPUTARGETLIST_జరుపు)) | (irq << GICD_SGIR_SGIINTID_జరుపు);
}

#నిర్వచితమైతే ENABLE_SMP_SUPPORT
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
శూన్య ipi_send_లక్ష్యం(irq_t irq, మాట_t cpu_లక్ష్యం_List)
{
    ఒకవేళ (config_సమితి(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpu_లక్ష్యం_List = ((cpu_లక్ష్యం_List & 0xf) << 4) | ((cpu_లక్ష్యం_List & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpu_లక్ష్యం_List << (GICD_SGIR_CPUTARGETLIST_జరుపు)) | (IRQT_గమ్యం_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_జరుపు);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
శూన్య సమితి_IRQTarget(irq_t irq, se_L4_మాట లక్ష్యం)
{
    uint8_t లక్ష్యం_List = 1 << లక్ష్యం;
    uint8_t *targets = (శూన్య *)(gic_dist->targets);
    మాట_t hwIRQ = IRQT_గమ్యం_IRQ(irq);

    /* Return early if PPI */
    ఒకవేళ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        తిరిగి;
    }
    targets[hwIRQ] = లక్ష్యం_List;
}
#అయితే_ముగింపు /* ENABLE_SMP_SUPPORT */

#నిర్వచితమైతే CONFIG_ARM_HYPERVISOR_SUPPORT

#నిర్వచితం_కాకపోతే GIC_V2_VCPUCTRL_PPTR
#దోషం GIC_V2_VCPUCTRL_PPTR must be defined కొరకు మాయా జ్ఞాపకం access గమ్యం the gic మాయా cpu interface control
#లేకపోతే  /* GIC_PL400_GICVCPUCTRL_PPTR */
అస్థిర నిర్మాణం gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (అస్థిర నిర్మాణం gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#అయితే_ముగింపు /* GIC_PL400_GICVCPUCTRL_PPTR */

మాట_t gic_vcpu_అంకె_list_regs;

#అయితే_ముగింపు /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
