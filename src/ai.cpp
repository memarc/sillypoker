/* ai.cpp - AI class
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

#include <cstdio>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

#include "poker.h"
#include "ui.h"

// globals
extern Interface* ui;
extern Deck* gamedeck;
extern vector<Player*> players;
extern int btmp[5];
extern int curbet;
extern int pot;
extern int betcap;
extern int game;
extern int lraise;
extern char buf[64];

int randInt(int, int);

AI::AI(int i) : Player(i) {
}

AI::~AI() {
}

void AI::printHand(int n) {
	cout << endl << "\t" << name() << "'s hand:" << endl;
	h->print(n);
}

void AI::createHand(Deck* deck, int a) {
	discarded = false;
	h = new Hand(deck, a);
}

void AI::drawCard(Deck* deck, bool pr, bool vis) {
	h->drawNew(deck, vis);
	if (pr && vis) {
		sprintf(buf, "%s is dealt the %s.", name(),
			h->card(h->getNum()-1)->toString().c_str());
		ui->playerEvent(buf);
	} else if (pr) {
		sprintf(buf, "%s is dealt a face down card.", name());
		ui->playerEvent(buf);
	}
}

void AI::updateHist() {
	for (int i=0; i<15; i++)
		rankHist[i] = 0;
	for (int i=0; i<4; i++)
		suitHist[i] = 0;
	for (int i=0; i<h->getNum(); i++) {
		rankHist[h->getRank(i)]++;
		suitHist[h->getSuit(i)]++;
	}
}

int AI::bet() {
	updateHist();
	int ingame = 0;
	int b = 0;
	int sc = h->score();

	// see how many players are still in the game
	for (int i=0; i<static_cast<signed>(players.size()); i++)
		if (players[i]->getStatus() == 1)
			ingame++;

	switch (game) {
	case 1:
		if (ingame == 0) {
			b = (curbet == 0 ? 2 : curbet);
		} else if (discarded && sc < 20 && ingame > 1) {
			b = 0;
		} else if (curbet == betcap) {
			b = betcap;
		} else if (curbet == 0) {
			b = 2;
		} else if (sc > 40) {
			b = curbet + (lraise > 0 ? lraise : 2);
		} else {
			switch (randInt(0, 10)) {
			case 0:
			case 1:
				b = curbet + (lraise > 0 ? lraise : 2);
			default:
				b = curbet;
				break;
			}
		}
		break;
	case 2:
		if (sc > 40) {
			b = curbet + (lraise > 0 ? lraise : 2);
		} else if (h->getNum() > 4 && sc < 40 && ingame > 2) {
			switch (randInt(0, 10)) {
			case 0:
			case 1:
				b = curbet + (lraise > 0 ? lraise : 2);
				break;
			default:
				b = 0;
				break;
			}
		} else if (h->getNum() > 3) {
			switch (randInt(0, 10)) {
			case 0:
			case 1:
				b = curbet + (lraise > 0 ? lraise : 2);
				break;
			default:
				b = (curbet == 0 ? 2 : curbet);
				break;
			}
		} else {
			b = (curbet == 0 ? 2 : curbet);
		}
		break;
	case 3:
		if (sc > 60) {
			b = curbet + (lraise > 0 ? lraise : 2);
		} else if (h->getNum() > 5 && sc < 40 && ingame > 2) {
			switch (randInt(0, 10)) {
			case 0:
			case 1:
				b = curbet + (lraise > 0 ? lraise : 2);
				break;
			default:
				b = 0;
				break;
			}
		} else if (h->getNum() > 3) {
			switch (randInt(0, 10)) {
			case 0:
			case 1:
				b = curbet + (lraise > 0 ? lraise : 2);
				break;
			default:
				b = (curbet == 0 ? 2 : curbet);
				break;
			}
		} else {
			b = (curbet == 0 ? 2 : curbet);
		}
		break;
	}
	if (b > betcap) {
		b = betcap;
	}
	
	if (b < curbet && b != 0) {
		cerr << "AI bet error. Bet less than curbet: "
			 << b << " < " << curbet
			 << endl;
	}

	return b;
}

void AI::discard() {
	updateHist();
	int n = 0;
	discarded = true;

	h->sort();
	for (int i=0; i<5; i++) {
		if (n > 2) break;
		if (rankHist[h->getRank(i)] == 1) {
			h->drawNew(gamedeck, i);
			n++;
		}
	}
	if (n > 0) {
		sprintf(buf, "%s discards and draws %d.", name(), n);
	} else {
		sprintf(buf, "%s stands pat.", name());
	}
	ui->playerEvent(buf);
	h->sort();
}

const char* AI::name() {
	const char* names[] = {
			"Kade", // Names I created playing Final Fantasy Legend I on my
			"Xexo", // Game Boy :)
			"Lash",
			"Jalo",
			"Jade", // What my sister dubbed the girl in Secret of Mana
			"Neep", // What my sister dubbed the sprite in Secret of Mana
			"Bob"   // The almighty master of the universe. Didn't you know?
			};
	return names[id];
}

string AI::betEvent(string str, int n) {
	if (str == "bet") {
		sprintf(buf, "%s bets $%d.", name(), n);
		return buf;
	} else if (str == "raise") {
		sprintf(buf, "%s raises with $%d.", name(), n);
		return buf;
	} else if (str == "call") {
		sprintf(buf, "%s calls.", name());
		return buf;
	} else if (str == "fold") {
		sprintf(buf, "%s folds.", name());
		return buf;
	} else if (str == "bankrupt") {
		sprintf(buf, "%s has gone bankrupt.", name());
		return buf;
	}
}

