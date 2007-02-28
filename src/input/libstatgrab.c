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

#include "libstatgrab.h"

#ifdef LIBSTATGRAB
/* do the actual work, get and print stats if verbose */
void get_iface_stats_libstat (char verbose) {
    sg_network_io_stats *network_stats=NULL;
    int num_network_stats,current_if_num=0,hidden_if=0;
	
	t_iface_speed_stats stats; /* local struct, used to calc total values */
    t_iface_speed_stats tmp_if_stats;
	memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
    
	network_stats = sg_get_network_io_stats(&num_network_stats);
    if (network_stats == NULL){
        deinit(1, "libstatgrab error!\n");
    }
	
	for (current_if_num=0;current_if_num<num_network_stats;current_if_num++) {
	    tmp_if_stats.bytes.in=network_stats->rx;
		tmp_if_stats.bytes.out=network_stats->tx;
	    tmp_if_stats.packets.in=network_stats->ipackets;
		tmp_if_stats.packets.out=network_stats->opackets;
	    tmp_if_stats.errors.in=network_stats->ierrors;
		tmp_if_stats.errors.out=network_stats->oerrors;
		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, network_stats->interface_name, current_if_num, verbose
#ifdef IOCTL
				,check_if_up(network_stats->interface_name)
#else
				,1
#endif
				);
        network_stats++;
	}
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);

	return;
}

void get_iface_stats_libstatdisk (char verbose) {
   sg_disk_io_stats *disk_stats=NULL;
   int num_disk_stats,current_if_num=0,hidden_if=0;

   t_iface_speed_stats stats; /* local struct, used to calc total values */
   t_iface_speed_stats tmp_if_stats;
   memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */

   disk_stats = sg_get_disk_io_stats(&num_disk_stats);
    if (disk_stats == NULL){
        deinit(1, "libstatgrab error!\n");
    }

   for (current_if_num=0;current_if_num<num_disk_stats;current_if_num++) {
		tmp_if_stats.bytes.in=disk_stats->read_bytes;
      tmp_if_stats.bytes.out=disk_stats->write_bytes;
      tmp_if_stats.packets.in=0;
      tmp_if_stats.packets.out=0;
      tmp_if_stats.errors.in=0;
      tmp_if_stats.errors.out=0;
      hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, disk_stats->disk_name, current_if_num, verbose,1);
      disk_stats++;
   }
   finish_iface_stats (verbose, stats, hidden_if,current_if_num);

   return;
}
#endif


