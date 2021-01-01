/* scrollwin.cpp - scrollable WINDOW buffer
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

#include <vector>
#include <string>
#include <curses.h>

#include "scrollwin.h"

ScrollWin::ScrollWin(WINDOW* msgwin, int _height, int _width) {
	win = msgwin;
	height = _height;
	width = _width;
	pos = 0;
}

ScrollWin::~ScrollWin() {
}

void ScrollWin::push(string str) {
	if (pos > 0) bottom();
	// The string is less the the width of the window, so it's easy
	if (static_cast<signed>(str.size()) < width) {
		buffer.push_back(str);
		wprintw(win, "\n%s", str.c_str());
	// The string is wider than the window, so we need to take care of
	// indenting
	} else if (static_cast<signed>(str.size()) > width-1) {
		vector<string> par;
		int br = str.rfind(' ', width-1);
		par.push_back(str.substr(0, br));
		str = str.substr(br+1);
		while (static_cast<signed>(str.size()) > width-2) {
			br = str.rfind(' ', width-2);
			par.push_back(" " + str.substr(0, br));
			str = str.substr(br+1);
		}
		par.push_back(" " + str);
		for (int i=0; i<static_cast<signed>(par.size()); i++) {
			buffer.push_back(par[i]);
			wprintw(win, "\n%s", par[i].c_str());
		}
	}
	wrefresh(win);
}

void ScrollWin::bottom() {
	werase(win);
	for (int i=0; i<height; i++) {
		mvwprintw(win, i, 0, "%s", buffer[buffer.size()-(height-i)].c_str());
	}
	pos = 0;
}

void ScrollWin::scrollDown() {
	if (pos == 0) return;
	wscrl(win, 1);
	pos--;
	mvwprintw(win, height-1, 0, "%s", buffer[buffer.size()-pos-1].c_str());
}

void ScrollWin::scrollPgDown() {
	if (pos == 0) return;
	int lines = height/2;
	if (lines > pos) {
		lines = pos;
		pos = 0;
	} else {
		pos -= lines;
	}
	wscrl(win, lines);
	for (int i=0; i<lines; i++) {
		mvwprintw(win, height-1-i, 0, "%s",
			buffer[buffer.size()-pos-1-i].c_str());
	}
}

void ScrollWin::scrollUp() {
	if (pos + height == static_cast<signed>(buffer.size()) ||
		static_cast<signed>(buffer.size()) < height) return;
	wscrl(win, -1);
	pos++;
	if (static_cast<signed>(buffer.size()) - pos > height-1) {
		mvwprintw(win, 0, 0, "%s",
			buffer[buffer.size()-pos-height].c_str());
	}
}

void ScrollWin::scrollPgUp() {
	if (pos + height == static_cast<signed>(buffer.size()) ||
		static_cast<signed>(buffer.size()) <height) return;
	int lines = height/2;
	if (lines + pos + height > static_cast<signed>(buffer.size())) {
		lines = static_cast<signed>(buffer.size()) - pos - height;
		pos = static_cast<signed>(buffer.size()) - height;
	} else {
		pos += lines;
	}
	wscrl(win, -lines);
	for (int i=0; i<lines; i++) {
		mvwprintw(win, 0+i, 0, "%s",
			buffer[buffer.size()-pos-height+i].c_str());
	}
}

#endif

