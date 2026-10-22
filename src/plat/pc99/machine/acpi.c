/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ਅਨ੍ਤਰ੍ਭਾਵ <config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <util.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <assert.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <machine/io.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <linker.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/acpi.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <plat/machine/devices.h>

ਗਿਣਤੀ acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_dmar {
    acpi_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint8_t       host_addr_ਚੌੜਾਈ;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               ਆਕਾਰ(acpi_dmar_t) == ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t) + 12)

/* DMA Remapping Structure Header */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_dmar_ਸਿਰਲੇਖ {
    uint16_t type;
    uint16_t ਲੰਮਾਈ;
} acpi_dmar_ਸਿਰਲੇਖ_t;
compile_assert(acpi_dmar_ਸਿਰਲੇਖ_packed, ਆਕਾਰ(acpi_dmar_ਸਿਰਲੇਖ_t) == 4)

/* DMA Remapping Structure Types */
ਗਿਣਤੀ acpi_ਸਾਰਣੀ_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_dmar_drhd {
    acpi_dmar_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_ਅਧਾਰ[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               ਆਕਾਰ(acpi_dmar_drhd_t) == ਆਕਾਰ(acpi_dmar_ਸਿਰਲੇਖ_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  ਲੰਮਾਈ;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  ਸ਼ੁਰੂ_bus;
    ਬਣਤਰ {
        uint8_t dev;
        uint8_t fun;
    } ਰਸਤਾ_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, ਆਕਾਰ(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_dmar_rmrr {
    acpi_dmar_ਸਿਰਲੇਖ_t   ਸਿਰਲੇਖ;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_ਅਧਾਰ[2];
    uint32_t             reg_ਹੱਦਬੰਦੀ[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, ਆਕਾਰ(acpi_dmar_rmrr_t) ==
               ਆਕਾਰ(acpi_dmar_ਸਿਰਲੇਖ_t) + 20 + ਆਕਾਰ(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_fadt {
    acpi_ਸਿਰਲੇਖ_t  ਸਿਰਲੇਖ;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               ਆਕਾਰ(acpi_fadt_t) == ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t) + 80)

/* Multiple APIC Description Table (MADT) */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt {
    acpi_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               ਆਕਾਰ(acpi_madt_t) == ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t) + 8)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt_ਸਿਰਲੇਖ {
    uint8_t type;
    uint8_t ਲੰਮਾਈ;
} acpi_madt_ਸਿਰਲੇਖ_t;
compile_assert(acpi_madt_ਸਿਰਲੇਖ_packed, ਆਕਾਰ(acpi_madt_ਸਿਰਲੇਖ_t) == 2)

ਗਿਣਤੀ acpi_ਸਾਰਣੀ_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt_apic {
    acpi_madt_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               ਆਕਾਰ(acpi_madt_apic_t) == ਆਕਾਰ(acpi_madt_ਸਿਰਲੇਖ_t) + 6)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt_x2apic {
    acpi_madt_ਸਿਰਲੇਖ_t  ਸਿਰਲੇਖ;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               ਆਕਾਰ(acpi_madt_x2apic_t) == ਆਕਾਰ(acpi_madt_ਸਿਰਲੇਖ_t) + 14)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt_ioapic {
    acpi_madt_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               ਆਕਾਰ(acpi_madt_ioapic_t) == ਆਕਾਰ(acpi_madt_ਸਿਰਲੇਖ_t) + 10)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ acpi_madt_iso {
    acpi_madt_ਸਿਰਲੇਖ_t ਸਿਰਲੇਖ;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            ਸਰੋਤ;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_ਨੀਵਾਂ;
    uint16_t           gsi_ਉੱਚਾ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               ਆਕਾਰ(acpi_madt_iso_t) == ਆਕਾਰ(acpi_madt_ਸਿਰਲੇਖ_t) + 8)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ ਆਕਾਰ(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_ਇੰਦਰਾਜ_ਮਾਪ, ਆਕਾਰ(acpi_madt_apic_t) <= MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ)
compile_assert(acpi_madt_ioapic_ਇੰਦਰਾਜ_ਮਾਪ, ਆਕਾਰ(acpi_madt_ioapic_t) <= MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ)
compile_assert(acpi_madt_iso_ਇੰਦਰਾਜ_ਮਾਪ, ਆਕਾਰ(acpi_madt_iso_t) <= MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ)

/* workaround because string literals are not supported by C parser */
ਅਚਲ ਅੱਖਰ acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ਅਚਲ ਅੱਖਰ acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ਅਚਲ ਅੱਖਰ acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ਅਚਲ ਅੱਖਰ acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ਅਚਲ ਅੱਖਰ madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ਅਚਲ ਅੱਖਰ madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ਅਚਲ ਅੱਖਰ madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ਅਚਲ ਅੱਖਰ madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_ਅੰਕੜੇ ਅੱਖਰ madt_ਇੰਦਰਾਜ[MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ] ALIGN(4);

BOOT_ਕੋਡ ਸਥਿਰ uint8_t acpi_calc_checksum(ਅੱਖਰ *ਸ਼ੁਰੂ, uint32_t ਲੰਮਾਈ)
{
    uint8_t checksum = 0;

    ਜਦੋਂ (ਲੰਮਾਈ > 0) {
        checksum += *ਸ਼ੁਰੂ;
        ਸ਼ੁਰੂ++;
        ਲੰਮਾਈ--;
    }
    ਵਾਪਸ checksum;
}

BOOT_ਕੋਡ ਸਥਿਰ acpi_rsdp_t *acpi_get_rsdp(ਖਾਲੀ)
{
    ਅੱਖਰ *addr;

    ਲਈ (addr = (ਅੱਖਰ *)BIOS_PADDR_ਸ਼ੁਰੂ; addr < (ਅੱਖਰ *)BIOS_PADDR_ਸਿਰਾ; addr += 16) {
        ਜੇ (ਲੜੀ_ਤੁਲਨਾ(addr, acpi_str_rsd, 8) == 0) {
            ਜੇ (acpi_calc_checksum(addr, ACPI_V1_ਮਾਪ) == 0) {
                ਵਾਪਸ (acpi_rsdp_t *)addr;
            }
        }
    }
    ਵਾਪਸ ਨਿਰਾ;
}

BOOT_ਕੋਡ ਸਥਿਰ ਖਾਲੀ *acpi_ਸਾਰਣੀ_init(ਖਾਲੀ *ਇੰਦਰਾਜ, ਗਿਣਤੀ acpi_type ਸਾਰਣੀ_type)
{
    ਖਾਲੀ *acpi_ਸਾਰਣੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪੰਨੇ_for_ਸਾਰਣੀ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ ਪੰਨੇ_for_ਸਿਰਲੇਖ = 1;

    /* if we need to map another page to read header */
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)ਇੰਦਰਾਜ & ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ);
    ਜੇ (ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ) - ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ < ਆਕਾਰ(acpi_rsdp_t)) {
        ਪੰਨੇ_for_ਸਿਰਲੇਖ++;
    }

    /* map in table's header */
    acpi_ਸਾਰਣੀ = map_ਆਰਜ਼ੀ_boot_ਪੰਨਾ(ਇੰਦਰਾਜ, ਪੰਨੇ_for_ਸਿਰਲੇਖ);

    ਚੋਣ (ਸਾਰਣੀ_type) {
    ਹਾਲਤ ACPI_RSDP: {
        acpi_rsdp_t *rsdp_ਇੰਦਰਾਜ = (acpi_rsdp_t *)ਇੰਦਰਾਜ;
        ਪੰਨੇ_for_ਸਾਰਣੀ = (rsdp_ਇੰਦਰਾਜ->ਲੰਮਾਈ + ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ) / ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ) + 1;
        ਰੋਕੋ;
    }
    ਹਾਲਤ ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_ਇੰਦਰਾਜ = (acpi_rsdt_t *)ਇੰਦਰਾਜ;
        ਪੰਨੇ_for_ਸਾਰਣੀ = (rsdt_ਇੰਦਰਾਜ->ਸਿਰਲੇਖ.ਲੰਮਾਈ + ਵਿੱਥ_ਵਿੱਚ_ਪੰਨਾ) / ਨਕਾਬ(LARGE_ਪੰਨਾ_ਬਿੱਟ) + 1;
        ਰੋਕੋ;
    }
    ਮੂਲ:
        ਲਿਖੋ("Error: Mapping unknown ACPI table type\n");
        ਪ੍ਰਤਿਜ੍ਞਾ(false);
        ਵਾਪਸ ਨਿਰਾ;
    }

    /* map in full table */
    acpi_ਸਾਰਣੀ = map_ਆਰਜ਼ੀ_boot_ਪੰਨਾ(ਇੰਦਰਾਜ, ਪੰਨੇ_for_ਸਾਰਣੀ);

    ਵਾਪਸ acpi_ਸਾਰਣੀ;
}

BOOT_ਕੋਡ bool_t acpi_init(acpi_rsdp_t *rsdp_ਅੰਕੜੇ)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    ਜੇ (acpi_rsdp == ਨਿਰਾ) {
        ਲਿਖੋ("BIOS: No ACPI support detected\n");
        ਵਾਪਸ false;
    }
    ਲਿਖੋ("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_ਸਾਰਣੀ_init(acpi_rsdp, ACPI_RSDP);
    ਲਿਖੋ("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_ਅੰਕੜੇ = *acpi_rsdp;

    /* perform final validation */
    ਵਾਪਸ acpi_validate_rsdp(rsdp_ਅੰਕੜੇ);
}

BOOT_ਕੋਡ bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    ਜੇ (acpi_calc_checksum((ਅੱਖਰ *)acpi_rsdp, ACPI_V1_ਮਾਪ) != 0) {
        ਲਿਖੋ("BIOS: ACPIv1 information corrupt\n");
        ਵਾਪਸ false;
    }

    ਜੇ (acpi_rsdp->revision > 0 && acpi_calc_checksum((ਅੱਖਰ *)acpi_rsdp, ਆਕਾਰ(*acpi_rsdp)) != 0) {
        ਲਿਖੋ("BIOS: ACPIv2 information corrupt\n");
        ਵਾਪਸ false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(ਸ਼ਬਦ_t)acpi_rsdp->rsdt_ਪਤਾ;
    ਲਿਖੋ("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ਸਾਰਣੀ_init(acpi_rsdt, ACPI_RSDT);
    ਲਿਖੋ("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    ਪ੍ਰਤਿਜ੍ਞਾ(acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ > 0);
    ਜੇ (acpi_calc_checksum((ਅੱਖਰ *)acpi_rsdt_mapped, acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ) != 0) {
        ਲਿਖੋ("ACPI: RSDT checksum failure\n");
        ਵਾਪਸ false;
    }

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਸਥਿਰ bool_t acpi_madt_valid(acpi_madt_ਸਿਰਲੇਖ_t *ਸਿਰਲੇਖ, ਅਚਲ ਅੱਖਰ *name, uint8_t expected)
{
    ਜੇ (ਸਿਰਲੇਖ->ਲੰਮਾਈ < expected) {
        ਲਿਖੋ("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, ਸਿਰਲੇਖ->ਲੰਮਾਈ, expected);
        ਵਾਪਸ false;
    }
    ਵਾਪਸ true;
}

BOOT_ਕੋਡ uint32_t acpi_madt_ਜਾਂਚ(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *ਅੰਕ_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ        entries;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ        ਇੰਦਰਾਜ_ਮਾਪ;
    uint32_t            ਅੰਕ_cpu;
    uint32_t            ਗਿਣਤੀ_2;
    acpi_madt_t        *acpi_madt;
    acpi_madt_ਸਿਰਲੇਖ_t *acpi_madt_ਸਿਰਲੇਖ;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ਸਾਰਣੀ_init((acpi_rsdt_t *)(ਸ਼ਬਦ_t)acpi_rsdp->rsdt_ਪਤਾ, ACPI_RSDT);

    ਅੰਕ_cpu = 0;
    *ਅੰਕ_ioapic = 0;

    ਪ੍ਰਤਿਜ੍ਞਾ(acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ >= ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ - ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t)) / ਆਕਾਰ(uint32_t);
    ਲਈ (ਗਿਣਤੀ_2 = 0; ਗਿਣਤੀ_2 < entries; ਗਿਣਤੀ_2++) {
        acpi_madt = (acpi_madt_t *)(ਸ਼ਬਦ_t)acpi_rsdt_mapped->ਇੰਦਰਾਜ[ਗਿਣਤੀ_2];
        acpi_madt_mapped = (acpi_madt_t *)acpi_ਸਾਰਣੀ_init(acpi_madt, ACPI_RSDT);

        ਜੇ (ਲੜੀ_ਤੁਲਨਾ(acpi_str_apic, acpi_madt_mapped->ਸਿਰਲੇਖ.signature, 4) == 0) {
            ਲਿਖੋ("ACPI: MADT paddr=%p\n", acpi_madt);
            ਲਿਖੋ("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            ਲਿਖੋ("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            ਲਿਖੋ("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_ਸਿਰਲੇਖ = (acpi_madt_ਸਿਰਲੇਖ_t *)(acpi_madt_mapped + 1);

            ਜਦੋਂ ((ਅੱਖਰ *)acpi_madt_ਸਿਰਲੇਖ < (ਅੱਖਰ *)acpi_madt_mapped + acpi_madt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ) {
                ਇੰਦਰਾਜ_ਮਾਪ = MIN(acpi_madt_ਸਿਰਲੇਖ->ਲੰਮਾਈ, MADT_MAX_ਇੰਦਰਾਜ_ਮਾਪ);
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(madt_ਇੰਦਰਾਜ, acpi_madt_ਸਿਰਲੇਖ, ਇੰਦਰਾਜ_ਮਾਪ);

                ਚੋਣ (acpi_madt_ਸਿਰਲੇਖ->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                ਹਾਲਤ MADT_APIC: {
                    ਜੇ (!acpi_madt_valid(acpi_madt_ਸਿਰਲੇਖ, madt_str_apic, ਆਕਾਰ(acpi_madt_apic_t))) {
                        ਰੋਕੋ;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_ਇੰਦਰਾਜ;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    ਜੇ (flags == 1) {
                        ਲਿਖੋ("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        ਜੇ (ਅੰਕ_cpu == CONFIG_MAX_ਅੰਕ_NODES) {
                            ਲਿਖੋ("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ਅੰਕ_NODES);
                        } ਨਹੀਂਤਾਂ {
                            cpu_list[ਅੰਕ_cpu] = cpu_id;
                            ਅੰਕ_cpu++;
                        }
                    }
                    ਰੋਕੋ;
                }
                ਹਾਲਤ MADT_x2APIC: {
                    ਜੇ (!acpi_madt_valid(acpi_madt_ਸਿਰਲੇਖ, madt_str_x2apic, ਆਕਾਰ(acpi_madt_x2apic_t))) {
                        ਰੋਕੋ;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_ਇੰਦਰਾਜ;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    ਜੇ (flags == 1) {
                        ਲਿਖੋ("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        ਜੇ (ਅੰਕ_cpu == CONFIG_MAX_ਅੰਕ_NODES) {
                            ਲਿਖੋ("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ਅੰਕ_NODES);
                        } ਨਹੀਂਤਾਂ {
                            cpu_list[ਅੰਕ_cpu] = cpu_id;
                            ਅੰਕ_cpu++;
                        }
                    }
                    ਰੋਕੋ;
                }
                ਹਾਲਤ MADT_IOAPIC:
                    ਜੇ (!acpi_madt_valid(acpi_madt_ਸਿਰਲੇਖ, madt_str_ioapic, ਆਕਾਰ(acpi_madt_ioapic_t))) {
                        ਰੋਕੋ;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_ਇੰਦਰਾਜ;

                    ਲਿਖੋ(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    ਜੇ (*ਅੰਕ_ioapic == CONFIG_MAX_ਅੰਕ_IOAPIC) {
                        ਲਿਖੋ("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_ਅੰਕ_IOAPIC);
                    } ਨਹੀਂਤਾਂ {
                        ioapic_paddrs[*ਅੰਕ_ioapic] = madt_ioapic->ioapic_addr;
                        (*ਅੰਕ_ioapic)++;
                    }
                    ਰੋਕੋ;
                ਹਾਲਤ MADT_ISO: {
                    ਜੇ (!acpi_madt_valid(acpi_madt_ਸਿਰਲੇਖ, madt_str_iso, ਆਕਾਰ(acpi_madt_iso_t))) {
                        ਰੋਕੋ;
                    }
                    ਅਣਵਰਤਿਆ acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_ਇੰਦਰਾਜ;

                    ਲਿਖੋ("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->ਸਰੋਤ, iso->gsi_ਉੱਚਾ, iso->gsi_ਨੀਵਾਂ, iso->flags);
                    ਰੋਕੋ;
                }
                ਮੂਲ:
                    ਰੋਕੋ;
                }
                acpi_madt_ਸਿਰਲੇਖ = (acpi_madt_ਸਿਰਲੇਖ_t *)((ਅੱਖਰ *)acpi_madt_ਸਿਰਲੇਖ + acpi_madt_ਸਿਰਲੇਖ->ਲੰਮਾਈ);
            }
        }
    }

    ਲਿਖੋ("ACPI: %d CPU(s) detected\n", ਅੰਕ_cpu);

    ਵਾਪਸ ਅੰਕ_cpu;
}

BOOT_ਕੋਡ bool_t acpi_fadt_ਜਾਂਚ(
    acpi_rsdp_t *acpi_rsdp
)
{
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ entries;
    uint32_t            ਗਿਣਤੀ_2;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ਸਾਰਣੀ_init((acpi_rsdt_t *)(ਸ਼ਬਦ_t)acpi_rsdp->rsdt_ਪਤਾ, ACPI_RSDT);

    ਪ੍ਰਤਿਜ੍ਞਾ(acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ >= ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ - ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t)) / ਆਕਾਰ(uint32_t);
    ਲਈ (ਗਿਣਤੀ_2 = 0; ਗਿਣਤੀ_2 < entries; ਗਿਣਤੀ_2++) {
        acpi_fadt = (acpi_fadt_t *)(ਸ਼ਬਦ_t)acpi_rsdt_mapped->ਇੰਦਰਾਜ[ਗਿਣਤੀ_2];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_ਸਾਰਣੀ_init(acpi_fadt, ACPI_RSDT);

        ਜੇ (ਲੜੀ_ਤੁਲਨਾ(acpi_str_fadt, acpi_fadt_mapped->ਸਿਰਲੇਖ.signature, 4) == 0) {
            ਲਿਖੋ("ACPI: FADT paddr=%p\n", acpi_fadt);
            ਲਿਖੋ("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            ਲਿਖੋ("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            ਜੇ (config_ਸੈੱਟ(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & ਦੋਆਧਾਰੀ_ਅੰਕ(19)) {
                ਲਿਖੋ("system requires apic physical mode\n");
                ਵਾਪਸ false;
            }
        }
    }

    ਵਾਪਸ true;
}

BOOT_ਕੋਡ ਖਾਲੀ acpi_dmar_ਜਾਂਚ(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *ਅੰਕ_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    ਸ਼ਬਦ_t i;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ entries;
    uint32_t ਗਿਣਤੀ_2;
    uint32_t reg_basel, reg_baseh;
    ਪੂਰਨ rmrr_ਗਿਣਤੀ;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_ਸਿਰਲੇਖ_t   *acpi_dmar_ਸਿਰਲੇਖ;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ਸਾਰਣੀ_init((acpi_rsdt_t *)(ਸ਼ਬਦ_t)acpi_rsdp->rsdt_ਪਤਾ, ACPI_RSDT);

    *ਅੰਕ_drhu = 0;
    rmrr_ਗਿਣਤੀ = 0;

    ਪ੍ਰਤਿਜ੍ਞਾ(acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ >= ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t));
    entries = (acpi_rsdt_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ - ਆਕਾਰ(acpi_ਸਿਰਲੇਖ_t)) / ਆਕਾਰ(uint32_t);
    ਲਈ (ਗਿਣਤੀ_2 = 0; ਗਿਣਤੀ_2 < entries; ਗਿਣਤੀ_2++) {
        acpi_dmar = (acpi_dmar_t *)(ਸ਼ਬਦ_t)acpi_rsdt_mapped->ਇੰਦਰਾਜ[ਗਿਣਤੀ_2];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_ਸਾਰਣੀ_init(acpi_dmar, ACPI_RSDT);

        ਜੇ (ਲੜੀ_ਤੁਲਨਾ(acpi_str_dmar, acpi_dmar_mapped->ਸਿਰਲੇਖ.signature, 4) == 0) {
            ਲਿਖੋ("ACPI: DMAR paddr=%p\n", acpi_dmar);
            ਲਿਖੋ("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            ਲਿਖੋ("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_ਚੌੜਾਈ + 1);
            acpi_dmar_ਸਿਰਲੇਖ = (acpi_dmar_ਸਿਰਲੇਖ_t *)(acpi_dmar_mapped + 1);

            ਜਦੋਂ ((ਅੱਖਰ *)acpi_dmar_ਸਿਰਲੇਖ < (ਅੱਖਰ *)acpi_dmar_mapped + acpi_dmar_mapped->ਸਿਰਲੇਖ.ਲੰਮਾਈ) {
                ਚੋਣ (acpi_dmar_ਸਿਰਲੇਖ->type) {

                ਹਾਲਤ DMAR_DRHD:
                    ਜੇ (*ਅੰਕ_drhu == max_drhu_list_len) {
                        ਲਿਖੋ("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *ਅੰਕ_drhu = 0; /* report zero IOMMUs */
                        ਵਾਪਸ;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_ਸਿਰਲੇਖ)->reg_ਅਧਾਰ[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_ਸਿਰਲੇਖ)->reg_ਅਧਾਰ[1];
                    /* check if value fits into uint32_t */
                    ਜੇ (reg_baseh != 0) {
                        ਲਿਖੋ("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *ਅੰਕ_drhu = 0; /* report zero IOMMUs */
                        ਵਾਪਸ;
                    }
                    drhu_list[*ਅੰਕ_drhu] = (paddr_t)reg_basel;
                    (*ਅੰਕ_drhu)++;
                    ਰੋਕੋ;

                ਹਾਲਤ DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_ਸਿਰਲੇਖ;
                    ਜੇ (acpi_dmar_rmrr->reg_ਅਧਾਰ[1] != 0 ||
                        acpi_dmar_rmrr->reg_ਹੱਦਬੰਦੀ[1] != 0) {
                        ਲਿਖੋ("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *ਅੰਕ_drhu = 0;
                        ਵਾਪਸ ;
                    }

                    ਲਈ (i = 0; i <= (acpi_dmar_ਸਿਰਲੇਖ->ਲੰਮਾਈ - ਆਕਾਰ(acpi_dmar_rmrr_t)) / ਆਕਾਰ(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        ਜੇ (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            ਲਿਖੋ("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *ਅੰਕ_drhu = 0; /* report zero IOMMUs */
                            ਵਾਪਸ;
                        }

                        ਜੇ (acpi_dmar_devscope->ਲੰਮਾਈ > ਆਕਾਰ(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            ਲਿਖੋ("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *ਅੰਕ_drhu = 0; /* report zero IOMMUs */
                            ਵਾਪਸ;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->ਸ਼ੁਰੂ_bus,
                                acpi_dmar_devscope->ਰਸਤਾ_0.dev,
                                acpi_dmar_devscope->ਰਸਤਾ_0.fun
                            );

                        ਜੇ (rmrr_ਗਿਣਤੀ == CONFIG_MAX_RMRR_ENTRIES) {
                            ਲਿਖੋ("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *ਅੰਕ_drhu = 0;
                            ਵਾਪਸ;
                        }
                        ਲਿਖੋ("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->ਸ਼ੁਰੂ_bus,
                               acpi_dmar_devscope->ਰਸਤਾ_0.dev,
                               acpi_dmar_devscope->ਰਸਤਾ_0.fun
                              );

                        rmrr_list->entries[rmrr_ਗਿਣਤੀ].device = dev_id;
                        rmrr_list->entries[rmrr_ਗਿਣਤੀ].ਅਧਾਰ = acpi_dmar_rmrr->reg_ਅਧਾਰ[0];
                        rmrr_list->entries[rmrr_ਗਿਣਤੀ].ਹੱਦਬੰਦੀ = acpi_dmar_rmrr->reg_ਹੱਦਬੰਦੀ[0];
                        rmrr_ਗਿਣਤੀ++;
                    }
                    ਰੋਕੋ;

                ਹਾਲਤ DMAR_ATSR:
                    /* not implemented yet */
                    ਰੋਕੋ;

                ਮੂਲ:
                    ਲਿਖੋ("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_ਸਿਰਲੇਖ->type);
                }
                acpi_dmar_ਸਿਰਲੇਖ = (acpi_dmar_ਸਿਰਲੇਖ_t *)((ਅੱਖਰ *)acpi_dmar_ਸਿਰਲੇਖ + acpi_dmar_ਸਿਰਲੇਖ->ਲੰਮਾਈ);
            }
        }
    }
    rmrr_list->ਅੰਕ = rmrr_ਗਿਣਤੀ;
    ਲਿਖੋ("ACPI: %d IOMMUs detected\n", *ਅੰਕ_drhu);
}
