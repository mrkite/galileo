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

#include "worldstructure.h"
#include "bitreader.h"

bool WorldStructure::load(BitReader &bits)
{
	bits.rv(); //length

	quint32 num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Overlay overlay;
		overlay.x=bits.rf();
		overlay.y=bits.rf();
		overlay.image=bits.rs();
		backgroundOverlays.append(overlay);
	}
	num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Overlay overlay;
		overlay.x=bits.rf();
		overlay.y=bits.rf();
		overlay.image=bits.rs();
		foregroundOverlays.append(overlay);
	}
	num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Object object;
		object.x=bits.r32();
		object.y=bits.r32();
		object.name=bits.rs();
		object.unknown=bits.r8();
		if (!object.parameters.load(bits))
			return false;
		objects.append(object);
	}
	num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Block block;
		block.x=bits.r32();
		block.y=bits.r32();
		if (!block.material.load(bits))
			return false;
		backgroundBlocks.append(block);
	}
	num=bits.rv();
	for (quint32 i=0;i<num;i++)
	{
		Block block;
		block.x=bits.r32();
		block.y=bits.r32();
		if (!block.material.load(bits))
			return false;
		foregroundBlocks.append(block);
	}
	//don't bother with the locations
	return true;
}
