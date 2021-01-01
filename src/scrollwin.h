/* scrollwin.h - scrollable WINDOW buffer
 * 
 * Copyright (C) 1999-2000  Kevin Radloff <kmradlof@colby.edu>
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_NCURSES

#ifndef __SP_SCROLLWIN_H
#define __SP_SCROLLWIN_H

#include <vector>
#include <string>
#include <curses.h>

class ScrollWin {

protected:
	WINDOW* win;
	int height;
	int width;
	vector<string> buffer;
	int pos;

public:
	ScrollWin(WINDOW*, int, int);
	~ScrollWin();
	void push(string);
	void bottom();
	void scrollUp();
	void scrollPgUp();
	void scrollDown();
	void scrollPgDown();

};

#endif

#endif

