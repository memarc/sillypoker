/* main.cpp - main
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
#include "conf.h"
#include "stats.h"
#include "ui.h"
#include "qtui.h"    // this _must_ be before ncurses.h
#include "ncurses.h"

// Ugh, conflicting macros.
#ifdef erase
#undef erase
#endif

#define NUM_AIS 7

// globals
Interface* ui;					// the UI
Deck* gamedeck;					// the game deck
char buf[64];					// temp buffers
char buf1[64];
char buf2[64];
vector<Player*> players;		// vector of players - the virtual table
int human;						// the number of the human player
int btmp[5];					// keep track of the previous bet
int curbet;						// the current bet
int highbet;					// the index of the highest better
int betcap;						// the betting cap
int lraise;						// the previous raise
int pot;						// the pot
int game;						// the game type
Config* conf;					// the configuration
Stats* gamestats;				// the stats

// from util.cpp
int randInt(int, int);
void seedrnd(void);
void space(void);
void enter(void);
void parsecmdline(int, char**);
// from score.cpp
int scorehands(void);
char* scoreString(int);
// from conf.cpp
void loadconfig(void);
// from stats.cpp
void loadstats(void);
void savestats(void);
// from games.cpp
void drawFive(void);
void studFive(void);
void studSeven(void);
// local
void createopponents(void);

// Keep looping and getting bets until everyone calls the highest better.
void bet(void) {
	curbet = lraise = 0;
	highbet = -1;
	for (int i=0; i<static_cast<signed>(players.size()); i++)
		btmp[i] = 0;
	while (true) {
		for (int i=0; i<static_cast<signed>(players.size()); i++) {
			// we've gone around the table
			if (highbet == i) return;
			// XXX: if all players fold, this loops infinitely
			// this shouldn't ever happen, though
			if (players[i]->getStatus() < 1) continue;
			// bankrupsy
			if (players[i]->balance() < curbet - btmp[i]) {
				ui->playerEvent(players[i]->betEvent("bankrupt", 0));
				players[i]->setStatus(-1);
				if (i == human) {
					ui->pause();
				}
				continue;
			}
			int b = players[i]->bet();
			// the player folds
			if (b == 0) {
				players[i]->setStatus(0);
				ui->playerEvent(players[i]->betEvent("fold", b));
				continue;
			// the player raises
			} else if (b > curbet) {
				highbet = i;
				lraise = b - curbet;
				if (curbet == 0)
					ui->playerEvent(players[i]->betEvent("bet", b));
				else
					ui->playerEvent(players[i]->betEvent("raise", b));
			// the player calls
			} else if (b == curbet) {
				ui->playerEvent(players[i]->betEvent("call", b));
			}
			curbet = b;
			players[i]->withdraw(b - btmp[i]);
			pot += b - btmp[i];
			btmp[i] = b;
		}
	}
}

// Let all our players discard.
void discard(void) {
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		// skip players not in the game
		if (players[i]->getStatus() < 1) continue;
		players[i]->discard();
	}
}

// The gameplay.
// return 0 for success, -1 means player bankrupsy.
int execgame(int x) {
	pot = 0;
	ui->genEvent("Shuffling deck!");
	ui->space();
	gamedeck->shuffle();

	switch (x) {
	case 1:
		game = 1;
		betcap = conf->getDrawBetcap();
		drawFive();
		break;
	case 2:
		game = 2;
		betcap = conf->getStudBetcap();
		studFive();
		break;
	case 3:
		game = 3;
		betcap = conf->getStudBetcap();
		studSeven();
		break;
	}

	ui->space();
	scorehands();
	// Let the player see the winning hands, if he's still in the pot
	// Scratch that, she can see no matter what.
//	if (players[human]->getStatus() > 0) {
		ui->viewMenu(0);
//	} else {
//		ui->pause();
//	}

	// If the last game bankrupted the player, we're going to exit
	if (players[human]->getStatus() == -1 || players[human]->balance() == 0) {
		ui->genEvent("You're bankrupt! Back to work for you.");
		// return -1, so we fall back to main().
		return -1;
	}
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		if (players[i]->getStatus() == 0) {
			players[i]->setStatus(1);
		}
		if (players[i]->getStatus() == 1)
			players[i]->deleteHand();
	}
	return 0;
}

// Sit the players at our virtual table. players[0] is who sits to the
// left of the dealer.
void createopponents(void) {
	// If we're running this a second time, delete the old first
	if (players.size() > 0) {
		players.erase(players.begin(), players.end());
	}
	int numplayers = conf->getPlayers();
	int num = randInt(0, numplayers-1);
	human = num;
	int cpu[5];
	for (int i=0; i<numplayers; i++) {
		if (i == human) {
			players.push_back(new Player(-1));
		} else {
			// eww, major hack for getting unique ids
			while (true) {
				bool b = false;
				int n = randInt(0, NUM_AIS-1);
				for (int j=0; j<5; j++)
					if (n == cpu[j])
						b = true;
				if (b) continue;
				players.push_back(new AI(n));
				cpu[i] = n;
				break;
			}
		}
	}
}

int main(int argc, char** argv) {
	parsecmdline(argc, argv);

	try {
		// Load statistics and options, etc.
		loadstats();
		loadconfig();
		seedrnd();
		createopponents();
		// create a UI
		ui = (conf->getInterface() == 0 ? new Interface() :
// UGH! What a _mess_! :)
#ifdef HAVE_NCURSES
			 (conf->getInterface() == 1 ? new CursesInterface() :
#endif
#ifdef HAVE_QT
			 (conf->getInterface() == 2 ? new QtInterface(argc, argv) :
#endif
			  new Interface())
#ifdef HAVE_NCURSES
			  )
#endif
#ifdef HAVE_QT
			  )
#endif
			  ;
		// Call the game loop
		//mainmenu();
		ui->main();
		// kill the interface
		delete ui;
		// Save stats, options
		gamestats->computeWinnings();
		conf->save();
		savestats();
	}
	catch (const exception &error) {
		delete ui;
		cerr << error.what() << endl;
	}
}

