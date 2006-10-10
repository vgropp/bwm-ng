/******************************************************************************
 *  bwm-ng parsing and retrieve stuff                                         *
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

#include "retrieve.h"

#if NETSTAT_BSD || NETSTAT_SOLARIS || NETSTAT_BSD_BYTES || NETSTAT_NETBSD
/* counts the tokens in a string */
long count_tokens(char *in_str) {
    long tokens=0;
    long i=0;
    char in_a_token=0;
    char *str;

    if (in_str==NULL) return 0;
    str=in_str;
    while (str[i]!='\0') {
        if (str[i]>32) {
            if (!in_a_token) {
                tokens++;
                in_a_token=1;
            }
        } else {
            if (in_a_token) in_a_token=0;
        }
        i++;
    }
    return tokens;
}
#endif

#ifdef IOCTL
/* test whether the iface is up or not */
char check_if_up(char *ifname) {
    struct ifreq ifr;
    /* check if we already opened the file descriptor
     * if not open it now */
    if (skfd < 0) {
        if ((skfd =  socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
            deinit(1, "socket error: %s\n",strerror(errno));
        }
    }
    /* setup the struct */
    strncpy(ifr.ifr_name, ifname,sizeof(ifr.ifr_name));
	ifr.ifr_name[sizeof(ifr.ifr_name)-1]='\0';
    /* lookup the status */
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
        return 0; /* return if as down if there was an error */
    }
    /* check against IFF_UP and return */
    return (ifr.ifr_flags & IFF_UP);
}
#endif


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
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit(1, "read of proc failed: %s\n",strerror(errno));
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
#endif


#ifdef NETSTAT
/* do the actual work, get and print stats if verbose */
void get_iface_stats_netstat (char verbose) {
    int current_if_num=0,hidden_if=0;
	char *buffer=NULL,*name=NULL;
#if NETSTAT_NETBSD
    char *str_buf=NULL;
    char *test_buf;
    char *buffer2=NULL;
    FILE *f2=NULL;
#endif
#if NETSTAT_BSD	|| NETSTAT_BSD_BYTES || NETSTAT_SOLARIS || NETSTAT_NETBSD
    char *last_name=NULL;
#endif	
	FILE *f=NULL;

	t_iface_speed_stats stats; /* local struct, used to calc total values */
    t_iface_speed_stats tmp_if_stats;
    memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
	if (!(f=popen(
#if NETSTAT_BSD || NETSTAT_BSD_BYTES
#if NETSTAT_BSD_LINK
	        NETSTAT_PATH " -iW -f link"
#else
		    NETSTAT_PATH " -iW"
#endif				  
#if NETSTAT_BSD_BYTES
			" -b"
#endif
#endif
#if NETSTAT_LINUX
                  show_all_if ? NETSTAT_PATH " -ia" : NETSTAT_PATH " -i"
#endif
#if NETSTAT_SOLARIS
            NETSTAT_PATH " -i -f inet -f inet6"
#endif
#if NETSTAT_NETBSD
            NETSTAT_PATH " -ib"
#endif
                    ,"r")))
        deinit(1, "no input stream found: %s\n",strerror(errno));
#if NETSTAT_NETBSD
    if (!(f2=popen( NETSTAT_PATH " -i","r")))
        deinit(1, "no input stream found: %s\n",strerror(errno));
    buffer2=(char *)malloc(MAX_LINE_BUFFER);
    if ((fgets(buffer2,MAX_LINE_BUFFER,f2) == NULL )) deinit(1, "read of netstat failed: %s\n",strerror(errno));
    str_buf=(char *)malloc(MAX_LINE_BUFFER);
#endif
    buffer=(char *)malloc(MAX_LINE_BUFFER);
#ifdef NETSTAT_LINUX
    /* we skip first 2 lines if not bsd at any mode */
    if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) 
		deinit(1, "read of netstat failed: %s\n",strerror(errno));
#endif
#if NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS || NETSTAT_NETBSD
    last_name=(char *)malloc(MAX_LINE_BUFFER);
    last_name[0]='\0'; /* init */
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit(1, "read of netstat failed: %s\n",strerror(errno));
#endif
    name=(char *)malloc(MAX_LINE_BUFFER);
    /* loop and read each line */
    while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL && buffer[0]!='\n') ) {
        memset(&tmp_if_stats,0,(size_t)sizeof(t_iface_speed_stats)); /* reinit it to zero */
#ifdef NETSTAT_LINUX		
        sscanf(buffer,"%s%*i%*i%llu%llu%*i%*i%llu%llu",name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
#endif
#if NETSTAT_BSD_BYTES 
        if (count_tokens(buffer)>=10) /* including address */
    		sscanf(buffer,"%s%*i%*s%*s%llu%llu%llu%llu%llu%llu",name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.bytes.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out,&tmp_if_stats.bytes.out);
        else /* w/o address */
            sscanf(buffer,"%s%*i%*s%llu%llu%llu%llu%llu%llu",name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.bytes.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out,&tmp_if_stats.bytes.out);
#endif
#if NETSTAT_BSD	|| NETSTAT_SOLARIS	
        if (count_tokens(buffer)>=8) /* including address */
		    sscanf(buffer,"%s%*i%*s%*s%llu%llu%llu%llu",name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
        else /* w/o address */
            sscanf(buffer,"%s%*i%*s%llu%llu%llu%llu",name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
#endif
#if NETSTAT_NETBSD
        test_buf=fgets(buffer2,MAX_LINE_BUFFER,f2); 
        if (count_tokens(buffer)>=6) { /* including address */
            if (test_buf) sscanf(buffer2,"%s%s%s%s%llu%llu%llu%llu",str_buf,str_buf,str_buf,str_buf,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
            sscanf(buffer,"%s%s%s%s%llu%llu",name,str_buf,str_buf,str_buf,&tmp_if_stats.bytes.in,&tmp_if_stats.bytes.out);
        } else {
            if (test_buf) sscanf(buffer2,"%s%s%s%llu%llu%llu%llu",str_buf,str_buf,str_buf,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out);
            sscanf(buffer,"%s%s%s%llu%llu",name,str_buf,str_buf,&tmp_if_stats.bytes.in,&tmp_if_stats.bytes.out);
        }
#endif
#if NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS || NETSTAT_NETBSD
        /* check if we have a new iface or if its only a second line of the same one */
        if (!strcmp(last_name,name)) continue; /* skip this line */
#if HAVE_STRLCPY		  
		  strlcpy(last_name,name,MAX_LINE_BUFFER - 1);
#else		  
        strcpy(last_name,name);
#endif		  
#endif
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose,
#if NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_NETBSD
		(name[strlen(name)-1]!='*')
#else
		1
#endif
        );
	
        current_if_num++;
    } /* fgets done (while) */
    /* add to total stats and output current stats if verbose */
    finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* clean buffers */
    free(buffer);
#if NETSTAT_NETBSD
    free(buffer2);
    free(str_buf);
    pclose(f2);
#endif
#if NETSTAT_BSD || NETSTAT_NETBSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS
    free(last_name);
#endif	
    free(name);
    /* close input stream */
    pclose(f);
    return;
}
#endif

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

    /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
    if (sysctl(mib, 6, NULL, &size, NULL, 0) < 0) deinit(1, "sysctl failed: %s\n",strerror(errno));
    if (!(bsd_if_buf = malloc(size))) deinit(1, "no memory: %s\n",strerror(errno));
    bzero(bsd_if_buf,size);
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


#if HAVE_LIBKSTAT
void get_iface_stats_kstat (char verbose) {
    kstat_ctl_t   *kc;
    kstat_t       *ksp;
    kstat_named_t *i_bytes,*o_bytes,*i_packets,*o_packets,*i_errors,*o_errors;
    char *name;
    int hidden_if=0,current_if_num=0,my_errno=0;
    t_iface_speed_stats tmp_if_stats;
    t_iface_speed_stats stats; /* local struct, used to calc total values */
    
    memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
    kc = kstat_open();
    if (kc==NULL) deinit(1, "kstat failed: %s\n",strerror(my_errno));
    name=(char *)malloc(KSTAT_STRLEN);
    /* loop for interfaces */
    for (ksp = kc->kc_chain;ksp != NULL;ksp = ksp->ks_next) {
        if (strcmp(ksp->ks_class, "net") != 0)
            continue; /* skip all other stats */
        strncpy(name,ksp->ks_name,KSTAT_STRLEN);
        name[KSTAT_STRLEN-1]='\0';
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
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, 1);
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

#ifdef WIN32
void get_iface_stats_win32 (char verbose) {
	PMIB_IFTABLE if_table,tmp;
	unsigned long tableSize;
	int i,current_if_num,hidden_if=0,err;
	char name[MAX_INTERFACE_NAME_LEN];
	t_iface_speed_stats tmp_if_stats;
	t_iface_speed_stats stats; /* local struct, used to calc total values */

	memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
	tableSize=sizeof(MIB_IFTABLE);
	if_table = malloc(sizeof(MIB_IFTABLE));
	if (if_table==NULL) return;

	/* get table size or data if table is big enough */
	if ((err=GetIfTable(if_table, &tableSize, 0)==ERROR_INSUFFICIENT_BUFFER)) {
		tmp = realloc(if_table, tableSize);
		if (tmp==NULL) {
			free(if_table);
			return;
		}
		if_table=tmp;
		/* get data */
		err=GetIfTable(if_table, &tableSize, 0);
	}
	if (err != NO_ERROR) {
		free(if_table);
		return;
	}

	current_if_num=0;

	for (i=0; i<if_table->dwNumEntries; i++) {
		strncpy(name,if_table->table[i].bDescr,MAX_INTERFACE_NAME_LEN);	
		name[MAX_INTERFACE_NAME_LEN-1]='\0';
		tmp_if_stats.bytes.in=if_table->table[i].dwInOctets;
		tmp_if_stats.bytes.out=if_table->table[i].dwOutOctets;
		tmp_if_stats.packets.in=if_table->table[i].dwInUcastPkts + if_table->table[i].dwInNUcastPkts;
		tmp_if_stats.packets.out=if_table->table[i].dwOutUcastPkts + if_table->table[i].dwOutNUcastPkts;
		tmp_if_stats.errors.in=if_table->table[i].dwInErrors;
		tmp_if_stats.errors.out=if_table->table[i].dwOutErrors;
		/* init new interfaces and add fetched data to old or new one */
		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, 1);
		current_if_num++;
	}	
	/* add to total stats and output current stats if verbose */
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);

	free(if_table);
	return;
}
#endif

