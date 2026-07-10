#ifndef BARWIDGET_H
#define BARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QPixmap>
#include <QRect>
#include <QMouseEvent>
#include <QSoundEffect>
#include <QList>

struct DrinkItem {
    QString name;
    QPixmap bottlePixmap;
    QRect boundingBox;
    QColor liquidColor;
    QPixmap glassPixmap;
    double volumeMl;
    double abvPercent;
};

class BarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BarWidget(QWidget *parent = nullptr);
    void resetBar();

signals:
    void finishedDrinking(double alcoholGrams);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void startPouring();
    void updatePouring();

    QList<DrinkItem> drinks;
    int hoveredBottleIndex{-1};
    int selectedBottleIndex{-1};

    QPushButton *btnPour{nullptr};
    QPushButton *btnGo{nullptr};

    QTimer *pourTimer{nullptr};
    int pourHeight{0};
    int maxPourHeight{120};

    QSoundEffect *pourSound{nullptr};
    double totalPureAlcoholMl{0.0};
};
#endif