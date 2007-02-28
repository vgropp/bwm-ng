/******************************************************************************
 *  bwm-ng parsing and retrieve stuff                                         *
 *                                                                            *
 *  Copyright (C) 2004-2007 Volker Gropp (bwmng@gropp.org)                    *
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

#include "devstat.h"


#if DEVSTAT_IN
#ifndef HAVE_DEVSTAT_GETDEVS
#define devstat_getdevs(_n,_m) getdevs(_m)
#define devstat_selectdevs selectdevs
#endif
void get_iface_stats_devstat (char verbose) {
   int current_if_num=0,hidden_if=0;
	struct statinfo dev_stats;
	struct device_selection *dev_sel = NULL;
	int n_selected, n_selections;
   long sel_gen;
   struct devstat *dev_ptr;

   t_iface_speed_stats stats; /* local struct, used to calc total values */
   t_iface_speed_stats tmp_if_stats;
   memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */

	if (!(dev_stats.dinfo=malloc(sizeof(struct devinfo))))
		deinit(1,"malloc failure\n");
	bzero(dev_stats.dinfo, sizeof(struct devinfo));
	
	if ((devstat_getdevs(NULL, (struct statinfo *)&dev_stats)) < 0) {
		free(dev_stats.dinfo);
		deinit(1,"getdevs failed: %s\n",devstat_errbuf);
	}
	if (devstat_selectdevs(&dev_sel, &n_selected, &n_selections, &sel_gen, dev_stats.dinfo->generation, dev_stats.dinfo->devices, dev_stats.dinfo->numdevs, NULL, 0, NULL, 0, DS_SELECT_ONLY, dev_stats.dinfo->numdevs, 1) < 0) {
		free(dev_stats.dinfo);
		deinit(1,"selectdevs failed: %s\n",devstat_errbuf);
	}
	for(current_if_num=0;current_if_num<dev_stats.dinfo->numdevs;current_if_num++){
		dev_ptr=&dev_stats.dinfo->devices[dev_sel[current_if_num].position];
#ifdef HAVE_STRUCT_DEVSTAT_BYTES_READ
		tmp_if_stats.bytes.in=dev_ptr->bytes_read;
		tmp_if_stats.bytes.out=dev_ptr->bytes_written;
		tmp_if_stats.packets.in=dev_ptr->num_read;
		tmp_if_stats.packets.out=dev_ptr->num_written;
#else
		tmp_if_stats.bytes.in=dev_ptr->bytes[DEVSTAT_READ];
		tmp_if_stats.bytes.out=dev_ptr->bytes[DEVSTAT_WRITE];
		tmp_if_stats.packets.in=dev_ptr->operations[DEVSTAT_READ];
		tmp_if_stats.packets.out=dev_ptr->operations[DEVSTAT_WRITE];
#endif
		tmp_if_stats.errors.in = tmp_if_stats.errors.out = 0;
		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, dev_ptr->device_name, current_if_num, verbose,(tmp_if_stats.bytes.in != 0 || tmp_if_stats.bytes.out != 0));
		
	}
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);
	free(dev_stats.dinfo);
	return;
}
#endif


