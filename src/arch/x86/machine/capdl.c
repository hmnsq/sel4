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
    لکھو("(ports: (%lu, %lu))\n",
           (لما بنانشانی صحيح)cap_io_پورٹ_cap_get_cap_IOPort_پہلا_پورٹ(ioports_cap),
           (لما بنانشانی صحيح)cap_io_پورٹ_cap_get_cap_IOPort_آخری_پورٹ(ioports_cap));
}

#جے_تعریف_شدہ CONFIG_IOMMU

فراغ x86_obj_iospace_print_attrs(cap_t iospace_cap)
{
    لکھو("(domain_id: %lu, pci_device: %lu)\n",
           (لما بنانشانی صحيح)cap_io_space_cap_get_capPCIDevice(iospace_cap),
           (لما بنانشانی صحيح)cap_io_space_cap_get_capDomainID(iospace_cap));
}

فراغ x86_obj_iopt_print_attrs(cap_t iopt_cap)
{
    لکھو("(level: %lu)\n", (لما بنانشانی صحيح)cap_io_ورقہ_جدول_cap_get_cap_IOPTLevel(iopt_cap));
}
#جے_انت

#جے_انت /* defind(CONFIG_DEBUG_BUILD) && defined(CONFIG_PRINTING) */
