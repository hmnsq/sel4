/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#అన్తర్భావ <config.h>
#అన్తర్భావ <util.h>
#అన్తర్భావ <assert.h>
#అన్తర్భావ <machine/io.h>
#అన్తర్భావ <linker.h>
#అన్తర్భావ <plat/machine.h>
#అన్తర్భావ <plat/machine/acpi.h>
#అన్తర్భావ <plat/machine/devices.h>

గణన acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
రకనిర్వచనం నిర్మాణం acpi_dmar {
    acpi_శీర్షిక_t శీర్షిక;
    uint8_t       host_addr_వెడల్పు;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               పరిమాణం(acpi_dmar_t) == పరిమాణం(acpi_శీర్షిక_t) + 12)

/* DMA Remapping Structure Header */
రకనిర్వచనం నిర్మాణం acpi_dmar_శీర్షిక {
    uint16_t type;
    uint16_t నిడివి;
} acpi_dmar_శీర్షిక_t;
compile_assert(acpi_dmar_శీర్షిక_packed, పరిమాణం(acpi_dmar_శీర్షిక_t) == 4)

/* DMA Remapping Structure Types */
గణన acpi_పట్టిక_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
రకనిర్వచనం నిర్మాణం acpi_dmar_drhd {
    acpi_dmar_శీర్షిక_t శీర్షిక;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_పునాది[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               పరిమాణం(acpi_dmar_drhd_t) == పరిమాణం(acpi_dmar_శీర్షిక_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
రకనిర్వచనం నిర్మాణం acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  నిడివి;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  ప్రారంభం_bus;
    నిర్మాణం {
        uint8_t dev;
        uint8_t fun;
    } మార్గం_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, పరిమాణం(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
రకనిర్వచనం నిర్మాణం acpi_dmar_rmrr {
    acpi_dmar_శీర్షిక_t   శీర్షిక;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_పునాది[2];
    uint32_t             reg_హద్దు[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, పరిమాణం(acpi_dmar_rmrr_t) ==
               పరిమాణం(acpi_dmar_శీర్షిక_t) + 20 + పరిమాణం(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
రకనిర్వచనం నిర్మాణం acpi_fadt {
    acpi_శీర్షిక_t  శీర్షిక;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               పరిమాణం(acpi_fadt_t) == పరిమాణం(acpi_శీర్షిక_t) + 80)

/* Multiple APIC Description Table (MADT) */
రకనిర్వచనం నిర్మాణం acpi_madt {
    acpi_శీర్షిక_t శీర్షిక;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               పరిమాణం(acpi_madt_t) == పరిమాణం(acpi_శీర్షిక_t) + 8)

రకనిర్వచనం నిర్మాణం acpi_madt_శీర్షిక {
    uint8_t type;
    uint8_t నిడివి;
} acpi_madt_శీర్షిక_t;
compile_assert(acpi_madt_శీర్షిక_packed, పరిమాణం(acpi_madt_శీర్షిక_t) == 2)

గణన acpi_పట్టిక_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

రకనిర్వచనం నిర్మాణం acpi_madt_apic {
    acpi_madt_శీర్షిక_t శీర్షిక;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               పరిమాణం(acpi_madt_apic_t) == పరిమాణం(acpi_madt_శీర్షిక_t) + 6)

రకనిర్వచనం నిర్మాణం acpi_madt_x2apic {
    acpi_madt_శీర్షిక_t  శీర్షిక;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               పరిమాణం(acpi_madt_x2apic_t) == పరిమాణం(acpi_madt_శీర్షిక_t) + 14)

రకనిర్వచనం నిర్మాణం acpi_madt_ioapic {
    acpi_madt_శీర్షిక_t శీర్షిక;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               పరిమాణం(acpi_madt_ioapic_t) == పరిమాణం(acpi_madt_శీర్షిక_t) + 10)

రకనిర్వచనం నిర్మాణం acpi_madt_iso {
    acpi_madt_శీర్షిక_t శీర్షిక;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            ఆధారం;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_దిగువ;
    uint16_t           gsi_ఎగువ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               పరిమాణం(acpi_madt_iso_t) == పరిమాణం(acpi_madt_శీర్షిక_t) + 8)

#నిర్వచించు MADT_MAX_నమోదు_కొలత పరిమాణం(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_నమోదు_కొలత, పరిమాణం(acpi_madt_apic_t) <= MADT_MAX_నమోదు_కొలత)
compile_assert(acpi_madt_ioapic_నమోదు_కొలత, పరిమాణం(acpi_madt_ioapic_t) <= MADT_MAX_నమోదు_కొలత)
compile_assert(acpi_madt_iso_నమోదు_కొలత, పరిమాణం(acpi_madt_iso_t) <= MADT_MAX_నమోదు_కొలత)

/* workaround because string literals are not supported by C parser */
స్థిరరాశి అక్షరం acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
స్థిరరాశి అక్షరం acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
స్థిరరాశి అక్షరం acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
స్థిరరాశి అక్షరం acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
స్థిరరాశి అక్షరం madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
స్థిరరాశి అక్షరం madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
స్థిరరాశి అక్షరం madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
స్థిరరాశి అక్షరం madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_దత్తాంశం అక్షరం madt_నమోదు[MADT_MAX_నమోదు_కొలత] ALIGN(4);

BOOT_సంకేతం స్థిర uint8_t acpi_calc_checksum(అక్షరం *ప్రారంభం, uint32_t నిడివి)
{
    uint8_t checksum = 0;

    అయితే (నిడివి > 0) {
        checksum += *ప్రారంభం;
        ప్రారంభం++;
        నిడివి--;
    }
    తిరిగి checksum;
}

BOOT_సంకేతం స్థిర acpi_rsdp_t *acpi_get_rsdp(శూన్య)
{
    అక్షరం *addr;

    కొరకు (addr = (అక్షరం *)BIOS_PADDR_ప్రారంభం; addr < (అక్షరం *)BIOS_PADDR_చివర; addr += 16) {
        ఒకవేళ (అక్షరశ్రేణి_పోలిక(addr, acpi_str_rsd, 8) == 0) {
            ఒకవేళ (acpi_calc_checksum(addr, ACPI_V1_కొలత) == 0) {
                తిరిగి (acpi_rsdp_t *)addr;
            }
        }
    }
    తిరిగి ఖాళీ;
}

BOOT_సంకేతం స్థిర శూన్య *acpi_పట్టిక_init(శూన్య *నమోదు_2, గణన acpi_type పట్టిక_type)
{
    శూన్య *acpi_పట్టిక;
    గుర్తులేని పూర్ణాంకం పుటలు_for_పట్టిక;
    గుర్తులేని పూర్ణాంకం పుటలు_for_శీర్షిక = 1;

    /* if we need to map another page to read header */
    గుర్తులేని దీర్ఘ స్థానభ్రంశం_లోపల_పుట = (గుర్తులేని దీర్ఘ)నమోదు_2 & ముసుగు(LARGE_పుట_బిట్లు);
    ఒకవేళ (ముసుగు(LARGE_పుట_బిట్లు) - స్థానభ్రంశం_లోపల_పుట < పరిమాణం(acpi_rsdp_t)) {
        పుటలు_for_శీర్షిక++;
    }

    /* map in table's header */
    acpi_పట్టిక = map_తాత్కాలిక_boot_పుట(నమోదు_2, పుటలు_for_శీర్షిక);

    ఎంపిక (పట్టిక_type) {
    స్థితి ACPI_RSDP: {
        acpi_rsdp_t *rsdp_నమోదు = (acpi_rsdp_t *)నమోదు_2;
        పుటలు_for_పట్టిక = (rsdp_నమోదు->నిడివి + స్థానభ్రంశం_లోపల_పుట) / ముసుగు(LARGE_పుట_బిట్లు) + 1;
        ఆపు;
    }
    స్థితి ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_నమోదు = (acpi_rsdt_t *)నమోదు_2;
        పుటలు_for_పట్టిక = (rsdt_నమోదు->శీర్షిక.నిడివి + స్థానభ్రంశం_లోపల_పుట) / ముసుగు(LARGE_పుట_బిట్లు) + 1;
        ఆపు;
    }
    అప్రమేయ:
        రాయి("Error: Mapping unknown ACPI table type\n");
        ప్రతిజ్ఞా(false);
        తిరిగి ఖాళీ;
    }

    /* map in full table */
    acpi_పట్టిక = map_తాత్కాలిక_boot_పుట(నమోదు_2, పుటలు_for_పట్టిక);

    తిరిగి acpi_పట్టిక;
}

BOOT_సంకేతం bool_t acpi_init(acpi_rsdp_t *rsdp_దత్తాంశం)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    ఒకవేళ (acpi_rsdp == ఖాళీ) {
        రాయి("BIOS: No ACPI support detected\n");
        తిరిగి false;
    }
    రాయి("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_పట్టిక_init(acpi_rsdp, ACPI_RSDP);
    రాయి("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_దత్తాంశం = *acpi_rsdp;

    /* perform final validation */
    తిరిగి acpi_validate_rsdp(rsdp_దత్తాంశం);
}

BOOT_సంకేతం bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    ఒకవేళ (acpi_calc_checksum((అక్షరం *)acpi_rsdp, ACPI_V1_కొలత) != 0) {
        రాయి("BIOS: ACPIv1 information corrupt\n");
        తిరిగి false;
    }

    ఒకవేళ (acpi_rsdp->revision > 0 && acpi_calc_checksum((అక్షరం *)acpi_rsdp, పరిమాణం(*acpi_rsdp)) != 0) {
        రాయి("BIOS: ACPIv2 information corrupt\n");
        తిరిగి false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(మాట_t)acpi_rsdp->rsdt_చిరునామా;
    రాయి("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_పట్టిక_init(acpi_rsdt, ACPI_RSDT);
    రాయి("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    ప్రతిజ్ఞా(acpi_rsdt_mapped->శీర్షిక.నిడివి > 0);
    ఒకవేళ (acpi_calc_checksum((అక్షరం *)acpi_rsdt_mapped, acpi_rsdt_mapped->శీర్షిక.నిడివి) != 0) {
        రాయి("ACPI: RSDT checksum failure\n");
        తిరిగి false;
    }

    తిరిగి true;
}

BOOT_సంకేతం స్థిర bool_t acpi_madt_valid(acpi_madt_శీర్షిక_t *శీర్షిక, స్థిరరాశి అక్షరం *name, uint8_t expected)
{
    ఒకవేళ (శీర్షిక->నిడివి < expected) {
        రాయి("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, శీర్షిక->నిడివి, expected);
        తిరిగి false;
    }
    తిరిగి true;
}

BOOT_సంకేతం uint32_t acpi_madt_పరిశీలన(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *అంకె_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    గుర్తులేని పూర్ణాంకం        entries;
    గుర్తులేని పూర్ణాంకం        నమోదు_కొలత;
    uint32_t            అంకె_cpu;
    uint32_t            లెక్క;
    acpi_madt_t        *acpi_madt;
    acpi_madt_శీర్షిక_t *acpi_madt_శీర్షిక;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_పట్టిక_init((acpi_rsdt_t *)(మాట_t)acpi_rsdp->rsdt_చిరునామా, ACPI_RSDT);

    అంకె_cpu = 0;
    *అంకె_ioapic = 0;

    ప్రతిజ్ఞా(acpi_rsdt_mapped->శీర్షిక.నిడివి >= పరిమాణం(acpi_శీర్షిక_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->శీర్షిక.నిడివి - పరిమాణం(acpi_శీర్షిక_t)) / పరిమాణం(uint32_t);
    కొరకు (లెక్క = 0; లెక్క < entries; లెక్క++) {
        acpi_madt = (acpi_madt_t *)(మాట_t)acpi_rsdt_mapped->నమోదు_2[లెక్క];
        acpi_madt_mapped = (acpi_madt_t *)acpi_పట్టిక_init(acpi_madt, ACPI_RSDT);

        ఒకవేళ (అక్షరశ్రేణి_పోలిక(acpi_str_apic, acpi_madt_mapped->శీర్షిక.signature, 4) == 0) {
            రాయి("ACPI: MADT paddr=%p\n", acpi_madt);
            రాయి("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            రాయి("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            రాయి("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_శీర్షిక = (acpi_madt_శీర్షిక_t *)(acpi_madt_mapped + 1);

            అయితే ((అక్షరం *)acpi_madt_శీర్షిక < (అక్షరం *)acpi_madt_mapped + acpi_madt_mapped->శీర్షిక.నిడివి) {
                నమోదు_కొలత = MIN(acpi_madt_శీర్షిక->నిడివి, MADT_MAX_నమోదు_కొలత);
                జ్ఞాపకం_నకలు(madt_నమోదు, acpi_madt_శీర్షిక, నమోదు_కొలత);

                ఎంపిక (acpi_madt_శీర్షిక->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                స్థితి MADT_APIC: {
                    ఒకవేళ (!acpi_madt_valid(acpi_madt_శీర్షిక, madt_str_apic, పరిమాణం(acpi_madt_apic_t))) {
                        ఆపు;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_నమోదు;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    ఒకవేళ (flags == 1) {
                        రాయి("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        ఒకవేళ (అంకె_cpu == CONFIG_MAX_అంకె_NODES) {
                            రాయి("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_అంకె_NODES);
                        } లేకపోతే {
                            cpu_list[అంకె_cpu] = cpu_id;
                            అంకె_cpu++;
                        }
                    }
                    ఆపు;
                }
                స్థితి MADT_x2APIC: {
                    ఒకవేళ (!acpi_madt_valid(acpi_madt_శీర్షిక, madt_str_x2apic, పరిమాణం(acpi_madt_x2apic_t))) {
                        ఆపు;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_నమోదు;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    ఒకవేళ (flags == 1) {
                        రాయి("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        ఒకవేళ (అంకె_cpu == CONFIG_MAX_అంకె_NODES) {
                            రాయి("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_అంకె_NODES);
                        } లేకపోతే {
                            cpu_list[అంకె_cpu] = cpu_id;
                            అంకె_cpu++;
                        }
                    }
                    ఆపు;
                }
                స్థితి MADT_IOAPIC:
                    ఒకవేళ (!acpi_madt_valid(acpi_madt_శీర్షిక, madt_str_ioapic, పరిమాణం(acpi_madt_ioapic_t))) {
                        ఆపు;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_నమోదు;

                    రాయి(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    ఒకవేళ (*అంకె_ioapic == CONFIG_MAX_అంకె_IOAPIC) {
                        రాయి("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_అంకె_IOAPIC);
                    } లేకపోతే {
                        ioapic_paddrs[*అంకె_ioapic] = madt_ioapic->ioapic_addr;
                        (*అంకె_ioapic)++;
                    }
                    ఆపు;
                స్థితి MADT_ISO: {
                    ఒకవేళ (!acpi_madt_valid(acpi_madt_శీర్షిక, madt_str_iso, పరిమాణం(acpi_madt_iso_t))) {
                        ఆపు;
                    }
                    వాడని acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_నమోదు;

                    రాయి("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->ఆధారం, iso->gsi_ఎగువ, iso->gsi_దిగువ, iso->flags);
                    ఆపు;
                }
                అప్రమేయ:
                    ఆపు;
                }
                acpi_madt_శీర్షిక = (acpi_madt_శీర్షిక_t *)((అక్షరం *)acpi_madt_శీర్షిక + acpi_madt_శీర్షిక->నిడివి);
            }
        }
    }

    రాయి("ACPI: %d CPU(s) detected\n", అంకె_cpu);

    తిరిగి అంకె_cpu;
}

BOOT_సంకేతం bool_t acpi_fadt_పరిశీలన(
    acpi_rsdp_t *acpi_rsdp
)
{
    గుర్తులేని పూర్ణాంకం entries;
    uint32_t            లెక్క;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_పట్టిక_init((acpi_rsdt_t *)(మాట_t)acpi_rsdp->rsdt_చిరునామా, ACPI_RSDT);

    ప్రతిజ్ఞా(acpi_rsdt_mapped->శీర్షిక.నిడివి >= పరిమాణం(acpi_శీర్షిక_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->శీర్షిక.నిడివి - పరిమాణం(acpi_శీర్షిక_t)) / పరిమాణం(uint32_t);
    కొరకు (లెక్క = 0; లెక్క < entries; లెక్క++) {
        acpi_fadt = (acpi_fadt_t *)(మాట_t)acpi_rsdt_mapped->నమోదు_2[లెక్క];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_పట్టిక_init(acpi_fadt, ACPI_RSDT);

        ఒకవేళ (అక్షరశ్రేణి_పోలిక(acpi_str_fadt, acpi_fadt_mapped->శీర్షిక.signature, 4) == 0) {
            రాయి("ACPI: FADT paddr=%p\n", acpi_fadt);
            రాయి("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            రాయి("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            ఒకవేళ (config_సమితి(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & బిట్టు(19)) {
                రాయి("system requires apic physical mode\n");
                తిరిగి false;
            }
        }
    }

    తిరిగి true;
}

BOOT_సంకేతం శూన్య acpi_dmar_పరిశీలన(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *అంకె_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    మాట_t i;
    గుర్తులేని పూర్ణాంకం entries;
    uint32_t లెక్క;
    uint32_t reg_basel, reg_baseh;
    పూర్ణాంకం rmrr_లెక్క;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_శీర్షిక_t   *acpi_dmar_శీర్షిక;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_పట్టిక_init((acpi_rsdt_t *)(మాట_t)acpi_rsdp->rsdt_చిరునామా, ACPI_RSDT);

    *అంకె_drhu = 0;
    rmrr_లెక్క = 0;

    ప్రతిజ్ఞా(acpi_rsdt_mapped->శీర్షిక.నిడివి >= పరిమాణం(acpi_శీర్షిక_t));
    entries = (acpi_rsdt_mapped->శీర్షిక.నిడివి - పరిమాణం(acpi_శీర్షిక_t)) / పరిమాణం(uint32_t);
    కొరకు (లెక్క = 0; లెక్క < entries; లెక్క++) {
        acpi_dmar = (acpi_dmar_t *)(మాట_t)acpi_rsdt_mapped->నమోదు_2[లెక్క];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_పట్టిక_init(acpi_dmar, ACPI_RSDT);

        ఒకవేళ (అక్షరశ్రేణి_పోలిక(acpi_str_dmar, acpi_dmar_mapped->శీర్షిక.signature, 4) == 0) {
            రాయి("ACPI: DMAR paddr=%p\n", acpi_dmar);
            రాయి("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            రాయి("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_వెడల్పు + 1);
            acpi_dmar_శీర్షిక = (acpi_dmar_శీర్షిక_t *)(acpi_dmar_mapped + 1);

            అయితే ((అక్షరం *)acpi_dmar_శీర్షిక < (అక్షరం *)acpi_dmar_mapped + acpi_dmar_mapped->శీర్షిక.నిడివి) {
                ఎంపిక (acpi_dmar_శీర్షిక->type) {

                స్థితి DMAR_DRHD:
                    ఒకవేళ (*అంకె_drhu == max_drhu_list_len) {
                        రాయి("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *అంకె_drhu = 0; /* report zero IOMMUs */
                        తిరిగి;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_శీర్షిక)->reg_పునాది[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_శీర్షిక)->reg_పునాది[1];
                    /* check if value fits into uint32_t */
                    ఒకవేళ (reg_baseh != 0) {
                        రాయి("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *అంకె_drhu = 0; /* report zero IOMMUs */
                        తిరిగి;
                    }
                    drhu_list[*అంకె_drhu] = (paddr_t)reg_basel;
                    (*అంకె_drhu)++;
                    ఆపు;

                స్థితి DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_శీర్షిక;
                    ఒకవేళ (acpi_dmar_rmrr->reg_పునాది[1] != 0 ||
                        acpi_dmar_rmrr->reg_హద్దు[1] != 0) {
                        రాయి("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *అంకె_drhu = 0;
                        తిరిగి ;
                    }

                    కొరకు (i = 0; i <= (acpi_dmar_శీర్షిక->నిడివి - పరిమాణం(acpi_dmar_rmrr_t)) / పరిమాణం(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        ఒకవేళ (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            రాయి("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *అంకె_drhu = 0; /* report zero IOMMUs */
                            తిరిగి;
                        }

                        ఒకవేళ (acpi_dmar_devscope->నిడివి > పరిమాణం(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            రాయి("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *అంకె_drhu = 0; /* report zero IOMMUs */
                            తిరిగి;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->ప్రారంభం_bus,
                                acpi_dmar_devscope->మార్గం_0.dev,
                                acpi_dmar_devscope->మార్గం_0.fun
                            );

                        ఒకవేళ (rmrr_లెక్క == CONFIG_MAX_RMRR_ENTRIES) {
                            రాయి("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *అంకె_drhu = 0;
                            తిరిగి;
                        }
                        రాయి("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->ప్రారంభం_bus,
                               acpi_dmar_devscope->మార్గం_0.dev,
                               acpi_dmar_devscope->మార్గం_0.fun
                              );

                        rmrr_list->entries[rmrr_లెక్క].device = dev_id;
                        rmrr_list->entries[rmrr_లెక్క].పునాది = acpi_dmar_rmrr->reg_పునాది[0];
                        rmrr_list->entries[rmrr_లెక్క].హద్దు = acpi_dmar_rmrr->reg_హద్దు[0];
                        rmrr_లెక్క++;
                    }
                    ఆపు;

                స్థితి DMAR_ATSR:
                    /* not implemented yet */
                    ఆపు;

                అప్రమేయ:
                    రాయి("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_శీర్షిక->type);
                }
                acpi_dmar_శీర్షిక = (acpi_dmar_శీర్షిక_t *)((అక్షరం *)acpi_dmar_శీర్షిక + acpi_dmar_శీర్షిక->నిడివి);
            }
        }
    }
    rmrr_list->అంకె = rmrr_లెక్క;
    రాయి("ACPI: %d IOMMUs detected\n", *అంకె_drhu);
}
