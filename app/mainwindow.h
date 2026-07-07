#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "barwidget.h"
#include "drivingwidget.h"
#include "policewidget.h"
#include "alcoholengine.h"
#include "realtimeprovider.h" // Подключаем наш реальный провайдер

class QStackedWidget;
class VideoCutsceneWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onVideoFinished();
    void switchToStart();
    void switchToBar();
    void switchToDriving();
    void switchToPolice();

private:
    Ui::MainWindow *ui;

    QStackedWidget *stackedWidget;
    BarWidget *barScreen;
    DrivingWidget *drivingScreen;
    PoliceWidget *policeScreen;
    QWidget *startScreen;
    QWidget *blackScreen;
    VideoCutsceneWidget *cutscenePlayer;

    QWidget *targetWidgetAfterVideo;
    QMediaPlayer *bgMusicPlayer;
    QAudioOutput *bgAudioOutput;


    RealTimeProvider *timeProvider;
    AlcoholEngine *gameEngine;

    void resetGameScreens();
};

#endif // MAINWINDOW_H