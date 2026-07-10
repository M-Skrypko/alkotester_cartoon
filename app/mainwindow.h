#pragma once

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QStackedWidget>

class BarWidget;
class DrivingWidget;
class PoliceWidget;
class AlcoholEngine;
class RealTimeProvider;
class VideoCutsceneWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onVideoFinished();
    void switchToStart();
    void switchToBar();
    void switchToDriving();
    void switchToPolice();

private:
    Ui::MainWindow *ui{nullptr};

    QStackedWidget *stackedWidget{nullptr};
    BarWidget *barScreen{nullptr};
    DrivingWidget *drivingScreen{nullptr};
    PoliceWidget *policeScreen{nullptr};
    QWidget *startScreen{nullptr};
    QWidget *blackScreen{nullptr};
    VideoCutsceneWidget *cutscenePlayer{nullptr};

    QWidget *targetWidgetAfterVideo{nullptr};

    QMediaPlayer *bgMusicPlayer{nullptr};
    QAudioOutput *bgAudioOutput{nullptr};

    RealTimeProvider *timeProvider{nullptr};
    AlcoholEngine *gameEngine{nullptr};

    void resetGameScreens();
};