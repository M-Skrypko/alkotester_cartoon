#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "videocutscenewidget.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QMessageBox>
#include <QFile>
#include <QCoreApplication>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , playerGender("Мужской")
    , playerCountry("Беларусь")
    , playerWeight(90)
    , consumedAlcoholGrams(0.0)
    , targetWidgetAfterVideo(nullptr)
    , barScreen(nullptr)
    , drivingScreen(nullptr)
    , policeScreen(nullptr)
{
    ui->setupUi(this);

    setWindowTitle("Мультик от Скрипко");
    if (statusBar()) {
        statusBar()->hide();
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

    startScreen = new QWidget(this);
    cutscenePlayer = new VideoCutsceneWidget(this);
    connect(cutscenePlayer, &VideoCutsceneWidget::videoFinished, this, &MainWindow::onVideoFinished);

    blackScreen = new QWidget(this);
    blackScreen->setStyleSheet("background-color: black;");

    QWidget *disclaimerScreen = new QWidget(this);
    disclaimerScreen->setStyleSheet("QWidget { background-color: #141414; }");

    QVBoxLayout *disclaimerLayout = new QVBoxLayout(disclaimerScreen);
    disclaimerLayout->setAlignment(Qt::AlignCenter);

    QWidget *discBox = new QWidget(disclaimerScreen);
    discBox->setStyleSheet(
        "QWidget { background-color: #1c1c1e; border: 2px solid #d63031; border-radius: 12px; padding: 30px; }"
        "QLabel { background: transparent; }"
        );
    QVBoxLayout *discBoxLayout = new QVBoxLayout(discBox);

    QLabel *lblDiscTitle = new QLabel("🔞 ВОЗРАСТНОЙ КОНТРОЛЬ 🔞", discBox);
    lblDiscTitle->setStyleSheet("color: #d63031; font-size: 22px; font-weight: bold;");
    lblDiscTitle->setAlignment(Qt::AlignCenter);

    QLabel *lblDiscText = new QLabel("Просмотр строго с 18 лет.\nЧрезмерное употребление алкоголя вредит вашему здоровью.", discBox);
    lblDiscText->setStyleSheet("color: #ffffff; font-size: 16px; font-weight: bold; margin: 20px 0;");
    lblDiscText->setAlignment(Qt::AlignCenter);

    QPushButton *btnConfirmAge = new QPushButton("Мне есть 18 лет (Подтвердить возраст)", discBox);
    btnConfirmAge->setStyleSheet(
        "QPushButton { padding: 12px 35px; font-size: 14px; font-weight: bold; background-color: #d63031; color: white; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #ff7675; }"
        );
    QPushButton *btnExit = new QPushButton("Выйти", discBox);
    btnExit->setStyleSheet(
        "QPushButton { padding: 12px 35px; font-size: 14px; font-weight: bold; background-color: #636e72; color: white; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #b2bec3; }"
        );

    discBoxLayout->addWidget(lblDiscTitle);
    discBoxLayout->addWidget(lblDiscText);
    discBoxLayout->addWidget(btnConfirmAge);
    discBoxLayout->addWidget(btnExit);
    disclaimerLayout->addWidget(discBox);

    connect(btnConfirmAge, &QPushButton::clicked, [=]() {
        stackedWidget->setCurrentWidget(startScreen);
    });
    connect(btnExit, &QPushButton::clicked, this, &QWidget::close);

    startScreen->setStyleSheet(
        "QWidget { background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 #2c3e50, stop:1 #3498db); }"
        );

    QWidget *settingsWidget = new QWidget(startScreen);
    settingsWidget->setStyleSheet(
        "QWidget { background-color: rgba(255, 255, 255, 230); border-radius: 15px; padding: 20px; }"
        "QLabel { background: transparent; color: #2c3e50; font-size: 14px; font-weight: bold; }"
        "QComboBox, QSpinBox { background: white; border: 1px solid #bdc3c7; border-radius: 5px; padding: 5px; font-size: 14px; color: black; }"
        );

    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);

    QLabel *lblWelcome = new QLabel("🚦 СИМУЛЯТОР ТРЕЗВОГО ВОЖДЕНИЯ 🚦", settingsWidget);
    lblWelcome->setStyleSheet("font-size: 18px; font-weight: bold; color: #c0392b; margin-bottom: 15px;");
    lblWelcome->setAlignment(Qt::AlignCenter);
    settingsLayout->addWidget(lblWelcome);

    QHBoxLayout *genderLayout = new QHBoxLayout();
    QLabel *lblGender = new QLabel("Ваш пол:", settingsWidget);
    QComboBox *comboGender = new QComboBox(settingsWidget);
    comboGender->addItems({"Мужской", "Женский"});
    genderLayout->addWidget(lblGender);
    genderLayout->addWidget(comboGender);
    settingsLayout->addLayout(genderLayout);

    QHBoxLayout *countryLayout = new QHBoxLayout();
    QLabel *lblCountry = new QLabel("Страна:", settingsWidget);
    QComboBox *comboCountry = new QComboBox(settingsWidget);
    comboCountry->addItems({
        "Беларусь", "Австралия", "Австрия", "Азербайджан", "Албания", "Алжир", "Ангола",
        "Аргентина", "Армения", "Багамы", "Бангладеш", "Барбадос", "Бахрейн",
        "Бельгия", "Болгария", "Боливия", "Босния и Герцеговина",
        "Ботсвана", "Бразилия", "Бруней", "Великобритания", "Венгрия", "Венесуэла",
        "Вьетнам", "Гайана", "Гана", "Германия", "Греция", "Грузия",
        "Дания", "Египет", "Замбия", "Зимбабве", "Израиль", "Индия",
        "Индонезия", "Иордания", "Иран", "Ирландия", "Исландия", "Испания",
        "Италия", "Йемен", "Казахстан", "Камбоджа", "Камерун", "Канада",
        "Катар", "Кения", "Кипр", "Китай", "Колумбия", "Кот-д'Ивуар",
        "Куба", "Кувейт", "Кыргызстан", "Лаос", "Латвия", "Ливан",
        "Ливия", "Литва", "Люксембург", "Мадагаскар", "Малайзия", "Мальдивы",
        "Мальта", "Марокко", "Мексика", "Мозамбик", "Молдова", "Монголия",
        "Мьянма", "Намибия", "Непал", "Нигер", "Нигерия", "Нидерланды",
        "Новая Зеландия", "Норвегия", "ОАЭ", "Оман", "Пакистан", "Папуа — Новая Гвинея",
        "Парагвай", "Перу", "Польша", "Португалия", "Россия", "Румыния",
        "Саудовская Аравия", "Северная Македония", "Сербия", "Словакия", "Словения",
        "США", "Судан", "Суринам", "Таджикистан", "Таиланд", "Танзания",
        "Тринидад и Тобаго", "Тунис", "Туркменистан", "Турция", "Уганда", "Узбекистан",
        "Украина", "Уругвай", "Фиджи", "Филиппины", "Финляндия", "Франция",
        "Хорватия", "Черногория", "Чехия", "Чили", "Швейцария", "Швеция",
        "Шри-Ланка", "Эквадор", "Эстония", "Эфиопия", "ЮАР", "Южная Корея",
        "Ямайка", "Япония"
    });
    countryLayout->addWidget(lblCountry);
    countryLayout->addWidget(comboCountry);
    settingsLayout->addLayout(countryLayout);

    QHBoxLayout *weightLayout = new QHBoxLayout();
    QLabel *lblWeight = new QLabel("Ваш вес (кг):", settingsWidget);
    QSpinBox *spinWeight = new QSpinBox(settingsWidget);

    spinWeight->setRange(1, 500);
    spinWeight->setValue(90);

    weightLayout->addWidget(lblWeight);
    weightLayout->addWidget(spinWeight);
    settingsLayout->addLayout(weightLayout);

    QPushButton *btnStart = new QPushButton("Сесть за столик в Бар ➡️", settingsWidget);
    btnStart->setStyleSheet(
        "QPushButton { padding: 12px; font-size: 15px; font-weight: bold; background-color: #27ae60; color: white; border-radius: 5px; border: none; }"
        "QPushButton:hover { background-color: #2ecc71; }"
        );
    settingsLayout->addWidget(btnStart);

    QVBoxLayout *startMainLayout = new QVBoxLayout(startScreen);
    startMainLayout->setAlignment(Qt::AlignCenter);
    startMainLayout->addWidget(settingsWidget);

    connect(btnStart, &QPushButton::clicked, [=]() {
        int currentWeight = spinWeight->value();


        if (currentWeight < 35 || currentWeight > 240) {
            QMessageBox::StandardButton reply;
            reply = QMessageBox::warning(this, "Предупреждение",
                                         QString("Указанный вес (%1 кг) выходит за рамки среднестатистической нормы.\n"
                                                 "Это может сильно исказить расчеты алкоголя в крови.\n\n"
                                                 "Вы уверены, что хотите продолжить с этим весом?").arg(currentWeight),
                                         QMessageBox::Yes | QMessageBox::No);

            if (reply == QMessageBox::No) {
                return;
            }
        }

        playerGender = comboGender->currentText();
        playerCountry = comboCountry->currentText();
        playerWeight = currentWeight;
        switchToBar();
    });

    stackedWidget->addWidget(disclaimerScreen);
    stackedWidget->addWidget(startScreen);
    stackedWidget->addWidget(blackScreen);
    stackedWidget->addWidget(cutscenePlayer);

    resetGameScreens();

    stackedWidget->setCurrentWidget(disclaimerScreen);
    resize(1024, 600);
}

MainWindow::~MainWindow()
{
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

    connect(barScreen, &BarWidget::finishedDrinking, this, &MainWindow::switchToDriving);
    connect(drivingScreen, &DrivingWidget::policePulledOver, this, &MainWindow::switchToPolice);
    connect(policeScreen, &PoliceWidget::restartRequested, this, &MainWindow::switchToStart);
}

void MainWindow::switchToStart()
{
    consumedAlcoholGrams = 0.0;

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
        QMessageBox::critical(this, "Видео не найдено", "Положите видео по этому пути:\n\n" + videoPath);
        stackedWidget->setCurrentWidget(barScreen);
        return;
    }

    targetWidgetAfterVideo = barScreen;

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    stackedWidget->setCurrentWidget(cutscenePlayer);
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::switchToDriving(double alcoholGrams)
{
    consumedAlcoholGrams = alcoholGrams;

    QString videoPath = QCoreApplication::applicationDirPath() + "/videos/exit.MP4";

    if (!QFile::exists(videoPath)) {
        QMessageBox::critical(this, "Видео не найдено", "Положите видео по этому пути:\n\n" + videoPath);
        drivingScreen->setAlcoholGrams(alcoholGrams);
        stackedWidget->setCurrentWidget(drivingScreen);
        return;
    }

    targetWidgetAfterVideo = drivingScreen;

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    stackedWidget->setCurrentWidget(cutscenePlayer);
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::onVideoFinished()
{
    if (!targetWidgetAfterVideo) return;

    QWidget *nextScreen = targetWidgetAfterVideo;
    targetWidgetAfterVideo = nullptr;

    cutscenePlayer->stopVideo();
    stackedWidget->setCurrentWidget(blackScreen);

    QTimer::singleShot(50, this, [=]() {
        if (nextScreen == policeScreen) {
            showPoliceScreen();
        } else {
            stackedWidget->setCurrentWidget(nextScreen);
        }

        if (nextScreen == drivingScreen) {
            drivingScreen->setAlcoholGrams(consumedAlcoholGrams);
        }
    });
}

void MainWindow::switchToPolice(double timeInHours)
{
    lastDrivingTime = timeInHours;

    QString videoPath = QCoreApplication::applicationDirPath() + "/videos/police_video.MP4";

    if (!QFile::exists(videoPath)) {
        showPoliceScreen();
        return;
    }

    targetWidgetAfterVideo = policeScreen;

    stackedWidget->setCurrentWidget(blackScreen);
    QCoreApplication::processEvents();

    stackedWidget->setCurrentWidget(cutscenePlayer);
    cutscenePlayer->playVideo(videoPath);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);

    if (cutscenePlayer) {
        cutscenePlayer->setGeometry(this->rect());
    }

    if (blackScreen) {
        blackScreen->setGeometry(this->rect());
    }
}

void MainWindow::showPoliceScreen()
{
    policeScreen->calculateAndShowResult(
        playerCountry, playerGender, playerWeight, consumedAlcoholGrams, lastDrivingTime);
    stackedWidget->setCurrentWidget(policeScreen);
}