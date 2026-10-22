/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>
#ಅನ್ತರ್ಭಾವ <util.h>
#ಅನ್ತರ್ಭಾವ <assert.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <plat/machine.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/acpi.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/devices.h>

ಗಣನಾ acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_dmar {
    acpi_header_t header;
    uint8_t       host_addr_width;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               ಪರಿಮಾಣ(acpi_dmar_t) == ಪರಿಮಾಣ(acpi_header_t) + 12)

/* DMA Remapping Structure Header */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_dmar_header {
    uint16_t type;
    uint16_t length;
} acpi_dmar_header_t;
compile_assert(acpi_dmar_header_packed, ಪರಿಮಾಣ(acpi_dmar_header_t) == 4)

/* DMA Remapping Structure Types */
ಗಣನಾ acpi_table_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_dmar_drhd {
    acpi_dmar_header_t header;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_base[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               ಪರಿಮಾಣ(acpi_dmar_drhd_t) == ಪರಿಮಾಣ(acpi_dmar_header_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  length;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    ಸಂರಚನಾ {
        uint8_t dev;
        uint8_t fun;
    } path_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, ಪರಿಮಾಣ(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_dmar_rmrr {
    acpi_dmar_header_t   header;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_base[2];
    uint32_t             reg_limit[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, ಪರಿಮಾಣ(acpi_dmar_rmrr_t) ==
               ಪರಿಮಾಣ(acpi_dmar_header_t) + 20 + ಪರಿಮಾಣ(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_fadt {
    acpi_header_t  header;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               ಪರಿಮಾಣ(acpi_fadt_t) == ಪರಿಮಾಣ(acpi_header_t) + 80)

/* Multiple APIC Description Table (MADT) */
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt {
    acpi_header_t header;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               ಪರಿಮಾಣ(acpi_madt_t) == ಪರಿಮಾಣ(acpi_header_t) + 8)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt_header {
    uint8_t type;
    uint8_t length;
} acpi_madt_header_t;
compile_assert(acpi_madt_header_packed, ಪರಿಮಾಣ(acpi_madt_header_t) == 2)

ಗಣನಾ acpi_table_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt_apic {
    acpi_madt_header_t header;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               ಪರಿಮಾಣ(acpi_madt_apic_t) == ಪರಿಮಾಣ(acpi_madt_header_t) + 6)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt_x2apic {
    acpi_madt_header_t  header;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               ಪರಿಮಾಣ(acpi_madt_x2apic_t) == ಪರಿಮಾಣ(acpi_madt_header_t) + 14)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt_ioapic {
    acpi_madt_header_t header;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               ಪರಿಮಾಣ(acpi_madt_ioapic_t) == ಪರಿಮಾಣ(acpi_madt_header_t) + 10)

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ acpi_madt_iso {
    acpi_madt_header_t header;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_ಕೆಳಗಿನ;
    uint16_t           gsi_ಮೇಲಿನ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               ಪರಿಮಾಣ(acpi_madt_iso_t) == ಪರಿಮಾಣ(acpi_madt_header_t) + 8)

#define MADT_MAX_ENTRY_SIZE ಪರಿಮಾಣ(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_entry_size, ಪರಿಮಾಣ(acpi_madt_apic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_ioapic_entry_size, ಪರಿಮಾಣ(acpi_madt_ioapic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_iso_entry_size, ಪರಿಮಾಣ(acpi_madt_iso_t) <= MADT_MAX_ENTRY_SIZE)

/* workaround because string literals are not supported by C parser */
ಧ್ರುವ ವರ್ಣ acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ಧ್ರುವ ವರ್ಣ acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ಧ್ರುವ ವರ್ಣ acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ಧ್ರುವ ವರ್ಣ acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ಧ್ರುವ ವರ್ಣ madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ಧ್ರುವ ವರ್ಣ madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ಧ್ರುವ ವರ್ಣ madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ಧ್ರುವ ವರ್ಣ madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_DATA ವರ್ಣ madt_entry[MADT_MAX_ENTRY_SIZE] ALIGN(4);

BOOT_CODE ಸ್ಥಿರ uint8_t acpi_calc_checksum(ವರ್ಣ *start, uint32_t length)
{
    uint8_t checksum = 0;

    ಯಾವತ್ (length > 0) {
        checksum += *start;
        start++;
        length--;
    }
    ಪ್ರತ್ಯಾಗಮ checksum;
}

BOOT_CODE ಸ್ಥಿರ acpi_rsdp_t *acpi_get_rsdp(ಶೂನ್ಯ)
{
    ವರ್ಣ *addr;

    ಪ್ರತಿ (addr = (ವರ್ಣ *)BIOS_PADDR_START; addr < (ವರ್ಣ *)BIOS_PADDR_END; addr += 16) {
        ಯದಿ (strncmp(addr, acpi_str_rsd, 8) == 0) {
            ಯದಿ (acpi_calc_checksum(addr, ACPI_V1_SIZE) == 0) {
                ಪ್ರತ್ಯಾಗಮ (acpi_rsdp_t *)addr;
            }
        }
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

BOOT_CODE ಸ್ಥಿರ ಶೂನ್ಯ *acpi_table_init(ಶೂನ್ಯ *entry, ಗಣನಾ acpi_type table_type)
{
    ಶೂನ್ಯ *acpi_table;
    unsigned ಪೂರ್ಣಾಙ್ಕ pages_for_table;
    unsigned ಪೂರ್ಣಾಙ್ಕ pages_for_header = 1;

    /* if we need to map another page to read header */
    unsigned long offset_in_page = (unsigned long)entry & MASK(LARGE_PAGE_BITS);
    ಯದಿ (MASK(LARGE_PAGE_BITS) - offset_in_page < ಪರಿಮಾಣ(acpi_rsdp_t)) {
        pages_for_header++;
    }

    /* map in table's header */
    acpi_table = map_temp_boot_page(entry, pages_for_header);

    ವಿಕಲ್ಪ (table_type) {
    ಪಕ್ಷ ACPI_RSDP: {
        acpi_rsdp_t *rsdp_entry = (acpi_rsdp_t *)entry;
        pages_for_table = (rsdp_entry->length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        ಭಙ್ಗ;
    }
    ಪಕ್ಷ ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_entry = (acpi_rsdt_t *)entry;
        pages_for_table = (rsdt_entry->header.length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        ಭಙ್ಗ;
    }
    ಸ್ವಭಾವ:
        printf("Error: Mapping unknown ACPI table type\n");
        ಪ್ರತಿಜ್ಞಾ(false);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    /* map in full table */
    acpi_table = map_temp_boot_page(entry, pages_for_table);

    ಪ್ರತ್ಯಾಗಮ acpi_table;
}

BOOT_CODE bool_t acpi_init(acpi_rsdp_t *rsdp_data)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    ಯದಿ (acpi_rsdp == ರಿಕ್ತ) {
        printf("BIOS: No ACPI support detected\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }
    printf("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_table_init(acpi_rsdp, ACPI_RSDP);
    printf("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_data = *acpi_rsdp;

    /* perform final validation */
    ಪ್ರತ್ಯಾಗಮ acpi_validate_rsdp(rsdp_data);
}

BOOT_CODE bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    ಯದಿ (acpi_calc_checksum((ವರ್ಣ *)acpi_rsdp, ACPI_V1_SIZE) != 0) {
        printf("BIOS: ACPIv1 information corrupt\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }

    ಯದಿ (acpi_rsdp->revision > 0 && acpi_calc_checksum((ವರ್ಣ *)acpi_rsdp, ಪರಿಮಾಣ(*acpi_rsdp)) != 0) {
        printf("BIOS: ACPIv2 information corrupt\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address;
    printf("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init(acpi_rsdt, ACPI_RSDT);
    printf("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->header.length > 0);
    ಯದಿ (acpi_calc_checksum((ವರ್ಣ *)acpi_rsdt_mapped, acpi_rsdt_mapped->header.length) != 0) {
        printf("ACPI: RSDT checksum failure\n");
        ಪ್ರತ್ಯಾಗಮ false;
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

BOOT_CODE ಸ್ಥಿರ bool_t acpi_madt_valid(acpi_madt_header_t *header, ಧ್ರುವ ವರ್ಣ *name, uint8_t expected)
{
    ಯದಿ (header->length < expected) {
        printf("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, header->length, expected);
        ಪ್ರತ್ಯಾಗಮ false;
    }
    ಪ್ರತ್ಯಾಗಮ true;
}

BOOT_CODE uint32_t acpi_madt_scan(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *ಅಂಕೆ_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ        entries;
    unsigned ಪೂರ್ಣಾಙ್ಕ        entry_size;
    uint32_t            ಅಂಕೆ_cpu;
    uint32_t            ಎಣಿಕೆ;
    acpi_madt_t        *acpi_madt;
    acpi_madt_header_t *acpi_madt_header;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    ಅಂಕೆ_cpu = 0;
    *ಅಂಕೆ_ioapic = 0;

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->header.length >= ಪರಿಮಾಣ(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - ಪರಿಮಾಣ(acpi_header_t)) / ಪರಿಮಾಣ(uint32_t);
    ಪ್ರತಿ (ಎಣಿಕೆ = 0; ಎಣಿಕೆ < entries; ಎಣಿಕೆ++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->entry[ಎಣಿಕೆ];
        acpi_madt_mapped = (acpi_madt_t *)acpi_table_init(acpi_madt, ACPI_RSDT);

        ಯದಿ (strncmp(acpi_str_apic, acpi_madt_mapped->header.signature, 4) == 0) {
            printf("ACPI: MADT paddr=%p\n", acpi_madt);
            printf("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            printf("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            printf("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_header = (acpi_madt_header_t *)(acpi_madt_mapped + 1);

            ಯಾವತ್ ((ವರ್ಣ *)acpi_madt_header < (ವರ್ಣ *)acpi_madt_mapped + acpi_madt_mapped->header.length) {
                entry_size = MIN(acpi_madt_header->length, MADT_MAX_ENTRY_SIZE);
                memcpy(madt_entry, acpi_madt_header, entry_size);

                ವಿಕಲ್ಪ (acpi_madt_header->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                ಪಕ್ಷ MADT_APIC: {
                    ಯದಿ (!acpi_madt_valid(acpi_madt_header, madt_str_apic, ಪರಿಮಾಣ(acpi_madt_apic_t))) {
                        ಭಙ್ಗ;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_entry;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    ಯದಿ (flags == 1) {
                        printf("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        ಯದಿ (ಅಂಕೆ_cpu == CONFIG_MAX_ಅಂಕೆ_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_NODES);
                        } ಅನ್ಯಥಾ {
                            cpu_list[ಅಂಕೆ_cpu] = cpu_id;
                            ಅಂಕೆ_cpu++;
                        }
                    }
                    ಭಙ್ಗ;
                }
                ಪಕ್ಷ MADT_x2APIC: {
                    ಯದಿ (!acpi_madt_valid(acpi_madt_header, madt_str_x2apic, ಪರಿಮಾಣ(acpi_madt_x2apic_t))) {
                        ಭಙ್ಗ;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_entry;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    ಯದಿ (flags == 1) {
                        printf("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        ಯದಿ (ಅಂಕೆ_cpu == CONFIG_MAX_ಅಂಕೆ_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_NODES);
                        } ಅನ್ಯಥಾ {
                            cpu_list[ಅಂಕೆ_cpu] = cpu_id;
                            ಅಂಕೆ_cpu++;
                        }
                    }
                    ಭಙ್ಗ;
                }
                ಪಕ್ಷ MADT_IOAPIC:
                    ಯದಿ (!acpi_madt_valid(acpi_madt_header, madt_str_ioapic, ಪರಿಮಾಣ(acpi_madt_ioapic_t))) {
                        ಭಙ್ಗ;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_entry;

                    printf(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    ಯದಿ (*ಅಂಕೆ_ioapic == CONFIG_MAX_ಅಂಕೆ_IOAPIC) {
                        printf("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_IOAPIC);
                    } ಅನ್ಯಥಾ {
                        ioapic_paddrs[*ಅಂಕೆ_ioapic] = madt_ioapic->ioapic_addr;
                        (*ಅಂಕೆ_ioapic)++;
                    }
                    ಭಙ್ಗ;
                ಪಕ್ಷ MADT_ISO: {
                    ಯದಿ (!acpi_madt_valid(acpi_madt_header, madt_str_iso, ಪರಿಮಾಣ(acpi_madt_iso_t))) {
                        ಭಙ್ಗ;
                    }
                    UNUSED acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_entry;

                    printf("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_ಮೇಲಿನ, iso->gsi_ಕೆಳಗಿನ, iso->flags);
                    ಭಙ್ಗ;
                }
                ಸ್ವಭಾವ:
                    ಭಙ್ಗ;
                }
                acpi_madt_header = (acpi_madt_header_t *)((ವರ್ಣ *)acpi_madt_header + acpi_madt_header->length);
            }
        }
    }

    printf("ACPI: %d CPU(s) detected\n", ಅಂಕೆ_cpu);

    ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ_cpu;
}

BOOT_CODE bool_t acpi_fadt_scan(
    acpi_rsdp_t *acpi_rsdp
)
{
    unsigned ಪೂರ್ಣಾಙ್ಕ entries;
    uint32_t            ಎಣಿಕೆ;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->header.length >= ಪರಿಮಾಣ(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - ಪರಿಮಾಣ(acpi_header_t)) / ಪರಿಮಾಣ(uint32_t);
    ಪ್ರತಿ (ಎಣಿಕೆ = 0; ಎಣಿಕೆ < entries; ಎಣಿಕೆ++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->entry[ಎಣಿಕೆ];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        ಯದಿ (strncmp(acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0) {
            printf("ACPI: FADT paddr=%p\n", acpi_fadt);
            printf("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            printf("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            ಯದಿ (config_set(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                printf("system requires apic physical mode\n");
                ಪ್ರತ್ಯಾಗಮ false;
            }
        }
    }

    ಪ್ರತ್ಯಾಗಮ true;
}

BOOT_CODE ಶೂನ್ಯ acpi_dmar_scan(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *ಅಂಕೆ_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    unsigned ಪೂರ್ಣಾಙ್ಕ entries;
    uint32_t ಎಣಿಕೆ;
    uint32_t reg_basel, reg_baseh;
    ಪೂರ್ಣಾಙ್ಕ rmrr_ಎಣಿಕೆ;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_header_t   *acpi_dmar_header;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    *ಅಂಕೆ_drhu = 0;
    rmrr_ಎಣಿಕೆ = 0;

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->header.length >= ಪರಿಮಾಣ(acpi_header_t));
    entries = (acpi_rsdt_mapped->header.length - ಪರಿಮಾಣ(acpi_header_t)) / ಪರಿಮಾಣ(uint32_t);
    ಪ್ರತಿ (ಎಣಿಕೆ = 0; ಎಣಿಕೆ < entries; ಎಣಿಕೆ++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->entry[ಎಣಿಕೆ];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_table_init(acpi_dmar, ACPI_RSDT);

        ಯದಿ (strncmp(acpi_str_dmar, acpi_dmar_mapped->header.signature, 4) == 0) {
            printf("ACPI: DMAR paddr=%p\n", acpi_dmar);
            printf("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            printf("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_width + 1);
            acpi_dmar_header = (acpi_dmar_header_t *)(acpi_dmar_mapped + 1);

            ಯಾವತ್ ((ವರ್ಣ *)acpi_dmar_header < (ವರ್ಣ *)acpi_dmar_mapped + acpi_dmar_mapped->header.length) {
                ವಿಕಲ್ಪ (acpi_dmar_header->type) {

                ಪಕ್ಷ DMAR_DRHD:
                    ಯದಿ (*ಅಂಕೆ_drhu == max_drhu_list_len) {
                        printf("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[1];
                    /* check if value fits into uint32_t */
                    ಯದಿ (reg_baseh != 0) {
                        printf("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                        ಪ್ರತ್ಯಾಗಮ;
                    }
                    drhu_list[*ಅಂಕೆ_drhu] = (paddr_t)reg_basel;
                    (*ಅಂಕೆ_drhu)++;
                    ಭಙ್ಗ;

                ಪಕ್ಷ DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_header;
                    ಯದಿ (acpi_dmar_rmrr->reg_base[1] != 0 ||
                        acpi_dmar_rmrr->reg_limit[1] != 0) {
                        printf("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *ಅಂಕೆ_drhu = 0;
                        ಪ್ರತ್ಯಾಗಮ ;
                    }

                    ಪ್ರತಿ (i = 0; i <= (acpi_dmar_header->length - ಪರಿಮಾಣ(acpi_dmar_rmrr_t)) / ಪರಿಮಾಣ(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        ಯದಿ (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                            ಪ್ರತ್ಯಾಗಮ;
                        }

                        ಯದಿ (acpi_dmar_devscope->length > ಪರಿಮಾಣ(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                            ಪ್ರತ್ಯಾಗಮ;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->path_0.dev,
                                acpi_dmar_devscope->path_0.fun
                            );

                        ಯದಿ (rmrr_ಎಣಿಕೆ == CONFIG_MAX_RMRR_ENTRIES) {
                            printf("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0;
                            ಪ್ರತ್ಯಾಗಮ;
                        }
                        printf("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->path_0.dev,
                               acpi_dmar_devscope->path_0.fun
                              );

                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].device = dev_id;
                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].base = acpi_dmar_rmrr->reg_base[0];
                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].limit = acpi_dmar_rmrr->reg_limit[0];
                        rmrr_ಎಣಿಕೆ++;
                    }
                    ಭಙ್ಗ;

                ಪಕ್ಷ DMAR_ATSR:
                    /* not implemented yet */
                    ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                    printf("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_header->type);
                }
                acpi_dmar_header = (acpi_dmar_header_t *)((ವರ್ಣ *)acpi_dmar_header + acpi_dmar_header->length);
            }
        }
    }
    rmrr_list->ಅಂಕೆ = rmrr_ಎಣಿಕೆ;
    printf("ACPI: %d IOMMUs detected\n", *ಅಂಕೆ_drhu);
}
