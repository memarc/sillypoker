/* util.cpp - utility functions
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
#include <cstring>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#ifdef TIME_WITH_SYS_TIME
#	include <time.h>
#	include <sys/time.h>
#else // !TIME_WITH_SYS_TIME
#	ifdef HAVE_SYS_TIME_H
#		include <sys/time.h>
#	else // !HAVE_SYS_TIME_H
#		include <time.h>
#	endif // HAVE_SYS_TIME_H
#endif // TIME_WITH_SYS_TIME

void enter(void) {
	cout << "Hit Enter to continue.";
	getchar();
}

void space(void) {
	cout << endl;
}

bool isnum(char* str) {
	char* ptr = str;
	int len = strlen(str);
	for (; *ptr; ptr++) {
		char c = *ptr;
		if (c == '\n' && len > 1) continue;
		if (isdigit(c) == 0) return false;
	}
	return true;
}

int randInt(int low, int high) {
	int x = rand() % (high+1-low) + low;
	return x;
}

void seedrnd(void) {
#ifdef HAVE_SYS_TIME_H
	struct timeval tv;
	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
#else // !HAVE_SYS_TIME_H
	srand((unsigned)time(NULL));
#endif // HAVE_SYS_TIME_H
}

void parsecmdline(int argc, char** argv) {
	for (int i=1; i<=argc-1; i++) {
		if (!strcmp(argv[i], "-V") || !strcmp(argv[i], "--version")) {
			cout << "silly Poker version " << VERSION
				 << ", Copyright (C) 1999-2000  Kevin Radloff" << endl
				 << "This program is free software; you may redistribute it under the terms of" << endl
				 << "the GNU General Public License.  This program has absolutely no warranty." << endl;
			exit(0);
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			cout << "silly Poker version " << VERSION
				 << ", Copyright (C) 1999-2000  Kevin Radloff" << endl << endl
				 << "Supported flags: [-V -h] [--version --help]" << endl;
			exit(0);
		}
#ifndef HAVE_QT
		else {
			cerr << "Unknown option `" << argv[i] << "'." << endl
				 << "Supported flags: [-V -h] [--version --help]" << endl;
			exit(0);
		}
#endif
	}
}

void showabout(void) {
	cout << endl << "silly Poker version " << VERSION
		 << ", Copyright (C) 1999-2000  Kevin Radloff" << endl
		 << "This program is free software, you may redistribute it under the terms of" << endl
		 << "the GNU General Public License.  This program has absolutely no warranty." << endl << endl
		 << "silly Poker was created for seemingly absolutely no good reason whatsoever," << endl
		 << "and was developed on GNU/Linux using VIM 5.5/6, gcc 2.95, and GDB." << endl
		 << "Thanks to Gameslink (irc.gameslink.net) #unreal for not helping to test," << endl
		 << "and attempting to belittle me because VC++ isn't ANSI compliant. ;)" << endl;
}

