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

#include "world.h"
#include "worldmeta.h"
#include <zlib.h>
#include <QDebug>

bool World::open(const QString filename)
{
	if (!BTDB::open(filename))
		return false;

	QByteArray key(5,0);  //key 0 = meta
	if (!meta.load(get(key)))
		return false;
	return true;
}

QByteArray World::sector(quint16 x,quint16 y)
{
	QByteArray key(5,0);
	key[0]=1;
	key[1]=x>>8;
	key[2]=x&0xff;
	key[3]=y>>8;
	key[4]=y&0xff;
	QByteArray data=get(key);
	QByteArray sector;
	static const int CHUNK_SIZE = 0x4000;
	z_stream strm;
	strm.zalloc=Z_NULL;
	strm.zfree=Z_NULL;
	strm.opaque=Z_NULL;
	strm.avail_in=data.size();
	strm.next_in=(Bytef*)data.constData();

	inflateInit(&strm);
	char out[CHUNK_SIZE];
	do {
		strm.avail_out=CHUNK_SIZE;
		strm.next_out=(Bytef*)out;
		inflate(&strm,Z_NO_FLUSH);
		sector.append(out,CHUNK_SIZE-strm.avail_out);
	} while (strm.avail_out==0);
	inflateEnd(&strm);

	return sector;
}
