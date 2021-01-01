/***
   menu.h - declaration file for my menu library
   Copyright (c) Gerard Paul R. Java 1997

***/

#ifdef HAVE_NCURSES

#include <curses.h>

#ifdef __cplusplus
extern "C" {
#endif
	
#define SELECTED 1
#define NOTSELECTED 0

#define SEPARATOR 0
#define REGULARITEM 1

#define OPTIONSTRLEN_MAX 50
#define DESCSTRLEN_MAX 81
#define SHORTCUTSTRLEN_MAX 25
struct ITEM {
    char option[OPTIONSTRLEN_MAX];
    char desc[DESCSTRLEN_MAX];
    unsigned int itemtype;
    struct ITEM *prev;
    struct ITEM *next;
};

typedef struct MENU {
    struct ITEM *itemlist;
    struct ITEM *selecteditem;
    struct ITEM *lastitem;
    int itemcount;
    int postn;
    int x1, y1;
    int x2, y2;
    unsigned int menu_maxx;
    WINDOW *menuwin;
    PANEL *menupanel;
    WINDOW *descwin;
    PANEL *descpanel;
    char shortcuts[SHORTCUTSTRLEN_MAX];
	char desc[DESCSTRLEN_MAX];
} MENU;

extern void initmenu(struct MENU *menu, const char *desc, int y1, int x1, int y2, int x2);
extern void additem(struct MENU *menu, char *item, char *desc);
extern void showitem(struct MENU *menu, struct ITEM *itemptr, int selected);
extern void showmenu(struct MENU *menu);
extern void operatemenu(struct MENU *menu, int *row, int *aborted);
extern void destroymenu(struct MENU *menu);

#ifdef __cplusplus
}
#endif

#endif

