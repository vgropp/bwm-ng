/******************************************************************************
 *  bwm-ng                                                                    *
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

struct iface_stats {
    char    *if_name;
    long double max_rec;
    long double max_send;
    long double max_total;
    long double max_prec;
    long double max_psend;
    long double max_ptotal;
    long double max_erec;
    long double max_esend;
    long double max_etotal;
    unsigned long long rec;
    unsigned long long send;
    unsigned long long p_rec;
    unsigned long long p_send;
    unsigned long long e_rec;
    unsigned long long e_send;
#if HAVE_GETTIMEOFDAY
    struct timeval time;
#endif
};

typedef struct iface_stats t_iface_stats;

struct iface_speed_stats {
    unsigned long long errors_in;
    unsigned long long errors_out;
    unsigned long long packets_out;
    unsigned long long packets_in;
    unsigned long long bytess;
    unsigned long long bytesr;
    long double max_rec;
    long double max_send;
    long double max_total;
    long double max_prec;
    long double max_psend;
    long double max_ptotal;
    long double max_erec;
    long double max_esend;
    long double max_etotal;
};

typedef struct iface_speed_stats t_iface_speed_stats;
