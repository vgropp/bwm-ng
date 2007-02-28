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

#include "win32.h"

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
		strncpy(name,(char*)(if_table->table[i].bDescr),MAX_INTERFACE_NAME_LEN);	
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



