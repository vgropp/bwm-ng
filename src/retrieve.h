/******************************************************************************
 *  bwm-ng parsing and retrive stuff                                          *
 *                                                                            *
 *  Copyright (C) 2004 Volker Gropp (vgropp@pefra.de)                         *
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

#include "defines.h"
#include "types.h"


#ifdef HAVE_LIBKSTAT
#include <kstat.h>
#endif

#ifdef GETIFADDRS
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#endif

#ifdef SYSCTL
#ifndef GETIFADDRS
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#endif

#include <sys/param.h> /* netbsd fix */
#include <sys/sysctl.h>
#include <net/route.h>
#include <net/if_dl.h>
#endif


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
#endif


#ifdef LIBSTATGRAB
#include <statgrab.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <iphlpapi.h>
#endif


#define MAX_LINE_BUFFER 1024

#include "process.h"
#include "bwm-ng.h"

inline void get_iface_stats(char _n);

#endif
