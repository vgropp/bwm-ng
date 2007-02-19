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

#ifndef __CURSES_TOOLS_H
#define __CURSES_TOOLS_H 

#include "defines.h"
#include "types.h"
#include "help.h"
#include "options.h"

#ifdef HAVE_CURSES
#ifdef HAVE_STROPTS_H
#include <stropts.h>
#endif
#include <unistd.h>
#ifdef HAVE_LINUX_TTY_H
#include <linux/tty.h>
#else
#ifdef HAVE_SYS_TTY_H
#include <sys/tty.h>
#endif
#endif

/* handle key input by user in gui (curses) mode */
void handle_gui_input(char c);
int init_curses();
void sigwinch(int sig);
#endif


#endif
