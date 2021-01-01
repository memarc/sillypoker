/*
   deskman.h - header file for deskman.c
   Written by Gerard Paul Java
   Copyright (c) Gerard Paul Java 1997, 1998

 */

#ifdef HAVE_NCURSES

#ifdef __cplusplus
extern "C" {
#endif

void draw_desktop(void);
void colorwin(WINDOW * win);
void coloreol();
void printnomem();
void printipcerr();
void printkeyhelp(char *keytext, char *desc, WINDOW * win);
void stdkeyhelp(WINDOW * win);
void tabkeyhelp(WINDOW * win);
void scrollkeyhelp();
void stdexitkeyhelp();
void menukeyhelp();
void listkeyhelp();
void indicate(char *message);
/*void printlargenum(unsigned long long i, WINDOW *win);*/
void standardcolors(int color);
void refresh_screen(void);

#ifdef __cplusplus
}
#endif

#endif

