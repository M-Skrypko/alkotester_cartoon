#include "policewidget.h"
#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>

PoliceWidget::PoliceWidget(QWidget *parent)
    : QWidget(parent)
{
    sirenSound = new QSoundEffect(this);
    sirenSound->setSource(QUrl::fromLocalFile(":/sounds/police_sound.mp3"));
    sirenSound->setLoopCount(QSoundEffect::Infinite);
    sirenSound->setVolume(0.5f);

    lblAlcoResult = new QLabel(this);
    lblVerdict = new QLabel(this);
    btnAction = new QPushButton(this);
    lblAlcoResult->hide();
    lblVerdict->hide();
    btnAction->hide();

    scanTimer = new QTimer(this);
    connect(scanTimer, &QTimer::timeout, this, &PoliceWidget::updateScanner);

    infoCard = new QWidget(this);
    infoCard->setStyleSheet("background-color: rgba(20, 25, 35, 230); border-radius: 12px; border: 2px solid #3498db;");

    QVBoxLayout *cardLayout = new QVBoxLayout(infoCard);
    lblResultText = new QLabel("Ожидайте... Идет анализ выдоха...", infoCard);
    lblResultText->setWordWrap(true);
    lblResultText->setAlignment(Qt::AlignCenter);
    lblResultText->setStyleSheet("font-size: 16px; color: #ecf0f1; font-weight: bold; line-height: 24px;");
    cardLayout->addWidget(lblResultText);

    finalOverlay = new QWidget(this);
    finalOverlay->setAttribute(Qt::WA_StyledBackground, true);
    finalOverlay->setStyleSheet("background-color: rgb(0, 0, 0);");
    finalOverlay->hide();

    fadeEffect = new QGraphicsOpacityEffect(finalOverlay);
    finalOverlay->setGraphicsEffect(fadeEffect);

    QVBoxLayout *finalLayout = new QVBoxLayout(finalOverlay);
    finalLayout->setAlignment(Qt::AlignCenter);
    finalLayout->setSpacing(40);

    lblFinalText = new QLabel("Не пейте за рулем.\nВас ждут дома.", finalOverlay);
    lblFinalText->setStyleSheet("color: #ffffff; font-size: 32px; font-weight: bold; line-height: 48px;");
    lblFinalText->setAlignment(Qt::AlignCenter);
    finalLayout->addWidget(lblFinalText);

    btnFinalRestart = new QPushButton("🔄 Начать заново", finalOverlay);
    btnFinalRestart->setStyleSheet(
        "QPushButton { padding: 15px 40px; font-size: 18px; font-weight: bold; background-color: #e74c3c; color: white; border-radius: 6px; border: none; }"
        "QPushButton:hover { background-color: #c0392b; }"
        );
    finalLayout->addWidget(btnFinalRestart, 0, Qt::AlignCenter);

    connect(btnFinalRestart, &QPushButton::clicked, this, &PoliceWidget::restartGame);
}

void PoliceWidget::displayVerdict(double bac, const QString &lawText, int outcomeType)
{
    resultPromille = bac;
    currentLawText = lawText;
    currentOutcome = outcomeType;
    displayedPromille = 0.0;

    btnAction->disconnect();

    if (currentOutcome == 0) {
        btnAction->setText("Продолжить путь");
        btnAction->setStyleSheet(
            "QPushButton { padding: 14px 25px; font-size: 16px; font-weight: bold; background-color: #27ae60; color: white; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #2ecc71; }"
            );
        connect(btnAction, &QPushButton::clicked, this, &PoliceWidget::restartGame);
    } else {
        btnAction->setText("Принять протокол ГАИ");
        btnAction->setStyleSheet(
            "QPushButton { padding: 14px 25px; font-size: 16px; font-weight: bold; background-color: #d35400; color: white; border-radius: 6px; border: none; }"
            "QPushButton:hover { background-color: #e67e22; }"
            );
        connect(btnAction, &QPushButton::clicked, this, &PoliceWidget::startFinalScenario);
    }

    lblVerdict->setText(currentLawText);
    lblResultText->setText("Ожидайте... Идет анализ выдоха...");
    btnAction->hide();
}

void PoliceWidget::showEvent(QShowEvent *event) {
    QWidget::showEvent(event);

    if (!currentLawText.isEmpty() && btnAction->isHidden() && !scanTimer->isActive()) {
        scanTimer->start(40);
    }
}

void PoliceWidget::updateScanner() {
    double step = (resultPromille > 0.0) ? (resultPromille / 50.0) : 0.01;
    displayedPromille += step;
    lblAlcoResult->setText(QString::number(displayedPromille, 'f', 2) + " ‰");

    if (displayedPromille >= (resultPromille - 0.001)) {
        scanTimer->stop();
        displayedPromille = resultPromille;
        lblAlcoResult->setText(QString::number(resultPromille, 'f', 2) + " ‰");
        lblResultText->setText(currentLawText);
        btnAction->show();
        btnAction->raise();
    }
    update();
}

void PoliceWidget::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    int cardW = width() * 0.8;
    infoCard->setGeometry((width() - cardW) / 2, height() - 210, cardW, 110);
    btnAction->setGeometry((width() - 240) / 2, height() - 70, 240, 45);

    if (finalOverlay && finalOverlay->isVisible()) {
        finalOverlay->resize(size());
    }
}

void PoliceWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(rect(), QColor(25, 30, 40));

    QPixmap copCar(":/images/police.png");
    if (!copCar.isNull()) {
        painter.drawPixmap(40, height() - 240, 200, 75, copCar);
    }

    QPixmap tester(":/images/alkotester.png");
    int tW = 850;
    int tH = 500;
    int tX = (width() - tW) / 2;
    int tY = 15;

    if (!tester.isNull()) {
        painter.drawPixmap(tX, tY, tW, tH, tester);
    }

    QColor displayColor = scanTimer->isActive() ? QColor(255, 200, 0) : ((resultPromille > 0.3) ? QColor(255, 50, 50) : QColor(0, 0, 255));
    painter.setPen(displayColor);
    painter.setFont(QFont("Courier New", 30, QFont::Bold));
    QString scoreText = QString("%1 ‰").arg(displayedPromille, 0, 'f', 2);
    painter.drawText(QRect(tX + 20 , tY + 115, tW, 60), Qt::AlignCenter, scoreText);
}

void PoliceWidget::startFinalScenario() {
    finalOverlay->resize(size());
    fadeEffect->setOpacity(0.0);
    finalOverlay->show();
    finalOverlay->raise();
    QPropertyAnimation *animation = new QPropertyAnimation(fadeEffect, "opacity");
    animation->setDuration(1500);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void PoliceWidget::restartGame() {
    if (scanTimer->isActive()) {
        scanTimer->stop();
    }
    resultPromille = 0.0;
    displayedPromille = 0.0;
    currentOutcome = 0;
    currentLawText = "";
    lblAlcoResult->setText("0.00 ‰");
    lblVerdict->clear();
    lblResultText->setText("Ожидайте... Идет анализ выдоха...");
    finalOverlay->hide();
    btnAction->hide();
    update();
    emit restartRequested();
}