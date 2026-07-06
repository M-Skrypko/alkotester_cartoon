#ifndef VIDEOCUTSCENEWIDGET_H
#define VIDEOCUTSCENEWIDGET_H

#include <QWidget>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

class VideoCutsceneWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VideoCutsceneWidget(QWidget *parent = nullptr);
    void playVideo(const QString &filePath);
    void stopVideo();

signals:
    void videoFinished();

protected:

    void paintEvent(QPaintEvent *event) override;

private:
    QVideoWidget *videoWidget;
    QMediaPlayer *mediaPlayer;
    QAudioOutput *audioOutput;
};

#endif // VIDEOCUTSCENEWIDGET_H