
/***

stdwinset.c - a single routine to set the standard options of the windows

***/

#ifdef HAVE_NCURSES

#include <curses.h>

void stdwinset(WINDOW * win)
{
    meta(win, TRUE);
    keypad(win, TRUE);
    notimeout(win, 0);
    scrollok(win, 1);
}

#endif

