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

#ifdef __RETRIEVE_H
#else
#define __RETRIEVE_H 1

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
#include <net/if.h>
#endif


#ifdef LIBSTATGRAB
#include <statgrab.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <iphlpapi.h>
#endif


#define MAX_LINE_BUFFER 1024

extern int process_if_data (int hidden_if, t_iface_speed_stats tmp_if_stats,t_iface_speed_stats *stats, char *name, int iface_number,char verbose, char iface_is_up);
extern void finish_iface_stats (char verbose, t_iface_speed_stats stats, int hidden_if, int iface_number);
#ifdef __STDC__
extern void deinit(int code, char *error_msg, ...) FUNCATTR_NORETURN;
#else
extern void deinit(int code, ...) FUNCATTR_NORETURN;
#endif

extern int if_count;
#ifdef PROC_NET_DEV
extern char PROC_FILE[PATH_MAX];
#endif
#ifdef PROC_DISKSTATS
extern char PROC_DISKSTATS_FILE[PATH_MAX];
#endif
extern unsigned int delay;
extern char dynamic;
extern char show_all_if;
extern char sumhidden;
extern char output_unit;
extern char output_type;
extern int output_method;
extern int input_method;
extern char *iface_list;


#ifdef IOCTL
/* fd for check_if_up and ioctl */
int skfd = -1;
#endif

#endif
