/* stats.cpp - statistics class
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
#include <string>
#include <iomanip>
#include <vector>
using namespace std;

#include "poker.h"
#include "conf.h"
#include "stats.h"

// globals
extern vector<Player*> players;
extern Stats* gamestats;
extern Config* conf;
extern int human;

void enter(void);

Stats::Stats() {
	wins = losses = winStreak = highestWinStreak = lossStreak =
		highestLossStreak = moneyWon = mostGamesPlayed = 0;
}

void Stats::init() {
	gamesPlayed = 0;
}

void Stats::addWin() {
	wins++;
	if (lossStreak > 0) {
		lossStreak = 0;
	}
	winStreak++;
	gamesPlayed++;

	if (winStreak > highestWinStreak) highestWinStreak++;
	if (gamesPlayed > mostGamesPlayed) mostGamesPlayed++;
}	

void Stats::addLoss() {
	losses++;
	if (winStreak > 0) {
		winStreak = 0;
	}
	lossStreak++;
	gamesPlayed++;

	if (lossStreak > highestLossStreak) highestLossStreak++;
	if (gamesPlayed > mostGamesPlayed) mostGamesPlayed++;
}

void Stats::computeWinnings() {
	moneyWon += players[human]->balance() - conf->getAccount();
}

void Stats::print() {
	int total = wins + losses;
	cout << endl << "User game statistics:" << endl
		 << setprecision(4) << "Total games: " << wins+losses
		 << "  Wins: " << wins << " ("
		 << (total > 0 ?
		 	 static_cast<double>(wins)/static_cast<double>(total)*100 : 0)
		 << "%)  Losses: " << losses << " ("
		 << (total > 0 ?
		 	 static_cast<double>(losses)/static_cast<double>(total)*100 : 0)
		 << "%)" << endl
		 << "Games played this session: " << gamesPlayed
		 << "  Most in one sitting: " << mostGamesPlayed << endl
		 << "Longest winning streak: " << highestWinStreak << " games" << endl
		 << "Longest losing streak: " << highestLossStreak << " games" << endl;
	if (winStreak > 1) {
		cout << "Current winning streak: " << winStreak << " games" << endl;
	} else if (lossStreak > 1) {
		cout << "Current losing streak: " << lossStreak << " games" << endl;
	}
	cout << "Money won: $"
		 << moneyWon + players[human]->balance() - conf->getAccount()
		 << endl << endl;
	enter();
}

int* Stats::getArray() {
	int* res = new int[9];
	res[0] = wins;
	res[1] = losses;
	res[2] = winStreak;
	res[3] = highestWinStreak;
	res[4] = lossStreak;
	res[5] = highestLossStreak;
	res[6] = gamesPlayed;
	res[7] = mostGamesPlayed;
	res[8] = moneyWon;
	return res;
}

void savestats(void) {
	char path[1024];
	FILE* statsfile;
	sprintf(path, "%s/.sillypokerstats", getenv("HOME"));
	if ((statsfile = fopen(path, "w"))) {
		fwrite(gamestats, sizeof(Stats), 1, statsfile);
	} else {
		cerr << "Error saving user stats." << endl;
	}
}

void loadstats(void) {
	char path[1024];
	FILE* statsfile;
	sprintf(path, "%s/.sillypokerstats", getenv("HOME"));
	if ((statsfile = fopen(path, "r"))) {
		gamestats = (Stats*)malloc(sizeof(Stats));
		fread(gamestats, sizeof(Stats), 1, statsfile);
		gamestats->init();
	} else {
		gamestats = new Stats();
	}
}

