#include "videocutscenewidget.h"
#include <QVBoxLayout>
#include <QUrl>
#include <QPainter>

VideoCutsceneWidget::VideoCutsceneWidget(QWidget *parent)
    : QWidget(parent)
{
    videoWidget = new QVideoWidget(this);
    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    audioOutput->setVolume(1.0);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);

    videoWidget->setStyleSheet("background-color: black;");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(videoWidget);

    connect(mediaPlayer, &QMediaPlayer::mediaStatusChanged, this, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia || status == QMediaPlayer::InvalidMedia) {
            emit videoFinished();
        }
    });
}


void VideoCutsceneWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(rect(), Qt::black);
}

void VideoCutsceneWidget::playVideo(const QString &filePath)
{
    videoWidget->show();
    mediaPlayer->setSource(QUrl::fromLocalFile(filePath));
    mediaPlayer->play();
}

void VideoCutsceneWidget::stopVideo()
{
    mediaPlayer->stop();
    mediaPlayer->setSource(QUrl());
    videoWidget->hide();
}