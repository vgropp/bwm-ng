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

#ifndef __HAVE_DEFINE_H
#define __HAVE_DEFINE_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <string.h>

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

#if HAVE_LIBCURSES || HAVE_LIBNCURSES
#define HAVE_CURSES
#endif

#if HAVE_WORKING_GETIFADDRS 
#define GETIFADDRS
#endif

#if HAVE_GETOPT_LONG
#define LONG_OPTIONS
#endif

#if HAVE_IOCTL
#include <sys/ioctl.h>
#ifdef SIOCGIFFLAGS
#define IOCTL
#endif
#endif

#ifdef HAVE_CURSES
#include <curses.h>
#ifndef ACS_VLINE
#define ACS_VLINE '|'
#endif
#ifndef ACS_HLINE
#define ACS_HLINE '-'
#endif
#endif

#ifdef LONG_OPTIONS
#include <getopt.h>
#endif

#if HAVE_GETTIMEOFDAY
#include <sys/time.h>
#include <time.h>
#endif

#ifdef NETSTAT
#if HAVE_NETSTAT_PATH
#define NETSTAT_PATH HAVE_NETSTAT_PATH
#else
#define NETSTAT_PATH "netstat"
#endif
#endif

#if HAVE__PROC_NET_DEV
#ifdef PROC_NET_DEV_PATH
#define PROC_NET_DEV PROC_NET_DEV_PATH
#else
#define PROC_NET_DEV "/proc/net/dev"
#endif
#endif

#if HAVE__PROC_DISKSTATS
#ifdef PROC_DISKSTATS_PATH
#define PROC_DISKSTATS PROC_DISKSTATS_PATH
#else
#define PROC_DISKSTATS "/proc/diskstats"
#endif
#ifdef PROC_PARTITIONS_PATH
#define PROC_PARTITIONS PROC_PARTITIONS_PATH
#else
#define PROC_PARTITIONS "/proc/partitions"
#endif
#endif

/* prepare input methods */
#ifdef NETSTAT
#define NETSTAT_IN 1
#define INPUT_METHODS_NETSTAT " netstat"
#else
#define NETSTAT_IN 0
#define INPUT_METHODS_NETSTAT
#endif

#ifdef PROC_NET_DEV
#define PROC_IN 2
#define INPUT_METHODS_PROC " proc"
#else
#define PROC_IN 0
#define INPUT_METHODS_PROC
#endif

#ifdef GETIFADDRS
#define GETIFADDRS_IN 4
#define INPUT_METHODS_GETIFADDRS " getifaddrs"
#else
#define GETIFADDRS_IN 0
#define INPUT_METHODS_GETIFADDRS
#endif

#ifdef LIBSTATGRAB
#define LIBSTAT_IN 8
#define LIBSTATDISK_IN 256
#define INPUT_METHODS_LIBSTATGRAB " libstatgrab"
#define INPUT_METHODS_LIBSTATDISK " libstatdisk"
#else
#define LIBSTAT_IN 0
#define INPUT_METHODS_LIBSTATGRAB
#define LIBSTATDISK_IN 0
#define INPUT_METHODS_LIBSTATDISK
#endif

#ifdef SYSCTL
#define SYSCTL_IN 16
#define INPUT_METHODS_SYSCTL " sysctl"
#else
#define SYSCTL_IN 0
#define INPUT_METHODS_SYSCTL
#endif

#if defined(HAVE_STRUCT_DISKSTATS) || defined(HAVE_STRUCT_DISK_SYSCTL)
#define SYSCTLDISK_IN 1024
#define INPUT_METHODS_SYSCTLDISK " sysctldisk"
#else
#define SYSCTLDISK_IN 0
#define INPUT_METHODS_SYSCTLDISK
#endif


#if HAVE_LIBKSTAT
#define KSTAT_IN 32
#define KSTATDISK_IN	512
#define INPUT_METHODS_KSTAT " kstat"
#define INPUT_METHODS_KSTATDISK " kstatdisk"
#else 
#define KSTAT_IN 0
#define KSTATDISK_IN 0
#define INPUT_METHODS_KSTAT
#define INPUT_METHODS_KSTATDISK
#endif

#ifdef WIN32
#define WIN32_IN 64
#define INPUT_METHODS_WIN32 " win32"
#else
#define WIN32_IN 0
#define INPUT_METHODS_WIN32
#endif

#ifdef PROC_DISKSTATS
#define DISKLINUX_IN 128
#define INPUT_METHODS_DISK_LINUX " disk"
#else
#define DISKLINUX_IN 0
#define INPUT_METHODS_DISK_LINUX
#endif

#if defined(HAVE_DEVSTAT_GETDEVS) || defined(HAVE_GETDEVS)
#define DEVSTAT_IN 2048
#define INPUT_METHODS_DEVSTAT " devstat"
#else
#define DEVSTAT_IN 0
#define INPUT_METHODS_DEVSTAT
#endif

#ifdef HAVE_IOSERVICE
#define IOSERVICE_IN 4096
#define INPUT_METHODS_IOSERVICE " ioservice"
#else
#define IOSERVICE_IN 0
#define INPUT_METHODS_IOSERVICE
#endif

#define INPUT_MASK (NETSTAT_IN | PROC_IN | GETIFADDRS_IN | LIBSTAT_IN | SYSCTL_IN | KSTAT_IN | KSTATDISK_IN | WIN32_IN | DISKLINUX_IN | LIBSTATDISK_IN | SYSCTLDISK_IN | DEVSTAT_IN | IOSERVICE_IN)
#define NET_INPUT (NETSTAT_IN | PROC_IN | GETIFADDRS_IN | LIBSTAT_IN | SYSCTL_IN | KSTAT_IN | WIN32_IN)
#define INPUT_METHODS INPUT_METHODS_PROC INPUT_METHODS_GETIFADDRS INPUT_METHODS_SYSCTL INPUT_METHODS_KSTAT INPUT_METHODS_NETSTAT INPUT_METHODS_WIN32 INPUT_METHODS_LIBSTATGRAB INPUT_METHODS_DISK_LINUX INPUT_METHODS_LIBSTATDISK INPUT_METHODS_KSTATDISK INPUT_METHODS_SYSCTLDISK INPUT_METHODS_DEVSTAT INPUT_METHODS_IOSERVICE

#define net_input_method(_n) (_n & NET_INPUT)

/* used for this nice spinning wheel */
#define IDLE_CHARS "-\\|/"


/* build output methods string: curses, plain, csv, html */
#ifdef HAVE_CURSES 
#define CURSES_OUTPUT_METHODS ", curses, curses2"
#define CURSES_OUT 0
#define CURSES2_OUT 5
#define IDLE_CHARS2 "   >--<   "
#else
#define CURSES_OUTPUT_METHODS
#endif

#ifdef CSV
#define CSV_OUTPUT_METHODS ", csv"
#define CSV_OUT 2
#else
#define CSV_OUTPUT_METHODS
#endif

#ifdef HTML
#define HTML_OUTPUT_METHODS ", html"
#define HTML_OUT 3
#else
#define HTML_OUTPUT_METHODS
#endif

#define OUTPUT_METHODS "plain" CURSES_OUTPUT_METHODS CSV_OUTPUT_METHODS HTML_OUTPUT_METHODS
#define PLAIN_OUT 1
#define PLAIN_OUT_ONCE 4

/* build short options */
#ifdef PROC_NET_DEV
#define PROC_SHORT_OPT "f:"
#else 
#define PROC_SHORT_OPT
#endif

#ifdef HTML
#define HTML_SHORT_OPT "H:R:"
#else
#define HTML_SHORT_OPT
#endif

#if NETSTAT && NETSTAT_OPTION
#define NETSTAT_SHORT_OPT "n:"
#else 
#define NETSTAT_SHORT_OPT
#endif

#ifdef CSV
#define CSV_SHORT_OPT "C:F:"
#else 
#define CSV_SHORT_OPT
#endif

#if EXTENDED_STATS
#define EXTENDED_STATS_SHORT_OPT "A:T:"
#else
#define EXTENDED_STATS_SHORT_OPT
#endif

#define SHORT_OPTIONS ":ht:d:Va:u:I:i:o:c:D:S:N" EXTENDED_STATS_SHORT_OPT PROC_SHORT_OPT HTML_SHORT_OPT NETSTAT_SHORT_OPT CSV_SHORT_OPT

#define BYTES_OUT 1
#define BITS_OUT 2
#define PACKETS_OUT 3
#define ERRORS_OUT 4

#define RATE_OUT 1
#if EXTENDED_STATS
#define MAX_OUT 2
#define SUM_OUT 3
#define AVG_OUT 4
#define TYPE_OUT_MAX 4
#else
#define TYPE_OUT_MAX 1
#endif

/* default length of avg in 1/1000sec */
#define AVG_LENGTH 30000

#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

#define WRAP_AROUND ULONG_MAX
#define WRAP_32BIT 4294967295ul

#ifdef SYS_64BIT
#ifdef HAVE_UNSIGNED_LONG_LONG

#undef WRAP_AROUND
#define WRAP_AROUND 18446744073709551615ull

#endif
#endif

#define print_version printf("Bandwidth Monitor NG (" PACKAGE_NAME ") v" VERSION "\nCopyright (C) 2004-2007 Volker Gropp <" PACKAGE_BUGREPORT ">\n"); 

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif

#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#ifndef HAVE_STRLCPY
#define strlcpy(_a,_b,_c) strncpy((_a),(_b),(_c)); (_a)[(_c) - 1]='\0';
#endif

#endif
