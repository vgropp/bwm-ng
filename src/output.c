/******************************************************************************
 *  bwm-ng output                                                             *
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

#include "output.h"


inline char *output_type2str() {
    switch (output_type) {
        case RATE_OUT:
            return "rate";
            break;
        case MAX_OUT:
            return "max";
            break;
    }
    return "";
}


inline char *input2str() {
    switch (input_method) {
#ifdef SYSCTL
        case SYSCTL_IN:
            return "sysctl";
            break;
#endif
#ifdef NETSTAT
        case NETSTAT_IN:
            return "netstat -i";
            break;
#endif
#ifdef LIBSTATGRAB
        case LIBSTAT_IN:
            return "libstatgrab";
            break;
#endif
#ifdef GETIFADDRS
        case GETIFADDRS_IN:
            return "getifaddrs";
            break;
#endif
#ifdef PROC_NET_DEV
        case PROC_IN:
            return "/proc/net/dev";
            break;
#endif
#if HAVE_LIBKSTAT
        case KSTAT_IN:
            return "kstat";
            break;
#endif
    }
    return "";
}

inline char *show_all_if2str() {
    switch (show_all_if) {
        case 1:
            return " (all)";
            break;
        case 2:
            return " (all and down)";
            break;
    }
    return "";
}

int print_header(int option) {
#if HTML
    FILE *tmp_out_file;
#endif    
	switch (output_method) {
#ifdef HAVE_CURSES
		case CURSES_OUT:
	        erase();
		    mvwprintw(stdscr,1,8,"bwm-ng v%i.%i%s (probing every %2.3fs), press 'q' to end this",MAJOR,MINOR,EXTRA,(float)delay/1000);
            mvwprintw(stdscr,2,8,"input: %s type: %s",input2str(),output_type2str());
            wprintw(stdscr,show_all_if2str());
	        mvwprintw(stdscr,3,8,"%c       iface               Rx                Tx             Total",(char)IDLE_CHARS[option]);
	        /* go to next char for next run */
	        option++;
			if (option>3) option=0;
	        mvwprintw(stdscr,4,8,"==================================================================");
			break;
#endif
#ifdef HTML
		case HTML_OUT:
            tmp_out_file=out_file==NULL ? stdout : out_file;
			if (html_header) {
		        fprintf(tmp_out_file,"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\">\n<html>\n<head>\n");
		        fprintf(tmp_out_file,"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n");
		        fprintf(tmp_out_file,"<META HTTP-EQUIV=Refresh CONTENT=\"%i\">\n",html_refresh);
		        fprintf(tmp_out_file,"<link rel=\"stylesheet\" href=\"bwm-ng.css\" type=\"text/css\" media=\"screen,projection,print\">\n");
		        fprintf(tmp_out_file,"<title>bwm-ng stats</title>\n</head>\n<body>\n");
			}
	        fprintf(tmp_out_file,"<div class='bwm-ng-header'>bwm-ng bwm-ng v%i.%i%s (refresh %is); input: ",MAJOR,MINOR,EXTRA,html_refresh);
            fprintf(tmp_out_file,input2str());
            fprintf(tmp_out_file,show_all_if2str());
	        fprintf(tmp_out_file,"</div><table class='bwm-ng-output'>");
			fprintf(tmp_out_file,"<tr class='bwm-ng-head'><td class='bwm-ng-name'>Interface</td><td>Rx</td><td>Tx</td><td>Total</td></tr>");
			break;
#endif			
		case PLAIN_OUT_ONCE:
		case PLAIN_OUT:
			if (output_method==PLAIN_OUT) printf("\033[1;2H");
	        printf("bwm-ng v%i.%i%s (delay %2.3fs); ",MAJOR,MINOR,EXTRA,(float)delay/1000);
			if (output_method==PLAIN_OUT) printf("press 'ctrl-c' to end this\033[2;2H"); else printf("input: ");
            printf(input2str());
            printf("%s\n",show_all_if2str());
			if (output_method==PLAIN_OUT) {
				printf("\033[3;2H");
				printf("%c",(char)IDLE_CHARS[option]); 
			} else printf(" ");
			printf("       iface               Rx                Tx             Total\n");
			if (output_method==PLAIN_OUT) printf("\033[4;2H");
	        printf("==================================================================\n");
			/* go to next char for next run */
			option++;
			if (option>3) option=0;
			break;
	}
	return option;
}


inline unsigned long long direction2value(char mode,struct inout_long stats) {
    switch (mode) {
        case 0:
            return stats.in;
        case 1:
            return stats.out;
        case 2: 
            return stats.in+stats.out;
    }
    return 0;
}

inline double direction_max2value(char mode,struct inouttotal_double stats) {
    switch (mode) {
        case 0:
            return stats.in;
        case 1:
            return stats.out;
        case 2:
            return stats.total;
    }
    return 0;
}


char *values2str(char mode,t_iface_speed_stats stats,t_iface_stats full_stats,float multiplier,char *str,int buf_size) {
    char byte_char=' ';
    double value=0;
    if (
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
        input_method==NETSTAT_IN ||
#endif
        output_unit==PACKETS_OUT) {
        switch (output_type) {
            case RATE_OUT:
                value=(double)direction2value(mode,stats.packets)*multiplier;
                break;
            case MAX_OUT:
                value=(double)direction_max2value(mode,full_stats.max.packets);
                break;
        }
        snprintf(str,buf_size,"%13.2f P/s",(double)value);
    } else {
        if (output_unit==BITS_OUT || output_unit==BYTES_OUT) {
            if (output_unit==BYTES_OUT) byte_char='B';
            switch (output_type) {
                case RATE_OUT:
                    value=(double)direction2value(mode,stats.bytes)*multiplier;
                    break;
                case MAX_OUT:
                    value=(double)direction_max2value(mode,full_stats.max.bytes);
                    break;
            }
            if (output_unit==BITS_OUT) {
                byte_char='b';
                value*=8;
            }
            if (dynamic) {
                if (value<1024)
                    snprintf(str,buf_size,"%12.2f  %c/s",value,byte_char);
                else
                    if (value<1048576)
                        snprintf(str,buf_size,"%12.2f K%c/s",value/1024,byte_char);
                    else
                        if (value<1073741824)
                            snprintf(str,buf_size,"%12.2f M%c/s",value/1048576,byte_char);
                        else
                            snprintf(str,buf_size,"%12.2f G%c/s",value/1073741824,byte_char);
            } else {
                snprintf(str,buf_size,"%12.2f K%c/s",value/1024,byte_char);
            }
        }
        if (output_unit==ERRORS_OUT) {
            switch (output_type) {
                case RATE_OUT:
                    value=(double)direction2value(mode,stats.errors)*multiplier;
                    break;
                case MAX_OUT:
                    value=(double)direction_max2value(mode,full_stats.max.errors);
                    break;
            }
            snprintf(str,buf_size,"%13.2f E/s",(double)value);
        }
    }
   return str;
}

/* do the actual output */
void print_values(int y,int x,char *if_name,t_iface_speed_stats stats,float multiplier,t_iface_stats full_stats) {
    char buffer[50];
#if CSV || HTML
	FILE *tmp_out_file;
#endif
    switch (output_method) {
#ifdef HAVE_CURSES		
        case CURSES_OUT:
            mvwprintw(stdscr,y,x,"%12s:",if_name); /* output the name */
            if (stats.errors.in && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
            wprintw(stdscr,"%s",values2str(0,stats,full_stats,multiplier,buffer,49));
            if (stats.errors.in && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
            wprintw(stdscr," ");
            if (stats.errors.out && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
            wprintw(stdscr,"%s",values2str(1,stats,full_stats,multiplier,buffer,49));
            if (stats.errors.out && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
            wprintw(stdscr," ");
            if ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
            wprintw(stdscr,"%s",values2str(2,stats,full_stats,multiplier,buffer,49));
            if ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
            break;
#endif
		case PLAIN_OUT_ONCE:
        case PLAIN_OUT:
			if (output_method==PLAIN_OUT) printf("\033[%d;2H",y);
            printf("%12s:",if_name); /* output the name */
            printf("%s %s %s\n",
                values2str(0,stats,full_stats,multiplier,buffer,49),
                values2str(1,stats,full_stats,multiplier,buffer,49),
                values2str(2,stats,full_stats,multiplier,buffer,49));
            break;
#ifdef HTML			
		case HTML_OUT:
            tmp_out_file=out_file==NULL ? stdout : out_file;
			fprintf(tmp_out_file,"<tr><td class='bwm-ng-name'>%12s:</td>",if_name);
			fprintf(tmp_out_file,"<td class='bwm-ng-in'>");
            if (stats.errors.in && output_unit!=ERRORS_OUT) 
                fprintf(tmp_out_file,"<span class='bwm-ng-error'>"); 
            else 
                fprintf(tmp_out_file,"<span class='bwm-ng-dummy'>");
            fprintf(tmp_out_file,"%s</span> </td>",values2str(0,stats,full_stats,multiplier,buffer,49));
            fprintf(tmp_out_file,"<td class='bwm-ng-out'>");
            if (stats.errors.out && output_unit!=ERRORS_OUT) 
                fprintf(tmp_out_file,"<span class='bwm-ng-error'>");
            else
                fprintf(tmp_out_file,"<span class='bwm-ng-dummy'>");
            fprintf(tmp_out_file,"%s</span> </td>",values2str(1,stats,full_stats,multiplier,buffer,49));
            fprintf(tmp_out_file,"<td class='bwm-ng-total'>");
            if ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT)
                fprintf(tmp_out_file,"<span class='bwm-ng-error'>");
            else
                fprintf(tmp_out_file,"<span class='bwm-ng-dummy'>");
            fprintf(tmp_out_file,"%s</span></td><tr>\n",values2str(2,stats,full_stats,multiplier,buffer,49));
            break;
#endif
#ifdef CSV
        case CSV_OUT:
			tmp_out_file=out_file==NULL ? stdout : out_file;
            fprintf(tmp_out_file,"%i%c%s%c",(int)time(NULL),csv_char,if_name,csv_char);
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
			if (input_method!=NETSTAT_IN)
#endif                    
                /* output Bytes/s */
                fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c",(double)(stats.bytes.out*multiplier),csv_char,(double)(stats.bytes.in*multiplier),csv_char,(double)((stats.bytes.out+stats.bytes.in)*multiplier),csv_char);
            /* show packets/s and errors/s */
            fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c%llu%c%llu\n",(double)stats.packets.out*multiplier,csv_char,(double)stats.packets.in*multiplier,csv_char,(double)(stats.packets.out+stats.packets.in)*multiplier,csv_char,stats.errors.out,csv_char,stats.errors.in);
            break;
#endif			
    }
}

