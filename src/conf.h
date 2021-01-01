/* conf.h - configuration settings class
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

#ifndef __SP_CONF_H
#define __SP_CONF_H

#include <cstdio>

class Config {

protected:
	int players;
	int account;
	int drawBetcap;
	int studBetcap;
#ifdef HAVE_NCURSES
	int color;
	int cardpic;
#endif
	int interface;

public:
	Config();
	inline int getPlayers() { return players; }
	inline int getAccount() { return account; }
	inline int getDrawBetcap() { return drawBetcap; }
	inline int getStudBetcap() { return studBetcap; }
	inline int getInterface() { return interface; }
	inline void setPlayers(int n) { players = n; }
	inline void setAccount(int n) { account = n; }
	inline void setDrawBetcap(int n) { drawBetcap = n; }
	inline void setStudBetcap(int n) { studBetcap = n; }
#ifdef HAVE_NCURSES
	inline int getColor() { return color; }
	inline int getCardPic() { return cardpic; }
	inline void toggleColor() { color = !color; }
	inline void toggleCardPic() { cardpic = !cardpic; }
#endif
	void open(FILE *);
	void save();

};

#endif

