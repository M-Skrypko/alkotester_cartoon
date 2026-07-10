#include "barwidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>
#include <QEvent>
#include <QCursor>
#include <QApplication>
#include <QEnterEvent>
#include <QSoundEffect>

BarWidget::BarWidget(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);

    drinks.push_back({"Пиво", QPixmap(":/images/bottle_beer.png"), QRect(), QColor(240, 190, 40, 220), QPixmap(":/images/glass.png"), 500.0, 5.0});
    drinks.push_back({"Вино", QPixmap(":/images/wine.png"), QRect(), QColor(128, 0, 32, 230), QPixmap(":/images/wine_glass.png"), 200.0, 12.0});
    drinks.push_back({"Виски", QPixmap(":/images/whiskey.png"), QRect(), QColor(184, 115, 51, 230), QPixmap(":/images/whiskey_glass.png"), 50.0, 40.0});
    drinks.push_back({"Водка", QPixmap(":/images/bottle_vodka.png"), QRect(), QColor(220, 240, 255, 140), QPixmap(":/images/whiskey_glass.png"), 50.0, 40.0});

    pourTimer = new QTimer(this);
    connect(pourTimer, &QTimer::timeout, this, &BarWidget::updatePouring);

    pourSound = new QSoundEffect(this);
    pourSound->setSource(QUrl("qrc:/sounds/naliv_sound.wav"));
    pourSound->setLoopCount(QSoundEffect::Infinite);
    pourSound->setVolume(0.5f);

    btnPour = new QPushButton("Налить 🍾", this);
    btnGo = new QPushButton("Сесть за руль 🚗", this);

    btnPour->setEnabled(false);
    btnPour->show();
    btnGo->show();

    QString btnStyle = "QPushButton { padding: 12px; font-size: 15px; background-color: #2c3e50; color: white; font-weight: bold; border-radius: 8px; border: 2px solid #34495e; }"
                       "QPushButton:hover { background-color: #34495e; border: 2px solid #3498db; }"
                       "QPushButton:disabled { background-color: #7f8c8d; border: 2px solid #95a5a6; color: #bdc3c7; }";
    btnPour->setStyleSheet(btnStyle);

    QString goStyle = "QPushButton { padding: 12px; font-size: 15px; background-color: #27ae60; color: white; font-weight: bold; border-radius: 8px; }"
                      "QPushButton:hover { background-color: #2ecc71; }";
    btnGo->setStyleSheet(goStyle);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addStretch();
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->addWidget(btnPour);
    actionLayout->addWidget(btnGo);
    mainLayout->addLayout(actionLayout);

    connect(btnPour, &QPushButton::clicked, this, &BarWidget::startPouring);
    connect(btnGo, &QPushButton::clicked, this, [=]() { emit finishedDrinking(totalPureAlcoholMl * 0.789); });

    QPixmap handPixmap(":/images/hand_cursor.png");
    QPixmap scaledPixmap = handPixmap.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    QCursor handCursor(scaledPixmap, 0, 0);
    this->setCursor(handCursor);
}

void BarWidget::resetBar() {
    totalPureAlcoholMl = 0.0;
    selectedBottleIndex = -1;
    hoveredBottleIndex = -1;
    pourHeight = 0;

    btnPour->setText("Налить 🍾");
    btnPour->setEnabled(false);
    btnPour->show();
    btnGo->show();

    if (pourSound->isPlaying()) {
        pourSound->stop();
    }

    setCursor(Qt::ArrowCursor);
    update();
}

void BarWidget::mouseMoveEvent(QMouseEvent *event) {
    int oldHover = hoveredBottleIndex;
    hoveredBottleIndex = -1;

    if (!pourTimer->isActive()) {
        for (int i = 0; i < drinks.size(); ++i) {
            if (drinks[i].boundingBox.contains(event->pos())) {
                hoveredBottleIndex = i;
                break;
            }
        }
    }

    if (oldHover != hoveredBottleIndex) {
        if (hoveredBottleIndex != -1) {
            setCursor(Qt::PointingHandCursor);
        } else {
            setCursor(Qt::ArrowCursor);
        }
        update();
    }
}

void BarWidget::mousePressEvent(QMouseEvent *event) {
    if (pourTimer->isActive() || btnPour->text() == "Выпить 🍻") {
        return;
    }

    for (int i = 0; i < drinks.size(); ++i) {
        if (drinks[i].boundingBox.contains(event->pos())) {
            selectedBottleIndex = i;
            pourHeight = 0;
            btnPour->setText("Налить 🍾");
            btnPour->setEnabled(true);
            update();
            break;
        }
    }
}

void BarWidget::startPouring() {
    if (btnPour->text() == "Выпить 🍻") {
        if (selectedBottleIndex != -1) {
            totalPureAlcoholMl += (drinks[selectedBottleIndex].volumeMl * (drinks[selectedBottleIndex].abvPercent / 100.0));
        }
        pourHeight = 0;
        selectedBottleIndex = -1;

        btnPour->setText("Налить 🍾");
        btnPour->setEnabled(false);
        setCursor(Qt::ArrowCursor);
        update();
        return;
    }

    pourHeight = 0;
    btnPour->setEnabled(false);
    btnPour->setText("Наливается...");
    pourTimer->start(20);
}

void BarWidget::updatePouring() {
    if (pourHeight < maxPourHeight) {
        pourHeight += 3;

        if (pourHeight >= 20) {
            if (!pourSound->isPlaying()) {
                pourSound->play();
            }
        }
        update();
    }
    else {
        pourTimer->stop();
        if (pourSound->isPlaying()) {
            pourSound->stop();
        }

        btnPour->setEnabled(true);
        btnPour->setText("Выпить 🍻");
        update();
    }
}

void BarWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/images/bg_bar.jpg"));

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 160));
    painter.drawRoundedRect(width() - 360, 20, 340, 50, 10, 10);

    painter.setPen(Qt::white);
    painter.setFont(QFont("Courier New", 14, QFont::Bold));
    painter.drawText(width() - 340, 52, QString("Выпито чистого спирта: %1 мл").arg(totalPureAlcoholMl, 0, 'f', 1));

    int baselineY = height() * 0.64;

    double scaleX = width() / 1000.0;
    double scaleY = height() / 600.0;

    int currentX = static_cast<int>(150 * scaleX);
    int spacing = static_cast<int>(35 * scaleX);

    for (int i = 0; i < drinks.size(); ++i) {
        int bW = 55;
        int bH = 150;

        if (drinks[i].name == "Вино") { bW = 128; bH = 165; }
        else if (drinks[i].name == "Виски") { bW = 120; bH = 135; }
        else if (drinks[i].name == "Пиво") { bW = 52; bH = 125; }
        else if (drinks[i].name == "Водка") { bW = 110; bH = 160; }

        bW = static_cast<int>(bW * scaleX);
        bH = static_cast<int>(bH * scaleY);

        int bX = currentX;
        int bY = baselineY - bH - static_cast<int>(30 * scaleY);

        drinks[i].boundingBox = QRect(bX, bY, bW, bH);

        if (i == selectedBottleIndex) {
            if (pourTimer->isActive()) {
                int baseGlassW = 160;
                int baseGlassX = 620;
                if (drinks[i].name == "Пиво") { baseGlassW = 180; baseGlassX = 600; }
                else if (drinks[i].name == "Вино") { baseGlassW = 160; baseGlassX = 620; }

                int targetGlassX = baseGlassX * scaleX;
                int streamX = targetGlassX + (83 * scaleX);
                int targetX = streamX - bW + (20 * scaleX);
                int targetY = baselineY - bH - (120 * scaleY);

                double ease = qMin(1.0, pourHeight / 20.0);
                int animX = bX + (targetX - bX) * ease;
                int animY = bY + (targetY - bY) * ease;
                double angle = 80.0 * ease;

                painter.save();
                painter.translate(animX + bW / 2.0, animY + bH / 2.0);
                painter.rotate(angle);
                painter.drawPixmap(-bW / 2, -bH / 2, bW, bH, drinks[i].bottlePixmap);
                painter.restore();

                if (ease >= 1.0) {
                    if (pourHeight < maxPourHeight) {
                        painter.save();
                        QLinearGradient streamGrad(streamX, targetY, streamX + (8 * scaleX), targetY);
                        streamGrad.setColorAt(0, drinks[i].liquidColor.darker(110));
                        streamGrad.setColorAt(0.5, Qt::white);
                        streamGrad.setColorAt(1, drinks[i].liquidColor.darker(110));

                        painter.setPen(Qt::NoPen);
                        painter.setBrush(streamGrad);

                        int neckY = animY + bH - (10 * scaleY);
                        int liquidLvlY = baselineY - (15 * scaleY) - (pourHeight * scaleY);
                        if (liquidLvlY > neckY) {
                            painter.drawRect(streamX - (3 * scaleX), neckY, 6 * scaleX, liquidLvlY - neckY);
                        }
                        painter.restore();
                    }
                }
            }
            else {
                painter.drawPixmap(bX, bY, bW, bH, drinks[i].bottlePixmap);

                painter.save();
                QPen selectPen(QColor(255, 215, 0, 220), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
                painter.setPen(selectPen);
                painter.setBrush(Qt::NoBrush);
                painter.drawRoundedRect(bX - 4, bY - 4, bW + 8, bH + 8, 6, 6);
                painter.restore();
            }
        }
        else {
            painter.drawPixmap(bX, bY, bW, bH, drinks[i].bottlePixmap);

            if (i == hoveredBottleIndex) {
                painter.save();
                painter.setPen(Qt::NoPen);
                painter.setBrush(QColor(52, 152, 219, 150));
                painter.drawRoundedRect(bX + 1, bY + 1, bW - 2, bH - 2, 6, 6);
                painter.restore();
            }
        }

        currentX += bW + spacing;
    }

    if (selectedBottleIndex != -1) {
        DrinkItem &drink = drinks[selectedBottleIndex];

        int baseGlassW = 160;
        int baseGlassH = 110;
        int baseGlassX = 620;
        int baseYOffset = 15;
        int baseBottomOffset = 12;

        if (drink.name == "Пиво") {
            baseGlassW = 180;
            baseGlassH = 133;
            baseGlassX = 600;
            baseYOffset = 15;
            baseBottomOffset = 10;
        } else if (drink.name == "Вино") {
            baseGlassW = 160;
            baseGlassH = 135;
            baseGlassX = 620;
            baseYOffset = 19;
            baseBottomOffset = 65;
        }

        int glassW = baseGlassW * scaleX;
        int glassH = baseGlassH * scaleY;
        int glassX = baseGlassX * scaleX;
        int glassY = baselineY - glassH - (baseYOffset * scaleY);
        int bottomY = glassY + glassH - (baseBottomOffset * scaleY);

        if (pourHeight > 0 || btnPour->text() == "Выпить 🍻") {
            double curProgress = static_cast<double>(pourHeight) / maxPourHeight;
            double curVol = curProgress * drink.volumeMl;
            double curAlcMassGrams = (curVol * (drink.abvPercent / 100.0)) * 0.789;

            painter.save();
            painter.setPen(Qt::NoPen);
            painter.setBrush(QColor(0, 0, 0, 180));
            painter.drawRoundedRect(glassX - (60 * scaleX), glassY - (80 * scaleY), 260 * scaleX, 65 * scaleY, 8, 8);

            painter.setPen(Qt::white);
            painter.setFont(QFont("Courier New", 11, QFont::Bold));
            painter.drawText(glassX - (50 * scaleX), glassY - (60 * scaleY), QString("Объем: %1 / %2 мл").arg(curVol, 0, 'f', 0).arg(drink.volumeMl, 0, 'f', 0));
            painter.drawText(glassX - (50 * scaleX), glassY - (40 * scaleY), QString("Чистый спирт: %1 г").arg(curAlcMassGrams, 0, 'f', 2));
            painter.restore();
        }

        painter.drawPixmap(glassX, glassY, glassW, glassH, drink.glassPixmap);

        painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        if (pourHeight > 0) {
            painter.save();

            QPainterPath clipPath;

            if (drink.name == "Вино") {
                clipPath.moveTo(glassX + 70 * scaleX, glassY + 15 * scaleY);
                clipPath.lineTo(glassX + 90 * scaleX, glassY + 15 * scaleY);
                clipPath.quadTo(glassX + 98 * scaleX, glassY + 40 * scaleY, glassX + 93 * scaleX, glassY + 65 * scaleY);
                clipPath.lineTo(glassX + 83 * scaleX, glassY + 70 * scaleY);
                clipPath.quadTo(glassX + 79 * scaleX, glassY + 75 * scaleY, glassX + 77 * scaleX, glassY + 70 * scaleY);
                clipPath.lineTo(glassX + 67 * scaleX, glassY + 65 * scaleY);
                clipPath.quadTo(glassX + 62 * scaleX, glassY + 40 * scaleY, glassX + 70 * scaleX, glassY + 15 * scaleY);
                clipPath.closeSubpath();
            }
            else if (drink.name == "Пиво") {
                int rectW = 24 * scaleX;
                int rectH = 75 * scaleY;
                int rectX = glassX + 77 * scaleX;
                int rectY = glassY + 20 * scaleY;

                clipPath.addRect(rectX, rectY, rectW, rectH);
                clipPath.closeSubpath();
            }
            else if (drink.name == "Виски" || drink.name == "Водка") {
                int rectW = 30 * scaleX;
                int rectH = 35 * scaleY;
                int rectX = glassX + 65 * scaleX;
                int rectY = glassY + 38 * scaleY;

                clipPath.addRect(rectX, rectY, rectW, rectH);
                clipPath.closeSubpath();
            }

            painter.setClipPath(clipPath);

            QLinearGradient liquidGrad(glassX, glassY, glassX + glassW, glassY);
            liquidGrad.setColorAt(0, drink.liquidColor.darker(130));
            liquidGrad.setColorAt(0.4, drink.liquidColor.lighter(110));
            liquidGrad.setColorAt(1, drink.liquidColor);

            painter.setPen(Qt::NoPen);
            painter.setBrush(liquidGrad);

            int scaledPourHeight = pourHeight * scaleY;
            int liquidTopY = bottomY - scaledPourHeight;
            if (drink.name == "Пиво") {
                liquidTopY += static_cast<int>(8 * scaleY);
            }
            painter.drawRect(glassX - (10 * scaleX), liquidTopY, glassW + (20 * scaleX), scaledPourHeight + (10 * scaleY));

            painter.setBrush(drink.liquidColor.lighter(130));
            if (drink.name == "Вино") {
                painter.drawEllipse(glassX + (31 * scaleX), liquidTopY - (4 * scaleY), glassW - (62 * scaleX), 8 * scaleY);
            } else if (drink.name == "Пиво") {
                painter.drawEllipse(glassX + (27 * scaleX), liquidTopY - (5 * scaleY), glassW - (54 * scaleX), 10 * scaleY);
            } else {
                painter.drawEllipse(glassX + (24 * scaleX), liquidTopY - (4 * scaleY), glassW - (48 * scaleX), 8 * scaleY);
            }

            painter.restore();

            if (drink.name == "Пиво" && pourHeight > 15) {
                painter.save();
                int foamHeight = 16 * scaleY;
                int foamTopY = qMax(liquidTopY, glassY + static_cast<int>(25 * scaleY));

                QLinearGradient foamGrad(0, foamTopY - foamHeight, 0, foamTopY);
                foamGrad.setColorAt(0.0, QColor(255, 255, 255, 245));
                foamGrad.setColorAt(0.6, QColor(252, 246, 228, 240));
                foamGrad.setColorAt(1.0, QColor(238, 222, 185, 245));

                painter.setBrush(foamGrad);
                painter.drawRoundedRect(glassX + (77 * scaleX), foamTopY - foamHeight, 24 * scaleX, foamHeight, 4 * scaleX, 4 * scaleY);
                painter.restore();
            }
        }
    }
}

void BarWidget::enterEvent(QEnterEvent *event)
{
    QPixmap handPixmap(":/images/hand_cursor.png");
    QCursor handCursor(handPixmap.scaled(95, 95, Qt::KeepAspectRatio, Qt::SmoothTransformation), 10, 5);

    QApplication::setOverrideCursor(handCursor);
    QWidget::enterEvent(event);
}

void BarWidget::leaveEvent(QEvent *event)
{
    QApplication::restoreOverrideCursor();
    QWidget::leaveEvent(event);
}