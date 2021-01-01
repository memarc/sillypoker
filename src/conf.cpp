/* conf.cpp - configuration settings class
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
using namespace std;

#include "conf.h"
#include "stats.h"

// globals
extern Config* conf;
extern bool optch;
extern char buf[64];
extern char buf2[64];
extern Stats* gamestats;

bool isnum(char*);

Config::Config() {
	players = 4;
	account = 1000;
	drawBetcap = 10;
	studBetcap = 8;
#ifdef HAVE_NCURSES
	color = 1;
	cardpic = 0;
	interface = 1;
#else
	interface = 0;
#endif
}

void Config::open(FILE* rc) {
	while (fscanf(rc, "%s = %s", buf, buf2) != EOF) {
		if (!strcmp(buf, "players")) {
			players = atoi(buf2);
		} else if (!strcmp(buf, "account")) {
			account = atoi(buf2);
		} else if (!strcmp(buf, "drawbetcap")) {
			drawBetcap = atoi(buf2);
		} else if (!strcmp(buf, "studbetcap")) {
			studBetcap = atoi(buf2);
#ifdef HAVE_NCURSES
		} else if (!strcmp(buf, "color")) {
			color = atoi(buf2);
		} else if (!strcmp(buf, "cardaspic")) {
			cardpic = atoi(buf2);
#endif
		} else if (!strcmp(buf, "interface")) {
			if (!strcmp(buf2, "text")) {
				interface = 0;
			} else if (!strcmp(buf2, "curses")) {
				interface = 1;
			} else if (!strcmp(buf2, "qt")) {
				interface = 2;
			}
		}
	}
}

void Config::save() {
	char path[1024];
	FILE* outFile;
	sprintf(path, "%s/.sillypokerrc", getenv("HOME"));
	if ((outFile = fopen(path, "w"))) {
		fprintf(outFile, "players = %d\n", players);
		fprintf(outFile, "account = %d\n", account);
		fprintf(outFile, "drawbetcap = %d\n", drawBetcap);
		fprintf(outFile, "studbetcap = %d\n", studBetcap);
#ifdef HAVE_NCURSES
		fprintf(outFile, "color = %d\n", color);
		fprintf(outFile, "cardaspic = %d\n", cardpic);
#endif
		fprintf(outFile, "interface = %s\n",
			(interface == 2 ? "qt" : (interface == 1 ? "curses" : "text")));
		fprintf(outFile, "\n");
		fclose(outFile);
	} else {
		cerr << "Error writing rc file." << endl;
	}
}

// Create a new Config object, then try to load options from the rc file
void loadconfig(void) {
	char path[1024];
	FILE* inFile;

	conf = new Config();
	sprintf(path, "%s/.sillypokerrc", getenv("HOME"));
	if (!(inFile = fopen(path, "r"))) {
		return;
	} else {
		conf->open(inFile);
	}
}

