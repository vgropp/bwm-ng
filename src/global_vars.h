/******************************************************************************
 *  bwm-ng global variables                                                   *
 *                                                                            *
 *  Copyright (C) 2007 Volker Gropp (bwmng@gropp.org)                         *
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

#ifndef __GLOBAL_VARS
#define __GLOBAL_VARS
#ifndef EXTERN
#define EXTERN extern
#endif

#include "defines.h"
#include "types.h"

EXTERN int if_count;
#ifdef PROC_NET_DEV
EXTERN char PROC_FILE[PATH_MAX];
#endif
#ifdef PROC_DISKSTATS
EXTERN char PROC_DISKSTATS_FILE[PATH_MAX];
EXTERN char PROC_PARTITIONS_FILE[PATH_MAX];
#endif
#ifdef NETSTAT
EXTERN char NETSTAT_FILE[PATH_MAX];
#endif
EXTERN unsigned int delay;
#if EXTENDED_STATS
EXTERN unsigned int avg_length;
#endif
EXTERN char output_unit;
EXTERN char output_type;
EXTERN char dynamic;
 
EXTERN char show_all_if;
#ifdef HAVE_CURSES
EXTERN int output_method;
EXTERN WINDOW *mywin;
EXTERN unsigned short cols;
EXTERN unsigned short rows;
EXTERN unsigned int max_rt;
EXTERN unsigned int scale;
EXTERN unsigned int show_only_if;
#else
EXTERN int output_method;
#endif
EXTERN char *iface_list;
#ifdef CSV
EXTERN char csv_char;
#endif
#if CSV || HTML
EXTERN FILE *out_file;
EXTERN char *out_file_path;
#endif
EXTERN int output_count;
EXTERN char daemonize;
EXTERN char sumhidden;
EXTERN char ansi_output;

EXTERN int input_method;

#ifdef HTML
EXTERN int html_refresh;
EXTERN int html_header;
#endif

#ifdef IOCTL
EXTERN int skfd;
#endif

#if IOSERVICE_IN
EXTERN char long_darwin_disk_names;
#endif

EXTERN t_iface_stats *if_stats;
/* total struct */
EXTERN t_iface_stats if_stats_total;

#endif
