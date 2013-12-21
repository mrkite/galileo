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

#include "players.h"
#include "bitreader.h"
#include "worldcoordinate.h"
#include <QDir>
#include <QDirIterator>
#include <QDebug>

class ClientContext : SBV
{
public:
	ClientContext() : SBV("SBCCV1") {}
	bool open(const QString fn);
	QString home,current;
};

Players::~Players()
{
	QListIterator<Player *>i(players);
	while (i.hasNext())
		delete i.next();
}

void Players::load(const QString &path)
{
	//search for player folder
	QDir dir(path);
	bool found=false;
	if (dir.cd("player"))
		found=true;
	else
	{
		if (dir.cd("linux32"))
		{
			if (dir.cd("player"))
				found=true;
			else
				dir.cdUp();
		}
		if (!found && dir.cd("linux64"))
		{
			if (dir.cd("player"))
				found=true;
			else
				dir.cdUp();
		}
		if (!found && dir.cd("win32"))
		{
			if (dir.cd("player"))
				found=true;
			else
				dir.cdUp();
		}
	}
	if (!found)
		return;
	//loop through all the players
	QDirIterator it(dir);
	while (it.hasNext())
	{
		it.next();
		if (it.fileInfo().suffix()=="player")
		{
			Player *p=new Player();
			if (p->open(it.filePath()))
			{
				p->ship=dir.filePath(p->uuid+".shipword");
				players.append(p);
			}
			else
				delete p;
		}
	}
	//now loop through all the client contexts
	dir.cdUp();
	if (dir.cd("universe"))
	{
		QListIterator<Player *>i(players);
		while (i.hasNext())
		{
			Player *p=i.next();
			ClientContext cc;
			if (cc.open(dir.filePath(p->uuid+".clientcontext")))
			{
				p->home=dir.filePath(cc.home);
				p->current=dir.filePath(cc.current);
			}
		}
	}
}
QListIterator<Player *> Players::iterator()
{
	return players;
}

bool Player::open(const QString fn)
{
	if (!SBV::open(fn))
		return false;

	BitReader bits(data.constData(),data.size());
	bool hasUUID=bits.rb();
	if (hasUUID)
	{
		for (int i=0;i<16;i++)
			uuid+=QString("%1").arg(bits.r8(),2,16,QChar('0'));
	}
	name=bits.rs();
	//we don't care about the rest of the identity
	return true;
}

bool ClientContext::open(const QString fn)
{
	if (!SBV::open(fn))
		return false;

	BitReader bits(data.constData(),data.size());
	bits.rb(); //isAdmin
	bits.rv(); //teamType
	bits.rv(); //team
	bits.rv(); //len of rest of context
	quint32 numBookmarks=bits.rv();
	for (quint32 i=0;i<numBookmarks;i++)
	{
		bits.rs(); //sector
		bits.r32(); //x
		bits.r32(); //y
		bits.r32(); //z
		bits.rs(); //name
	}
	quint32 numSectors=bits.rv();
	for (quint32 i=0;i<numSectors;i++)
	{
		bits.rs(); //name
		bits.rb(); //charted
	}
	quint32 numSystems=bits.rv();
	for (quint32 i=0;i<numSystems;i++)
	{
		bits.rs(); //sector
		bits.r32(); //x
		bits.r32(); //y
		bits.r32(); //z
	}
	WorldCoordinate c(bits);
	current=c.filename;
	WorldCoordinate h(bits);
	home=h.filename;
	return true;
}
