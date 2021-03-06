/******************************************************************************
    VideoWall:  this file is part of QtAV examples
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

#include "VideoWall.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QEvent>
#include <QFileDialog>
#include <QKeyEvent>
#include <QtAV/AudioOutput.h>

using namespace QtAV;

VideoWall::VideoWall(QObject *parent) :
    QObject(parent),r(3),c(3)
{
}

VideoWall::~VideoWall()
{
    if (!players.isEmpty()) {
        foreach (AVPlayer *player, players) {
            player->stop();
            WidgetRenderer* renderer = static_cast<WidgetRenderer*>(player->renderer());
            if (renderer) {
                renderer->QWidget::close(); //TODO: rename
                if (!renderer->testAttribute(Qt::WA_DeleteOnClose))
                    delete renderer;
                delete player;
            }
        }
        players.clear();
    }
}

void VideoWall::setRows(int n)
{
    r = n;
}

void VideoWall::setCols(int n)
{
    c = n;
}

int VideoWall::rows() const
{
    return r;
}

int VideoWall::cols() const
{
    return c;
}

void VideoWall::show()
{
    if (!players.isEmpty()) {
        foreach (AVPlayer *player, players) {
            player->stop();
            WidgetRenderer* renderer = static_cast<WidgetRenderer*>(player->renderer());
            if (renderer) {
                renderer->QWidget::close(); //TODO: rename
                if (!renderer->testAttribute(Qt::WA_DeleteOnClose))
                    delete renderer;
                delete player;
            }
        }
        players.clear();
    }
    qDebug("show wall: %d x %d", r, c);

    int w = qApp->desktop()->width()/c;
    int h = qApp->desktop()->height()/r;
    for (int i = 0; i < r; ++i) {
        for (int j = 0; j < c; ++j) {
            WidgetRenderer* renderer = new WidgetRenderer;
            renderer->setWindowFlags(Qt::FramelessWindowHint);
            renderer->resize(w, h);
            renderer->move(j*w, i*h);
            renderer->show();
            AVPlayer *player = new AVPlayer;
            player->setRenderer(renderer);
            renderer->registerEventFilter(this);
            players.append(player);
        }
    }
}

void VideoWall::play(const QString &file)
{
    if (players.isEmpty())
        return;
    foreach (AVPlayer *player, players) {
        player->play(file);
    }
}


bool VideoWall::eventFilter(QObject *watched, QEvent *event)
{
    //qDebug("EventFilter::eventFilter to %p", watched);
    Q_UNUSED(watched);
    if (players.isEmpty())
        return false;
    QEvent::Type type = event->type();
    switch (type) {
    case QEvent::KeyPress: {
        //qDebug("Event target = %p %p", watched, player->renderer);
        //avoid receive an event multiple times
        int key = static_cast<QKeyEvent*>(event)->key();
        switch (key) {
        case Qt::Key_N: //check playing?
            foreach (AVPlayer* player, players) {
                player->playNextFrame();
            }
            break;
        case Qt::Key_P:
            foreach (AVPlayer* player, players) {
                player->play();
            }
            break;
        case Qt::Key_S:
            foreach (AVPlayer* player, players) {
                player->stop(); //check playing?
            }
            break;
        case Qt::Key_Space: //check playing?
            foreach (AVPlayer* player, players) {
                player->pause(!player->isPaused());
            }
            break;
        case Qt::Key_Up:
            foreach (AVPlayer* player, players) {
                if (player->audio()) {
                    qreal v = player->audio()->volume();
                    if (v > 0.5)
                        v += 0.1;
                    else if (v > 0.1)
                        v += 0.05;
                    else
                        v += 0.025;
                    player->audio()->setVolume(v);
                }
            }
            break;
        case Qt::Key_Down:
            foreach (AVPlayer* player, players) {
                if (player->audio()) {
                    qreal v = player->audio()->volume();
                    if (v > 0.5)
                        v -= 0.1;
                    else if (v > 0.1)
                        v -= 0.05;
                    else
                        v -= 0.025;
                    player->audio()->setVolume(v);
                }
            }


            break;
        case Qt::Key_O:
        {
            //TODO: emit a signal so we can use custome dialogs
            QString file = QFileDialog::getOpenFileName(0, tr("Open a video"));
            if (!file.isEmpty()) {
                foreach (AVPlayer* player, players) {
                    qDebug("%p", player);
                    player->play(file);
                }
            }
        }
            break;
        case Qt::Key_Left:
            qDebug("<-");
            foreach (AVPlayer* player, players) {
                player->seekBackward();
            }
            break;
        case Qt::Key_Right:
            qDebug("->");
            foreach (AVPlayer* player, players) {
                player->seekForward();
            }
            break;
        case Qt::Key_M:
            foreach (AVPlayer* player, players) {
                if (player->audio()) {
                    player->audio()->setMute(!player->audio()->isMute());
                }
            }
            break;
        default:
            return false;
        }
        break;
    }
    default:
        return false;
    }
    return true; //false: for text input
}
