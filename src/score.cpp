/* score.cpp - hand scoring functions
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
#include <vector>
#include <algorithm>
using namespace std;

#include "poker.h"
#include "stats.h"
#include "ui.h"

// globals
extern Interface* ui;
extern char buf[64];
extern char buf2[64];
extern vector<Player*> players;
extern int pot;
extern int human;
extern Stats* gamestats;

int Hand::score() {
	int score = 0;
	int rankHist[15] = { 0 };
	int numCards = static_cast<signed>(cards.size());
	int count = 0;

	for (int i=0; i<numCards; i++) {
		int rank = getRank(i);
		rankHist[rank]++;
	}
/*
 * 0. two of a kind
 * 1. 2 2 of a kind
 * 2. 3 of a kind
 * 3. 4 of a kind
 * 4. straight
 * 5. flush
 * 6. full house
 */
	// Hrm.. 7 possibilities, and keep track of the rank
	int card[7][2] = { { 0 }, { 0 } };
	// Check for multiples
	for (int i=2; i<15; i++) {
		switch (rankHist[i]) {
		case 0:
		case 1:
			break;
		case 2:
			if (i > card[0][1]) {
				if (card[0][1] > card[1][1]) {
					card[1][0] = 1;
					card[1][1] = card[0][1];
				}
				card[0][0] = 1;
				card[0][1] = i;
			} else if (i > card[1][1]) {
				card[1][0] = 1;
				card[1][1] = i;
			}
			break;
		case 3:
			if (i > card[2][1]) {
				card[2][0] = 1;
				card[2][1] = i;
			}
			break;
		case 4:
			card[3][0] = 1;
			card[3][1] = i;
			break;
		default:
			cout << "Something went real fucking wrong. Oops. "
				 << rankHist[i] << endl;
			break;
		}
		// Test for a straight.
		if (i == 2 && rankHist[14] > 0) count = 1;// aces can be low
		if (rankHist[i] > 0) {
			count++;
			if (count == 5) {
				card[4][0] = 1;
			}
		} else {
			count = 0;
		}
	}

	// This seems so lost floating in the middle here. :)
	card[5][1] = isFlush();
	if (card[5][1] >= 0) card[5][0] = 1;

	if (card[4][0] == 1 || card[5][0] == 1) {
		card[4][1] = getRank(3); // FIXME: this is 3, until we improve the alg
		// FIXME: ugh... but.. it works..
		if (card[4][0] == 1 && card[5][0] == 1) {
			if (numCards > 5) {
				Hand tmp;
				// Do it top down, to grab the highest ranks
				for (int i=numCards-1; i>=0; i--) {
					if (getSuit(i) == card[5][1]) {
						tmp.cards.push_back(cards[i]);
						tmp.vis.push_back(true);
					}
					if (static_cast<signed>(tmp.cards.size()) == 5) break;
				}
				if (tmp.isStraight()) {
					int add;
					// Ace could be the low or high card, let's check
					if (tmp.cards[0]->getRank() == 14 &&
						tmp.cards[1]->getRank() != 13) {
						add = tmp.cards[1]->getRank();
					} else {
						add = tmp.cards[2]->getRank();
					}
					score = add + 150;// straight flush
				} else {
					score = card[5][1] + 100;// just a flush
				}
			} else {
				score = card[4][1] + 150;
			}
		} else if (card[4][0] == 1) {
			score = card[4][1] + 80;
		} else if (card[5][0] == 1) {
			score = card[5][1] + 100;
		}
	// Full house goes first; even in 7 card stud, the only additional scoring
	// cards would be a pair.
	} else if (card[0][0] == 1 && card[2][0] == 1) {
		score = card[2][1] + 110;
	} else if (card[0][0] == 1 && card[1][0] == 1) {
		score = max(card[0][1], card[1][1]) + 40;
	} else if (card[3][0] == 1) {
		score = card[3][1] + 130;
	} else if (card[2][0] == 1) {
		score = card[2][1] + 60;
	} else if (card[0][0] == 1) {
		score = card[0][1] + 20;
	} else {
		score = cards[numCards-1]->getRank();
	}
	return score;
}

void Hand::set(int n) {
	switch (n) {
	case 1:
		break;
	case 2:
		break;
	case 3:
		cards[0] = new Card(4, 3);
		cards[1] = new Card(14, 3);
		cards[2] = new Card(8, 2);
		cards[3] = new Card(2, 1);
		cards[4] = new Card(3, 1);
		cards[5] = new Card(4, 1);
		cards[6] = new Card(5, 1);
		break;
	}
}

/*
 * 2 of a kind: value + 20
 * 22 -> 34
 * 2 2 of a kind: higher value + 40
 * 43 -> 54
 * 3 of a kind: value + 60
 * 62 -> 74
 * straight: middle value + 80
 * 82 -> 94
 * flush: suit + 100
 * 100 -> 103
 * full house: 3 of a kind value + 110
 * 112 -> 124
 * 4 of a kind: value + 130
 * 132 -> 144
 * straight flush: high value + 150
 * 155 -> 164
 * 
 */

const string scoreString(int s) {
	string str;
	if (s < 20) {
		str = "the high card";
	} else if (s >= 20 && s < 40) {
		str = "two of a kind";
	} else if (s >= 40 && s < 60) {
		str = "two pair";
	} else if (s >= 60 && s < 80) {
		str = "three of a kind";
	} else if (s >= 80 && s < 100) {
		str = "a straight";
	} else if (s >= 100 && s < 110) {
		str = "a flush";
	} else if (s >= 110 && s < 130) {
		str = "a full house";
	} else if (s >= 130 && s < 150) {
		str = "four of a kind";
	} else if (s >= 150) {
		str = "a straight flush";
	}
	return str;
}

bool Hand::isStraight() {
	if (static_cast<signed>(cards.size()) < 5) return false;
	int rankHist[15] = { 0 };
	int count = 0;
	for (int i=0; i<static_cast<signed>(cards.size()); i++) {
		rankHist[getSuit(i)]++;
	}
	for (int i=2; i<15; i++) {
		if (i == 2 && rankHist[14] > 0) count = 1;
		if (rankHist[i] > 0) {
			count++;
			if (count == 5) return true;
		} else {
			count = 0;
		}
	}
	return false;
}

int Hand::isFlush() {
	int suitHist[4] = { 0 };
	for (int i=0; i<static_cast<signed>(cards.size()); i++) {
		suitHist[getSuit(i)]++;
	}
	for (int i=0; i<4; i++) {
		if (suitHist[i] >= 5) return i;
	}
	return -1;
}

void scorehands(void) {
	int score[5];
	int high = 0;
	int count = 1;
	string str;
	for (int i=0; i<static_cast<signed>(players.size()); i++) {
		if (players[i]->getStatus() > 0) {
			Hand tmp(players[i]->hand());
			tmp.sort();
			score[i] = tmp.score();
		} else {
			score[i] = -1;
		}
	}
	for (int i=1; i<static_cast<signed>(players.size()); i++) {
		if (score[i] > score[high]) {
			high = i;
			count = 1;
		} else if (score[i] == score[high]) {
			count++;
		}
	}
	if (count > 1) {
		int split = pot / count;
		for (int i=0; i<static_cast<signed>(players.size()); i++) {
			if (score[i] == score[high]) {
				if (count == 1) {
					sprintf(buf,
							"and %s tie with %s and split the $%d in the pot.",
							players[i]->name(),
							scoreString(score[high]).c_str(), pot);
					str += buf;
					players[i]->deposit(split);
					break;
				} else {
					sprintf(buf, "%s%s",
						players[i]->name(),
						// to , or not to ,
						(count > 2 ? ", " : " "));
					str += buf;
					players[i]->deposit(split);
					count--;
				}
			}
		}
	} else {
		if (high == human) {
			sprintf(buf, "You win with %s, and collect $%d from the pot.",
				scoreString(score[high]).c_str(), pot);
			str = buf;
			gamestats->addWin();
		} else {
			sprintf(buf, "%s wins with %s, and collects $%d from the pot.",
				players[high]->name(),
				scoreString(score[high]).c_str(), pot);
			str = buf;
			gamestats->addLoss();
		}
		players[high]->deposit(pot);
	}
	ui->genEvent(str);
}

