/* ncurses.cpp - curses interface
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

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <curses.h>
#include <panel.h>
#include <term.h>
using namespace std;

#include "ui.h"
#include "scrollwin.h"
#include "ncurses.h"
#include "conf.h"
#include "poker.h"
#include "stats.h"

#include "menurt.h"
#include "deskman.h"
#include "attrs.h"
#include "stdwinset.h"
#include "error.h"
#include "input.h"

extern Interface* ui;
extern vector<Player*> players;
extern Deck* gamedeck;
extern int human;
extern Config* conf;
extern Stats* gamestats;
extern int btmp[5];
extern int betcap;
extern int curbet;
extern int lraise;
extern char buf[64];

int execgame(int);
void createopponents(void);

void updateTitle(WINDOW* win, const char* str) {
	int x, y;
	getyx(win, y, x);
	wattrset(win, BOXATTR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattrset(win, STDATTR);
	mvwprintw(win, 0, 2, " %s ", str);
	wmove(win, y, x);
	wrefresh(win);
}

void CursesInterface::about(void) {
	WINDOW* win;
	PANEL* panel;
	int ch;

	win = newwin(18, 50, (LINES - 18) / 2, (COLS - 50) / 2);
	panel = new_panel(win);

	stdwinset(win);
	wtimeout(win, -1);
	wbkgd(win, BOXATTR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattrset(win, STDATTR);
	mvwprintw(win, 0, 2, " About ");
	mvwprintw(win, 2, 2, "silly Poker");
	mvwprintw(win, 3, 2, "A very silly poker game");
	mvwprintw(win, 4, 2, "Version %s", VERSION);
	mvwprintw(win, 6, 2, "Written by Kevin Radloff, with special thanks");
	mvwprintw(win, 7, 2, "to Gerard Paul Java for the pretty UI. :)");
	mvwprintw(win, 9, 2, "Copyright (C) 1999-2000  Kevin Radloff");
	mvwprintw(win, 11, 2, "This program is distributed under the terms of");
	mvwprintw(win, 12, 2, "the GNU General Public License Version 2 or");
	mvwprintw(win, 13, 2, "any later version. See the included COPYING");
	mvwprintw(win, 14, 2, "file for details.");

	wattrset(win, HIGHATTR);
	mvwprintw(win, 16, 2, ANYKEY_MSG);

	update_panels();
	doupdate();

	do {
		ch = wgetch(win);
		if (ch == 12)
			refresh_screen();
	} while (ch == 12);

	del_panel(panel);
	delwin(win);
	update_panels();
	doupdate();
}

CursesInterface::CursesInterface() {
	initscr();
	if (LINES < 24 || COLS < 80) {
		endwin();
		cout << "This program requires at least an 80x24 display." << endl;
		::exit(0);
	}
	start_color();
	standardcolors(conf->getColor());
	cbreak();
	noecho();
	nonl();
	curs_set(0);
//	intrflush(stdscr, FALSE);
//	keypad(stdscr, TRUE);
}

CursesInterface::~CursesInterface() {
	endwin();
}

void CursesInterface::main(void) {
    MENU menu;
    int endloop;
    int aborted;
	int row = 1;

    draw_desktop();
    attrset(STDATTR);
    move(0, 1);
    printw("silly Poker");
	move(0, COLS-2-strlen(VERSION));
	printw("v%s", VERSION);
    initmenu(&menu, "Main", 9, 20, (LINES - 9) /2, (COLS - 20) / 2);
    additem(&menu, " ^G^ames", "Get your poker face ready!");
	additem(&menu, " ^S^tatistics", "View the global game statistics");
	additem(&menu, " ^O^ptions", "Show the options menu");
	additem(&menu, NULL, NULL);
	additem(&menu, " ^A^bout", "About silly Poker");
	additem(&menu, NULL, NULL);
	additem(&menu, " E^x^it", "Exit silly Poker");
	endloop = 0;
	do {
		showmenu(&menu);
		operatemenu(&menu, &row, &aborted);
		switch (row) {
		case 1:
			if (playMenu()) endloop = 1;
			break;
		case 2:
			stats();
			break;
		case 3:
			options();
			break;
		case 5:
			about();
			break;
		case 7:
			endloop = 1;
			break;
        }
    } while (!endloop);
    destroymenu(&menu);
    erase();
    update_panels();
    doupdate();
}

void CursesInterface::stats() {
	int* stats = gamestats->getArray();
	WINDOW* win;
	PANEL* panel;
	int ch;
	int total = stats[0] + stats[1];

	win = newwin(12, 60, (LINES - 12) / 2, (COLS - 60) / 2);
	panel = new_panel(win);

	stdwinset(win);
	wtimeout(win, -1);
	wbkgd(win, BOXATTR);
	box(win, ACS_VLINE, ACS_HLINE);
	wattrset(win, STDATTR);
	mvwprintw(win, 0, 2, " User Statistics ");
	mvwprintw(win, 2, 2, "Games: %d  Wins: %d (%.2f%%)  Losses: %d (%.2f%%)",
		total,
		stats[0], (total > 0 ?
		static_cast<double>(stats[0])/static_cast<double>(total)*100 : 0),
		stats[1], (total > 0 ?
		static_cast<double>(stats[1])/static_cast<double>(total)*100 : 0));
	mvwprintw(win, 3, 2, "Games played this session: %d", stats[6]);
	mvwprintw(win, 4, 2, "Most in one sitting: %d", stats[7]);
	mvwprintw(win, 5, 2, "Longest winning streak: %d", stats[3]);
	mvwprintw(win, 6, 2, "Longest losing streak: %d", stats[5]);
	if (stats[2] > 1) {
		mvwprintw(win, 7, 2, "Current winning streak: %d", stats[2]);
	} else if (stats[4] > 1) {
		mvwprintw(win, 7, 2, "Current losing streak: %d", stats[4]);
	}
	mvwprintw(win, 8, 2, "Money won: $%d", stats[8] +
		players[human]->balance() - conf->getAccount());
	
	wattrset(win, HIGHATTR);
	mvwprintw(win, 10, 2, ANYKEY_MSG);

	update_panels();
	doupdate();

	do {
		ch = wgetch(win);
		if (ch == 12)
			refresh_screen();
	} while (ch == 12);

	del_panel(panel);
	delwin(win);
	update_panels();
	doupdate();
	delete stats;
}

int getInput(const char* desc, const char* comment, int from, int to, int len, int* aborted) {
	WINDOW* dlgwin;
	PANEL* dlgpanel;
	FIELDLIST field;
	int resp;
	int val;
	int endloop = 0;

	dlgwin = newwin(7, 40, (LINES - 7) / 2, (COLS - 40) / 2);
	dlgpanel = new_panel(dlgwin);

	wbkgd(dlgwin, BOXATTR);
	box(dlgwin, ACS_VLINE, ACS_HLINE);

	wattrset(dlgwin, STDATTR);
	mvwprintw(dlgwin, 0, 2, " %s ", desc);
	mvwprintw(dlgwin, 1, 2, "%s", comment);
	mvwprintw(dlgwin, 2, 2, "Enter value from %d-%d", from, to);
	wmove(dlgwin, 5, 2);
	stdkeyhelp(dlgwin);

	initfields(&field, 1, 10, (LINES - 7) / 2 + 3, (COLS - 40) / 2 + 2);
	addfield(&field, len, 0, 0, "");

	do {
		fillfields(&field, aborted);

		if (!(*aborted)) {
			val = atoi(field.list->buf);
			if (val > to || val < from)
				errbox("Error - please input a valid value", ANYKEY_MSG, &resp);
			else
				endloop = 1;
		}
	} while (!(*aborted) && endloop == 0);

	del_panel(dlgpanel);
	delwin(dlgwin);

	destroyfields(&field);
	update_panels();
	doupdate();

	return val;
}

void updateoptions(WINDOW* win) {
	werase(win);
	updateTitle(win, "Currently");
	wattrset(win, HIGHATTR);
	mvwprintw(win, 1, 2, "%d", conf->getPlayers());
	mvwprintw(win, 2, 2, "$%d", conf->getAccount());
	mvwprintw(win, 3, 2, "$%d", conf->getDrawBetcap());
	mvwprintw(win, 4, 2, "$%d", conf->getStudBetcap());
	mvwprintw(win, 5, 2, "%s", conf->getColor() == 1 ? "Yes" : "No ");
	mvwprintw(win, 6, 2, "%s", conf->getCardPic() == 1 ? "Yes" : "No ");
}

void CursesInterface::options() {
	MENU menu;
	WINDOW* optwin;
	PANEL* optpanel;
	int n;
	int aborted;
	int row = 1;
	int resp;
	bool optch = false;

	initmenu(&menu, "Options", 10, 37, (LINES - 10) / 2, (COLS - 37) / 3);
	additem(&menu, " ^N^umber of players", "The number of players (including the human) in the game");
	additem(&menu, " Starting ^a^ccount amount", "The amount in each player's account at the beginning of the game");
	additem(&menu, " Betting cap in ^d^raw poker", "The cap on bets in five card draw poker");
	additem(&menu, " Betting cap in ^s^tud poker", "The cap on bets in five and seven card stud");
	additem(&menu, " ^C^olor interface", "Toggles use of curses color (must restart to take effect)");
//	additem(&menu, " Show cards as ^p^ics", "Show a graphical representation of cards instead of the text");
	additem(&menu, " Show cards as ^p^ics", "(unimplemented)");
	additem(&menu, NULL, NULL);
	additem(&menu, " E^x^it options", "Return to main menu");
	
	optwin = newwin(8, 15, (LINES - 10) / 2, (COLS - 37) / 3 + 37);
	optpanel = new_panel(optwin);

	wbkgd(optwin, BOXATTR);
	updateoptions(optwin);

	do {
		showmenu(&menu);
		operatemenu(&menu, &row, &aborted);

		switch (row) {
		case 1:
			n = getInput("Players", "", 2, 5, 1, &aborted);
			if (!aborted) {
				conf->setPlayers(n);
				updateoptions(optwin);
				optch = true;
			}
			break;
		case 2:
			n = getInput("Starting accounts", "",
				max(conf->getDrawBetcap(), conf->getStudBetcap()),
				100000, 6, &aborted);
			if (!aborted) {
				gamestats->computeWinnings(); // Must be recalculated
				conf->setAccount(n);
				updateoptions(optwin);
				optch = true;
			}
			break;
		case 3:
			n = getInput("Draw bet cap", "", 1, 1000, 4, &aborted);
			if (!aborted) {
				conf->setDrawBetcap(n);
				updateoptions(optwin);
			}
			break;
		case 4:
			n = getInput("Stud bet cap", "", 1, 1000, 4, &aborted);
			if (!aborted) {
				conf->setStudBetcap(n);
				updateoptions(optwin);
			}
			break;
		case 5:
			conf->toggleColor();
			updateoptions(optwin);
			break;
		case 6:
			conf->toggleCardPic();
			updateoptions(optwin);
			break;
		}
	} while (row != 8);

	if (optch) {
		errbox("Player options have changed; the poker table will be regenerated.",
			ANYKEY_MSG, &resp);
		createopponents();
		optch = false;
	}

	destroymenu(&menu);
	del_panel(optpanel);
	delwin(optwin);
	update_panels();
	doupdate();
}

int CursesInterface::playMenu() {
	MENU menu;
	int aborted;
	int row = 1;
	int endloop = 0;
	
	initmenu(&menu, "Games", 7, 25, (LINES - 7) / 2, (COLS - 25) / 2);
	additem(&menu, " Five card ^d^raw", "Play five card draw poker");
	additem(&menu, " ^F^ive card stud", "Play five card stud poker");
	additem(&menu, " ^S^even card stud", "Play seven card stud poker");
	additem(&menu, NULL, NULL);
	additem(&menu, " E^x^it", "Return to main menu");

	if (gamedeck == NULL)
		gamedeck = new Deck();

	do {
		showmenu(&menu);
		operatemenu(&menu, &row, &aborted);

		switch (row) {
		case 1:
			initGameUI();
			if (execgame(1)) return -1;
			destroyGameUI();
			break;
		case 2:
			initGameUI();
			if (execgame(2)) return -1;
			destroyGameUI();
			break;
		case 3:
			initGameUI();
			if (execgame(3)) return -1;
			destroyGameUI();
			break;
		case 5:
			endloop = 1;
			break;
		}
	} while (endloop == 0);

	destroymenu(&menu);
	update_panels();
	doupdate();

	return 0;
}

void CursesInterface::initGameUI() {
	int numplayers = static_cast<signed>(players.size());
	int height = (LINES - 2) / 2 - 1;
	int verextra = LINES % 2 + 2;
	int width = COLS / (numplayers - (numplayers == 5 ? 1 : 0));
	int horextra = COLS % (numplayers - (numplayers == 5 ? 1 : 0));
	int ex = horextra;
	for (int i=0; i<numplayers; i++) {
		if (i != human) {
			int plWinYPos, plWinWidth;
			if (i < human) {
				// player is before human
				if (numplayers < 5)
					plWinYPos = (i+1) * width + ex - horextra;
				else
					plWinYPos = i * width + ex - horextra;
			} else {
				// player is after human
				if (numplayers < 5)
					plWinYPos = i * width + ex - horextra;
				else
					plWinYPos = (i-1) * width + ex - horextra;
			}
			if (horextra > 0) {
				// need to add extra columns
				plWinWidth = width + 1;
				horextra--;
			} else {
				plWinWidth = width;
			}
			plwin[i] = newwin(height, plWinWidth, 1, plWinYPos);
		} else {
			int plWinHeight, plWinWidth, plWinYPos;
			if (numplayers == 5) {
				plWinHeight = height + verextra;
				plWinWidth = 20;
				plWinYPos = height + 1;
			} else {
				plWinHeight = height;
				plWinWidth = width;
				plWinYPos = 1;
			}
			plwin[i] = newwin(plWinHeight, plWinWidth, plWinYPos, 0);
		}
		plpanel[i] = new_panel(plwin[i]);
		stdwinset(plwin[i]);
		wbkgd(plwin[i], BOXATTR);
		box(plwin[i], ACS_VLINE, ACS_HLINE);
		wattrset(plwin[i], STDATTR);
		if (i != human)
			mvwprintw(plwin[i], 0, 2, " %s's hand ",
				players[i]->name());
		wmove(plwin[i], 1, 0);
	}

	hwin = plwin[human];
	hpanel = plpanel[human];
//	hwin = newwin(height, 20, height+1, 0);
//	plwin[human] = hwin;
//	hpanel = new_panel(hwin);
//	plpanel[human] = hpanel;
//	stdwinset(hwin);
//	wbkgd(hwin, BOXATTR);
//	box(hwin, ACS_VLINE, ACS_HLINE);
//	wattrset(hwin, STDATTR);
	mvwprintw(hwin, 0, 2, " Your cards ");

	awin = newwin(height+verextra, 15, height+1, (numplayers == 5 ? 20 : 0));
	apanel = new_panel(awin);
	stdwinset(awin);
	wbkgd(awin, BOXATTR);
	box(awin, ACS_VLINE, ACS_HLINE);
	wattrset(awin, STDATTR);
	mvwprintw(awin, 0, 2, " Accounts ");
	wmove(awin, 1, 0);

	int msgsize[] = { height+verextra, 0, height+1, 0 };
	if (numplayers == 5) {
		msgsize[1] = COLS - 35;
		msgsize[3] = 35;
	} else {
		msgsize[1] = COLS - 15;
		msgsize[3] = 15;
	}
	mainmsgwin = newwin(msgsize[0], msgsize[1], msgsize[2], msgsize[3]);
	mainmsgpanel = new_panel(mainmsgwin);
	stdwinset(mainmsgwin);
	wbkgd(mainmsgwin, BOXATTR);
	box(mainmsgwin, ACS_VLINE, ACS_HLINE);
	wattrset(mainmsgwin, STDATTR);
	mvwprintw(mainmsgwin, 0, 2, " Messages ");
	msgwin = newwin(msgsize[0]-2, msgsize[1]-2, msgsize[2]+1, msgsize[3]+1);
	msgpanel = new_panel(msgwin);
	wbkgd(msgwin, BOXATTR);
	wattrset(msgwin, STDATTR);
	msgbuf = new ScrollWin(msgwin, msgsize[0]-2, msgsize[1]-2);

	update_panels();
	doupdate();
	scrollok(msgwin, 1);
	keypad(msgwin, 1);
	wmove(msgwin, 0, 0);
}

void CursesInterface::destroyGameUI() {
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		del_panel(plpanel[i]);
		delwin(plwin[i]);
	}
//	del_panel(hpanel);
//	delwin(hwin);
	del_panel(apanel);
	delwin(awin);
	del_panel(msgpanel);
	delwin(msgwin);
	del_panel(mainmsgpanel);
	delwin(mainmsgwin);
	delete msgbuf;

	update_panels();
	doupdate();
}

void CursesInterface::genEvent(string str) {
	msgbuf->push(str);
}

void CursesInterface::playerEvent(string str) {
	msgbuf->push(str);
}

void CursesInterface::pause() {
	int ch;
	int aborted = 0;

	updateAccountWin();
	move(LINES - 1, 1);
	printkeyhelp("(Pg)Up/(Pg)Down", "-scroll messages  ", stdscr);
	printkeyhelp("Enter", "-continue", stdscr);
	coloreol();
	wrefresh(stdscr);
	do {
		ch = wgetch(msgwin);

		switch (ch) {
		case 'k':
		case KEY_UP:
			msgbuf->scrollUp();
			break;
		case 'p':
		case KEY_PPAGE:
			msgbuf->scrollPgUp();
			break;
		case 'j':
		case KEY_DOWN:
			msgbuf->scrollDown();
			break;
		case 'n':
		case KEY_NPAGE:
			msgbuf->scrollPgDown();
			break;
		case 12:
			refresh_screen();
			break;
		case 13:
			aborted = 1;
			break;
		}
	} while (aborted == 0);
}

void CursesInterface::space() {
	genEvent("");
}

void CursesInterface::viewMenu(int round) {
	// ³
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		werase(plwin[i]);
		wmove(plwin[i], 1, 0);
		vector<string> v = players[i]->handToStringVector();
		for (int j=0; j<static_cast<signed>(v.size()); j++) {
			wprintw(plwin[i], " %s\n", v[j].c_str());
		}
		if (i == human) {
			updateTitle(plwin[i], "Your hand");
		} else {
			sprintf(buf, "%s's hand", players[i]->name());
			updateTitle(plwin[i], buf);
		}
	}
	if (human != 0 || round == 0) pause(); // Hack!
}

int CursesInterface::playerBet() {
	int n, aborted, resp;

	pause();
	sprintf(buf, "Current bet: $%d", curbet);
	do {
		n = getInput("Your bet", buf, 0, betcap, 4, &aborted);
		if (n < curbet && n != 0) {
			errbox("You must at least match the current bet.", ANYKEY_MSG,
				&resp);
			aborted = 1;
		} else if (n > curbet && n < curbet + lraise && n != betcap) {
			errbox("You must match the last raise.", ANYKEY_MSG, &resp);
			aborted = 1;
		} else if (n - btmp[human] > players[human]->balance()) {
			errbox("You don't have that much!", ANYKEY_MSG, &resp);
			aborted = 1;
		}
	} while (aborted);

	return n;
}

void draw(Deck* deck, Hand* hand, bool* disc) {
	vector<string> discarded;
	vector<string> drawn;

	for (int i=0; i<5; i++) {
		if (disc[i]) {
			discarded.push_back(hand->card(i)->toString());
			hand->drawNew(deck, i);
			drawn.push_back(hand->card(i)->toString());
		}
	}
	if (static_cast<signed>(discarded.size()) > 0) {
		string out1("You discard ");
		string out2("You draw ");
		if (static_cast<signed>(discarded.size()) > 1) {
			for (int i=0; i<static_cast<signed>(discarded.size()); i++) {
				if (i == static_cast<signed>(discarded.size()) - 1) {
					out1 += "and the " + discarded[i] + ".";
					out2 += "and the " + drawn[i] + ".";
				} else {
					out1 += "the " + discarded[i] + ", ";
					out2 += "the " + drawn[i] + ", ";
				}
			}
		} else {
			out1 += discarded[0] + ", ";
			out2 += drawn[0] + ".";
		}
		ui->playerEvent(out1);
		ui->playerEvent(out2);
	} else {
		ui->playerEvent("You stand pat.");
	}
}

void CursesInterface::playerDiscard() {
	pause();

	MENU menu;
	WINDOW* cardwin;
	PANEL* cardpanel;
	vector<string> v = players[human]->handToStringVector();
	vector<char> c;
	bool disc[5];
	int aborted;
	int row = 1;
	
	updateAccountWin();
	initmenu(&menu, "Discard", 9, 21, (LINES - 9) / 2, (COLS - 21) / 3);
	for (int i=0; i<5; i++) {
		int pos = 0;
		for (int j=0; j<i; j++)
			while (v[i].at(pos) == c[j])
				pos++;
		c.push_back(v[i].at(pos));
		v[i].insert(pos+1, "^");
		v[i].insert(pos, "^");
		v[i].insert(0, " ");
		additem(&menu, const_cast<char*>(v[i].c_str()), "Toggle discarding of this card");
	}
	additem(&menu, NULL, NULL);
	additem(&menu, " ^D^one", "Exit the discard menu");

	cardwin = newwin(7, 5, (LINES - 9) / 2, (COLS - 21) / 3 + 21);
	cardpanel = new_panel(cardwin);

	wbkgd(cardwin, BOXATTR);
	box(cardwin, ACS_VLINE, ACS_HLINE);
	wattrset(cardwin, STDATTR);

	for (int i=0; i<5; i++) {
		disc[i] = false;
		mvwprintw(cardwin, i+1, 1, "%s", "No ");
	}

	do {
		showmenu(&menu);
		operatemenu(&menu, &row, &aborted);

		switch (row) {
		case 1:
			disc[0] = (disc[0] ? false : true);
			mvwprintw(cardwin, 1, 1, "%s", (disc[0] ? "Yes" : "No "));
			break;
		case 2:
			disc[1] = (disc[1] ? false : true);
			mvwprintw(cardwin, 2, 1, "%s", (disc[1] ? "Yes" : "No "));
			break;
		case 3:
			disc[2] = (disc[2] ? false : true);
			mvwprintw(cardwin, 3, 1, "%s", (disc[2] ? "Yes" : "No "));
			break;
		case 4:
			disc[3] = (disc[3] ? false : true);
			mvwprintw(cardwin, 4, 1, "%s", (disc[3] ? "Yes" : "No "));
			break;
		case 5:
			disc[4] = (disc[4] ? false : true);
			mvwprintw(cardwin, 5, 1, "%s", (disc[4] ? "Yes" : "No "));
			break;
		}
	} while (row != 7);

	destroymenu(&menu);
	del_panel(cardpanel);
	delwin(cardwin);
	update_panels();
	doupdate();

	draw(gamedeck, players[human]->hand(), disc);
}

void CursesInterface::updateAccountWin() {
	werase(awin);
	wmove(awin, 1, 0);
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		wprintw(awin, " %s\n", (i == human ? "You" :
			players[i]->name()));
		wprintw(awin, "  $%d\n", players[i]->balance());
	}
	updateTitle(awin, "Accounts");
}

#endif

