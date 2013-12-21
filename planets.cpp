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

#include "planets.h"
#include "bitreader.h"
#include <QDir>
#include <QDirIterator>

void Planets::load(const QString &path)
{
	//search for universe folder
	QDir dir(path);
	bool found=false;
	if (dir.cd("universe"))
		found=true;
	if (!found && dir.cd("linux32"))
	{
		if (dir.cd("universe"))
			found=true;
		else
			dir.cdUp();
	}
	if (!found && dir.cd("linux64"))
	{
		if (dir.cd("universe"))
			found=true;
		else
			dir.cdUp();
	}
	if (!found && dir.cd("win32"))
	{
		if (dir.cd("universe"))
			found=true;
		else
			dir.cdUp();
	}

	if (!found)
		return;

	//loop through all worlds
	QDirIterator it(dir);
	while (it.hasNext())
	{
		it.next();
		if (it.fileInfo().suffix()=="world")
			planets.append(it.filePath());
	}
}
QListIterator<QString> Planets::iterator()
{
	return planets;
}
