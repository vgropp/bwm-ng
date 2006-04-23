/******************************************************************************
 *  bwm-ng process data                                                       *
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

#if __PROCESS_H || __HELP_H || __BWM_NG_H || __RETRIEVE_H || __CURSES_TOOLS_H || __OPTIONS_H || __OUTPUT_H
#else
#define __PROCESS_H 1

#include "defines.h"
#include "types.h"


extern void print_values(int y,int x,char *if_name,t_iface_speed_stats stats,float multiplier,t_iface_stats full_stats);

extern int if_count;
extern unsigned int delay;
extern char dynamic;
extern char show_all_if;
extern char sumhidden;
extern char output_unit;
extern char output_type;
extern int output_method;
extern int input_method;
extern char *iface_list;
#if EXTENDED_STATS
extern unsigned int avg_length;
#endif

/* global buffer to store all data of interfaces in */
t_iface_stats *if_stats=NULL;
/* total struct */
t_iface_stats if_stats_total;

#endif
