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

#include "retrieve.h"

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

/* chooses the correct get_iface_stats() to use */
inline void get_iface_stats(char _n) {
   switch (input_method) {
#ifdef NETSTAT
       case NETSTAT_IN:
         get_iface_stats_netstat(_n);
           break;
#endif
#ifdef LIBSTATGRAB
      case LIBSTAT_IN:
            get_iface_stats_libstat(_n);
            break;
      case LIBSTATDISK_IN:
            get_iface_stats_libstatdisk(_n);
            break;
#endif
#ifdef PROC_NET_DEV
       case PROC_IN:
            get_iface_stats_proc(_n);
            break;
#endif
#ifdef GETIFADDRS
       case GETIFADDRS_IN:
            get_iface_stats_getifaddrs(_n);
            break;
#endif
#if DEVSTAT_IN
		case DEVSTAT_IN:
				get_iface_stats_devstat(_n);
				break;
#endif
#ifdef SYSCTL
        case SYSCTL_IN:
            get_iface_stats_sysctl(_n);
            break;
#endif				
#if SYSCTLDISK_IN
			case SYSCTLDISK_IN:
				get_iface_stats_sysctldisk(_n);
				break;
#endif
#if HAVE_LIBKSTAT
		  case KSTATDISK_IN:
		  case KSTAT_IN:
            get_iface_stats_kstat(_n);
            break;
#endif
#ifdef WIN32
        case WIN32_IN:
            get_iface_stats_win32(_n);
            break;
#endif
#ifdef PROC_DISKSTATS
        case DISKLINUX_IN:
            get_disk_stats_proc(_n);
            break;
#endif
#if IOSERVICE_IN
		  case IOSERVICE_IN:
				get_disk_stats_ioservice(_n);
				break;
#endif
   }
}

