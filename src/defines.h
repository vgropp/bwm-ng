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

#if NETSTAT_LINUX || NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS || NETSTAT_NETBSD
#define NETSTAT 1
#endif

#if HAVE_LIBSTATGRAB
#define LIBSTATGRAB
#endif

#if HAVE__PROC_NET_DEV
#ifdef PROC_NET_DEV_PATH
#define PROC_NET_DEV PROC_NET_DEV_PATH
#else
#define PROC_NET_DEV "/proc/net/dev"
#endif
#endif

#if HAVE_LIBCURSES || HAVE_LIBNCURSES
#define HAVE_CURSES
#endif

#if HAVE_STRUCT_IF_DATA_IFI_IBYTES
#define GETIFADDRS
#endif

#if HAVE_IOCTL
#include <sys/ioctl.h>
#if SIOCGIFFLAGS
#define IOCTL
#endif
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
#include <errno.h>

#ifdef HAVE_CURSES
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
#define EXTRA "-pre2-cvs"

#define WRAP_AROUND ULONG_MAX
#define WRAP_32BIT 4294967296ul

#ifdef SYS_64BIT 
#undef WRAP_AROUND
#define WRAP_AROUND 18446744073709551616ull
#endif

#ifdef HAVE_CURSES
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
#define INPUT_4 (INPUT_3 | 16)
#define INPUT_METHODS_4 INPUT_METHODS_3 " sysctl"
#else
#define INPUT_METHODS_4 INPUT_METHODS_3
#define INPUT_4 INPUT_3
#endif

#if HAVE_LIBKSTAT
#define KSTAT_IN 32
#define INPUT_MASK (INPUT_4 | 32)
#define INPUT_METHODS INPUT_METHODS_4 " kstat"
#else
#define INPUT_MASK INPUT_4
#define INPUT_METHODS INPUT_METHODS_4
#endif




/* used for this nice spinning wheel */
#define IDLE_CHARS "-\\|/"


/* build output methods string: curses, plain, csv, html */
#define OUTPUT_METHODS_0 "plain"

#ifdef HAVE_CURSES 
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
#define SHORT_OPTIONS_0 ":ht:f:dVa:u:I:i:o:c:DST:"
#else
#define SHORT_OPTIONS_0 ":ht:dVa:u:I:i:o:c:DST:"
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

#define BYTES_OUT 1
#define BITS_OUT 2
#define PACKETS_OUT 3
#define ERRORS_OUT 4

#define RATE_OUT 1
#define MAX_OUT 2
/* planned for future 
#define SUM_OUT 3
#define AVERAGE_OUT 4
*/

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define print_version printf("Bandwidth Monitor NG (bmw-ng) v%i.%i%s\nCopyright (C) 2004,2005 Volker Gropp <bwmng@gropp.org>\n",MAJOR,MINOR,EXTRA); 
