
/***

error.c - An error-handling subroutine--just displays a red error box

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
#include <panel.h>
#include "deskman.h"
#include "attrs.h"
#include "error.h"
/*#include "log.h"*/

void errbox(char *message, char *prompt, int *response)
{
    WINDOW *win;
    PANEL *panel;

    win = newwin(4, 70, (LINES - 4) / 2, (COLS - 70) / 2);
    panel = new_panel(win);

    wattrset(win, ERRBOXATTR);
    colorwin(win);
    box(win, ACS_VLINE, ACS_HLINE);
    wmove(win, 2, 2);
    wprintw(win, "%s", prompt);
    wattrset(win, ERRTXTATTR);
    wmove(win, 1, 2);
    wprintw(win, "%s", message);
    update_panels();
    doupdate();

    do {
	*response = wgetch(win);
	if (*response == 12)
	    refresh_screen();
    } while (*response == 12);

    del_panel(panel);
    delwin(win);
    update_panels();
    doupdate();
}

/*void write_error(char *msg, int daemonized)
{
    int response;

    if (daemonized)
	write_daemon_err(msg);
    else
	errbox(msg, ANYKEY_MSG, &response);
}*/

#endif

