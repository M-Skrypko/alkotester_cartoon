#include "drivingwidget.h"
#include <QPainter>
#include <QShowEvent>
#include <cmath>
#include <QTimer>
#include <QRandomGenerator>
#include <QSoundEffect>

DrivingWidget::DrivingWidget(QWidget *parent)
    : QWidget(parent)
{
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &DrivingWidget::updateAnimation);

    rideDurationTimer = new QTimer(this);
    connect(rideDurationTimer, &QTimer::timeout, this, &DrivingWidget::handleTimeOut);

    sirenSound = new QSoundEffect(this);
    sirenSound->setSource(QUrl("qrc:/sounds/police_sound.wav"));
    sirenSound->setLoopCount(QSoundEffect::Infinite);
    sirenSound->setVolume(0.5f);
}

void DrivingWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);
}

void DrivingWidget::startSimulation(int durationSeconds) {
    timeLeft = durationSeconds;
    virtualTimeHours = 0.0;
    roadOffset = 0.0;
    speedMultiplier = 14.0;
    policeAppearing = false;

    carX = 120.0;
    policeX = (this->width() > 0 ? this->width() : 800) + 100.0;

    if (sirenSound->isPlaying()) {
        sirenSound->stop();
    }

    if (timeLeft <= 2) {
        sirenSound->play();
    }

    animationTimer->start(16);
    rideDurationTimer->start(1000);
}

void DrivingWidget::updateAnimation() {
    roadOffset += speedMultiplier;
    if (roadOffset >= width()) {
        roadOffset = 0;
    }

    if (!policeAppearing) {
        virtualTimeHours += 0.0032;
    } else {
        double targetPoliceX = width() - 320.0;
        double targetCarX = targetPoliceX - 230.0;

        speedMultiplier += (0.0 - speedMultiplier) * 0.05;
        policeX += (targetPoliceX - policeX) * 0.05;
        carX += (targetCarX - carX) * 0.04;

        if (std::abs(policeX - targetPoliceX) < 2.0) {
            if (speedMultiplier < 0.2) {
                animationTimer->stop();
                rideDurationTimer->stop();

                if (sirenSound->isPlaying()) {
                    sirenSound->stop();
                }

                emit policePulledOver(virtualTimeHours);
                return;
            }
        }
    }
    update();
}

void DrivingWidget::handleTimeOut() {
    if (!policeAppearing) {
        timeLeft -= 1;

        if (timeLeft == 2) {
            if (!sirenSound->isPlaying()) {
                sirenSound->play();
            }
        }

        if (timeLeft <= 0) {
            rideDurationTimer->stop();
            policeX = this->width() + 100.0;
            policeAppearing = true;
        }
    }
}

void DrivingWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.fillRect(rect(), QColor(135, 206, 235));

    QPixmap road(":/images/bg_road.jpg");
    int roadH = 160;
    int roadY = height() - roadH;

    if (!road.isNull()) {
        painter.drawPixmap(-static_cast<int>(roadOffset), roadY, width(), roadH, road);
        painter.drawPixmap(width() - static_cast<int>(roadOffset), roadY, width(), roadH, road);
    }

    int vehicleY = roadY + 35;

    QPixmap car(":/images/car_geely.png");
    if (!car.isNull()) {
        painter.drawPixmap(static_cast<int>(carX), vehicleY, 220, 85, car);
    }

    if (policeAppearing) {
        QPixmap policeCar(":/images/police.png");
        if (!policeCar.isNull()) {
            painter.drawPixmap(static_cast<int>(policeX), vehicleY, 220, 85, policeCar);
        }
    }

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 150));
    painter.drawRect(0, 0, width(), 90);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Arial", 14, QFont::Bold));
    if (!policeAppearing) {
        painter.drawText(20, 40, "Статус: Движение по трассе на Geely Emgrand...");
        painter.drawText(20, 70, QString("До патруля ГАИ осталось: %1 сек.").arg(timeLeft));
    } else {
        painter.setPen(QColor(255, 80, 80));
        painter.drawText(20, 40, "🚨 ГАИ: Водитель, прижимаемся к обочине и останавливаемся! 🚨");
    }

    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier New", 16, QFont::Bold));
    int hours = static_cast<int>(virtualTimeHours);
    int mins = static_cast<int>((virtualTimeHours - hours) * 60);
    painter.drawText(width() - 380, 50, QString("Время в пути: %1 ч. %2 мин.").arg(hours, 2, 10, QChar('0')).arg(mins, 2, 10, QChar('0')));
}

void DrivingWidget::setAlcoholGrams(double grams)
{
    int minSeconds = 3;
    int maxSeconds = 12;

    if (grams > 30.0) {
        maxSeconds = 6;
    } else if (grams == 0.0) {
        minSeconds = 9;
        maxSeconds = 12;
    }

    int randomRealSeconds = QRandomGenerator::global()->bounded(minSeconds, maxSeconds + 1);
    startSimulation(randomRealSeconds);
}