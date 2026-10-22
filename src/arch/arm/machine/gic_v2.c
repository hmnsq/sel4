/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <arch/machine/gic_v2.h>

#تعریف_کړه موخه_CPU_ALLINT(CPU) ( \
        ( ((CPU)&0xff)<<0u  ) |\
        ( ((CPU)&0xff)<<8u  ) |\
        ( ((CPU)&0xff)<<16u ) |\
        ( ((CPU)&0xff)<<24u ) \
    )
#تعریف_کړه موخه_CPU0_ALLINT   موخه_CPU_ALLINT(بټ(0))

/* Use this to forward interrupts to all CPUs when debugging */
#تعریف_کړه موخه_CPU_ALL_ALLINT   موخه_CPU_ALLINT(0xff)

#تعریف_کړه IRQ_سېټ_ALL 0xffffffff

#که_نه_تعریف_شوی GIC_V2_DISTRIBUTOR_PPTR
#تېروتنه GIC_V2_DISTRIBUTOR_PPTR must be defined لكل مجازي حافظه access منزل the gic distributer
#وإلا  /* GIC_DISTRIBUTOR_PPTR */
متطاير بنية gic_dist_map *ثابت gic_dist =
    (متطاير بنية gic_dist_map *)(GIC_V2_DISTRIBUTOR_PPTR);
#د_که_پای /* GIC_DISTRIBUTOR_PPTR */

#که_نه_تعریف_شوی GIC_V2_CONTROLLER_PPTR
#تېروتنه GIC_V2_CONTROLLER_PPTR must be defined لكل مجازي حافظه access منزل the gic cpu interface
#وإلا  /* GIC_CONTROLLER_PPTR */
متطاير بنية gic_cpu_iface_map *ثابت gic_cpuiface =
    (متطاير بنية gic_cpu_iface_map *)(GIC_V2_CONTROLLER_PPTR);
#د_که_پای /* GIC_CONTROLLER_PPTR */

کلمه_t فعال_irq[CONFIG_MAX_عدد_NODES];

/* Get the target id for this processor. We rely on the constraint that the registers
 * for PPI are read only and return only the current processor as the target.
 * If this doesn't lead to a valid ID, we emit a warning and default to core 0.
 */
BOOT_کوډ ساكن uint8_t infer_cpu_gic_id(صحيح nirqs)
{
    کلمه_t i;
    uint32_t موخه = 0;
    لكل (i = 0; i < nirqs; i += 4) {
        موخه = gic_dist->targets[i >> 2];
        موخه |= موخه >> 16;
        موخه |= موخه >> 8;
        إذا (موخه) {
            اكسر;
        }
    }
    إذا (!موخه) {
        چاپ_کړه("Warning: Could not infer GIC interrupt target ID, assuming 0.\n");
        موخه = بټ(0);
    }
    أرجع موخه & 0xff;
}

BOOT_کوډ ساكن فراغ dist_init(فراغ)
{
    کلمه_t i;
    صحيح nirqs = 32 * ((gic_dist->ic_type & 0x1f) + 1);
    gic_dist->enable = 0;

    لكل (i = 0; i < nirqs; i += 32) {
        /* disable */
        gic_dist->enable_clr[i >> 5] = IRQ_سېټ_ALL;
        /* clear pending */
        gic_dist->pending_clr[i >> 5] = IRQ_سېټ_ALL;
    }

    /* reset interrupts priority */
    لكل (i = 32; i < nirqs; i += 4) {
        إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT)) {
            gic_dist->لومړیتوب[i >> 2] = 0x80808080;
        } وإلا {
            gic_dist->لومړیتوب[i >> 2] = 0;
        }
    }

    /*
     * reset int target to current cpu
     * We query which id that the GIC uses for us and use that.
     */
    uint8_t موخه = infer_cpu_gic_id(nirqs);
    لكل (i = 0; i < nirqs; i += 4) {
        gic_dist->targets[i >> 2] = موخه_CPU_ALLINT(موخه);
    }

    /* level-triggered, 1-N */
    لكل (i = 64; i < nirqs; i += 32) {
        gic_dist->config[i >> 5] = 0x55555555;
    }

    /* group 0 for secure; group 1 for non-secure */
    لكل (i = 0; i < nirqs; i += 32) {
        إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_سېټ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
            gic_dist->security[i >> 5] = 0xffffffff;
        } وإلا {
            gic_dist->security[i >> 5] = 0;
        }
    }
    /* enable the int controller */
    gic_dist->enable = 1;
}

BOOT_کوډ ساكن فراغ cpu_iface_init(فراغ)
{
    uint32_t i;

    /* For non-Exynos4, the registers are banked per CPU, need to clear them */
    gic_dist->enable_clr[0] = IRQ_سېټ_ALL;
    gic_dist->pending_clr[0] = IRQ_سېټ_ALL;

    /* put everything in group 0; group 1 if in hyp mode */
    إذا (config_سېټ(CONFIG_ARM_HYPERVISOR_SUPPORT) && !config_سېټ(CONFIG_PLAT_QEMU_ARM_VIRT)) {
        gic_dist->security[0] = 0xffffffff;
        gic_dist->لومړیتوب[0] = 0x80808080;
    } وإلا {
        gic_dist->security[0] = 0;
        gic_dist->لومړیتوب[0] = 0x0;
    }

    /* clear any software generated interrupts */
    لكل (i = 0; i < 16; i += 4) {
        gic_dist->sgi_pending_clr[i >> 2] = IRQ_سېټ_ALL;
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
    طالما ((i & IRQ_ماسک) != IRQ_NONE) {
        gic_cpuiface->eoi = i;
        i = gic_cpuiface->int_ack;
    }
    gic_cpuiface->icontrol = 1;
}

فراغ سېټ_IRQTrigger(irq_t irq, bool_t trigger)
{
    /* in the gic_config, there is a 2 bit field for each irq,
     * setting the most significant bit of this field makes the irq edge-triggered,
     * while 0 indicates that it is level-triggered */
    کلمه_t شاخص = IRQT_منزل_IRQ(irq) / 16u;
    کلمه_t دننه_واټن = (IRQT_منزل_IRQ(irq) % 16u) * 2;
    إذا (trigger) {
        /* set the bit */
        gic_dist->config[شاخص] |= بټ(دننه_واټن + 1);
    } وإلا {
        gic_dist->config[شاخص] &= ~بټ(دننه_واټن + 1);
    }
}

BOOT_کوډ فراغ initIRQController(فراغ)
{
    /* irqInvalid cannot correspond to a valid IRQ index into the irq state array */
    أكد(INT_STATE_ARRAY_اندازه < IRQT_منزل_IRQ(irqInvalid));
    dist_init();
}

BOOT_کوډ فراغ cpu_initLocalIRQController(فراغ)
{
    فعال_irq[CURRENT_CPU_شاخص()] = IRQ_NONE;
    cpu_iface_init();
}

bool_t plat_SGITargetValid(کلمه_t موخه)
{
    /* written as <= so that the term is the same as in gic_v3 for the proofs */
    أرجع موخه <= GIC_SGI_عدد_TARGETS - 1;
}

فراغ plat_sendSGI(کلمه_t irq, کلمه_t موخه)
{
    gic_dist->sgi_control = (بټ(موخه) << (GICD_SGIR_CPUTARGETLIST_ښويدنه)) | (irq << GICD_SGIR_SGIINTID_ښويدنه);
}

#که_تعریف_شوی ENABLE_SMP_SUPPORT
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
فراغ ipi_send_موخه(irq_t irq, کلمه_t cpu_موخه_List)
{
    إذا (config_سېټ(CONFIG_PLAT_TX2)) {
        /* We need to swap the top 4 bits and the bottom 4 bits of the
         * cpuTargetList since the A57 cores with logical core ID 0-3 are
         * in cluster 1 and the Denver2 cores with logical core ID 4-5 are
         * in cluster 0. */
        cpu_موخه_List = ((cpu_موخه_List & 0xf) << 4) | ((cpu_موخه_List & 0xf0) >> 4);
    }
    gic_dist->sgi_control = (cpu_موخه_List << (GICD_SGIR_CPUTARGETLIST_ښويدنه)) | (IRQT_منزل_IRQ(
                                                                                      irq) << GICD_SGIR_SGIINTID_ښويدنه);
}

/*
 * Set CPU target for the interrupt if it's not a PPI
 */
فراغ سېټ_IRQTarget(irq_t irq, se_L4_کلمه موخه)
{
    uint8_t موخه_List = 1 << موخه;
    uint8_t *targets = (فراغ *)(gic_dist->targets);
    کلمه_t hwIRQ = IRQT_منزل_IRQ(irq);

    /* Return early if PPI */
    إذا (IRQ_IS_PPI(irq)) {
        fail("PPI can't have designated target core\n");
        أرجع;
    }
    targets[hwIRQ] = موخه_List;
}
#د_که_پای /* ENABLE_SMP_SUPPORT */

#که_تعریف_شوی CONFIG_ARM_HYPERVISOR_SUPPORT

#که_نه_تعریف_شوی GIC_V2_VCPUCTRL_PPTR
#تېروتنه GIC_V2_VCPUCTRL_PPTR must be defined لكل مجازي حافظه access منزل the gic مجازي cpu interface control
#وإلا  /* GIC_PL400_GICVCPUCTRL_PPTR */
متطاير بنية gich_vcpu_ctrl_map *gic_vcpu_ctrl =
    (متطاير بنية gich_vcpu_ctrl_map *)(GIC_V2_VCPUCTRL_PPTR);
#د_که_پای /* GIC_PL400_GICVCPUCTRL_PPTR */

کلمه_t gic_vcpu_عدد_list_regs;

#د_که_پای /* End of CONFIG_ARM_HYPERVISOR_SUPPORT */
