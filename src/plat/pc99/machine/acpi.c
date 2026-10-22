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
    acpi_سرو_t سرو;
    uint8_t       host_addr_ويڪر;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               حجم(acpi_dmar_t) == حجم(acpi_سرو_t) + 12)

/* DMA Remapping Structure Header */
تعريفنوع بنية acpi_dmar_سرو {
    uint16_t type;
    uint16_t ڊيگهائي;
} acpi_dmar_سرو_t;
compile_assert(acpi_dmar_سرو_packed, حجم(acpi_dmar_سرو_t) == 4)

/* DMA Remapping Structure Types */
تعداد acpi_جدول_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
تعريفنوع بنية acpi_dmar_drhd {
    acpi_dmar_سرو_t سرو;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_بنياد[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               حجم(acpi_dmar_drhd_t) == حجم(acpi_dmar_سرو_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  ڊيگهائي;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  شروعات_bus;
    بنية {
        uint8_t dev;
        uint8_t fun;
    } رستو_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, حجم(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_rmrr {
    acpi_dmar_سرو_t   سرو;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_بنياد[2];
    uint32_t             reg_حد_بندي[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, حجم(acpi_dmar_rmrr_t) ==
               حجم(acpi_dmar_سرو_t) + 20 + حجم(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
تعريفنوع بنية acpi_fadt {
    acpi_سرو_t  سرو;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               حجم(acpi_fadt_t) == حجم(acpi_سرو_t) + 80)

/* Multiple APIC Description Table (MADT) */
تعريفنوع بنية acpi_madt {
    acpi_سرو_t سرو;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               حجم(acpi_madt_t) == حجم(acpi_سرو_t) + 8)

تعريفنوع بنية acpi_madt_سرو {
    uint8_t type;
    uint8_t ڊيگهائي;
} acpi_madt_سرو_t;
compile_assert(acpi_madt_سرو_packed, حجم(acpi_madt_سرو_t) == 2)

تعداد acpi_جدول_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

تعريفنوع بنية acpi_madt_apic {
    acpi_madt_سرو_t سرو;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               حجم(acpi_madt_apic_t) == حجم(acpi_madt_سرو_t) + 6)

تعريفنوع بنية acpi_madt_x2apic {
    acpi_madt_سرو_t  سرو;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               حجم(acpi_madt_x2apic_t) == حجم(acpi_madt_سرو_t) + 14)

تعريفنوع بنية acpi_madt_ioapic {
    acpi_madt_سرو_t سرو;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               حجم(acpi_madt_ioapic_t) == حجم(acpi_madt_سرو_t) + 10)

تعريفنوع بنية acpi_madt_iso {
    acpi_madt_سرو_t سرو;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            ذريعو;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_هيٺيون;
    uint16_t           gsi_مٿاهون;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               حجم(acpi_madt_iso_t) == حجم(acpi_madt_سرو_t) + 8)

#وصف_ڪريو MADT_MAX_داخلا_سائيز حجم(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_داخلا_سائيز, حجم(acpi_madt_apic_t) <= MADT_MAX_داخلا_سائيز)
compile_assert(acpi_madt_ioapic_داخلا_سائيز, حجم(acpi_madt_ioapic_t) <= MADT_MAX_داخلا_سائيز)
compile_assert(acpi_madt_iso_داخلا_سائيز, حجم(acpi_madt_iso_t) <= MADT_MAX_داخلا_سائيز)

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
BOOT_معلومات حرف madt_داخلا[MADT_MAX_داخلا_سائيز] ALIGN(4);

BOOT_ڪوڊ ساكن uint8_t acpi_calc_checksum(حرف *شروعات, uint32_t ڊيگهائي)
{
    uint8_t checksum = 0;

    طالما (ڊيگهائي > 0) {
        checksum += *شروعات;
        شروعات++;
        ڊيگهائي--;
    }
    أرجع checksum;
}

BOOT_ڪوڊ ساكن acpi_rsdp_t *acpi_get_rsdp(فراغ)
{
    حرف *addr;

    لكل (addr = (حرف *)BIOS_PADDR_شروعات; addr < (حرف *)BIOS_PADDR_پڇاڙي; addr += 16) {
        إذا (لڙي_ڀيٽ(addr, acpi_str_rsd, 8) == 0) {
            إذا (acpi_calc_checksum(addr, ACPI_V1_سائيز) == 0) {
                أرجع (acpi_rsdp_t *)addr;
            }
        }
    }
    أرجع عدم;
}

BOOT_ڪوڊ ساكن فراغ *acpi_جدول_init(فراغ *داخلا, تعداد acpi_type جدول_type)
{
    فراغ *acpi_جدول;
    بنانشان صحيح صفحا_for_جدول;
    بنانشان صحيح صفحا_for_سرو = 1;

    /* if we need to map another page to read header */
    بنانشان ڊگهو وٿي_اندريون_صفحو = (بنانشان ڊگهو)داخلا & نقاب(LARGE_صفحو_بٽ);
    إذا (نقاب(LARGE_صفحو_بٽ) - وٿي_اندريون_صفحو < حجم(acpi_rsdp_t)) {
        صفحا_for_سرو++;
    }

    /* map in table's header */
    acpi_جدول = map_عارضي_boot_صفحو(داخلا, صفحا_for_سرو);

    بدل (جدول_type) {
    حالة ACPI_RSDP: {
        acpi_rsdp_t *rsdp_داخلا = (acpi_rsdp_t *)داخلا;
        صفحا_for_جدول = (rsdp_داخلا->ڊيگهائي + وٿي_اندريون_صفحو) / نقاب(LARGE_صفحو_بٽ) + 1;
        اكسر;
    }
    حالة ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_داخلا = (acpi_rsdt_t *)داخلا;
        صفحا_for_جدول = (rsdt_داخلا->سرو.ڊيگهائي + وٿي_اندريون_صفحو) / نقاب(LARGE_صفحو_بٽ) + 1;
        اكسر;
    }
    افتراضي:
        لکو("Error: Mapping unknown ACPI table type\n");
        أكد(false);
        أرجع عدم;
    }

    /* map in full table */
    acpi_جدول = map_عارضي_boot_صفحو(داخلا, صفحا_for_جدول);

    أرجع acpi_جدول;
}

BOOT_ڪوڊ bool_t acpi_init(acpi_rsdp_t *rsdp_معلومات)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    إذا (acpi_rsdp == عدم) {
        لکو("BIOS: No ACPI support detected\n");
        أرجع false;
    }
    لکو("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_جدول_init(acpi_rsdp, ACPI_RSDP);
    لکو("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_معلومات = *acpi_rsdp;

    /* perform final validation */
    أرجع acpi_validate_rsdp(rsdp_معلومات);
}

BOOT_ڪوڊ bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    إذا (acpi_calc_checksum((حرف *)acpi_rsdp, ACPI_V1_سائيز) != 0) {
        لکو("BIOS: ACPIv1 information corrupt\n");
        أرجع false;
    }

    إذا (acpi_rsdp->revision > 0 && acpi_calc_checksum((حرف *)acpi_rsdp, حجم(*acpi_rsdp)) != 0) {
        لکو("BIOS: ACPIv2 information corrupt\n");
        أرجع false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتو;
    لکو("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init(acpi_rsdt, ACPI_RSDT);
    لکو("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    أكد(acpi_rsdt_mapped->سرو.ڊيگهائي > 0);
    إذا (acpi_calc_checksum((حرف *)acpi_rsdt_mapped, acpi_rsdt_mapped->سرو.ڊيگهائي) != 0) {
        لکو("ACPI: RSDT checksum failure\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_ڪوڊ ساكن bool_t acpi_madt_valid(acpi_madt_سرو_t *سرو, ثابت حرف *name, uint8_t expected)
{
    إذا (سرو->ڊيگهائي < expected) {
        لکو("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, سرو->ڊيگهائي, expected);
        أرجع false;
    }
    أرجع true;
}

BOOT_ڪوڊ uint32_t acpi_madt_جاچ(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *انگ_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    بنانشان صحيح        entries;
    بنانشان صحيح        داخلا_سائيز;
    uint32_t            انگ_cpu;
    uint32_t            ڳڻپ;
    acpi_madt_t        *acpi_madt;
    acpi_madt_سرو_t *acpi_madt_سرو;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتو, ACPI_RSDT);

    انگ_cpu = 0;
    *انگ_ioapic = 0;

    أكد(acpi_rsdt_mapped->سرو.ڊيگهائي >= حجم(acpi_سرو_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرو.ڊيگهائي - حجم(acpi_سرو_t)) / حجم(uint32_t);
    لكل (ڳڻپ = 0; ڳڻپ < entries; ڳڻپ++) {
        acpi_madt = (acpi_madt_t *)(لفظ_t)acpi_rsdt_mapped->داخلا[ڳڻپ];
        acpi_madt_mapped = (acpi_madt_t *)acpi_جدول_init(acpi_madt, ACPI_RSDT);

        إذا (لڙي_ڀيٽ(acpi_str_apic, acpi_madt_mapped->سرو.signature, 4) == 0) {
            لکو("ACPI: MADT paddr=%p\n", acpi_madt);
            لکو("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            لکو("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            لکو("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_سرو = (acpi_madt_سرو_t *)(acpi_madt_mapped + 1);

            طالما ((حرف *)acpi_madt_سرو < (حرف *)acpi_madt_mapped + acpi_madt_mapped->سرو.ڊيگهائي) {
                داخلا_سائيز = MIN(acpi_madt_سرو->ڊيگهائي, MADT_MAX_داخلا_سائيز);
                يادگيري_نقل(madt_داخلا, acpi_madt_سرو, داخلا_سائيز);

                بدل (acpi_madt_سرو->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                حالة MADT_APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرو, madt_str_apic, حجم(acpi_madt_apic_t))) {
                        اكسر;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_داخلا;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    إذا (flags == 1) {
                        لکو("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        إذا (انگ_cpu == CONFIG_MAX_انگ_NODES) {
                            لکو("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_انگ_NODES);
                        } وإلا {
                            cpu_list[انگ_cpu] = cpu_id;
                            انگ_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_x2APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرو, madt_str_x2apic, حجم(acpi_madt_x2apic_t))) {
                        اكسر;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_داخلا;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    إذا (flags == 1) {
                        لکو("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        إذا (انگ_cpu == CONFIG_MAX_انگ_NODES) {
                            لکو("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_انگ_NODES);
                        } وإلا {
                            cpu_list[انگ_cpu] = cpu_id;
                            انگ_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_IOAPIC:
                    إذا (!acpi_madt_valid(acpi_madt_سرو, madt_str_ioapic, حجم(acpi_madt_ioapic_t))) {
                        اكسر;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_داخلا;

                    لکو(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    إذا (*انگ_ioapic == CONFIG_MAX_انگ_IOAPIC) {
                        لکو("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_انگ_IOAPIC);
                    } وإلا {
                        ioapic_paddrs[*انگ_ioapic] = madt_ioapic->ioapic_addr;
                        (*انگ_ioapic)++;
                    }
                    اكسر;
                حالة MADT_ISO: {
                    إذا (!acpi_madt_valid(acpi_madt_سرو, madt_str_iso, حجم(acpi_madt_iso_t))) {
                        اكسر;
                    }
                    اڻ_استعمال_ٿيل acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_داخلا;

                    لکو("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->ذريعو, iso->gsi_مٿاهون, iso->gsi_هيٺيون, iso->flags);
                    اكسر;
                }
                افتراضي:
                    اكسر;
                }
                acpi_madt_سرو = (acpi_madt_سرو_t *)((حرف *)acpi_madt_سرو + acpi_madt_سرو->ڊيگهائي);
            }
        }
    }

    لکو("ACPI: %d CPU(s) detected\n", انگ_cpu);

    أرجع انگ_cpu;
}

BOOT_ڪوڊ bool_t acpi_fadt_جاچ(
    acpi_rsdp_t *acpi_rsdp
)
{
    بنانشان صحيح entries;
    uint32_t            ڳڻپ;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتو, ACPI_RSDT);

    أكد(acpi_rsdt_mapped->سرو.ڊيگهائي >= حجم(acpi_سرو_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرو.ڊيگهائي - حجم(acpi_سرو_t)) / حجم(uint32_t);
    لكل (ڳڻپ = 0; ڳڻپ < entries; ڳڻپ++) {
        acpi_fadt = (acpi_fadt_t *)(لفظ_t)acpi_rsdt_mapped->داخلا[ڳڻپ];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_جدول_init(acpi_fadt, ACPI_RSDT);

        إذا (لڙي_ڀيٽ(acpi_str_fadt, acpi_fadt_mapped->سرو.signature, 4) == 0) {
            لکو("ACPI: FADT paddr=%p\n", acpi_fadt);
            لکو("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            لکو("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            إذا (config_سيٽ(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & ٻٽو_انگ(19)) {
                لکو("system requires apic physical mode\n");
                أرجع false;
            }
        }
    }

    أرجع true;
}

BOOT_ڪوڊ فراغ acpi_dmar_جاچ(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *انگ_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    لفظ_t i;
    بنانشان صحيح entries;
    uint32_t ڳڻپ;
    uint32_t reg_basel, reg_baseh;
    صحيح rmrr_ڳڻپ;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_سرو_t   *acpi_dmar_سرو;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتو, ACPI_RSDT);

    *انگ_drhu = 0;
    rmrr_ڳڻپ = 0;

    أكد(acpi_rsdt_mapped->سرو.ڊيگهائي >= حجم(acpi_سرو_t));
    entries = (acpi_rsdt_mapped->سرو.ڊيگهائي - حجم(acpi_سرو_t)) / حجم(uint32_t);
    لكل (ڳڻپ = 0; ڳڻپ < entries; ڳڻپ++) {
        acpi_dmar = (acpi_dmar_t *)(لفظ_t)acpi_rsdt_mapped->داخلا[ڳڻپ];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_جدول_init(acpi_dmar, ACPI_RSDT);

        إذا (لڙي_ڀيٽ(acpi_str_dmar, acpi_dmar_mapped->سرو.signature, 4) == 0) {
            لکو("ACPI: DMAR paddr=%p\n", acpi_dmar);
            لکو("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            لکو("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_ويڪر + 1);
            acpi_dmar_سرو = (acpi_dmar_سرو_t *)(acpi_dmar_mapped + 1);

            طالما ((حرف *)acpi_dmar_سرو < (حرف *)acpi_dmar_mapped + acpi_dmar_mapped->سرو.ڊيگهائي) {
                بدل (acpi_dmar_سرو->type) {

                حالة DMAR_DRHD:
                    إذا (*انگ_drhu == max_drhu_list_len) {
                        لکو("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *انگ_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_سرو)->reg_بنياد[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_سرو)->reg_بنياد[1];
                    /* check if value fits into uint32_t */
                    إذا (reg_baseh != 0) {
                        لکو("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *انگ_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    drhu_list[*انگ_drhu] = (paddr_t)reg_basel;
                    (*انگ_drhu)++;
                    اكسر;

                حالة DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_سرو;
                    إذا (acpi_dmar_rmrr->reg_بنياد[1] != 0 ||
                        acpi_dmar_rmrr->reg_حد_بندي[1] != 0) {
                        لکو("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *انگ_drhu = 0;
                        أرجع ;
                    }

                    لكل (i = 0; i <= (acpi_dmar_سرو->ڊيگهائي - حجم(acpi_dmar_rmrr_t)) / حجم(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        إذا (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            لکو("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *انگ_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        إذا (acpi_dmar_devscope->ڊيگهائي > حجم(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            لکو("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *انگ_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->شروعات_bus,
                                acpi_dmar_devscope->رستو_0.dev,
                                acpi_dmar_devscope->رستو_0.fun
                            );

                        إذا (rmrr_ڳڻپ == CONFIG_MAX_RMRR_ENTRIES) {
                            لکو("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *انگ_drhu = 0;
                            أرجع;
                        }
                        لکو("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->شروعات_bus,
                               acpi_dmar_devscope->رستو_0.dev,
                               acpi_dmar_devscope->رستو_0.fun
                              );

                        rmrr_list->entries[rmrr_ڳڻپ].device = dev_id;
                        rmrr_list->entries[rmrr_ڳڻپ].بنياد = acpi_dmar_rmrr->reg_بنياد[0];
                        rmrr_list->entries[rmrr_ڳڻپ].حد_بندي = acpi_dmar_rmrr->reg_حد_بندي[0];
                        rmrr_ڳڻپ++;
                    }
                    اكسر;

                حالة DMAR_ATSR:
                    /* not implemented yet */
                    اكسر;

                افتراضي:
                    لکو("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_سرو->type);
                }
                acpi_dmar_سرو = (acpi_dmar_سرو_t *)((حرف *)acpi_dmar_سرو + acpi_dmar_سرو->ڊيگهائي);
            }
        }
    }
    rmrr_list->انگ = rmrr_ڳڻپ;
    لکو("ACPI: %d IOMMUs detected\n", *انگ_drhu);
}
