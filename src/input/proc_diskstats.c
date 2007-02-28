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

#include "proc_diskstats.h"


#ifdef PROC_DISKSTATS

/* convert devfs name to a short abr - some ugly code 
 * return: 1 on success, -1 on nothing to be done, 0 on failure */
int get_short_devfs_name(char *devicename) {
	char *short_devicename=NULL;
	char *ptr=NULL;

	if ((ptr=strchr(devicename,'/'))) {
		if (!(short_devicename=(char *)malloc(MAX_LINE_BUFFER)))
			return 0;
		strncpy(short_devicename,devicename,(int)(ptr-devicename));
		short_devicename[(int)(ptr-devicename)]=0;
		while ((ptr=strchr(ptr,'/'))) {
			ptr++;
			strncat(short_devicename,&ptr[0],1);
			while (tolower(ptr[0])>='a' && tolower(ptr[0])<='z') ptr++;
			while (ptr[0]>='0' && ptr[0]<='9') {
				strncat(short_devicename,&ptr[0],1);
				ptr++;
			}
		}
		strcpy(devicename,short_devicename);
		free(short_devicename);
		return 1;
	}
	return -1;
}

/* do the actual work, get and print stats if verbose */
void get_disk_stats_proc (char verbose) {
   FILE *f=NULL,*f_s=NULL;
   char *buffer=NULL,*name=NULL,*ptr;
	ullong tmp_long;
	int n,major,minor,maj_s,min_s;
	static char diskstats_works = 1;
	static char proc_stat[PATH_MAX] = "";

   int hidden_if=0,current_if_num=0;
   t_iface_speed_stats stats; /* local struct, used to calc total values */
   t_iface_speed_stats tmp_if_stats;

   memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
   /* dont open proc_net_dev if netstat_i is requested, else try to open and if it fails fallback */
	if (diskstats_works && !(f=fopen(PROC_DISKSTATS_FILE,"r"))) {
		diskstats_works = 0;
	}
	buffer=(char *)malloc(MAX_LINE_BUFFER);
   name=(char *)malloc(MAX_LINE_BUFFER);
   if (!name || !buffer) {
      if (name) free(name);
      if (buffer) free(buffer);
		if (f) fclose(f);
      deinit(1,"mem alloc failed: %s\n",strerror(errno));
   }

	if (!diskstats_works) {
		if (!(f=fopen(PROC_PARTITIONS_FILE,"r"))) {
			diskstats_works = 1;
		} else {
			/* skip first two lines in /proc/partitions */
			fgets(buffer,MAX_LINE_BUFFER,f);
			fgets(buffer,MAX_LINE_BUFFER,f);
		}
	}

   while ( (fgets(buffer,MAX_LINE_BUFFER,f) != NULL) ) {
      n = sscanf(buffer,
				(diskstats_works ? "%i %i %s %llu%llu%llu%llu%llu%llu%llu%*i" : "%i %i %*i %s %llu%llu%llu%llu%llu%llu%llu%*i"),
				&major,&minor,name,&tmp_if_stats.packets.in,&tmp_if_stats.errors.in,&tmp_if_stats.bytes.in,&tmp_long,&tmp_if_stats.packets.out,&tmp_if_stats.errors.out,&tmp_if_stats.bytes.out);
		/* skip loop devices, we dont see stats anyway */
		if (major == 7) continue;
		if (n == 7) {
			/* its a partition with shorter stats, move all to correct fields */
			tmp_if_stats.packets.out=tmp_if_stats.bytes.in;
			tmp_if_stats.bytes.in=tmp_if_stats.errors.in;
			tmp_if_stats.bytes.out=tmp_long;
			tmp_if_stats.errors.in=0;
			tmp_if_stats.errors.out=0;
		} else { 
			/* having 10 fields all is just fine, else check for other formats */
			if (n != 10) {
				if (diskstats_works == 0 && n == 3) { 
					/* we are reading /proc/partitions and have the 
					 * old 2.4 partitions format, look in /proc/stat for block devince data */
					if (proc_stat[0] == 0) { 
						/* build /proc/stat path */
						strcpy(proc_stat,PROC_PARTITIONS_FILE);
						if ((ptr=strrchr(proc_stat,'/'))) {
							ptr++;
							strcpy(ptr,"stat");
						} else {
							free(name);
							free(buffer);
							deinit(1, "strange /proc/partitions name, couldnt build /proc/stats name\n");
						}
					}
					if (!(f_s=fopen(proc_stat,"r"))) {
	               free(name);
                  free(buffer);
                  deinit(1, "couldnt open %s: %s\n",proc_stat,strerror(errno));
               }
					while ( (fgets(buffer,MAX_LINE_BUFFER,f_s) != NULL)) {
						if (!strncmp("disk_io:",buffer,8)) {
							ptr=buffer+9;
							while (ptr[0]!=0) {
								n = sscanf(ptr,"(%i,%i): (%*i,%llu,%llu,%llu,%llu)",&maj_s,&min_s,&tmp_if_stats.packets.in,&tmp_if_stats.bytes.in,&tmp_if_stats.packets.out,&tmp_if_stats.bytes.out);
								if (maj_s==major && min_s==minor) {
									/* we found the correct device */
									fclose(f_s);
									f_s=NULL;
									break;
								}
								if (!(ptr=strchr(ptr,' ')))
									break;
								ptr++;
							}
							if (!f_s) 
								break;
						}
					}
					if (f_s) {
						fclose(f_s);
						/* couldnt find the entry, prolly a partition */
						continue;
					}
				} else {
					/* neither new nor old /proc/partitions nor /proc/diskstats */
					free(name);
					free(buffer);
					deinit(1, "wrong format of procfile. %i: %s\n",n,buffer);
				}
			}
		}
		/* stats are in 512 byte blocks */
		tmp_if_stats.bytes.in*=512;
		tmp_if_stats.bytes.out*=512;
		/* convert devfs name to a short abr */
		get_short_devfs_name(name);
      /* init new interfaces and add fetched data to old or new one */
      hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, name, current_if_num, verbose,(n==10 || proc_stat[0] != 0));
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


