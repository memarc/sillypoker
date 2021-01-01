/***

deskman.c - desktop management routines

Written by Gerard Paul Java
Copyright (c) Gerard Paul Java 1997, 1998

This software is open source; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License in the included COPYING file for
details.

***/

#ifdef HAVE_NCURSES

#include <curses.h>
#include <stdlib.h>
#include <panel.h>
#include <string.h>
#include "stdwinset.h"
#include "deskman.h"
#include "error.h"

/* Attribute variables */

int STDATTR;
int HIGHATTR;
int BOXATTR;
int ACTIVEATTR;
int BARSTDATTR;
int BARHIGHATTR;
int DESCATTR;
int PTRATTR;
int FIELDATTR;
int ERRBOXATTR;
int ERRTXTATTR;
/*int OSPFATTR;
int UDPATTR;
int IGPATTR;
int IGMPATTR;
int IGRPATTR;
int ARPATTR;
int UNKNATTR;*/


/*  draw the basic desktop common to my screen-oriented programs */

void draw_desktop(void)
{
    int row;			/* counter for desktop construction */
    char sp_buf[10];

    attrset(STDATTR);
    move(0, 0);
    sprintf(sp_buf, "%%%dc", COLS);
    printw(sp_buf, ' ');	/* these two print the top n' bottom */
    move(LINES - 1, 0);
    printw(sp_buf, ' ');	/* lines */

    attrset(FIELDATTR);

    for (row = 1; row <= LINES - 2; row++) {	/* draw the background */
	move(row, 0);
	printw(sp_buf, ' ');
    }

    refresh();
}

/* for some reason, the ncurses box routine does not color
   its inside.  it would therefore be better to call this
   routine before drawing the box.
 */

void colorwin(WINDOW * win)
{
    int ctr;
    char *blankpad;
    blankpad = (char *) malloc(sizeof(char) * (COLS + 1));

    strcpy(blankpad, "");

    for (ctr = 0; ctr <= win->_maxx; ctr++) {
	strcat(blankpad, " ");
    }

    scrollok(win, 0);
    for (ctr = 0; ctr <= win->_maxy; ctr++) {
	wmove(win, ctr, 0);
	wprintw(win, "%s", blankpad);
    }
    scrollok(win, 1);
    free(blankpad);
}

void coloreol()
{
    char *blankpad;
    int y, x;
    int i;

    blankpad = (char *) malloc(sizeof(char) * (COLS + 1));
    strcpy(blankpad, "");
    getyx(stdscr, y, x);

    for (i = x; i <= COLS - 1; i++)
	strcat(blankpad, " ");

    printw(blankpad);
    free(blankpad);
}

/*void about()
{
    WINDOW *win;
    PANEL *panel;
    int ch;

    win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    panel = new_panel(win);

    stdwinset(win);
    wtimeout(win, -1);
    wattrset(win, BOXATTR);
    colorwin(win);
    box(win, ACS_VLINE, ACS_HLINE);
    wattrset(win, STDATTR);
    mvwprintw(win, 1, 2, "IPTraf");
    mvwprintw(win, 2, 2, "An IP Network Statistics Utility");
    mvwprintw(win, 3, 2, "Version %s", VERSION);
    mvwprintw(win, 5, 2, "Written by Gerard Paul Java");
    mvwprintw(win, 6, 2, "Copyright (c) Gerard Paul Java 1997, 1998");
    mvwprintw(win, 8, 2, "This program is distributed under the terms of");
    mvwprintw(win, 9, 2, "the GNU General Public License Version 2 or");
    mvwprintw(win, 10, 2, "any later version. See the included COPYING");
    mvwprintw(win, 11, 2, "file for details.");

    wattrset(win, HIGHATTR);
    mvwprintw(win, 13, 2, ANYKEY_MSG);

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

void masq_warning(void)
{
    WINDOW *win;
    PANEL *panel;

    win = newwin(15, 50, (LINES - 15) / 2, (COLS - 50) / 2);
    panel = new_panel(win);

    stdwinset(win);
    wtimeout(win, -1);
    wattrset(win, ERRBOXATTR);
    colorwin(win);
    box(win, ACS_VLINE, ACS_HLINE);
    wattrset(win, ERRTXTATTR);
    mvwprintw(win, 1, 2, "This computer is running with IP Masquerading");
    mvwprintw(win, 2, 2, "enabled.  If this machine is a masquerading");
    mvwprintw(win, 3, 2, "router, the various facilities may give");
    mvwprintw(win, 4, 2, "strange results because of the translation of");
    mvwprintw(win, 5, 2, "the packets' IP addresses.  You will get");
    mvwprintw(win, 6, 2, "better results if you run IPTraf on the");
    mvwprintw(win, 7, 2, "network segments connected to this machine,");
    mvwprintw(win, 8, 2, "but not on this machine itself.");
    mvwprintw(win, 10, 2, "If this computer is not a masquerading router");
    mvwprintw(win, 11, 2, "you can ignore this message.");
    wattrset(win, ERRBOXATTR);
    mvwprintw(win, 13, 2, ANYKEY_MSG);

    wgetch(win);

    del_panel(panel);
    delwin(win);
    update_panels();
    doupdate();
}

void printnomem()
{
    attrset(ERRTXTATTR);
    mvprintw(0, 68, " Memory Low ");
}

void printipcerr()
{
    attrset(ERRTXTATTR);
    mvprintw(0, 68, "  IPC Error ");
}*/

void printkeyhelp(char *keytext, char *desc, WINDOW * win)
{
    wattrset(win, HIGHATTR);
    wprintw(win, "%s", keytext);
    wattrset(win, STDATTR);
    wprintw(win, "%s", desc);
}

void stdkeyhelp(WINDOW * win)
{
    printkeyhelp("Enter", "-accept  ", win);
    printkeyhelp("Ctrl+X", "-cancel", win);
}

void stdexitkeyhelp()
{
    printkeyhelp("X/Ctrl+X", "-Exit", stdscr);
    coloreol(stdscr);
}

void scrollkeyhelp()
{
    printkeyhelp("Up/Down/PgUp/PgDn", "-scroll window  ", stdscr);
}

void menukeyhelp()
{
    move(LINES - 1, 1);
    printkeyhelp("Up/Down", "-Move selector  ", stdscr);
    printkeyhelp("Enter", "-execute", stdscr);
    coloreol(stdscr);
}

void listkeyhelp()
{
    move(LINES - 1, 1);
    printkeyhelp("Up/Down", "-move pointer  ", stdscr);
    printkeyhelp("Enter", "-select  ", stdscr);
    stdexitkeyhelp();
}

void tabkeyhelp(WINDOW * win)
{
    printkeyhelp("Tab", "-next field  ", win);
}

void indicate(char *message)
{
    char sp_buf[10];
    attrset(STDATTR);
    sprintf(sp_buf, "%%%dc", COLS - 1);
    mvprintw(LINES - 1, 1, sp_buf, ' ');
    mvprintw(LINES - 1, 1, message);
    refresh();
}

#if 0
void printlargenum(unsigned long long i, WINDOW * win)
{
    if (i < 100000000)		/* less than 100 million */
	wprintw(win, "%9llu", i);
    else if (i < 1000000000)	/* less than 1 billion */
	wprintw(win, "%8lluK", i / 1000);
    else if (i < 100000000000ULL)	/* less than 100 billion */
	wprintw(win, "%8lluM", i / 1000000);
    else if (i < 100000000000000ULL)	/* less than 100 trillion */
	wprintw(win, "%8lluG", i / 1000000000ULL);
    else
	wprintw(win, "%8lluT", i / 1000000000000ULL);
}
#endif

void standardcolors(int color)
{
    if ((color) && (has_colors())) {
	init_pair(1, COLOR_BLUE, COLOR_WHITE);
	init_pair(2, COLOR_BLACK, COLOR_CYAN);
	init_pair(3, COLOR_CYAN, COLOR_BLUE);
	init_pair(4, COLOR_YELLOW, COLOR_RED);
	init_pair(5, COLOR_WHITE, COLOR_RED);
	init_pair(6, COLOR_BLUE, COLOR_CYAN);
	init_pair(7, COLOR_BLUE, COLOR_WHITE);
	init_pair(9, COLOR_RED, COLOR_WHITE);
	init_pair(10, COLOR_GREEN, COLOR_BLUE);
	init_pair(11, COLOR_CYAN, COLOR_BLACK);
	init_pair(12, COLOR_RED, COLOR_CYAN);
	init_pair(14, COLOR_YELLOW, COLOR_BLUE);
	init_pair(15, COLOR_YELLOW, COLOR_BLACK);
	init_pair(16, COLOR_WHITE, COLOR_CYAN);
	init_pair(17, COLOR_YELLOW, COLOR_CYAN);

	STDATTR = COLOR_PAIR(14) | A_BOLD;
	HIGHATTR = COLOR_PAIR(3) | A_BOLD;
	BOXATTR = COLOR_PAIR(3);
	ACTIVEATTR = COLOR_PAIR(10) | A_BOLD;
	BARSTDATTR = COLOR_PAIR(15) | A_BOLD;
	BARHIGHATTR = COLOR_PAIR(11) | A_BOLD;
	DESCATTR = COLOR_PAIR(2);
	PTRATTR = COLOR_PAIR(10) | A_BOLD;
	FIELDATTR = COLOR_PAIR(1);
	ERRBOXATTR = COLOR_PAIR(5) | A_BOLD;
	ERRTXTATTR = COLOR_PAIR(4) | A_BOLD;
/*	OSPFATTR = COLOR_PAIR(2);
	UDPATTR = COLOR_PAIR(9);
	IGPATTR = COLOR_PAIR(12);
	IGMPATTR = COLOR_PAIR(10) | A_BOLD;
	IGRPATTR = COLOR_PAIR(16) | A_BOLD;
	ARPATTR = COLOR_PAIR(5) | A_BOLD;
	UNKNATTR = COLOR_PAIR(4) | A_BOLD;*/
    } else {
	STDATTR = A_NORMAL;
	HIGHATTR = A_BOLD;
	BOXATTR = A_NORMAL;
	ACTIVEATTR = A_BOLD;
	BARSTDATTR = A_REVERSE;
	BARHIGHATTR = A_BOLD;
	DESCATTR = A_NORMAL;
	PTRATTR = A_BOLD;
	FIELDATTR = A_REVERSE;
	ERRBOXATTR = A_BOLD;
	ERRTXTATTR = A_NORMAL;
/*	OSPFATTR = A_REVERSE;
	UDPATTR = A_BOLD;
	IGPATTR = A_REVERSE;
	IGMPATTR = A_REVERSE;
	IGRPATTR = A_REVERSE;
	ARPATTR = A_BOLD;
	UNKNATTR = A_BOLD;*/
    }
}

void refresh_screen(void)
{
    endwin();
    doupdate();
    curs_set(0);
}

#endif

