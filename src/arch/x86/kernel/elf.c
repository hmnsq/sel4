/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#অন্তর্ভাব <arch/kernel/elf.h>
#অন্তর্ভাব <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_CODE bool_t elf_checkFile(Elf_Header_t *elfFile, word_t max_দৈর্ঘ্য)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    word_t      phdr_table_আকার;

    যদি (max_দৈর্ঘ্য < আকার(Elf_Header_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != আকার(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        ফেরত false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_table_আকার = elfFile->e_phnum * আকার(Elf_Phdr_t);
    যদি (elfFile->e_phoff > max_দৈর্ঘ্য || phdr_table_আকার > max_দৈর্ঘ্য - elfFile->e_phoff) {
        ফেরত false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    জন্য (i = 0; i < elfFile->e_phnum; i++) {
        যদি (phdr[i].p_type != PT_LOAD) {
            চালু;
        }
        যদি (phdr[i].p_offset > max_দৈর্ঘ্য || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_দৈর্ঘ্য - phdr[i].p_offset ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            ফেরত false;
        }
    }
    ফেরত true;
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
    জন্য (i = 0; i < elfFile->e_phnum; i++) {
        যদি (phdr[i].p_type == PT_LOAD && phdr[i].p_memsz > 0) {
            sect_start = phdr[i].p_vaddr;
            sect_end = sect_start + phdr[i].p_memsz;
            যদি (sect_start < elf_reg.start) {
                elf_reg.start = sect_start;
            }
            যদি (sect_end > elf_reg.end) {
                elf_reg.end = sect_end;
            }
        }
    }
    ফেরত elf_reg;
}

BOOT_CODE শূন্য elf_load(Elf_Header_t *elfFile, seL4_Word offset)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    word_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    জন্য (i = 0; i < elfFile->e_phnum; i++) {
        যদি (phdr[i].p_type != PT_LOAD || phdr[i].p_memsz == 0) {
            চালু;
        }
        src = (paddr_t)elfFile + phdr[i].p_offset;
        dst = phdr[i].p_vaddr + offset;
        len = phdr[i].p_filesz;
        memcpy((শূন্য *)dst, (অক্ষর *)src, len);
    }
}
