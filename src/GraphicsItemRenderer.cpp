/******************************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012-2013 Wang Bin <wbsecg1@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/

#include <QtAV/GraphicsItemRenderer.h>
#include <private/GraphicsItemRenderer_p.h>
#include <QGraphicsScene>
#include <QtGui/QPainter>
#include <QEvent>
#include <QKeyEvent>
#include <QtAV/EventFilter.h>
#include <QGraphicsSceneEvent>

namespace QtAV {

GraphicsItemRenderer::GraphicsItemRenderer(QGraphicsItem * parent)
    :GraphicsWidget(parent),ImageRenderer(*new GraphicsItemRendererPrivate())
{
    setFlag(ItemIsFocusable); //receive key events
    //setAcceptHoverEvents(true);
#if CONFIG_GRAPHICSWIDGET
    setFocusPolicy(Qt::ClickFocus); //for widget
#endif //CONFIG_GRAPHICSWIDGET
}

GraphicsItemRenderer::GraphicsItemRenderer(GraphicsItemRendererPrivate &d, QGraphicsItem *parent)
    :GraphicsWidget(parent),ImageRenderer(d)
{
    setFlag(ItemIsFocusable); //receive key events
    //setAcceptHoverEvents(true);
#if CONFIG_GRAPHICSWIDGET
    setFocusPolicy(Qt::ClickFocus); //for widget
#endif //CONFIG_GRAPHICSWIDGET
}

GraphicsItemRenderer::~GraphicsItemRenderer()
{
}

bool GraphicsItemRenderer::write()
{
    scene()->update(sceneBoundingRect());
    //update(); //does not cause an immediate paint. my not redraw.
	return true;
}

QRectF GraphicsItemRenderer::boundingRect() const
{
    return QRectF(0, 0, videoWidth(), videoHeight());
}

void GraphicsItemRenderer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	DPTR_D(GraphicsItemRenderer);
    if (!d.scale_in_qt) {
        d.img_mutex.lock();
    }
    if (!d.image.isNull()) {
        if (d.image.size() == QSize(d.width, d.height))
            painter->drawImage(QPointF(), d.image);
        else
            painter->drawImage(boundingRect(), d.image);
    } else if (!d.preview.isNull()){
        if (d.preview.size() == QSize(d.width, d.height))
            painter->drawImage(QPointF(), d.preview);
        else
            painter->drawImage(boundingRect(), d.preview);
    } else {
        d.preview = QImage(videoSize(), QImage::Format_RGB32);
        d.preview.fill(QColor(Qt::black));
        painter->drawImage(QPointF(), d.preview);
    }
    if (!d.scale_in_qt) {
        d.img_mutex.unlock();
    }
}
//GraphicsWidget will lose focus forever if focus out. Why?

#if CONFIG_GRAPHICSWIDGET
bool GraphicsItemRenderer::event(QEvent *event)
{
    setFocus(); //WHY: Force focus
    QEvent::Type type = event->type();
    qDebug("GraphicsItemRenderer event type = %d", type);
    if (type == QEvent::KeyPress) {
        qDebug("KeyPress Event. key=%d", static_cast<QKeyEvent*>(event)->key());
    }
    return true;
}
#else
/*simply passes event to QGraphicsWidget::event(). you should not have to
 *reimplement sceneEvent() in a subclass of QGraphicsWidget.
 */
/*
bool GraphicsItemRenderer::sceneEvent(QEvent *event)
{
    QEvent::Type type = event->type();
    qDebug("sceneEvent type = %d", type);
    if (type == QEvent::KeyPress) {
        qDebug("KeyPress Event. key=%d", static_cast<QKeyEvent*>(event)->key());
    }
    return true;
}
*/
#endif //!CONFIG_GRAPHICSWIDGET
} //namespace QtAV
