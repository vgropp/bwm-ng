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

#include "global_vars.h"
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
inline void cmdln_printhelp(void) FUNCATTR_NORETURN;
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
    print_help_line("-d","-d, --dynamic [value]","show values dynamicly (Byte KB or MB)");
    print_help_line("-a [mode]","-a, --allif [mode]","where mode is one of:");
    print_help_line("","","0=show only up (and selected) interfaces");
    print_help_line("","","1=show all up interfaces (default)");
    print_help_line("","","2=show all and down interfaces");
    print_help_line("-I <list>","-I, --interfaces <list>","show only interfaces in <list> (comma seperated), or");
    print_help_line("","","if list is prefaced with % show all but interfaces");
    print_help_line("","","in list");
    print_help_line("-S","-S, --sumhidden [value]","count hidden interfaces for total");
#if EXTENDED_STATS    
    print_help_line("-A","-A, --avglength <sec>","sets the span of average stats (Default 30s)");
#endif    
    print_help_line("-D","-D, --daemon [value]","fork into background and daemonize");
    print_help_line("-h","-h, --help","displays this help");
    print_help_line("-V","-V, --version","print version info");
    printf("\nInput:\n");
    print_help_line("-i <method>","-i, --input <method>","input method, one of:" INPUT_METHODS);
#ifdef PROC_NET_DEV
    print_help_line("-f <file>","-f, --procfile <file>","filename to read raw data from. (" PROC_NET_DEV ")");
#endif
#ifdef PROC_DISKSTATS
	 print_help_line("","    --diskstatsfile <file>","filename to read diskstats (Linux 2.6+) from. (" PROC_DISKSTATS ")");
	 print_help_line("","    --partitionsfile <file>","filename to read diskstats (Linux 2.4) from. (" PROC_PARTITIONS ")");
#endif	 
#if ALLOW_NETSTATPATH
#ifdef NETSTAT
    print_help_line("-n <path>","-n, --netstat <path>","use <path> as netstat binary");
#endif
#endif
    printf("\nOutput:\n");
    print_help_line("-o <method>","-o, --output <method>","output method, one of: ");
	 print_help_line("","",OUTPUT_METHODS);
    print_help_line("-u","-u, --unit <value>","unit to show. one of bytes, bits, packets, errors");
#if EXTENDED_STATS    
    print_help_line("-T","-T, --type <value>","type of stats. one of rate, max, sum, avg");
#endif    
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
	 print_help_line("-N","-N, --ansiout","disable ansi codes for plain output");
    print_help_line("","","(ie 1 for one single output)");
    printf("\n");
    exit(EXIT_SUCCESS);
}

#ifdef HAVE_CURSES
void print_online_help() {
    WINDOW *helpwin;
    helpwin=newwin(15,76,0,1);
    box(helpwin,ACS_VLINE, ACS_HLINE);
    mvwprintw(helpwin,0,2,"bwm-ng v" VERSION " - Keybindings:");
    mvwprintw(helpwin,2,2,"'h'  show this help");
    mvwprintw(helpwin,3,2,"'q'  exit");
    mvwprintw(helpwin,4,2,"'+'  increases timeout by 100ms");
    mvwprintw(helpwin,5,2,"'-'  decreases timeout by 100ms");
    if (output_method==CURSES_OUT) 
		mvwprintw(helpwin,6,2,"'d'  switch KB and auto assign Byte/KB/MB/GB");
    else 
		mvwprintw(helpwin,6,2,"'d'  cycle: show interfaces step by step");
    mvwprintw(helpwin,7,2,"'a'  cycle: show all interfaces, only those which are up,");
    mvwprintw(helpwin,8,2,"            only up and not hidden");
    mvwprintw(helpwin,9,2,"'s'  sum hidden ifaces to total aswell or not");
    mvwprintw(helpwin,10,2,"'n'  cycle: input methods");
    if (output_method==CURSES_OUT) { mvwprintw(helpwin,11,2,"'u'  cycle: bytes,bits,packets,errors");
#if EXTENDED_STATS    
    mvwprintw(helpwin,12,2,"'t'  cycle: current rate, max, sum since start, average for last 30s");
#endif 
    }
    else {
#if EXTENDED_STATS    
      mvwprintw(helpwin,11,2,"'t'  cycle: current rate, max, sum since start, average for last 30s");
#endif    
    };
    mvwprintw(helpwin,14,2," press any key to continue... ");
    wrefresh(helpwin);
    timeout(-1);
    getch();
    timeout(delay);
    delwin(helpwin);
}
#endif
