/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#അന്തര്ഭാവ <config.h>
#അന്തര്ഭാവ <util.h>
#അന്തര്ഭാവ <assert.h>
#അന്തര്ഭാവ <machine/io.h>
#അന്തര്ഭാവ <linker.h>
#അന്തര്ഭാവ <plat/machine.h>
#അന്തര്ഭാവ <plat/machine/acpi.h>
#അന്തര്ഭാവ <plat/machine/devices.h>

എണ്ണൽ acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
തരനിർവചനം ഘടന acpi_dmar {
    acpi_header_t header;
    uint8_t       host_addr_width;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               വലുപ്പം(acpi_dmar_t) == വലുപ്പം(acpi_header_t) + 12)

/* DMA Remapping Structure Header */
തരനിർവചനം ഘടന acpi_dmar_header {
    uint16_t type;
    uint16_t length;
} acpi_dmar_header_t;
compile_assert(acpi_dmar_header_packed, വലുപ്പം(acpi_dmar_header_t) == 4)

/* DMA Remapping Structure Types */
എണ്ണൽ acpi_table_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
തരനിർവചനം ഘടന acpi_dmar_drhd {
    acpi_dmar_header_t header;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_base[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               വലുപ്പം(acpi_dmar_drhd_t) == വലുപ്പം(acpi_dmar_header_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
തരനിർവചനം ഘടന acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  length;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    ഘടന {
        uint8_t dev;
        uint8_t fun;
    } path_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, വലുപ്പം(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
തരനിർവചനം ഘടന acpi_dmar_rmrr {
    acpi_dmar_header_t   header;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_base[2];
    uint32_t             reg_limit[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, വലുപ്പം(acpi_dmar_rmrr_t) ==
               വലുപ്പം(acpi_dmar_header_t) + 20 + വലുപ്പം(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
തരനിർവചനം ഘടന acpi_fadt {
    acpi_header_t  header;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               വലുപ്പം(acpi_fadt_t) == വലുപ്പം(acpi_header_t) + 80)

/* Multiple APIC Description Table (MADT) */
തരനിർവചനം ഘടന acpi_madt {
    acpi_header_t header;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               വലുപ്പം(acpi_madt_t) == വലുപ്പം(acpi_header_t) + 8)

തരനിർവചനം ഘടന acpi_madt_header {
    uint8_t type;
    uint8_t length;
} acpi_madt_header_t;
compile_assert(acpi_madt_header_packed, വലുപ്പം(acpi_madt_header_t) == 2)

എണ്ണൽ acpi_table_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

തരനിർവചനം ഘടന acpi_madt_apic {
    acpi_madt_header_t header;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               വലുപ്പം(acpi_madt_apic_t) == വലുപ്പം(acpi_madt_header_t) + 6)

തരനിർവചനം ഘടന acpi_madt_x2apic {
    acpi_madt_header_t  header;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               വലുപ്പം(acpi_madt_x2apic_t) == വലുപ്പം(acpi_madt_header_t) + 14)

തരനിർവചനം ഘടന acpi_madt_ioapic {
    acpi_madt_header_t header;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               വലുപ്പം(acpi_madt_ioapic_t) == വലുപ്പം(acpi_madt_header_t) + 10)

തരനിർവചനം ഘടന acpi_madt_iso {
    acpi_madt_header_t header;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_താഴെയുള്ള;
    uint16_t           gsi_മുകളിലെ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               വലുപ്പം(acpi_madt_iso_t) == വലുപ്പം(acpi_madt_header_t) + 8)

#define MADT_MAX_ENTRY_SIZE വലുപ്പം(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_entry_size, വലുപ്പം(acpi_madt_apic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_ioapic_entry_size, വലുപ്പം(acpi_madt_ioapic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_iso_entry_size, വലുപ്പം(acpi_madt_iso_t) <= MADT_MAX_ENTRY_SIZE)

/* workaround because string literals are not supported by C parser */
സ്ഥിരം അക്ഷരം acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
സ്ഥിരം അക്ഷരം acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
സ്ഥിരം അക്ഷരം acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
സ്ഥിരം അക്ഷരം acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
സ്ഥിരം അക്ഷരം madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
സ്ഥിരം അക്ഷരം madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
സ്ഥിരം അക്ഷരം madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
സ്ഥിരം അക്ഷരം madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_DATA അക്ഷരം madt_entry[MADT_MAX_ENTRY_SIZE] ALIGN(4);

BOOT_CODE സ്ഥിതം uint8_t acpi_calc_checksum(അക്ഷരം *start, uint32_t length)
{
    uint8_t checksum = 0;

    ആകുമ്പോൾ (length > 0) {
        checksum += *start;
        start++;
        length--;
    }
    തിരികെ checksum;
}

BOOT_CODE സ്ഥിതം acpi_rsdp_t *acpi_get_rsdp(ശൂന്യം)
{
    അക്ഷരം *addr;

    വേണ്ടി (addr = (അക്ഷരം *)BIOS_PADDR_START; addr < (അക്ഷരം *)BIOS_PADDR_END; addr += 16) {
        എങ്കിൽ (strncmp(addr, acpi_str_rsd, 8) == 0) {
            എങ്കിൽ (acpi_calc_checksum(addr, ACPI_V1_SIZE) == 0) {
                തിരികെ (acpi_rsdp_t *)addr;
            }
        }
    }
    തിരികെ ശൂന്യ;
}

BOOT_CODE സ്ഥിതം ശൂന്യം *acpi_table_init(ശൂന്യം *entry, എണ്ണൽ acpi_type table_type)
{
    ശൂന്യം *acpi_table;
    unsigned പൂർണ്ണസംഖ്യ pages_for_table;
    unsigned പൂർണ്ണസംഖ്യ pages_for_header = 1;

    /* if we need to map another page to read header */
    unsigned long offset_in_page = (unsigned long)entry & MASK(LARGE_PAGE_BITS);
    എങ്കിൽ (MASK(LARGE_PAGE_BITS) - offset_in_page < വലുപ്പം(acpi_rsdp_t)) {
        pages_for_header++;
    }

    /* map in table's header */
    acpi_table = map_temp_boot_page(entry, pages_for_header);

    തിരഞ്ഞെടുപ്പ് (table_type) {
    അവസ്ഥ ACPI_RSDP: {
        acpi_rsdp_t *rsdp_entry = (acpi_rsdp_t *)entry;
        pages_for_table = (rsdp_entry->length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        നിർത്തുക;
    }
    അവസ്ഥ ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_entry = (acpi_rsdt_t *)entry;
        pages_for_table = (rsdt_entry->header.length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        നിർത്തുക;
    }
    സ്ഥിരസ്ഥിതി:
        printf("Error: Mapping unknown ACPI table type\n");
        പ്രതിജ്ഞാ(false);
        തിരികെ ശൂന്യ;
    }

    /* map in full table */
    acpi_table = map_temp_boot_page(entry, pages_for_table);

    തിരികെ acpi_table;
}

BOOT_CODE bool_t acpi_init(acpi_rsdp_t *rsdp_data)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    എങ്കിൽ (acpi_rsdp == ശൂന്യ) {
        printf("BIOS: No ACPI support detected\n");
        തിരികെ false;
    }
    printf("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_table_init(acpi_rsdp, ACPI_RSDP);
    printf("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_data = *acpi_rsdp;

    /* perform final validation */
    തിരികെ acpi_validate_rsdp(rsdp_data);
}

BOOT_CODE bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    എങ്കിൽ (acpi_calc_checksum((അക്ഷരം *)acpi_rsdp, ACPI_V1_SIZE) != 0) {
        printf("BIOS: ACPIv1 information corrupt\n");
        തിരികെ false;
    }

    എങ്കിൽ (acpi_rsdp->revision > 0 && acpi_calc_checksum((അക്ഷരം *)acpi_rsdp, വലുപ്പം(*acpi_rsdp)) != 0) {
        printf("BIOS: ACPIv2 information corrupt\n");
        തിരികെ false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address;
    printf("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init(acpi_rsdt, ACPI_RSDT);
    printf("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    പ്രതിജ്ഞാ(acpi_rsdt_mapped->header.length > 0);
    എങ്കിൽ (acpi_calc_checksum((അക്ഷരം *)acpi_rsdt_mapped, acpi_rsdt_mapped->header.length) != 0) {
        printf("ACPI: RSDT checksum failure\n");
        തിരികെ false;
    }

    തിരികെ true;
}

BOOT_CODE സ്ഥിതം bool_t acpi_madt_valid(acpi_madt_header_t *header, സ്ഥിരം അക്ഷരം *name, uint8_t expected)
{
    എങ്കിൽ (header->length < expected) {
        printf("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, header->length, expected);
        തിരികെ false;
    }
    തിരികെ true;
}

BOOT_CODE uint32_t acpi_madt_scan(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *അക്കം_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    unsigned പൂർണ്ണസംഖ്യ        entries;
    unsigned പൂർണ്ണസംഖ്യ        entry_size;
    uint32_t            അക്കം_cpu;
    uint32_t            എണ്ണം;
    acpi_madt_t        *acpi_madt;
    acpi_madt_header_t *acpi_madt_header;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    അക്കം_cpu = 0;
    *അക്കം_ioapic = 0;

    പ്രതിജ്ഞാ(acpi_rsdt_mapped->header.length >= വലുപ്പം(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - വലുപ്പം(acpi_header_t)) / വലുപ്പം(uint32_t);
    വേണ്ടി (എണ്ണം = 0; എണ്ണം < entries; എണ്ണം++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->entry[എണ്ണം];
        acpi_madt_mapped = (acpi_madt_t *)acpi_table_init(acpi_madt, ACPI_RSDT);

        എങ്കിൽ (strncmp(acpi_str_apic, acpi_madt_mapped->header.signature, 4) == 0) {
            printf("ACPI: MADT paddr=%p\n", acpi_madt);
            printf("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            printf("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            printf("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_header = (acpi_madt_header_t *)(acpi_madt_mapped + 1);

            ആകുമ്പോൾ ((അക്ഷരം *)acpi_madt_header < (അക്ഷരം *)acpi_madt_mapped + acpi_madt_mapped->header.length) {
                entry_size = MIN(acpi_madt_header->length, MADT_MAX_ENTRY_SIZE);
                memcpy(madt_entry, acpi_madt_header, entry_size);

                തിരഞ്ഞെടുപ്പ് (acpi_madt_header->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                അവസ്ഥ MADT_APIC: {
                    എങ്കിൽ (!acpi_madt_valid(acpi_madt_header, madt_str_apic, വലുപ്പം(acpi_madt_apic_t))) {
                        നിർത്തുക;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_entry;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    എങ്കിൽ (flags == 1) {
                        printf("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        എങ്കിൽ (അക്കം_cpu == CONFIG_MAX_അക്കം_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_അക്കം_NODES);
                        } അല്ലെങ്കിൽ {
                            cpu_list[അക്കം_cpu] = cpu_id;
                            അക്കം_cpu++;
                        }
                    }
                    നിർത്തുക;
                }
                അവസ്ഥ MADT_x2APIC: {
                    എങ്കിൽ (!acpi_madt_valid(acpi_madt_header, madt_str_x2apic, വലുപ്പം(acpi_madt_x2apic_t))) {
                        നിർത്തുക;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_entry;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    എങ്കിൽ (flags == 1) {
                        printf("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        എങ്കിൽ (അക്കം_cpu == CONFIG_MAX_അക്കം_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_അക്കം_NODES);
                        } അല്ലെങ്കിൽ {
                            cpu_list[അക്കം_cpu] = cpu_id;
                            അക്കം_cpu++;
                        }
                    }
                    നിർത്തുക;
                }
                അവസ്ഥ MADT_IOAPIC:
                    എങ്കിൽ (!acpi_madt_valid(acpi_madt_header, madt_str_ioapic, വലുപ്പം(acpi_madt_ioapic_t))) {
                        നിർത്തുക;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_entry;

                    printf(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    എങ്കിൽ (*അക്കം_ioapic == CONFIG_MAX_അക്കം_IOAPIC) {
                        printf("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_അക്കം_IOAPIC);
                    } അല്ലെങ്കിൽ {
                        ioapic_paddrs[*അക്കം_ioapic] = madt_ioapic->ioapic_addr;
                        (*അക്കം_ioapic)++;
                    }
                    നിർത്തുക;
                അവസ്ഥ MADT_ISO: {
                    എങ്കിൽ (!acpi_madt_valid(acpi_madt_header, madt_str_iso, വലുപ്പം(acpi_madt_iso_t))) {
                        നിർത്തുക;
                    }
                    UNUSED acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_entry;

                    printf("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_മുകളിലെ, iso->gsi_താഴെയുള്ള, iso->flags);
                    നിർത്തുക;
                }
                സ്ഥിരസ്ഥിതി:
                    നിർത്തുക;
                }
                acpi_madt_header = (acpi_madt_header_t *)((അക്ഷരം *)acpi_madt_header + acpi_madt_header->length);
            }
        }
    }

    printf("ACPI: %d CPU(s) detected\n", അക്കം_cpu);

    തിരികെ അക്കം_cpu;
}

BOOT_CODE bool_t acpi_fadt_scan(
    acpi_rsdp_t *acpi_rsdp
)
{
    unsigned പൂർണ്ണസംഖ്യ entries;
    uint32_t            എണ്ണം;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    പ്രതിജ്ഞാ(acpi_rsdt_mapped->header.length >= വലുപ്പം(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - വലുപ്പം(acpi_header_t)) / വലുപ്പം(uint32_t);
    വേണ്ടി (എണ്ണം = 0; എണ്ണം < entries; എണ്ണം++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->entry[എണ്ണം];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        എങ്കിൽ (strncmp(acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0) {
            printf("ACPI: FADT paddr=%p\n", acpi_fadt);
            printf("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            printf("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            എങ്കിൽ (config_set(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                printf("system requires apic physical mode\n");
                തിരികെ false;
            }
        }
    }

    തിരികെ true;
}

BOOT_CODE ശൂന്യം acpi_dmar_scan(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *അക്കം_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    unsigned പൂർണ്ണസംഖ്യ entries;
    uint32_t എണ്ണം;
    uint32_t reg_basel, reg_baseh;
    പൂർണ്ണസംഖ്യ rmrr_എണ്ണം;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_header_t   *acpi_dmar_header;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    *അക്കം_drhu = 0;
    rmrr_എണ്ണം = 0;

    പ്രതിജ്ഞാ(acpi_rsdt_mapped->header.length >= വലുപ്പം(acpi_header_t));
    entries = (acpi_rsdt_mapped->header.length - വലുപ്പം(acpi_header_t)) / വലുപ്പം(uint32_t);
    വേണ്ടി (എണ്ണം = 0; എണ്ണം < entries; എണ്ണം++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->entry[എണ്ണം];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_table_init(acpi_dmar, ACPI_RSDT);

        എങ്കിൽ (strncmp(acpi_str_dmar, acpi_dmar_mapped->header.signature, 4) == 0) {
            printf("ACPI: DMAR paddr=%p\n", acpi_dmar);
            printf("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            printf("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_width + 1);
            acpi_dmar_header = (acpi_dmar_header_t *)(acpi_dmar_mapped + 1);

            ആകുമ്പോൾ ((അക്ഷരം *)acpi_dmar_header < (അക്ഷരം *)acpi_dmar_mapped + acpi_dmar_mapped->header.length) {
                തിരഞ്ഞെടുപ്പ് (acpi_dmar_header->type) {

                അവസ്ഥ DMAR_DRHD:
                    എങ്കിൽ (*അക്കം_drhu == max_drhu_list_len) {
                        printf("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *അക്കം_drhu = 0; /* report zero IOMMUs */
                        തിരികെ;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[1];
                    /* check if value fits into uint32_t */
                    എങ്കിൽ (reg_baseh != 0) {
                        printf("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *അക്കം_drhu = 0; /* report zero IOMMUs */
                        തിരികെ;
                    }
                    drhu_list[*അക്കം_drhu] = (paddr_t)reg_basel;
                    (*അക്കം_drhu)++;
                    നിർത്തുക;

                അവസ്ഥ DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_header;
                    എങ്കിൽ (acpi_dmar_rmrr->reg_base[1] != 0 ||
                        acpi_dmar_rmrr->reg_limit[1] != 0) {
                        printf("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *അക്കം_drhu = 0;
                        തിരികെ ;
                    }

                    വേണ്ടി (i = 0; i <= (acpi_dmar_header->length - വലുപ്പം(acpi_dmar_rmrr_t)) / വലുപ്പം(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        എങ്കിൽ (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *അക്കം_drhu = 0; /* report zero IOMMUs */
                            തിരികെ;
                        }

                        എങ്കിൽ (acpi_dmar_devscope->length > വലുപ്പം(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *അക്കം_drhu = 0; /* report zero IOMMUs */
                            തിരികെ;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->path_0.dev,
                                acpi_dmar_devscope->path_0.fun
                            );

                        എങ്കിൽ (rmrr_എണ്ണം == CONFIG_MAX_RMRR_ENTRIES) {
                            printf("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *അക്കം_drhu = 0;
                            തിരികെ;
                        }
                        printf("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->path_0.dev,
                               acpi_dmar_devscope->path_0.fun
                              );

                        rmrr_list->entries[rmrr_എണ്ണം].device = dev_id;
                        rmrr_list->entries[rmrr_എണ്ണം].base = acpi_dmar_rmrr->reg_base[0];
                        rmrr_list->entries[rmrr_എണ്ണം].limit = acpi_dmar_rmrr->reg_limit[0];
                        rmrr_എണ്ണം++;
                    }
                    നിർത്തുക;

                അവസ്ഥ DMAR_ATSR:
                    /* not implemented yet */
                    നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                    printf("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_header->type);
                }
                acpi_dmar_header = (acpi_dmar_header_t *)((അക്ഷരം *)acpi_dmar_header + acpi_dmar_header->length);
            }
        }
    }
    rmrr_list->അക്കം = rmrr_എണ്ണം;
    printf("ACPI: %d IOMMUs detected\n", *അക്കം_drhu);
}
