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

ಎಣಿಕೆ acpi_type {
    ACPI_RSDP,
    ACPI_RSDT
};

/* DMA Remapping Reporting Table */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_dmar {
    acpi_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint8_t       host_addr_ಅಗಲ;
    uint8_t       flags;
    uint8_t       reserved[10];
} acpi_dmar_t;
compile_assert(acpi_dmar_packed,
               ಗಾತ್ರ(acpi_dmar_t) == ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t) + 12)

/* DMA Remapping Structure Header */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_dmar_ಶೀರ್ಷಿಕೆ {
    uint16_t type;
    uint16_t ಉದ್ದಳತೆ;
} acpi_dmar_ಶೀರ್ಷಿಕೆ_t;
compile_assert(acpi_dmar_ಶೀರ್ಷಿಕೆ_packed, ಗಾತ್ರ(acpi_dmar_ಶೀರ್ಷಿಕೆ_t) == 4)

/* DMA Remapping Structure Types */
ಎಣಿಕೆ acpi_ಕೋಷ್ಟಕ_dmar_struct_type {
    DMAR_DRHD = 0,
    DMAR_RMRR = 1,
    DMAR_ATSR = 2,
};

/* DMA Remapping Hardware unit Definition */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_dmar_drhd {
    acpi_dmar_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint8_t            flags;
    uint8_t            reserved;
    uint16_t           segment;
    uint32_t           reg_ತಳ[2];
} acpi_dmar_drhd_t;
compile_assert(acpi_dmar_drhd_packed,
               ಗಾತ್ರ(acpi_dmar_drhd_t) == ಗಾತ್ರ(acpi_dmar_ಶೀರ್ಷಿಕೆ_t) + 12)

/* Reserved Memory Region Reporting structure Definition */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_dmar_devscope {
    uint8_t  type;
    uint8_t  ಉದ್ದಳತೆ;
    uint16_t reserved;
    uint8_t  enum_id;
    uint8_t  ಆರಂಭ_bus;
    ರಚನೆ {
        uint8_t dev;
        uint8_t fun;
    } ಮಾರ್ಗ_0;
} acpi_dmar_devscope_t;
compile_assert(acpi_dmar_devscope_packed, ಗಾತ್ರ(acpi_dmar_devscope_t) == 8)

/* Reserved Memory Region Reporting structure Definition */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_dmar_rmrr {
    acpi_dmar_ಶೀರ್ಷಿಕೆ_t   ಶೀರ್ಷಿಕೆ;
    uint16_t             reserved;
    uint16_t             segment;
    uint32_t             reg_ತಳ[2];
    uint32_t             reg_ಎಲ್ಲೆ[2];
    acpi_dmar_devscope_t devscope_0;
} acpi_dmar_rmrr_t;
compile_assert(acpi_dmar_rmrr_packed, ಗಾತ್ರ(acpi_dmar_rmrr_t) ==
               ಗಾತ್ರ(acpi_dmar_ಶೀರ್ಷಿಕೆ_t) + 20 + ಗಾತ್ರ(acpi_dmar_devscope_t))

/* Fixed ACPI Description Table (FADT), partial as we only need flags */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_fadt {
    acpi_ಶೀರ್ಷಿಕೆ_t  ಶೀರ್ಷಿಕೆ;
    uint8_t        reserved[76];
    uint32_t       flags;
} acpi_fadt_t;
compile_assert(acpi_fadt_packed,
               ಗಾತ್ರ(acpi_fadt_t) == ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t) + 80)

/* Multiple APIC Description Table (MADT) */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt {
    acpi_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint32_t      apic_addr;
    uint32_t      flags;
} acpi_madt_t;
compile_assert(acpi_madt_packed,
               ಗಾತ್ರ(acpi_madt_t) == ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t) + 8)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt_ಶೀರ್ಷಿಕೆ {
    uint8_t type;
    uint8_t ಉದ್ದಳತೆ;
} acpi_madt_ಶೀರ್ಷಿಕೆ_t;
compile_assert(acpi_madt_ಶೀರ್ಷಿಕೆ_packed, ಗಾತ್ರ(acpi_madt_ಶೀರ್ಷಿಕೆ_t) == 2)

ಎಣಿಕೆ acpi_ಕೋಷ್ಟಕ_madt_struct_type {
    MADT_APIC   = 0,
    MADT_IOAPIC = 1,
    MADT_ISO    = 2,
    MADT_x2APIC = 9
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt_apic {
    acpi_madt_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint8_t            cpu_id;
    uint8_t            apic_id;
    uint32_t           flags;
} acpi_madt_apic_t;
compile_assert(acpi_madt_apic_packed,
               ಗಾತ್ರ(acpi_madt_apic_t) == ಗಾತ್ರ(acpi_madt_ಶೀರ್ಷಿಕೆ_t) + 6)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt_x2apic {
    acpi_madt_ಶೀರ್ಷಿಕೆ_t  ಶೀರ್ಷಿಕೆ;
    uint16_t            reserved;
    uint32_t            x2apic_id;
    uint32_t            flags;
    uint32_t            acpi_processor_uid;
} acpi_madt_x2apic_t;
compile_assert(acpi_madt_x2apic_packed,
               ಗಾತ್ರ(acpi_madt_x2apic_t) == ಗಾತ್ರ(acpi_madt_ಶೀರ್ಷಿಕೆ_t) + 14)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt_ioapic {
    acpi_madt_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint8_t            ioapic_id;
    uint8_t            reserved[1];
    uint32_t           ioapic_addr;
    uint32_t           gsib;
} acpi_madt_ioapic_t;
compile_assert(acpi_madt_ioapic_packed,
               ಗಾತ್ರ(acpi_madt_ioapic_t) == ಗಾತ್ರ(acpi_madt_ಶೀರ್ಷಿಕೆ_t) + 10)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ acpi_madt_iso {
    acpi_madt_ಶೀರ್ಷಿಕೆ_t ಶೀರ್ಷಿಕೆ;
    uint8_t            bus; /* always 0 (ISA) */
    uint8_t            ಮೂಲ;
    /* Split the u32 into 2x u16 to prevent the compiler from padding the struct. */
    uint16_t           gsi_ಕೆಳಗಿನ;
    uint16_t           gsi_ಮೇಲಿನ;
    uint16_t           flags;
} acpi_madt_iso_t;
compile_assert(acpi_madt_iso_packed,
               ಗಾತ್ರ(acpi_madt_iso_t) == ಗಾತ್ರ(acpi_madt_ಶೀರ್ಷಿಕೆ_t) + 8)

#ವ್ಯಾಖ್ಯಾನಿಸು MADT_MAX_ನಮೂದು_ಗಾತ್ರ ಗಾತ್ರ(acpi_madt_x2apic_t)

compile_assert(acpi_madt_apic_ನಮೂದು_ಗಾತ್ರ, ಗಾತ್ರ(acpi_madt_apic_t) <= MADT_MAX_ನಮೂದು_ಗಾತ್ರ)
compile_assert(acpi_madt_ioapic_ನಮೂದು_ಗಾತ್ರ, ಗಾತ್ರ(acpi_madt_ioapic_t) <= MADT_MAX_ನಮೂದು_ಗಾತ್ರ)
compile_assert(acpi_madt_iso_ನಮೂದು_ಗಾತ್ರ, ಗಾತ್ರ(acpi_madt_iso_t) <= MADT_MAX_ನಮೂದು_ಗಾತ್ರ)

/* workaround because string literals are not supported by C parser */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ acpi_str_rsd[]  = {'R', 'S', 'D', ' ', 'P', 'T', 'R', ' ', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ acpi_str_fadt[] = {'F', 'A', 'C', 'P', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ acpi_str_apic[] = {'A', 'P', 'I', 'C', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ acpi_str_dmar[] = {'D', 'M', 'A', 'R', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ madt_str_apic[] = {'A', 'P', 'I', 'C', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ madt_str_x2apic[] = {'x', '2', 'A', 'P', 'I', 'C', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ madt_str_ioapic[] = {'I', 'O', 'A', 'P', 'I', 'C', 0};
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ madt_str_iso[] = {'I', 'S', 'O', 0};

/* aligned(4) so that we can copy the BIOS provided MADT entries into this buffer
 * then cast it to it's proper type then dereference it in an aligned manner. */
BOOT_ದತ್ತಾಂಶ ಅಕ್ಷರ madt_ನಮೂದು[MADT_MAX_ನಮೂದು_ಗಾತ್ರ] ALIGN(4);

BOOT_ಸಂಕೇತ ಸ್ಥಿರ uint8_t acpi_calc_checksum(ಅಕ್ಷರ *ಆರಂಭ, uint32_t ಉದ್ದಳತೆ)
{
    uint8_t checksum = 0;

    ಆಗ (ಉದ್ದಳತೆ > 0) {
        checksum += *ಆರಂಭ;
        ಆರಂಭ++;
        ಉದ್ದಳತೆ--;
    }
    ಹಿಂತಿರುಗಿ checksum;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ acpi_rsdp_t *acpi_get_rsdp(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ *addr;

    ಗಾಗಿ (addr = (ಅಕ್ಷರ *)BIOS_PADDR_ಆರಂಭ; addr < (ಅಕ್ಷರ *)BIOS_PADDR_ಕೊನೆ; addr += 16) {
        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(addr, acpi_str_rsd, 8) == 0) {
            ಆದರೆ (acpi_calc_checksum(addr, ACPI_V1_ಗಾತ್ರ) == 0) {
                ಹಿಂತಿರುಗಿ (acpi_rsdp_t *)addr;
            }
        }
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ ಶೂನ್ಯ *acpi_ಕೋಷ್ಟಕ_init(ಶೂನ್ಯ *ನಮೂದು, ಎಣಿಕೆ acpi_type ಕೋಷ್ಟಕ_type)
{
    ಶೂನ್ಯ *acpi_ಕೋಷ್ಟಕ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪುಟಗಳು_for_ಕೋಷ್ಟಕ;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ ಪುಟಗಳು_for_ಶೀರ್ಷಿಕೆ = 1;

    /* if we need to map another page to read header */
    ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)ನಮೂದು & ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು);
    ಆದರೆ (ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು) - ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ < ಗಾತ್ರ(acpi_rsdp_t)) {
        ಪುಟಗಳು_for_ಶೀರ್ಷಿಕೆ++;
    }

    /* map in table's header */
    acpi_ಕೋಷ್ಟಕ = map_ತಾತ್ಕಾಲಿಕ_boot_ಪುಟ(ನಮೂದು, ಪುಟಗಳು_for_ಶೀರ್ಷಿಕೆ);

    ಆಯ್ಕೆ (ಕೋಷ್ಟಕ_type) {
    ಸ್ಥಿತಿ ACPI_RSDP: {
        acpi_rsdp_t *rsdp_ನಮೂದು = (acpi_rsdp_t *)ನಮೂದು;
        ಪುಟಗಳು_for_ಕೋಷ್ಟಕ = (rsdp_ನಮೂದು->ಉದ್ದಳತೆ + ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ) / ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು) + 1;
        ನಿಲ್ಲಿಸು;
    }
    ಸ್ಥಿತಿ ACPI_RSDT: { // RSDT, MADT, DMAR etc.
        acpi_rsdt_t *rsdt_ನಮೂದು = (acpi_rsdt_t *)ನಮೂದು;
        ಪುಟಗಳು_for_ಕೋಷ್ಟಕ = (rsdt_ನಮೂದು->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ + ಸ್ಥಾನಾಂತರ_ಒಳ_ಪುಟ) / ಮುಖವಾಡ(LARGE_ಪುಟ_ಬಿಟ್ಟುಗಳು) + 1;
        ನಿಲ್ಲಿಸು;
    }
    ಪೂರ್ವನಿಯೋಜಿತ:
        ಬರೆ("Error: Mapping unknown ACPI table type\n");
        ಪ್ರತಿಜ್ಞಾ(false);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    /* map in full table */
    acpi_ಕೋಷ್ಟಕ = map_ತಾತ್ಕಾಲಿಕ_boot_ಪುಟ(ನಮೂದು, ಪುಟಗಳು_for_ಕೋಷ್ಟಕ);

    ಹಿಂತಿರುಗಿ acpi_ಕೋಷ್ಟಕ;
}

BOOT_ಸಂಕೇತ bool_t acpi_init(acpi_rsdp_t *rsdp_ದತ್ತಾಂಶ)
{
    acpi_rsdp_t *acpi_rsdp = acpi_get_rsdp();

    ಆದರೆ (acpi_rsdp == ಖಾಲಿ) {
        ಬರೆ("BIOS: No ACPI support detected\n");
        ಹಿಂತಿರುಗಿ false;
    }
    ಬರೆ("ACPI: RSDP paddr=%p\n", acpi_rsdp);
    acpi_rsdp = acpi_ಕೋಷ್ಟಕ_init(acpi_rsdp, ACPI_RSDP);
    ಬರೆ("ACPI: RSDP vaddr=%p\n", acpi_rsdp);

    /* create a copy of the rsdp data */
    *rsdp_ದತ್ತಾಂಶ = *acpi_rsdp;

    /* perform final validation */
    ಹಿಂತಿರುಗಿ acpi_validate_rsdp(rsdp_ದತ್ತಾಂಶ);
}

BOOT_ಸಂಕೇತ bool_t acpi_validate_rsdp(acpi_rsdp_t *acpi_rsdp)
{
    acpi_rsdt_t *acpi_rsdt;
    acpi_rsdt_t *acpi_rsdt_mapped;

    ಆದರೆ (acpi_calc_checksum((ಅಕ್ಷರ *)acpi_rsdp, ACPI_V1_ಗಾತ್ರ) != 0) {
        ಬರೆ("BIOS: ACPIv1 information corrupt\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಆದರೆ (acpi_rsdp->revision > 0 && acpi_calc_checksum((ಅಕ್ಷರ *)acpi_rsdp, ಗಾತ್ರ(*acpi_rsdp)) != 0) {
        ಬರೆ("BIOS: ACPIv2 information corrupt\n");
        ಹಿಂತಿರುಗಿ false;
    }

    /* verify the rsdt, even though we do not actually make use of the mapping right now */
    acpi_rsdt = (acpi_rsdt_t *)(ಶಬ್ದ_t)acpi_rsdp->rsdt_ವಿಳಾಸ;
    ಬರೆ("ACPI: RSDT paddr=%p\n", acpi_rsdt);
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ಕೋಷ್ಟಕ_init(acpi_rsdt, ACPI_RSDT);
    ಬರೆ("ACPI: RSDT vaddr=%p\n", acpi_rsdt_mapped);

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ > 0);
    ಆದರೆ (acpi_calc_checksum((ಅಕ್ಷರ *)acpi_rsdt_mapped, acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ) != 0) {
        ಬರೆ("ACPI: RSDT checksum failure\n");
        ಹಿಂತಿರುಗಿ false;
    }

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಸ್ಥಿರ bool_t acpi_madt_valid(acpi_madt_ಶೀರ್ಷಿಕೆ_t *ಶೀರ್ಷಿಕೆ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, uint8_t expected)
{
    ಆದರೆ (ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ < expected) {
        ಬರೆ("ACPI: BIOS bug, MADT %s entry size %u < %u expected bytes.\n",
               name, ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ, expected);
        ಹಿಂತಿರುಗಿ false;
    }
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ uint32_t acpi_madt_ಪರಿಶೀಲನೆ(
    acpi_rsdp_t *acpi_rsdp,
    cpu_id_t    *cpu_list,
    uint32_t    *ಅಂಕೆ_ioapic,
    paddr_t     *ioapic_paddrs
)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ        entries;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ        ನಮೂದು_ಗಾತ್ರ;
    uint32_t            ಅಂಕೆ_cpu;
    uint32_t            ಎಣಿಕೆ_2;
    acpi_madt_t        *acpi_madt;
    acpi_madt_ಶೀರ್ಷಿಕೆ_t *acpi_madt_ಶೀರ್ಷಿಕೆ;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_madt_t *acpi_madt_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ಕೋಷ್ಟಕ_init((acpi_rsdt_t *)(ಶಬ್ದ_t)acpi_rsdp->rsdt_ವಿಳಾಸ, ACPI_RSDT);

    ಅಂಕೆ_cpu = 0;
    *ಅಂಕೆ_ioapic = 0;

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ >= ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t)) / ಗಾತ್ರ(uint32_t);
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; ಎಣಿಕೆ_2 < entries; ಎಣಿಕೆ_2++) {
        acpi_madt = (acpi_madt_t *)(ಶಬ್ದ_t)acpi_rsdt_mapped->ನಮೂದು[ಎಣಿಕೆ_2];
        acpi_madt_mapped = (acpi_madt_t *)acpi_ಕೋಷ್ಟಕ_init(acpi_madt, ACPI_RSDT);

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(acpi_str_apic, acpi_madt_mapped->ಶೀರ್ಷಿಕೆ.signature, 4) == 0) {
            ಬರೆ("ACPI: MADT paddr=%p\n", acpi_madt);
            ಬರೆ("ACPI: MADT vaddr=%p\n", acpi_madt_mapped);
            ಬರೆ("ACPI: MADT apic_addr=0x%x\n", acpi_madt_mapped->apic_addr);
            ಬರೆ("ACPI: MADT flags=0x%x\n", acpi_madt_mapped->flags);

            acpi_madt_ಶೀರ್ಷಿಕೆ = (acpi_madt_ಶೀರ್ಷಿಕೆ_t *)(acpi_madt_mapped + 1);

            ಆಗ ((ಅಕ್ಷರ *)acpi_madt_ಶೀರ್ಷಿಕೆ < (ಅಕ್ಷರ *)acpi_madt_mapped + acpi_madt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ) {
                ನಮೂದು_ಗಾತ್ರ = MIN(acpi_madt_ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ, MADT_MAX_ನಮೂದು_ಗಾತ್ರ);
                ಸ್ಮರಣೆ_ನಕಲು(madt_ನಮೂದು, acpi_madt_ಶೀರ್ಷಿಕೆ, ನಮೂದು_ಗಾತ್ರ);

                ಆಯ್ಕೆ (acpi_madt_ಶೀರ್ಷಿಕೆ->type) {
                /* ACPI specifies the following rules when listing APIC IDs:
                 *  - Boot processor is listed first
                 *  - For multi-threaded processors, BIOS should list the first logical
                 *    processor of each of the individual multi-threaded processors in MADT
                 *    before listing any of the second logical processors.
                 *  - APIC IDs < 0xFF should be listed in APIC subtable, APIC IDs >= 0xFF
                 *    should be listed in X2APIC subtable */
                ಸ್ಥಿತಿ MADT_APIC: {
                    ಆದರೆ (!acpi_madt_valid(acpi_madt_ಶೀರ್ಷಿಕೆ, madt_str_apic, ಗಾತ್ರ(acpi_madt_apic_t))) {
                        ನಿಲ್ಲಿಸು;
                    }
                    acpi_madt_apic_t *madt_apic = (acpi_madt_apic_t *) madt_ನಮೂದು;

                    /* what Intel calls apic_id is what is called cpu_id in seL4! */
                    uint8_t  cpu_id = madt_apic->apic_id;
                    uint32_t flags  = madt_apic->flags;
                    ಆದರೆ (flags == 1) {
                        ಬರೆ("ACPI: MADT_APIC apic_id=0x%x\n", cpu_id);
                        ಆದರೆ (ಅಂಕೆ_cpu == CONFIG_MAX_ಅಂಕೆ_NODES) {
                            ಬರೆ("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_NODES);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            cpu_list[ಅಂಕೆ_cpu] = cpu_id;
                            ಅಂಕೆ_cpu++;
                        }
                    }
                    ನಿಲ್ಲಿಸು;
                }
                ಸ್ಥಿತಿ MADT_x2APIC: {
                    ಆದರೆ (!acpi_madt_valid(acpi_madt_ಶೀರ್ಷಿಕೆ, madt_str_x2apic, ಗಾತ್ರ(acpi_madt_x2apic_t))) {
                        ನಿಲ್ಲಿಸು;
                    }
                    acpi_madt_x2apic_t *madt_x2apic = (acpi_madt_x2apic_t *) madt_ನಮೂದು;

                    uint32_t cpu_id = madt_x2apic->x2apic_id;
                    uint32_t flags  = madt_x2apic->flags;
                    ಆದರೆ (flags == 1) {
                        ಬರೆ("ACPI: MADT_x2APIC apic_id=0x%x\n", cpu_id);
                        ಆದರೆ (ಅಂಕೆ_cpu == CONFIG_MAX_ಅಂಕೆ_NODES) {
                            ಬರೆ("ACPI: Not recording this APIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_NODES);
                        } ಇಲ್ಲದಿದ್ದರೆ {
                            cpu_list[ಅಂಕೆ_cpu] = cpu_id;
                            ಅಂಕೆ_cpu++;
                        }
                    }
                    ನಿಲ್ಲಿಸು;
                }
                ಸ್ಥಿತಿ MADT_IOAPIC:
                    ಆದರೆ (!acpi_madt_valid(acpi_madt_ಶೀರ್ಷಿಕೆ, madt_str_ioapic, ಗಾತ್ರ(acpi_madt_ioapic_t))) {
                        ನಿಲ್ಲಿಸು;
                    }
                    acpi_madt_ioapic_t *madt_ioapic = (acpi_madt_ioapic_t *) madt_ನಮೂದು;

                    ಬರೆ(
                        "ACPI: MADT_IOAPIC ioapic_id=%d ioapic_addr=0x%x gsib=%d\n",
                        madt_ioapic->ioapic_id,
                        madt_ioapic->ioapic_addr,
                        madt_ioapic->gsib
                    );
                    ಆದರೆ (*ಅಂಕೆ_ioapic == CONFIG_MAX_ಅಂಕೆ_IOAPIC) {
                        ಬರೆ("ACPI: Not recording this IOAPIC, only support %d\n", CONFIG_MAX_ಅಂಕೆ_IOAPIC);
                    } ಇಲ್ಲದಿದ್ದರೆ {
                        ioapic_paddrs[*ಅಂಕೆ_ioapic] = madt_ioapic->ioapic_addr;
                        (*ಅಂಕೆ_ioapic)++;
                    }
                    ನಿಲ್ಲಿಸು;
                ಸ್ಥಿತಿ MADT_ISO: {
                    ಆದರೆ (!acpi_madt_valid(acpi_madt_ಶೀರ್ಷಿಕೆ, madt_str_iso, ಗಾತ್ರ(acpi_madt_iso_t))) {
                        ನಿಲ್ಲಿಸು;
                    }
                    ಬಳಸದ acpi_madt_iso_t *iso = (acpi_madt_iso_t *) madt_ನಮೂದು;

                    ಬರೆ("ACPI: MADT_ISO bus=%d source=%d gsi=0x%x%02x flags=0x%x\n",
                           iso->bus, iso->ಮೂಲ, iso->gsi_ಮೇಲಿನ, iso->gsi_ಕೆಳಗಿನ, iso->flags);
                    ನಿಲ್ಲಿಸು;
                }
                ಪೂರ್ವನಿಯೋಜಿತ:
                    ನಿಲ್ಲಿಸು;
                }
                acpi_madt_ಶೀರ್ಷಿಕೆ = (acpi_madt_ಶೀರ್ಷಿಕೆ_t *)((ಅಕ್ಷರ *)acpi_madt_ಶೀರ್ಷಿಕೆ + acpi_madt_ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ);
            }
        }
    }

    ಬರೆ("ACPI: %d CPU(s) detected\n", ಅಂಕೆ_cpu);

    ಹಿಂತಿರುಗಿ ಅಂಕೆ_cpu;
}

BOOT_ಸಂಕೇತ bool_t acpi_fadt_ಪರಿಶೀಲನೆ(
    acpi_rsdp_t *acpi_rsdp
)
{
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ entries;
    uint32_t            ಎಣಿಕೆ_2;
    acpi_fadt_t        *acpi_fadt;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_fadt_t *acpi_fadt_mapped;
    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ಕೋಷ್ಟಕ_init((acpi_rsdt_t *)(ಶಬ್ದ_t)acpi_rsdp->rsdt_ವಿಳಾಸ, ACPI_RSDT);

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ >= ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t));
    /* Divide by uint32_t explicitly as this is the size as mandated by the ACPI standard */
    entries = (acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t)) / ಗಾತ್ರ(uint32_t);
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; ಎಣಿಕೆ_2 < entries; ಎಣಿಕೆ_2++) {
        acpi_fadt = (acpi_fadt_t *)(ಶಬ್ದ_t)acpi_rsdt_mapped->ನಮೂದು[ಎಣಿಕೆ_2];
        acpi_fadt_mapped = (acpi_fadt_t *)acpi_ಕೋಷ್ಟಕ_init(acpi_fadt, ACPI_RSDT);

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(acpi_str_fadt, acpi_fadt_mapped->ಶೀರ್ಷಿಕೆ.signature, 4) == 0) {
            ಬರೆ("ACPI: FADT paddr=%p\n", acpi_fadt);
            ಬರೆ("ACPI: FADT vaddr=%p\n", acpi_fadt_mapped);
            ಬರೆ("ACPI: FADT flags=0x%x\n", acpi_fadt_mapped->flags);

            ಆದರೆ (config_ಗಣ(CONFIG_USE_LOGICAL_IDS) &&
                acpi_fadt_mapped->flags & ಬಿಟ್(19)) {
                ಬರೆ("system requires apic physical mode\n");
                ಹಿಂತಿರುಗಿ false;
            }
        }
    }

    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ acpi_dmar_ಪರಿಶೀಲನೆ(
    acpi_rsdp_t *acpi_rsdp,
    paddr_t     *drhu_list,
    uint32_t    *ಅಂಕೆ_drhu,
    uint32_t     max_drhu_list_len,
    acpi_rmrr_list_t *rmrr_list
)
{
    ಶಬ್ದ_t i;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ entries;
    uint32_t ಎಣಿಕೆ_2;
    uint32_t reg_basel, reg_baseh;
    ಪೂರ್ಣಾಂಕ rmrr_ಎಣಿಕೆ;
    dev_id_t dev_id;

    acpi_dmar_t          *acpi_dmar;
    acpi_dmar_ಶೀರ್ಷಿಕೆ_t   *acpi_dmar_ಶೀರ್ಷಿಕೆ;
    acpi_dmar_rmrr_t     *acpi_dmar_rmrr;
    acpi_dmar_devscope_t *acpi_dmar_devscope;

    acpi_rsdt_t *acpi_rsdt_mapped;
    acpi_dmar_t *acpi_dmar_mapped;

    acpi_rsdt_mapped = (acpi_rsdt_t *)acpi_ಕೋಷ್ಟಕ_init((acpi_rsdt_t *)(ಶಬ್ದ_t)acpi_rsdp->rsdt_ವಿಳಾಸ, ACPI_RSDT);

    *ಅಂಕೆ_drhu = 0;
    rmrr_ಎಣಿಕೆ = 0;

    ಪ್ರತಿಜ್ಞಾ(acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ >= ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t));
    entries = (acpi_rsdt_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ - ಗಾತ್ರ(acpi_ಶೀರ್ಷಿಕೆ_t)) / ಗಾತ್ರ(uint32_t);
    ಗಾಗಿ (ಎಣಿಕೆ_2 = 0; ಎಣಿಕೆ_2 < entries; ಎಣಿಕೆ_2++) {
        acpi_dmar = (acpi_dmar_t *)(ಶಬ್ದ_t)acpi_rsdt_mapped->ನಮೂದು[ಎಣಿಕೆ_2];
        acpi_dmar_mapped = (acpi_dmar_t *)acpi_ಕೋಷ್ಟಕ_init(acpi_dmar, ACPI_RSDT);

        ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(acpi_str_dmar, acpi_dmar_mapped->ಶೀರ್ಷಿಕೆ.signature, 4) == 0) {
            ಬರೆ("ACPI: DMAR paddr=%p\n", acpi_dmar);
            ಬರೆ("ACPI: DMAR vaddr=%p\n", acpi_dmar_mapped);
            ಬರೆ("ACPI: IOMMU host address width: %d\n", acpi_dmar_mapped->host_addr_ಅಗಲ + 1);
            acpi_dmar_ಶೀರ್ಷಿಕೆ = (acpi_dmar_ಶೀರ್ಷಿಕೆ_t *)(acpi_dmar_mapped + 1);

            ಆಗ ((ಅಕ್ಷರ *)acpi_dmar_ಶೀರ್ಷಿಕೆ < (ಅಕ್ಷರ *)acpi_dmar_mapped + acpi_dmar_mapped->ಶೀರ್ಷಿಕೆ.ಉದ್ದಳತೆ) {
                ಆಯ್ಕೆ (acpi_dmar_ಶೀರ್ಷಿಕೆ->type) {

                ಸ್ಥಿತಿ DMAR_DRHD:
                    ಆದರೆ (*ಅಂಕೆ_drhu == max_drhu_list_len) {
                        ಬರೆ("ACPI: too many IOMMUs, disabling IOMMU support\n");
                        /* try to increase MAX_NUM_DRHU in config.h */
                        *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                        ಹಿಂತಿರುಗಿ;
                    }
                    reg_basel = ((acpi_dmar_drhd_t *)acpi_dmar_ಶೀರ್ಷಿಕೆ)->reg_ತಳ[0];
                    reg_baseh = ((acpi_dmar_drhd_t *)acpi_dmar_ಶೀರ್ಷಿಕೆ)->reg_ತಳ[1];
                    /* check if value fits into uint32_t */
                    ಆದರೆ (reg_baseh != 0) {
                        ಬರೆ("ACPI: DMAR_DRHD reg_base exceeds 32 bit, disabling IOMMU support\n");
                        /* try to make BIOS map it below 4G */
                        *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                        ಹಿಂತಿರುಗಿ;
                    }
                    drhu_list[*ಅಂಕೆ_drhu] = (paddr_t)reg_basel;
                    (*ಅಂಕೆ_drhu)++;
                    ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DMAR_RMRR:
                    /* loop through all device scopes of this RMRR */
                    acpi_dmar_rmrr = (acpi_dmar_rmrr_t *)acpi_dmar_ಶೀರ್ಷಿಕೆ;
                    ಆದರೆ (acpi_dmar_rmrr->reg_ತಳ[1] != 0 ||
                        acpi_dmar_rmrr->reg_ಎಲ್ಲೆ[1] != 0) {
                        ಬರೆ("ACPI: RMRR device above 4GiB, disabling IOMMU support\n");
                        *ಅಂಕೆ_drhu = 0;
                        ಹಿಂತಿರುಗಿ ;
                    }

                    ಗಾಗಿ (i = 0; i <= (acpi_dmar_ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ - ಗಾತ್ರ(acpi_dmar_rmrr_t)) / ಗಾತ್ರ(acpi_dmar_devscope_t); i++) {
                        acpi_dmar_devscope = &acpi_dmar_rmrr->devscope_0 + i;

                        ಆದರೆ (acpi_dmar_devscope->type != 1) {
                            /* FIXME - bugzilla bug 170 */
                            ಬರೆ("ACPI: RMRR device scope: non-PCI-Endpoint-Devices not supported yet, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                            ಹಿಂತಿರುಗಿ;
                        }

                        ಆದರೆ (acpi_dmar_devscope->ಉದ್ದಳತೆ > ಗಾತ್ರ(acpi_dmar_devscope_t)) {
                            /* FIXME - bugzilla bug 170 */
                            ಬರೆ("ACPI: RMRR device scope: devices behind bridges not supported yet, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0; /* report zero IOMMUs */
                            ಹಿಂತಿರುಗಿ;
                        }

                        dev_id =
                            get_dev_id(
                                acpi_dmar_devscope->ಆರಂಭ_bus,
                                acpi_dmar_devscope->ಮಾರ್ಗ_0.dev,
                                acpi_dmar_devscope->ಮಾರ್ಗ_0.fun
                            );

                        ಆದರೆ (rmrr_ಎಣಿಕೆ == CONFIG_MAX_RMRR_ENTRIES) {
                            ಬರೆ("ACPI: Too many RMRR entries, disabling IOMMU support\n");
                            *ಅಂಕೆ_drhu = 0;
                            ಹಿಂತಿರುಗಿ;
                        }
                        ಬರೆ("\tACPI: registering RMRR entry for region for device: bus=0x%x dev=0x%x fun=0x%x\n",
                               acpi_dmar_devscope->ಆರಂಭ_bus,
                               acpi_dmar_devscope->ಮಾರ್ಗ_0.dev,
                               acpi_dmar_devscope->ಮಾರ್ಗ_0.fun
                              );

                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].device = dev_id;
                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].ತಳ = acpi_dmar_rmrr->reg_ತಳ[0];
                        rmrr_list->entries[rmrr_ಎಣಿಕೆ].ಎಲ್ಲೆ = acpi_dmar_rmrr->reg_ಎಲ್ಲೆ[0];
                        rmrr_ಎಣಿಕೆ++;
                    }
                    ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DMAR_ATSR:
                    /* not implemented yet */
                    ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                    ಬರೆ("ACPI: Unknown DMA remapping structure type: %x\n", acpi_dmar_ಶೀರ್ಷಿಕೆ->type);
                }
                acpi_dmar_ಶೀರ್ಷಿಕೆ = (acpi_dmar_ಶೀರ್ಷಿಕೆ_t *)((ಅಕ್ಷರ *)acpi_dmar_ಶೀರ್ಷಿಕೆ + acpi_dmar_ಶೀರ್ಷಿಕೆ->ಉದ್ದಳತೆ);
            }
        }
    }
    rmrr_list->ಅಂಕೆ = rmrr_ಎಣಿಕೆ;
    ಬರೆ("ACPI: %d IOMMUs detected\n", *ಅಂಕೆ_drhu);
}
