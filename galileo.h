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

#ifndef __GALILEO_H__
#define __GALILEO_H__

#include <QtWidgets/QMainWindow>
#include "assets.h"
#include "players.h"
#include "planets.h"

class QAction;
class QMenu;
class MapView;
class Steam;

class Galileo : public QMainWindow
{
	Q_OBJECT
public:
	Galileo();
	void load(const QString sbpath);
private slots:
	void jumpToSpawn();
	void about();
	void openPlanet();
signals:
	void worldLoaded(bool isLoaded);
private:
	void createActions();
	void createMenus();
	void createStatusBar();
	void verifyPath();
	void loadAssets(const QString sbpath);
	void populateSubMenus();
	QAction *createPlanetItem(const QString name,const QString data); 

	QMenu *fileMenu,*planetMenu,*playerMenu,*viewMenu,*helpMenu;
	QList<QMenu *> playerMenus;
	QAction *exitAct,*jumpSpawnAct,*aboutAct;

	MapView *mapview;
	Assets assets;
	Players players;
	Planets planets;
};

#endif
