/******************************************************************************
 *  bwm-ng output                                                             *
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA *
 *                                                                            *
 *****************************************************************************/

#ifndef __OUTPUT_H
#define __OUTPUT_H 

#include "defines.h"
#include "types.h"

/* for csv output and time() */
#include <time.h>

#include "options.h"

int print_header(int option);
void print_values(unsigned int y,unsigned int x,const char *if_name,t_iface_speed_stats stats,float multiplier,t_iface_stats full_stats);

#endif
