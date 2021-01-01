/* poker.h - card/player objects
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

#ifndef __SP_POKER_H
#define __SP_POKER_H

#ifndef VERSION
#	define VERSION "0.25.5"
#endif // VERSION

enum {
	TWO=2, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN,
	JACK, QUEEN, KING, ACE
};

enum {
	CLUBS, DIAMONDS, HEARTS, SPADES
};

class Card {

protected:
	int rank, suit;

public:
	Card();
	Card(int rank, int suit);

	void print();
	int getRank();
	int getSuit();
	const string toString();

};

class Deck {

protected:
	Card* cards[52];
	int pos;

public:
	Deck();
	~Deck();

	int getRank(int i);
	int getSuit(int i);
	int getPos();
	inline Card* card(int i) { return cards[i]; }
	void printCard(int i);
	void print();
	void shuffle();
	Card* drawNext();
	void sort();

protected:
	// drawNext() should take care of this..
	inline void setPos(int i) { pos = i; }
	void swapCards(int a, int b);
	
};

class Hand {

protected:
	vector<Card*> cards;
	vector<bool> vis;

public:
	Hand();
	Hand(Hand* hand);
	Hand(Deck* deck, int n);
	~Hand();

	Card* card(int i);
	vector<string> toStringVector();
	void print();
	void print(int i);
	int getRank(int i);
	int getSuit(int i);
	int getNum();
	int score();
	void drawNew(Deck* deck, int i);
	void drawNew(Deck* deck, bool b);
	void makeVis();
	void sort();
	void set(int);

protected:
	bool isStraight();
	int isFlush();
	void swapCards(int a, int b);
	void swapCards(int i, Card* c);

};

class Player {

protected:
	int id;
	int account;
	int status;
	Hand* h;

public:
	Player(int i);
	virtual ~Player();
	virtual const char* name();
	int balance();
	int withdraw(int amt);
	int deposit(int amt);
	int getStatus();
	void setStatus(int i);
	virtual int bet();
	virtual void discard();
	virtual void createHand(Deck* deck, int i);
	void createHand();
	void deleteHand();
	Hand* hand();
	virtual void drawCard(Deck* deck, bool pr, bool vis);
	virtual string betEvent(string str, int n);
	virtual void printHand(int n);
	void visHand();
	vector<string> handToStringVector();

};

class AI : public Player {

protected:
	int rankHist[15];
	int suitHist[4];
	bool discarded;

public:
	AI(int i);
	~AI();
	void printHand(int n);
	void createHand(Deck* deck, int a);
	void updateHist();
	int bet();
	void discard();
	const char* name();
	void drawCard(Deck* deck, bool pr, bool vis);
	string betEvent(string str, int n);

};

#endif

