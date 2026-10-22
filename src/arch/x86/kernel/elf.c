/*
 * Copyright 2020, Data61, CSIRO (ABN 41 687 119 230)
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <arch/kernel/elf.h>
#अन्तर्भाव <linker.h>

/* minimal ELF functionality for loading GRUB boot module */

BOOT_CODE bool_t elf_checkFile(Elf_Header_t *elfFile, word_t max_length)
{
    Elf_Phdr_t *phdr;
    uint32_t    i;
    word_t      phdr_table_size;

    यदि (max_length < परिमाण(Elf_Header_t) ||
        elfFile->e_ident[0] != '\177' || elfFile->e_ident[1] != 'E' ||
        elfFile->e_ident[2] != 'L'    || elfFile->e_ident[3] != 'F' ||
        elfFile->e_ident[4] != ELF_EXPECTED_CLASS ||
        elfFile->e_machine != ELF_EXPECTED_MACHINE ||
        (elfFile->e_type != ET_EXEC && elfFile->e_type != ET_DYN) ||
        elfFile->e_phentsize != परिमाण(Elf_Phdr_t) || elfFile->e_phnum == 0) {
        प्रत्यागम false;
    }
    /* Overflow-safe check that the program table fits within the file. */
    phdr_table_size = elfFile->e_phnum * परिमाण(Elf_Phdr_t);
    यदि (elfFile->e_phoff > max_length || phdr_table_size > max_length - elfFile->e_phoff) {
        प्रत्यागम false;
    }
    /* Overflow-safe checks that the segments are contained within the
     * file, and that p_vaddr + p_memsz doesn't overflow. */
    phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    प्रति (i = 0; i < elfFile->e_phnum; i++) {
        यदि (phdr[i].p_type != PT_LOAD) {
            अनुवर्त;
        }
        यदि (phdr[i].p_offset > max_length || phdr[i].p_filesz > phdr[i].p_memsz ||
            phdr[i].p_filesz > max_length - phdr[i].p_offset ||
            phdr[i].p_memsz > ~0UL - phdr[i].p_vaddr) {
            प्रत्यागम false;
        }
    }
    प्रत्यागम true;
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
    प्रति (i = 0; i < elfFile->e_phnum; i++) {
        यदि (phdr[i].p_type == PT_LOAD && phdr[i].p_memsz > 0) {
            sect_start = phdr[i].p_vaddr;
            sect_end = sect_start + phdr[i].p_memsz;
            यदि (sect_start < elf_reg.start) {
                elf_reg.start = sect_start;
            }
            यदि (sect_end > elf_reg.end) {
                elf_reg.end = sect_end;
            }
        }
    }
    प्रत्यागम elf_reg;
}

BOOT_CODE शून्य elf_load(Elf_Header_t *elfFile, seL4_Word offset)
{
    Elf_Phdr_t *phdr = (Elf_Phdr_t *)((paddr_t)elfFile + elfFile->e_phoff);
    paddr_t    src;
    paddr_t    dst;
    word_t     len;
    uint32_t   i;

    /* Loop through all segments and load them. Relies on the caller
     * having already zeroed the destination memory (load_boot_module()
     * does, via memzero()). */
    प्रति (i = 0; i < elfFile->e_phnum; i++) {
        यदि (phdr[i].p_type != PT_LOAD || phdr[i].p_memsz == 0) {
            अनुवर्त;
        }
        src = (paddr_t)elfFile + phdr[i].p_offset;
        dst = phdr[i].p_vaddr + offset;
        len = phdr[i].p_filesz;
        memcpy((शून्य *)dst, (वर्ण *)src, len);
    }
}
