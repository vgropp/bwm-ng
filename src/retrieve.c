/******************************************************************************
 *  bwm-ng parsing and retrive stuff                                          *
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


#ifdef IOCTL
/* test whether the iface is up or not */
char check_if_up(char *ifname) {
    struct ifreq ifr;
    if (skfd < 0) {
        /* maybe check some /proc file first like net-tools do */
        if ((skfd =  socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
            deinit("socket error: %s\n",strerror(errno));
        }
    }
    strncpy(ifr.ifr_name, ifname,sizeof(ifr.ifr_name));
	ifr.ifr_name[sizeof(ifr.ifr_name)-1]='\0';
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0) {
        return 0; /* return if as down if there was some error */
    }
    if (ifr.ifr_flags & IFF_UP) return 1; /* check against IFF_UP and return */
        else return 0;
}
#endif


/* returns the whether to show the iface or not
 * if is in list return 1, if list is prefaced with ! or 
 * name not found return 0 */
short show_iface(char *instr, char *searchstr) {
	int pos = 0,k,success_ret=1;
    if (instr==NULL) return success_ret;
    if (instr[0]=='%') {
        success_ret=!success_ret;
        *instr=*instr+1; /* dont use *instr++ to avoid netbsd/openbsd warning here */
    }
	k = strlen( searchstr );
	do {
		switch ( *instr ) {
			case 0:
			case ',':
				if ( k == pos && ! strncasecmp( instr - pos, searchstr, pos ) ) {
					return success_ret;
                }
				pos = 0;
				break;
			default:
				pos++;
				break;
			}
		} while ( *instr++ );
	return !success_ret;
}


#if HAVE_GETTIMEOFDAY
/* Returns: the time difference in milliseconds. */
long tvdiff(struct timeval newer, struct timeval older) {
  return labs((newer.tv_sec-older.tv_sec)*1000+
    (newer.tv_usec-older.tv_usec)/1000);
}

/* returns the milliseconds since old stat */
float get_time_delay(int iface_num) {
    struct timeval now;
    float ret;
    gettimeofday(&now,NULL);
    ret=(float)1000/tvdiff(now,if_stats[iface_num].time);
    if_stats[iface_num].time.tv_sec=now.tv_sec;
    if_stats[iface_num].time.tv_usec=now.tv_usec;    
    return ret;
}
#endif


int process_if_data (int hidden_if, t_iface_stats tmp_if_stats,t_iface_stats *stats, char *name, int iface_number, char verbose, char iface_is_up) {
#if HAVE_GETTIMEOFDAY
    float multiplier;
#else
	float multiplier=(float)1000/delay;
#endif    
	int local_if_count;

    for (local_if_count=0;local_if_count<if_count;local_if_count++) {
        /*check if its the correct if */
        if (!strcmp(name,if_stats[local_if_count].if_name)) break;
    }
    if (local_if_count==if_count) {
        /* iface not found, seems like there is a new one! */
        if_count++;
        if_stats=(t_iface_stats*)realloc(if_stats,sizeof(t_iface_stats)*if_count);
        /* copy the iface name or add a dummy one */
        if (name[0]!='\0')
            if_stats[if_count-1].if_name=(char*)strdup(name);
        else
            if_stats[if_count-1].if_name=(char*)strdup("unknown");
        /* set it to current value, so there is no peak at first announce,
         * we cannot copy the struct cause we wanna safe the name */
        if (sumhidden || ((show_all_if>1 || iface_is_up) &&
            (show_all_if || show_iface(iface_list,name)))) {
            if_stats_total.send+=if_stats[local_if_count].send=tmp_if_stats.send;
            if_stats_total.rec+=if_stats[local_if_count].rec=tmp_if_stats.rec;
            if_stats_total.p_send+=if_stats[local_if_count].p_send=tmp_if_stats.p_send;
            if_stats_total.p_rec+=if_stats[local_if_count].p_rec=tmp_if_stats.p_rec;
            if_stats_total.e_send+=if_stats[local_if_count].e_send=tmp_if_stats.e_send;
            if_stats_total.e_rec+=if_stats[local_if_count].e_rec=tmp_if_stats.e_rec;
        } else {
            if_stats[local_if_count].send=tmp_if_stats.send;
            if_stats[local_if_count].rec=tmp_if_stats.rec;
            if_stats[local_if_count].p_send=tmp_if_stats.p_send;
            if_stats[local_if_count].p_rec=tmp_if_stats.p_rec;
            if_stats[local_if_count].e_send=tmp_if_stats.e_send;
            if_stats[local_if_count].e_rec=tmp_if_stats.e_rec;
        }
    }
#if HAVE_GETTIMEOFDAY
    multiplier=(float)get_time_delay(local_if_count);
#endif    
    if (verbose) { /* any output at all? */
        //cycle: show all interfaces, only those which are up, only up and not hidden
        if (
            (show_all_if>1 || iface_is_up) && /* is it up or do we show all ifaces? */
            (show_all_if || show_iface(iface_list,name))) {
            print_values(5+iface_number-hidden_if,8,name,tmp_if_stats,if_stats[local_if_count],multiplier);
		} else
            hidden_if++; /* increase the opt cause we dont show this if */
    }
    /* save current stats for the next run add current iface stats to total */
    if (sumhidden || ((show_all_if>1 || iface_is_up) &&
            (show_all_if || show_iface(iface_list,name)))) {
        stats->send+=if_stats[local_if_count].send=tmp_if_stats.send;
        stats->rec+=if_stats[local_if_count].rec=tmp_if_stats.rec;
        stats->p_send+=if_stats[local_if_count].p_send=tmp_if_stats.p_send;
        stats->p_rec+=if_stats[local_if_count].p_rec=tmp_if_stats.p_rec;
        stats->e_send+=if_stats[local_if_count].e_send=tmp_if_stats.e_send;
        stats->e_rec+=if_stats[local_if_count].e_rec=tmp_if_stats.e_rec;
    } else {
        if_stats[local_if_count].send=tmp_if_stats.send;
        if_stats[local_if_count].rec=tmp_if_stats.rec;
        if_stats[local_if_count].p_send=tmp_if_stats.p_send;
        if_stats[local_if_count].p_rec=tmp_if_stats.p_rec;
        if_stats[local_if_count].e_send=tmp_if_stats.e_send;
        if_stats[local_if_count].e_rec=tmp_if_stats.e_rec;
    }
	return hidden_if;
}	

void finish_iface_stats (char verbose, t_iface_stats stats, int hidden_if, int iface_number) {
    int i;
#if HAVE_GETTIMEOFDAY
    struct timeval now;
    float multiplier;
    gettimeofday(&now,NULL);
    multiplier=(float)1000/tvdiff(now,if_stats_total.time);
    if_stats_total.time.tv_sec=now.tv_sec;
    if_stats_total.time.tv_usec=now.tv_usec;
#else
	float multiplier=(float)1000/delay;
#endif    

    if (verbose) {
        /* output total ifaces stats */
#ifdef HAVE_CURSES		
        if (output_method==CURSES_OUT)
            mvwprintw(stdscr,5+iface_number-hidden_if,8,"------------------------------------------------------------------");
        else 
#endif			
			if (output_method==PLAIN_OUT || output_method==PLAIN_OUT_ONCE)
				printf("%s------------------------------------------------------------------\n",output_method==PLAIN_OUT ? " " : "");
        print_values(6+iface_number-hidden_if,8,"total",stats,if_stats_total,multiplier);
    }
    /* save the data in total-struct */
    if_stats_total.send=stats.send;
    if_stats_total.rec=stats.rec;
    if_stats_total.p_send=stats.p_send;
    if_stats_total.p_rec=stats.p_rec;
    if_stats_total.e_send=stats.e_send;
    if_stats_total.e_rec=stats.e_rec;
	if (output_method==PLAIN_OUT)
		for (i=0;i<if_count-iface_number;i++) printf("%70s\n"," "); /* clear unused lines */
	return;
}


#ifdef GETIFADDRS
/* do the actual work, get and print stats if verbose */
void get_iface_stats_getifaddrs (char verbose) {
    char iface_is_up=0;
	
    char *name=NULL;

    struct ifaddrs *net, *net_ptr=NULL;
    struct if_data *net_data;
	
	int hidden_if=0,current_if_num=0;
    t_iface_stats stats,tmp_if_stats; /* local struct, used to calc total values */

    memset(&stats,0,(size_t)sizeof(t_iface_stats)); /* init it */

    /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
	if (getifaddrs(&net) != 0) {
		deinit("sysctl (getifaddr) failed: %s\n",strerror(errno));
	}
	net_ptr=net;
    /* loop either while netstat enabled and still lines to read
     * or still buffer (buf) left */
    while (net_ptr!=NULL) {
        memset(&tmp_if_stats,0,(size_t)sizeof(t_iface_stats)); /* reinit it to zero */
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
            tmp_if_stats.rec=net_data->ifi_ibytes;
            tmp_if_stats.send=net_data->ifi_obytes;
            tmp_if_stats.p_rec=net_data->ifi_ipackets;
            tmp_if_stats.p_send=net_data->ifi_opackets;
            tmp_if_stats.e_rec=net_data->ifi_ierrors;
            tmp_if_stats.e_send=net_data->ifi_oerrors;
        } else {
            net_ptr=net_ptr->ifa_next;
            continue;
        }
		iface_is_up= (show_all_if || (net_ptr->ifa_flags & IFF_UP));
		net_ptr=net_ptr->ifa_next;
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, iface_is_up);
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
    unsigned long long buf;
    
	int hidden_if=0,current_if_num=0;
	t_iface_stats stats,tmp_if_stats; /* local struct, used to calc total values */

	memset(&stats,0,(size_t)sizeof(t_iface_stats)); /* init it */
    /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
    if (!(f=fopen(PROC_FILE,"r"))) {
		deinit("open of procfile failed: %s\n",strerror(errno));
	}
	buffer=(char *)malloc(MAX_LINE_BUFFER);
	/* we skip first 2 lines if not bsd at any mode */
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit("read of proc failed: %s\n",strerror(errno));
	name=(char *)malloc(MAX_LINE_BUFFER);
	while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL) ) {
		memset(&tmp_if_stats,0,(size_t)sizeof(t_iface_stats)); /* reinit it to zero */
        /* get the name */
        ptr=strchr(buffer,':');
        /* wrong format */
        if (ptr==NULL) { deinit("wrong format of input stream\n"); }
		/* set : to end_of_string and move to first char of "next" string (to first data) */
        *ptr++ = 0;
        sscanf(ptr,"%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu%llu",&tmp_if_stats.rec,&tmp_if_stats.p_rec,&tmp_if_stats.e_rec,&buf,&buf,&buf,&buf,&buf,&tmp_if_stats.send,&tmp_if_stats.p_send,&tmp_if_stats.e_send);
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
	char *name;
	
	t_iface_stats stats,tmp_if_stats; /* local struct, used to calc total values */
	memset(&stats,0,(size_t)sizeof(t_iface_stats)); /* init it */
    
	network_stats = sg_get_network_io_stats(&num_network_stats);
    if (network_stats == NULL){
        deinit("libstatgrab error!\n");
    }
	
	name=(char *)malloc(MAX_LINE_BUFFER);
	while (num_network_stats>current_if_num) {
	    tmp_if_stats.rec=network_stats->rx;
		tmp_if_stats.send=network_stats->tx;
	    tmp_if_stats.p_rec=network_stats->ipackets;
		tmp_if_stats.p_send=network_stats->opackets;
	    tmp_if_stats.e_rec=network_stats->ierrors;
		tmp_if_stats.e_send=network_stats->oerrors;
		strncpy(name,network_stats->interface_name,MAX_LINE_BUFFER);
		name[MAX_LINE_BUFFER-1]='\0';
		network_stats++;

		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose
#ifdef IOCTL
				,check_if_up(name)
#else
				,1
#endif
				);
		current_if_num++;
	}
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);
	free(name);

	return;
}
#endif


#ifdef NETSTAT
/* do the actual work, get and print stats if verbose */
void get_iface_stats_netstat (char verbose) {
    int current_if_num=0,hidden_if=0;
	char *buffer=NULL,*name=NULL;
#if NETSTAT_BSD	|| NETSTAT_BSD_BYTES || NETSTAT_SOLARIS
	char *str_buf=NULL;
#endif	
	FILE *f=NULL;
	unsigned long long buf;

	t_iface_stats stats,tmp_if_stats; /* local struct, used to calc total values */
    memset(&stats,0,(size_t)sizeof(t_iface_stats)); /* init it */
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
                    ,"r")))
        deinit("no input stream found: %s\n",strerror(errno));
    buffer=(char *)malloc(MAX_LINE_BUFFER);
#ifdef NETSTAT_LINUX
    /* we skip first 2 lines if not bsd at any mode */
    if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) 
		deinit("read of netstat failed: %s\n",strerror(errno));
#endif
#if NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS
	str_buf=(char *)malloc(MAX_LINE_BUFFER);
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit("read of netstat failed: %s\n",strerror(errno));
#endif
    name=(char *)malloc(MAX_LINE_BUFFER);
    while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL && buffer[0]!='\n') ) {
        memset(&tmp_if_stats,0,(size_t)sizeof(t_iface_stats)); /* reinit it to zero */
#ifdef NETSTAT_LINUX		
        sscanf(buffer,"%s%llu%llu%llu%llu%llu%llu%llu%llu",name,&buf,&buf,&tmp_if_stats.p_rec,&tmp_if_stats.e_rec,&buf,&buf,&tmp_if_stats.p_send,&tmp_if_stats.e_send);
#endif
#if NETSTAT_BSD_BYTES
		sscanf(buffer,"%s%llu%s%s%llu%llu%llu%llu%llu%llu",name,&buf,str_buf,str_buf,&tmp_if_stats.p_rec,&tmp_if_stats.e_rec,&tmp_if_stats.rec,&tmp_if_stats.p_send,&tmp_if_stats.e_send,&tmp_if_stats.send);
#endif
#if NETSTAT_BSD	|| NETSTAT_SOLARIS	
		 sscanf(buffer,"%s%llu%s%s%llu%llu%llu%llu",name,&buf,str_buf,str_buf,&tmp_if_stats.p_rec,&tmp_if_stats.e_rec,&tmp_if_stats.p_send,&tmp_if_stats.e_send);
#endif
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose,
#if NETSTAT_BSD || NETSTAT_BSD_BYTES
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
#ifdef NETSTAT_BSD	
	free(str_buf);
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
    t_iface_stats stats,tmp_if_stats; /* local struct, used to calc total values */

    memset(&stats,0,(size_t)sizeof(t_iface_stats)); /* init it */

    /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
    if (sysctl(mib, 6, NULL, &size, NULL, 0) < 0) deinit("sysctl failed: %s\n",strerror(errno));
    if (!(bsd_if_buf = malloc(size))) deinit("no memory: %s\n",strerror(errno));
    bzero(bsd_if_buf,size);
    if (sysctl(mib, 6, bsd_if_buf, &size, NULL, 0) < 0) {
        my_errno=errno;
        free(bsd_if_buf);
        deinit("sysctl failed: %s\n",strerror(my_errno));
    }

    lim = (bsd_if_buf + size);

    next = bsd_if_buf;
    name=(char *)malloc(MAX_LINE_BUFFER);
    /* loop either while netstat enabled and still lines to read
     * or still buffer (buf) left */
    while (next < (bsd_if_buf + size)) {
        memset(&tmp_if_stats,0,(size_t)sizeof(t_iface_stats)); /* reinit it to zero */
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
        strncpy(name,saddr->sdl_data,MAX_LINE_BUFFER);
		name[MAX_LINE_BUFFER-1]='\0';
        tmp_if_stats.rec=ifmhdr->ifm_data.ifi_ibytes;
        tmp_if_stats.send=ifmhdr->ifm_data.ifi_obytes;
        tmp_if_stats.p_rec=ifmhdr->ifm_data.ifi_ipackets;
        tmp_if_stats.p_send=ifmhdr->ifm_data.ifi_opackets; 
		tmp_if_stats.e_rec=ifmhdr->ifm_data.ifi_ierrors;
        tmp_if_stats.e_send=ifmhdr->ifm_data.ifi_oerrors;
        /* init new interfaces and add fetched data to old or new one */
        hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose, iface_is_up);
        current_if_num++;
    } /* fgets done (while) */
    /* add to total stats and output current stats if verbose */
    finish_iface_stats (verbose, stats, hidden_if,current_if_num);
    /* clean buffers */
    free(name);
    /* close input stream */
    free(bsd_if_buf);
    return;
}
#endif
