/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>
#समावेश <util.h>
#समावेश <assert.h>
#समावेश <machine/io.h>
#समावेश <linker.h>
#समावेश <plat/machine.h>
#समावेश <plat/machine/acpi.h>
#समावेश <plat/machine/devices.h>

क्रमागत acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
प्रकार काष्ठा acpi_dmar {
    acpi_header_t header;
    uint8_t       host_addr_width;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               माप(acpi_dmar_t) == माप(acpi_header_t) + 12)

/* DMA Remapping Structure Header */
प्रकार काष्ठा acpi_dmar_header {
    uint16_t type;
    uint16_t लंबाई;
} acpi_dmar_header_t;
compile_assert(acpi_dmar_header_packed, माप(acpi_dmar_header_t) == 4)

/* DMA Remapping Structure Types */
क्रमागत acpi_table_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
प्रकार काष्ठा acpi_dmar_drhd {
    acpi_dmar_header_t header;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_base[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               माप(acpi_dmar_drhd_t) == माप(acpi_dmar_header_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
प्रकार काष्ठा acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  लंबाई;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  start_bus;
    काष्ठा {
        uint8_t dev;
        uint8_t fun;
    } path_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, माप(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
प्रकार काष्ठा acpi_dmar_rmrr {
    acpi_dmar_header_t   header;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_base[2];
    uint32_t             reg_limit[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, माप(acpi_dmar_rmrr_t) ==
               माप(acpi_dmar_header_t) + 20 + माप(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
प्रकार काष्ठा acpi_fadt {
    acpi_header_t  header;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               माप(acpi_fadt_t) == माप(acpi_header_t) + 80)

/* Multiple APIC Description Table (MADT) */
प्रकार काष्ठा acpi_madt {
    acpi_header_t header;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               माप(acpi_madt_t) == माप(acpi_header_t) + 8)

प्रकार काष्ठा acpi_madt_header {
    uint8_t type;
    uint8_t लंबाई;
} acpi_madt_header_t;
compile_assert(acpi_madt_header_packed, माप(acpi_madt_header_t) == 2)

क्रमागत acpi_table_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

प्रकार काष्ठा acpi_madt_apic {
    acpi_madt_header_t header;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               माप(acpi_madt_apic_t) == माप(acpi_madt_header_t) + 6)

प्रकार काष्ठा acpi_madt_x2apic {
    acpi_madt_header_t  header;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               माप(acpi_madt_x2apic_t) == माप(acpi_madt_header_t) + 14)

प्रकार काष्ठा acpi_madt_ioapic {
    acpi_madt_header_t header;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               माप(acpi_madt_ioapic_t) == माप(acpi_madt_header_t) + 10)

प्रकार काष्ठा acpi_madt_iso {
    acpi_madt_header_t header;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            source;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_निचला;
    uint16_t           gsi_ऊपरी;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               माप(acpi_madt_iso_t) == माप(acpi_madt_header_t) + 8)

#घोषणा MADT_MAX_ENTRY_आकार माप(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_entry_आकार, माप(acpi_madt_apic_t) <= MADT_MAX_ENTRY_आकार)
compile_assert(acpi_madt_ioapic_entry_आकार, माप(acpi_madt_ioapic_t) <= MADT_MAX_ENTRY_आकार)
compile_assert(acpi_madt_iso_entry_आकार, माप(acpi_madt_iso_t) <= MADT_MAX_ENTRY_आकार)

/* workaround because string literals are not supported by C parser */
स्थिर अक्षर acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
स्थिर अक्षर acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
स्थिर अक्षर acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
स्थिर अक्षर acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
स्थिर अक्षर madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
स्थिर अक्षर madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
स्थिर अक्षर madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
स्थिर अक्षर madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_तथ्य अक्षर madt_entry[MADT_MAX_ENTRY_आकार] ALIGN(4);

BOOT_CODE अटल uint8_t acpi_calc_checksum(अक्षर *start, uint32_t लंबाई)
{
    uint8_t checksum = 0;

    जबतक (लंबाई > 0) {
        checksum += *start;
        start++;
        लंबाई--;
    }
    वापस checksum;
}

BOOT_CODE अटल acpi_rsdp_t *acpi_get_rsdp(व्योम)
{
    अक्षर *addr;

    क्रम (addr = (अक्षर *)BIOS_PADDR_START; addr < (अक्षर *)BIOS_PADDR_END; addr += 16) {
        अगर (म_भेदन(addr, acpi_str_rsd, 8) == 0) {
            अगर (acpi_calc_checksum(addr, ACPI_V1_आकार) == 0) {
                वापस (acpi_rsdp_t *)addr;
            }
        }
    }
    वापस शून्य;
}

BOOT_CODE अटल व्योम *acpi_table_init(व्योम *entry, क्रमागत acpi_type table_type)
{
    व्योम *acpi_table;
    अचिन्हित पूर्णांक pages_for_table;
    अचिन्हित पूर्णांक pages_for_header = 1;

    /* if we need to map another page to read header */
    अचिन्हित दीर्घ offset_in_page = (अचिन्हित दीर्घ)entry & MASK(LARGE_PAGE_BITS);
    अगर (MASK(LARGE_PAGE_BITS) - offset_in_page < माप(acpi_rsdp_t)) {
        pages_for_header++;
    }

    /* map in table's header */
    acpi_table = map_अस्थायी_boot_page(entry, pages_for_header);

    चयन (table_type) {
    हाल ACPI_RSDP: {
        acpi_rsdp_t *rsdp_entry = (acpi_rsdp_t *)entry;
        pages_for_table = (rsdp_entry->लंबाई + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        अवरोध;
    }
    हाल ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_entry = (acpi_rsdt_t *)entry;
        pages_for_table = (rsdt_entry->header.लंबाई + offset_in_page) / MASK(LARGE_PAGE_BITS) + 1;
        अवरोध;
    }
    शेष:
        म_लिखो("Error: Mapping unknown ACPI table type\n");
        निश्चित(false);
        वापस शून्य;
    }

    /* map in full table */
    acpi_table = map_अस्थायी_boot_page(entry, pages_for_table);

    वापस acpi_table;
}

BOOT_CODE bool_t acpi_init(acpi_rsdp_t *rsdp_तथ्य)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    अगर (acpi_rsdp == शून्य) {
        म_लिखो("BIOS: No ACPI support detected\n");
        वापस false;
    }
    म_लिखो("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_table_init(acpi_rsdp, ACPI_RSDP);
    म_लिखो("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_तथ्य = *acpi_rsdp;

    /* perform final validation */
    वापस acpi_validate_rsdp(rsdp_तथ्य);
}

BOOT_CODE bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    अगर (acpi_calc_checksum((अक्षर *)acpi_rsdp, ACPI_V1_आकार) != 0) {
        म_लिखो("BIOS: ACPIv1 information corrupt\n");
        वापस false;
    }

    अगर (acpi_rsdp->revision > 0 && acpi_calc_checksum((अक्षर *)acpi_rsdp, माप(*acpi_rsdp)) != 0) {
        म_लिखो("BIOS: ACPIv2 information corrupt\n");
        वापस false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address;
    म_लिखो("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init(acpi_rsdt, ACPI_RSDT);
    म_लिखो("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    निश्चित(acpi_rsdt_mapped->header.लंबाई > 0);
    अगर (acpi_calc_checksum((अक्षर *)acpi_rsdt_mapped, acpi_rsdt_mapped->header.लंबाई) != 0) {
        म_लिखो("ACPI: RSDT checksum failure\n");
        वापस false;
    }

    वापस true;
}

BOOT_CODE अटल bool_t acpi_madt_valid(acpi_madt_header_t *header, स्थिर अक्षर *name, uint8_t expected)
{
    अगर (header->लंबाई < expected) {
        म_लिखो("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, header->लंबाई, expected);
        वापस false;
    }
    वापस true;
}

BOOT_CODE uint32_t acpi_madt_scan(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *अंक_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    अचिन्हित पूर्णांक        entries;
    अचिन्हित पूर्णांक        entry_आकार;
    uint32_t            अंक_cpu;
    uint32_t            गिनती;
    acpi_madt_t        *acpi_madt;
    acpi_madt_header_t *acpi_madt_header;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    अंक_cpu = 0;
    *अंक_ioapic = 0;

    निश्चित(acpi_rsdt_mapped->header.लंबाई >= माप(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.लंबाई - माप(acpi_header_t)) / माप(uint32_t);
    क्रम (गिनती = 0; गिनती < entries; गिनती++) {
        acpi_madt = (acpi_madt_t *)(word_t)acpi_rsdt_mapped->entry[गिनती];
        acpi_madt_mapped = (acpi_madt_t *)acpi_table_init(acpi_madt, ACPI_RSDT);

        अगर (म_भेदन(acpi_str_apic, acpi_madt_mapped->header.signature, 4) == 0) {
            म_लिखो("ACPI: MADT paddr=%p\n", acpi_madt);
            म_लिखो("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            म_लिखो("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            म_लिखो("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_header = (acpi_madt_header_t *)(acpi_madt_mapped + 1);

            जबतक ((अक्षर *)acpi_madt_header < (अक्षर *)acpi_madt_mapped + acpi_madt_mapped->header.लंबाई) {
                entry_आकार = MIN(acpi_madt_header->लंबाई, MADT_MAX_ENTRY_आकार);
                स_नकल(madt_entry, acpi_madt_header, entry_आकार);

                चयन (acpi_madt_header->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                हाल MADT_APIC: {
                    अगर (!acpi_madt_valid(acpi_madt_header, madt_str_apic, माप(acpi_madt_apic_t))) {
                        अवरोध;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_entry;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    अगर (flags == 1) {
                        म_लिखो("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        अगर (अंक_cpu == CONFIG_MAX_अंक_NODES) {
                            म_लिखो("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_अंक_NODES);
                        } अन्यथा {
                            cpu_list[अंक_cpu] = cpu_id;
                            अंक_cpu++;
                        }
                    }
                    अवरोध;
                }
                हाल MADT_x2APIC: {
                    अगर (!acpi_madt_valid(acpi_madt_header, madt_str_x2apic, माप(acpi_madt_x2apic_t))) {
                        अवरोध;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_entry;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    अगर (flags == 1) {
                        म_लिखो("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        अगर (अंक_cpu == CONFIG_MAX_अंक_NODES) {
                            म_लिखो("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_अंक_NODES);
                        } अन्यथा {
                            cpu_list[अंक_cpu] = cpu_id;
                            अंक_cpu++;
                        }
                    }
                    अवरोध;
                }
                हाल MADT_IOAPIC:
                    अगर (!acpi_madt_valid(acpi_madt_header, madt_str_ioapic, माप(acpi_madt_ioapic_t))) {
                        अवरोध;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_entry;

                    म_लिखो(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    अगर (*अंक_ioapic == CONFIG_MAX_अंक_IOAPIC) {
                        म_लिखो("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_अंक_IOAPIC);
                    } अन्यथा {
                        ioapic_paddrs[*अंक_ioapic] = madt_ioapic->ioapic_addr;
                        (*अंक_ioapic)++;
                    }
                    अवरोध;
                हाल MADT_ISO: {
                    अगर (!acpi_madt_valid(acpi_madt_header, madt_str_iso, माप(acpi_madt_iso_t))) {
                        अवरोध;
                    }
                    UNUSED acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_entry;

                    म_लिखो("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->source, iso->gsi_ऊपरी, iso->gsi_निचला, iso->flags);
                    अवरोध;
                }
                शेष:
                    अवरोध;
                }
                acpi_madt_header = (acpi_madt_header_t *)((अक्षर *)acpi_madt_header + acpi_madt_header->लंबाई);
            }
        }
    }

    म_लिखो("ACPI: %d CPU(s) detected\n", अंक_cpu);

    वापस अंक_cpu;
}

BOOT_CODE bool_t acpi_fadt_scan(
    acpi_rsdp_t *acpi_rsdp
)
{
    अचिन्हित पूर्णांक entries;
    uint32_t            गिनती;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    निश्चित(acpi_rsdt_mapped->header.लंबाई >= माप(acpi_header_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->header.लंबाई - माप(acpi_header_t)) / माप(uint32_t);
    क्रम (गिनती = 0; गिनती < entries; गिनती++) {
        acpi_fadt = (acpi_fadt_t *)(word_t)acpi_rsdt_mapped->entry[गिनती];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_table_init(acpi_fadt, ACPI_RSDT);

        अगर (म_भेदन(acpi_str_fadt, acpi_fadt_mapped->header.signature, 4) == 0) {
            म_लिखो("ACPI: FADT paddr=%p\n", acpi_fadt);
            म_लिखो("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            म_लिखो("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            अगर (config_set(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & BIT(19)) {
                म_लिखो("system requires apic physical mode\n");
                वापस false;
            }
        }
    }

    वापस true;
}

BOOT_CODE व्योम acpi_dmar_scan(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *अंक_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    word_t i;
    अचिन्हित पूर्णांक entries;
    uint32_t गिनती;
    uint32_t reg_basel, reg_baseh;
    पूर्णांक rmrr_गिनती;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_header_t   *acpi_dmar_header;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_table_init((acpi_rsdt_t *)(word_t)acpi_rsdp->rsdt_address, ACPI_RSDT);

    *अंक_drhu = 0;
    rmrr_गिनती = 0;

    निश्चित(acpi_rsdt_mapped->header.लंबाई >= माप(acpi_header_t));
    entries = (acpi_rsdt_mapped->header.लंबाई - माप(acpi_header_t)) / माप(uint32_t);
    क्रम (गिनती = 0; गिनती < entries; गिनती++) {
        acpi_dmar = (acpi_dmar_t *)(word_t)acpi_rsdt_mapped->entry[गिनती];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_table_init(acpi_dmar, ACPI_RSDT);

        अगर (म_भेदन(acpi_str_dmar, acpi_dmar_mapped->header.signature, 4) == 0) {
            म_लिखो("ACPI: DMAR paddr=%p\n", acpi_dmar);
            म_लिखो("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            म_लिखो("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_width + 1);
            acpi_dmar_header = (acpi_dmar_header_t *)(acpi_dmar_mapped + 1);

            जबतक ((अक्षर *)acpi_dmar_header < (अक्षर *)acpi_dmar_mapped + acpi_dmar_mapped->header.लंबाई) {
                चयन (acpi_dmar_header->type) {

                हाल DMAR_DRHD:
                    अगर (*अंक_drhu == max_drhu_list_len) {
                        म_लिखो("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *अंक_drhu = 0; /* report zero IOMMUs */
                        वापस;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_header)->reg_base[1];
                    /* check if value fits into uint32_t */
                    अगर (reg_baseh != 0) {
                        म_लिखो("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *अंक_drhu = 0; /* report zero IOMMUs */
                        वापस;
                    }
                    drhu_list[*अंक_drhu] = (paddr_t)reg_basel;
                    (*अंक_drhu)++;
                    अवरोध;

                हाल DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_header;
                    अगर (acpi_dmar_rmrr->reg_base[1] != 0 ||
                        acpi_dmar_rmrr->reg_limit[1] != 0) {
                        म_लिखो("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *अंक_drhu = 0;
                        वापस ;
                    }

                    क्रम (i = 0; i <= (acpi_dmar_header->लंबाई - माप(acpi_dmar_rmrr_t)) / माप(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        अगर (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            म_लिखो("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *अंक_drhu = 0; /* report zero IOMMUs */
                            वापस;
                        }

                        अगर (acpi_dmar_devscope->लंबाई > माप(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            म_लिखो("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *अंक_drhu = 0; /* report zero IOMMUs */
                            वापस;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->start_bus,
                                acpi_dmar_devscope->path_0.dev,
                                acpi_dmar_devscope->path_0.fun
                            );

                        अगर (rmrr_गिनती == CONFIG_MAX_RMRR_ENTRIES) {
                            म_लिखो("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *अंक_drhu = 0;
                            वापस;
                        }
                        म_लिखो("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->start_bus,
                               acpi_dmar_devscope->path_0.dev,
                               acpi_dmar_devscope->path_0.fun
                              );

                        rmrr_list->entries[rmrr_गिनती].device = dev_id;
                        rmrr_list->entries[rmrr_गिनती].base = acpi_dmar_rmrr->reg_base[0];
                        rmrr_list->entries[rmrr_गिनती].limit = acpi_dmar_rmrr->reg_limit[0];
                        rmrr_गिनती++;
                    }
                    अवरोध;

                हाल DMAR_ATSR:
                    /* not implemented yet */
                    अवरोध;

                शेष:
                    म_लिखो("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_header->type);
                }
                acpi_dmar_header = (acpi_dmar_header_t *)((अक्षर *)acpi_dmar_header + acpi_dmar_header->लंबाई);
            }
        }
    }
    rmrr_list->अंक = rmrr_गिनती;
    म_लिखो("ACPI: %d IOMMUs detected\n", *अंक_drhu);
}
