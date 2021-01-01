/* cards.cpp - cards/player classes
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
#include <algorithm>
using namespace std;

#include "poker.h"

// globals
extern Deck* gamedeck;
extern char buf[64];

int randInt(int, int);

const string ranks[] = {
	"null", "null", "Two", "Three", "Four", "Five", "Six", "Seven",
	"Eight", "Nine", "Ten", "Jack", "Queen", "King", "Ace"
};

const string suits[] = {
	"Clubs", "Diamonds", "Hearts", "Spades"
};

Card::Card() {
	rank = 0;
	suit = 0;
}

Card::Card(int _rank, int _suit) {
	rank = _rank;
	suit = _suit;
}

int Card::getRank() {
	return rank;
}

int Card::getSuit() {
	return suit;
}

void Card::print() {
	cout << ranks[rank] + " of " + suits[suit] << endl;
}

const string Card::toString() {
	return ranks[rank] + " of " + suits[suit];
}

Deck::Deck() {
	int i = 0;
	for (int suit=0; suit<=3; suit++) {
		for (int rank=2; rank<=14; rank++) {
			cards[i] = new Card(rank, suit);
			i++;
		}
	}
	pos = 0;
}

Deck::~Deck() {
	for (int i=0; i<52; i++) {
		delete cards[i];
	}
}

int Deck::getRank(int i) {
	return cards[i]->getRank();
}

int Deck::getSuit(int i) {
	return cards[i]->getSuit();
}

int Deck::getPos() {
	return pos;
}

void Deck::printCard(int i) {
	cards[i]->print();
}

void Deck::print() {
	for (int i=0; i<52; i++)
		cards[i]->print();
}

void Deck::swapCards(int a, int b) {
	swap(cards[a], cards[b]);
}

void Deck::shuffle() {
	int rand;
	for (int i=0; i<52; i++) {
		rand = randInt(i, 51);
		swapCards(i, rand);
	}
	setPos(0);
}

Card* Deck::drawNext() {
	int x = getPos();
	setPos(x+1);
	return card(x);
}

Hand::Hand() {
}

Hand::Hand(Hand* hand) {
	cards.resize(hand->cards.size());
	vis.resize(hand->vis.size());
	copy(hand->cards.begin(), hand->cards.end(),
		 cards.begin());
	copy(hand->vis.begin(), hand->vis.end(),
		 vis.begin());
}

Hand::Hand(Deck* deck, int n) {
	for (int i=0; i<n; i++) {
		cards.push_back(deck->drawNext());
		vis.push_back(false);
	}
}

Hand::~Hand() {
}

Card* Hand::card(int i) {
	return cards[i];
}

vector<string> Hand::toStringVector() {
	vector<string> v;
	for (int i=0; i<static_cast<signed>(cards.size()); i++) {
		if (vis[i]) {
			v.push_back(cards[i]->toString());
		} else {
			v.push_back("Face down card");
		}
	}
	return v;
}

void Hand::print() {
	for (int i=0; i<5; i++)
		if (vis[i] == true)
			cards[i]->print();
}

void Hand::print(int i) {
	switch (i) {
	// Indent a little
	case 0:
		for (int j=0; j<static_cast<signed>(cards.size()); j++) {
			cout << "\t\t";
			if (vis[j]) {
				cards[j]->print();
			} else {
				cout << "Face down card" << endl;
			}
		}
		break;
	// Number them
	case 1:
		for (int j=0; j<static_cast<signed>(cards.size()); j++) {
			cout << j << ". ";
			if (vis[j]) {
				cards[j]->print();
			} else {
				cout << "Face down card" << endl;
			}
		}
		break;
	case 2:
		for (int j=0; j<static_cast<signed>(cards.size()); j++) {
			cout << "\t\t" << j+1 << ". ";
			if (vis[j]) {
				cards[j]->print();
			} else {
				cout << "Face down card" << endl;
			}
		}
		break;
	}
}

int Hand::getSuit(int i) {
	return cards[i]->getSuit();
}

int Hand::getRank(int i) {
	return cards[i]->getRank();
}

int Hand::getNum() {
	return cards.size();
}

void Hand::swapCards(int a, int b) {
	swap(cards[a], cards[b]);
}

// Discard and draw, for five card draw
void Hand::drawNew(Deck* deck, int i) {
	cards[i] = deck->drawNext();
}

// Add another card to the hand, for stud
void Hand::drawNew(Deck* deck, bool b) {
	cards.push_back(deck->drawNext());
	vis.push_back(b);
}

// Make all cards visible to the player, postgame function for stud
void Hand::makeVis() {
	for (int i=0; i<static_cast<signed>(cards.size()); i++) {
		vis[i] = true;
	}
}

int compareCards(Card* a, Card* b) {
	return  (a->getRank() < b->getRank() ? -1 :
			(a->getRank() > b->getRank() ? 1  :
			(a->getSuit() < b->getSuit() ? -1 :
			(a->getSuit() > b->getSuit() ? 1  : 0))));
}

bool sameCard(Card* a, Card* b) {
	return (a->getSuit() == b->getSuit() &&
			a->getRank() == b->getRank());
}

int findCard(Deck* deck, Card* c) {
	for (int i=0; i<52; i++) {
		if (sameCard(deck->card(i), c)) return i;
	}
	return -1;
}

int findLowestCard(Deck* deck, int lowIndex, int highIndex) {
	int low = lowIndex;
	for (int i=lowIndex+1; i<=highIndex; i++)
		if (compareCards(deck->card(low), deck->card(i)) == 1)
			low = i;
	return low;
}

int findLowestCard(Hand* hand, int lowIndex, int highIndex) {
	int low = lowIndex;
	for (int i=lowIndex+1; i<=highIndex; i++)
		if (compareCards(hand->card(low), hand->card(i)) == 1)
			low = i;
	return low;
}

void Deck::sort() {
	for (int i=0; i<52; i++)
		swapCards(i, findLowestCard(this, i, 51));
}

void Hand::sort() {
	for (int i=0; i<static_cast<signed>(cards.size()); i++)
		swapCards(i, findLowestCard(this, i, cards.size()-1));
}

