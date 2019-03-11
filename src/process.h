/******************************************************************************
 *  bwm-ng process data                                                       *
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

#ifndef __PROCESS_H
#define __PROCESS_H 

#include "defines.h"
#include "types.h"
#include "output.h"
#include "options.h"

void finish_iface_stats (char verbose, t_iface_speed_stats stats, int hidden_if, int iface_number);
void clean_down_ifaces(void);
int process_if_data (int hidden_if, t_iface_speed_stats tmp_if_stats,t_iface_speed_stats *stats, char *name, int iface_number, char verbose, char iface_is_up);

#endif
