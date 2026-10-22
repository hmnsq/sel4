/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ଅନ୍ତର୍ଭାଵ <config.h>

#ଯଦି defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#ଅନ୍ତର୍ଭାଵ <arch/machine/capdl.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <kernel/cspace.h>
#ଅନ୍ତର୍ଭାଵ <machine/io.h>
#ଅନ୍ତର୍ଭାଵ <arch/object/iospace.h>


ଶୂନ୍ୟ x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    printf("(ports: (%lu, %lu))\n",
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

ଶୂନ୍ୟ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    printf("(domain_id: %lu, pci_device: %lu)\n",
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_io_space_cap_get_capDomainID(iospace_cap));
}

ଶୂନ୍ୟ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    printf("(level: %lu)\n", (long unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
