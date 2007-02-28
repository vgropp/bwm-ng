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

#include "ioservice.h"

#if IOSERVICE_IN 
void get_disk_stats_ioservice (char verbose) {
   int current_if_num=0,hidden_if=0;
	io_iterator_t dlist  = 0;
	mach_port_t port = 0;
	io_registry_entry_t disk = 0; 
	CFDictionaryRef props = 0;
	CFDictionaryRef props2 = 0;
	CFDictionaryRef dstats = 0;
	CFNumberRef value = NULL;
	CFMutableDictionaryRef match;
	io_registry_entry_t parent;
	io_name_t name; 
	char deviceFilePath[MAXPATHLEN]; //MAXPATHLEN is defined in sys/param.h
	CFStringRef name_str;

   t_iface_speed_stats stats; /* local struct, used to calc total values */
   t_iface_speed_stats tmp_if_stats;
   memset(&stats,0,(size_t)sizeof(t_iface_speed_stats)); /* init it */
	
	if (IOMasterPort(MACH_PORT_NULL, &port)) 
		deinit(1,"failure while initializing disk port\n");
	match = IOServiceMatching("IOMedia");
	CFDictionaryAddValue(match, CFSTR(kIOMediaWholeKey), kCFBooleanTrue);	
	if (IOServiceGetMatchingServices(port, match, &dlist)!=KERN_SUCCESS)
		deinit(1,"failure while getting disk list\n");
	while ( (disk = IOIteratorNext(dlist)) ) {
		IORegistryEntryCreateCFProperties (disk,(CFMutableDictionaryRef *) &props,kCFAllocatorDefault,kNilOptions);
		if (props) {
			if (!long_darwin_disk_names) {
				name_str = (CFStringRef)CFDictionaryGetValue(props, CFSTR(kIOBSDNameKey));
				if (name_str) {
					CFStringGetCString(name_str, deviceFilePath, MAXPATHLEN-1, CFStringGetSystemEncoding());
				} else {
					snprintf((char *)deviceFilePath,MAXPATHLEN-1,"unknown%i",current_if_num);
               deviceFilePath[MAXPATHLEN-1]=0;
				}
			} else {
				if (IORegistryEntryGetName(disk, name )!=KERN_SUCCESS) {
					snprintf((char *)name,sizeof(name)-1,"unknown%i",current_if_num);
					name[sizeof(name)-1]=0;
				}
			}
			if (IORegistryEntryGetParentEntry(disk, kIOServicePlane, &parent)!=KERN_SUCCESS) {
				CFRelease(props);
				IOObjectRelease(disk);
				IOIteratorReset(dlist);
				deinit(1,"disk has no parent\n");
			}
			IORegistryEntryCreateCFProperties(parent, (CFMutableDictionaryRef *)&props2,kCFAllocatorDefault, kNilOptions);
			if (props2) {
				dstats = CFDictionaryGetValue(props2, CFSTR(kIOBlockStorageDriverStatisticsKey));
				if (dstats) {
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsBytesReadKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.bytes.in);
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsBytesWrittenKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.bytes.out);
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsReadsKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.packets.in);
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsWritesKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.packets.out);
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsReadErrorsKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.errors.in);
					value = CFDictionaryGetValue (dstats,CFSTR(kIOBlockStorageDriverStatisticsWriteErrorsKey));
					if (value)
						CFNumberGetValue(value, kCFNumberSInt64Type, &tmp_if_stats.errors.out);
					hidden_if = process_if_data (hidden_if, tmp_if_stats, &stats, long_darwin_disk_names ?	(char *)name :	deviceFilePath
							, current_if_num, verbose,(tmp_if_stats.bytes.in != 0 || tmp_if_stats.bytes.out != 0));
					current_if_num++;
				}
				CFRelease(props2);
			}
			IOObjectRelease(parent);
			CFRelease(props); 
			props = 0;
		}
		IOObjectRelease(disk);
		disk = 0;
	}
	finish_iface_stats (verbose, stats, hidden_if,current_if_num);
	IOObjectRelease(dlist);
}
#endif


