/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <arch/machine/gic_v2.h>

#परिभाषय TARGET_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#परिभाषय TARGET_CPU0_ALLINT   TARGET_CPU_ALLINT(BIT(0))

/* Use this to forward interrupts to all CPUs when debugging */
#परिभाषय TARGET_CPU_ALL_ALLINT   TARGET_CPU_ALLINT(0xff)

#परिभाषय IRQ_समुच्चयः_ALL 0xffffffff

#यदि_अपरिभाषितम् GIC_V2_DISTRIBUTOR_PPTR
#दोषः GIC_V2_DISTRIBUTOR_PPTR must be defined प्रति आभास स्मृतिः access गन्तव्यम् the gic distributer
#अन्यथा  /* GIC_DISTRIBUTOR_PPTR */
चञ्चल संरचना gic_dist_map *ध्रुव gic_dist =
    (चञ्चल संरचना gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#यदि_अन्तः /* GIC_DISTRIBUTOR_PPTR */

#यदि_अपरिभाषितम् GIC_V2_CONTROLLER_PPTR
#दोषः GIC_V2_CONTROLLER_PPTR must be defined प्रति आभास स्मृतिः access गन्तव्यम् the gic cpu interface
#अन्यथा  /* GIC_CONTROLLER_PPTR */
चञ्चल संरचना gic_cpu_iface_map *ध्रुव gic_cpuiface =
    (चञ्चल संरचना gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#यदि_अन्तः /* GIC_CONTROLLER_PPTR */

word_t सक्रिय_irq[CONFIG_MAX_अङ्कः_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_संहिता स्थिर uint8_t infer_cpu_gic_id(पूर्णाङ्क nirqs)
{
    word_t i;
    uint32_t target = 0;
    प्रति (i = 0; i < nirqs; i += 4) {
        target = gic_dist->targets[i >> 2];
        target |= target >> 16;
        target |= target >> 8;
        यदि (target) {
            भङ्ग;
        }
    }
    यदि (!target) {
        प्रारूप_लिख("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        target = BIT(0);
    }
    प्रत्यागम target & 0xff;
}

BOOT_संहिता स्थिर शून्य dist_init(शून्य)
{
    word_t i;
    पूर्णाङ्क nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    प्रति (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_समुच्चयः_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_समुच्चयः_ALL;
    }

    /* reset interrupts priority */
    प्रति (i = 32; i < nirqs; i += 4) {
        यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->प्राथमिकता[i >> 2] = 0x80808080;
        } अन्यथा {
            gic_dist->प्राथमिकता[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t target = infer_cpu_gic_id(nirqs);
    प्रति (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = TARGET_CPU_ALLINT(target);
    }

    /* level-triggered, 1-N */
    प्रति (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    प्रति (i = 0; i < nirqs; i += 32) {
        यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_समुच्चयः(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } अन्यथा {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_संहिता स्थिर शून्य cpu_iface_init(शून्य)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_समुच्चयः_ALL;
    gic_dist->pending_clr[0] = IRQ_समुच्चयः_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    यदि (config_समुच्चयः(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_समुच्चयः(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->प्राथमिकता[0] = 0x80808080;
    } अन्यथा {
        gic_dist->security[0] = 0;
        gic_dist->प्राथमिकता[0] = 0x0;
    }

    /* clear any software generated interrupts */
    प्रति (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_समुच्चयः_ALL;
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
    यावत् ((i & IRQ_आवरणम्) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

शून्य समुच्चयः_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    word_t सूचकांक = IRQT_गन्तव्यम्_IRQ(irq) / 16u;
    word_t विस्थापनम् = (IRQT_गन्तव्यम्_IRQ(irq) % 16u) * 2;
    यदि (trigger) {
        /* set the bit */
        gic_dist->config[सूचकांक] |= BIT(विस्थापनम् + 1);
    } अन्यथा {
        gic_dist->config[सूचकांक] &= ~BIT(विस्थापनम् + 1);
    }
}

BOOT_संहिता शून्य initIRQController(शून्य)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    प्रतिज्ञा(INT_STATE_ARRAY_आकार < IRQT_गन्तव्यम्_IRQ(irqInvalid));
    dist_init();
}

BOOT_संहिता शून्य cpu_initLocalIRQController(शून्य)
{
    सक्रिय_irq[CURRENT_CPU_सूचकांक()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(word_t target)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    प्रत्यागम target <= GIC_SGI_अङ्कः_TARGETS - 1;
}

शून्य plat_sendSGI(word_t irq, word_t target)
{
    gic_dist->sgi_control = (BIT(target) << (GICD_SGIR_CPUTARGETLIST_सरणम्)) | (irq << GICD_SGIR_SGIINTID_सरणम्);
}

#यदि_परिभाषितम् ENABLE_SMP_SUPPORT
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
शून्य ipi_send_target(irq_t irq, word_t cpuTargetList)
{
    यदि (config_समुच्चयः(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpuTargetList = ((cpuTargetList & 0xf) << 4) | ((cpuTargetList & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpuTargetList << (GICD_SGIR_CPUTARGETLIST_सरणम्)) | (IRQT_गन्तव्यम्_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_सरणम्);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
शून्य समुच्चयः_IRQTarget(irq_t irq, seL4_Word target)
{
    uint8_t targetList = 1 << target;
    uint8_t *targets = (शून्य *)(gic_dist->targets);
    word_t hwIRQ = IRQT_गन्तव्यम्_IRQ(irq);

    /* Return early if PPI */
    यदि (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        प्रत्यागम;
    }
    targets[hwIRQ] = targetList;
}
#यदि_अन्तः /* ENABLE_SMP_SUPPORT */

#यदि_परिभाषितम् CONFIG_ARM_HYPERVISOR_SUPPORT

#यदि_अपरिभाषितम् GIC_V2_VCPUCTRL_PPTR
#दोषः GIC_V2_VCPUCTRL_PPTR must be defined प्रति आभास स्मृतिः access गन्तव्यम् the gic आभास cpu interface control
#अन्यथा  /* GIC_PL400_GICVCPUCTRL_PPTR */
चञ्चल संरचना gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (चञ्चल संरचना gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#यदि_अन्तः /* GIC_PL400_GICVCPUCTRL_PPTR */

word_t gic_vcpu_अङ्कः_list_regs;

#यदि_अन्तः /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
