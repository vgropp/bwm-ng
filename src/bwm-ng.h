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

#include "../config.h"


/* ugly defines to handle different compile time options */

#if HAVE_STRUCT_IF_MSGHDR_IFM_DATA
#define SYSCTL
#endif

#if NETSTAT_LINUX || NETSTAT_BSD || NETSTAT_BSD_BYTES
#define NETSTAT
#endif

#if HAVE_LIBSTATGRAB
#define LIBSTATGRAB
#endif

#if HAVE__PROC_NET_DEV
#if PROC_NET_DEV_PATH
#define PROC_NET_DEV PROC_NET_DEV_PATH
#else
#define PROC_NET_DEV "/proc/net/dev"
#endif
#endif

#if HAVE_LIBCURSES || HAVE_LIBNCURSES
#define CURSES
#endif

#if HAVE_STRUCT_IF_DATA_IFI_IBYTES
#define GETIFADDRS
#endif

#if HAVE_IOCTL
#define IOCTL
#endif

#if HAVE_GETOPT_LONG
#define LONG_OPTIONS
#endif

#ifdef NETSTAT
#if HAVE_NETSTAT_PATH
#define NETSTAT_PATH HAVE_NETSTAT_PATH
#else
#define NETSTAT_PATH "netstat"
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef CURSES
#include <curses.h>
#endif

#ifdef LONG_OPTIONS
#include <getopt.h>
#endif

#include <limits.h>
#include <signal.h>
#include <string.h>

#if HAVE_GETTIMEOFDAY
#include <sys/time.h>
#include <time.h>
#endif


#define MAJOR 0
#define MINOR 5
#define EXTRA "-pre1"

#define WRAP_AROUND 4294967296ull

#ifdef SYS_64BIT 
#undef WRAP_AROUND
#define WRAP_AROUND 18446744073709551616ull
#else
#ifdef __WORDSIZE
#if __WORDSIZE == 64
#undef WRAP_AROUND
#define WRAP_AROUND 18446744073709551616ull
#endif
#endif
#endif

#ifdef CURSES
#define CURSES_OUT 0
#endif
#define PLAIN_OUT 1
#ifdef CSV
#define CSV_OUT 2
#endif
#ifdef HTML
#define HTML_OUT 3
#endif
#define PLAIN_OUT_ONCE 4


#ifdef NETSTAT
#define NETSTAT_IN 1
#define INPUT_0 1
#define INPUT_METHODS_0 " netstat"
#else 
#define INPUT_METHODS_0 ""
#define INPUT_0 0
#endif

#ifdef PROC_NET_DEV
#define PROC_IN 2
#define INPUT_1 (INPUT_0 | 2)
#define INPUT_METHODS_1 INPUT_METHODS_0 " proc"
#else
#define INPUT_1 INPUT_0
#define INPUT_METHODS_1 INPUT_METHODS_0
#endif

#ifdef GETIFADDRS
#define GETIFADDRS_IN 4
#define INPUT_2 (INPUT_1 | 4)
#define INPUT_METHODS_2 INPUT_METHODS_1 " getifaddrs"
#else
#define INPUT_METHODS_2 INPUT_METHODS_1
#define INPUT_2 INPUT_1
#endif

#ifdef LIBSTATGRAB
#define LIBSTAT_IN 8
#define INPUT_3 (INPUT_2 | 8)
#define INPUT_METHODS_3 INPUT_METHODS_2 " libstatgrab"
#else
#define INPUT_METHODS_3 INPUT_METHODS_2
#define INPUT_3 INPUT_2
#endif

#ifdef SYSCTL
#define SYSCTL_IN 16
#define INPUT_MASK (INPUT_3 | 16)
#define INPUT_METHODS INPUT_METHODS_3 " sysctl"
#else
#define INPUT_METHODS INPUT_METHODS_3
#define INPUT_MASK INPUT_3
#endif




/* used for this nice spinning wheel */
#define IDLE_CHARS "-\\|/"


/* build output methods string: curses, plain, csv, html */
#define OUTPUT_METHODS_0 "plain"

#ifdef CURSES 
#define OUTPUT_METHODS_1 OUTPUT_METHODS_0 ", curses"
#else 
#define OUTPUT_METHODS_1 OUTPUT_METHODS_0
#endif

#ifdef CSV
#define OUTPUT_METHODS_2 OUTPUT_METHODS_1 ", csv"
#else
#define OUTPUT_METHODS_2 OUTPUT_METHODS_1
#endif

#ifdef HTML
#define OUTPUT_METHODS OUTPUT_METHODS_2 ", html"
#else 
#define OUTPUT_METHODS OUTPUT_METHODS_2
#endif

/* build short options */
#ifdef PROC_NET_DEV
#define SHORT_OPTIONS_0 ":ht:f:dVa:pI:i:o:c:DS"
#else
#define SHORT_OPTIONS_0 ":ht:dVa:pI:i:o:c:DS"
#endif

#ifdef HTML
#define SHORT_OPTIONS_1 SHORT_OPTIONS_0 "HR:"
#else
#define SHORT_OPTIONS_1 SHORT_OPTIONS_0
#endif

#if (NETSTAT_LINUX || NETSTAT_BSD) && NETSTAT_OPTION
#define SHORT_OPTIONS_2 SHORT_OPTIONS_1 "n:"
#else
#ifdef NETSTAT
#define SHORT_OPTIONS_2 SHORT_OPTIONS_1 "n"
#else 
#define SHORT_OPTIONS_2 SHORT_OPTIONS_1
#endif
#endif

#ifdef CSV
#define SHORT_OPTIONS SHORT_OPTIONS_2 "C:F:"
#else
#define SHORT_OPTIONS SHORT_OPTIONS_2
#endif

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

struct iface_stats {
    char    *if_name;
    unsigned long long rec;
    unsigned long long send;
    unsigned long long p_rec;
    unsigned long long p_send;
	unsigned long long e_rec;
	unsigned long long e_send;
#if HAVE_GETTIMEOFDAY
    struct timeval time;
#endif    
};

typedef struct iface_stats t_iface_stats;

#define print_version printf("Bandwidth Monitor NG (bmw-ng) v%i.%i%s\nCopyright (C) 2004 Volker Gropp <bwmng@gropp.org>\n",MAJOR,MINOR,EXTRA); 
