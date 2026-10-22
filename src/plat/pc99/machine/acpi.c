/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>
#अन्तर्भाव <util.h>
#अन्तर्भाव <assert.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <linker.h>
#अन्तर्भाव <plat/machine.h>
#अन्तर्भाव <plat/machine/acpi.h>
#अन्तर्भाव <plat/machine/devices.h>

गणना acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
प्रकारनिर्देश संरचना acpi_dmar {
    acpi_शीर्षकम्_t शीर्षकम्;
    uint8_t       host_addr_विस्तारः;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               परिमाण(acpi_dmar_t) == परिमाण(acpi_शीर्षकम्_t) + 12)

/* DMA Remapping Structure Header */
प्रकारनिर्देश संरचना acpi_dmar_शीर्षकम् {
    uint16_t type;
    uint16_t दीर्घता;
} acpi_dmar_शीर्षकम्_t;
compile_assert(acpi_dmar_शीर्षकम्_packed, परिमाण(acpi_dmar_शीर्षकम्_t) == 4)

/* DMA Remapping Structure Types */
गणना acpi_सारणी_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
प्रकारनिर्देश संरचना acpi_dmar_drhd {
    acpi_dmar_शीर्षकम्_t शीर्षकम्;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_आधारः[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               परिमाण(acpi_dmar_drhd_t) == परिमाण(acpi_dmar_शीर्षकम्_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
प्रकारनिर्देश संरचना acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  दीर्घता;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    संरचना {
        uint8_t dev;
        uint8_t fun;
    } मार्गः_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, परिमाण(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
प्रकारनिर्देश संरचना acpi_dmar_rmrr {
    acpi_dmar_शीर्षकम्_t   शीर्षकम्;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_आधारः[2];
    uint32_t             reg_सीमा[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, परिमाण(acpi_dmar_rmrr_t) ==
               परिमाण(acpi_dmar_शीर्षकम्_t) + 20 + परिमाण(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
प्रकारनिर्देश संरचना acpi_fadt {
    acpi_शीर्षकम्_t  शीर्षकम्;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               परिमाण(acpi_fadt_t) == परिमाण(acpi_शीर्षकम्_t) + 80)

/* Multiple APIC Description Table (MADT) */
प्रकारनिर्देश संरचना acpi_madt {
    acpi_शीर्षकम्_t शीर्षकम्;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               परिमाण(acpi_madt_t) == परिमाण(acpi_शीर्षकम्_t) + 8)

प्रकारनिर्देश संरचना acpi_madt_शीर्षकम् {
    uint8_t type;
    uint8_t दीर्घता;
} acpi_madt_शीर्षकम्_t;
compile_assert(acpi_madt_शीर्षकम्_packed, परिमाण(acpi_madt_शीर्षकम्_t) == 2)

गणना acpi_सारणी_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

प्रकारनिर्देश संरचना acpi_madt_apic {
    acpi_madt_शीर्षकम्_t शीर्षकम्;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               परिमाण(acpi_madt_apic_t) == परिमाण(acpi_madt_शीर्षकम्_t) + 6)

प्रकारनिर्देश संरचना acpi_madt_x2apic {
    acpi_madt_शीर्षकम्_t  शीर्षकम्;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               परिमाण(acpi_madt_x2apic_t) == परिमाण(acpi_madt_शीर्षकम्_t) + 14)

प्रकारनिर्देश संरचना acpi_madt_ioapic {
    acpi_madt_शीर्षकम्_t शीर्षकम्;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               परिमाण(acpi_madt_ioapic_t) == परिमाण(acpi_madt_शीर्षकम्_t) + 10)

प्रकारनिर्देश संरचना acpi_madt_iso {
    acpi_madt_शीर्षकम्_t शीर्षकम्;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_नीचः;
    uint16_t           gsi_उच्चः;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               परिमाण(acpi_madt_iso_t) == परिमाण(acpi_madt_शीर्षकम्_t) + 8)

#परिभाषय MADT_MAX_प्रविष्टिः_आकार परिमाण(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_प्रविष्टिः_आकार, परिमाण(acpi_madt_apic_t) <= MADT_MAX_प्रविष्टिः_आकार)
compile_assert(acpi_madt_ioapic_प्रविष्टिः_आकार, परिमाण(acpi_madt_ioapic_t) <= MADT_MAX_प्रविष्टिः_आकार)
compile_assert(acpi_madt_iso_प्रविष्टिः_आकार, परिमाण(acpi_madt_iso_t) <= MADT_MAX_प्रविष्टिः_आकार)

/* workaround because string literals are not supported by C parser */
ध्रुव वर्ण acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ध्रुव वर्ण acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ध्रुव वर्ण acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ध्रुव वर्ण acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ध्रुव वर्ण madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ध्रुव वर्ण madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ध्रुव वर्ण madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ध्रुव वर्ण madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_दत्त वर्ण madt_प्रविष्टिः[MADT_MAX_प्रविष्टिः_आकार] ALIGN(4);

BOOT_संहिता स्थिर uint8_t acpi_calc_checksum(वर्ण *start, uint32_t दीर्घता)
{
    uint8_t checksum = 0;

    यावत् (दीर्घता > 0) {
        checksum += *start;
        start++;
        दीर्घता--;
    }
    प्रत्यागम checksum;
}

BOOT_संहिता स्थिर acpi_rsdp_t *acpi_get_rsdp(शून्य)
{
    वर्ण *addr;

    प्रति (addr = (वर्ण *)BIOS_PADDR_START; addr < (वर्ण *)BIOS_PADDR_अन्तः; addr += 16) {
        यदि (वाक्ये_तुलय(addr, acpi_str_rsd, 8) == 0) {
            यदि (acpi_calc_checksum(addr, ACPI_V1_आकार) == 0) {
                प्रत्यागम (acpi_rsdp_t *)addr;
            }
        }
    }
    प्रत्यागम रिक्त;
}

BOOT_संहिता स्थिर शून्य *acpi_सारणी_init(शून्य *प्रविष्टिः, गणना acpi_type सारणी_type)
{
    शून्य *acpi_सारणी;
    अचिह्नित पूर्णाङ्क पृष्ठानि_for_सारणी;
    अचिह्नित पूर्णाङ्क पृष्ठानि_for_शीर्षकम् = 1;

    /* if we need to map another page to read header */
    अचिह्नित दीर्घ विस्थापनम्_अन्तर्_page = (अचिह्नित दीर्घ)प्रविष्टिः & आवरणम्(LARGE_PAGE_द्वयङ्काः);
    यदि (आवरणम्(LARGE_PAGE_द्वयङ्काः) - विस्थापनम्_अन्तर्_page < परिमाण(acpi_rsdp_t)) {
        पृष्ठानि_for_शीर्षकम्++;
    }

    /* map in table's header */
    acpi_सारणी = map_अस्थायी_boot_page(प्रविष्टिः, पृष्ठानि_for_शीर्षकम्);

    विकल्प (सारणी_type) {
    पक्ष ACPI_RSDP: {
        acpi_rsdp_t *rsdp_प्रविष्टिः = (acpi_rsdp_t *)प्रविष्टिः;
        पृष्ठानि_for_सारणी = (rsdp_प्रविष्टिः->दीर्घता + विस्थापनम्_अन्तर्_page) / आवरणम्(LARGE_PAGE_द्वयङ्काः) + 1;
        भङ्ग;
    }
    पक्ष ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_प्रविष्टिः = (acpi_rsdt_t *)प्रविष्टिः;
        पृष्ठानि_for_सारणी = (rsdt_प्रविष्टिः->शीर्षकम्.दीर्घता + विस्थापनम्_अन्तर्_page) / आवरणम्(LARGE_PAGE_द्वयङ्काः) + 1;
        भङ्ग;
    }
    स्वभाव:
        प्रारूप_लिख("Error: Mapping unknown ACPI table type\n");
        प्रतिज्ञा(false);
        प्रत्यागम रिक्त;
    }

    /* map in full table */
    acpi_सारणी = map_अस्थायी_boot_page(प्रविष्टिः, पृष्ठानि_for_सारणी);

    प्रत्यागम acpi_सारणी;
}

BOOT_संहिता bool_t acpi_init(acpi_rsdp_t *rsdp_दत्त)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    यदि (acpi_rsdp == रिक्त) {
        प्रारूप_लिख("BIOS: No ACPI support detected\n");
        प्रत्यागम false;
    }
    प्रारूप_लिख("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_सारणी_init(acpi_rsdp, ACPI_RSDP);
    प्रारूप_लिख("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_दत्त = *acpi_rsdp;

    /* perform final validation */
    प्रत्यागम acpi_validate_rsdp(rsdp_दत्त);
}

BOOT_संहिता bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    यदि (acpi_calc_checksum((वर्ण *)acpi_rsdp, ACPI_V1_आकार) != 0) {
        प्रारूप_लिख("BIOS: ACPIv1 information corrupt\n");
        प्रत्यागम false;
    }

    यदि (acpi_rsdp->revision > 0 && acpi_calc_checksum((वर्ण *)acpi_rsdp, परिमाण(*acpi_rsdp)) != 0) {
        प्रारूप_लिख("BIOS: ACPIv2 information corrupt\n");
        प्रत्यागम false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_स्थानाङ्कः;
    प्रारूप_लिख("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_सारणी_init(acpi_rsdt, ACPI_RSDT);
    प्रारूप_लिख("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    प्रतिज्ञा(acpi_rsdt_mapped->शीर्षकम्.दीर्घता > 0);
    यदि (acpi_calc_checksum((वर्ण *)acpi_rsdt_mapped, acpi_rsdt_mapped->शीर्षकम्.दीर्घता) != 0) {
        प्रारूप_लिख("ACPI: RSDT checksum failure\n");
        प्रत्यागम false;
    }

    प्रत्यागम true;
}

BOOT_संहिता स्थिर bool_t acpi_madt_valid(acpi_madt_शीर्षकम्_t *शीर्षकम्, ध्रुव वर्ण *name, uint8_t expected)
{
    यदि (शीर्षकम्->दीर्घता < expected) {
        प्रारूप_लिख("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, शीर्षकम्->दीर्घता, expected);
        प्रत्यागम false;
    }
    प्रत्यागम true;
}

BOOT_संहिता uint32_t acpi_madt_अवलोकनम्(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *अङ्कः_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    अचिह्नित पूर्णाङ्क        entries;
    अचिह्नित पूर्णाङ्क        प्रविष्टिः_आकार;
    uint32_t            अङ्कः_cpu;
    uint32_t            गणनम्;
    acpi_madt_t        *acpi_madt;
    acpi_madt_शीर्षकम्_t *acpi_madt_शीर्षकम्;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_सारणी_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_स्थानाङ्कः, ACPI_RSDT);

    अङ्कः_cpu = 0;
    *अङ्कः_ioapic = 0;

    प्रतिज्ञा(acpi_rsdt_mapped->शीर्षकम्.दीर्घता >= परिमाण(acpi_शीर्षकम्_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->शीर्षकम्.दीर्घता - परिमाण(acpi_शीर्षकम्_t)) / परिमाण(uint32_t);
    प्रति (गणनम् = 0; गणनम् < entries; गणनम्++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->प्रविष्टिः[गणनम्];
        acpi_madt_mapped = (acpi_madt_t *)acpi_सारणी_init(acpi_madt, ACPI_RSDT);

        यदि (वाक्ये_तुलय(acpi_str_apic, acpi_madt_mapped->शीर्षकम्.signature, 4) == 0) {
            प्रारूप_लिख("ACPI: MADT paddr=%p\n", acpi_madt);
            प्रारूप_लिख("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            प्रारूप_लिख("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            प्रारूप_लिख("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_शीर्षकम् = (acpi_madt_शीर्षकम्_t *)(acpi_madt_mapped + 1);

            यावत् ((वर्ण *)acpi_madt_शीर्षकम् < (वर्ण *)acpi_madt_mapped + acpi_madt_mapped->शीर्षकम्.दीर्घता) {
                प्रविष्टिः_आकार = MIN(acpi_madt_शीर्षकम्->दीर्घता, MADT_MAX_प्रविष्टिः_आकार);
                स्मृतिं_प्रतिलिख(madt_प्रविष्टिः, acpi_madt_शीर्षकम्, प्रविष्टिः_आकार);

                विकल्प (acpi_madt_शीर्षकम्->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                पक्ष MADT_APIC: {
                    यदि (!acpi_madt_valid(acpi_madt_शीर्षकम्, madt_str_apic, परिमाण(acpi_madt_apic_t))) {
                        भङ्ग;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_प्रविष्टिः;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    यदि (flags == 1) {
                        प्रारूप_लिख("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        यदि (अङ्कः_cpu == CONFIG_MAX_अङ्कः_NODES) {
                            प्रारूप_लिख("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_अङ्कः_NODES);
                        } अन्यथा {
                            cpu_list[अङ्कः_cpu] = cpu_id;
                            अङ्कः_cpu++;
                        }
                    }
                    भङ्ग;
                }
                पक्ष MADT_x2APIC: {
                    यदि (!acpi_madt_valid(acpi_madt_शीर्षकम्, madt_str_x2apic, परिमाण(acpi_madt_x2apic_t))) {
                        भङ्ग;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_प्रविष्टिः;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    यदि (flags == 1) {
                        प्रारूप_लिख("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        यदि (अङ्कः_cpu == CONFIG_MAX_अङ्कः_NODES) {
                            प्रारूप_लिख("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_अङ्कः_NODES);
                        } अन्यथा {
                            cpu_list[अङ्कः_cpu] = cpu_id;
                            अङ्कः_cpu++;
                        }
                    }
                    भङ्ग;
                }
                पक्ष MADT_IOAPIC:
                    यदि (!acpi_madt_valid(acpi_madt_शीर्षकम्, madt_str_ioapic, परिमाण(acpi_madt_ioapic_t))) {
                        भङ्ग;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_प्रविष्टिः;

                    प्रारूप_लिख(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    यदि (*अङ्कः_ioapic == CONFIG_MAX_अङ्कः_IOAPIC) {
                        प्रारूप_लिख("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_अङ्कः_IOAPIC);
                    } अन्यथा {
                        ioapic_paddrs[*अङ्कः_ioapic] = madt_ioapic->ioapic_addr;
                        (*अङ्कः_ioapic)++;
                    }
                    भङ्ग;
                पक्ष MADT_ISO: {
                    यदि (!acpi_madt_valid(acpi_madt_शीर्षकम्, madt_str_iso, परिमाण(acpi_madt_iso_t))) {
                        भङ्ग;
                    }
                    अप्रयुक्तम् acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_प्रविष्टिः;

                    प्रारूप_लिख("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_उच्चः, iso->gsi_नीचः, iso->flags);
                    भङ्ग;
                }
                स्वभाव:
                    भङ्ग;
                }
                acpi_madt_शीर्षकम् = (acpi_madt_शीर्षकम्_t *)((वर्ण *)acpi_madt_शीर्षकम् + acpi_madt_शीर्षकम्->दीर्घता);
            }
        }
    }

    प्रारूप_लिख("ACPI: %d CPU(s) detected\n", अङ्कः_cpu);

    प्रत्यागम अङ्कः_cpu;
}

BOOT_संहिता bool_t acpi_fadt_अवलोकनम्(
    acpi_rsdp_t *acpi_rsdp
)
{
    अचिह्नित पूर्णाङ्क entries;
    uint32_t            गणनम्;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_सारणी_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_स्थानाङ्कः, ACPI_RSDT);

    प्रतिज्ञा(acpi_rsdt_mapped->शीर्षकम्.दीर्घता >= परिमाण(acpi_शीर्षकम्_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->शीर्षकम्.दीर्घता - परिमाण(acpi_शीर्षकम्_t)) / परिमाण(uint32_t);
    प्रति (गणनम् = 0; गणनम् < entries; गणनम्++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->प्रविष्टिः[गणनम्];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_सारणी_init(acpi_fadt, ACPI_RSDT);

        यदि (वाक्ये_तुलय(acpi_str_fadt, acpi_fadt_mapped->शीर्षकम्.signature, 4) == 0) {
            प्रारूप_लिख("ACPI: FADT paddr=%p\n", acpi_fadt);
            प्रारूप_लिख("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            प्रारूप_लिख("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            यदि (config_समुच्चयः(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                प्रारूप_लिख("system requires apic physical mode\n");
                प्रत्यागम false;
            }
        }
    }

    प्रत्यागम true;
}

BOOT_संहिता शून्य acpi_dmar_अवलोकनम्(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *अङ्कः_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    अचिह्नित पूर्णाङ्क entries;
    uint32_t गणनम्;
    uint32_t reg_basel, reg_baseh;
    पूर्णाङ्क rmrr_गणनम्;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_शीर्षकम्_t   *acpi_dmar_शीर्षकम्;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_सारणी_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_स्थानाङ्कः, ACPI_RSDT);

    *अङ्कः_drhu = 0;
    rmrr_गणनम् = 0;

    प्रतिज्ञा(acpi_rsdt_mapped->शीर्षकम्.दीर्घता >= परिमाण(acpi_शीर्षकम्_t));
    entries = (acpi_rsdt_mapped->शीर्षकम्.दीर्घता - परिमाण(acpi_शीर्षकम्_t)) / परिमाण(uint32_t);
    प्रति (गणनम् = 0; गणनम् < entries; गणनम्++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->प्रविष्टिः[गणनम्];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_सारणी_init(acpi_dmar, ACPI_RSDT);

        यदि (वाक्ये_तुलय(acpi_str_dmar, acpi_dmar_mapped->शीर्षकम्.signature, 4) == 0) {
            प्रारूप_लिख("ACPI: DMAR paddr=%p\n", acpi_dmar);
            प्रारूप_लिख("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            प्रारूप_लिख("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_विस्तारः + 1);
            acpi_dmar_शीर्षकम् = (acpi_dmar_शीर्षकम्_t *)(acpi_dmar_mapped + 1);

            यावत् ((वर्ण *)acpi_dmar_शीर्षकम् < (वर्ण *)acpi_dmar_mapped + acpi_dmar_mapped->शीर्षकम्.दीर्घता) {
                विकल्प (acpi_dmar_शीर्षकम्->type) {

                पक्ष DMAR_DRHD:
                    यदि (*अङ्कः_drhu == max_drhu_list_len) {
                        प्रारूप_लिख("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *अङ्कः_drhu = 0; /* report zero IOMMUs */
                        प्रत्यागम;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_शीर्षकम्)->reg_आधारः[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_शीर्षकम्)->reg_आधारः[1];
                    /* check if value fits into uint32_t */
                    यदि (reg_baseh != 0) {
                        प्रारूप_लिख("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *अङ्कः_drhu = 0; /* report zero IOMMUs */
                        प्रत्यागम;
                    }
                    drhu_list[*अङ्कः_drhu] = (paddr_t)reg_basel;
                    (*अङ्कः_drhu)++;
                    भङ्ग;

                पक्ष DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_शीर्षकम्;
                    यदि (acpi_dmar_rmrr->reg_आधारः[1] != 0 ||
                        acpi_dmar_rmrr->reg_सीमा[1] != 0) {
                        प्रारूप_लिख("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *अङ्कः_drhu = 0;
                        प्रत्यागम ;
                    }

                    प्रति (i = 0; i <= (acpi_dmar_शीर्षकम्->दीर्घता - परिमाण(acpi_dmar_rmrr_t)) / परिमाण(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        यदि (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            प्रारूप_लिख("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *अङ्कः_drhu = 0; /* report zero IOMMUs */
                            प्रत्यागम;
                        }

                        यदि (acpi_dmar_devscope->दीर्घता > परिमाण(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            प्रारूप_लिख("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *अङ्कः_drhu = 0; /* report zero IOMMUs */
                            प्रत्यागम;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->मार्गः_0.dev,
                                acpi_dmar_devscope->मार्गः_0.fun
                            );

                        यदि (rmrr_गणनम् == CONFIG_MAX_RMRR_ENTRIES) {
                            प्रारूप_लिख("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *अङ्कः_drhu = 0;
                            प्रत्यागम;
                        }
                        प्रारूप_लिख("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->मार्गः_0.dev,
                               acpi_dmar_devscope->मार्गः_0.fun
                              );

                        rmrr_list->entries[rmrr_गणनम्].device = dev_id;
                        rmrr_list->entries[rmrr_गणनम्].आधारः = acpi_dmar_rmrr->reg_आधारः[0];
                        rmrr_list->entries[rmrr_गणनम्].सीमा = acpi_dmar_rmrr->reg_सीमा[0];
                        rmrr_गणनम्++;
                    }
                    भङ्ग;

                पक्ष DMAR_ATSR:
                    /* not implemented yet */
                    भङ्ग;

                स्वभाव:
                    प्रारूप_लिख("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_शीर्षकम्->type);
                }
                acpi_dmar_शीर्षकम् = (acpi_dmar_शीर्षकम्_t *)((वर्ण *)acpi_dmar_शीर्षकम् + acpi_dmar_शीर्षकम्->दीर्घता);
            }
        }
    }
    rmrr_list->अङ्कः = rmrr_गणनम्;
    प्रारूप_लिख("ACPI: %d IOMMUs detected\n", *अङ्कः_drhu);
}
