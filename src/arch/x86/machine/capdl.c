/*
 * Copyright 2014, General Dynamics C4 Systems
 *
 * SPDX-License-Identifier: GPL-2.0-only
 */

#شامل <config.h>

#اگر defined(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING)

#شامل <arch/machine/capdl.h>
#شامل <string.h>
#شامل <kernel/cspace.h>
#شامل <machine/io.h>
#شامل <arch/object/iospace.h>


خالی x86_obj_ioports_print_attrs(cap_t ioports_cap)
{
    لکھو("(ports: (%lu, %lu))\n",
           (طویل غیرعلامتی صحیح)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(ioports_cap),
           (طویل غیرعلامتی صحیح)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(ioports_cap));
}

#اگر_تعریف_شدہ CONFIG_IOMMU

خالی x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    لکھو("(domain_id: %lu, pci_device: %lu)\n",
           (طویل غیرعلامتی صحیح)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (طویل غیرعلامتی صحیح)cap_io_space_cap_get_capDomainID(iospace_cap));
}

خالی x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    لکھو("(level: %lu)\n", (طویل غیرعلامتی صحیح)cap_io_صفحہ_جدول_cap_get_cap_IOPTLevel(iopt_cap));
}
#اگر_ختم

#اگر_ختم /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
