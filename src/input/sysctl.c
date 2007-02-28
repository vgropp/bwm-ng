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

#include "sysctl.h"

#ifdef SYSCTL
/* do the actual work, get and print stats if verbose */
void get_iface_stats_sysctl (char verbose) {
    size_t size;
    int mib[] = {CTL_NET, PF_ROUTE, 0, 0, NET_RT_IFLIST, 0};
    char *bsd_if_buf=NULL, *next=NULL, *lim=NULL;
    char iface_is_up=0;
    struct if_msghdr *ifmhdr, *nextifmhdr;
    struct sockaddr_dl *saddr;

    char *name=NULL;

    int hidden_if=0,current_if_num=0,my_errno=0;
    t_iface_speed_stats tmp_if_stats;
    t_iface_speed_stats stats; /* local struct, used to calc total values */

    memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */

    if (sysctl(mib, 6, NULL, &size, NULL, 0) < 0) 
		 deinit(1, "sysctl failed: %s\n",strerror(errno));
    if (!(bsd_if_buf = malloc(size))) deinit(1, "no memory: %s\n",strerror(errno));
    memset(bsd_if_buf,0,size);
    if (sysctl(mib, 6, bsd_if_buf, &size, NULL, 0) < 0) {
        my_errno=errno;
        free(bsd_if_buf);
        deinit(1, "sysctl failed: %s\n",strerror(my_errno));
    }

    lim = (bsd_if_buf + size);

    next = bsd_if_buf;
    /* loop either while netstat enabled and still lines to read
     * or still buffer (buf) left */
    while (next < (bsd_if_buf + size)) {
        /* BSD sysctl code */
        ifmhdr = (struct if_msghdr *) next;
        if (ifmhdr->ifm_type != RTM_IFINFO) break;
        next += ifmhdr->ifm_msglen;
        while (next < lim) {
            nextifmhdr = (struct if_msghdr *) next;
            if (nextifmhdr->ifm_type != RTM_NEWADDR) break;
            next += nextifmhdr->ifm_msglen;
        }
        saddr = (struct sockaddr_dl *) (ifmhdr + 1);
        if(saddr->sdl_family != AF_LINK) continue;
		iface_is_up= (show_all_if || (ifmhdr->ifm_flags & IFF_UP));
        /* we have to copy here to use saddr->sdl_nlen */
        name=(char *)malloc(saddr->sdl_nlen+1);
	     if (!name) {
        	  deinit(1,"mem alloc failed: %s\n",strerror(errno));
	     }

		strncpy(name,saddr->sdl_data,saddr->sdl_nlen);
        name[saddr->sdl_nlen]='\0';
        tmp_if_stats.bytes.in=ifmhdr->ifm_data.ifi_ibytes;
        tmp_if_stats.bytes.out=ifmhdr->ifm_data.ifi_obytes;
        tmp_if_stats.packets.in=ifmhdr->ifm_data.ifi_ipackets;
        tmp_if_stats.packets.out=ifmhdr->ifm_data.ifi_opackets; 
		tmp_if_stats.errors.in=ifmhdr->ifm_data.ifi_ierrors;
        tmp_if_stats.errors.out=ifmhdr->ifm_data.ifi_oerrors;
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, iface_is_up);
        free(name);
        current_if_num++;
    } /* fgets done (while) */
    /* add to total stats and output current stats if verbose */
    finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* close input stream */
    free(bsd_if_buf);
    return;
}
#endif

#if SYSCTLDISK_IN
#ifdef HAVE_STRUCT_SYSCTL
#define MIBCOUNT 3
#else 
#define MIBCOUNT 2
#endif
void get_iface_stats_sysctldisk (char verbose) {
	size_t size;
	int mib[MIBCOUNT]; 
#if defined(HW_DISKCOUNT) && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
	int diskcount = 0;
	char *name_str = NULL;
	char **name_arr = NULL;
	char *ptr = NULL;
#endif
#if defined(HAVE_STRUCT_DISKSTATS)	
#define DISK_STRUCT struct diskstats	
#elif defined(HAVE_STRUCT_DISK_SYSCTL)
#define DISK_STRUCT struct disk_sysctl
#endif		
	DISK_STRUCT *dstats = NULL;
	int num,i;
	char *name=NULL;
	char free_name=0;

	int hidden_if=0,current_if_num=0;
	t_iface_speed_stats tmp_if_stats;
	t_iface_speed_stats stats; /* local struct, used to calc total values */

	memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */

	mib[0]=CTL_HW;

/* get name list on systems without dk_name */
#if defined(HW_DISKCOUNT) && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
	mib[1]=HW_DISKCOUNT;
	if (sysctl(mib, 2, &diskcount, &size, NULL, 0) < 0) 
		deinit(1, "sysctl failed: %s\n",strerror(errno));

	mib[1]=HW_DISKNAMES;
	if (sysctl(mib, 2, NULL, &size, NULL, 0) < 0)
		deinit(1, "sysctl failed: %s\n",strerror(errno));
	if (!(name_str=(char *)malloc(size)))
		deinit(1, "malloc failed for name_str: %s\n",strerror(errno));
	if (sysctl(mib, 2, name_str, &size, NULL, 0) < 0) {
		free(name_str);
		deinit(1, "malloc failed for name_str: %s\n",strerror(errno));
	}
	/* assume comma seperated list as on OpenBSD */
	if (!(name_arr = (char **)malloc(diskcount * sizeof(char *)))) {
		free(name_str);
		deinit(1, "malloc failed for name_arr: %s\n",strerror(errno));
	}
	ptr = name_str;
	i = 0;
	while (i<diskcount-1 && (next = strchr(ptr,','))) {
		next[0]=0;
		next++;
		name_arr[i] = ptr;
		ptr = next;
		i++;
	}
	/* add last element aswell */
	name_arr[i] = ptr;
#endif

/* get actual stats */	
	mib[1] = HW_DISKSTATS;
#ifdef HAVE_STRUCT_SYSCTL
	mib[2] = sizeof(struct sysctl);
#endif

	if (sysctl(mib, MIBCOUNT, NULL, &size, NULL, 0) < 0) {
#if HW_DISKCOUNT && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
		free(name_str);
		free(name_arr);
#endif		
		deinit(1, "sysctl failed: %s\n",strerror(errno));
	}
	
	num = size / sizeof(DISK_STRUCT);
	if (!(dstats = (DISK_STRUCT *)malloc(size))) {
#if HW_DISKCOUNT && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
      free(name_str);
      free(name_arr);
#endif
      deinit(1, "malloc failed: %s\n",strerror(errno));
	}

	if (sysctl(mib, MIBCOUNT, dstats, &size, NULL, 0) < 0) {
		free(dstats);
#if HW_DISKCOUNT && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
      free(name_str);
      free(name_arr);
#endif
      deinit(1, "sysctl failed: %s\n",strerror(errno));
	}
	for (i = 0; i < num; i++) {

#ifdef HAVE_STRUCT_DISKSTATS
#ifdef HAVE_STRUCT_DISKSTATS_DS_RBYTES
		tmp_if_stats.bytes.in = dstats[i].ds_rbytes;
		tmp_if_stats.bytes.out = dstats[i].ds_wbytes;
		tmp_if_stats.packets.in = dstats[i].ds_rxfer;
		tmp_if_stats.packets.out = dstats[i].ds_wxfer;
#else
		tmp_if_stats.bytes.in = tmp_if_stats.bytes.out = (ullong)(dstats[i].ds_bytes / 2);
		tmp_if_stats.packets.in = tmp_if_stats.packets.out = (ullong)(dstats[i].ds_xfer / 2);
#endif
#if !defined(HAVE_STRUCT_DISKSTATS_DS_NAME)		
		name = name_arr[i];
#else
		name = dstats[i].ds_name;
#endif		
#else
#ifdef HAVE_STRUCT_DISK_SYSCTL_DK_RBYTES
		tmp_if_stats.bytes.in = dstats[i].dk_rbytes;
		tmp_if_stats.bytes.out = dstats[i].dk_wbytes;
      tmp_if_stats.packets.in = dstats[i].dk_rxfer;
      tmp_if_stats.packets.out = dstats[i].dk_wxfer;
#else
		tmp_if_stats.bytes.in = tmp_if_stats.bytes.out = (ullong)(dstats[i].dk_bytes / 2);
		tmp_if_stats.packets.in = tmp_if_stats.packets.out = (ullong)(dstats[i].dk_xfer / 2);
#endif
		name = dstats[i].dk_name;
#endif
		tmp_if_stats.errors.in = tmp_if_stats.errors.out = 0;

		if (!name || name[0]==0) {
			name=malloc(11);
      	snprintf((char *)name,10,"unknown%i",current_if_num);
         name[10]=0;
			free_name=1;
		}

		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, (tmp_if_stats.bytes.in != 0 || tmp_if_stats.bytes.out != 0));

		if (free_name) free(name);
		current_if_num++;
	}
	/* add to total stats and output current stats if verbose */
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);
	free(dstats);
#if HW_DISKCOUNT && !defined(HAVE_STRUCT_DISKSTATS_DS_NAME) && defined(HAVE_STRUCT_DISKSTATS)
	free(name_str);
	free(name_arr);
#endif
}
#endif


