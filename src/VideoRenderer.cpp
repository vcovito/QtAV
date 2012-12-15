/******************************************************************************
    QtAV:  Media play library based on Qt and FFmpeg
    Copyright (C) 2012 Wang Bin <wbsecg1@gmail.com>

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

#include <QtAV/VideoRenderer.h>
#include <QtAV/VideoDecoder.h>
#include <private/VideoRenderer_p.h>
#include <QtCore/QCoreApplication>
#include <QtAV/EventFilter.h>
#include <QtAV/ImageConvert.h>
/*!
    EZX:
    PIX_FMT_BGR565, 16bpp,
    PIX_FMT_RGB18,  18bpp,
*/

namespace QtAV {

class RenderEvent : public QEvent
{
public:
    RenderEvent(const QByteArray& data, int width, int height)
        :QEvent(registeredType()),w(width),h(height),decoded_data(data)
    {}

    int width() const { return w; }
    int height() const { return h;}
    QByteArray decodedData() const { return decoded_data; }

    static Type registeredType()
    {
        static Type EventType = static_cast<Type>(registerEventType());
        return EventType;
    }

private:
    int w, h;
    QByteArray decoded_data;
};

RendererHelper::RendererHelper(QObject* parent)
    :QObject(parent),renderer(0)
{
    /*
    if (thread() != qApp->thread()) {
        moveToThread(qApp->thread());
    }
    */
}

void RendererHelper::setRenderer(VideoRenderer* renderer)
{
    this->renderer = renderer;
}

void RendererHelper::scheduleWrite(const QByteArray& data, int width, int height)
{
    qApp->postEvent(this, new RenderEvent(data, width, height));
}

bool RendererHelper::event(QEvent* e)
{
    if (e->type() == RenderEvent::registeredType()) {
        RenderEvent *event = static_cast<RenderEvent*>(e);
        event->accept();
        //all in the same thread(main) en sure size is right to QImage:
        //scale to renderer size=>write to renderer
        renderer->setSourceSize(event->width(), event->height());
        renderer->write(event->decodedData());
        return true;
   }
   return false;
}


VideoRenderer::VideoRenderer()
    :AVOutput(*new VideoRendererPrivate)
{
    d_func().helper->setRenderer(this);
}

VideoRenderer::VideoRenderer(VideoRendererPrivate &d)
    :AVOutput(d)
{
    d_func().helper->setRenderer(this);
}

VideoRenderer::~VideoRenderer()
{
}

void VideoRenderer::setSourceSize(int width, int height)
{
    DPTR_D(VideoRenderer);
    d.src_width = width;
    d.src_height = height;
}

void VideoRenderer::scheduleWrite(const QByteArray &data, int width, int height)
{
    d_func().helper->scheduleWrite(data, width, height);
}

void VideoRenderer::registerEventFilter(EventFilter *filter)
{
    d_func().event_filter = filter;
    qApp->installEventFilter(filter);
}

bool VideoRenderer::open()
{
    return true;
}

bool VideoRenderer::close()
{
    return true;
}

void VideoRenderer::resizeVideo(const QSize &size)
{
    resizeVideo(size.width(), size.height());
}

void VideoRenderer::resizeVideo(int width, int height)
{
    DPTR_D(VideoRenderer);
    if (width == 0 || height == 0)
        return;
#if SCALE_VIDEO_THREAD
    if (d.dec) {
        //static_cast<VideoDecoder*>(d.dec)->resizeVideo(width, height);
    }
#endif //SCALE_VIDEO_THREAD
    d.width = width;
    d.height = height;
}

QSize VideoRenderer::videoSize() const
{
    return QSize(d_func().width, d_func().height);
}

int VideoRenderer::videoWidth() const
{
    return d_func().width;
}

int VideoRenderer::videoHeight() const
{
    return d_func().height;
}

} //namespace QtAV
