/*  libticalcs - calculator library, a part of the TiLP project
 *  Copyright (C) 1999-2002  Romain Lievin
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __CABLES_PAUSE_H__
#define __CABLES_PAUSE_H__

// Little pause in milli-seconds

#define pause_between_vars	250		// 250ms

#if defined(__UNIX__) || defined(__LINUX__)
# include <unistd.h>
# define PAUSE(x)  usleep(1000*(x));
#elif defined(__WIN32__)
# include <windows.h>
# define PAUSE(x)  Sleep((x));
#endif

#endif


