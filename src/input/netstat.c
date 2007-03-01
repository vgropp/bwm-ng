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

#include "netstat.h"

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
	if (!(f=(FILE *)popen(
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
        if (count_tokens(buffer)>10) /* including address */
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
		  strlcpy(last_name,name,MAX_LINE_BUFFER);
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


