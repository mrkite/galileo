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
#include "mapview.h"
#include "math.h"
#include <QPainter>
#include <QResizeEvent>
#include "world.h"

static const double MinZoom=1.0;
static const double MaxZoom=20.0;
static const double PanSpeed=10.0;
static const double SPanScale=2.0;
static const double CPanScale=4.0;

MapView::MapView(Assets &assets, QWidget *parent) : QWidget(parent), renderer(assets)
{
	zoom=MinZoom;
	connect(&cache, SIGNAL(sectorLoaded()),
			this, SLOT(redraw()));
	setMouseTracking(true);
	setFocusPolicy(Qt::StrongFocus);
	world=NULL;
}

QSize MapView::minimumSizeHint() const
{
	return QSize(300,300);
}
QSize MapView::sizeHint() const
{
	return QSize(400,400);
}

void MapView::setWorld(World *world)
{
	if (this->world)
		delete this->world;
	this->world=world;
	cache.clear();
	cache.setWorld(world);
	jumpToSpawn();
}
void MapView::jumpToSpawn()
{
	this->x=world->meta.spawnX;
	this->x=world->meta.spawnY;
	redraw();
}

static int lastX=-1,lastY=-1;
static bool dragging=false;
void MapView::mousePressEvent(QMouseEvent *event)
{
	lastX=event->x();
	lastY=event->y();
	dragging=true;
}
void MapView::mouseMoveEvent(QMouseEvent *event)
{
	if (!dragging)
	{
		int centerblockx=floor(this->x);
		int centerblocky=floor(this->y);

		int centerx=image.width()/2;
		int centery=image.height()/2;

		centerx-=(this->x-centerblockx)*zoom;
		centery-=(this->y-centerblocky)*zoom;

		int mx=floor(centerblockx-(centerx-event->x())/zoom);
		int my=floor(centerblocky-(centery-event->y())/zoom);

		getToolTip(mx,my);
		return;
	}
	x+=(lastX-event->x())/zoom;
	y+=(lastY-event->y())/zoom;
	lastX=event->x();
	lastY=event->y();
	redraw();
}
void MapView::mouseReleaseEvent(QMouseEvent *)
{
	dragging=false;
}
void MapView::wheelEvent(QWheelEvent *event)
{
	zoom+=floor(event->delta()/90.0);
	if (zoom<MinZoom) zoom=MinZoom;
	if (zoom>MaxZoom) zoom=MaxZoom;
	redraw();
}
void MapView::keyPressEvent(QKeyEvent *event)
{
	double speed=PanSpeed;
	if (event->modifiers()&Qt::ShiftModifier)
		speed*=SPanScale;
	if (event->modifiers()&Qt::ControlModifier)
		speed*=CPanScale;

	switch (event->key())
	{
	case Qt::Key_Up:
	case Qt::Key_W:

		y-=speed/zoom;
		redraw();
		break;
	case Qt::Key_Down:
	case Qt::Key_S:
		y+=speed/zoom;
		redraw();
		break;
	case Qt::Key_Left:
	case Qt::Key_A:
		x-=speed/zoom;
		redraw();
		break;
	case Qt::Key_Right:
	case Qt::Key_D:
		x+=speed/zoom;
		redraw();
		break;
	case Qt::Key_PageUp:
	case Qt::Key_Q:
		zoom++;
		if (zoom>MaxZoom) zoom=MaxZoom;
		redraw();
		break;
	case Qt::Key_PageDown:
	case Qt::Key_E:
		zoom--;
		if (zoom<MinZoom) zoom=MinZoom;
		redraw();
		break;
	}
}
void MapView::resizeEvent(QResizeEvent *event)
{
	image=QImage(event->size(),QImage::Format_RGB32);
	redraw();
}
void MapView::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	p.drawImage(QPoint(0,0),image);
	p.end();
}

void MapView::redraw()
{
	if (!this->isEnabled())
	{
		//blank
		uchar *bits=image.bits();
		int imgstride=image.bytesPerLine();
		int imgoffset=0;
		for (int y=0;y<image.height();y++,imgoffset+=imgstride)
			memset(bits+imgoffset,0xee,imgstride);
		update();
		return;
	}
	// otherwise draw

	uchar *bits=image.bits();
	int imgstride=image.bytesPerLine();

	quint16 sx,sy;
	sx=x/32;
	sy=y/32;

	TileSector *sector=cache.fetch(sx,sy);
	if (sector)
		renderer.render(sector,bits,imgstride,image.width(),image.height());

	update();
}

void MapView::getToolTip(int x, int y)
{
	emit hoverTextChanged(tr("X:%1 Z:%2 - Unknown")
						  .arg(x)
						  .arg(y));
}
