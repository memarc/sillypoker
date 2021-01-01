/* games.cpp - gameplay execution
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

#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "poker.h"
#include "stats.h"
#include "conf.h"
#include "ui.h"

// globals
extern Interface* ui;
extern Deck* gamedeck;
extern vector<Player*> players;
extern Config* conf;
extern Stats* gamestats;
extern int human;
extern char buf[64];

// from main.cpp
void bet(void);
void discard(void);
// from util.cpp
void space(void);
void enter(void);

void drawFive(void) {
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		if (players[i]->getStatus() > 0) {
			players[i]->createHand(gamedeck, 5);
			if (i == human) {
				ui->playerEvent("You are dealt 5 cards.");
				players[i]->visHand();
			} else {
				sprintf(buf, "%s is dealt 5 cards.",
					players[i]->name());
				ui->playerEvent(buf);
			}
		}
	}
	players[human]->hand()->sort();
	ui->viewMenu(1);
	ui->space();
	bet();
	ui->space();
	discard();
	ui->viewMenu(2);
	ui->space();
	bet();
	for (int i=0; i<static_cast<signed>(players.size()); i++)
		if (players[i]->getStatus() > -1)
			players[i]->visHand();
}

void studSeven(void) {
	ui->genEvent("Each player is dealt two face down cards.");
	players[human]->createHand();
	for (int i=0; i<2; i++)
		players[human]->drawCard(gamedeck, true, true);
	ui->space();
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		if (i != human) {
			players[i]->createHand();
			players[i]->drawCard(gamedeck, false, false);
			players[i]->drawCard(gamedeck, false, false);
		}
		players[i]->drawCard(gamedeck, true, true);
	}
	ui->viewMenu(1);
	ui->space();
	bet();
	for (int i=3; i<6; i++) {
		ui->space();
		for (unsigned int j=0; j<players.size(); j++) {
			if (players[j]->getStatus() > 0) {
				players[j]->drawCard(gamedeck, true, true);
			}
		}
		ui->viewMenu(i-1);
		ui->space();
		bet();
	}
	ui->space();
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		if (players[i]->getStatus() > 0) {
			players[i]->drawCard(gamedeck, true, (i == human ? true : false));
		}
	}
	// for testing :)
	//players[human]->hand()->set(3);
	ui->viewMenu(5);
	ui->space();
	bet();
	for (int i=0; i<static_cast<signed>(players.size()); i++)
		if (players[i]->getStatus() > -1)
			players[i]->visHand();
}

void studFive(void) {
	ui->genEvent("Each player is dealt one card face down.");
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		players[i]->createHand();
		players[i]->drawCard(gamedeck, (i == human ? true : false),
			(i == human ? true : false));
	}
	for (int i=1; i<5; i++) {
		ui->space();
		for (unsigned int j=0; j<players.size(); j++) {
			if (players[j]->getStatus() > 0) {
				players[j]->drawCard(gamedeck, true, true);
			}
		}
		ui->viewMenu(i);
		ui->space();
		bet();
	}
	for (int i=0; i<static_cast<signed>(players.size()); i++)
		if (players[i]->getStatus() > -1)
			players[i]->visHand();
}

