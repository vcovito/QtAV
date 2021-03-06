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

#include <QtAV/VideoThread.h>
#include <private/AVThread_p.h>
#include <QtAV/Packet.h>
#include <QtAV/AVClock.h>
#include <QtAV/VideoCapture.h>
#include <QtAV/VideoDecoder.h>
#include <QtAV/VideoRenderer.h>
#include <QtAV/ImageConverter.h>
#include <QtGui/QImage>

namespace QtAV {

const double kSyncThreshold = 0.005; // 5 ms

class VideoThreadPrivate : public AVThreadPrivate
{
public:
    VideoThreadPrivate():conv(0),delay(0),capture(0){}
    ImageConverter *conv;
    double delay;
    double pts; //current decoded pts. for capture
    //QImage image; //use QByteArray? Then must allocate a picture in ImageConverter, see VideoDecoder
    VideoCapture *capture;
};

VideoThread::VideoThread(QObject *parent) :
    AVThread(*new VideoThreadPrivate(), parent)
{
}

ImageConverter* VideoThread::setImageConverter(ImageConverter *converter)
{
    DPTR_D(VideoThread);
    QtAV::ImageConverter* old = d.conv;
    d.conv = converter;
    return old;
}

ImageConverter* VideoThread::imageConverter() const
{
    return d_func().conv;
}

double VideoThread::currentPts() const
{
    return d_func().pts;
}
//it is called in main thread usually, but is being used in video thread,
VideoCapture* VideoThread::setVideoCapture(VideoCapture *cap)
{
    qDebug("setCapture %p", cap);
    DPTR_D(VideoThread);
    QMutexLocker locker(&d.mutex);
    VideoCapture *old = d.capture;
    d.capture = cap;
    return old;
}

//TODO: if output is null or dummy, the use duration to wait
void VideoThread::run()
{
    DPTR_D(VideoThread);
    //TODO: no d.writer is ok, just a audio player
    if (!d.dec || !d.dec->isAvailable() || !d.writer)// || !d.conv)
        return;
    resetState();
    Q_ASSERT(d.clock != 0);
    VideoDecoder *dec = static_cast<VideoDecoder*>(d.dec);
    VideoRenderer* vo = static_cast<VideoRenderer*>(d.writer);
    while (!d.stop) {
        //TODO: why put it at the end of loop then playNextFrame() not work?
        if (tryPause()) { //DO NOT continue, or playNextFrame() will fail
            if (d.stop)
                break; //the queue is empty and may block. should setBlocking(false) wake up cond empty?
        }
        QMutexLocker locker(&d.mutex);
        Q_UNUSED(locker);
        if (d.packets.isEmpty() && !d.stop) {
            d.stop = d.demux_end;
            if (d.stop) {
                break;
            }
        }
        Packet pkt = d.packets.take(); //wait to dequeue
        //Compare to the clock
        if (!pkt.isValid()) {
            qDebug("Invalid packet! flush video codec context!!!!!!!!!!");
            dec->flush();
            continue;
        }
        d.delay = pkt.pts  - d.clock->value();
        /*
         *after seeking forward, a packet may be the old, v packet may be
         *the new packet, then the d.delay is very large, omit it.
         *TODO: 1. how to choose the value
         * 2. use last delay when seeking
        */
        if (qAbs(d.delay) < 2.718) {
            if (d.delay > kSyncThreshold) { //Slow down
                //d.delay_cond.wait(&d.mutex, d.delay*1000); //replay may fail. why?
                //qDebug("~~~~~wating for %f msecs", d.delay*1000);
                usleep(d.delay * 1000000);
            } else if (d.delay < -kSyncThreshold) { //Speed up. drop frame?
                //continue;
            }
        } else { //when to drop off?
            qDebug("delay %f/%f", d.delay, d.clock->value());
            if (d.delay > 0) {
                msleep(64);
            } else {
                //audio packet not cleaned up?
                continue;
            }
        }
        d.clock->updateVideoPts(pkt.pts); //here?
        //DO NOT decode and convert if vo is not available or null!
        //if ()
        bool vo_ok = vo && vo->isAvailable();
        if (vo_ok) {
            //use the last size first then update the last size so that decoder(converter) can update output size
            if (vo->lastWidth() > 0 && vo->lastHeight() > 0 && !vo->scaleInQt())
                dec->resizeVideo(vo->lastSize());
            else
                vo->setSourceSize(dec->width(), dec->height()); //setLastSize()
        }
        //still decode, we may need capture. TODO: decode only if existing a capture request if no vo
        if (dec->decode(pkt.data)) {
            d.pts = pkt.pts;
            if (d.capture) {
                d.capture->setRawImage(dec->data(), dec->width(), dec->height());
            }
            //TODO: Add filters here. Capture is also a filter
            /*if (d.image.width() != d.width || d.image.height() != d.height)
                d.image = QImage(d.width, d.height, QImage::Format_RGB32);
            d.conv->setInSize(d.width, d.height);
            if (!d.conv->convert(d.decoded_data.constData(), d.image.bits())) {
            }*/
            if (vo_ok) {
                vo->writeData(dec->data());
            }
        }
        //use the last size first then update the last size so that decoder(converter) can update output size
        if (vo_ok && !vo->scaleInQt())
            vo->setSourceSize(vo->videoSize());
    }
    qDebug("Video thread stops running...");
}

} //namespace QtAV
