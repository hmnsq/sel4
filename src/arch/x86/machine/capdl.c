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
    چاپ_کن("(ports: (%lu, %lu))\n",
           (دراز بیعلامت صحيح)cap_io_پورت_cap_get_cap_IOPort_اولی_پورت(ioports_cap),
           (دراز بیعلامت صحيح)cap_io_پورت_cap_get_cap_IOPort_آخرین_پورت(ioports_cap));
}

#اگر_تعریف_شده CONFIG_IOMMU

فراغ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    چاپ_کن("(domain_id: %lu, pci_device: %lu)\n",
           (دراز بیعلامت صحيح)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (دراز بیعلامت صحيح)cap_io_space_cap_get_capDomainID(iospace_cap));
}

فراغ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    چاپ_کن("(level: %lu)\n", (دراز بیعلامت صحيح)cap_io_صفحه_جدول_cap_get_cap_IOPTLevel(iopt_cap));
}
#پایان_اگر

#پایان_اگر /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
