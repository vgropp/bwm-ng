/******************************************************************************
 *  bwm-ng parsing stuff                                                      *
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

#ifdef CONFIG_FILE
#include <ctype.h>
#include <string.h>
#ifdef HAVE_GETPWUID
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif
#endif



int if_count=0;
#ifdef PROC_NET_DEV
char PROC_FILE[PATH_MAX];
#endif
#ifdef NETSTAT
char NETSTAT_FILE[PATH_MAX];
#endif
unsigned int delay=500;
char show_kb=1;
char show_all_if=0;
char show_packets=0;
#ifdef HAVE_CURSES
int output_method=CURSES_OUT;
WINDOW *myscr=NULL;
#else
int output_method=PLAIN_OUT;
#endif
char *iface_list=NULL;
#ifdef CSV
char csv_char=';';
#endif
#if CSV || HTML
FILE *out_file=NULL;
char *out_file_path=NULL;
#endif
int output_count=-1;
char daemonize=0;
char sumhidden=0;

#ifdef PROC_NET_DEV
int input_method=PROC_IN;
#else
#ifdef GETIFADDRS
int input_method=GETIFADDRS_IN;
#else
#ifdef LIBSTATGRAB
int input_method=LIBSTAT_IN;
#else
#ifdef SYSCTL
int input_method=SYSCTL_IN;
#else
#if HAVE_LIBKSTAT
int input_method=KSTAT_IN;
#else
#ifdef NETSTAT
int input_method=NETSTAT_IN;
#else
#error "NO INPUT DEFINED!"
int input_method=0;
#endif
#endif 
#endif
#endif
#endif
#endif

#ifdef HTML
int html_refresh=5;
int html_header=0;
#endif
