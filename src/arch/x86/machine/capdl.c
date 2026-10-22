/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#અન્તર્ભાવ <config.h>

#જો defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#અન્તર્ભાવ <arch/machine/capdl.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <kernel/cspace.h>
#અન્તર્ભાવ <machine/io.h>
#અન્તર્ભાવ <arch/object/iospace.h>


શૂન્ય x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    printf("(ports: (%lu, %lu))\n",
           (long unsigned પૂર્ણાંક)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (long unsigned પૂર્ણાંક)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

શૂન્ય x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    printf("(domain_id: %lu, pci_device: %lu)\n",
           (long unsigned પૂર્ણાંક)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (long unsigned પૂર્ણાંક)cap_io_space_cap_get_capDomainID(iospace_cap));
}

શૂન્ય x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    printf("(level: %lu)\n", (long unsigned પૂર્ણાંક)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
