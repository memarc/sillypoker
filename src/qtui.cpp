/* qtui.cpp - a Qt interface
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

#include <string>
#include <vector>
#include <cstdlib>

#include <qapplication.h>
#include <qmainwindow.h>
#include <qpushbutton.h>
#include <qpopupmenu.h>
#include <qwhatsthis.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qdialog.h>
#include <qinputdialog.h>
#include <qlineedit.h>
#include <qlabel.h>
#include <qvalidator.h>
#include <qlayout.h>

#include "poker.h"
#include "stats.h"
#include "conf.h"
#include "ui.h"
#include "qtui.h"

extern Interface* ui;
extern vector<Player*> players;
extern Stats* gamestats;
extern Config* conf;
extern int human;
extern char buf[64];
extern int curbet;
extern int game;

void createopponents(void);

SPWindow::SPWindow()
	: QMainWindow(0, "silly Poker main window", WDestructiveClose) {
	int id;
	QPixmap newIcon, statIcon, optIcon, aboutIcon;

//	sptoolbar = new QToolBar(this, "toolbar");
//	sptoolbar->setLabel(tr("Toolbar"));

//	(void)QWhatsThis::whatsThisButton(sptoolbar);

	QPopupMenu* file = new QPopupMenu(this);
	menuBar()->insertItem("&File", file);

	id = file->insertItem("&Statistics", this, SLOT(stats()), CTRL+Key_S);
	file->setWhatsThis(id, "Show statistics");
	id = file->insertItem("&Options", this, SLOT(options()), CTRL+Key_O);
	file->setWhatsThis(id, "Edit game options");
	file->insertSeparator();
	file->insertItem("&Quit", qApp, SLOT(closeAllWindows()), CTRL+Key_Q);

	QPopupMenu* help = new QPopupMenu(this);
	menuBar()->insertSeparator();
	menuBar()->insertItem("&Help", help);
	
	help->insertItem("&About", this, SLOT(about()));
	help->insertItem("About &Qt", this, SLOT(aboutQt()));
	help->insertSeparator();
	help->insertItem("What's &This", this, SLOT(whatsThis()), SHIFT+Key_F1);

	QStatusBar* status = new QStatusBar(this, "silly Poker status");
	status->setSizeGripEnabled(false);
	status->message("Welcome to silly Poker!");

	main = new QWidget(this);
	main->setBackgroundMode(PaletteBase);
	setCentralWidget(main);

	resize(600, 500);

	menu = new SPMainMenu(this);
	menu->resize(menu->sizeHint());
	menu->move((width() - menu->width())/2, (height() - menu->height())/2);
	menu->show();
	gm = new SPGameMenu(this);
	gm->resize(gm->sizeHint());
	gm->move((width() - gm->width())/2, (height() - gm->height())/2);
	gm->hide();
}

void SPWindow::closeEvent(QCloseEvent* ce) {
	switch (QMessageBox::information(this, "silly Poker",
			"Thanks for playing!\nReally quit?", "Cancel", "Quit", 0, 1)) {
	case 0:
		ce->ignore();
		return;
	case 1:
	default:
		ce->accept();
		break;
	}
}

void SPWindow::resizeEvent(QResizeEvent* re) {
	menu->move((width() - menu->width())/2, (height() - menu->height())/2);
}

void SPWindow::about() {
	QMessageBox::about(this, "silly Poker v" VERSION,
					   "silly Poker v" VERSION
					   "\n\nA very silly poker game.\n\n"
					   "Copyright (C) 1999-2000  Kevin Radloff");
}

void SPWindow::aboutQt() {
	QMessageBox::aboutQt(this, "silly Poker v" VERSION);
}

void SPWindow::options() {
	ui->options();
}

void SPWindow::newgame() {
	ui->playMenu();
}

void SPWindow::stats() {
	ui->stats();
}

void SPWindow::showMenu() {
	menu->show();
}

void SPWindow::hideMenu() {
	menu->hide();
}

void SPWindow::showGameMenu() {
	gm->show();
}

void SPWindow::hideGameMenu() {
	gm->hide();
}

QtInterface::QtInterface(int argc, char** argv) {
	app = new QApplication(argc, argv);
	app->connect(app, SIGNAL(lastWindowClosed()), app, SLOT(quit()));
	appwin = new SPWindow();
	appwin->setCaption("silly Poker");
}

QtInterface::~QtInterface() {
	delete app;
}

void QtInterface::main() {
	appwin->show();
	app->exec();
}

void QtInterface::stats() {
	int *stats = gamestats->getArray();
	int total = stats[0] + stats[1];
	QString str, str2;
	
	str.sprintf(
		"Games: %d  Wins: %d (%.2f%%)  Losses: %d (%.2f%%)\n"
		"Games played this session: %d\n"
		"Most in one sitting: %d\n"
		"Longest winning streak: %d\n"
		"Longest losing streak: %d\n"
		,
		total,
		stats[0], (total > 0 ?
			static_cast<double>(stats[0])/static_cast<double>(total)*100 : 0),
		stats[1], (total > 0 ?
			static_cast<double>(stats[1])/static_cast<double>(total)*100 : 0),
		stats[6],
		stats[7],
		stats[3],
		stats[5]
		);
	if (stats[2] > 1) {
		str2.sprintf("Current winning streak: %d\n", stats[2]);
		str += str2;
	} else if (stats[4] > 1) {
		str2.sprintf("Current losing streak: %d\n", stats[4]);
		str += str2;
	}
	str2.sprintf("Money won: $%d", stats[8] +
		players[human]->balance() - conf->getAccount());
	str += str2;

	QMessageBox statswin("User Statistics", str,
						 QMessageBox::Information,
						 QMessageBox::Ok, 0, 0,
						 appwin);
	statswin.exec();
	delete stats;
}

void QtInterface::options() {
	SPOptWindow optWin(appwin);
	optWin.exec();
}

int QtInterface::playerBet() {
	sprintf(buf, "Current bet: $%d\nYour bet:", curbet);
	bool ok = false;
	int n = QInputDialog::getInteger("Bet", buf, curbet, curbet,
		(game == 1 ? conf->getDrawBetcap() : conf->getStudBetcap()),
		1, &ok, appwin);
	if (ok)
		return n;
	else
		return 0;
}

void QtInterface::playerDiscard() {
}

int QtInterface::playMenu() {
	appwin->hideMenu();
	appwin->showGameMenu();
	return 0;
}

void QtInterface::genEvent(string str) {
}

void QtInterface::playerEvent(string str) {
}

void QtInterface::pause() {
}

void QtInterface::space() {
}

SPOptWindow::SPOptWindow(QWidget* parent) : QTabDialog(parent, 0, true, 0) {
	main = parent;

	// Poker options
	pgPoker = new QWidget(this);
	grdPoker = new QGridLayout(pgPoker, 4, 2, 6);
	// number of players
	edtPlayers = new QLineEdit(QString::number(conf->getPlayers()), pgPoker);
	edtPlayers->setValidator(new QIntValidator(edtPlayers));
	edtPlayers->setMaxLength(1);
	lblPlayers = new QLabel(edtPlayers, "&Number of players:", pgPoker);
	grdPoker->addWidget(edtPlayers, 0, 1);
	grdPoker->addWidget(lblPlayers, 0, 0);
	// starting account amount
	edtAccounts = new QLineEdit(QString::number(conf->getAccount()), pgPoker);
	edtAccounts->setValidator(new QIntValidator(edtAccounts));
	edtAccounts->setMaxLength(6);
	lblAccounts = new QLabel(edtAccounts, "Starting &account amount:", pgPoker);
	grdPoker->addWidget(edtAccounts, 1, 1);
	grdPoker->addWidget(lblAccounts, 1, 0);
	// draw bet cap
	edtDrawCap = new QLineEdit(QString::number(conf->getDrawBetcap()), pgPoker);
	edtDrawCap->setValidator(new QIntValidator(edtDrawCap));
	edtDrawCap->setMaxLength(3);
	lblDrawCap = new QLabel(edtDrawCap, "&Draw bet cap:", pgPoker);
	grdPoker->addWidget(edtDrawCap, 2, 1);
	grdPoker->addWidget(lblDrawCap, 2, 0);
	// stud bet cap
	edtStudCap = new QLineEdit(QString::number(conf->getStudBetcap()), pgPoker);
	edtStudCap->setValidator(new QIntValidator(edtStudCap));
	edtStudCap->setMaxLength(3);
	lblStudCap = new QLabel(edtStudCap, "&Stud bet cap:", pgPoker);
	grdPoker->addWidget(edtStudCap, 3, 1);
	grdPoker->addWidget(lblStudCap, 3, 0);
	
	addTab(pgPoker, "&Poker options");
	setOkButton();
	setDefaultButton();
	setCancelButton();
	connect(this, SIGNAL(applyButtonPressed()), this, SLOT(applyOptions()));
	connect(this, SIGNAL(defaultButtonPressed()), this, SLOT(resetDefaults()));
}

void SPOptWindow::applyOptions() {
	int n;
	bool optch = false;
	if (edtPlayers->edited()) {
		n = atoi(edtPlayers->text());
		if (n > 1 && n < 6) {
			conf->setPlayers(n);
			optch = true;
		}
	}
	if (edtAccounts->edited()) {
		n = atoi(edtAccounts->text());
		if (n > conf->getStudBetcap() && n > conf->getDrawBetcap()) {
			gamestats->computeWinnings();
			conf->setAccount(n);
			optch = true;
		}
	}
	if (edtDrawCap->edited()) {
		n = atoi(edtDrawCap->text());
		if (n < conf->getAccount())
			conf->setDrawBetcap(n);
	}
	if (edtStudCap->edited()) {
		n = atoi(edtStudCap->text());
		if (n < conf->getAccount())
			conf->setStudBetcap(n);
	}
	if (optch) {
		QMessageBox::information(this, "Options Changed",
			"Player options have changed;\n"
			"the poker table will be regenerated.",
			"Ok");
		createopponents();
	}
}

void SPOptWindow::resetDefaults() {
	if (edtPlayers->edited()) {
		edtPlayers->setText(QString::number(conf->getPlayers()));
	}
	if (edtAccounts->edited()) {
		edtAccounts->setText(QString::number(conf->getAccount()));
	}
	if (edtDrawCap->edited()) {
		edtDrawCap->setText(QString::number(conf->getDrawBetcap()));
	}
	if (edtStudCap->edited()) {
		edtStudCap->setText(QString::number(conf->getStudBetcap()));
	}
}

SPMainMenu::SPMainMenu(QWidget* parent)
	: QButtonGroup(4, Qt::Vertical, 0, parent) {
	main = parent;
	btnGameMenu = new QPushButton("&Games", this);
	btnStats = new QPushButton("&Statistics", this);
	btnOptions = new QPushButton("&Options", this);
	btnQuit = new QPushButton("&Quit", this);
	setFrameStyle(QFrame::WinPanel | QFrame::Raised);

	connect(this, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
}

void SPMainMenu::buttonPressed(int n) {
	switch (n) {
	case 0:
		ui->playMenu();
		break;
	case 1:
		ui->stats();
		break;
	case 2:
		ui->options();
		break;
	case 3:
		main->close();
		break;
	}
}

SPGameMenu::SPGameMenu(QWidget* parent)
	: QButtonGroup(4, Qt::Vertical, 0, parent) {
	main = parent;
	btnDraw = new QPushButton("Five &Draw", this);
	btnStudFive = new QPushButton("Five St&ud", this);
	btnStudSeven = new QPushButton("&Seven Stud", this);
	btnBack = new QPushButton("&Back", this);
	setFrameStyle(QFrame::WinPanel | QFrame::Raised);

	connect(this, SIGNAL(clicked(int)), this, SLOT(buttonPressed(int)));
}

void SPGameMenu::buttonPressed(int n) {
	switch(n) {
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		hide();
		dynamic_cast<SPWindow*>(main)->showMenu();
		break;
	}
}

#endif

