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

#include "assets.h"
#include <QDir>
#include <QDirIterator>

class Material {
};
class Mod {
};

Assets::~Assets()
{
	QHashIterator<int, Material *>i(materials);
	while (i.hasNext())
	{
		i.next();
		delete i.value();
	}
	QHashIterator<int, Mod *>m(mods);
	while (m.hasNext())
	{
		m.next();
		delete m.value();
	}
}

bool Assets::verify(const QString &path)
{
	QString mats=path+QDir::toNativeSeparators("/assets/tiles/materials");
	return QFile::exists(mats);
}

void Assets::load(const QString &path)
{
	QDir assets(path);
	if (!assets.cd("assets")) return;
	if (!assets.cd("tiles")) return;
	if (assets.cd("materials"))
	{
		QDirIterator it(assets);
		while (it.hasNext())
		{
			it.next();
			if (it.fileInfo().suffix()=="material")
				parseMaterial(assets,it.filePath());
		}
		assets.cdUp();
	}
	if (assets.cd("platforms"))
	{
		QDirIterator it(assets);
		while (it.hasNext())
		{
			it.next();
			if (it.fileInfo().suffix()=="material")
				parseMaterial(assets,it.filePath());
		}
		assets.cdUp();
	}
	if (assets.cd("mods"))
	{
		QDirIterator it(assets);
		while (it.hasNext())
		{
			it.next();
			if (it.fileInfo().suffix()=="matmod")
				parseMod(assets,it.filePath());
		}
		assets.cdUp();
	}
}

void Assets::parseMaterial(QDir &dir,QString fn)
{
	JSON json(fn);
	Material *m=new Material(dir,json);
	materials[m->id]=m;
}
void Assets::parseMod(QDir &dir,QString fn)
{
	JSON json(fn);
	Mod *m=new Mod(dir,json);
	mods[m->id]=m;
}
