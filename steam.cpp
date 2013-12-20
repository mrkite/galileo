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

#include "steam.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QString>
#include <QTextStream>

class VDF {
public:
	VDF(QStringList &lines);
	QString find(QString path);
	~VDF();
private:
	QHash<QString, VDF*> children;
	QString name;
	QString value;
};

QString Steam::installDir(const char *gamenum)
{
	//find config.vdf
	QString path=QStandardPaths::locate(QStandardPaths::GenericDataLocation,"Steam",QStandardPaths::LocateDirectory);
	path+=QDir::toNativeSeparators("/config/config.vdf");
	QFile file(path);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		QStringList lines;

		while (!in.atEnd())
			lines<<in.readLine();
		VDF vdf(lines);
		return vdf.find(QString("software/valve/steam/apps/%1/installdir").arg(gamenum));
	}
	return NULL;
}

static QRegularExpression vdfkey("\"([^\"]*)\"");
static QRegularExpression vdfopen("^\\s*{");
static QRegularExpression vdfclose("^\\s*}");

VDF::VDF(QStringList &lines)
{
	QString line=lines.takeFirst();


	QRegularExpressionMatchIterator i=vdfkey.globalMatch(line);
	if (i.hasNext())
	{
		QRegularExpressionMatch match=i.next();
		name=match.captured(1).toLower();
		if (i.hasNext()) //value
		{
			match=i.next();
			value=match.captured(1);
		}
	}
	if (vdfopen.match(lines.first()).hasMatch())
	{
		lines.removeFirst();
		while (true)
		{
			if (vdfclose.match(lines.first()).hasMatch())
			{
				lines.removeFirst();
				return;
			}
			VDF *child=new VDF(lines);
			children[child->name]=child;
		}
	}
}
VDF::~VDF()
{
	QHashIterator<QString, VDF *>i(children);
	while (i.hasNext())
	{
		i.next();
		delete i.value();
	}
}
QString VDF::find(QString path)
{
	QString key=path.section('/',0,0);
	QString value=path.section('/',1);
	if (!children[key])
		return NULL;
	if (value.isNull())
		return children[key]->value;
	return children[key]->find(value);
}
