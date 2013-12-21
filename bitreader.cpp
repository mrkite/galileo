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

#include "bitreader.h"

BitReader::BitReader(const char *data,size_t len)
{
	this->data=(const quint8 *)data;
	this->ptr=this->data;
	this->end=this->ptr+len;
}

bool BitReader::rb()
{
	return *ptr++;
}

quint8 BitReader::r8()
{
	return *ptr++;
}
quint16 BitReader::r16()
{
	quint16 r=(ptr[0]<<16)|ptr[1];
	ptr+=2;
	return r;
}
quint32 BitReader::r32()
{
	quint32 r=(ptr[0]<<24)|(ptr[1]<<16)|(ptr[2]<<8)|ptr[3];
	ptr+=4;
	return r;
}
quint64 BitReader::r64()
{
	quint64 r=0;
	for (int i=0;i<8;i++)
	{
		r<<=8;
		r|=ptr[i];
	}
	ptr+=8;
	return r;
}
quint32 BitReader::rv()
{
	quint32 r=0;
	quint8 b;
	do {
		r<<=7;
		b=*ptr++;
		r|=b&0x7f;
	} while (b&0x80);
	return r;
}
float BitReader::rf()
{
	union {
		quint32 d;
		float f;
	} df;
	df.d=r32();
	return df.f;
}
double BitReader::rd()
{
	union {
		quint64 d;
		double f;
	} df;
	df.d=r64();
	return df.d;
}
const QString BitReader::rs()
{
	quint32 len=rv();
	QByteArray r((const char *)ptr,len);
	ptr+=len;
	return r;
}
void BitReader::skip(size_t len)
{
	ptr+=len;
}
const QByteArray BitReader::read(size_t len)
{
	QByteArray r((const char *)ptr,len);
	ptr+=len;
	return r;
}
