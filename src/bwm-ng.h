/******************************************************************************
 *  bwm-ng                                                                    *
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

#include "defines.h"
#include "types.h"

#if HAVE_SYS_VARARGS_H
#include <sys/varargs.h> /* solaris */
#else
#include <stdarg.h>
#endif

extern void get_cmdln_options(int argc, char *argv[]);
extern int print_header(int option);

/* global vars and options */
extern int if_count;
#ifdef PROC_NET_DEV
extern char PROC_FILE[PATH_MAX];
#endif
extern unsigned int delay;
extern char show_kb;
extern char show_all_if;
extern char show_bits;
extern char show_packets;
extern char sumhidden;
extern int output_method;
extern int input_method;
extern char *iface_list;
#ifdef CSV
extern char csv_char;
#endif
#if CSV || HTML
extern FILE *out_file;
extern char *out_file_path;
#endif
extern int output_count;
extern char daemonize;
#ifdef HTML
extern int html_refresh;
extern int html_header;
#endif
#ifdef HAVE_CURSES
extern WINDOW *myscr;
#endif

#ifdef IOCTL
/* fd for check_if_up and ioctl */
extern int skfd;
#endif

/* global buffer to store all data of interfaces in */
extern t_iface_stats *if_stats;
/* total struct */
extern t_iface_stats if_stats_total;


#ifdef PROC_NET_DEV
extern void get_iface_stats_proc (char verbose);
#endif

#ifdef LIBSTATGRAB
extern void get_iface_stats_libstat (char verbose);
#endif

#ifdef GETIFADDRS
extern void get_iface_stats_getifaddrs (char verbose);
#endif

#ifdef SYSCTL
extern void get_iface_stats_sysctl (char verbose);
#endif

#ifdef NETSTAT
extern void get_iface_stats_netstat (char verbose);
#endif

#ifdef HAVE_LIBKSTAT
extern void get_iface_stats_kstat (char verbose);
#endif

