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

#include "libkstat.h"

#if HAVE_LIBKSTAT
void get_iface_stats_kstat (char verbose) {
    kstat_ctl_t   *kc;
    kstat_t       *ksp;
    kstat_io_t     kio;	
    kstat_named_t *i_bytes=NULL,*o_bytes=NULL,*i_packets=NULL,*o_packets=NULL,*i_errors=NULL,*o_errors=NULL;
    char *name;
    int hidden_if=0,current_if_num=0,my_errno=0;
    t_iface_speed_stats tmp_if_stats;
    t_iface_speed_stats stats; /* local struct, used to calc total values */
    
    memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
    kc = kstat_open();
    if (kc==NULL) deinit(1, "kstat failed: %s\n",strerror(my_errno));
    name=(char *)malloc(KSTAT_STRLEN);
    if (!name) {
		kstat_close(kc);
       deinit(1,"mem alloc failed: %s\n",strerror(errno));
    }
	 
    /* loop for interfaces */
    for (ksp = kc->kc_chain;ksp != NULL;ksp = ksp->ks_next) {
			if ((strcmp(ksp->ks_class, "net") != 0 && input_method==KSTAT_IN) || (strcmp(ksp->ks_class, "disk") != 0 && input_method==KSTATDISK_IN && ksp->ks_type != KSTAT_TYPE_IO))
            continue; /* skip all other stats */
			strncpy(name,ksp->ks_name,KSTAT_STRLEN);
			name[KSTAT_STRLEN-1]='\0';
			if (KSTAT_IN==input_method) {
				kstat_read(kc, ksp, NULL);
				i_bytes=(kstat_named_t *)kstat_data_lookup(ksp, "rbytes");
				o_bytes=(kstat_named_t *)kstat_data_lookup(ksp, "obytes");
				i_packets=(kstat_named_t *)kstat_data_lookup(ksp, "ipackets");
				o_packets=(kstat_named_t *)kstat_data_lookup(ksp, "opackets");
				i_errors=(kstat_named_t *)kstat_data_lookup(ksp, "ierrors");
				o_errors=(kstat_named_t *)kstat_data_lookup(ksp, "oerrors");
				if (!i_bytes || !o_bytes || !i_packets || !o_packets || !i_errors || !o_errors) 
					continue;
				/* use ui32 values, the 64 bit values return strange (very big) differences */
				tmp_if_stats.bytes.in=i_bytes->value.ui32;
				tmp_if_stats.bytes.out=o_bytes->value.ui32;
				tmp_if_stats.packets.in=i_packets->value.ui32;
				tmp_if_stats.packets.out=o_packets->value.ui32;
				tmp_if_stats.errors.in=i_errors->value.ui32;
				tmp_if_stats.errors.out=o_errors->value.ui32;
			} else if (KSTATDISK_IN==input_method) {
				kstat_read(kc, ksp, &kio);  	
				tmp_if_stats.bytes.in=kio.nread;
				tmp_if_stats.bytes.out=kio.nwritten;
				tmp_if_stats.packets.in=kio.reads;
				tmp_if_stats.packets.out=kio.writes;
				tmp_if_stats.errors.in=tmp_if_stats.errors.out=0;
			} else {
				free(name);
				kstat_close(kc);
				deinit(1,"im confused about kstat input methods!\n");
			}
			/* init new interfaces and add fetched data to old or new one */
			hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, (tmp_if_stats.bytes.in != 0 || tmp_if_stats.bytes.out != 0));
			current_if_num++;
    }
    /* add to total stats and output current stats if verbose */
    finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* clean buffers */
    free(name);
    kstat_close(kc);
    return;
}
#endif
