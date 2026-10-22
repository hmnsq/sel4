/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <config.h>

#ಅನ್ತರ್ಭಾವ <linker.h>
#ಅನ್ತರ್ಭಾವ <machine/io.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/hardware.h>
#ಅನ್ತರ್ಭಾವ <plat/machine/ioapic.h>

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REGSEL 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_ಕಿಟಕಿ 0x10

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REG_IOAPICID 0x00
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REG_IOAPICVER 0x01
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPIC_REG_IOREDTBL 0x10

/*
 * Maximum Redirection Entry, bits 23:16 in IOAPIC_REG_IOAPICVER -- corresponds
 * to number of interrupt input pins for the IOAPIC minus one.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPICVER_MRE_ಮುಖವಾಡ 0xff0000

#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ(reg) (IOAPIC_REG_IOREDTBL + (reg) * 2)
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಮೇಲಿನ(reg) (IOREDTBL_ಕೆಳಗಿನ(reg) + 1)

#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_ಅಡಚಣೆ_ಮುಖವಾಡ ಬಿಟ್(16)
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_TRIGGER_ಬಹುಲಕ_LEVEL ಬಿಟ್(15)
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_TRIGGER_ಬಹುಲಕ_ಸರಿಸು    15
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_POLARITY_ಕೆಳಗಿನ ಬಿಟ್(13)
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_POLARITY_ಸರಿಸು         13
#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಕೆಳಗಿನ_DEST_ಬಹುಲಕ_LOGCIAL ಬಿಟ್(11)

#ವ್ಯಾಖ್ಯಾನಿಸು IOAPICID_ID_ಬಿಟ್ಟುಗಳು 4
#ವ್ಯಾಖ್ಯಾನಿಸು IOAPICID_ID_ಸ್ಥಾನಾಂತರ 24

#ವ್ಯಾಖ್ಯಾನಿಸು IOREDTBL_ಮೇಲಿನ_RESERVED_ಬಿಟ್ಟುಗಳು 24

/*
 * Cache what we believe is in the low word of the IOREDTBL. This
 * has all the state of trigger modes etc etc.
 * IOAPIC_IRQ_LINES is the upper bound on the number of lines
 * per IOAPIC.
 */
ಸ್ಥಿರ uint32_t ioredtbl_state[IOAPIC_IRQ_ಸಾಲುಗಳು * MAX(1, CONFIG_MAX_ಅಂಕೆ_IOAPIC)];
/*
 * The number of IRQ lines for each IOAPIC
 */
ಸ್ಥಿರ uint8_t ioapic_nirqs[MAX(1, CONFIG_MAX_ಅಂಕೆ_IOAPIC)];

/* Number of IOAPICs in the system */
ಸ್ಥಿರ uint32_t ಅಂಕೆ_ioapics = 0;

ಸ್ಥಿರ ಶೂನ್ಯ ioapic_write(uint32_t ioapic, ಶಬ್ದ_t reg, uint32_t ಮೌಲ್ಯ)
{
    *(ಅಸ್ಥಿರ uint32_t *)((ಶಬ್ದ_t)(PPTR_IOAPIC_ಆರಂಭ + ioapic * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) + reg) = ಮೌಲ್ಯ;
}

ಸ್ಥಿರ uint32_t ioapic_read(uint32_t ioapic, ಶಬ್ದ_t reg)
{
    ಹಿಂತಿರುಗಿ *(ಅಸ್ಥಿರ uint32_t *)((ಶಬ್ದ_t)(PPTR_IOAPIC_ಆರಂಭ + ioapic * ಬಿಟ್(ಪುಟ_ಬಿಟ್ಟುಗಳು)) + reg);
}

ಸ್ಥಿರ ಶೂನ್ಯ single_ioapic_init(ಶಬ್ದ_t ioapic)
{
    uint32_t i;
    uint32_t nirqs;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOAPIC_REG_IOAPICVER);
    nirqs = ((ioapic_read(ioapic, IOAPIC_ಕಿಟಕಿ) & IOAPICVER_MRE_ಮುಖವಾಡ) >> 16) + 1;
    ioapic_nirqs[ioapic] = nirqs;

    /*
     * The MRE field of the IOAPICVER register is supposed to only be able to
     * return a max value of 239, which means a max nirqs value of IOAPIC_IRQ_LINES.
     */
    ಆದರೆ (nirqs > IOAPIC_IRQ_ಸಾಲುಗಳು) {
        userError("%s: ioapic %lu has %u IRQs,\n"
                  "which is greater than the max handled (%u)\n",
                  __func__, ioapic, nirqs, IOAPIC_IRQ_ಸಾಲುಗಳು);
        halt();
    }

    /* Mask all the IRQs. In doing so we happen to set
     * the vector to 0, which we can assert against in
     * mask_interrupt to ensure a vector is assigned
     * before we unmask */
    ಗಾಗಿ (i = 0; i < nirqs; i++) {
        /* Send to ioapic_target_cpu, mask, and set 0 vector */
        ioapic_map_pin_ಗಮ್ಯ_ಸದಿಶ(ioapic, i, 0, 0, 0);
    }
}

ಸ್ಥಿರ  cpu_id_t ioapic_ಗುರಿ_cpu = 0;
ಶೂನ್ಯ ioapic_init(uint32_t ಅಂಕೆ_nodes, cpu_id_t *cpu_list, uint32_t ಅಂಕೆ_ioapic)
{
    uint32_t ioapic;
    ಅಂಕೆ_ioapics = ಅಂಕೆ_ioapic;
    ioapic_ಗುರಿ_cpu = cpu_list[0];

    ಗಾಗಿ (ioapic = 0; ioapic < ಅಂಕೆ_ioapic; ioapic++) {
        /* Init this ioapic */
        single_ioapic_init(ioapic);
    }
}

ಶೂನ್ಯ ioapic_ಮುಖವಾಡ(bool_t ಮುಖವಾಡ_2, uint32_t ioapic, uint32_t pin)
{
    ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ = ioapic * IOAPIC_IRQ_ಸಾಲುಗಳು + pin;
    ಆದರೆ (ioapic >= ಅಂಕೆ_ioapics || pin >= ioapic_nirqs[ioapic]) {
        /* silently ignore requests to non existent parts of the interrupt space */
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (ಮುಖವಾಡ_2) {
        ioredtbl_state[ಸೂಚ್ಯಂಕ] |= IOREDTBL_ಕೆಳಗಿನ_ಅಡಚಣೆ_ಮುಖವಾಡ;
    } ಇಲ್ಲದಿದ್ದರೆ {
        ioredtbl_state[ಸೂಚ್ಯಂಕ] &= ~IOREDTBL_ಕೆಳಗಿನ_ಅಡಚಣೆ_ಮುಖವಾಡ;
        /* it should not be possible to be unmasking an interrupt, without
         * it having been mapped to a vector, assert that this is the case */
        ಪ್ರತಿಜ್ಞಾ((ioredtbl_state[ಸೂಚ್ಯಂಕ] & 0xff) != 0);
    }
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ಕೆಳಗಿನ(pin));
    ioapic_write(ioapic, IOAPIC_ಕಿಟಕಿ, ioredtbl_state[ಸೂಚ್ಯಂಕ]);
}

exception_t ioapic_decode_map_pin_ಗಮ್ಯ_ಸದಿಶ(ಶಬ್ದ_t ioapic, ಶಬ್ದ_t pin, ಶಬ್ದ_t level,
                                            ಶಬ್ದ_t polarity, ಶಬ್ದ_t ಸದಿಶ)
{
    ಆದರೆ (ಅಂಕೆ_ioapics == 0) {
        userError("System has no IOAPICs");
        current_syscall_error.type = seL4_IllegalOperation;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (ioapic >= ಅಂಕೆ_ioapics) {
        userError("Invalid IOAPIC %ld, only have %ld", (ದೀರ್ಘ)ioapic, (ದೀರ್ಘ)ಅಂಕೆ_ioapics);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ಅಂಕೆ_ioapics - 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (pin >= ioapic_nirqs[ioapic]) {
        userError("Invalid IOAPIC pin %ld, there are %d pins", (ದೀರ್ಘ)pin, ioapic_nirqs[ioapic]);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = ioapic_nirqs[ioapic] - 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }

    ಆದರೆ (level != 0 && level != 1) {
        userError("Level should be 0 or 1, not %d", (ಪೂರ್ಣಾಂಕ)level);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಆದರೆ (polarity != 0 && polarity != 1) {
        userError("Polarity should be 0 or 1, not %d", (ಪೂರ್ಣಾಂಕ)polarity);
        current_syscall_error.type = seL4_RangeError;
        current_syscall_error.rangeErrorMin = 0;
        current_syscall_error.rangeErrorMax = 1;
        ಹಿಂತಿರುಗಿ EXCEPTION_SYSCALL_ERROR;
    }
    ಹಿಂತಿರುಗಿ EXCEPTION_NONE;
}

ಶೂನ್ಯ ioapic_map_pin_ಗಮ್ಯ_ಸದಿಶ(ಶಬ್ದ_t ioapic, ಶಬ್ದ_t pin, ಶಬ್ದ_t level,
                              ಶಬ್ದ_t polarity, ಶಬ್ದ_t ಸದಿಶ)
{
    uint32_t ioredtbl_ಮೇಲಿನ = 0;
    uint32_t ಸೂಚ್ಯಂಕ = 0;

    ಸೂಚ್ಯಂಕ = ioapic * IOAPIC_IRQ_ಸಾಲುಗಳು + pin;
    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ಮೇಲಿನ(pin));
    ioredtbl_ಮೇಲಿನ = ioapic_read(ioapic, IOAPIC_ಕಿಟಕಿ) & ಮುಖವಾಡ(IOREDTBL_ಮೇಲಿನ_RESERVED_ಬಿಟ್ಟುಗಳು);
    /* delivery mode: physical mode only, using APIC ID */
    ioredtbl_ಮೇಲಿನ |= (ioapic_ಗುರಿ_cpu << IOREDTBL_ಮೇಲಿನ_RESERVED_ಬಿಟ್ಟುಗಳು);
    ioapic_write(ioapic, IOAPIC_ಕಿಟಕಿ, ioredtbl_ಮೇಲಿನ);
    /* we do not need to add IRQ_INT_OFFSET to the vector here */
    ioredtbl_state[ಸೂಚ್ಯಂಕ] = IOREDTBL_ಕೆಳಗಿನ_ಅಡಚಣೆ_ಮುಖವಾಡ |
                            (level << IOREDTBL_ಕೆಳಗಿನ_TRIGGER_ಬಹುಲಕ_ಸರಿಸು) |
                            (polarity << IOREDTBL_ಕೆಳಗಿನ_POLARITY_ಸರಿಸು) |
                            ಸದಿಶ;

    ioapic_write(ioapic, IOAPIC_REGSEL, IOREDTBL_ಕೆಳಗಿನ(pin));
    /* the upper 16 bits are reserved */
    ioredtbl_state[ಸೂಚ್ಯಂಕ] |= ioapic_read(ioapic, IOAPIC_ಕಿಟಕಿ) & ~ಮುಖವಾಡ(16);
    ioapic_write(ioapic, IOAPIC_ಕಿಟಕಿ, ioredtbl_state[ಸೂಚ್ಯಂಕ]);
}
