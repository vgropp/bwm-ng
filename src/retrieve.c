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
	int pos = 0,k,i=0,success_ret=1;
    if (instr==NULL) return success_ret;
    if (instr[0]=='%') {
        success_ret=!success_ret;
        i++;
    }
	k = strlen( searchstr );
	for (;i<=strlen(instr);i++) {
		switch ( instr[i] ) {
			case 0:
			case ',':
				if ( k == pos && ! strncasecmp( &instr[i] - pos, searchstr, pos ) ) {
					return success_ret;
                }
				pos = 0;
				break;
			default:
				pos++;
				break;
		}
    }
	return !success_ret;
}


/* counts the tokens in a string */
long count_tokens(char *in_str) {
    long tokens=0;
    long i=0;
    char in_a_token=0;
    char *str;

    if (in_str==NULL) return 0;
    str=strdup(in_str);
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
    free(str);
    return tokens;
}


#if HAVE_GETTIMEOFDAY
/* Returns: the time difference in milliseconds. */
inline long tvdiff(struct timeval newer, struct timeval older) {
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

inline unsigned long long calc_new_values(unsigned long long new, unsigned long long old) {
    /* FIXME: WRAP_AROUND _might_ be wrong for libstatgrab, where the type is always long long */
    return (new>=old) ? (unsigned long long)(new-old) : (unsigned long long)((
#ifdef HAVE_LIBKSTAT
            (input_method==KSTAT_IN) ?
            WRAP_32BIT :
#endif
            WRAP_AROUND)
            -old)+new;
}


/* calc actual new-old values */
t_iface_speed_stats convert2calced_values(t_iface_speed_stats new, t_iface_speed_stats old) {
    t_iface_speed_stats calced_stats;
    calced_stats.errors.in=calc_new_values(new.errors.in,old.errors.in);
    calced_stats.errors.out=calc_new_values(new.errors.out,old.errors.out);
    calced_stats.packets.out=calc_new_values(new.packets.out,old.packets.out);
    calced_stats.packets.in=calc_new_values(new.packets.in,old.packets.in);
    calced_stats.bytes.out=calc_new_values(new.bytes.out,old.bytes.out);
    calced_stats.bytes.in=calc_new_values(new.bytes.in,old.bytes.in);
    return calced_stats;
}


inline void save_max(struct inouttotal_double *stats,struct inout_long calced_stats,float multiplier) {
    if (multiplier*calced_stats.in > stats->in)
        stats->in=multiplier*calced_stats.in;
    if (multiplier*calced_stats.out>stats->out)
        stats->out=multiplier*calced_stats.out;
    if (multiplier*(calced_stats.out+calced_stats.in)>stats->total)
        stats->total=multiplier*(calced_stats.in+calced_stats.out);
}

inline void init_double_types(struct inouttotal_double *in) {
    in->out=in->in=in->total=0;    
}

int process_if_data (int hidden_if, t_iface_speed_stats tmp_if_stats,t_iface_speed_stats *stats, char *name, int iface_number, char verbose, char iface_is_up) {
#if HAVE_GETTIMEOFDAY
    float multiplier;
#else
	float multiplier=(float)1000/delay;
#endif    
	int local_if_count;
    t_iface_speed_stats calced_stats;
    
    /* if_count starts at 1 for 1 interface, local_if_count starts at 0 */
    for (local_if_count=0;local_if_count<if_count;local_if_count++) {
        /* check if its the correct if */
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
        /* set it to current value, so there is no peak at first announce */
        if_stats[local_if_count].data=tmp_if_stats;
        if (sumhidden || ((show_all_if>1 || iface_is_up) &&
            (show_all_if || show_iface(iface_list,name)))) {
            /* add the values to total stats now */
            if_stats_total.data.bytes.out+=tmp_if_stats.bytes.out;
            if_stats_total.data.bytes.in+=tmp_if_stats.bytes.in;
            if_stats_total.data.packets.out+=tmp_if_stats.packets.out;
            if_stats_total.data.packets.in+=tmp_if_stats.packets.in;
            if_stats_total.data.errors.out+=tmp_if_stats.errors.out;
            if_stats_total.data.errors.in+=tmp_if_stats.errors.in;
        }
        /* init max values with 0 */
        init_double_types(&if_stats[local_if_count].max.bytes);
        init_double_types(&if_stats[local_if_count].max.packets);
        init_double_types(&if_stats[local_if_count].max.errors);
    }
#if HAVE_GETTIMEOFDAY
    multiplier=(float)get_time_delay(local_if_count);
#endif
    /* calc new-old, so we have the new bytes,errors,packets */
    calced_stats=convert2calced_values(tmp_if_stats,if_stats[local_if_count].data);
    /* save new max values in both, calced (for output) and ifstats */
    save_max(&if_stats[local_if_count].max.bytes,calced_stats.bytes,multiplier);
    save_max(&if_stats[local_if_count].max.errors,calced_stats.errors,multiplier);
    save_max(&if_stats[local_if_count].max.packets,calced_stats.packets,multiplier);
    if (verbose) { /* any output at all? */
        /* cycle: show all interfaces, only those which are up, only up and not hidden */
        if ((show_all_if>1 || iface_is_up) && /* is it up or do we show all ifaces? */
            (show_all_if || show_iface(iface_list,name))) {
            print_values(5+iface_number-hidden_if,8,name,calced_stats,multiplier,if_stats[local_if_count]);
		} else
            hidden_if++; /* increase the opt cause we dont show this if */
    }
    /* save current stats for the next run */
    if_stats[local_if_count].data=tmp_if_stats;
    /* add stats to new total */
    if (sumhidden || ((show_all_if>1 || iface_is_up) &&
            (show_all_if || show_iface(iface_list,name)))) {
        stats->bytes.out+=tmp_if_stats.bytes.out;
        stats->bytes.in+=tmp_if_stats.bytes.in;
        stats->packets.out+=tmp_if_stats.packets.out;
        stats->packets.in+=tmp_if_stats.packets.in;
        stats->errors.out+=tmp_if_stats.errors.out;
        stats->errors.in+=tmp_if_stats.errors.in;
    } 
	return hidden_if;
}	

void finish_iface_stats (char verbose, t_iface_speed_stats stats, int hidden_if, int iface_number) {
    int i;
    t_iface_speed_stats calced_stats;
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
    calced_stats=convert2calced_values(stats,if_stats_total.data);
    /* save new max values in both, calced (for output) and final stats */
    save_max(&if_stats_total.max.bytes,calced_stats.bytes,multiplier);
    save_max(&if_stats_total.max.errors,calced_stats.errors,multiplier);
    save_max(&if_stats_total.max.packets,calced_stats.packets,multiplier);

    if (verbose) {
        /* output total ifaces stats */
#ifdef HAVE_CURSES		
        if (output_method==CURSES_OUT)
            mvwprintw(stdscr,5+iface_number-hidden_if,8,"------------------------------------------------------------------");
        else 
#endif			
			if (output_method==PLAIN_OUT || output_method==PLAIN_OUT_ONCE)
				printf("%s------------------------------------------------------------------\n",output_method==PLAIN_OUT ? " " : "");
        print_values(6+iface_number-hidden_if,8,"total",calced_stats,multiplier,if_stats_total);
    }
    /* save the data in total-struct */
    if_stats_total.data=stats;
	if (output_method==PLAIN_OUT)
		for (i=0;i<if_count-iface_number;i++) printf("%70s\n"," "); /* clear unused lines */
	return;
}


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
		deinit("getifaddr failed: %s\n",strerror(errno));
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
		deinit("open of procfile failed: %s\n",strerror(errno));
	}
	buffer=(char *)malloc(MAX_LINE_BUFFER);
	/* we skip first 2 lines if not bsd at any mode */
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit("read of proc failed: %s\n",strerror(errno));
	name=(char *)malloc(MAX_LINE_BUFFER);
	while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL) ) {
        /* get the name */
        ptr=strchr(buffer,':');
        /* wrong format */
        if (ptr==NULL) { deinit("wrong format of input stream\n"); }
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
        deinit("libstatgrab error!\n");
    }
	
	while (num_network_stats>current_if_num) {
	    tmp_if_stats.bytes.in=network_stats->rx;
		tmp_if_stats.bytes.out=network_stats->tx;
	    tmp_if_stats.packets.in=network_stats->ipackets;
		tmp_if_stats.packets.out=network_stats->opackets;
	    tmp_if_stats.errors.in=network_stats->ierrors;
		tmp_if_stats.errors.out=network_stats->oerrors;
		network_stats++;

		hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, network_stats->interface_name, current_if_num, verbose
#ifdef IOCTL
				,check_if_up(network_stats->interface_name)
#else
				,1
#endif
				);
		current_if_num++;
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
        deinit("no input stream found: %s\n",strerror(errno));
#if NETSTAT_NETBSD
    if (!(f2=popen( NETSTAT_PATH " -i","r")))
        deinit("no input stream found: %s\n",strerror(errno));
    buffer2=(char *)malloc(MAX_LINE_BUFFER);
    if ((fgets(buffer2,MAX_LINE_BUFFER,f2) == NULL )) deinit("read of netstat failed: %s\n",strerror(errno));
    str_buf=(char *)malloc(MAX_LINE_BUFFER);
#endif
    buffer=(char *)malloc(MAX_LINE_BUFFER);
#ifdef NETSTAT_LINUX
    /* we skip first 2 lines if not bsd at any mode */
    if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL ) || (fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) 
		deinit("read of netstat failed: %s\n",strerror(errno));
#endif
#if NETSTAT_BSD || NETSTAT_BSD_BYTES || NETSTAT_SOLARIS || NETSTAT_NETBSD
    last_name=(char *)malloc(MAX_LINE_BUFFER);
    last_name[0]='\0'; /* init */
	if ((fgets(buffer,MAX_LINE_BUFFER,f) == NULL )) deinit("read of netstat failed: %s\n",strerror(errno));
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
        strcpy(last_name,name);
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
    if (kc==NULL) deinit("kstat failed: %s\n",strerror(my_errno));
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
