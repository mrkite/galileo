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

#include "variant.h"
#include "bitreader.h"
#include <QList>
#include <QHash>

class VariantNull : public Variant {
public:
	virtual bool isNull() const { return true; }
};
class VariantBool : public Variant {
public:
	VariantBool(bool b) : data(b) {}
	virtual bool toBoolean() const { return data; }
private:
	bool data;
};
class VariantString : public Variant {
public:
	VariantString(const QString s) : data(s) {}
	virtual const QString toString() const { return data; }
private:
	const QString data;
};
class VariantDouble : public Variant {
public:
	VariantDouble(double d) : data(d) {}
	virtual double toDouble() const { return data; }
	virtual quint32 toInt() const { return data; }
private:
	double data;
};
class VariantInt : public Variant {
public:
	VariantInt(quint32 d) : data(d) {}
	virtual quint32 toInt() const { return data; }
	virtual double toDouble() const { return data; }
private:
	quint32 data;
};
class VariantList : public Variant {
public:
	bool load(BitReader &bits);
	virtual ~VariantList();
	virtual int length() const;
	virtual const Variant &operator[](int index) const;
private:
	QList<Variant *> children;
};
class VariantMap : public Variant {
public:
	bool load(BitReader &bits);
	virtual ~VariantMap();
	virtual bool contains(const QString key) const;
	virtual const Variant &operator[](const QString key) const;
private:
	QHash<QString,Variant *> children;
};

static VariantNull Null;

static Variant *parse(BitReader &bits)
{
	quint8 type=bits.r8();
	switch (type)
	{
		case 1: return new VariantNull();
		case 2: return new VariantDouble(bits.rd());
		case 3: return new VariantBool(bits.rb());
		case 4: return new VariantInt(bits.rv());
		case 5: return new VariantString(bits.rs());
		case 6:
			{
				VariantList *l=new VariantList();
				if (!l->load(bits))
				{
					delete l;
					l=NULL;
				}
				return l;
			}
		case 7:
			{
				VariantMap *m=new VariantMap();
				if (!m->load(bits))
				{
					delete m;
					m=NULL;
				}
				return m;
			}
		default: return NULL;
	}
}

bool VariantList::load(BitReader &bits)
{
	quint32 num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Variant *child=parse(bits);
		if (!child) //error: child failed to parse
			return false;
		children.append(child);
	}
	return true;
}
VariantList::~VariantList()
{
	QListIterator<Variant *>i(children);
	while (i.hasNext())
		delete i.next();
}
int VariantList::length() const
{
	return children.length();
}
const Variant &VariantList::operator[](int index) const
{
	if (index<children.length())
		return *children[index];
	return Null;
}

bool VariantMap::load(BitReader &bits)
{
	quint32 num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		QString key=bits.rs();
		if (key.length()==0) //error: empty key
			return false;
		Variant *child=parse(bits);
		if (!child) //error: child failed to parse
			return false;
		children.insert(key,child);
	}
	return true;
}
VariantMap::~VariantMap()
{
	QHashIterator<QString,Variant*>i(children);
	while (i.hasNext())
	{
		i.next();
		delete i.value();
	}
}
bool VariantMap::contains(const QString key) const
{
	return children.contains(key);
}
const Variant &VariantMap::operator[](const QString key) const
{
	if (children.contains(key))
		return *children[key];
	return Null;
}

Variant::Variant()
{
	root=NULL;
}
Variant::~Variant()
{
	if (root)
		delete root;
}
bool Variant::load(BitReader &bits)
{
	root=parse(bits);
	return root!=NULL;
}
bool Variant::isNull() const
{
	if (root)
		return root->isNull();
	return false;
}
bool Variant::contains(const QString key) const
{
	if (root)
		return root->contains(key);
	return false;
}
const Variant &Variant::operator[](const QString key) const
{
	if (root)
		return root->operator[](key);
	return Null;
}
const Variant &Variant::operator[](int i) const
{
	if (root)
		return root->operator[](i);
	return Null;
}
int Variant::length() const
{
	if (root)
		return root->length();
	return 0;
}
const QString Variant::toString() const
{
	if (root)
		return root->toString();
	return NULL;
}
double Variant::toDouble() const
{
	if (root)
		return root->toDouble();
	return 0.0;
}
quint32 Variant::toInt() const
{
	if (root)
		return root->toInt();
	return 0;
}
bool Variant::toBoolean() const
{
	if (root)
		return root->toBoolean();
	return false;
}
