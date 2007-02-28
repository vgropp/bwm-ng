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

#include "proc_net_dev.h"

#ifdef PROC_NET_DEV
/* do the actual work, get and print stats if verbose */
void get_iface_stats_proc (char verbose) {
	char *ptr;

	FILE *f=NULL;
	char *buffer=NULL,*name=NULL;
    
	int hidden_if=0,current_if_num=0;
	t_iface_speed_stats stats; /* local struct, used to calc total values */
	t_iface_speed_stats tmp_if_stats;

	memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
	/* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
	if (!(f=fopen(PROC_FILE,"r"))) {
		deinit(1, "open of procfile failed: %s\n",strerror(errno));
	}
	buffer=(char *)malloc(MAX_LINE_BUFFER);

	/* we skip first 2 lines if not bsd at any mode */
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || 
			(fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) 
		deinit(1, "read of proc failed: %s\n",strerror(errno));

	name=(char *)malloc(MAX_LINE_BUFFER);
	while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL) ) {
        /* get the name */
        ptr=strchr(buffer,':');
        /* wrong format */
        if (ptr==NULL) { deinit(1, "wrong format of input stream\n"); }
		/* set : to end_of_string and move to first char of "next" string (to first data) */
        *ptr++ = 0;
        sscanf(ptr,"%llu%llu%llu%*i%*i%*i%*i%*i%llu%llu%llu",&tmp_if_stats.bytes.in,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.bytes.out,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
        sscanf(buffer,"%s",name);
		/* init new interfaces and add fetched data to old or new one */
		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose
#ifdef IOCTL
                ,check_if_up(name)
#else
                ,1
#endif
				);
		current_if_num++;
    } /* fgets done (while) */
	/* add to total stats and output current stats if verbose */
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* clean buffers */
	free(buffer);
	free(name);
	/* close input stream */
	fclose(f);
    return;
}
#endif

