#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videocutscenewidget.h"
#include "barwidget.h"
#include "drivingwidget.h"
#include "policewidget.h"
#include "alcoholengine.h"
#include "realtimeprovider.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Мультик от Скрипко");

    if (statusBar()) {
        statusBar()->hide();
    }

    timeProvider = new RealTimeProvider();
    gameEngine = new AlcoholEngine(timeProvider, this);

    if (!gameEngine->getLastError().isEmpty()) {
        QMessageBox::critical(this, "Критическая ошибка", gameEngine->getLastError());
    }

    bgAudioOutput = new QAudioOutput(this);
    bgAudioOutput->setVolume(0.5);

    bgMusicPlayer = new QMediaPlayer(this);
    bgMusicPlayer->setAudioOutput(bgAudioOutput);
    bgMusicPlayer->setSource(QUrl("qrc:/sounds/menu_sound.wav"));
    bgMusicPlayer->setLoops(QMediaPlayer::Infinite);
    bgMusicPlayer->play();

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    cutscenePlayer = new VideoCutsceneWidget(this);
    cutscenePlayer->hide();
    connect(cutscenePlayer, &VideoCutsceneWidget::videoFinished, this, &MainWindow::onVideoFinished);

    startScreen = new QWidget(this);

    blackScreen = new QWidget(this);
    blackScreen->setStyleSheet("background-color: black;");

    auto *disclaimerScreen = new QWidget(this);
    disclaimerScreen->setStyleSheet("QWidget { background-color: #141414; }");
    auto *disclaimerLayout = new QVBoxLayout(disclaimerScreen);
    disclaimerLayout->setAlignment(Qt::AlignCenter);

    auto *discBox = new QWidget(disclaimerScreen);
    discBox->setStyleSheet(
        "QWidget { background-color: #1c1c1e; border: 2px solid #d63031; border-radius: 12px; padding: 30px; }"
        "QLabel { background: transparent; }"
        );
    auto *discBoxLayout = new QVBoxLayout(discBox);

    auto *lblDiscTitle = new QLabel("🔞 ВОЗРАСТНОЙ КОНТРОЛЬ 🔞", discBox);
    lblDiscTitle->setStyleSheet("color: #d63031; font-size: 22px; font-weight: bold;");
    lblDiscTitle->setAlignment(Qt::AlignCenter);

    auto *lblDiscText = new QLabel("Просмотр строго с 18 лет.\nЧрезмерное употребление алкоголя вредит вашему здоровью.", discBox);
    lblDiscText->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold; margin: 20px 0;");
    lblDiscText->setAlignment(Qt::AlignCenter);

    auto *btnConfirmAge = new QPushButton("Мне есть 18 лет (Подтвердить возраст)", discBox);
    btnConfirmAge->setStyleSheet(
        "QPushButton { padding: 12px 35px; font-size: 14px; font-weight: bold; background-color: #d63031; color: white; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #ff7675; }"
        );

    auto *btnExit = new QPushButton("Выйти", discBox);
    btnExit->setStyleSheet(
        "QPushButton { padding: 12px 35px; font-size: 14px; font-weight: bold; background-color: #636e72; color: white; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #b2bec3; }"
        );

    discBoxLayout->addWidget(lblDiscTitle);
    discBoxLayout->addWidget(lblDiscText);
    discBoxLayout->addWidget(btnConfirmAge);
    discBoxLayout->addWidget(btnExit);
    disclaimerLayout->addWidget(discBox);

    connect(btnConfirmAge, &QPushButton::clicked, [this]() { stackedWidget->setCurrentWidget(startScreen); });
    connect(btnExit, &QPushButton::clicked, this, &QWidget::close);

    startScreen->setStyleSheet("QWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2c3e50, stop:1 #3498db); }");

    auto *settingsWidget = new QWidget(startScreen);
    settingsWidget->setStyleSheet(
        "QWidget { background-color: rgba(255, 255, 255, 230); border-radius: 15px; padding: 20px; }"
        "QLabel { background: transparent; color: #2c3e50; font-size: 14px; font-weight: bold; }"
        "QComboBox, QSpinBox { background: white; border: 1px solid #bdc3c7; border-radius: 5px; padding: 5px; font-size: 14px; color: black; }"
        );
    auto *settingsLayout = new QVBoxLayout(settingsWidget);

    auto *lblWelcome = new QLabel("🚦 СИМУЛЯТОР ТРЕЗВОГО ВОЖДЕНИЯ 🚦", settingsWidget);
    lblWelcome->setStyleSheet("font-size: 18px; font-weight: bold; color: #c0392b; margin-bottom: 15px;");
    lblWelcome->setAlignment(Qt::AlignCenter);
    settingsLayout->addWidget(lblWelcome);

    auto *comboGender = new QComboBox(settingsWidget);
    comboGender->addItems({"Мужской", "Женский"});
    auto *genderLayout = new QHBoxLayout();
    genderLayout->addWidget(new QLabel("Ваш пол:", settingsWidget));
    genderLayout->addWidget(comboGender);
    settingsLayout->addLayout(genderLayout);

    auto *comboCountry = new QComboBox(settingsWidget);
    comboCountry->addItems(gameEngine->getAvailableCountries());
    comboCountry->setCurrentText("Беларусь");
    auto *countryLayout = new QHBoxLayout();
    countryLayout->addWidget(new QLabel("Страна:", settingsWidget));
    countryLayout->addWidget(comboCountry);
    settingsLayout->addLayout(countryLayout);

    auto *spinWeight = new QSpinBox(settingsWidget);
    spinWeight->setRange(1, 500);
    spinWeight->setValue(90);
    auto *weightLayout = new QHBoxLayout();
    weightLayout->addWidget(new QLabel("Ваш вес (кг):", settingsWidget));
    weightLayout->addWidget(spinWeight);
    settingsLayout->addLayout(weightLayout);

    auto *btnStart = new QPushButton("Сесть за столик в Бар ➡️", settingsWidget);
    btnStart->setStyleSheet(
        "QPushButton { padding: 12px; font-size: 15px; font-weight: bold; background-color: #27ae60; color: white; border-radius: 5px; border: none; }"
        "QPushButton:hover { background-color: #2ecc71; }"
        );
    settingsLayout->addWidget(btnStart);

    auto *startMainLayout = new QVBoxLayout(startScreen);
    startMainLayout->setAlignment(Qt::AlignCenter);
    startMainLayout->addWidget(settingsWidget);

    connect(btnStart, &QPushButton::clicked, [=]() {
        int currentWeight = spinWeight->value();
        if (currentWeight < 35 || currentWeight > 240) {
            auto reply = QMessageBox::warning(this, "Предупреждение",
                                              QString("Указанный вес (%1 кг) выходит за рамки нормы.\nВы уверены?").arg(currentWeight),
                                              QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::No) return;
        }
        gameEngine->setProfile(comboGender->currentText(), currentWeight, comboCountry->currentText());
        switchToBar();
    });

    stackedWidget->addWidget(disclaimerScreen);
    stackedWidget->addWidget(startScreen);
    stackedWidget->addWidget(blackScreen);

    resetGameScreens();
    stackedWidget->setCurrentWidget(disclaimerScreen);
    resize(1024, 600);
}

MainWindow::~MainWindow()
{
    delete timeProvider;
    delete ui;
}

void MainWindow::resetGameScreens()
{
    if (barScreen) {
        stackedWidget->removeWidget(barScreen);
        barScreen->deleteLater();
    }
    if (drivingScreen) {
        stackedWidget->removeWidget(drivingScreen);
        drivingScreen->deleteLater();
    }
    if (policeScreen) {
        stackedWidget->removeWidget(policeScreen);
        policeScreen->deleteLater();
    }

    barScreen = new BarWidget(this);
    drivingScreen = new DrivingWidget(this);
    policeScreen = new PoliceWidget(this);

    stackedWidget->addWidget(barScreen);
    stackedWidget->addWidget(drivingScreen);
    stackedWidget->addWidget(policeScreen);

    connect(barScreen, &BarWidget::finishedDrinking, gameEngine, &AlcoholEngine::addAlcoholGrams);
    connect(barScreen, &BarWidget::finishedDrinking, this, &MainWindow::switchToDriving);

    connect(drivingScreen, &DrivingWidget::policePulledOver, this, [this](double hoursElapsed) {
        timeProvider->advanceTime(hoursElapsed);
        gameEngine->generateVerdict();
        switchToPolice();
    });

    connect(gameEngine, &AlcoholEngine::verdictReady, policeScreen, &PoliceWidget::displayVerdict);
    connect(policeScreen, &PoliceWidget::restartRequested, this, &MainWindow::switchToStart);
}

void MainWindow::switchToStart()
{
    timeProvider->reset();
    gameEngine->resetData();
    stackedWidget->setCurrentWidget(blackScreen);
    resetGameScreens();

    if (bgMusicPlayer) {
        bgMusicPlayer->play();
    }
    stackedWidget->setCurrentWidget(startScreen);
}

void MainWindow::switchToBar()
{
    if (bgMusicPlayer) {
        bgMusicPlayer->stop();
    }

    QString videoPath = QCoreApplication::applicationDirPath() + "/videos/entrance.MP4";
    if (!QFile::exists(videoPath)) {
        stackedWidget->setCurrentWidget(barScreen);
        return;
    }

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    targetWidgetAfterVideo = barScreen;
    cutscenePlayer->raise();
    cutscenePlayer->show();
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::switchToDriving()
{
    QString videoPath = QCoreApplication::applicationDirPath() + "/videos/exit.MP4";
    if (!QFile::exists(videoPath)) {
        drivingScreen->setAlcoholGrams(gameEngine->getConsumedGrams());
        stackedWidget->setCurrentWidget(drivingScreen);
        return;
    }

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    targetWidgetAfterVideo = drivingScreen;
    cutscenePlayer->raise();
    cutscenePlayer->show();
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::switchToPolice()
{
    QString videoPath = QCoreApplication::applicationDirPath() + "/videos/police_video.MP4";
    if (!QFile::exists(videoPath)) {
        stackedWidget->setCurrentWidget(policeScreen);
        return;
    }

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    targetWidgetAfterVideo = policeScreen;
    cutscenePlayer->raise();
    cutscenePlayer->show();
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::onVideoFinished()
{
    if (!targetWidgetAfterVideo) return;

    auto *nextScreen = targetWidgetAfterVideo;
    targetWidgetAfterVideo = nullptr;

    if (nextScreen == drivingScreen) {
        drivingScreen->setAlcoholGrams(gameEngine->getConsumedGrams());
    }

    stackedWidget->setCurrentWidget(nextScreen);
    nextScreen->repaint();
    QCoreApplication::processEvents();

    cutscenePlayer->hide();
    cutscenePlayer->stopVideo();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (cutscenePlayer) {
        cutscenePlayer->setGeometry(this->rect());
    }
}