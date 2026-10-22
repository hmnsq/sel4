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
    acpi_سرخی_t سرخی;
    uint8_t       host_addr_چوڑائی;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               حجم(acpi_dmar_t) == حجم(acpi_سرخی_t) + 12)

/* DMA Remapping Structure Header */
تعريفنوع بنية acpi_dmar_سرخی {
    uint16_t type;
    uint16_t لمائی;
} acpi_dmar_سرخی_t;
compile_assert(acpi_dmar_سرخی_packed, حجم(acpi_dmar_سرخی_t) == 4)

/* DMA Remapping Structure Types */
تعداد acpi_جدول_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
تعريفنوع بنية acpi_dmar_drhd {
    acpi_dmar_سرخی_t سرخی;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_ادھار[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               حجم(acpi_dmar_drhd_t) == حجم(acpi_dmar_سرخی_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  لمائی;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  شروع_bus;
    بنية {
        uint8_t dev;
        uint8_t fun;
    } رستہ_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, حجم(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_rmrr {
    acpi_dmar_سرخی_t   سرخی;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_ادھار[2];
    uint32_t             reg_حد_بندی[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, حجم(acpi_dmar_rmrr_t) ==
               حجم(acpi_dmar_سرخی_t) + 20 + حجم(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
تعريفنوع بنية acpi_fadt {
    acpi_سرخی_t  سرخی;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               حجم(acpi_fadt_t) == حجم(acpi_سرخی_t) + 80)

/* Multiple APIC Description Table (MADT) */
تعريفنوع بنية acpi_madt {
    acpi_سرخی_t سرخی;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               حجم(acpi_madt_t) == حجم(acpi_سرخی_t) + 8)

تعريفنوع بنية acpi_madt_سرخی {
    uint8_t type;
    uint8_t لمائی;
} acpi_madt_سرخی_t;
compile_assert(acpi_madt_سرخی_packed, حجم(acpi_madt_سرخی_t) == 2)

تعداد acpi_جدول_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

تعريفنوع بنية acpi_madt_apic {
    acpi_madt_سرخی_t سرخی;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               حجم(acpi_madt_apic_t) == حجم(acpi_madt_سرخی_t) + 6)

تعريفنوع بنية acpi_madt_x2apic {
    acpi_madt_سرخی_t  سرخی;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               حجم(acpi_madt_x2apic_t) == حجم(acpi_madt_سرخی_t) + 14)

تعريفنوع بنية acpi_madt_ioapic {
    acpi_madt_سرخی_t سرخی;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               حجم(acpi_madt_ioapic_t) == حجم(acpi_madt_سرخی_t) + 10)

تعريفنوع بنية acpi_madt_iso {
    acpi_madt_سرخی_t سرخی;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            سروت;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_کم;
    uint16_t           gsi_بلند;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               حجم(acpi_madt_iso_t) == حجم(acpi_madt_سرخی_t) + 8)

#تعریف_کرو MADT_MAX_اندراج_ناپ حجم(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_اندراج_ناپ, حجم(acpi_madt_apic_t) <= MADT_MAX_اندراج_ناپ)
compile_assert(acpi_madt_ioapic_اندراج_ناپ, حجم(acpi_madt_ioapic_t) <= MADT_MAX_اندراج_ناپ)
compile_assert(acpi_madt_iso_اندراج_ناپ, حجم(acpi_madt_iso_t) <= MADT_MAX_اندراج_ناپ)

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
BOOT_انکڑے حرف madt_اندراج[MADT_MAX_اندراج_ناپ] ALIGN(4);

BOOT_کوڈ ساكن uint8_t acpi_calc_checksum(حرف *شروع, uint32_t لمائی)
{
    uint8_t checksum = 0;

    طالما (لمائی > 0) {
        checksum += *شروع;
        شروع++;
        لمائی--;
    }
    أرجع checksum;
}

BOOT_کوڈ ساكن acpi_rsdp_t *acpi_get_rsdp(فراغ)
{
    حرف *addr;

    لكل (addr = (حرف *)BIOS_PADDR_شروع; addr < (حرف *)BIOS_PADDR_سرا; addr += 16) {
        إذا (لڑی_مقابلہ(addr, acpi_str_rsd, 8) == 0) {
            إذا (acpi_calc_checksum(addr, ACPI_V1_ناپ) == 0) {
                أرجع (acpi_rsdp_t *)addr;
            }
        }
    }
    أرجع عدم;
}

BOOT_کوڈ ساكن فراغ *acpi_جدول_init(فراغ *اندراج, تعداد acpi_type جدول_type)
{
    فراغ *acpi_جدول;
    بنانشانی صحيح ورقے_for_جدول;
    بنانشانی صحيح ورقے_for_سرخی = 1;

    /* if we need to map another page to read header */
    بنانشانی لما وتھ_وچ_ورقہ = (بنانشانی لما)اندراج & نقاب(LARGE_ورقہ_بٹ);
    إذا (نقاب(LARGE_ورقہ_بٹ) - وتھ_وچ_ورقہ < حجم(acpi_rsdp_t)) {
        ورقے_for_سرخی++;
    }

    /* map in table's header */
    acpi_جدول = map_عارضی_boot_ورقہ(اندراج, ورقے_for_سرخی);

    بدل (جدول_type) {
    حالة ACPI_RSDP: {
        acpi_rsdp_t *rsdp_اندراج = (acpi_rsdp_t *)اندراج;
        ورقے_for_جدول = (rsdp_اندراج->لمائی + وتھ_وچ_ورقہ) / نقاب(LARGE_ورقہ_بٹ) + 1;
        اكسر;
    }
    حالة ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_اندراج = (acpi_rsdt_t *)اندراج;
        ورقے_for_جدول = (rsdt_اندراج->سرخی.لمائی + وتھ_وچ_ورقہ) / نقاب(LARGE_ورقہ_بٹ) + 1;
        اكسر;
    }
    افتراضي:
        لکھو("Error: Mapping unknown ACPI table type\n");
        أكد(false);
        أرجع عدم;
    }

    /* map in full table */
    acpi_جدول = map_عارضی_boot_ورقہ(اندراج, ورقے_for_جدول);

    أرجع acpi_جدول;
}

BOOT_کوڈ bool_t acpi_init(acpi_rsdp_t *rsdp_انکڑے)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    إذا (acpi_rsdp == عدم) {
        لکھو("BIOS: No ACPI support detected\n");
        أرجع false;
    }
    لکھو("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_جدول_init(acpi_rsdp, ACPI_RSDP);
    لکھو("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_انکڑے = *acpi_rsdp;

    /* perform final validation */
    أرجع acpi_validate_rsdp(rsdp_انکڑے);
}

BOOT_کوڈ bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    إذا (acpi_calc_checksum((حرف *)acpi_rsdp, ACPI_V1_ناپ) != 0) {
        لکھو("BIOS: ACPIv1 information corrupt\n");
        أرجع false;
    }

    إذا (acpi_rsdp->revision > 0 && acpi_calc_checksum((حرف *)acpi_rsdp, حجم(*acpi_rsdp)) != 0) {
        لکھو("BIOS: ACPIv2 information corrupt\n");
        أرجع false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتہ;
    لکھو("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init(acpi_rsdt, ACPI_RSDT);
    لکھو("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    أكد(acpi_rsdt_mapped->سرخی.لمائی > 0);
    إذا (acpi_calc_checksum((حرف *)acpi_rsdt_mapped, acpi_rsdt_mapped->سرخی.لمائی) != 0) {
        لکھو("ACPI: RSDT checksum failure\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کوڈ ساكن bool_t acpi_madt_valid(acpi_madt_سرخی_t *سرخی, ثابت حرف *name, uint8_t expected)
{
    إذا (سرخی->لمائی < expected) {
        لکھو("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, سرخی->لمائی, expected);
        أرجع false;
    }
    أرجع true;
}

BOOT_کوڈ uint32_t acpi_madt_جانچ(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *عدد_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    بنانشانی صحيح        entries;
    بنانشانی صحيح        اندراج_ناپ;
    uint32_t            عدد_cpu;
    uint32_t            گنتی;
    acpi_madt_t        *acpi_madt;
    acpi_madt_سرخی_t *acpi_madt_سرخی;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتہ, ACPI_RSDT);

    عدد_cpu = 0;
    *عدد_ioapic = 0;

    أكد(acpi_rsdt_mapped->سرخی.لمائی >= حجم(acpi_سرخی_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرخی.لمائی - حجم(acpi_سرخی_t)) / حجم(uint32_t);
    لكل (گنتی = 0; گنتی < entries; گنتی++) {
        acpi_madt = (acpi_madt_t *)(لفظ_t)acpi_rsdt_mapped->اندراج[گنتی];
        acpi_madt_mapped = (acpi_madt_t *)acpi_جدول_init(acpi_madt, ACPI_RSDT);

        إذا (لڑی_مقابلہ(acpi_str_apic, acpi_madt_mapped->سرخی.signature, 4) == 0) {
            لکھو("ACPI: MADT paddr=%p\n", acpi_madt);
            لکھو("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            لکھو("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            لکھو("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_سرخی = (acpi_madt_سرخی_t *)(acpi_madt_mapped + 1);

            طالما ((حرف *)acpi_madt_سرخی < (حرف *)acpi_madt_mapped + acpi_madt_mapped->سرخی.لمائی) {
                اندراج_ناپ = MIN(acpi_madt_سرخی->لمائی, MADT_MAX_اندراج_ناپ);
                حافظہ_نقل(madt_اندراج, acpi_madt_سرخی, اندراج_ناپ);

                بدل (acpi_madt_سرخی->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                حالة MADT_APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرخی, madt_str_apic, حجم(acpi_madt_apic_t))) {
                        اكسر;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_اندراج;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    إذا (flags == 1) {
                        لکھو("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        إذا (عدد_cpu == CONFIG_MAX_عدد_NODES) {
                            لکھو("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_عدد_NODES);
                        } وإلا {
                            cpu_list[عدد_cpu] = cpu_id;
                            عدد_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_x2APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرخی, madt_str_x2apic, حجم(acpi_madt_x2apic_t))) {
                        اكسر;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_اندراج;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    إذا (flags == 1) {
                        لکھو("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        إذا (عدد_cpu == CONFIG_MAX_عدد_NODES) {
                            لکھو("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_عدد_NODES);
                        } وإلا {
                            cpu_list[عدد_cpu] = cpu_id;
                            عدد_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_IOAPIC:
                    إذا (!acpi_madt_valid(acpi_madt_سرخی, madt_str_ioapic, حجم(acpi_madt_ioapic_t))) {
                        اكسر;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_اندراج;

                    لکھو(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    إذا (*عدد_ioapic == CONFIG_MAX_عدد_IOAPIC) {
                        لکھو("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_عدد_IOAPIC);
                    } وإلا {
                        ioapic_paddrs[*عدد_ioapic] = madt_ioapic->ioapic_addr;
                        (*عدد_ioapic)++;
                    }
                    اكسر;
                حالة MADT_ISO: {
                    إذا (!acpi_madt_valid(acpi_madt_سرخی, madt_str_iso, حجم(acpi_madt_iso_t))) {
                        اكسر;
                    }
                    بن_ورتیا acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_اندراج;

                    لکھو("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->سروت, iso->gsi_بلند, iso->gsi_کم, iso->flags);
                    اكسر;
                }
                افتراضي:
                    اكسر;
                }
                acpi_madt_سرخی = (acpi_madt_سرخی_t *)((حرف *)acpi_madt_سرخی + acpi_madt_سرخی->لمائی);
            }
        }
    }

    لکھو("ACPI: %d CPU(s) detected\n", عدد_cpu);

    أرجع عدد_cpu;
}

BOOT_کوڈ bool_t acpi_fadt_جانچ(
    acpi_rsdp_t *acpi_rsdp
)
{
    بنانشانی صحيح entries;
    uint32_t            گنتی;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتہ, ACPI_RSDT);

    أكد(acpi_rsdt_mapped->سرخی.لمائی >= حجم(acpi_سرخی_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرخی.لمائی - حجم(acpi_سرخی_t)) / حجم(uint32_t);
    لكل (گنتی = 0; گنتی < entries; گنتی++) {
        acpi_fadt = (acpi_fadt_t *)(لفظ_t)acpi_rsdt_mapped->اندراج[گنتی];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_جدول_init(acpi_fadt, ACPI_RSDT);

        إذا (لڑی_مقابلہ(acpi_str_fadt, acpi_fadt_mapped->سرخی.signature, 4) == 0) {
            لکھو("ACPI: FADT paddr=%p\n", acpi_fadt);
            لکھو("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            لکھو("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            إذا (config_سیٹ(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & ثنائی_ہندسہ(19)) {
                لکھو("system requires apic physical mode\n");
                أرجع false;
            }
        }
    }

    أرجع true;
}

BOOT_کوڈ فراغ acpi_dmar_جانچ(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *عدد_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    لفظ_t i;
    بنانشانی صحيح entries;
    uint32_t گنتی;
    uint32_t reg_basel, reg_baseh;
    صحيح rmrr_گنتی;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_سرخی_t   *acpi_dmar_سرخی;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(لفظ_t)acpi_rsdp->rsdt_پتہ, ACPI_RSDT);

    *عدد_drhu = 0;
    rmrr_گنتی = 0;

    أكد(acpi_rsdt_mapped->سرخی.لمائی >= حجم(acpi_سرخی_t));
    entries = (acpi_rsdt_mapped->سرخی.لمائی - حجم(acpi_سرخی_t)) / حجم(uint32_t);
    لكل (گنتی = 0; گنتی < entries; گنتی++) {
        acpi_dmar = (acpi_dmar_t *)(لفظ_t)acpi_rsdt_mapped->اندراج[گنتی];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_جدول_init(acpi_dmar, ACPI_RSDT);

        إذا (لڑی_مقابلہ(acpi_str_dmar, acpi_dmar_mapped->سرخی.signature, 4) == 0) {
            لکھو("ACPI: DMAR paddr=%p\n", acpi_dmar);
            لکھو("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            لکھو("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_چوڑائی + 1);
            acpi_dmar_سرخی = (acpi_dmar_سرخی_t *)(acpi_dmar_mapped + 1);

            طالما ((حرف *)acpi_dmar_سرخی < (حرف *)acpi_dmar_mapped + acpi_dmar_mapped->سرخی.لمائی) {
                بدل (acpi_dmar_سرخی->type) {

                حالة DMAR_DRHD:
                    إذا (*عدد_drhu == max_drhu_list_len) {
                        لکھو("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *عدد_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_سرخی)->reg_ادھار[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_سرخی)->reg_ادھار[1];
                    /* check if value fits into uint32_t */
                    إذا (reg_baseh != 0) {
                        لکھو("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *عدد_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    drhu_list[*عدد_drhu] = (paddr_t)reg_basel;
                    (*عدد_drhu)++;
                    اكسر;

                حالة DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_سرخی;
                    إذا (acpi_dmar_rmrr->reg_ادھار[1] != 0 ||
                        acpi_dmar_rmrr->reg_حد_بندی[1] != 0) {
                        لکھو("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *عدد_drhu = 0;
                        أرجع ;
                    }

                    لكل (i = 0; i <= (acpi_dmar_سرخی->لمائی - حجم(acpi_dmar_rmrr_t)) / حجم(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        إذا (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            لکھو("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *عدد_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        إذا (acpi_dmar_devscope->لمائی > حجم(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            لکھو("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *عدد_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->شروع_bus,
                                acpi_dmar_devscope->رستہ_0.dev,
                                acpi_dmar_devscope->رستہ_0.fun
                            );

                        إذا (rmrr_گنتی == CONFIG_MAX_RMRR_ENTRIES) {
                            لکھو("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *عدد_drhu = 0;
                            أرجع;
                        }
                        لکھو("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->شروع_bus,
                               acpi_dmar_devscope->رستہ_0.dev,
                               acpi_dmar_devscope->رستہ_0.fun
                              );

                        rmrr_list->entries[rmrr_گنتی].device = dev_id;
                        rmrr_list->entries[rmrr_گنتی].ادھار = acpi_dmar_rmrr->reg_ادھار[0];
                        rmrr_list->entries[rmrr_گنتی].حد_بندی = acpi_dmar_rmrr->reg_حد_بندی[0];
                        rmrr_گنتی++;
                    }
                    اكسر;

                حالة DMAR_ATSR:
                    /* not implemented yet */
                    اكسر;

                افتراضي:
                    لکھو("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_سرخی->type);
                }
                acpi_dmar_سرخی = (acpi_dmar_سرخی_t *)((حرف *)acpi_dmar_سرخی + acpi_dmar_سرخی->لمائی);
            }
        }
    }
    rmrr_list->عدد = rmrr_گنتی;
    لکھو("ACPI: %d IOMMUs detected\n", *عدد_drhu);
}
