/******************************************************************************
 *  bwm-ng online help                                                        *
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

#include "help.h"

inline void print_help_line(char *short_c,char * long_c,char *descr) {
#ifdef LONG_OPTIONS
    printf("  %-23s",long_c);
#else
    printf("  %-23s",short_c);
#endif
    printf(" %s\n",descr);
}

/* prints a helpscreen and exists */
inline void cmdln_printhelp() {
    print_version;
    printf("USAGE: bwm-ng [OPTION] ...");
#if CONFIG_FILE
    printf(" [CONFIGFILE]\n");
#else
    printf("\n");
#endif
    printf("displays current ethernet interfaces stats\n\n");
    printf("Options:\n");
    print_help_line("-t <msec>","-t, --timeout <msec>","displays stats every <msec> (1msec = 1/1000sec)");
    print_help_line("","","default: 500");
    print_help_line("-d","-d, --dynamic","show values dynamicly (Byte KB or MB)");
    print_help_line("-a [mode]","-a, --allif [mode]","where mode is one of:");
    print_help_line("","","0=show only up (and selected) interfaces");
    print_help_line("","","1=show all up interfaces (default)");
    print_help_line("","","2=show all and down interfaces");
    print_help_line("-I <list>","-I, --interfaces <list>","show only interfaces in <list> (comma seperated), or");
    print_help_line("","","if list is prefaced with %% show all but interfaces");
    print_help_line("","","in list");
    print_help_line("-S","-S, --sumhidden","count hidden interfaces for total");
    print_help_line("-D","-D, --daemon","fork into background and daemonize");
    print_help_line("-h","-h, --help","displays this help");
    print_help_line("-V","-V, --version","print version info");
    printf("\nInput:\n");
    print_help_line("-i <method>","-i, --input <method>","input method, one of:" INPUT_METHODS);
#ifdef PROC_NET_DEV
    print_help_line("-f <file>","-f, --procfile <file>","filename to read raw data from. (" PROC_NET_DEV ")");
#endif
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    print_help_line("-n <path>","-n, --netstat <path>","use <path> as netstat binary");
#endif
#endif
    printf("\nOutput:\n");
    print_help_line("-o <method>","-o, --output <method>","output method, one of: " OUTPUT_METHODS);
    print_help_line("-u","-u, --unit <value>","unit to show. one of bytes, bits, packets, errors");
    print_help_line("-T","-T, --type <value>","type of stats. one of rate, max, sum, avg");
#ifdef CSV
    print_help_line("-C <char>","-C, --csvchar <char>","delimiter for csv");
#endif
#if CSV || HTML
    print_help_line("-F <file>","-F, --outfile <file>","output file for csv and html (default stdout)");
#endif
#ifdef HTML
    print_help_line("-R <num>","-R, --htmlrefresh <num>","meta refresh for html output");
    print_help_line("-H","-H, --htmlheader","show <html> and <meta> frame for html output");
#endif
    print_help_line("-c <num>","-c, --count <num>","number of query/output for plain & csv");
    print_help_line("","","(ie 1 for one single output)");
    printf("\n");
    exit(0);
}

