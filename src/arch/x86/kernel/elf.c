/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ಅನ್ತರ್ಭಾವ <arch/kernel/elf.h>
#ಅನ್ತರ್ಭಾವ <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_ಸಂಕೇತ bool_t elf_checkFile(Elf_ಶೀರ್ಷಿಕೆ_t *elfFile, ಶಬ್ದ_t max_ಉದ್ದಳತೆ)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    ಶಬ್ದ_t      phdr_ಕೋಷ್ಟಕ_ಗಾತ್ರ;

    ಆದರೆ (max_ಉದ್ದಳತೆ < ಗಾತ್ರ(Elf_ಶೀರ್ಷಿಕೆ_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != ಗಾತ್ರ(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        ಹಿಂತಿರುಗಿ false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_ಕೋಷ್ಟಕ_ಗಾತ್ರ = elfFile->e_phnum * ಗಾತ್ರ(Elf_Phdr_t);
    ಆದರೆ (elfFile->e_phoff > max_ಉದ್ದಳತೆ || phdr_ಕೋಷ್ಟಕ_ಗಾತ್ರ > max_ಉದ್ದಳತೆ - elfFile->e_phoff) {
        ಹಿಂತಿರುಗಿ false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    ಗಾಗಿ (i = 0; i < elfFile->e_phnum; i++) {
        ಆದರೆ (phdr[i].p_type != PT_ತುಂಬು) {
            ಮುಂದುವರಿ;
        }
        ಆದರೆ (phdr[i].p_ಸ್ಥಾನಾಂತರ > max_ಉದ್ದಳತೆ || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_ಉದ್ದಳತೆ - phdr[i].p_ಸ್ಥಾನಾಂತರ ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            ಹಿಂತಿರುಗಿ false;
        }
    }
    ಹಿಂತಿರುಗಿ true;
}

BOOT_ಸಂಕೇತ v_region_t elf_get_ಸ್ಮರಣೆ_Bounds(Elf_ಶೀರ್ಷಿಕೆ_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_ಆರಂಭ;
    vptr_t     sect_ಕೊನೆ;
    uint32_t   i;

    elf_reg.ಆರಂಭ = ~0UL;
    elf_reg.ಕೊನೆ = 0;

    /* Loop through all loadable segments and record start/end address. */
    ಗಾಗಿ (i = 0; i < elfFile->e_phnum; i++) {
        ಆದರೆ (phdr[i].p_type == PT_ತುಂಬು && phdr[i].p_memsz > 0) {
            sect_ಆರಂಭ = phdr[i].p_vaddr;
            sect_ಕೊನೆ = sect_ಆರಂಭ + phdr[i].p_memsz;
            ಆದರೆ (sect_ಆರಂಭ < elf_reg.ಆರಂಭ) {
                elf_reg.ಆರಂಭ = sect_ಆರಂಭ;
            }
            ಆದರೆ (sect_ಕೊನೆ > elf_reg.ಕೊನೆ) {
                elf_reg.ಕೊನೆ = sect_ಕೊನೆ;
            }
        }
    }
    ಹಿಂತಿರುಗಿ elf_reg;
}

BOOT_ಸಂಕೇತ ಶೂನ್ಯ elf_ತುಂಬು(Elf_ಶೀರ್ಷಿಕೆ_t *elfFile, se_L4_ಶಬ್ದ ಸ್ಥಾನಾಂತರ)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    ಶಬ್ದ_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    ಗಾಗಿ (i = 0; i < elfFile->e_phnum; i++) {
        ಆದರೆ (phdr[i].p_type != PT_ತುಂಬು || phdr[i].p_memsz == 0) {
            ಮುಂದುವರಿ;
        }
        src = (paddr_t)elfFile + phdr[i].p_ಸ್ಥಾನಾಂತರ;
        dst = phdr[i].p_vaddr + ಸ್ಥಾನಾಂತರ;
        len = phdr[i].p_filesz;
        ಸ್ಮರಣೆ_ನಕಲು((ಶೂನ್ಯ *)dst, (ಅಕ್ಷರ *)src, len);
    }
}
