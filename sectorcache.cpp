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

#include "sectorcache.h"
#include "world.h"
#include "sectorloader.h"

#include <QThreadPool>

SectorCache::SectorCache()
{
	//each sector is 32x32 tiles.  1440p is 2560x1440.  So to fill a
	//1440p screen with 1 pixel = 1 tile, we only need 3600 sectors
	//let's cache double that to be on the safe side
	cache.setMaxCost(7200);
}
void SectorCache::clear()
{
	QThreadPool::globalInstance()->waitForDone(); //wait for any loading sectors
	mutex.lock();
	cache.clear();
	mutex.unlock();
}
void SectorCache::setWorld(World *world)
{
	this->world=world;
}

TileSector *SectorCache::fetch(quint16 x,quint16 y)
{
	quint32 id=x<<16|y;
	mutex.lock();
	TileSector *sector=cache[id];
	mutex.unlock();
	if (sector!=NULL)
	{
		if (sector->loaded)
			return sector;
		return NULL; //we're still loading this sector
	}
	//launch background process to load this sector
	sector=new TileSector();
	mutex.lock();
	cache.insert(id,sector);
	mutex.unlock();
	SectorLoader *loader=new SectorLoader(world,x,y,cache,mutex);
	connect(loader,SIGNAL(loaded(quint16,quint16)),
			this,SLOT(gotSector(quint16,quint16)));
	QThreadPool::globalInstance()->start(loader);
	return NULL;
}
void SectorCache::gotSector(quint16,quint16)
{
	emit sectorLoaded();
}
