/******************************************************************************
 *  bwm-ng options header                                                     *
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


#ifndef __OPTIONS_H
#define __OPTIONS_H

#include "defines.h"
#include "types.h"

#ifdef CONFIG_FILE
#include <ctype.h>
#include <string.h>
#ifdef HAVE_GETPWUID
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#endif
#endif

#include "help.h"
#include "bwm-ng.h"

void get_cmdln_options(int argc, char *argv[]);

#endif
