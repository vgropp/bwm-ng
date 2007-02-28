/******************************************************************************
 *  bwm-ng parsing and retrive stuff                                          *
 *                                                                            *
 *  Copyright (C) 2004-2007 Volker Gropp (bwmng@gropp.org)                    *
 *                                                                            *
 *  for more info read README.                                                *
 *                                                                            *
 *  This program is free software; you can redistribute it and/or modify      *
 *  it under the terms of the GNU General Public License as published by      *
 *  the Free Software Foundation; either version 2 of the License, or         *
 *  (at your option) any later version.                                       *
 *                                                                            *
 *  This program is distributed in the hope that it will be useful,           *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 *  GNU General Public License for more details.                              *
 *                                                                            *
 *  You should have received a copy of the GNU General Public License         *
 *  along with this program; if not, write to the Free Software               *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                            *
 *****************************************************************************/

#ifndef __SYSCTL_H
#define __SYSCTL_H 

#include "retrieve.h"

#if defined(SYSCTL) || defined(HAVE_SYS_DISK_H)
#ifndef GETIFADDRS
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#endif

#include <sys/param.h> /* netbsd fix */
#include <sys/sysctl.h>

#ifdef SYSCTL
#include <net/route.h>
#include <net/if_dl.h>

void get_iface_stats_sysctl (char verbose);

#endif

#ifdef HAVE_SYS_DISK_H
#include <sys/disk.h>
#endif

#if SYSCTLDISK_IN
void get_iface_stats_sysctldisk (char verbose);
#endif


#endif


#endif
