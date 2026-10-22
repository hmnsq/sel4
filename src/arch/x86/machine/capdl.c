/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#समावेश <config.h>

#अगर defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#समावेश <arch/machine/capdl.h>
#समावेश <string.h>
#समावेश <kernel/cspace.h>
#समावेश <machine/io.h>
#समावेश <arch/object/iospace.h>


व्योम x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    म_लिखो("(ports: (%lu, %lu))\n",
           (दीर्घ अचिन्हित पूर्णांक)cap_io_port_cap_get_capIOPortFirstPort(ioports_cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_io_port_cap_get_capIOPortLastPort(ioports_cap));
}

#अगर_घोषित CONFIG_IOMMU

व्योम x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    म_लिखो("(domain_id: %lu, pci_device: %lu)\n",
           (दीर्घ अचिन्हित पूर्णांक)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (दीर्घ अचिन्हित पूर्णांक)cap_io_space_cap_get_capDomainID(iospace_cap));
}

व्योम x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    म_लिखो("(level: %lu)\n", (दीर्घ अचिन्हित पूर्णांक)cap_io_page_table_cap_get_capIOPTLevel(iopt_cap));
}
#पूर्ण_अगर

#पूर्ण_अगर /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
