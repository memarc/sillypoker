/* player.cpp - player class
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
using namespace std;

#include "poker.h"
#include "conf.h"
#include "ui.h"

// globals
extern int pot;
extern Config* conf;
extern Interface* ui;
extern char buf[64];

Player::Player(int i) {
	id = i;
	account = conf->getAccount();
	status = 1;
}

Player::~Player() {
}

const char* Player::name() {
	return "You";
}

int Player::balance() {
	return account;
}

int Player::withdraw(int amt) {
	account -= amt;
	return account;
}

int Player::deposit(int amt) {
	account += amt;
	return account;
}

int Player::getStatus() {
	return status;
}

/* 
 * 1: normal, in play currently
 * 0: folded, will be in play next round
 * -1: bankrupt, out of play permanently
 */

void Player::setStatus(int i) {
	status = i;
}

int Player::bet() {
	int b = ui->playerBet();
	return b;
}

void Player::discard() {
	ui->playerDiscard();
	h->sort();
}

void Player::createHand(Deck* deck, int i) {
	h = new Hand(deck, i);
}

void Player::createHand() {
	h = new Hand();
}

void Player::deleteHand() {
	delete h;
}

Hand* Player::hand() {
	return h;
}

void Player::drawCard(Deck* deck, bool pr, bool vis) {
	h->drawNew(deck, vis);
	if (pr) {
		ui->playerEvent("You are dealt the " +
			h->card(h->getNum()-1)->toString() + '.');
	}
}

string Player::betEvent(string str, int n) {
	if (str == "raise") {
		sprintf(buf, "You raise with $%d.", n);
		return buf;
	} else if (str == "call") {
		return "You call.";
	} else if (str == "bet") {
		sprintf(buf, "You bet $%d.", n);
		return buf;
	} else if (str == "fold") {
		return "You fold.";
	} else if (str == "bankrupt") {
		return "You have gone bankrupt.";
	}
}

void Player::printHand(int n) {
	if (id == -1) {
		printf("\n\tYour hand:\n");
	}
	h->print(n);
}

void Player::visHand() {
	h->makeVis();
}

vector<string> Player::handToStringVector() {
	return h->toStringVector();
}

