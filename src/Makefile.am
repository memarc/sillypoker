AUTOMAKE_OPTIONS = 1.4

SUBDIRS = menus

bin_PROGRAMS = sillypoker

sillypoker_LDFLAGS = $(QT_LDFLAGS)

INCLUDES = -I. -I.. -Imenus $(QT_INCLUDES)

if QT
MOCSRCS = qtui.moc.cpp
endif

sillypoker_SOURCES = \
	ai.cpp \
	cards.cpp \
	conf.cpp \
	games.cpp \
	main.cpp \
	ncurses.cpp \
	player.cpp \
	qtui.cpp \
	$(MOCSRCS) \
	score.cpp \
	scrollwin.cpp \
	stats.cpp \
	ui.cpp \
	util.cpp

if QT
qtui.moc.cpp: qtui.h
	$(MOC) qtui.h -o qtui.moc.cpp

clean:
	rm -f qtui.moc.cpp

distclean:
	rm -f qtui.moc.cpp
endif

sillypoker_DEPENDENCIES = menus/libmenus.a

sillypoker_LDADD = menus/libmenus.a

