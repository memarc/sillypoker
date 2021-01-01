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

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "poker.h"
#include "ui.h"
#include "stats.h"
#include "conf.h"

// globals
extern Deck* gamedeck;
extern vector<Player*> players;
extern int human;
extern Stats* gamestats;
extern Config* conf;
extern int game;
extern int curbet;
extern int betcap;
extern int btmp[5];
extern int lraise;
extern char buf[64];

int execgame(int);
void createopponents(void);
void draw(Deck*, Hand*, char*);
bool isnum(char*);
void enter(void);
void space(void);
void showabout(void);
void confmenu(void);

Interface::Interface() {
	cout << "Welcome to silly Poker v" << VERSION << "!" << endl;
}

Interface::~Interface() {
	cout << endl << "Thanks for playing!" << endl;
}

// Get a bet from the player.
int Interface::playerBet() {
	if (game == 1) {
		players[human]->printHand(0);
		pause();
	}
	while (true) {
		if (curbet > 0) {
			cout << endl << "You must call or raise (0 to fold)."
				 << " Current bet is $" << curbet << ". (max $" << betcap
				 << ")" << endl;
		} else {
			cout << endl << "How much are you betting (0 to fold)? (max $"
				 << betcap << ")" << endl;
		}
		cout << "[$" << players[human]->balance() << "]> ";
		fgets(buf, 64, stdin);
		if (isnum((char*)&buf)) {
			int b = atoi(buf);
			if (b == 0) {
				// this shouldn't be here...
//				players[human]->setStatus(0);
//				cout << endl << "You fold!" << endl;
				return b;
			} else if (b - btmp[human] > players[human]->balance()) {
				cout << "You don't have that much!" << endl;
			} else if (b < curbet) {
				cout << "You need to at least match the current bet!" << endl;
			} else if (b > curbet && (b < betcap && b - curbet < lraise)) {
				cout << "You must at least match the last raise to raise again." << endl;
			} else if (b > betcap) {
				cout << "Your bet exceeds the cap." << endl;
			} else {
				return b;
			}
		} else if (*buf == '\n') {
			players[human]->printHand(0);
		}
	}
}

// Discard the selected cards.
void Interface::playerDiscard() {
	players[human]->printHand(2);
	cout << "Which cards would you like to discard?" << endl
		 << "[$" << players[human]->balance() << "]> ";
	fgets(buf, 64, stdin);
	if (strlen(buf) > 1) {
		draw(gamedeck, players[human]->hand(), (char*)(&buf));
		players[human]->hand()->sort();
	} else {
		cout << endl << "You stand pat." << endl;
	}
	enter();
}


// Let the player view the hands in play, unless he has folded.
void Interface::viewMenu(int round) {
	if (game == 1 && round != 0) return;
	cout << endl;
	if (players[human]->getStatus() < 1) {
		cout << "You are no longer in the game." << endl;
		enter();
		return;
	}
	while (true) {
		cout << "View hands (enter a number): ";
		for (int i=0; i<static_cast<signed>(players.size()); i++) {
			if (players[i]->getStatus() > 0) {
				if (i == human) {
					cout << " " << i+1 << " (yours)";
				} else {
					cout << " " << i+1;
				}
			}
		}
		cout << endl << "0. Continue" << endl
			 << "[$" << players[human]->balance() << "]> ";
		fgets(buf, 64, stdin);
		int c = atoi(buf);
		if (c == 0) {
			return;
		} else if (c > 0 && c < static_cast<signed>(players.size())+1) {
			if (players[c-1]->getStatus() < 1 && game == 1 ) {
				cout << (c-1 == human ? "You are" : "That player is")
					 << " no longer in the current game." << endl;
			} else {
				players[c-1]->printHand(0);
			}
		}
	}
	cout << endl;
}

// return 0 for success, -1 for bankrupsy
int Interface::playMenu() {
	// If the number of players or account amount has been changed, we'll
	// recreate the virtual table
	while (true) {
		cout << endl << "You: $" << players[human]->balance() << endl;
		for (int i=0; i<static_cast<signed>(players.size()); i++) {
			if (i != human) {
				if (players[i]->getStatus() == -1) {
					cout << players[i]->name() << ": out ";
				} else {
					cout << players[i]->name() << ": $"
						 << players[i]->balance() << "  ";
				}
			}
		}
		cout << endl << "Choose one:" << endl
			 << "1. Play five card draw" << endl
			 << "2. Play five card stud" << endl
			 << "3. Play seven card stud" << endl
			 << "4. Back to main menu" << endl
			 << "[$" << players[human]->balance() << "]> ";
		fgets(buf, 64, stdin);
		if (!strncmp(buf, "1", 1) || !strncmp(buf, "2", 1) ||
			!strncmp(buf, "3", 1)) {
			if (gamedeck == NULL) {
				gamedeck = new Deck();
			}
			// execgame(). if it returns anything but 0, we're getting out
			// of here.
			if (execgame(atoi(buf))) return -1;
		} else if (!strncmp(buf, "4", 1)) {
			break;
		}
	}
	return 0;
}

void Interface::stats() {
	gamestats->print();
}

// The main game loop.
void Interface::main() {
	while (true) {
		cout << endl << "Choose one:" << endl
			 << "1. Begin playing" << endl
			 << "2. View statistics" << endl
			 << "3. Edit options" << endl
			 << "4. Exit" << endl
			 << "0. About" << endl
			 << "=> ";
		fgets(buf, 64, stdin);
		if (!strncmp(buf, "1", 1)) {
			// If we get anything but 0, quit the loop.
			if (playMenu()) return;
		} else if (!strncmp(buf, "2", 1)) {
			stats();
		} else if (!strncmp(buf, "3", 1)) {
			options();
		} else if (!strncmp(buf, "4", 1)) {
			break;
		} else if (!strncmp(buf, "0", 1)) {
			showabout();
		}
	}
}

// The options menu
void Interface::options(void) {
	bool optch = false;

	while (true) {
		cout << endl << "Options:" << endl
			 << "1. Number of players: " << conf->getPlayers() << endl
			 << "2. Starting account amount: " << conf->getAccount() << endl
			 << "3. Betting cap in draw poker: " << conf->getDrawBetcap()<<endl
			 << "4. Betting cap in stud poker: " << conf->getStudBetcap()<<endl
			 << "5. Back to main menu" << endl
			 << "=> ";
		fgets(buf, 64, stdin);
		if (isnum((char*)&buf)) {
			int c = atoi(buf);
			int n;
			switch (c) {
			case 1:
				cout << endl << "Number of players: ";
				fgets(buf, 64, stdin);
				n = atoi(buf);
				if (n < 6 && n > 0) {
					conf->setPlayers(n);
					optch = true;
				}
				break;
			case 2:
				// If we don't recompute this, the changed account value
				// will confuse things
				gamestats->computeWinnings();
				cout << endl << "Starting account amount: ";
				fgets(buf, 64, stdin);
				n = atoi(buf);
				if (n > 0) {
					conf->setAccount(n);
					optch = true;
				}
				break;
			case 3:
				cout << endl << "Betting cap in draw poker: ";
				fgets(buf, 64, stdin);
				n = atoi(buf);
				if (n > 0) conf->setDrawBetcap(n);
				break;
			case 4:
				cout << endl << "Betting cap in stud poker: ";
				fgets(buf, 64, stdin);
				n = atoi(buf);
				if (n > 0) conf->setStudBetcap(n);
				break;
			case 5:
				if (optch) {
					cout << endl << "*** You have changed player options, and your poker table"
						 << endl << "    is being regenerated." << endl;
					createopponents();
					optch = false;
					space();
					pause();
				}
				return;
			}
		}
	}

}

void Interface::genEvent(string str) {
	cout << str << endl;
}

void Interface::playerEvent(string str) {
	cout << str << endl;
}

void Interface::pause() {
	enter();
}

void Interface::space() {
	::space();
}

// Discard and draw new cards for hand from deck
void draw(Deck* deck, Hand* hand, char* str) {
	char* ptr = str;
	int n[5] = { 0 };
	cout << "Discarding.." << endl;
	for (; *ptr; ptr++) {
		if (*ptr > '0' && *ptr < '6') {
			char c = *ptr;
			int i = atoi(&c)-1;
			if (n[i] != 1) {
				cout << "\t" << i+1 << ". ";
				hand->card(i)->print();
				n[i] = 1;
				hand->drawNew(deck, i);
			}
		}
	}
	hand->sort();
	cout << "Drawing new.." << endl;
	int count=0;
	for (int i=0; i<5; i++) {
		if (n[i] == 1) {
			count++;
			cout << "\t";
			gamedeck->card(gamedeck->getPos()-count)->print();
		}
	}
}

