/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <arch/kernel/elf.h>
#समावेश <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_CODE bool_t elf_checkFile(Elf_Header_t *elfFile, word_t max_लंबाई)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    word_t      phdr_table_आकार;

    अगर (max_लंबाई < माप(Elf_Header_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != माप(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        वापस false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_table_आकार = elfFile->e_phnum * माप(Elf_Phdr_t);
    अगर (elfFile->e_phoff > max_लंबाई || phdr_table_आकार > max_लंबाई - elfFile->e_phoff) {
        वापस false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    क्रम (i = 0; i < elfFile->e_phnum; i++) {
        अगर (phdr[i].p_type != PT_LOAD) {
            जारी;
        }
        अगर (phdr[i].p_offset > max_लंबाई || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_लंबाई - phdr[i].p_offset ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            वापस false;
        }
    }
    वापस true;
}

BOOT_CODE v_region_t elf_getMemoryBounds(Elf_Header_t *elfFile)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    v_region_t elf_reg;
    vptr_t     sect_start;
    vptr_t     sect_end;
    uint32_t   i;

    elf_reg.start = ~0UL;
    elf_reg.end = 0;

    /* Loop through all loadable segments and record start/end address. */
    क्रम (i = 0; i < elfFile->e_phnum; i++) {
        अगर (phdr[i].p_type == PT_LOAD && phdr[i].p_memsz > 0) {
            sect_start = phdr[i].p_vaddr;
            sect_end = sect_start + phdr[i].p_memsz;
            अगर (sect_start < elf_reg.start) {
                elf_reg.start = sect_start;
            }
            अगर (sect_end > elf_reg.end) {
                elf_reg.end = sect_end;
            }
        }
    }
    वापस elf_reg;
}

BOOT_CODE व्योम elf_load(Elf_Header_t *elfFile, seL4_Word offset)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    word_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    क्रम (i = 0; i < elfFile->e_phnum; i++) {
        अगर (phdr[i].p_type != PT_LOAD || phdr[i].p_memsz == 0) {
            जारी;
        }
        src = (paddr_t)elfFile + phdr[i].p_offset;
        dst = phdr[i].p_vaddr + offset;
        len = phdr[i].p_filesz;
        स_नकल((व्योम *)dst, (अक्षर *)src, len);
    }
}
