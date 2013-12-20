/*
Copyright (c) 2013, Sean Kasun
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "galileo.h"
#include "mapview.h"
#include "steam.h"
#include <QApplication>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QAction>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QMessageBox>
#include <QDebug>

Galileo::Galileo(QString sbpath) : sbpath(sbpath)
{
	mapview = new MapView;
	connect(mapview,SIGNAL(hoverTextChanged(QString)),
			statusBar(),SLOT(showMessage(QString)));
	connect(this, SIGNAL(worldLoaded(bool)),
			mapview, SLOT(setEnabled(bool)));

	createActions();
	createMenus();
	createStatusBar();

	setCentralWidget(mapview);
	layout()->setContentsMargins(0,0,0,0);

	setWindowTitle(qApp->applicationName());

	emit worldLoaded(false);

	assets.load(sbpath);
}

void Galileo::createActions()
{

	exitAct=new QAction(tr("E&xit"),this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	exitAct->setStatusTip(tr("Exit %1").arg(qApp->applicationName()));
	connect(exitAct, SIGNAL(triggered()),
			this, SLOT(close()));

	jumpSpawnAct=new QAction(tr("Jump to &Spawn"),this);
	jumpSpawnAct->setShortcut(tr("F1"));
	jumpSpawnAct->setStatusTip(tr("Jump to world spawn"));
	connect(jumpSpawnAct, SIGNAL(triggered()),
			this, SLOT(jumpToSpawn()));
	connect(this, SIGNAL(worldLoaded(bool)),
			jumpSpawnAct, SLOT(setEnabled(bool)));

	aboutAct=new QAction(tr("&About"),this);
	aboutAct->setStatusTip(tr("About %1").arg(qApp->applicationName()));
	connect(aboutAct, SIGNAL(triggered()),
			this, SLOT(about()));
}
void Galileo::createMenus()
{
	fileMenu=menuBar()->addMenu(tr("&File"));
	planetMenu=fileMenu->addMenu(tr("&Open Planet"));
	planetMenu->setEnabled(false);
	playerMenu=fileMenu->addMenu(tr("Players"));
	playerMenu->setEnabled(false);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);

	viewMenu=menuBar()->addMenu(tr("&View"));
	viewMenu->addAction(jumpSpawnAct);

	menuBar()->addSeparator();
	helpMenu=menuBar()->addMenu(tr("&Help"));
	helpMenu->addAction(aboutAct);
}
void Galileo::createStatusBar()
{
	statusBar()->showMessage(tr("Ready"));
}


void Galileo::jumpToSpawn()
{
//	mapview->setLocation(spawnx,spawny);
}

void Galileo::about()
{
	QMessageBox::about(this,tr("About %1").arg(qApp->applicationName()),
					   tr("<b>%1</b> v%2<br/>\n"
						  "&copy; Copyright %3, %4").arg(qApp->applicationName())
					   .arg(qApp->applicationVersion())
					   .arg(2013)
					   .arg(qApp->organizationName()));
}
