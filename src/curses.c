/******************************************************************************
 *  bwm-ng curses stuff                                                       *
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

#include "curses.h"


#ifdef HAVE_CURSES
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
        case 'a':
        case 'A':
            show_all_if++;
            if (show_all_if>2) show_all_if=0;
            if (iface_list==NULL && show_all_if==1) show_all_if=2;
            /* get stats so all values are uptodate */
            get_iface_stats(0);
            /* a short sleep, else we get "nan" values due to very short 
               delay till next get_iface_stats */
            usleep(100);
            break;
        case 's':
        case 'S':
            sumhidden=!sumhidden;
            /* get stats so all values are uptodate */
            get_iface_stats(0);
            /* a short sleep, else we get "nan" values due to very short
               delay till next get_iface_stats */
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
            dynamic=!dynamic;
            break;
        case 'u':
        case 'U':
            if (output_unit<4) 
                output_unit++;
            else 
                output_unit=1;
	        break;
#if EXTENDED_STATS            
        case 't':
        case 'T':
            if (output_type<TYPE_OUT_MAX)
                output_type++;
            else 
                output_type=1;
            break;
#endif            
        case 'h':
            print_online_help();
            break;
    }
}	

void init_curses() {
        myscr=initscr();
        if (myscr == NULL) {
            deinit("failed to init curses: %s\n",strerror(errno));
        }
        cbreak();
        noecho();
        nonl();
#if HAVE_CURS_SET        
        curs_set(0);
#endif        
        timeout(delay); /* set the timeout of getch to delay in ms) */
}

void sigwinch(int sig) {
    struct winsize size;
    if (ioctl(fileno(stdout), TIOCGWINSZ, &size) == 0) {
#if HAVE_RESIZETERM        
        resizeterm(size.ws_row, size.ws_col);
#endif        
        if (endwin()==ERR) deinit("failed to deinit curses: %s\n",strerror(errno));
        init_curses();
    }
}


#endif
