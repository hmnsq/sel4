/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাৱ <config.h>
#অন্তর্ভাৱ <util.h>
#অন্তর্ভাৱ <assert.h>
#অন্তর্ভাৱ <machine/io.h>
#অন্তর্ভাৱ <linker.h>
#অন্তর্ভাৱ <plat/machine.h>
#অন্তর্ভাৱ <plat/machine/acpi.h>
#অন্তর্ভাৱ <plat/machine/devices.h>

গণনা acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
প্রকারনির্দেশ সংরচনা acpi_dmar {
    acpi_header_t header;
    uint8_t       host_addr_width;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               পরিমাণ(acpi_dmar_t) == পরিমাণ(acpi_header_t) + 12)

/* DMA Remapping Structure Header */
প্রকারনির্দেশ সংরচনা acpi_dmar_header {
    uint16_t type;
    uint16_t length;
} acpi_dmar_header_t;
compile_assert(acpi_dmar_header_packed, পরিমাণ(acpi_dmar_header_t) == 4)

/* DMA Remapping Structure Types */
গণনা acpi_table_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
প্রকারনির্দেশ সংরচনা acpi_dmar_drhd {
    acpi_dmar_header_t header;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_base[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               পরিমাণ(acpi_dmar_drhd_t) == পরিমাণ(acpi_dmar_header_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
প্রকারনির্দেশ সংরচনা acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  length;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    সংরচনা {
        uint8_t dev;
        uint8_t fun;
    } path_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, পরিমাণ(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
প্রকারনির্দেশ সংরচনা acpi_dmar_rmrr {
    acpi_dmar_header_t   header;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_base[2];
    uint32_t             reg_limit[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, পরিমাণ(acpi_dmar_rmrr_t) ==
               পরিমাণ(acpi_dmar_header_t) + 20 + পরিমাণ(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
প্রকারনির্দেশ সংরচনা acpi_fadt {
    acpi_header_t  header;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               পরিমাণ(acpi_fadt_t) == পরিমাণ(acpi_header_t) + 80)

/* Multiple APIC Description Table (MADT) */
প্রকারনির্দেশ সংরচনা acpi_madt {
    acpi_header_t header;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               পরিমাণ(acpi_madt_t) == পরিমাণ(acpi_header_t) + 8)

প্রকারনির্দেশ সংরচনা acpi_madt_header {
    uint8_t type;
    uint8_t length;
} acpi_madt_header_t;
compile_assert(acpi_madt_header_packed, পরিমাণ(acpi_madt_header_t) == 2)

গণনা acpi_table_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

প্রকারনির্দেশ সংরচনা acpi_madt_apic {
    acpi_madt_header_t header;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               পরিমাণ(acpi_madt_apic_t) == পরিমাণ(acpi_madt_header_t) + 6)

প্রকারনির্দেশ সংরচনা acpi_madt_x2apic {
    acpi_madt_header_t  header;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               পরিমাণ(acpi_madt_x2apic_t) == পরিমাণ(acpi_madt_header_t) + 14)

প্রকারনির্দেশ সংরচনা acpi_madt_ioapic {
    acpi_madt_header_t header;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               পরিমাণ(acpi_madt_ioapic_t) == পরিমাণ(acpi_madt_header_t) + 10)

প্রকারনির্দেশ সংরচনা acpi_madt_iso {
    acpi_madt_header_t header;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_নিচেৰ;
    uint16_t           gsi_উপৰেৰ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               পরিমাণ(acpi_madt_iso_t) == পরিমাণ(acpi_madt_header_t) + 8)

#define MADT_MAX_ENTRY_SIZE পরিমাণ(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_entry_size, পরিমাণ(acpi_madt_apic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_ioapic_entry_size, পরিমাণ(acpi_madt_ioapic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_iso_entry_size, পরিমাণ(acpi_madt_iso_t) <= MADT_MAX_ENTRY_SIZE)

/* workaround because string literals are not supported by C parser */
ধ্রুৱ ৱর্ণ acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ধ্রুৱ ৱর্ণ acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ধ্রুৱ ৱর্ণ acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ধ্রুৱ ৱর্ণ acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ধ্রুৱ ৱর্ণ madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ধ্রুৱ ৱর্ণ madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ধ্রুৱ ৱর্ণ madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ধ্রুৱ ৱর্ণ madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_DATA ৱর্ণ madt_entry[MADT_MAX_ENTRY_SIZE] ALIGN(4);

BOOT_CODE স্থির uint8_t acpi_calc_checksum(ৱর্ণ *start, uint32_t length)
{
    uint8_t checksum = 0;

    যেতিয়ালৈ (length > 0) {
        checksum += *start;
        start++;
        length--;
    }
    ঘূৰাই checksum;
}

BOOT_CODE স্থির acpi_rsdp_t *acpi_get_rsdp(শূন্য)
{
    ৱর্ণ *addr;

    বাবে (addr = (ৱর্ণ *)BIOS_PADDR_START; addr < (ৱর্ণ *)BIOS_PADDR_END; addr += 16) {
        যদি (strncmp(addr, acpi_str_rsd, 8) == 0) {
            যদি (acpi_calc_checksum(addr, ACPI_V1_SIZE) == 0) {
                ঘূৰাই (acpi_rsdp_t *)addr;
            }
        }
    }
    ঘূৰাই রিক্ত;
}

BOOT_CODE স্থির শূন্য *acpi_table_init(শূন্য *entry, গণনা acpi_type table_type)
{
    শূন্য *acpi_table;
    unsigned পূর্ণাঙ্ক pages_for_table;
    unsigned পূর্ণাঙ্ক pages_for_header = 1;

    /* if we need to map another page to read header */
    unsigned long offset_in_page = (unsigned long)entry & MASK(LARGE_PAGE_BITS);
    যদি (MASK(LARGE_PAGE_BITS) - offset_in_page < পরিমাণ(acpi_rsdp_t)) {
        pages_for_header++;
    }

    /* map in table's header */
    acpi_table = map_temp_boot_page(entry, pages_for_header);

    ৱিকল্প (table_type) {
    পক্ষ ACPI_RSDP: {
        acpi_rsdp_t *rsdp_entry = (acpi_rsdp_t *)entry;
        pages_for_table = (rsdp_entry->length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        ভঙ্গ;
    }
    পক্ষ ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_entry = (acpi_rsdt_t *)entry;
        pages_for_table = (rsdt_entry->header.length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        ভঙ্গ;
    }
    স্ৱভাৱ:
        printf("Error: Mapping unknown ACPI table type\n");
        প্রতিজ্ঞা(false);
        ঘূৰাই রিক্ত;
    }

    /* map in full table */
    acpi_table = map_temp_boot_page(entry, pages_for_table);

    ঘূৰাই acpi_table;
}

BOOT_CODE bool_t acpi_init(acpi_rsdp_t *rsdp_data)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    যদি (acpi_rsdp == রিক্ত) {
        printf("BIOS: No ACPI support detected\n");
        ঘূৰাই false;
    }
    printf("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_table_init(acpi_rsdp, ACPI_RSDP);
    printf("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_data = *acpi_rsdp;

    /* perform final validation */
    ঘূৰাই acpi_validate_rsdp(rsdp_data);
}

BOOT_CODE bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    যদি (acpi_calc_checksum((ৱর্ণ *)acpi_rsdp, ACPI_V1_SIZE) != 0) {
        printf("BIOS: ACPIv1 information corrupt\n");
        ঘূৰাই false;
    }

    যদি (acpi_rsdp->revision > 0 && acpi_calc_checksum((ৱর্ণ *)acpi_rsdp, পরিমাণ(*acpi_rsdp)) != 0) {
        printf("BIOS: ACPIv2 information corrupt\n");
        ঘূৰাই false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address;
    printf("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init(acpi_rsdt, ACPI_RSDT);
    printf("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    প্রতিজ্ঞা(acpi_rsdt_mapped->header.length > 0);
    যদি (acpi_calc_checksum((ৱর্ণ *)acpi_rsdt_mapped, acpi_rsdt_mapped->header.length) != 0) {
        printf("ACPI: RSDT checksum failure\n");
        ঘূৰাই false;
    }

    ঘূৰাই true;
}

BOOT_CODE স্থির bool_t acpi_madt_valid(acpi_madt_header_t *header, ধ্রুৱ ৱর্ণ *name, uint8_t expected)
{
    যদি (header->length < expected) {
        printf("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, header->length, expected);
        ঘূৰাই false;
    }
    ঘূৰাই true;
}

BOOT_CODE uint32_t acpi_madt_scan(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *অঙ্ক_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    unsigned পূর্ণাঙ্ক        entries;
    unsigned পূর্ণাঙ্ক        entry_size;
    uint32_t            অঙ্ক_cpu;
    uint32_t            গুনতি;
    acpi_madt_t        *acpi_madt;
    acpi_madt_header_t *acpi_madt_header;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    অঙ্ক_cpu = 0;
    *অঙ্ক_ioapic = 0;

    প্রতিজ্ঞা(acpi_rsdt_mapped->header.length >= পরিমাণ(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - পরিমাণ(acpi_header_t)) / পরিমাণ(uint32_t);
    বাবে (গুনতি = 0; গুনতি < entries; গুনতি++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->entry[গুনতি];
        acpi_madt_mapped = (acpi_madt_t *)acpi_table_init(acpi_madt, ACPI_RSDT);

        যদি (strncmp(acpi_str_apic, acpi_madt_mapped->header.signature, 4) == 0) {
            printf("ACPI: MADT paddr=%p\n", acpi_madt);
            printf("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            printf("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            printf("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_header = (acpi_madt_header_t *)(acpi_madt_mapped + 1);

            যেতিয়ালৈ ((ৱর্ণ *)acpi_madt_header < (ৱর্ণ *)acpi_madt_mapped + acpi_madt_mapped->header.length) {
                entry_size = MIN(acpi_madt_header->length, MADT_MAX_ENTRY_SIZE);
                memcpy(madt_entry, acpi_madt_header, entry_size);

                ৱিকল্প (acpi_madt_header->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                পক্ষ MADT_APIC: {
                    যদি (!acpi_madt_valid(acpi_madt_header, madt_str_apic, পরিমাণ(acpi_madt_apic_t))) {
                        ভঙ্গ;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_entry;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    যদি (flags == 1) {
                        printf("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        যদি (অঙ্ক_cpu == CONFIG_MAX_অঙ্ক_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_অঙ্ক_NODES);
                        } অন্যথা {
                            cpu_list[অঙ্ক_cpu] = cpu_id;
                            অঙ্ক_cpu++;
                        }
                    }
                    ভঙ্গ;
                }
                পক্ষ MADT_x2APIC: {
                    যদি (!acpi_madt_valid(acpi_madt_header, madt_str_x2apic, পরিমাণ(acpi_madt_x2apic_t))) {
                        ভঙ্গ;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_entry;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    যদি (flags == 1) {
                        printf("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        যদি (অঙ্ক_cpu == CONFIG_MAX_অঙ্ক_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_অঙ্ক_NODES);
                        } অন্যথা {
                            cpu_list[অঙ্ক_cpu] = cpu_id;
                            অঙ্ক_cpu++;
                        }
                    }
                    ভঙ্গ;
                }
                পক্ষ MADT_IOAPIC:
                    যদি (!acpi_madt_valid(acpi_madt_header, madt_str_ioapic, পরিমাণ(acpi_madt_ioapic_t))) {
                        ভঙ্গ;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_entry;

                    printf(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    যদি (*অঙ্ক_ioapic == CONFIG_MAX_অঙ্ক_IOAPIC) {
                        printf("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_অঙ্ক_IOAPIC);
                    } অন্যথা {
                        ioapic_paddrs[*অঙ্ক_ioapic] = madt_ioapic->ioapic_addr;
                        (*অঙ্ক_ioapic)++;
                    }
                    ভঙ্গ;
                পক্ষ MADT_ISO: {
                    যদি (!acpi_madt_valid(acpi_madt_header, madt_str_iso, পরিমাণ(acpi_madt_iso_t))) {
                        ভঙ্গ;
                    }
                    UNUSED acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_entry;

                    printf("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_উপৰেৰ, iso->gsi_নিচেৰ, iso->flags);
                    ভঙ্গ;
                }
                স্ৱভাৱ:
                    ভঙ্গ;
                }
                acpi_madt_header = (acpi_madt_header_t *)((ৱর্ণ *)acpi_madt_header + acpi_madt_header->length);
            }
        }
    }

    printf("ACPI: %d CPU(s) detected\n", অঙ্ক_cpu);

    ঘূৰাই অঙ্ক_cpu;
}

BOOT_CODE bool_t acpi_fadt_scan(
    acpi_rsdp_t *acpi_rsdp
)
{
    unsigned পূর্ণাঙ্ক entries;
    uint32_t            গুনতি;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    প্রতিজ্ঞা(acpi_rsdt_mapped->header.length >= পরিমাণ(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - পরিমাণ(acpi_header_t)) / পরিমাণ(uint32_t);
    বাবে (গুনতি = 0; গুনতি < entries; গুনতি++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->entry[গুনতি];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        যদি (strncmp(acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0) {
            printf("ACPI: FADT paddr=%p\n", acpi_fadt);
            printf("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            printf("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            যদি (config_set(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                printf("system requires apic physical mode\n");
                ঘূৰাই false;
            }
        }
    }

    ঘূৰাই true;
}

BOOT_CODE শূন্য acpi_dmar_scan(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *অঙ্ক_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    unsigned পূর্ণাঙ্ক entries;
    uint32_t গুনতি;
    uint32_t reg_basel, reg_baseh;
    পূর্ণাঙ্ক rmrr_গুনতি;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_header_t   *acpi_dmar_header;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    *অঙ্ক_drhu = 0;
    rmrr_গুনতি = 0;

    প্রতিজ্ঞা(acpi_rsdt_mapped->header.length >= পরিমাণ(acpi_header_t));
    entries = (acpi_rsdt_mapped->header.length - পরিমাণ(acpi_header_t)) / পরিমাণ(uint32_t);
    বাবে (গুনতি = 0; গুনতি < entries; গুনতি++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->entry[গুনতি];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_table_init(acpi_dmar, ACPI_RSDT);

        যদি (strncmp(acpi_str_dmar, acpi_dmar_mapped->header.signature, 4) == 0) {
            printf("ACPI: DMAR paddr=%p\n", acpi_dmar);
            printf("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            printf("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_width + 1);
            acpi_dmar_header = (acpi_dmar_header_t *)(acpi_dmar_mapped + 1);

            যেতিয়ালৈ ((ৱর্ণ *)acpi_dmar_header < (ৱর্ণ *)acpi_dmar_mapped + acpi_dmar_mapped->header.length) {
                ৱিকল্প (acpi_dmar_header->type) {

                পক্ষ DMAR_DRHD:
                    যদি (*অঙ্ক_drhu == max_drhu_list_len) {
                        printf("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *অঙ্ক_drhu = 0; /* report zero IOMMUs */
                        ঘূৰাই;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[1];
                    /* check if value fits into uint32_t */
                    যদি (reg_baseh != 0) {
                        printf("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *অঙ্ক_drhu = 0; /* report zero IOMMUs */
                        ঘূৰাই;
                    }
                    drhu_list[*অঙ্ক_drhu] = (paddr_t)reg_basel;
                    (*অঙ্ক_drhu)++;
                    ভঙ্গ;

                পক্ষ DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_header;
                    যদি (acpi_dmar_rmrr->reg_base[1] != 0 ||
                        acpi_dmar_rmrr->reg_limit[1] != 0) {
                        printf("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *অঙ্ক_drhu = 0;
                        ঘূৰাই ;
                    }

                    বাবে (i = 0; i <= (acpi_dmar_header->length - পরিমাণ(acpi_dmar_rmrr_t)) / পরিমাণ(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        যদি (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *অঙ্ক_drhu = 0; /* report zero IOMMUs */
                            ঘূৰাই;
                        }

                        যদি (acpi_dmar_devscope->length > পরিমাণ(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *অঙ্ক_drhu = 0; /* report zero IOMMUs */
                            ঘূৰাই;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->path_0.dev,
                                acpi_dmar_devscope->path_0.fun
                            );

                        যদি (rmrr_গুনতি == CONFIG_MAX_RMRR_ENTRIES) {
                            printf("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *অঙ্ক_drhu = 0;
                            ঘূৰাই;
                        }
                        printf("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->path_0.dev,
                               acpi_dmar_devscope->path_0.fun
                              );

                        rmrr_list->entries[rmrr_গুনতি].device = dev_id;
                        rmrr_list->entries[rmrr_গুনতি].base = acpi_dmar_rmrr->reg_base[0];
                        rmrr_list->entries[rmrr_গুনতি].limit = acpi_dmar_rmrr->reg_limit[0];
                        rmrr_গুনতি++;
                    }
                    ভঙ্গ;

                পক্ষ DMAR_ATSR:
                    /* not implemented yet */
                    ভঙ্গ;

                স্ৱভাৱ:
                    printf("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_header->type);
                }
                acpi_dmar_header = (acpi_dmar_header_t *)((ৱর্ণ *)acpi_dmar_header + acpi_dmar_header->length);
            }
        }
    }
    rmrr_list->অঙ্ক = rmrr_গুনতি;
    printf("ACPI: %d IOMMUs detected\n", *অঙ্ক_drhu);
}
