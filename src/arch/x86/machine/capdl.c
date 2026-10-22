/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ضمن <config.h>

#إذا defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#ضمن <arch/machine/capdl.h>
#ضمن <string.h>
#ضمن <kernel/cspace.h>
#ضمن <machine/io.h>
#ضمن <arch/object/iospace.h>


فراغ x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    لکو("(ports: (%lu, %lu))\n",
           (ڊگهو بنانشان صحيح)cap_io_پورٽ_cap_get_cap_IOPort_پهريون_پورٽ(ioports_cap),
           (ڊگهو بنانشان صحيح)cap_io_پورٽ_cap_get_cap_IOPort_آخري_پورٽ(ioports_cap));
}

#جيڪڏهن_وصف_ٿيل CONFIG_IOMMU

فراغ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    لکو("(domain_id: %lu, pci_device: %lu)\n",
           (ڊگهو بنانشان صحيح)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (ڊگهو بنانشان صحيح)cap_io_space_cap_get_capDomainID(iospace_cap));
}

فراغ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    لکو("(level: %lu)\n", (ڊگهو بنانشان صحيح)cap_io_صفحو_جدول_cap_get_cap_IOPTLevel(iopt_cap));
}
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
