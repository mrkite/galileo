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

#include <QFile>
#include <QList>
#include <QHash>
#include "json.h"

enum Token {
	TokenNULL,
	TokenTRUE,
	TokenFALSE,
	TokenString,
	TokenNumber,
	TokenObject,
	TokenArray,
	TokenObjectClose,
	TokenArrayClose,
	TokenKeySeparator,
	TokenValueSeparator,
	TokenEOF,
	TokenInvalid
};

class JSONHelper
{
public:
	JSONHelper(QString data) : data(data) { pos=0;len=data.length();}
	Token peekToken()
	{
		int oldpos=pos;
		Token t=nextToken();
		pos=oldpos;
		return t;
	}
	Token nextToken()
	{
		while (pos<len && data.at(pos).isSpace()) pos++;
		if (pos==len) return TokenEOF;
		QChar c=data.at(pos++);
		if (c.isLetter()) //keyword like NULL, TRUE, or FALSE
		{
			int start=pos-1;
			while (pos<len && data.at(pos).isLetter())
				pos++;
			QStringRef ref=data.midRef(start,pos-start);
			if (ref.compare("null",Qt::CaseInsensitive)==0)
				return TokenNULL;
			if (ref.compare("true",Qt::CaseInsensitive)==0)
				return TokenTRUE;
			if (ref.compare("false",Qt::CaseInsensitive)==0)
				return TokenFALSE;
			return TokenInvalid;
		}
		if (c.isDigit() || c=='-') //double or hex
		{
			pos--;
			return TokenNumber;
		}
		switch (c.unicode())
		{
		case '"': return TokenString;
		case '{': return TokenObject;
		case '}': return TokenObjectClose;
		case '[': return TokenArray;
		case ']': return TokenArrayClose;
		case ':': return TokenKeySeparator;
		case ',': return TokenValueSeparator;
		default: return TokenInvalid;
		}
	}
	QString readString()
	{
		QString r;
		while (pos<len && data.at(pos)!='"')
		{
			if (data.at(pos)=='\\')
			{
				pos++; if (pos==len) return r;
				switch (data.at(pos++).unicode())
				{
				case '"': r+='"'; break;
				case '\\': r+='\\'; break;
				case '/': r+='/'; break;
				case 'b': r+='\b'; break;
				case 'f': r+='\f'; break;
				case 'n': r+='\n'; break;
				case 'r': r+='\r'; break;
				case 't': r+='\t'; break;
				case 'u': //hex
						  {
							  int num=0;
							  for (int i=0;i<4;i++)
							  {
								  if (pos==len)
									  return r;
								  num<<=4;
								  char c=data.at(pos++).unicode();
								  if (c>='0' && c<='9')
									  num|=c-'0';
								  else if (c>='a' && c<='f')
									  num|=c-'a'+10;
								  else if (c>='A' && c<='F')
									  num|=c-'A'+10;
								  else
									  return r;
							  }
							  r+=QChar(num);
						  }
						  break;
				default: return r;
				}
			}
			else
				r+=data.at(pos++);
		}
		pos++;
		return r;
	}
	double readDouble()
	{
		double sign=1.0;
		if (data.at(pos)=='-')
		{
			sign=-1.0;
			pos++;
		}
		else if (data.at(pos)=='+')
			pos++;
		double value=0.0;
		while (pos<len && data.at(pos).isDigit())
		{
			value*=10.0;
			value+=data.at(pos++).unicode()-'0';
		}
		if (pos==len) return sign*value;
		if (data.at(pos)=='.')
		{
			double pow10=10.0;
			pos++;
			while (pos<len && data.at(pos).isDigit())
			{
				value+=(data.at(pos++).unicode()-'0')/pow10;
				pow10*=10.0;
			}
		}
		if (pos==len) return sign*value;
		double scale=1.0;
		bool frac=false;
		if (data.at(pos)=='e' || data.at(pos)=='E')
		{
			pos++;
			if (pos==len) return sign*value;
			if (data.at(pos)=='-')
			{
				frac=true;
				pos++;
			}
			else if (data.at(pos)=='+')
				pos++;
			unsigned int expon=0;
			while (pos<len && data.at(pos).isDigit())
			{
				expon*=10.0;
				expon+=data.at(pos++).unicode()-'0';
			}
			if (expon>308) expon=308;
			while (expon>=50) { scale *= 1E50; expon-=50; }
			while (expon>=8) { scale *= 1E8; expon-=8; }
			while (expon>0) { scale *= 10.0; expon--; }
		}
		return sign*(frac?(value/scale):(value*scale));
	}
private:
	int pos,len;
	QString data;
};

class JSONNull : public JSON {
public:
	virtual bool isNull() const { return true; }
};
class JSONBool : public JSON {
public:
	JSONBool(bool b) : data(b) {}
	virtual bool toBoolean() const { return data; }
private:
	bool data;
};
class JSONString : public JSON {
public:
	JSONString(const QString s) : data(s) {}
	virtual const QString toString() const { return data; }
private:
	const QString data;
};
class JSONNumber : public JSON {
public:
	JSONNumber(double d) : data(d) {}
	virtual double toNumber() const { return data; }
private:
	double data;
};
class JSONObject : public JSON {
public:
	bool load(JSONHelper &reader);
	virtual ~JSONObject();
	virtual bool contains(const QString key) const;
	virtual const JSON &operator[](const QString key) const;
private:
	QHash<QString,JSON *> children;
};
class JSONArray : public JSON {
public:
	bool load(JSONHelper &reader);
	virtual ~JSONArray();
	virtual int length() const;
	virtual const JSON &operator[](int index) const;
private:
	QList<JSON *> children;
};
static JSONNull Null;

static JSON *parse(JSONHelper &reader)
{
	Token token=reader.nextToken();
	switch (token)
	{
		case TokenNULL: return new JSONNull();
		case TokenTRUE: return new JSONBool(true);
		case TokenFALSE: return new JSONBool(false);
		case TokenString: return new JSONString(reader.readString());
		case TokenNumber: return new JSONNumber(reader.readDouble());
		case TokenObject:
			{
				JSONObject *o=new JSONObject();
				if (!o->load(reader))
				{
					delete o;
					o=NULL;
				}
				return o;
			}
		case TokenArray:
			{
				JSONArray *a=new JSONArray();
				if (!a->load(reader))
				{
					delete a;
					a=NULL;
				}
				return a;
			}
		default: return NULL;
	}
	return NULL;
}

bool JSONObject::load(JSONHelper &reader)
{
	while (true)
	{
		Token token=reader.nextToken();
		if (token==TokenObjectClose)
			break;
		if (token!=TokenString) //error: object doesn't start with key
			return false;
		QString key=reader.readString();
		if (key.length()==0) //error: empty key
			return false;
		if (reader.nextToken()!=TokenKeySeparator) //error: no colon after key
			return false;
		JSON *child=parse(reader);
		if (!child)  //error: child failed to parse
			return false;
		children.insert(key,child);
		token=reader.nextToken();
		if (token==TokenObjectClose)
			break;
		if (token!=TokenValueSeparator) //error: no comma
			return false;
	}
	return true;
}
JSONObject::~JSONObject()
{
	QHashIterator<QString,JSON*>i(children);
	while (i.hasNext())
	{
		i.next();
		delete i.value();
	}
}
bool JSONObject::contains(const QString key) const
{
	return children.contains(key);
}
const JSON &JSONObject::operator[](const QString key) const
{
	if (children.contains(key))
		return *children[key];
	return Null;
}

bool JSONArray::load(JSONHelper &reader)
{
	Token token=reader.peekToken();
	if (token==TokenArrayClose)
	{
		reader.nextToken();
		return true;
	}
	while (true)
	{
		JSON *child=parse(reader);
		if (!child)	//error: child failed to parse
			return false;
		children.append(child);
		token=reader.nextToken();
		if (token==TokenArrayClose)
			break;
		if (token!=TokenValueSeparator) //error: no comma
			return false;
	}
	return true;
}
JSONArray::~JSONArray()
{
	QListIterator<JSON *>i(children);
	while (i.hasNext())
		delete i.next();
}
int JSONArray::length() const
{
	return children.length();
}
const JSON &JSONArray::operator[](int index) const
{
	if (index<children.length())
		return *children[index];
	return Null;
}


JSON::JSON()
{
	root=NULL;
}
JSON::~JSON()
{
	if (root)
		delete root;
}
bool JSON::open(const QString filename)
{
	QFile f(filename);
	f.open(QIODevice::ReadOnly);
	JSONHelper reader(f.readAll());
	f.close();

	root=parse(reader);
	return root!=NULL;
}
bool JSON::isNull() const
{
	if (root)
		return root->isNull();
	return false;
}
bool JSON::contains(const QString key) const
{
	if (root)
		return root->contains(key);
	return false;
}
const JSON &JSON::operator[](const QString key) const
{
	if (root)
		return root->operator[](key);
	return Null;
}
const JSON &JSON::operator[](int i) const
{
	if (root)
		return root->operator[](i);
	return Null;
}
int JSON::length() const
{
	if (root)
		return root->length();
	return 0;
}
const QString JSON::toString() const
{
	if (root)
		return root->toString();
	return NULL;
}
double JSON::toNumber() const
{
	if (root)
		return root->toNumber();
	return 0.0;
}
bool JSON::toBoolean() const
{
	if (root)
		return root->toBoolean();
	return false;
}
