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

#include "bwm-ng.h"

#if HAVE_SYS_VARARGS_H
#include <sys/varargs.h>
#endif

extern void get_cmdln_options(int argc, char *argv[]);
extern int print_header(int option);

/* global vars and options */
extern int if_count;
#ifdef PROC_NET_DEV
extern char PROC_FILE[PATH_MAX];
#endif
extern unsigned int delay;
extern char show_kb;
extern char show_all_if;
extern char show_packets;
extern char sumhidden;
extern int output_method;
extern unsigned int input_method;
extern char *iface_list;
#ifdef CSV
extern char csv_char;
extern FILE *csv_file;
#endif
extern int output_count;
extern char daemonize;
#ifdef HTML
extern int html_refresh;
extern int html_header;
#endif

#ifdef IOCTL
/* fd for check_if_up and ioctl */
extern int skfd;
#endif

/* global buffer to store all data of interfaces in */
extern t_iface_stats *if_stats;
/* total struct */
extern t_iface_stats if_stats_total;


#ifdef PROC_NET_DEV
extern void get_iface_stats_proc (char verbose);
#endif

#ifdef LIBSTATGRAB
extern void get_iface_stats_libstat (char verbose);
#endif

#ifdef GETIFADDRS
extern void get_iface_stats_getifaddrs (char verbose);
#endif

#ifdef SYSCTL
extern void get_iface_stats_sysctl (char verbose);
#endif

#ifdef NETSTAT
extern void get_iface_stats_netstat (char verbose);
#endif

inline void get_iface_stats(char _n) {
	switch (input_method) { 
#ifdef NETSTAT		
	    case NETSTAT_IN: 
			get_iface_stats_netstat(_n); 
	        break; 
#endif			
#ifdef LIBSTATGRAB 
		case LIBSTAT_IN: 
            get_iface_stats_libstat(_n); 
            break; 
#endif 
#ifdef PROC_NET_DEV 
	    case PROC_IN: 
            get_iface_stats_proc(_n); 
            break; 
#endif 
#ifdef GETIFADDRS 
	    case GETIFADDRS_IN: 
            get_iface_stats_getifaddrs(_n); 
            break; 
#endif 
#ifdef SYSCTL
        case SYSCTL_IN:
            get_iface_stats_sysctl(_n);
            break;
#endif
			
	}
}


/* clear stuff */
int deinit(char *error_msg, ...) FUNCATTR_NORETURN;
void sigint(int sig) FUNCATTR_NORETURN;


int deinit(char *error_msg, ...) {
    va_list    ap;
#ifdef CURSES	
	if (output_method==CURSES_OUT) {
		/* first close curses, so we dont leave mess behind */
		endwin();
		curs_set(1);
	}
#endif	
#ifdef IOCTL	
	/* close socket if we opened it for ioctl */
	if (skfd >= 0) { 
		close(skfd);
	}
#endif	
	/* we should clean if_state, i think, */
	if (if_stats!=NULL) free(if_stats);
	/* free the opt iface_list */
	if (iface_list!=NULL) free(iface_list);
#ifdef CSV	
	/* close the csv_file */
	if (csv_file!=NULL) fclose(csv_file);
#endif	
	/* output errormsg if given */
	if (error_msg!=NULL) {
        va_start(ap, error_msg);
		vprintf(error_msg,ap);
	}
	/* we are done, say goodbye */
    exit(0);
}


/* sigint handler */
void sigint(int sig) {
	/* we got a sigint, call deinit and exit */
	deinit(NULL);
}


#ifdef CURSES
/* handle key input by user in gui (curses) mode */
void handle_gui_input(char c) {
    switch (c) {
        /* lets check for known keys */
        case '+': /* increase delay */
            delay+=100;
            timeout(delay);
            break;
        case '-': /* decrease delay */
            if (delay>100) {
                delay+=-100;
                timeout(delay);
            }
            break;
        case 'p':
        case 'P':
            show_packets=!show_packets;
            break;
        case 'a':
        case 'A':
            show_all_if++;
            if (show_all_if>2) show_all_if=0;
            if (iface_list==NULL && show_all_if==1) show_all_if=2;
            // get stats so all values are uptodate
            get_iface_stats(0);
            // a short sleep, else we get "nan" values due to very short 
            // delay till next get_iface_stats
            usleep(100);
            break;
        case 's':
        case 'S':
            sumhidden=!sumhidden;
            // get stats so all values are uptodate
            get_iface_stats(0);
            // a short sleep, else we get "nan" values due to very short
            // delay till next get_iface_stats
            usleep(100);
            break;
        case 'n':
        case 'N':
			do {
				input_method=input_method<<1;
				if (input_method>INPUT_MASK) input_method=1;
			} while (!(input_method & INPUT_MASK)); 
            /* switched input, reset iface stats */
            free(if_stats);
            if_stats=NULL;
            if_count=0;
            memset(&if_stats_total,0,(size_t)sizeof(t_iface_stats));
            break;
        case 'q':
        case 'Q':
            /* we are asked to exit */
            deinit(NULL);
            break;
        case 'd':
        case 'D':
        case 'k':
        case 'K':
            /* switch kilobyte/autoassign */
            show_kb=!show_kb;
            break;
    }
}	
#endif


int main (int argc, char *argv[]) {
	char c='\0'; /* used for getch */ 
	unsigned char idle_chars_p=0;
	char ch;

#ifdef PROC_NET_DEV 
	strcpy(PROC_FILE,PROC_NET_DEV);
#endif	
	get_cmdln_options(argc,argv);
	memset(&if_stats_total,0,(size_t)sizeof(t_iface_stats));
#ifdef CURSES
	if (output_method==CURSES_OUT) {
		/* init curses */
		initscr(); 
		cbreak(); 
		noecho();
		nonl();
		curs_set(0);
		timeout(delay); /* set the timeout of getch to delay in ms) */
	}
#endif	
	/* end of init curses, now set a sigint handler to deinit the screen on ctrl-break */
	signal(SIGINT,sigint);
	signal(SIGTERM,sigint);
#ifdef CSV	
	if (output_method==CSV_OUT && output_count>-1) {
		get_iface_stats(0);
		usleep(delay*1000);
	}
#endif	
	if (daemonize) {
	  	int nbyt = 0;
	  	/* lets fork into background */
		if ((nbyt = fork()) == -1) {
			deinit("could not fork into background: %s\n",strerror(errno));
		}
		if (nbyt != 0) { /* nbyt is the new child pid here */
			deinit("forking into background\n");
		}
		setsid();
	}
	if (output_count==0) output_count=-1;
	if (output_method==PLAIN_OUT && output_count==1) output_method=PLAIN_OUT_ONCE;
	if (output_method==PLAIN_OUT) printf("\033[2J"); /* clear screen for plain out */
    while (1) { /* do the main loop */
		if (
#ifdef HTML				
				output_method!=HTML_OUT && 
#endif				
				output_method!=PLAIN_OUT_ONCE) 
			idle_chars_p=print_header(idle_chars_p); /* output only if looping */
		/* use a var to surpress pedantic warning with gcc */
		ch=!( output_method==PLAIN_OUT_ONCE
#ifdef HTML
                    || output_method==HTML_OUT
#endif
			);				
		get_iface_stats(ch); /* do the actual work, get and print stats */
		if ((
#ifdef CSV					
					output_method==CSV_OUT || 
#endif					
					output_method==PLAIN_OUT) && output_count>0) { 
			output_count--;
			if (output_count==0) break;
		}
#ifdef CURSES		
		if (output_method==CURSES_OUT) {
			refresh();
			handle_gui_input(getch());
		} else {
#endif			
			usleep(delay*1000);
#ifdef CURSES			
		}
#endif		
		c='\0'; /* not really needed, but i like it this way */
		if (output_method==PLAIN_OUT_ONCE 
#ifdef HTML				
				|| output_method==HTML_OUT
#endif
				) break; /* dont loop when we have plain output */
    }
#ifdef HTML	
	/* do we need to output for html? */
	if (output_method==HTML_OUT) {
		print_header(0);
		get_iface_stats(1);
		if (html_header) printf("</table>\n</body>\n</html>\n");
	}
#endif	
	/* do we need to output for plain? */
	if (output_method==PLAIN_OUT_ONCE) {
		print_header(0);
		get_iface_stats(1);
	}
	deinit(NULL);
	return 0; /* only to avoid gcc warning */
}
