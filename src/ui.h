/* ui.h - generic interface
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

#ifndef __SP_UI_H
#define __SP_UI_H

#include <string>

class Interface {

protected:

public:
	Interface();
	virtual ~Interface();
	virtual void main();
	virtual int playerBet();
	virtual void playerDiscard();
	virtual void viewMenu(int);
	virtual int playMenu();
	virtual void stats();
	virtual void options();
	virtual void genEvent(std::string);
	virtual void playerEvent(std::string);
	virtual void pause();
	virtual void space();
	
};

#endif

