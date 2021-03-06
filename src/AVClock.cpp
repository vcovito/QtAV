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


#include <QtAV/AVClock.h>

namespace QtAV {

AVClock::AVClock(AVClock::ClockType c, QObject *parent)
    :QObject(parent),auto_clock(true),clock_type(c)
{
    pts_ = pts_v = delay_ = 0;
}

AVClock::AVClock(QObject *parent)
    :QObject(parent),auto_clock(true),clock_type(AudioClock)
{
    pts_ = pts_v = delay_ = 0;
}

void AVClock::setClockType(ClockType ct)
{
    clock_type = ct;
}

AVClock::ClockType AVClock::clockType() const
{
    return clock_type;
}

bool AVClock::isActive() const
{
    return clock_type == AudioClock || timer.isValid();
}

void AVClock::setClockAuto(bool a)
{
    auto_clock = a;
}

bool AVClock::isClockAuto() const
{
    return auto_clock;
}

void AVClock::updateExternalClock(qint64 msecs)
{
    if (clock_type != ExternalClock)
        return;
    qDebug("External clock change: %f ==> %f", value(), double(msecs) * kThousandth);
    pts_ = double(msecs) * kThousandth; //can not use msec/1000.
    timer.restart();
}

void AVClock::start()
{
    qDebug("AVClock started!!!!!!!!");
    timer.start();
    emit started();
}

void AVClock::pause(bool p)
{
    if (clock_type != ExternalClock)
        return;
    if (p) {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        timer.invalidate();
#else
        timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        emit paused();
    } else {
        timer.start();
        emit resumed();
    }
    emit paused(p);
}

void AVClock::reset()
{
    pts_ = pts_v = delay_ = 0;
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    timer.invalidate();
#else
    timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    emit resetted();
}


} //namespace QtAV
