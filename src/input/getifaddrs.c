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

#include "getifaddrs.h"

#ifdef GETIFADDRS
/* do the actual work, get and print stats if verbose */
void get_iface_stats_getifaddrs (char verbose) {
    char *name=NULL;

    struct ifaddrs *net, *net_ptr=NULL;
    struct if_data *net_data;
	
	int hidden_if=0,current_if_num=0;
    t_iface_speed_stats stats; /* local struct, used to calc total values */
    t_iface_speed_stats tmp_if_stats;

    memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */

    /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
	if (getifaddrs(&net) != 0) {
		deinit(1, "getifaddr failed: %s\n",strerror(errno));
	}
	net_ptr=net;
    /* loop either while netstat enabled and still lines to read
     * or still buffer (buf) left */
    while (net_ptr!=NULL) {
        /* move getifaddr data to my struct */
		if (net_ptr->ifa_addr==NULL || net_ptr->ifa_addr->sa_family != AF_LINK) {
			net_ptr=net_ptr->ifa_next;
			continue;
		}
		if (net_ptr->ifa_name!=NULL)
			name=strdup(net_ptr->ifa_name);
		else 
			name=strdup("");
        if (net_ptr->ifa_data!=NULL) {
		      net_data=(struct if_data *)net_ptr->ifa_data;
            tmp_if_stats.bytes.in=net_data->ifi_ibytes;
            tmp_if_stats.bytes.out=net_data->ifi_obytes;
            tmp_if_stats.packets.in=net_data->ifi_ipackets;
            tmp_if_stats.packets.out=net_data->ifi_opackets;
            tmp_if_stats.errors.in=net_data->ifi_ierrors;
            tmp_if_stats.errors.out=net_data->ifi_oerrors;
        } else {
            net_ptr=net_ptr->ifa_next;
            continue;
        }
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, (show_all_if || (net_ptr->ifa_flags & IFF_UP)));
        net_ptr=net_ptr->ifa_next;
		free(name);
		current_if_num++;
    } /* fgets done (while) */
    /* add to total stats and output current stats if verbose */
    finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* close input stream */
	freeifaddrs(net);
    return;
}
#endif


