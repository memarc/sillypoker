/***

error.h - public declaration for error box
Written by Gerard Paul Java
Copyright (c) Gerard Paul Java 1997

***/

#ifdef HAVE_NCURSES

#ifdef __cplusplus
extern "C" {
#endif

#define ANYKEY_MSG "Press a key to continue"
#define ABORT_MSG "Press a key to abort"

void errbox(char *message, char *prompt, int *response);
/*void write_error(char *message, int daemonized);*/

#ifdef __cplusplus
}
#endif

#endif

