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

#include "defines.h"
#include "types.h"

#ifdef HAVE_CURSES
extern void print_online_help();
extern inline void get_iface_stats(char _n);
#ifdef __STDC__
extern void deinit(char *error_msg, ...) FUNCATTR_NORETURN;
#else
extern void deinit(...) FUNCATTR_NORETURN;
#endif

extern WINDOW *mywin;
extern SCREEN *myscr;
extern char dynamic;
extern char show_all_if;
extern char sumhidden;
extern int output_method;
extern char output_unit;
extern char output_type;
extern int input_method;
extern unsigned int delay;
extern char *iface_list;
extern int if_count;

/* global buffer to store all data of interfaces in */
extern t_iface_stats *if_stats;
/* total struct */
extern t_iface_stats if_stats_total;

#endif
