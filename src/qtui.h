/* qtui.h - a Qt interface
 * 
 * Copyright (C) 1999-2000  Kevin Radloff <kmradlof@colby.edu>
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_QT

#ifndef __SP_QTUI_H
#define __SP_QTUI_H

#include <vector>

#include <qmainwindow.h>
#include <qtabdialog.h>
#include <qbuttongroup.h>

#include "ui.h"

class QApplication;
class QGridLayout;
class QTextView;
class QLabel;
class QLineEdit;
class SPMainMenu;
class SPGameMenu;

class SPWindow : public QMainWindow {

	Q_OBJECT

public:
	SPWindow();
	void showMenu();
	void hideMenu();
	void showGameMenu();
	void hideGameMenu();

protected:
	void closeEvent(QCloseEvent*);
	void resizeEvent(QResizeEvent*);

private slots:
	void about();
	void aboutQt();
	void options();
	void newgame();
	void stats();

private:
	QWidget* main;
	QToolBar* sptoolbar;
	SPMainMenu* menu;
	SPGameMenu* gm;

};

class QtInterface : public Interface {

protected:
	QApplication* app;
	SPWindow* appwin;
	QGridLayout* gamegrid;
	vector<QLabel*> plabel;
	vector<QTextView*> pwin;
	QLabel* msglabel;
	QTextView* msgbox;
	QLabel* accntlabel;
	QTextView* accntbox;

public:
	QtInterface(int, char**);
	~QtInterface();
	void main();
	void stats();
	void options();
	int playerBet();
	void playerDiscard();
	int playMenu();
	void genEvent(string);
	void playerEvent(string);
	void pause();
	void space();

};

class SPOptWindow : public QTabDialog {

	Q_OBJECT

public:
	SPOptWindow(QWidget*);

private slots:
	void applyOptions();
	void resetDefaults();

private:
	QWidget* main;

	// Poker options
	QWidget* pgPoker;
	QGridLayout* grdPoker;
	QLineEdit* edtPlayers, * edtAccounts, * edtDrawCap, * edtStudCap;
	QLabel* lblPlayers, * lblAccounts, * lblDrawCap, * lblStudCap;

};

class SPMainMenu : public QButtonGroup {

	Q_OBJECT

public:
	SPMainMenu(QWidget*);

private slots:
	void buttonPressed(int);

private:
	QWidget* main;
	QPushButton *btnGameMenu, *btnStats, *btnOptions, *btnQuit;

};

class SPGameMenu : public QButtonGroup {

	Q_OBJECT

public:
	SPGameMenu(QWidget*);

private slots:
	void buttonPressed(int);

private:
	QWidget* main;
	QPushButton *btnDraw, *btnStudFive, *btnStudSeven, *btnBack;

};

#endif

#endif

