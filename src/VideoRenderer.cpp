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

/*!
    EZX:
    PIX_FMT_BGR565, 16bpp,
    PIX_FMT_RGB18,  18bpp,
*/

namespace QtAV {

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
        qApp->postEvent(this, new QEvent(QEvent::User));
    }

    bool RendererHelper::event(QEvent* e)
    {
        if (e->type() == QEvent::User) {
            e->accept();
            //TODO: SWScale, new event class
            //renderer->write(
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
    if (d.dec) {
        static_cast<VideoDecoder*>(d.dec)->resizeVideo(width, height);
    }
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
