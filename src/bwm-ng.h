/******************************************************************************
 *  bwm-ng                                                                    *
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
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA *
 *                                                                            *
 *****************************************************************************/

#ifndef __BWM_NG_H
#define __BWM_NG_H

#include "defines.h"
#include "types.h"
#include "curses_tools.h"
#include "options.h"
#include "output.h"
#include "input/retrieve.h"
#include "help.h"

#ifdef WIN32
#include <windows.h>
#endif

#ifdef __STDC__
#include <stdarg.h>
void deinit(int code, const char *error_msg, ...) FUNCATTR_NORETURN;
#else
#include <varargs.h>
void deinit(int code, ...) FUNCATTR_NORETURN;
#endif

#endif
