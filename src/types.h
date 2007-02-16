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

#ifndef __TYPES_H
#define __TYPES_H
#include "../config.h"

#ifdef HAVE_UNSIGNED_LONG_LONG
#define ullong unsigned long long
#else
#define ullong unsigned long
#endif

struct inout_long {
    ullong in;
    ullong out;
};

#if EXTENDED_STATS
struct inouttotal_double {
    long double in;
    long double out;
    long double total;
};

struct double_types {
    struct inouttotal_double bytes;
    struct inouttotal_double packets;
    struct inouttotal_double errors;
};
#endif

struct iface_speed_stats {
    struct inout_long bytes;
    struct inout_long packets;
    struct inout_long errors;
};

typedef struct iface_speed_stats t_iface_speed_stats;

#if EXTENDED_STATS
struct double_list {
    struct double_types data;
    struct double_list *next;
};

struct t_avg {
    struct double_list *first;
    struct double_list *last;
    /* number of items in list */
    int items;
    /* cached current avg values */
    struct double_types item_sum;
};
#endif

struct iface_stats {
    char    *if_name;
    t_iface_speed_stats data;
#if EXTENDED_STATS    
    struct double_types max;
    struct iface_speed_stats sum;
    struct t_avg avg;
#endif    
#ifdef HAVE_GETTIMEOFDAY
    struct timeval time;
#endif
};

typedef struct iface_stats t_iface_stats;
typedef struct double_types t_double_types;

#endif
