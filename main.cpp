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
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

#include "galileo.h"
#include "steam.h"

int main(int argc,char *argv[])
{
	QApplication app(argc,argv);

	QString locale = QLocale::system().name();

	QTranslator translator;
	translator.load(QString("galileo_")+locale);
	app.installTranslator(&translator);

	app.setApplicationName("Galileo");
	app.setApplicationVersion("0.1");
	app.setOrganizationName("seancode");

	QString path=Steam::installDir("211820");
	if (path.isNull() || !Assets::verify(path))
	{
		//prompt for valid path
		qDebug()<<"couldn't find starbound";
		app.exit();
		return 0;
	}

	Galileo galileo(path);
	galileo.show();
	return app.exec();
}
