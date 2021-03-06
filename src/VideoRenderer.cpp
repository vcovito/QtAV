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

VideoRenderer::VideoRenderer()
    :AVOutput(*new VideoRendererPrivate)
{
}

VideoRenderer::VideoRenderer(VideoRendererPrivate &d)
    :AVOutput(d)
{
}

VideoRenderer::~VideoRenderer()
{
}

void VideoRenderer::scaleInQt(bool q)
{
    d_func().scale_in_qt = q;
}

bool VideoRenderer::scaleInQt() const
{
    return d_func().scale_in_qt;
}

void VideoRenderer::setSourceSize(const QSize& s)
{
    setSourceSize(s.width(), s.height());
}

void VideoRenderer::setSourceSize(int width, int height)
{
    DPTR_D(VideoRenderer);
    d.src_width = width;
    d.src_height = height;
}

QSize VideoRenderer::lastSize() const
{
    DPTR_D(const VideoRenderer);
    return QSize(d.src_width, d.src_height);
}

int VideoRenderer::lastWidth() const
{
    DPTR_D(const VideoRenderer);
    return d.src_width;
}
int VideoRenderer::lastHeight() const
{
    DPTR_D(const VideoRenderer);
    return  d.src_height;
}

void VideoRenderer::registerEventFilter(QObject *filter)
{
    DPTR_D(VideoRenderer);
    qApp->removeEventFilter(d.event_filter);
    d.event_filter = filter;
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
