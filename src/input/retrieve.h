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

#ifndef __RETRIEVE_H
#define __RETRIEVE_H 

#include "../global_vars.h"
#include "../defines.h"
#include "../types.h"
#include "../process.h"
#include "../bwm-ng.h"


#ifdef IOCTL
/* following only for check_if_up and ioctl */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#ifdef HAVE_LINUX_IF_H
#include <linux/if.h>
#else
#include <net/if.h>
#endif

char check_if_up(char *ifname);

#endif


#define MAX_LINE_BUFFER 1024


#include "devstat.h"
#include "ioservice.h"
#include "getifaddrs.h"
#include "libkstat.h"
#include "libstatgrab.h"
#include "netstat.h"
#include "proc_diskstats.h"
#include "proc_net_dev.h"
#include "sysctl.h"
#include "win32.h"

inline void get_iface_stats(char _n);

#endif
