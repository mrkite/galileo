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

#include "btdb.h"
#include "bitreader.h"

bool BTDB::open(const QString filename)
{
	if (!SBSF::open(filename))
		return false;

	QByteArray head=header();
	if (!head.startsWith("BTDB"))
		return false;

	BitReader bits(head.constData(),head.size());
	bits.skip(4); //skip BTDB
	root=bits.r64();
	rootLeaf=bits.rb();
	bits.skip(19); //seek to metadata
	bits.rv(); //meta length
	if (bits.rs()!=magic)
		return false;

	keySize=bits.r32();
	return true;
}

class Leaf
{
};

const QByteArray BTDB::get(const QByteArray key)
{
	if (!cache.contains(key))
	{
		if (rootLeaf)
			cache[key]=findLeaf(root,key);
		else
			cache[key]=findIndex(root,key);
	}
	return cache[key];
}

const QByteArray BTDB::findLeaf(quint32 blockid,const QByteArray key)
{
	//load leaf blocks
	QByteArray data;
	quint32 next=blockid;
	while (next!=0xffffffff)
	{
		QByteArray b=block(next);
		if (!b.startsWith("LL")) //not a valid block
			return NULL;
		BitReader bits(b.constData()+2,b.size()-2);
		data+=bits.read(b.size()-10);
		next=bits.r64();
	}

	BitReader bits(data.constData(),data.size());
	//parse leaf data
	quint32 numChildren=bits.r32();
	for (quint32 i=0;i<numChildren;i++)
	{
		QByteArray lkey=bits.read(keySize);
		quint32 len=bits.rv();
		if (lkey==key) //found it
			return bits.read(len);
		bits.skip(len);
	}
	return NULL;
}

const QByteArray BTDB::findIndex(quint32 blockid,const QByteArray key)
{
	QByteArray b=block(blockid);
	if (!b.startsWith("II")) //not a valid block
		return NULL;
	BitReader bits(b.constData()+2,b.size()-2);
	quint8 level=bits.r8();
	quint32 numChildren=bits.r32();
	blockid=bits.r64();
	//search for index as we parse
	for (quint32 i=0;i<numChildren;i++)
	{
		QByteArray ikey=bits.read(keySize);
		if (key<ikey)
			break;
		blockid=bits.r64();
	}
	if (level==0)
		return findLeaf(blockid,key);
	return findIndex(blockid,key);
}
