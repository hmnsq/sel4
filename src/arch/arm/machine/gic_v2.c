/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#அந்தர்பாவ <config.h>
#அந்தர்பாவ <arch/machine/gic_v2.h>

#define TARGET_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#define TARGET_CPU0_ALLINT   TARGET_CPU_ALLINT(BIT(0))

/* Use this to forward interrupts to all CPUs when debugging */
#define TARGET_CPU_ALL_ALLINT   TARGET_CPU_ALLINT(0xff)

#define IRQ_SET_ALL 0xffffffff

#ifndef GIC_V2_DISTRIBUTOR_PPTR
#error GIC_V2_DISTRIBUTOR_PPTR must be defined க்கு மெய்நிகர் memory access to the gic distributer
#இல்லையெனில்  /* GIC_DISTRIBUTOR_PPTR */
நிலையற்ற கட்டமைப்பு gic_dist_map *மாறிலி gic_dist =
    (நிலையற்ற கட்டமைப்பு gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#endif /* GIC_DISTRIBUTOR_PPTR */

#ifndef GIC_V2_CONTROLLER_PPTR
#error GIC_V2_CONTROLLER_PPTR must be defined க்கு மெய்நிகர் memory access to the gic cpu interface
#இல்லையெனில்  /* GIC_CONTROLLER_PPTR */
நிலையற்ற கட்டமைப்பு gic_cpu_iface_map *மாறிலி gic_cpuiface =
    (நிலையற்ற கட்டமைப்பு gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#endif /* GIC_CONTROLLER_PPTR */

word_t active_irq[CONFIG_MAX_இலக்கம்_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_CODE நிலையான uint8_t infer_cpu_gic_id(முழு nirqs)
{
    word_t i;
    uint32_t target = 0;
    க்கு (i = 0; i < nirqs; i += 4) {
        target = gic_dist->targets[i >> 2];
        target |= target >> 16;
        target |= target >> 8;
        எனில் (target) {
            நிறுத்து;
        }
    }
    எனில் (!target) {
        printf("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        target = BIT(0);
    }
    திரும்பு target & 0xff;
}

BOOT_CODE நிலையான வெறுமை dist_init(வெறுமை)
{
    word_t i;
    முழு nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    க்கு (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_SET_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_SET_ALL;
    }

    /* reset interrupts priority */
    க்கு (i = 32; i < nirqs; i += 4) {
        எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->priority[i >> 2] = 0x80808080;
        } இல்லையெனில் {
            gic_dist->priority[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t target = infer_cpu_gic_id(nirqs);
    க்கு (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = TARGET_CPU_ALLINT(target);
    }

    /* level-triggered, 1-N */
    க்கு (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    க்கு (i = 0; i < nirqs; i += 32) {
        எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } இல்லையெனில் {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_CODE நிலையான வெறுமை cpu_iface_init(வெறுமை)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_SET_ALL;
    gic_dist->pending_clr[0] = IRQ_SET_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    எனில் (config_set(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_set(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->priority[0] = 0x80808080;
    } இல்லையெனில் {
        gic_dist->security[0] = 0;
        gic_dist->priority[0] = 0x0;
    }

    /* clear any software generated interrupts */
    க்கு (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_SET_ALL;
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
    போது ((i & IRQ_MASK) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

வெறுமை setIRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    word_t index = IRQT_TO_IRQ(irq) / 16u;
    word_t offset = (IRQT_TO_IRQ(irq) % 16u) * 2;
    எனில் (trigger) {
        /* set the bit */
        gic_dist->config[index] |= BIT(offset + 1);
    } இல்லையெனில் {
        gic_dist->config[index] &= ~BIT(offset + 1);
    }
}

BOOT_CODE வெறுமை initIRQController(வெறுமை)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    ப்ரதிஜ்ஞா(INT_STATE_ARRAY_SIZE < IRQT_TO_IRQ(irqInvalid));
    dist_init();
}

BOOT_CODE வெறுமை cpu_initLocalIRQController(வெறுமை)
{
    active_irq[CURRENT_CPU_INDEX()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(word_t target)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    திரும்பு target <= GIC_SGI_இலக்கம்_TARGETS - 1;
}

வெறுமை plat_sendSGI(word_t irq, word_t target)
{
    gic_dist->sgi_control = (BIT(target) << (GICD_SGIR_CPUTARGETLIST_SHIFT)) | (irq << GICD_SGIR_SGIINTID_SHIFT);
}

#ifdef ENABLE_SMP_SUPPORT
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
வெறுமை ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    எனில் (config_set(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpuTargetList = ((cpuTargetList & 0xf) << 4) | ((cpuTargetList & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpuTargetList << (GICD_SGIR_CPUTARGETLIST_SHIFT)) | (IRQT_TO_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_SHIFT);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
வெறுமை setIRQTarget(irq_t irq, seL4_Word target)
{
    uint8_t targetList = 1 << target;
    uint8_t *targets = (வெறுமை *)(gic_dist->targets);
    word_t hwIRQ = IRQT_TO_IRQ(irq);

    /* Return early if PPI */
    எனில் (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        திரும்பு;
    }
    targets[hwIRQ] = targetList;
}
#endif /* ENABLE_SMP_SUPPORT */

#ifdef CONFIG_ARM_HYPERVISOR_SUPPORT

#ifndef GIC_V2_VCPUCTRL_PPTR
#error GIC_V2_VCPUCTRL_PPTR must be defined க்கு மெய்நிகர் memory access to the gic மெய்நிகர் cpu interface control
#இல்லையெனில்  /* GIC_PL400_GICVCPUCTRL_PPTR */
நிலையற்ற கட்டமைப்பு gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (நிலையற்ற கட்டமைப்பு gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#endif /* GIC_PL400_GICVCPUCTRL_PPTR */

word_t gic_vcpu_இலக்கம்_list_regs;

#endif /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
