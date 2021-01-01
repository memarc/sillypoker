/***

stdwinset.h - prototype declaration for setting the standard window settings
for IPTraf

***/

#ifdef HAVE_NCURSES

#include <curses.h>

#ifdef __cplusplus
extern "C" {
#endif

void stdwinset(WINDOW * win);

#ifdef __cplusplus
}
#endif

#endif

