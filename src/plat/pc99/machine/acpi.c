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
    acpi_سرلیک_t سرلیک;
    uint8_t       host_addr_پلنوالی;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               حجم(acpi_dmar_t) == حجم(acpi_سرلیک_t) + 12)

/* DMA Remapping Structure Header */
تعريفنوع بنية acpi_dmar_سرلیک {
    uint16_t type;
    uint16_t اوږدتیا;
} acpi_dmar_سرلیک_t;
compile_assert(acpi_dmar_سرلیک_packed, حجم(acpi_dmar_سرلیک_t) == 4)

/* DMA Remapping Structure Types */
تعداد acpi_جدول_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
تعريفنوع بنية acpi_dmar_drhd {
    acpi_dmar_سرلیک_t سرلیک;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_بنسټ[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               حجم(acpi_dmar_drhd_t) == حجم(acpi_dmar_سرلیک_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  اوږدتیا;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  پیل_bus;
    بنية {
        uint8_t dev;
        uint8_t fun;
    } لار_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, حجم(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
تعريفنوع بنية acpi_dmar_rmrr {
    acpi_dmar_سرلیک_t   سرلیک;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_بنسټ[2];
    uint32_t             reg_حد[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, حجم(acpi_dmar_rmrr_t) ==
               حجم(acpi_dmar_سرلیک_t) + 20 + حجم(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
تعريفنوع بنية acpi_fadt {
    acpi_سرلیک_t  سرلیک;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               حجم(acpi_fadt_t) == حجم(acpi_سرلیک_t) + 80)

/* Multiple APIC Description Table (MADT) */
تعريفنوع بنية acpi_madt {
    acpi_سرلیک_t سرلیک;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               حجم(acpi_madt_t) == حجم(acpi_سرلیک_t) + 8)

تعريفنوع بنية acpi_madt_سرلیک {
    uint8_t type;
    uint8_t اوږدتیا;
} acpi_madt_سرلیک_t;
compile_assert(acpi_madt_سرلیک_packed, حجم(acpi_madt_سرلیک_t) == 2)

تعداد acpi_جدول_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

تعريفنوع بنية acpi_madt_apic {
    acpi_madt_سرلیک_t سرلیک;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               حجم(acpi_madt_apic_t) == حجم(acpi_madt_سرلیک_t) + 6)

تعريفنوع بنية acpi_madt_x2apic {
    acpi_madt_سرلیک_t  سرلیک;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               حجم(acpi_madt_x2apic_t) == حجم(acpi_madt_سرلیک_t) + 14)

تعريفنوع بنية acpi_madt_ioapic {
    acpi_madt_سرلیک_t سرلیک;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               حجم(acpi_madt_ioapic_t) == حجم(acpi_madt_سرلیک_t) + 10)

تعريفنوع بنية acpi_madt_iso {
    acpi_madt_سرلیک_t سرلیک;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            سرچینه;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_ټیټ;
    uint16_t           gsi_لوړ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               حجم(acpi_madt_iso_t) == حجم(acpi_madt_سرلیک_t) + 8)

#تعریف_کړه MADT_MAX_ثبت_اندازه حجم(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_ثبت_اندازه, حجم(acpi_madt_apic_t) <= MADT_MAX_ثبت_اندازه)
compile_assert(acpi_madt_ioapic_ثبت_اندازه, حجم(acpi_madt_ioapic_t) <= MADT_MAX_ثبت_اندازه)
compile_assert(acpi_madt_iso_ثبت_اندازه, حجم(acpi_madt_iso_t) <= MADT_MAX_ثبت_اندازه)

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
BOOT_معلومات حرف madt_ثبت[MADT_MAX_ثبت_اندازه] ALIGN(4);

BOOT_کوډ ساكن uint8_t acpi_calc_checksum(حرف *پیل, uint32_t اوږدتیا)
{
    uint8_t checksum = 0;

    طالما (اوږدتیا > 0) {
        checksum += *پیل;
        پیل++;
        اوږدتیا--;
    }
    أرجع checksum;
}

BOOT_کوډ ساكن acpi_rsdp_t *acpi_get_rsdp(فراغ)
{
    حرف *addr;

    لكل (addr = (حرف *)BIOS_PADDR_پیل; addr < (حرف *)BIOS_PADDR_پای_ټکی; addr += 16) {
        إذا (مزي_پرتله(addr, acpi_str_rsd, 8) == 0) {
            إذا (acpi_calc_checksum(addr, ACPI_V1_اندازه) == 0) {
                أرجع (acpi_rsdp_t *)addr;
            }
        }
    }
    أرجع عدم;
}

BOOT_کوډ ساكن فراغ *acpi_جدول_init(فراغ *ثبت, تعداد acpi_type جدول_type)
{
    فراغ *acpi_جدول;
    بینښه صحيح پاڼې_for_جدول;
    بینښه صحيح پاڼې_for_سرلیک = 1;

    /* if we need to map another page to read header */
    بینښه اوږد دننه_واټن_دننه_پاڼه = (بینښه اوږد)ثبت & ماسک(LARGE_پاڼه_بټونه);
    إذا (ماسک(LARGE_پاڼه_بټونه) - دننه_واټن_دننه_پاڼه < حجم(acpi_rsdp_t)) {
        پاڼې_for_سرلیک++;
    }

    /* map in table's header */
    acpi_جدول = map_لنډمهاله_boot_پاڼه(ثبت, پاڼې_for_سرلیک);

    بدل (جدول_type) {
    حالة ACPI_RSDP: {
        acpi_rsdp_t *rsdp_ثبت = (acpi_rsdp_t *)ثبت;
        پاڼې_for_جدول = (rsdp_ثبت->اوږدتیا + دننه_واټن_دننه_پاڼه) / ماسک(LARGE_پاڼه_بټونه) + 1;
        اكسر;
    }
    حالة ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_ثبت = (acpi_rsdt_t *)ثبت;
        پاڼې_for_جدول = (rsdt_ثبت->سرلیک.اوږدتیا + دننه_واټن_دننه_پاڼه) / ماسک(LARGE_پاڼه_بټونه) + 1;
        اكسر;
    }
    افتراضي:
        چاپ_کړه("Error: Mapping unknown ACPI table type\n");
        أكد(false);
        أرجع عدم;
    }

    /* map in full table */
    acpi_جدول = map_لنډمهاله_boot_پاڼه(ثبت, پاڼې_for_جدول);

    أرجع acpi_جدول;
}

BOOT_کوډ bool_t acpi_init(acpi_rsdp_t *rsdp_معلومات)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    إذا (acpi_rsdp == عدم) {
        چاپ_کړه("BIOS: No ACPI support detected\n");
        أرجع false;
    }
    چاپ_کړه("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_جدول_init(acpi_rsdp, ACPI_RSDP);
    چاپ_کړه("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_معلومات = *acpi_rsdp;

    /* perform final validation */
    أرجع acpi_validate_rsdp(rsdp_معلومات);
}

BOOT_کوډ bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    إذا (acpi_calc_checksum((حرف *)acpi_rsdp, ACPI_V1_اندازه) != 0) {
        چاپ_کړه("BIOS: ACPIv1 information corrupt\n");
        أرجع false;
    }

    إذا (acpi_rsdp->revision > 0 && acpi_calc_checksum((حرف *)acpi_rsdp, حجم(*acpi_rsdp)) != 0) {
        چاپ_کړه("BIOS: ACPIv2 information corrupt\n");
        أرجع false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_پته;
    چاپ_کړه("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init(acpi_rsdt, ACPI_RSDT);
    چاپ_کړه("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    أكد(acpi_rsdt_mapped->سرلیک.اوږدتیا > 0);
    إذا (acpi_calc_checksum((حرف *)acpi_rsdt_mapped, acpi_rsdt_mapped->سرلیک.اوږدتیا) != 0) {
        چاپ_کړه("ACPI: RSDT checksum failure\n");
        أرجع false;
    }

    أرجع true;
}

BOOT_کوډ ساكن bool_t acpi_madt_valid(acpi_madt_سرلیک_t *سرلیک, ثابت حرف *name, uint8_t expected)
{
    إذا (سرلیک->اوږدتیا < expected) {
        چاپ_کړه("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, سرلیک->اوږدتیا, expected);
        أرجع false;
    }
    أرجع true;
}

BOOT_کوډ uint32_t acpi_madt_لټون(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *عدد_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    بینښه صحيح        entries;
    بینښه صحيح        ثبت_اندازه;
    uint32_t            عدد_cpu;
    uint32_t            شمېر;
    acpi_madt_t        *acpi_madt;
    acpi_madt_سرلیک_t *acpi_madt_سرلیک;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_پته, ACPI_RSDT);

    عدد_cpu = 0;
    *عدد_ioapic = 0;

    أكد(acpi_rsdt_mapped->سرلیک.اوږدتیا >= حجم(acpi_سرلیک_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرلیک.اوږدتیا - حجم(acpi_سرلیک_t)) / حجم(uint32_t);
    لكل (شمېر = 0; شمېر < entries; شمېر++) {
        acpi_madt = (acpi_madt_t *)(کلمه_t)acpi_rsdt_mapped->ثبت[شمېر];
        acpi_madt_mapped = (acpi_madt_t *)acpi_جدول_init(acpi_madt, ACPI_RSDT);

        إذا (مزي_پرتله(acpi_str_apic, acpi_madt_mapped->سرلیک.signature, 4) == 0) {
            چاپ_کړه("ACPI: MADT paddr=%p\n", acpi_madt);
            چاپ_کړه("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            چاپ_کړه("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            چاپ_کړه("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_سرلیک = (acpi_madt_سرلیک_t *)(acpi_madt_mapped + 1);

            طالما ((حرف *)acpi_madt_سرلیک < (حرف *)acpi_madt_mapped + acpi_madt_mapped->سرلیک.اوږدتیا) {
                ثبت_اندازه = MIN(acpi_madt_سرلیک->اوږدتیا, MADT_MAX_ثبت_اندازه);
                حافظه_نقل(madt_ثبت, acpi_madt_سرلیک, ثبت_اندازه);

                بدل (acpi_madt_سرلیک->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                حالة MADT_APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرلیک, madt_str_apic, حجم(acpi_madt_apic_t))) {
                        اكسر;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_ثبت;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    إذا (flags == 1) {
                        چاپ_کړه("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        إذا (عدد_cpu == CONFIG_MAX_عدد_NODES) {
                            چاپ_کړه("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_عدد_NODES);
                        } وإلا {
                            cpu_list[عدد_cpu] = cpu_id;
                            عدد_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_x2APIC: {
                    إذا (!acpi_madt_valid(acpi_madt_سرلیک, madt_str_x2apic, حجم(acpi_madt_x2apic_t))) {
                        اكسر;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_ثبت;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    إذا (flags == 1) {
                        چاپ_کړه("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        إذا (عدد_cpu == CONFIG_MAX_عدد_NODES) {
                            چاپ_کړه("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_عدد_NODES);
                        } وإلا {
                            cpu_list[عدد_cpu] = cpu_id;
                            عدد_cpu++;
                        }
                    }
                    اكسر;
                }
                حالة MADT_IOAPIC:
                    إذا (!acpi_madt_valid(acpi_madt_سرلیک, madt_str_ioapic, حجم(acpi_madt_ioapic_t))) {
                        اكسر;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_ثبت;

                    چاپ_کړه(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    إذا (*عدد_ioapic == CONFIG_MAX_عدد_IOAPIC) {
                        چاپ_کړه("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_عدد_IOAPIC);
                    } وإلا {
                        ioapic_paddrs[*عدد_ioapic] = madt_ioapic->ioapic_addr;
                        (*عدد_ioapic)++;
                    }
                    اكسر;
                حالة MADT_ISO: {
                    إذا (!acpi_madt_valid(acpi_madt_سرلیک, madt_str_iso, حجم(acpi_madt_iso_t))) {
                        اكسر;
                    }
                    ناکارول_شوی acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_ثبت;

                    چاپ_کړه("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->سرچینه, iso->gsi_لوړ, iso->gsi_ټیټ, iso->flags);
                    اكسر;
                }
                افتراضي:
                    اكسر;
                }
                acpi_madt_سرلیک = (acpi_madt_سرلیک_t *)((حرف *)acpi_madt_سرلیک + acpi_madt_سرلیک->اوږدتیا);
            }
        }
    }

    چاپ_کړه("ACPI: %d CPU(s) detected\n", عدد_cpu);

    أرجع عدد_cpu;
}

BOOT_کوډ bool_t acpi_fadt_لټون(
    acpi_rsdp_t *acpi_rsdp
)
{
    بینښه صحيح entries;
    uint32_t            شمېر;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_پته, ACPI_RSDT);

    أكد(acpi_rsdt_mapped->سرلیک.اوږدتیا >= حجم(acpi_سرلیک_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->سرلیک.اوږدتیا - حجم(acpi_سرلیک_t)) / حجم(uint32_t);
    لكل (شمېر = 0; شمېر < entries; شمېر++) {
        acpi_fadt = (acpi_fadt_t *)(کلمه_t)acpi_rsdt_mapped->ثبت[شمېر];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_جدول_init(acpi_fadt, ACPI_RSDT);

        إذا (مزي_پرتله(acpi_str_fadt, acpi_fadt_mapped->سرلیک.signature, 4) == 0) {
            چاپ_کړه("ACPI: FADT paddr=%p\n", acpi_fadt);
            چاپ_کړه("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            چاپ_کړه("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            إذا (config_سېټ(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & بټ(19)) {
                چاپ_کړه("system requires apic physical mode\n");
                أرجع false;
            }
        }
    }

    أرجع true;
}

BOOT_کوډ فراغ acpi_dmar_لټون(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *عدد_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    کلمه_t i;
    بینښه صحيح entries;
    uint32_t شمېر;
    uint32_t reg_basel, reg_baseh;
    صحيح rmrr_شمېر;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_سرلیک_t   *acpi_dmar_سرلیک;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_جدول_init((acpi_rsdt_t *)(کلمه_t)acpi_rsdp->rsdt_پته, ACPI_RSDT);

    *عدد_drhu = 0;
    rmrr_شمېر = 0;

    أكد(acpi_rsdt_mapped->سرلیک.اوږدتیا >= حجم(acpi_سرلیک_t));
    entries = (acpi_rsdt_mapped->سرلیک.اوږدتیا - حجم(acpi_سرلیک_t)) / حجم(uint32_t);
    لكل (شمېر = 0; شمېر < entries; شمېر++) {
        acpi_dmar = (acpi_dmar_t *)(کلمه_t)acpi_rsdt_mapped->ثبت[شمېر];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_جدول_init(acpi_dmar, ACPI_RSDT);

        إذا (مزي_پرتله(acpi_str_dmar, acpi_dmar_mapped->سرلیک.signature, 4) == 0) {
            چاپ_کړه("ACPI: DMAR paddr=%p\n", acpi_dmar);
            چاپ_کړه("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            چاپ_کړه("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_پلنوالی + 1);
            acpi_dmar_سرلیک = (acpi_dmar_سرلیک_t *)(acpi_dmar_mapped + 1);

            طالما ((حرف *)acpi_dmar_سرلیک < (حرف *)acpi_dmar_mapped + acpi_dmar_mapped->سرلیک.اوږدتیا) {
                بدل (acpi_dmar_سرلیک->type) {

                حالة DMAR_DRHD:
                    إذا (*عدد_drhu == max_drhu_list_len) {
                        چاپ_کړه("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *عدد_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_سرلیک)->reg_بنسټ[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_سرلیک)->reg_بنسټ[1];
                    /* check if value fits into uint32_t */
                    إذا (reg_baseh != 0) {
                        چاپ_کړه("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *عدد_drhu = 0; /* report zero IOMMUs */
                        أرجع;
                    }
                    drhu_list[*عدد_drhu] = (paddr_t)reg_basel;
                    (*عدد_drhu)++;
                    اكسر;

                حالة DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_سرلیک;
                    إذا (acpi_dmar_rmrr->reg_بنسټ[1] != 0 ||
                        acpi_dmar_rmrr->reg_حد[1] != 0) {
                        چاپ_کړه("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *عدد_drhu = 0;
                        أرجع ;
                    }

                    لكل (i = 0; i <= (acpi_dmar_سرلیک->اوږدتیا - حجم(acpi_dmar_rmrr_t)) / حجم(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        إذا (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            چاپ_کړه("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *عدد_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        إذا (acpi_dmar_devscope->اوږدتیا > حجم(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            چاپ_کړه("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *عدد_drhu = 0; /* report zero IOMMUs */
                            أرجع;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->پیل_bus,
                                acpi_dmar_devscope->لار_0.dev,
                                acpi_dmar_devscope->لار_0.fun
                            );

                        إذا (rmrr_شمېر == CONFIG_MAX_RMRR_ENTRIES) {
                            چاپ_کړه("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *عدد_drhu = 0;
                            أرجع;
                        }
                        چاپ_کړه("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->پیل_bus,
                               acpi_dmar_devscope->لار_0.dev,
                               acpi_dmar_devscope->لار_0.fun
                              );

                        rmrr_list->entries[rmrr_شمېر].device = dev_id;
                        rmrr_list->entries[rmrr_شمېر].بنسټ = acpi_dmar_rmrr->reg_بنسټ[0];
                        rmrr_list->entries[rmrr_شمېر].حد = acpi_dmar_rmrr->reg_حد[0];
                        rmrr_شمېر++;
                    }
                    اكسر;

                حالة DMAR_ATSR:
                    /* not implemented yet */
                    اكسر;

                افتراضي:
                    چاپ_کړه("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_سرلیک->type);
                }
                acpi_dmar_سرلیک = (acpi_dmar_سرلیک_t *)((حرف *)acpi_dmar_سرلیک + acpi_dmar_سرلیک->اوږدتیا);
            }
        }
    }
    rmrr_list->عدد = rmrr_شمېر;
    چاپ_کړه("ACPI: %d IOMMUs detected\n", *عدد_drhu);
}
