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
#include "json.h"
#include <QDir>
#include <QDirIterator>
#include <QDebug>
#include <QPixmap>

class Material {
public:
	Material(const QDir &dir,const QString path);
	~Material();
	int id;
	QString description;
	bool platform,multicolored;
	int variants,stairVariants;
	QPixmap *pixmap,*stairs;
};
class Mod {
public:
	Mod(const QDir &dir,const QString path);
	~Mod();
	int id;
	QString description;
	int variants; 
	QPixmap *pixmap;
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
			{
				Material *m=new Material(assets,it.filePath());
				materials[m->id]=m;
			}
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
			{
				Material *m=new Material(assets,it.filePath());
				materials[m->id]=m;
			}
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
			{
				Mod *m=new Mod(assets,it.filePath());
				mods[m->id]=m;
			}
		}
		assets.cdUp();
	}
}

Material::Material(const QDir &dir,const QString path)
{
	JSON json(path);
	id=json["materialId"].toNumber();
	description=json["shortdescription"].toString();
	multicolored=json["multicolored"].toBoolean();
	platform=json["platform"].toBoolean();
	if (platform)
	{
		pixmap=new QPixmap(dir.filePath(json["platformImage"].toString()));
		variants=json["platformVariants"].toNumber();
		stairs=new QPixmap(dir.filePath(json["stairImage"].toString()));
		stairVariants=json["stairVariants"].toNumber();
	}
	else
	{
		pixmap=new QPixmap(dir.filePath(json["frames"].toString()));
		variants=json["variants"].toNumber();
		stairs=NULL;
	}
}
Material::~Material()
{
	delete pixmap;
	if (stairs)
		delete stairs;
}
Mod::Mod(const QDir &dir,const QString path)
{
	JSON json(path);
	id=json["modId"].toNumber();
	description=json["Description"].toString();
	variants=json["variants"].toNumber();
	pixmap=new QPixmap(dir.filePath(json["frames"].toString()));
}
Mod::~Mod()
{
	delete pixmap;
}
