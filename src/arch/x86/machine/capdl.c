/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#अन्तर्भाव <config.h>

#यदि defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#अन्तर्भाव <arch/machine/capdl.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <kernel/cspace.h>
#अन्तर्भाव <machine/io.h>
#अन्तर्भाव <arch/object/iospace.h>


शून्य x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    म_लिखो("(ports: (%lu, %lu))\n",
           (दीर्घ अचिन्हित पूर्णाङ्क)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (दीर्घ अचिन्हित पूर्णाङ्क)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#ifdef CONFIG_IOMMU

शून्य x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    म_लिखो("(domain_id: %lu, pci_device: %lu)\n",
           (दीर्घ अचिन्हित पूर्णाङ्क)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (दीर्घ अचिन्हित पूर्णाङ्क)cap_io_space_cap_get_capDomainID(iospace_cap));
}

शून्य x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    म_लिखो("(level: %lu)\n", (दीर्घ अचिन्हित पूर्णाङ्क)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#endif

#endif /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
