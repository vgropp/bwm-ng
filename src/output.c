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
#if EXTENDED_STATS
    static char str[25];
#endif
    switch (output_type) {
        case RATE_OUT:
            return "rate";
            break;
#if EXTENDED_STATS            
        case MAX_OUT:
            return "max";
            break;
        case SUM_OUT:
            return "sum";
            break;
        case AVG_OUT:
            snprintf(str,24,"avg (%is)",(int)avg_length/1000);
            return str;
            break;
#endif            
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
		    mvwprintw(stdscr,1,2,"bwm-ng v%i.%i%s (probing every %2.3fs), press 'h' for help",MAJOR,MINOR,EXTRA,(float)delay/1000);
            mvwprintw(stdscr,2,2,"input: %s type: %s",input2str(),output_type2str());
            wprintw(stdscr,show_all_if2str());
	        mvwprintw(stdscr,3,2,"%c       iface                  Rx                   Tx                Total",(char)IDLE_CHARS[option]);
	        /* go to next char for next run */
	        option++;
			if (option>3) option=0;
	        mvwprintw(stdscr,4,2,"===========================================================================");
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
			printf("       iface                  Rx                   Tx                Total\n");
			if (output_method==PLAIN_OUT) printf("\033[4;2H");
	        printf("===========================================================================\n");
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

#if EXTENDED_STATS
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
#endif

inline char *dyn_byte_value2str(double value,char *str,int buf_size) {
    if (dynamic) {
        if (value<1024)
            snprintf(str,buf_size,"%15.2f  ",value);
        else
            if (value<1048576)
                snprintf(str,buf_size,"%15.2f K",value/1024);
            else
                if (value<1073741824)
                    snprintf(str,buf_size,"%15.2f M",value/1048576);
                else
                    snprintf(str,buf_size,"%15.2f G",value/1073741824);
    } else {
        snprintf(str,buf_size,"%15.2f K",value/1024);
    }
    return str;
}

char *values2str(char mode,t_iface_speed_stats stats,t_iface_stats full_stats,float multiplier,char *str,int buf_size) {
    char byte_char=' ';
    char speed[3];
    double value=0;
    char *str_buf=NULL;
    if (output_type==RATE_OUT 
#if EXTENDED_STATS
            || output_type==MAX_OUT || output_type==AVG_OUT
#endif
            ) 
        strcpy(speed,"/s"); 
    else 
        strcpy(speed,"  ");
    if (
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
        input_method==NETSTAT_IN ||
#endif
        output_unit==PACKETS_OUT) {
        switch (output_type) {
            case RATE_OUT:
                value=(double)direction2value(mode,stats.packets)*multiplier;
                break;
#if EXTENDED_STATS                
            case SUM_OUT:
                value=direction2value(mode,full_stats.sum.packets);
                break;
            case MAX_OUT:
                value=(double)direction_max2value(mode,full_stats.max.packets);
                break;
            case AVG_OUT:
                value=(double)direction_max2value(mode,full_stats.avg.value.packets);
                break;
#endif                
        }
        snprintf(str,buf_size,"%16.2f P%s",(double)value,speed);
    } else {
        switch (output_unit) {
            case BITS_OUT:
            case BYTES_OUT:
                if (output_unit==BYTES_OUT) byte_char='B';
                switch (output_type) {
                    case RATE_OUT:
                        value=(double)direction2value(mode,stats.bytes)*multiplier;
                        break;
#if EXTENDED_STATS
                    case SUM_OUT:
                        value=direction2value(mode,full_stats.sum.bytes);
                        break;
                    case MAX_OUT:
                        value=(double)direction_max2value(mode,full_stats.max.bytes);
                        break;
                    case AVG_OUT:
                        value=(double)direction_max2value(mode,full_stats.avg.value.bytes);
#endif
                }
                if (output_unit==BITS_OUT) {
                    byte_char='b';
                    value*=8;
                }
                str_buf=(char *)malloc(buf_size);
                snprintf(str,buf_size,"%s%c%s",dyn_byte_value2str(value,str_buf,buf_size),byte_char,speed);
                break;
            case ERRORS_OUT:
                switch (output_type) {
                    case RATE_OUT:
                        value=(double)direction2value(mode,stats.errors)*multiplier;
                        break;
#if EXTENDED_STATS
                    case SUM_OUT:
                        value=direction2value(mode,full_stats.sum.errors);
                        break;
                    case MAX_OUT:
                        value=(double)direction_max2value(mode,full_stats.max.errors);
                        break;
                    case AVG_OUT:
                        value=(double)direction_max2value(mode,full_stats.avg.value.errors);
                        break;
#endif
                }
                snprintf(str,buf_size,"%16.2f E%s",(double)value,speed);
                break;
        }
    }
    if (str_buf!=NULL) free(str_buf);
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
#if HAVE_WATTRON            
            if (stats.errors.in && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
#endif            
            wprintw(stdscr,"%s",values2str(0,stats,full_stats,multiplier,buffer,49));
#if HAVE_WATTRON            
            if (stats.errors.in && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
#endif            
            wprintw(stdscr," ");
#if HAVE_WATTRON            
            if (stats.errors.out && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
#endif            
            wprintw(stdscr,"%s",values2str(1,stats,full_stats,multiplier,buffer,49));
#if HAVE_WATTRON            
            if (stats.errors.out && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
#endif            
            wprintw(stdscr," ");
#if HAVE_WATTRON            
            if ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) wattron(stdscr, A_REVERSE);
#endif            
            wprintw(stdscr,"%s",values2str(2,stats,full_stats,multiplier,buffer,49));
#if HAVE_WATTRON            
            if ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) wattroff(stdscr, A_REVERSE);
#endif            
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
			fprintf(tmp_out_file,"<tr><td class='bwm-ng-name'>%12s:</td><td class='bwm-ng-in'><span class='bwm-ng-%s'>",if_name,
                    (stats.errors.in && output_unit!=ERRORS_OUT) ? "error" : "dummy");
            fprintf(tmp_out_file,"%s</span> </td><td class='bwm-ng-out'><span class='bwm-ng-%s'>",
                    values2str(0,stats,full_stats,multiplier,buffer,49),
                    (stats.errors.out && output_unit!=ERRORS_OUT) ? "error" : "dummy");
            fprintf(tmp_out_file,"%s</span> </td><td class='bwm-ng-total'><span class='bwm-ng-%s'>",
                    values2str(1,stats,full_stats,multiplier,buffer,49),
                    ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) ? "error" : "dummy" );
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
                fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c%llu%c%llu%c",(double)(stats.bytes.out*multiplier),csv_char,(double)(stats.bytes.in*multiplier),csv_char,(double)((stats.bytes.out+stats.bytes.in)*multiplier),csv_char,stats.bytes.in,csv_char,stats.bytes.out,csv_char);
            /* show packets/s and errors/s */
            fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c%llu%c%llu",(double)stats.packets.out*multiplier,csv_char,(double)stats.packets.in*multiplier,csv_char,(double)(stats.packets.out+stats.packets.in)*multiplier,csv_char,stats.packets.in,csv_char,stats.packets.out);
            fprintf(tmp_out_file,"%c%.2f%c%.2f%c%llu%c%llu\n",csv_char,stats.errors.out*multiplier,csv_char,stats.errors.in*multiplier,csv_char,stats.errors.in,csv_char,stats.errors.out);
            break;
#endif			
    }
}

