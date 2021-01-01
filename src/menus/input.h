
/***

input.h - structure declarations and function prototypes for input.c
Written by Gerard Paul Java
Copyright (c) Gerard Paul Java 1997

***/

#ifdef HAVE_NCURSES

#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include "stdwinset.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CTRL_X 24

typedef struct FIELD {
    char *buf;
    unsigned int len;
    unsigned int tlen;
    unsigned int xpos;
    unsigned int ypos;
    struct FIELD *prevfield;
    struct FIELD *nextfield;
} FIELD;

typedef struct FIELDLIST {
    struct FIELD *list;
    WINDOW *fieldwin;
    PANEL *fieldpanel;
} FIELDLIST;

void initfields(struct FIELDLIST *list, int leny, int lenx, int begy, int begx);
void addfield(struct FIELDLIST *list, unsigned int len, unsigned int y,
	      unsigned int x, char *initstr);
void getinput(struct FIELDLIST *list, struct FIELD *field, int *exitkey);
void fillfields(struct FIELDLIST *list, int *aborted);
void destroyfields(struct FIELDLIST *list);

#ifdef __cplusplus
}
#endif

#endif

