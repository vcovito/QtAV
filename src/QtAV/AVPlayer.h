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

#ifndef QTAV_AVPLAYER_H
#define QTAV_AVPLAYER_H

#include <QtAV/AVClock.h>
#include <QtAV/AVDemuxer.h>

namespace QtAV {

class AudioOutput;
class AudioThread;
class VideoThread;
class AudioDecoder;
class VideoDecoder;
class VideoRenderer;
class AVClock;
class AVDemuxThread;
class EventFilter;
class VideoCapture;
class Q_EXPORT AVPlayer : public QObject
{
    Q_OBJECT
public:
    explicit AVPlayer(QObject *parent = 0);
    ~AVPlayer();

    //NOT const. This is the only way to access the clock.
    AVClock* masterClock();
    void setFile(const QString& path);
	QString file() const;
    bool load(const QString& path);
    bool load();
    bool isLoaded() const;
    /*
     * default: [fmt: PNG, dir: capture, name: basename]
     * replace the existing capture; return the replaced one
     * set 0 will disable the capture
     */
    VideoCapture* setVideoCapture(VideoCapture* cap);
    VideoCapture *videoCapture();
    void setCaptureName(const QString& name);//TODO: remove. base name
    void setCaptureSaveDir(const QString& dir); //TODO: remove
    bool captureVideo();
    bool play(const QString& path);
	bool isPlaying() const;
    bool isPaused() const;
    //this will install the default EventFilter. To use customized filter, register after this
    VideoRenderer* setRenderer(VideoRenderer* renderer);
    VideoRenderer* renderer();
    AudioOutput* audio();
    void setMute(bool mute);
    bool isMute() const;

signals:
    void started();
    void stopped();

public slots:
    void pause(bool p);
    void play(); //replay
    void stop();
    void playNextFrame();
    void seek(qreal pos);
    void seekForward();
    void seekBackward();
    void updateClock(qint64 msecs); //update AVClock's external clock

protected slots:
    void resizeVideo(const QSize& size);

protected:
    bool loaded;
    AVFormatContext	*formatCtx; //changed when reading a packet
    AVCodecContext *aCodecCtx, *vCodecCtx; //set once and not change
    QString path;
    QString capture_name, capture_dir;

    //the following things are required and must be setted not null
    AVDemuxer demuxer;
    AVDemuxThread *demuxer_thread;
    AVClock *clock;
    VideoRenderer *_renderer; //list?
    AudioOutput *_audio;
    AudioDecoder *audio_dec;
    VideoDecoder *video_dec;
    AudioThread *audio_thread;
    VideoThread *video_thread;

    //tODO: (un)register api
    friend class EventFilter;
    EventFilter *event_filter;
    VideoCapture *video_capture;
};

} //namespace QtAV
#endif // QTAV_AVPLAYER_H
