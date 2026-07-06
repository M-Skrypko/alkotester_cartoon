#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "barwidget.h"
#include "drivingwidget.h"
#include "policewidget.h"

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
    // Метод переопределения изменения размера объявлен ТОЛЬКО ОДИН РАЗ
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onVideoFinished();
    void switchToStart();
    void switchToBar();
    void switchToDriving(double alcoholGrams);
    void switchToPolice(double timeInHours);

private:
    Ui::MainWindow *ui;

    QStackedWidget *stackedWidget;

    // Игровые экраны
    BarWidget *barScreen;
    DrivingWidget *drivingScreen;
    PoliceWidget *policeScreen;
    QWidget *startScreen;
    QWidget *blackScreen;
    VideoCutsceneWidget *cutscenePlayer;
    double lastDrivingTime;
    void showPoliceScreen();
    QWidget *targetWidgetAfterVideo;

    // Фоновая музыка
    QMediaPlayer *bgMusicPlayer;
    QAudioOutput *bgAudioOutput;

    // Данные игрока
    QString playerGender;
    QString playerCountry;
    int playerWeight;
    double consumedAlcoholGrams;

    // Метод сброса
    void resetGameScreens();
};

#endif // MAINWINDOW_H