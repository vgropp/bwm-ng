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

#include "global_vars.h"
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
#if SYSCTLDISK_IN
			case SYSCTLDISK_IN:
				return "sysctldisk";
				break;
#endif				
#ifdef NETSTAT
        case NETSTAT_IN:
            return "netstat -i";
            break;
#endif
#ifdef LIBSTATGRAB
        case LIBSTAT_IN:
            return "libstatnet";
            break;
			case LIBSTATDISK_IN:
				return "libstatdisk";
				break;
#endif
#ifdef GETIFADDRS
        case GETIFADDRS_IN:
            return "getifaddrs";
            break;
#endif
#if DEVSTAT_IN
		  case DEVSTAT_IN:
				return "devstat";
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
		  case KSTATDISK_IN:
				return "kstatdisk";
				break;
#endif
#if IOSERVICE_IN
		  case IOSERVICE_IN:
				return "ioservice disk IO";
				break;
#endif
#ifdef PROC_DISKSTATS
		  case DISKLINUX_IN:
				return "disk IO";
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
				if (net_input_method(input_method))
	            return " (all and down)";
				else
					return " (all)";
            break;
    }
    return "";
}



int print_header(int option) {
#if HTML
    FILE *tmp_out_file;
#endif
#ifdef HAVE_CURSES
    unsigned int row=0;
    unsigned int col=0;
    unsigned int width=0;
#endif
	switch (output_method) {
#ifdef HAVE_CURSES
		case CURSES_OUT:
	        erase();
		    mvwprintw(stdscr,1,2,"bwm-ng v" VERSION" (probing every %2.3fs), press 'h' for help",(float)delay/1000);
            mvwprintw(stdscr,2,2,"input: %s type: %s",input2str(),output_type2str());
            wprintw(stdscr,show_all_if2str());
	        mvwprintw(stdscr,3,2,"%c         iface                   Rx                   Tx                Total",(char)IDLE_CHARS[option]);
	        /* go to next char for next run */
	        option++;
			if (option>3) option=0;
	        mvwprintw(stdscr,4,2,"==============================================================================");
			break;
		case CURSES2_OUT:
			erase();
			if (cols<48 || rows<45) 
				mvwprintw(stdscr,1,2,"window size too small.\n  it has to be at least 48x45.");
			else {
				width=(cols-3-16-4)/3;
				mvwprintw(stdscr,1,2,"+---{ bwm-ng v" VERSION" }");
				for (col=17+sizeof(VERSION);col<32+cols-48;col++) 
					mvwprintw(stdscr,1,col,"-");
				mvwprintw(stdscr,1,32+cols-48,"+- -- - -- -->");
				mvwprintw(stdscr,2,2,"|"), mvwprintw(stdscr,2,32+cols-48,"|------.");
				for (col=0;col<width-2;col++) { 
					mvwaddch(stdscr,2,col+6,ACS_HLINE); 
					attron(COLOR_PAIR(1));mvwprintw(stdscr,35,col+6," ");attroff(COLOR_PAIR(1));
					mvwaddch(stdscr,2,2*width+6+col,ACS_HLINE); 
					attron(COLOR_PAIR(2));mvwprintw(stdscr,35,2*width+6+col," ");attroff(COLOR_PAIR(2));
				};
				for (row=3;row<=36;row++) { 
					mvwprintw(stdscr,row,2,"|"); 
					mvwprintw(stdscr,row,32+cols-48,"|"); 
				}
				mvwprintw(stdscr,34,33+cols-48,"<"); mvwprintw(stdscr,35,33+cols-48,"------'");
				mvwprintw(stdscr,36,((width-8)/2)+5,"%c%c%cRx%c%c%c",
						IDLE_CHARS2[option+2],
						IDLE_CHARS2[option+1],
						IDLE_CHARS2[option],
						IDLE_CHARS2[9-option],
						IDLE_CHARS2[8-option],
						IDLE_CHARS2[7-option]);
				mvwprintw(stdscr,36,((width-8)/2)+5+2*width,"%c%c%cTx%c%c%c",
						IDLE_CHARS2[9-option],
						IDLE_CHARS2[8-option],
						IDLE_CHARS2[7-option],
						IDLE_CHARS2[option+2],
						IDLE_CHARS2[option+1],
						IDLE_CHARS2[option]);


				mvwprintw(stdscr,37,2,"+"); mvwprintw(stdscr,37,32+cols-48,"+");
				for (col=3;col<32+cols-48;col++) 
					mvwprintw(stdscr,37,col,"-");

				mvwprintw(stdscr,38,2,"`+--> %c probing every: %2.3fs",(char)IDLE_CHARS[option],(float)delay/1000);
				mvwprintw(stdscr,39,2," +-----> interface: wait...   ");
				mvwprintw(stdscr,40,2," +--------> type: %s",output_type2str());
				mvwprintw(stdscr,41,2," `-----------> input: %s",input2str());
				scale=max_rt/32;
				/* print scale */
				if (max_rt>=1024) {
					for (row=0;row<=31;row++) 
						mvwprintw(stdscr,row+3,34+cols-48,"%2.2fM|",(float)(max_rt-row*scale)/1024);
				} else {
					for (row=0;row<=31;row++) 
						mvwprintw(stdscr,row+3,34+cols-48,"%4uk|",max_rt-row*scale);
				}
			};
			/* go to next char for next run */
			option++;
			if (option>3) option=0;
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
	        fprintf(tmp_out_file,"<div class=\"bwm-ng-header\">bwm-ng bwm-ng v" VERSION " (refresh %is); input: ",html_refresh);
            fprintf(tmp_out_file,input2str());
            fprintf(tmp_out_file,show_all_if2str());
	        fprintf(tmp_out_file,"</div><table class=\"bwm-ng-output\">");
			fprintf(tmp_out_file,"<tr class=\"bwm-ng-head\"><td class=\"bwm-ng-name\">Interface</td><td>Rx</td><td>Tx</td><td>Total</td></tr>");
			break;
#endif			
		case PLAIN_OUT_ONCE:
		case PLAIN_OUT:
			if (output_method==PLAIN_OUT && ansi_output) printf("\033[1;2H");
	        printf("bwm-ng v" VERSION " (delay %2.3fs); ",(float)delay/1000);
			if (output_method==PLAIN_OUT) printf("press 'ctrl-c' to end this%s",(ansi_output ? "\033[2;2H" : "")); else printf("input: ");
            printf(input2str());
            printf("%s\n",show_all_if2str());
			if (output_method==PLAIN_OUT) {
				if (ansi_output)
					printf("\033[3;2H");
				printf("%c",(char)IDLE_CHARS[option]); 
			} else printf(" ");
			printf("         iface                    Rx                   Tx               Total\n");
			if (output_method==PLAIN_OUT && ansi_output) printf("\033[4;2H");
	        printf("==============================================================================\n");
			/* go to next char for next run */
			option++;
			if (option>3) option=0;
			break;
	}
	return option;
}


inline ullong direction2value(char mode,struct inout_long stats) {
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
inline double direction_max2value(char mode,struct inouttotal_double stats,int items) {
    switch (mode) {
        case 0:
            return (double)(stats.in/items);
        case 1:
            return (double)(stats.out/items);
        case 2:
            return (double)(stats.total/items);
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
	 {
        strlcpy(speed,"/s",3);
	 } else {
        strlcpy(speed,"  ",3);
	 }
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
                value=(double)direction_max2value(mode,full_stats.max.packets,1);
                break;
            case AVG_OUT:
                value=(double)direction_max2value(mode,full_stats.avg.item_sum.packets,full_stats.avg.items);
                break;
#endif                
        }
        snprintf(str,buf_size,"%16.2f %c%s",(double)value,net_input_method(input_method) ? 'P' : '#',speed);
    } else {
        switch (output_unit) {
            case BITS_OUT:
            case BYTES_OUT:
                switch (output_type) {
                    case RATE_OUT:
                        value=(double)direction2value(mode,stats.bytes)*multiplier;
                        break;
#if EXTENDED_STATS
                    case SUM_OUT:
                        value=direction2value(mode,full_stats.sum.bytes);
                        break;
                    case MAX_OUT:
                        value=(double)direction_max2value(mode,full_stats.max.bytes,1);
                        break;
                    case AVG_OUT:
                        value=(double)direction_max2value(mode,full_stats.avg.item_sum.bytes,full_stats.avg.items);
#endif
                }
                if (output_unit==BITS_OUT) {
                    byte_char='b';
                    value*=8;
                } else
                     byte_char='B';
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
                        value=(double)direction_max2value(mode,full_stats.max.errors,1);
                        break;
                    case AVG_OUT:
                        value=(double)direction_max2value(mode,full_stats.avg.item_sum.errors,full_stats.avg.items);
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
   t_iface_speed_stats *stats_csv = NULL;
   t_double_types stats_csv_d;
#ifdef HAVE_CURSES
	unsigned int row=0;
	unsigned int col=0;
	unsigned int width=0;
	int i=0, j=0;
	char adjust=0;
#endif
    switch (output_method) {
#ifdef HAVE_CURSES		
        case CURSES_OUT:
            mvwprintw(stdscr,y,x,"%15s:",if_name); /* output the name */
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
        case CURSES2_OUT:
	  if (cols<48 || rows<45) mvwprintw(stdscr,1,2,"window size too small.\n  it has to be at least 49x46.");
	  else {
	    if (show_only_if+5==y) { /* show only one interface at a time */
	      width=(cols-3-16-4)/3;
	      mvwprintw(stdscr,39,2," +-----> interface: %s        ",if_name); /* output the name */
	      switch (output_type) {
	      case RATE_OUT:
		i=(int)(stats.bytes.in*multiplier/1024); /* incoming */
		j=(int)(stats.bytes.out*multiplier/1024); /* outgoing */
		break;
#ifdef EXTENDED_STATS
	      case MAX_OUT:
		i=(int)(full_stats.max.bytes.in/1024);
		j=(int)(full_stats.max.bytes.out/1024);
		break;
	      case SUM_OUT:
		i=(int)(full_stats.sum.bytes.in/1024);
		j=(int)(full_stats.sum.bytes.out/1024);
		break;
	      case AVG_OUT:
		i=(int)(full_stats.avg.item_sum.bytes.in/1024);
		j=(int)(full_stats.avg.item_sum.bytes.out/1024);
		break;
#endif
	      };
	      /* adjust scale */
	      if (i>max_rt || j>max_rt) {
		adjust=1;
		if (i>j) max_rt=i; else max_rt=j;
	      } else if (i<=max_rt-24*scale && j<=max_rt-24*scale) { adjust=1; max_rt=31*scale; };
	      if (adjust) {
		adjust=!adjust;
		if (max_rt<32) max_rt=32;
		/* print scale */
		scale=max_rt/32;
		if (max_rt>=1024) for (row=0;row<=31;row++) mvwprintw(stdscr,row+3,34+cols-48,"%2.2fM|",(float)(max_rt-row*scale)/1024);
		else for (row=0;row<=31;row++) mvwprintw(stdscr,row+3,3+cols-484,"%4uk|",max_rt-row*scale);
	      };
	      /* print bar (incoming) */
	      if (i>0) { if (i<(max_rt-31*scale)) i=1; else i=(i-(max_rt-31*scale))/scale+2; };
	      for (row=34;row>=3;row--) {
		if (i>0) attron(COLOR_PAIR(1));
		for (col=0;col<width;col++) mvwaddch(stdscr,row,col+5,ACS_HLINE);
		if (i>0) { attroff(COLOR_PAIR(1)); i--; }
	      };
	      /* print bar (outgoing) */
	      if (j>0) { if (j<(max_rt-31*scale)) j=1; else j=(j-(max_rt-31*scale))/scale+2; };
	      for (row=34;row>=3;row--) {
		if (j>0) attron(COLOR_PAIR(2));
		for (col=0;col<width;col++) mvwaddch(stdscr,row,2*width+5+col,ACS_HLINE);
		if (j>0) { attroff(COLOR_PAIR(2)); j--; }
	      };
	    } else if (show_only_if+6==y) if (strcmp("total",if_name)==0) show_only_if=0;
	  };
            break;
#endif
		case PLAIN_OUT_ONCE:
        case PLAIN_OUT:
			if (output_method==PLAIN_OUT && ansi_output) printf("\033[%d;2H",y);
            printf("%15s:",if_name); /* output the name */
            printf("%s %s %s\n",
                values2str(0,stats,full_stats,multiplier,buffer,49),
                values2str(1,stats,full_stats,multiplier,buffer,49),
                values2str(2,stats,full_stats,multiplier,buffer,49));
            break;
#ifdef HTML			
		case HTML_OUT:
            tmp_out_file=out_file==NULL ? stdout : out_file;
			fprintf(tmp_out_file,"<tr><td class=\"bwm-ng-name\">%15s:</td><td class=\"bwm-ng-in\"><span class=\"bwm-ng-%s\">",if_name,
                    (stats.errors.in && output_unit!=ERRORS_OUT) ? "error" : "dummy");
            fprintf(tmp_out_file,"%s</span> </td><td class=\"bwm-ng-out\"><span class=\"bwm-ng-%s\">",
                    values2str(0,stats,full_stats,multiplier,buffer,49),
                    (stats.errors.out && output_unit!=ERRORS_OUT) ? "error" : "dummy");
            fprintf(tmp_out_file,"%s</span> </td><td class=\"bwm-ng-total\"><span class=\"bwm-ng-%s\">",
                    values2str(1,stats,full_stats,multiplier,buffer,49),
                    ((stats.errors.out || stats.errors.in) && output_unit!=ERRORS_OUT) ? "error" : "dummy" );
            fprintf(tmp_out_file,"%s</span></td><tr>\n",values2str(2,stats,full_stats,multiplier,buffer,49));
            break;
#endif
#ifdef CSV
        case CSV_OUT:
				tmp_out_file=out_file==NULL ? stdout : out_file;
            fprintf(tmp_out_file,"%i%c%s%c",(int)time(NULL),csv_char,if_name,csv_char);
				if (output_type == RATE_OUT || output_type == SUM_OUT) {
					if (output_type == RATE_OUT) {
						stats_csv = &stats;
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
	               if (input_method!=NETSTAT_IN)
#endif
                   /* output Bytes/s */
                   fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c%llu%c%llu%c",(double)(stats_csv->bytes.out*multiplier),csv_char,(double)(stats_csv->bytes.in*multiplier),csv_char,(double)((stats_csv->bytes.out+stats_csv->bytes.in)*multiplier),csv_char,stats_csv->bytes.in,csv_char,stats_csv->bytes.out,csv_char);
   	            /* show packets/s and errors/s */
      	         fprintf(tmp_out_file,"%.2f%c%.2f%c%.2f%c%llu%c%llu",(double)stats_csv->packets.out*multiplier,csv_char,(double)stats_csv->packets.in*multiplier,csv_char,(double)(stats_csv->packets.out+stats_csv->packets.in)*multiplier,csv_char,stats_csv->packets.in,csv_char,stats_csv->packets.out);
         	      fprintf(tmp_out_file,"%c%.2f%c%.2f%c%llu%c%llu\n",csv_char,stats_csv->errors.out*multiplier,csv_char,stats_csv->errors.in*multiplier,csv_char,stats_csv->errors.in,csv_char,stats_csv->errors.out);

					} else {
						stats_csv = &full_stats.sum;
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
						if (input_method!=NETSTAT_IN)
#endif                    
						 /* output Bytes */
						 fprintf(tmp_out_file,"%llu%c%llu%c%llu%c",stats_csv->bytes.out,csv_char,stats_csv->bytes.in,csv_char,(stats_csv->bytes.out+stats_csv->bytes.in),csv_char);
						/* show packets and errors */
						fprintf(tmp_out_file,"%llu%c%llu%c%llu",stats_csv->packets.out,csv_char,stats_csv->packets.in,csv_char,(stats_csv->packets.out+stats_csv->packets.in));
						fprintf(tmp_out_file,"%c%llu%c%llu\n",csv_char,stats_csv->errors.out,csv_char,stats_csv->errors.in);
						}
				} else { /* MAX_OUT or AVG_OUT */
					if (output_type == MAX_OUT)
						stats_csv_d = full_stats.max;
					else {
						stats_csv_d.bytes.out = full_stats.avg.item_sum.bytes.out/full_stats.avg.items;
						stats_csv_d.bytes.in = full_stats.avg.item_sum.bytes.in/full_stats.avg.items;
						stats_csv_d.bytes.total = full_stats.avg.item_sum.bytes.total/full_stats.avg.items;
						stats_csv_d.packets.out = full_stats.avg.item_sum.packets.out/full_stats.avg.items;
						stats_csv_d.packets.in = full_stats.avg.item_sum.packets.in/full_stats.avg.items;
						stats_csv_d.packets.total = full_stats.avg.item_sum.packets.total/full_stats.avg.items;
						stats_csv_d.errors.out = full_stats.avg.item_sum.errors.out/full_stats.avg.items;
						stats_csv_d.errors.in = full_stats.avg.item_sum.errors.in/full_stats.avg.items;
						stats_csv_d.errors.total = full_stats.avg.item_sum.errors.total/full_stats.avg.items;
					}
#if !NETSTAT_BSD_BYTES && !NETSTAT_NETBSD && NETSTAT
               if (input_method!=NETSTAT_IN)
#endif
                   /* output Bytes/s */
                   fprintf(tmp_out_file,"%.2Lf%c%.2Lf%c%.2Lf%c",stats_csv_d.bytes.out,csv_char,stats_csv_d.bytes.in,csv_char,stats_csv_d.bytes.total,csv_char);
               /* show packets/s and errors/s */
               fprintf(tmp_out_file,"%.2Lf%c%.2Lf%c%.2Lf",stats_csv_d.packets.out,csv_char,stats_csv_d.packets.in,csv_char,stats_csv_d.packets.total);
               fprintf(tmp_out_file,"%c%.2Lf%c%.2Lf\n",csv_char,stats_csv_d.errors.out,csv_char,stats_csv_d.errors.in);
						
				}
            break;
#endif			
    }
}

