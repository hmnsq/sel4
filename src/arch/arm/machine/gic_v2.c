/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <arch/machine/gic_v2.h>

#ವ್ಯಾಖ್ಯಾನಿಸು ಗುರಿ_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುರಿ_CPU0_ALLINT   ಗುರಿ_CPU_ALLINT(ಬಿಟ್(0))

/* Use this to forward interrupts to all CPUs when debugging */
#ವ್ಯಾಖ್ಯಾನಿಸು ಗುರಿ_CPU_ALL_ALLINT   ಗುರಿ_CPU_ALLINT(0xff)

#ವ್ಯಾಖ್ಯಾನಿಸು IRQ_ಗಣ_ALL 0xffffffff

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GIC_V2_DISTRIBUTOR_PPTR
#ದೋಷ GIC_V2_DISTRIBUTOR_PPTR must be defined ಗಾಗಿ ವಾಸ್ತವಿಕ ಸ್ಮರಣೆ access ಗಮ್ಯ the gic distributer
#ಇಲ್ಲದಿದ್ದರೆ  /* GIC_DISTRIBUTOR_PPTR */
ಅಸ್ಥಿರ ರಚನೆ gic_dist_map *ಸ್ಥಿರಾಂಕ gic_dist =
    (ಅಸ್ಥಿರ ರಚನೆ gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#ಆದರೆ_ಅಂತ್ಯ /* GIC_DISTRIBUTOR_PPTR */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GIC_V2_CONTROLLER_PPTR
#ದೋಷ GIC_V2_CONTROLLER_PPTR must be defined ಗಾಗಿ ವಾಸ್ತವಿಕ ಸ್ಮರಣೆ access ಗಮ್ಯ the gic cpu interface
#ಇಲ್ಲದಿದ್ದರೆ  /* GIC_CONTROLLER_PPTR */
ಅಸ್ಥಿರ ರಚನೆ gic_cpu_iface_map *ಸ್ಥಿರಾಂಕ gic_cpuiface =
    (ಅಸ್ಥಿರ ರಚನೆ gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#ಆದರೆ_ಅಂತ್ಯ /* GIC_CONTROLLER_PPTR */

ಶಬ್ದ_t ಸಕ್ರಿಯ_irq[CONFIG_MAX_ಅಂಕೆ_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_ಸಂಕೇತ ಸ್ಥಿರ uint8_t infer_cpu_gic_id(ಪೂರ್ಣಾಂಕ nirqs)
{
    ಶಬ್ದ_t i;
    uint32_t ಗುರಿ = 0;
    ಗಾಗಿ (i = 0; i < nirqs; i += 4) {
        ಗುರಿ = gic_dist->targets[i >> 2];
        ಗುರಿ |= ಗುರಿ >> 16;
        ಗುರಿ |= ಗುರಿ >> 8;
        ಆದರೆ (ಗುರಿ) {
            ನಿಲ್ಲಿಸು;
        }
    }
    ಆದರೆ (!ಗುರಿ) {
        ಬರೆ("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        ಗುರಿ = ಬಿಟ್(0);
    }
    ಹಿಂತಿರುಗಿ ಗುರಿ & 0xff;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ dist_init(ಶೂನ್ಯ)
{
    ಶಬ್ದ_t i;
    ಪೂರ್ಣಾಂಕ nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    ಗಾಗಿ (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_ಗಣ_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_ಗಣ_ALL;
    }

    /* reset interrupts priority */
    ಗಾಗಿ (i = 32; i < nirqs; i += 4) {
        ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->ಆದ್ಯತೆ[i >> 2] = 0x80808080;
        } ಇಲ್ಲದಿದ್ದರೆ {
            gic_dist->ಆದ್ಯತೆ[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t ಗುರಿ = infer_cpu_gic_id(nirqs);
    ಗಾಗಿ (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = ಗುರಿ_CPU_ALLINT(ಗುರಿ);
    }

    /* level-triggered, 1-N */
    ಗಾಗಿ (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    ಗಾಗಿ (i = 0; i < nirqs; i += 32) {
        ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_ಗಣ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } ಇಲ್ಲದಿದ್ದರೆ {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ cpu_iface_init(ಶೂನ್ಯ)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_ಗಣ_ALL;
    gic_dist->pending_clr[0] = IRQ_ಗಣ_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    ಆದರೆ (config_ಗಣ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_ಗಣ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->ಆದ್ಯತೆ[0] = 0x80808080;
    } ಇಲ್ಲದಿದ್ದರೆ {
        gic_dist->security[0] = 0;
        gic_dist->ಆದ್ಯತೆ[0] = 0x0;
    }

    /* clear any software generated interrupts */
    ಗಾಗಿ (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_ಗಣ_ALL;
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
    ಆಗ ((i & IRQ_ಮುಖವಾಡ) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

ಶೂನ್ಯ ಗಣ_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    ಶಬ್ದ_t ಸೂಚ್ಯಂಕ = IRQT_ಗಮ್ಯ_IRQ(irq) / 16u;
    ಶಬ್ದ_t ಸ್ಥಾನಾಂತರ = (IRQT_ಗಮ್ಯ_IRQ(irq) % 16u) * 2;
    ಆದರೆ (trigger) {
        /* set the bit */
        gic_dist->config[ಸೂಚ್ಯಂಕ] |= ಬಿಟ್(ಸ್ಥಾನಾಂತರ + 1);
    } ಇಲ್ಲದಿದ್ದರೆ {
        gic_dist->config[ಸೂಚ್ಯಂಕ] &= ~ಬಿಟ್(ಸ್ಥಾನಾಂತರ + 1);
    }
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ initIRQController(ಶೂನ್ಯ)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    ಪ್ರತಿಜ್ಞಾ(INT_STATE_ARRAY_ಗಾತ್ರ < IRQT_ಗಮ್ಯ_IRQ(irqInvalid));
    dist_init();
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ cpu_initLocalIRQController(ಶೂನ್ಯ)
{
    ಸಕ್ರಿಯ_irq[CURRENT_CPU_ಸೂಚ್ಯಂಕ()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(ಶಬ್ದ_t ಗುರಿ)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    ಹಿಂತಿರುಗಿ ಗುರಿ <= GIC_SGI_ಅಂಕೆ_TARGETS - 1;
}

ಶೂನ್ಯ plat_sendSGI(ಶಬ್ದ_t irq, ಶಬ್ದ_t ಗುರಿ)
{
    gic_dist->sgi_control = (ಬಿಟ್(ಗುರಿ) << (GICD_SGIR_CPUTARGETLIST_ಸರಿಸು)) | (irq << GICD_SGIR_SGIINTID_ಸರಿಸು);
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ENABLE_SMP_SUPPORT
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
ಶೂನ್ಯ ipi_send_ಗುರಿ(irq_t irq, ಶಬ್ದ_t cpu_ಗುರಿ_List)
{
    ಆದರೆ (config_ಗಣ(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpu_ಗುರಿ_List = ((cpu_ಗುರಿ_List & 0xf) << 4) | ((cpu_ಗುರಿ_List & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpu_ಗುರಿ_List << (GICD_SGIR_CPUTARGETLIST_ಸರಿಸು)) | (IRQT_ಗಮ್ಯ_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_ಸರಿಸು);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
ಶೂನ್ಯ ಗಣ_IRQTarget(irq_t irq, se_L4_ಶಬ್ದ ಗುರಿ)
{
    uint8_t ಗುರಿ_List = 1 << ಗುರಿ;
    uint8_t *targets = (ಶೂನ್ಯ *)(gic_dist->targets);
    ಶಬ್ದ_t hwIRQ = IRQT_ಗಮ್ಯ_IRQ(irq);

    /* Return early if PPI */
    ಆದರೆ (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        ಹಿಂತಿರುಗಿ;
    }
    targets[hwIRQ] = ಗುರಿ_List;
}
#ಆದರೆ_ಅಂತ್ಯ /* ENABLE_SMP_SUPPORT */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_ARM_HYPERVISOR_SUPPORT

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GIC_V2_VCPUCTRL_PPTR
#ದೋಷ GIC_V2_VCPUCTRL_PPTR must be defined ಗಾಗಿ ವಾಸ್ತವಿಕ ಸ್ಮರಣೆ access ಗಮ್ಯ the gic ವಾಸ್ತವಿಕ cpu interface control
#ಇಲ್ಲದಿದ್ದರೆ  /* GIC_PL400_GICVCPUCTRL_PPTR */
ಅಸ್ಥಿರ ರಚನೆ gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (ಅಸ್ಥಿರ ರಚನೆ gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#ಆದರೆ_ಅಂತ್ಯ /* GIC_PL400_GICVCPUCTRL_PPTR */

ಶಬ್ದ_t gic_vcpu_ಅಂಕೆ_list_regs;

#ಆದರೆ_ಅಂತ್ಯ /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
