/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>
#ضمن <util.h>
#ضمن <assert.h>
#ضمن <machine/io.h>
#ضمن <linker.h>
#ضمن <plat/machine.h>
#ضمن <plat/machine/acpi.h>
#ضمن <plat/machine/devices.h>

تعداد acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
تعريفنوع بنية acpi_dmar {
    acpi_header_t header;
    uint8_t       host_addr_width;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               حجم(acpi_dmar_t) == حجم(acpi_header_t) + 12)

/* DMA Remapping Structure Header */
تعريفنوع بنية acpi_dmar_header {
    uint16_t type;
    uint16_t length;
} acpi_dmar_header_t;
compile_assert(acpi_dmar_header_packed, حجم(acpi_dmar_header_t) == 4)

/* DMA Remapping Structure Types */
تعداد acpi_table_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
تعريفنوع بنية acpi_dmar_drhd {
    acpi_dmar_header_t header;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_base[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               حجم(acpi_dmar_drhd_t) == حجم(acpi_dmar_header_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  length;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    بنية {
        uint8_t dev;
        uint8_t fun;
    } path_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, حجم(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_rmrr {
    acpi_dmar_header_t   header;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_base[2];
    uint32_t             reg_limit[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, حجم(acpi_dmar_rmrr_t) ==
               حجم(acpi_dmar_header_t) + 20 + حجم(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
تعريفنوع بنية acpi_fadt {
    acpi_header_t  header;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               حجم(acpi_fadt_t) == حجم(acpi_header_t) + 80)

/* Multiple APIC Description Table (MADT) */
تعريفنوع بنية acpi_madt {
    acpi_header_t header;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               حجم(acpi_madt_t) == حجم(acpi_header_t) + 8)

تعريفنوع بنية acpi_madt_header {
    uint8_t type;
    uint8_t length;
} acpi_madt_header_t;
compile_assert(acpi_madt_header_packed, حجم(acpi_madt_header_t) == 2)

تعداد acpi_table_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

تعريفنوع بنية acpi_madt_apic {
    acpi_madt_header_t header;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               حجم(acpi_madt_apic_t) == حجم(acpi_madt_header_t) + 6)

تعريفنوع بنية acpi_madt_x2apic {
    acpi_madt_header_t  header;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               حجم(acpi_madt_x2apic_t) == حجم(acpi_madt_header_t) + 14)

تعريفنوع بنية acpi_madt_ioapic {
    acpi_madt_header_t header;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               حجم(acpi_madt_ioapic_t) == حجم(acpi_madt_header_t) + 10)

تعريفنوع بنية acpi_madt_iso {
    acpi_madt_header_t header;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_أدنى;
    uint16_t           gsi_أعلى;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               حجم(acpi_madt_iso_t) == حجم(acpi_madt_header_t) + 8)

#define MADT_MAX_ENTRY_SIZE حجم(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_entry_size, حجم(acpi_madt_apic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_ioapic_entry_size, حجم(acpi_madt_ioapic_t) <= MADT_MAX_ENTRY_SIZE)
compile_assert(acpi_madt_iso_entry_size, حجم(acpi_madt_iso_t) <= MADT_MAX_ENTRY_SIZE)

/* workaround because string literals are not supported by C parser */
ثابت حرف acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ثابت حرف acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ثابت حرف acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ثابت حرف acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ثابت حرف madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ثابت حرف madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ثابت حرف madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ثابت حرف madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_DATA حرف madt_entry[MADT_MAX_ENTRY_SIZE] ALIGN(4);

BOOT_CODE ساكن uint8_t acpi_calc_checksum(حرف *start, uint32_t length)
{
    uint8_t checksum = 0;

    بينما (length > 0) {
        checksum += *start;
        start++;
        length--;
    }
    إرجاع checksum;
}

BOOT_CODE ساكن acpi_rsdp_t *acpi_get_rsdp(فراغ)
{
    حرف *addr;

    لكل (addr = (حرف *)BIOS_PADDR_START; addr < (حرف *)BIOS_PADDR_END; addr += 16) {
        إذا (strncmp(addr, acpi_str_rsd, 8) == 0) {
            إذا (acpi_calc_checksum(addr, ACPI_V1_SIZE) == 0) {
                إرجاع (acpi_rsdp_t *)addr;
            }
        }
    }
    إرجاع عدم;
}

BOOT_CODE ساكن فراغ *acpi_table_init(فراغ *entry, تعداد acpi_type table_type)
{
    فراغ *acpi_table;
    unsigned صحيح pages_for_table;
    unsigned صحيح pages_for_header = 1;

    /* if we need to map another page to read header */
    unsigned long offset_in_page = (unsigned long)entry & MASK(LARGE_PAGE_BITS);
    إذا (MASK(LARGE_PAGE_BITS) - offset_in_page < حجم(acpi_rsdp_t)) {
        pages_for_header++;
    }

    /* map in table's header */
    acpi_table = map_temp_boot_page(entry, pages_for_header);

    بدل (table_type) {
    حالة ACPI_RSDP: {
        acpi_rsdp_t *rsdp_entry = (acpi_rsdp_t *)entry;
        pages_for_table = (rsdp_entry->length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        اكسر;
    }
    حالة ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_entry = (acpi_rsdt_t *)entry;
        pages_for_table = (rsdt_entry->header.length + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        اكسر;
    }
    افتراضي:
        printf("Error: Mapping unknown ACPI table type\n");
        أكد(false);
        إرجاع عدم;
    }

    /* map in full table */
    acpi_table = map_temp_boot_page(entry, pages_for_table);

    إرجاع acpi_table;
}

BOOT_CODE bool_t acpi_init(acpi_rsdp_t *rsdp_data)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    إذا (acpi_rsdp == عدم) {
        printf("BIOS: No ACPI support detected\n");
        إرجاع false;
    }
    printf("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_table_init(acpi_rsdp, ACPI_RSDP);
    printf("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_data = *acpi_rsdp;

    /* perform final validation */
    إرجاع acpi_validate_rsdp(rsdp_data);
}

BOOT_CODE bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    إذا (acpi_calc_checksum((حرف *)acpi_rsdp, ACPI_V1_SIZE) != 0) {
        printf("BIOS: ACPIv1 information corrupt\n");
        إرجاع false;
    }

    إذا (acpi_rsdp->revision > 0 && acpi_calc_checksum((حرف *)acpi_rsdp, حجم(*acpi_rsdp)) != 0) {
        printf("BIOS: ACPIv2 information corrupt\n");
        إرجاع false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address;
    printf("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init(acpi_rsdt, ACPI_RSDT);
    printf("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    أكد(acpi_rsdt_mapped->header.length > 0);
    إذا (acpi_calc_checksum((حرف *)acpi_rsdt_mapped, acpi_rsdt_mapped->header.length) != 0) {
        printf("ACPI: RSDT checksum failure\n");
        إرجاع false;
    }

    إرجاع true;
}

BOOT_CODE ساكن bool_t acpi_madt_valid(acpi_madt_header_t *header, ثابت حرف *name, uint8_t expected)
{
    إذا (header->length < expected) {
        printf("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, header->length, expected);
        إرجاع false;
    }
    إرجاع true;
}

BOOT_CODE uint32_t acpi_madt_scan(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *رقم_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    unsigned صحيح        entries;
    unsigned صحيح        entry_size;
    uint32_t            رقم_cpu;
    uint32_t            تعداد_2;
    acpi_madt_t        *acpi_madt;
    acpi_madt_header_t *acpi_madt_header;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    رقم_cpu = 0;
    *رقم_ioapic = 0;

    أكد(acpi_rsdt_mapped->header.length >= حجم(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - حجم(acpi_header_t)) / حجم(uint32_t);
    لكل (تعداد_2 = 0; تعداد_2 < entries; تعداد_2++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->entry[تعداد_2];
        acpi_madt_mapped = (acpi_madt_t *)acpi_table_init(acpi_madt, ACPI_RSDT);

        إذا (strncmp(acpi_str_apic, acpi_madt_mapped->header.signature, 4) == 0) {
            printf("ACPI: MADT paddr=%p\n", acpi_madt);
            printf("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            printf("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            printf("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_header = (acpi_madt_header_t *)(acpi_madt_mapped + 1);

            بينما ((حرف *)acpi_madt_header < (حرف *)acpi_madt_mapped + acpi_madt_mapped->header.length) {
                entry_size = MIN(acpi_madt_header->length, MADT_MAX_ENTRY_SIZE);
                memcpy(madt_entry, acpi_madt_header, entry_size);

                بدل (acpi_madt_header->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                حالة MADT_APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_header, madt_str_apic, حجم(acpi_madt_apic_t))) {
                        اكسر;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_entry;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    إذا (flags == 1) {
                        printf("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        إذا (رقم_cpu == CONFIG_MAX_رقم_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_رقم_NODES);
                        } وإلا {
                            cpu_list[رقم_cpu] = cpu_id;
                            رقم_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_x2APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_header, madt_str_x2apic, حجم(acpi_madt_x2apic_t))) {
                        اكسر;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_entry;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    إذا (flags == 1) {
                        printf("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        إذا (رقم_cpu == CONFIG_MAX_رقم_NODES) {
                            printf("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_رقم_NODES);
                        } وإلا {
                            cpu_list[رقم_cpu] = cpu_id;
                            رقم_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_IOAPIC:
                    إذا (!acpi_madt_valid(acpi_madt_header, madt_str_ioapic, حجم(acpi_madt_ioapic_t))) {
                        اكسر;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_entry;

                    printf(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    إذا (*رقم_ioapic == CONFIG_MAX_رقم_IOAPIC) {
                        printf("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_رقم_IOAPIC);
                    } وإلا {
                        ioapic_paddrs[*رقم_ioapic] = madt_ioapic->ioapic_addr;
                        (*رقم_ioapic)++;
                    }
                    اكسر;
                حالة MADT_ISO: {
                    إذا (!acpi_madt_valid(acpi_madt_header, madt_str_iso, حجم(acpi_madt_iso_t))) {
                        اكسر;
                    }
                    UNUSED acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_entry;

                    printf("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_أعلى, iso->gsi_أدنى, iso->flags);
                    اكسر;
                }
                افتراضي:
                    اكسر;
                }
                acpi_madt_header = (acpi_madt_header_t *)((حرف *)acpi_madt_header + acpi_madt_header->length);
            }
        }
    }

    printf("ACPI: %d CPU(s) detected\n", رقم_cpu);

    إرجاع رقم_cpu;
}

BOOT_CODE bool_t acpi_fadt_scan(
    acpi_rsdp_t *acpi_rsdp
)
{
    unsigned صحيح entries;
    uint32_t            تعداد_2;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    أكد(acpi_rsdt_mapped->header.length >= حجم(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.length - حجم(acpi_header_t)) / حجم(uint32_t);
    لكل (تعداد_2 = 0; تعداد_2 < entries; تعداد_2++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->entry[تعداد_2];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        إذا (strncmp(acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0) {
            printf("ACPI: FADT paddr=%p\n", acpi_fadt);
            printf("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            printf("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            إذا (config_set(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                printf("system requires apic physical mode\n");
                إرجاع false;
            }
        }
    }

    إرجاع true;
}

BOOT_CODE فراغ acpi_dmar_scan(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *رقم_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    unsigned صحيح entries;
    uint32_t تعداد_2;
    uint32_t reg_basel, reg_baseh;
    صحيح rmrr_تعداد;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_header_t   *acpi_dmar_header;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    *رقم_drhu = 0;
    rmrr_تعداد = 0;

    أكد(acpi_rsdt_mapped->header.length >= حجم(acpi_header_t));
    entries = (acpi_rsdt_mapped->header.length - حجم(acpi_header_t)) / حجم(uint32_t);
    لكل (تعداد_2 = 0; تعداد_2 < entries; تعداد_2++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->entry[تعداد_2];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_table_init(acpi_dmar, ACPI_RSDT);

        إذا (strncmp(acpi_str_dmar, acpi_dmar_mapped->header.signature, 4) == 0) {
            printf("ACPI: DMAR paddr=%p\n", acpi_dmar);
            printf("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            printf("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_width + 1);
            acpi_dmar_header = (acpi_dmar_header_t *)(acpi_dmar_mapped + 1);

            بينما ((حرف *)acpi_dmar_header < (حرف *)acpi_dmar_mapped + acpi_dmar_mapped->header.length) {
                بدل (acpi_dmar_header->type) {

                حالة DMAR_DRHD:
                    إذا (*رقم_drhu == max_drhu_list_len) {
                        printf("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *رقم_drhu = 0; /* report zero IOMMUs */
                        إرجاع;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[1];
                    /* check if value fits into uint32_t */
                    إذا (reg_baseh != 0) {
                        printf("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *رقم_drhu = 0; /* report zero IOMMUs */
                        إرجاع;
                    }
                    drhu_list[*رقم_drhu] = (paddr_t)reg_basel;
                    (*رقم_drhu)++;
                    اكسر;

                حالة DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_header;
                    إذا (acpi_dmar_rmrr->reg_base[1] != 0 ||
                        acpi_dmar_rmrr->reg_limit[1] != 0) {
                        printf("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *رقم_drhu = 0;
                        إرجاع ;
                    }

                    لكل (i = 0; i <= (acpi_dmar_header->length - حجم(acpi_dmar_rmrr_t)) / حجم(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        إذا (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *رقم_drhu = 0; /* report zero IOMMUs */
                            إرجاع;
                        }

                        إذا (acpi_dmar_devscope->length > حجم(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            printf("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *رقم_drhu = 0; /* report zero IOMMUs */
                            إرجاع;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->path_0.dev,
                                acpi_dmar_devscope->path_0.fun
                            );

                        إذا (rmrr_تعداد == CONFIG_MAX_RMRR_ENTRIES) {
                            printf("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *رقم_drhu = 0;
                            إرجاع;
                        }
                        printf("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->path_0.dev,
                               acpi_dmar_devscope->path_0.fun
                              );

                        rmrr_list->entries[rmrr_تعداد].device = dev_id;
                        rmrr_list->entries[rmrr_تعداد].base = acpi_dmar_rmrr->reg_base[0];
                        rmrr_list->entries[rmrr_تعداد].limit = acpi_dmar_rmrr->reg_limit[0];
                        rmrr_تعداد++;
                    }
                    اكسر;

                حالة DMAR_ATSR:
                    /* not implemented yet */
                    اكسر;

                افتراضي:
                    printf("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_header->type);
                }
                acpi_dmar_header = (acpi_dmar_header_t *)((حرف *)acpi_dmar_header + acpi_dmar_header->length);
            }
        }
    }
    rmrr_list->رقم = rmrr_تعداد;
    printf("ACPI: %d IOMMUs detected\n", *رقم_drhu);
}
