/***

menurt.c- ncurses-based menu definition module
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
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "deskman.h"
#include "attrs.h"
#include "menurt.h"

/* initialize menu system */

void initmenu(struct MENU *menu, const char *desc, int y1, int x1, int y2, int x2)
{
    menu->itemlist = NULL;
    menu->itemcount = 0;
    strcpy(menu->shortcuts, "");
    menu->x1 = x1;
    menu->y1 = y1;
    menu->x2 = x2;
    menu->y2 = y2;
    menu->menuwin = newwin(y1, x1, y2, x2);
    menu->menupanel = new_panel(menu->menuwin);
    menu->menu_maxx = x1 - 2;
	strcpy(menu->desc, desc);

    keypad(menu->menuwin, 1);
    meta(menu->menuwin, 1);
    noecho();
    wtimeout(menu->menuwin, -1);	/* block until input */
    notimeout(menu->menuwin, 0);	/* disable Esc timer */
    nonl();
    cbreak();
}

/* add menu item */

void additem(struct MENU *menu, char *item, char *desc)
{
    struct ITEM *tnode;
    char cur_option[OPTIONSTRLEN_MAX];
    char thekey[2];
    
    if (menu->itemcount >= 25)
        return;
        
    tnode = malloc(sizeof(struct ITEM));

    if (item != NULL) {
	strcpy(tnode->option, item);
	strcpy(tnode->desc, desc);
	tnode->itemtype = REGULARITEM;

	strcpy(cur_option, item);
	strtok(cur_option, "^");
	strcpy(thekey, strtok(NULL, "^"));
	thekey[0] = toupper(thekey[0]);
	strcat(menu->shortcuts, thekey);
    } else {
	tnode->itemtype = SEPARATOR;
	strcat(menu->shortcuts, "^");	/* mark shortcut position for seps */
    }

    if (menu->itemlist == NULL) {
	menu->itemlist = tnode;
    } else {
	menu->lastitem->next = tnode;
	tnode->prev = menu->lastitem;
    }

    menu->itemlist->prev = tnode;
    menu->lastitem = tnode;
    tnode->next = menu->itemlist;
    menu->itemcount++;
}

/* show each individual item */

void showitem(struct MENU *menu, struct ITEM *itemptr, int selected)
{
    int hiattr = 0;
    int loattr = 0;
    int ctr;
    char curoption[OPTIONSTRLEN_MAX];
    char padding[OPTIONSTRLEN_MAX];

    if (itemptr->itemtype == REGULARITEM) {
	switch (selected) {
	case NOTSELECTED:
	    hiattr = HIGHATTR;
	    loattr = STDATTR;
	    break;
	case SELECTED:
	    hiattr = BARHIGHATTR;
	    loattr = BARSTDATTR;
	    break;
	}

	strcpy(curoption, itemptr->option);

	wattrset(menu->menuwin, loattr);
	wprintw(menu->menuwin, "%s", strtok(curoption, "^"));
	wattrset(menu->menuwin, hiattr);
	wprintw(menu->menuwin, "%s", strtok((char *) NULL, "^"));
	wattrset(menu->menuwin, loattr);
	wprintw(menu->menuwin, "%s", strtok((char *) NULL, "^"));

	strcpy(padding, "");

	for (ctr = strlen(itemptr->option); ctr <= menu->x1 - 1; ctr++)
	    strcat(padding, " ");

	wprintw(menu->menuwin, "%s", padding);
    } else {
	wattrset(menu->menuwin, BOXATTR);
	whline(menu->menuwin, ACS_HLINE, menu->menu_maxx);
    }

    update_panels();
    doupdate();
}

/* repeatedly calls showitem to display individual items */

void showmenu(struct MENU *menu)
{
    struct ITEM *itemptr;	/* points to each item in turn */
    int ctr = 1;		/* counts each item */

	wbkgd(menu->menuwin, BOXATTR);
#if 0
    wattrset(menu->menuwin, BOXATTR);	/* set to bg+/b */
    colorwin(menu->menuwin);	/* color window */
#endif
    box(menu->menuwin, ACS_VLINE, ACS_HLINE);	/* draw border */

    itemptr = menu->itemlist;	/* point to start */

    wattrset(menu->menuwin, STDATTR);
	if (strlen(menu->desc) > 0)
		mvwprintw(menu->menuwin, 0, 2, " %s ", menu->desc);

    do {			/* display items */
	wmove(menu->menuwin, ctr, 1);
	showitem(menu, itemptr, NOTSELECTED);	/* show items, initially unselected */
	ctr++;
	itemptr = itemptr->next;
    } while (ctr <= menu->itemcount);

    update_panels();
    doupdate();
}

void menumoveto(struct MENU *menu, struct ITEM **itemptr, unsigned int row)
{
    struct ITEM *tnode;
    unsigned int i;

    tnode = menu->itemlist;
    for (i = 1; i < row; i++)
	tnode = tnode->next;

    *itemptr = tnode;
}

/* actually to the menu operation after all the initialization */

void operatemenu(struct MENU *menu, int *position, int *aborted)
{
    struct ITEM *itemptr;
    int row = *position;
    int exitloop = 0;
    int ch;
    char *keyptr;

    menukeyhelp();
    *aborted = 0;
    menumoveto(menu, &itemptr, row);

    menu->descwin = newwin(1, COLS, LINES - 2, 0);
    menu->descpanel = new_panel(menu->descwin);

    do {
	wmove(menu->menuwin, row, 1);
	showitem(menu, itemptr, SELECTED);

	/*
	 * Print item description
	 */

	wattrset(menu->descwin, DESCATTR);
	wbkgd(menu->descwin, DESCATTR);
	werase(menu->descwin);
/*	colorwin(menu->descwin);*/
	wmove(menu->descwin, 0, 0);
	wprintw(menu->descwin, " %s", itemptr->desc);
	update_panels();
	doupdate();

	wmove(menu->menuwin, row, 2);
	ch = wgetch(menu->menuwin);
	wmove(menu->menuwin, row, 1);
	showitem(menu, itemptr, NOTSELECTED);

	switch (ch) {
	case KEY_UP:
	    if (row == 1)
		row = menu->itemcount;
	    else
		row--;

	    itemptr = itemptr->prev;

	    if (itemptr->itemtype == SEPARATOR) {
		row--;
		itemptr = itemptr->prev;
	    }
	    break;
	case KEY_DOWN:
	    if (row == menu->itemcount)
		row = 1;
	    else
		row++;

	    itemptr = itemptr->next;
	    if (itemptr->itemtype == SEPARATOR) {
		row++;
		itemptr = itemptr->next;
	    }
	    break;
	case 12:
	    refresh_screen();
	    break;
	case 13:
	    exitloop = 1;
	    break;
	    /* case 27: exitloop = 1;*aborted = 1;row=menu->itemcount;break; */
	case '^':
	    break;		/* ignore caret key */
	default:
	    keyptr = strchr(menu->shortcuts, toupper(ch));
	    if ((keyptr != NULL) && keyptr - menu->shortcuts < menu->itemcount) {
		row = keyptr - menu->shortcuts + 1;
		exitloop = 1;
	    }
	}
    } while (!(exitloop));

    *position = row;		/* position of executed option is in *position */
    del_panel(menu->descpanel);
    delwin(menu->descwin);
    update_panels();
    doupdate();
}


void destroymenu(struct MENU *menu)
{
    struct ITEM *tnode;
    struct ITEM *tnextnode;

    if (menu->itemlist != NULL) {
	tnode = menu->itemlist;
	tnextnode = menu->itemlist->next;

	tnode->prev->next = NULL;

	while (tnode != NULL) {
	    free(tnode);
	    tnode = tnextnode;

	    if (tnextnode != NULL)
		tnextnode = tnextnode->next;
	}
    }
    del_panel(menu->menupanel);
    delwin(menu->menuwin);
    update_panels();
    doupdate();
}

#endif

