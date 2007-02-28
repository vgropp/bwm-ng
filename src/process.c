/******************************************************************************
 *  bwm-ng process data                                                       *
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

#include "global_vars.h"
#include "process.h"

/* returns the whether to show the iface or not
 * if is in list return 1, if list is prefaced with ! or 
 * name not found return 0 */
short show_iface(char *instr, char *searchstr,char iface_is_up) {
	int pos = 0,k,i=0,success_ret=1;
    if (instr==NULL) return iface_is_up || (show_all_if==2);
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
					return success_ret || (iface_is_up && show_all_if);
                }
				pos = 0;
				break;
			default:
				pos++;
				break;
		}
    }
	return !success_ret || (iface_is_up && show_all_if) || (show_all_if==2);
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

/* basically new-old, but handles "overflow" of source aswell */
inline ullong calc_new_values(ullong new, ullong old) {
    /* FIXME: WRAP_AROUND _might_ be wrong for libstatgrab, where the type is always long long */
    return (new>=old) ? (ullong)(new-old) : (ullong)((
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

/* calc actual new-old values */
t_iface_speed_stats convert2calced_disk_values(t_iface_speed_stats new, t_iface_speed_stats old) {
   t_iface_speed_stats calced_stats;
	calced_stats.bytes.out=calc_new_values(new.bytes.out,old.bytes.out);
	calced_stats.bytes.in=calc_new_values(new.bytes.in,old.bytes.in);
	/* needed for linux stats, read and write count */
	calced_stats.packets.out=calc_new_values(new.packets.out,old.packets.out)*(calc_new_values(new.errors.out,old.errors.out)+1);
	calced_stats.packets.in=calc_new_values(new.packets.in,old.packets.in)*(calc_new_values(new.errors.in,old.errors.in)+1);
	calced_stats.errors.in=0;
	calced_stats.errors.out=0;
	return calced_stats;
}



#if EXTENDED_STATS
/* sub old values from cached for avg stats */
inline void sub_avg_values(struct inouttotal_double *values,struct inouttotal_double data) {
    values->in-=data.in;
    values->out-=data.out;
    values->total-=data.total;
}

inline void add_avg_values(struct inouttotal_double *values,struct inouttotal_double data) {
    values->in+=data.in;
    values->out+=data.out;
    values->total+=data.total;
}


/* put new-old bytes in inout_long struct into a inouttotal_double struct 
 * and add values to cached .value struct */
inline void save_avg_values(struct inouttotal_double *values,struct inouttotal_double *data,struct inout_long calced_stats,float multiplier) {
    data->in=calced_stats.in*multiplier;
    data->out=calced_stats.out*multiplier;
    data->total=(calced_stats.in+calced_stats.out)*multiplier;
    add_avg_values(values,*data);
}


/* manages the list of values for avg
 * saves data in list
 * removes old entries
 * calculates the current value */
void save_avg(struct t_avg *avg,struct iface_speed_stats calced_stats,float multiplier) {
    struct double_list *list_p;
    if (avg->first==NULL) { /* first element */
        avg->first=avg->last=(struct double_list *)malloc(sizeof(struct double_list));
        /* init it to zero and NULL */
        memset(avg->first,0,sizeof(struct double_list)); 
        /* save data and add to cache */
        save_avg_values(&avg->item_sum.bytes,&avg->first->data.bytes,calced_stats.bytes,multiplier);
        save_avg_values(&avg->item_sum.errors,&avg->first->data.errors,calced_stats.errors,multiplier);
        save_avg_values(&avg->item_sum.packets,&avg->first->data.packets,calced_stats.packets,multiplier);
        avg->items=1;
    } else { /* we already have a list */
        avg->last->next=(struct double_list *)malloc(sizeof(struct double_list));
        memset(avg->last->next,0,sizeof(struct double_list));
        avg->last=avg->last->next;
        /* save data and add to cache */
        save_avg_values(&avg->item_sum.bytes,&avg->last->data.bytes,calced_stats.bytes,multiplier);
        save_avg_values(&avg->item_sum.errors,&avg->last->data.errors,calced_stats.errors,multiplier);
        save_avg_values(&avg->item_sum.packets,&avg->last->data.packets,calced_stats.packets,multiplier);
        avg->items++;
        /* remove only entries if at least two items added, 
         * else we might leave an empty list 
         * avg->first has to be != NULL at this point (if in 2nd line of this function) */
        while (avg->first->next!=NULL && avg->items>avg_length/delay) {
            /* list is full, remove first entry */
            list_p=avg->first;
            avg->first=avg->first->next;
            /* sub values from cache */
            sub_avg_values(&avg->item_sum.bytes,list_p->data.bytes);
            sub_avg_values(&avg->item_sum.errors,list_p->data.errors);
            sub_avg_values(&avg->item_sum.packets,list_p->data.packets);
            free(list_p);
            avg->items--;
        }
    }
}

/* add current in and out bytes to totals struct */
inline void save_sum(struct inout_long *stats,struct inout_long new_stats_values) {
    stats->in+=new_stats_values.in;
    stats->out+=new_stats_values.out;
}

/* lookup old max values and save new if higher */
inline void save_max(struct inouttotal_double *stats,struct inout_long calced_stats,float multiplier) {
    if (multiplier*calced_stats.in > stats->in)
        stats->in=multiplier*calced_stats.in;
    if (multiplier*calced_stats.out>stats->out)
        stats->out=multiplier*calced_stats.out;
    if (multiplier*(calced_stats.out+calced_stats.in)>stats->total)
        stats->total=multiplier*(calced_stats.in+calced_stats.out);
}
#endif

/* will be called by get_iface_stats for each interface
 * inserts and calcs current stats.
 * will call output (print_values) aswell if needed */
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
        /* alloc and init */
        if_stats=(t_iface_stats*)realloc(if_stats,sizeof(t_iface_stats)*if_count);
        memset(&if_stats[local_if_count],0,(size_t)sizeof(t_iface_stats));
        /* copy the iface name or add a dummy one */
        if (name[0]!='\0')
            if_stats[if_count-1].if_name=(char*)strdup(name);
        else
            if_stats[if_count-1].if_name=(char*)strdup("unknown");
        /* set it to current value, so there is no peak at first announce */
        if_stats[local_if_count].data=tmp_if_stats;
        if (show_iface(iface_list,name,iface_is_up) && (net_input_method(input_method) || iface_is_up)) {
            /* add the values to total stats now */
            if_stats_total.data.bytes.out+=tmp_if_stats.bytes.out;
            if_stats_total.data.bytes.in+=tmp_if_stats.bytes.in;
            if_stats_total.data.packets.out+=tmp_if_stats.packets.out;
            if_stats_total.data.packets.in+=tmp_if_stats.packets.in;
            if_stats_total.data.errors.out+=tmp_if_stats.errors.out;
            if_stats_total.data.errors.in+=tmp_if_stats.errors.in;
        }
    }
#if HAVE_GETTIMEOFDAY
    multiplier=(float)get_time_delay(local_if_count);
#endif
    /* calc new-old, so we have the new bytes,errors,packets */
	 if (net_input_method(input_method))
	    calced_stats=convert2calced_values(tmp_if_stats,if_stats[local_if_count].data);
	 else
		 calced_stats=convert2calced_disk_values(tmp_if_stats,if_stats[local_if_count].data);
#if EXTENDED_STATS    
    /* save new max values in both, calced (for output) and ifstats */
    save_max(&if_stats[local_if_count].max.bytes,calced_stats.bytes,multiplier);
    save_max(&if_stats[local_if_count].max.errors,calced_stats.errors,multiplier);
    save_max(&if_stats[local_if_count].max.packets,calced_stats.packets,multiplier);
    /* save sum now aswell */
    save_sum(&if_stats[local_if_count].sum.bytes,calced_stats.bytes);
    save_sum(&if_stats[local_if_count].sum.packets,calced_stats.packets);
    save_sum(&if_stats[local_if_count].sum.errors,calced_stats.errors);
    /* fill avg struct if there is old data */
    save_avg(&if_stats[local_if_count].avg,calced_stats,multiplier); 
#endif    
    if (verbose) { /* any output at all? */
        /* cycle: show all interfaces, only those which are up, only up and not hidden */
        if (show_iface(iface_list,name,iface_is_up)) {/* is it up or do we show all ifaces? */
            print_values(5+iface_number-hidden_if,2,name,calced_stats,multiplier,if_stats[local_if_count]);
		} else
            hidden_if++; /* increase the opt cause we dont show this if */
    }
    /* save current stats for the next run */
    if_stats[local_if_count].data=tmp_if_stats;
    /* add stats to new total */
    if (show_iface(iface_list,name,iface_is_up)) {
        stats->bytes.out+=tmp_if_stats.bytes.out;
        stats->bytes.in+=tmp_if_stats.bytes.in;
        stats->packets.out+=tmp_if_stats.packets.out;
        stats->packets.in+=tmp_if_stats.packets.in;
        stats->errors.out+=tmp_if_stats.errors.out;
        stats->errors.in+=tmp_if_stats.errors.in;
    } 
	return hidden_if;
}	

/* handles and calls output totals of all interfaces */
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
	if (net_input_method(input_method))
		calced_stats=convert2calced_values(stats,if_stats_total.data);
	else
		calced_stats=convert2calced_disk_values(stats,if_stats_total.data);
#if EXTENDED_STATS    
    /* save new max values in both, calced (for output) and final stats */
    save_max(&if_stats_total.max.bytes,calced_stats.bytes,multiplier);
    save_max(&if_stats_total.max.errors,calced_stats.errors,multiplier);
    save_max(&if_stats_total.max.packets,calced_stats.packets,multiplier);
    save_sum(&if_stats_total.sum.bytes,calced_stats.bytes);
    save_sum(&if_stats_total.sum.packets,calced_stats.packets);
    save_sum(&if_stats_total.sum.errors,calced_stats.errors);
    save_avg(&if_stats_total.avg,calced_stats,multiplier);
#endif
    if (verbose) {
        /* output total ifaces stats */
#ifdef HAVE_CURSES		
        if (output_method==CURSES_OUT)
            mvwprintw(stdscr,5+iface_number-hidden_if,2,"------------------------------------------------------------------------------");
        else 
#endif			
			if (output_method==PLAIN_OUT || output_method==PLAIN_OUT_ONCE)
				printf("%s------------------------------------------------------------------------------\n",output_method==PLAIN_OUT ? " " : "");
        print_values(6+iface_number-hidden_if,2,"total",calced_stats,multiplier,if_stats_total);
    }
    /* save the data in total-struct */
    if_stats_total.data=stats;
	if (output_method==PLAIN_OUT)
		for (i=0;i<if_count-iface_number;i++) printf("%70s\n"," "); /* clear unused lines */
	return;
}

