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
    acpi_سرآیند_t سرآیند;
    uint8_t       host_addr_عرض;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               حجم(acpi_dmar_t) == حجم(acpi_سرآیند_t) + 12)

/* DMA Remapping Structure Header */
تعريفنوع بنية acpi_dmar_سرآیند {
    uint16_t type;
    uint16_t درازی;
} acpi_dmar_سرآیند_t;
compile_assert(acpi_dmar_سرآیند_packed, حجم(acpi_dmar_سرآیند_t) == 4)

/* DMA Remapping Structure Types */
تعداد acpi_جدول_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
تعريفنوع بنية acpi_dmar_drhd {
    acpi_dmar_سرآیند_t سرآیند;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_پایه[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               حجم(acpi_dmar_drhd_t) == حجم(acpi_dmar_سرآیند_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  درازی;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  شروع_bus;
    بنية {
        uint8_t dev;
        uint8_t fun;
    } مسیر_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, حجم(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_rmrr {
    acpi_dmar_سرآیند_t   سرآیند;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_پایه[2];
    uint32_t             reg_حد[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, حجم(acpi_dmar_rmrr_t) ==
               حجم(acpi_dmar_سرآیند_t) + 20 + حجم(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
تعريفنوع بنية acpi_fadt {
    acpi_سرآیند_t  سرآیند;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               حجم(acpi_fadt_t) == حجم(acpi_سرآیند_t) + 80)

/* Multiple APIC Description Table (MADT) */
تعريفنوع بنية acpi_madt {
    acpi_سرآیند_t سرآیند;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               حجم(acpi_madt_t) == حجم(acpi_سرآیند_t) + 8)

تعريفنوع بنية acpi_madt_سرآیند {
    uint8_t type;
    uint8_t درازی;
} acpi_madt_سرآیند_t;
compile_assert(acpi_madt_سرآیند_packed, حجم(acpi_madt_سرآیند_t) == 2)

تعداد acpi_جدول_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

تعريفنوع بنية acpi_madt_apic {
    acpi_madt_سرآیند_t سرآیند;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               حجم(acpi_madt_apic_t) == حجم(acpi_madt_سرآیند_t) + 6)

تعريفنوع بنية acpi_madt_x2apic {
    acpi_madt_سرآیند_t  سرآیند;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               حجم(acpi_madt_x2apic_t) == حجم(acpi_madt_سرآیند_t) + 14)

تعريفنوع بنية acpi_madt_ioapic {
    acpi_madt_سرآیند_t سرآیند;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               حجم(acpi_madt_ioapic_t) == حجم(acpi_madt_سرآیند_t) + 10)

تعريفنوع بنية acpi_madt_iso {
    acpi_madt_سرآیند_t سرآیند;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            منبع;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_کم;
    uint16_t           gsi_بالا;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               حجم(acpi_madt_iso_t) == حجم(acpi_madt_سرآیند_t) + 8)

#تعریف_کن MADT_MAX_مدخل_اندازه حجم(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_مدخل_اندازه, حجم(acpi_madt_apic_t) <= MADT_MAX_مدخل_اندازه)
compile_assert(acpi_madt_ioapic_مدخل_اندازه, حجم(acpi_madt_ioapic_t) <= MADT_MAX_مدخل_اندازه)
compile_assert(acpi_madt_iso_مدخل_اندازه, حجم(acpi_madt_iso_t) <= MADT_MAX_مدخل_اندازه)

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
BOOT_داده حرف madt_مدخل[MADT_MAX_مدخل_اندازه] ALIGN(4);

BOOT_کد ساكن uint8_t acpi_calc_checksum(حرف *شروع, uint32_t درازی)
{
    uint8_t checksum = 0;

    طالما (درازی > 0) {
        checksum += *شروع;
        شروع++;
        درازی--;
    }
    أرجع checksum;
}

BOOT_کد ساكن acpi_rsdp_t *acpi_get_rsdp(فراغ)
{
    حرف *addr;

    لكل (addr = (حرف *)BIOS_PADDR_شروع; addr < (حرف *)BIOS_PADDR_انتها; addr += 16) {
        إذا (مقایسه_رشته(addr, acpi_str_rsd, 8) == 0) {
            إذا (acpi_calc_checksum(addr, ACPI_V1_اندازه) == 0) {
                أرجع (acpi_rsdp_t *)addr;
            }
        }
    }
    أرجع عدم;
}

BOOT_کد ساكن فراغ *acpi_جدول_init(فراغ *مدخل, تعداد acpi_type جدول_type)
{
    فراغ *acpi_جدول;
    بیعلامت صحيح صفحات_for_جدول;
    بیعلامت صحيح صفحات_for_سرآیند = 1;

    /* if we need to map another page to read header */
    بیعلامت دراز جابه_جایی_درون_صفحه = (بیعلامت دراز)مدخل & ماسک(LARGE_صفحه_بیتها);
    إذا (ماسک(LARGE_صفحه_بیتها) - جابه_جایی_درون_صفحه < حجم(acpi_rsdp_t)) {
        صفحات_for_سرآیند++;
    }

    /* map in table's header */
    acpi_جدول = map_موقت_boot_صفحه(مدخل, صفحات_for_سرآیند);

    بدل (جدول_type) {
    حالة ACPI_RSDP: {
        acpi_rsdp_t *rsdp_مدخل = (acpi_rsdp_t *)مدخل;
        صفحات_for_جدول = (rsdp_مدخل->درازی + جابه_جایی_درون_صفحه) / ماسک(LARGE_صفحه_بیتها) + 1;
        اكسر;
    }
    حالة ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_مدخل = (acpi_rsdt_t *)مدخل;
        صفحات_for_جدول = (rsdt_مدخل->سرآیند.درازی + جابه_جایی_درون_صفحه) / ماسک(LARGE_صفحه_بیتها) + 1;
        اكسر;
    }
    افتراضي:
        چاپ_کن("Error: Mapping unknown ACPI table type\n");
        أكد(false);
        أرجع عدم;
    }

    /* map in full table */
    acpi_جدول = map_موقت_boot_صفحه(مدخل, صفحات_for_جدول);

    أرجع acpi_جدول;
}

BOOT_کد bool_t acpi_init(acpi_rsdp_t *rsdp_داده)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    إذا (acpi_rsdp == عدم) {
        چاپ_کن("BIOS: No ACPI support detected\n");
        أرجع false;
    }
    چاپ_کن("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_جدول_init(acpi_rsdp, ACPI_RSDP);
    چاپ_کن("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_داده = *acpi_rsdp;

    /* perform final validation */
    أرجع acpi_validate_rsdp(rsdp_داده);
}

BOOT_کد bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    إذا (acpi_calc_checksum((حرف *)acpi_rsdp, ACPI_V1_اندازه) != 0) {
        چاپ_کن("BIOS: ACPIv1 information corrupt\n");
        أرجع false;
    }

    إذا (acpi_rsdp->revision > 0 && acpi_calc_checksum((حرف *)acpi_rsdp, حجم(*acpi_rsdp)) != 0) {
        چاپ_کن("BIOS: ACPIv2 information corrupt\n");
        أرجع false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_آدرس;
    چاپ_کن("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init(acpi_rsdt, ACPI_RSDT);
    چاپ_کن("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    أكد(acpi_rsdt_mapped->سرآیند.درازی > 0);
    إذا (acpi_calc_checksum((حرف *)acpi_rsdt_mapped, acpi_rsdt_mapped->سرآیند.درازی) != 0) {
        چاپ_کن("ACPI: RSDT checksum failure\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کد ساكن bool_t acpi_madt_valid(acpi_madt_سرآیند_t *سرآیند, ثابت حرف *name, uint8_t expected)
{
    إذا (سرآیند->درازی < expected) {
        چاپ_کن("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, سرآیند->درازی, expected);
        أرجع false;
    }
    أرجع true;
}

BOOT_کد uint32_t acpi_madt_پیمایش(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *رقم_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    بیعلامت صحيح        entries;
    بیعلامت صحيح        مدخل_اندازه;
    uint32_t            رقم_cpu;
    uint32_t            شمار;
    acpi_madt_t        *acpi_madt;
    acpi_madt_سرآیند_t *acpi_madt_سرآیند;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_آدرس, ACPI_RSDT);

    رقم_cpu = 0;
    *رقم_ioapic = 0;

    أكد(acpi_rsdt_mapped->سرآیند.درازی >= حجم(acpi_سرآیند_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرآیند.درازی - حجم(acpi_سرآیند_t)) / حجم(uint32_t);
    لكل (شمار = 0; شمار < entries; شمار++) {
        acpi_madt = (acpi_madt_t *)(کلمه_t)acpi_rsdt_mapped->مدخل[شمار];
        acpi_madt_mapped = (acpi_madt_t *)acpi_جدول_init(acpi_madt, ACPI_RSDT);

        إذا (مقایسه_رشته(acpi_str_apic, acpi_madt_mapped->سرآیند.signature, 4) == 0) {
            چاپ_کن("ACPI: MADT paddr=%p\n", acpi_madt);
            چاپ_کن("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            چاپ_کن("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            چاپ_کن("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_سرآیند = (acpi_madt_سرآیند_t *)(acpi_madt_mapped + 1);

            طالما ((حرف *)acpi_madt_سرآیند < (حرف *)acpi_madt_mapped + acpi_madt_mapped->سرآیند.درازی) {
                مدخل_اندازه = MIN(acpi_madt_سرآیند->درازی, MADT_MAX_مدخل_اندازه);
                کپی_حافظه(madt_مدخل, acpi_madt_سرآیند, مدخل_اندازه);

                بدل (acpi_madt_سرآیند->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                حالة MADT_APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرآیند, madt_str_apic, حجم(acpi_madt_apic_t))) {
                        اكسر;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_مدخل;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    إذا (flags == 1) {
                        چاپ_کن("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        إذا (رقم_cpu == CONFIG_MAX_رقم_NODES) {
                            چاپ_کن("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_رقم_NODES);
                        } وإلا {
                            cpu_list[رقم_cpu] = cpu_id;
                            رقم_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_x2APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرآیند, madt_str_x2apic, حجم(acpi_madt_x2apic_t))) {
                        اكسر;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_مدخل;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    إذا (flags == 1) {
                        چاپ_کن("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        إذا (رقم_cpu == CONFIG_MAX_رقم_NODES) {
                            چاپ_کن("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_رقم_NODES);
                        } وإلا {
                            cpu_list[رقم_cpu] = cpu_id;
                            رقم_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_IOAPIC:
                    إذا (!acpi_madt_valid(acpi_madt_سرآیند, madt_str_ioapic, حجم(acpi_madt_ioapic_t))) {
                        اكسر;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_مدخل;

                    چاپ_کن(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    إذا (*رقم_ioapic == CONFIG_MAX_رقم_IOAPIC) {
                        چاپ_کن("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_رقم_IOAPIC);
                    } وإلا {
                        ioapic_paddrs[*رقم_ioapic] = madt_ioapic->ioapic_addr;
                        (*رقم_ioapic)++;
                    }
                    اكسر;
                حالة MADT_ISO: {
                    إذا (!acpi_madt_valid(acpi_madt_سرآیند, madt_str_iso, حجم(acpi_madt_iso_t))) {
                        اكسر;
                    }
                    استفادهنشده acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_مدخل;

                    چاپ_کن("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->منبع, iso->gsi_بالا, iso->gsi_کم, iso->flags);
                    اكسر;
                }
                افتراضي:
                    اكسر;
                }
                acpi_madt_سرآیند = (acpi_madt_سرآیند_t *)((حرف *)acpi_madt_سرآیند + acpi_madt_سرآیند->درازی);
            }
        }
    }

    چاپ_کن("ACPI: %d CPU(s) detected\n", رقم_cpu);

    أرجع رقم_cpu;
}

BOOT_کد bool_t acpi_fadt_پیمایش(
    acpi_rsdp_t *acpi_rsdp
)
{
    بیعلامت صحيح entries;
    uint32_t            شمار;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_آدرس, ACPI_RSDT);

    أكد(acpi_rsdt_mapped->سرآیند.درازی >= حجم(acpi_سرآیند_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرآیند.درازی - حجم(acpi_سرآیند_t)) / حجم(uint32_t);
    لكل (شمار = 0; شمار < entries; شمار++) {
        acpi_fadt = (acpi_fadt_t *)(کلمه_t)acpi_rsdt_mapped->مدخل[شمار];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_جدول_init(acpi_fadt, ACPI_RSDT);

        إذا (مقایسه_رشته(acpi_str_fadt, acpi_fadt_mapped->سرآیند.signature, 4) == 0) {
            چاپ_کن("ACPI: FADT paddr=%p\n", acpi_fadt);
            چاپ_کن("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            چاپ_کن("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            إذا (config_مجموعه(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & بیت(19)) {
                چاپ_کن("system requires apic physical mode\n");
                أرجع false;
            }
        }
    }

    أرجع true;
}

BOOT_کد فراغ acpi_dmar_پیمایش(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *رقم_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    کلمه_t i;
    بیعلامت صحيح entries;
    uint32_t شمار;
    uint32_t reg_basel, reg_baseh;
    صحيح rmrr_شمار;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_سرآیند_t   *acpi_dmar_سرآیند;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_آدرس, ACPI_RSDT);

    *رقم_drhu = 0;
    rmrr_شمار = 0;

    أكد(acpi_rsdt_mapped->سرآیند.درازی >= حجم(acpi_سرآیند_t));
    entries = (acpi_rsdt_mapped->سرآیند.درازی - حجم(acpi_سرآیند_t)) / حجم(uint32_t);
    لكل (شمار = 0; شمار < entries; شمار++) {
        acpi_dmar = (acpi_dmar_t *)(کلمه_t)acpi_rsdt_mapped->مدخل[شمار];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_جدول_init(acpi_dmar, ACPI_RSDT);

        إذا (مقایسه_رشته(acpi_str_dmar, acpi_dmar_mapped->سرآیند.signature, 4) == 0) {
            چاپ_کن("ACPI: DMAR paddr=%p\n", acpi_dmar);
            چاپ_کن("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            چاپ_کن("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_عرض + 1);
            acpi_dmar_سرآیند = (acpi_dmar_سرآیند_t *)(acpi_dmar_mapped + 1);

            طالما ((حرف *)acpi_dmar_سرآیند < (حرف *)acpi_dmar_mapped + acpi_dmar_mapped->سرآیند.درازی) {
                بدل (acpi_dmar_سرآیند->type) {

                حالة DMAR_DRHD:
                    إذا (*رقم_drhu == max_drhu_list_len) {
                        چاپ_کن("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *رقم_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_سرآیند)->reg_پایه[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_سرآیند)->reg_پایه[1];
                    /* check if value fits into uint32_t */
                    إذا (reg_baseh != 0) {
                        چاپ_کن("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *رقم_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    drhu_list[*رقم_drhu] = (paddr_t)reg_basel;
                    (*رقم_drhu)++;
                    اكسر;

                حالة DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_سرآیند;
                    إذا (acpi_dmar_rmrr->reg_پایه[1] != 0 ||
                        acpi_dmar_rmrr->reg_حد[1] != 0) {
                        چاپ_کن("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *رقم_drhu = 0;
                        أرجع ;
                    }

                    لكل (i = 0; i <= (acpi_dmar_سرآیند->درازی - حجم(acpi_dmar_rmrr_t)) / حجم(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        إذا (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            چاپ_کن("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *رقم_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        إذا (acpi_dmar_devscope->درازی > حجم(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            چاپ_کن("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *رقم_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->شروع_bus,
                                acpi_dmar_devscope->مسیر_0.dev,
                                acpi_dmar_devscope->مسیر_0.fun
                            );

                        إذا (rmrr_شمار == CONFIG_MAX_RMRR_ENTRIES) {
                            چاپ_کن("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *رقم_drhu = 0;
                            أرجع;
                        }
                        چاپ_کن("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->شروع_bus,
                               acpi_dmar_devscope->مسیر_0.dev,
                               acpi_dmar_devscope->مسیر_0.fun
                              );

                        rmrr_list->entries[rmrr_شمار].device = dev_id;
                        rmrr_list->entries[rmrr_شمار].پایه = acpi_dmar_rmrr->reg_پایه[0];
                        rmrr_list->entries[rmrr_شمار].حد = acpi_dmar_rmrr->reg_حد[0];
                        rmrr_شمار++;
                    }
                    اكسر;

                حالة DMAR_ATSR:
                    /* not implemented yet */
                    اكسر;

                افتراضي:
                    چاپ_کن("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_سرآیند->type);
                }
                acpi_dmar_سرآیند = (acpi_dmar_سرآیند_t *)((حرف *)acpi_dmar_سرآیند + acpi_dmar_سرآیند->درازی);
            }
        }
    }
    rmrr_list->رقم = rmrr_شمار;
    چاپ_کن("ACPI: %d IOMMUs detected\n", *رقم_drhu);
}
