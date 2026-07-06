#ifndef BARWIDGET_H
#define BARWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QTimer>
#include <QPixmap>
#include <QRect>
#include <QMouseEvent>
#include <QSoundEffect>

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
private slots:
    void startPouring();
    void updatePouring();

private:
    QList<DrinkItem> drinks;
    int hoveredBottleIndex;
    int selectedBottleIndex;

    QPushButton *btnPour;
    QPushButton *btnGo;

    QTimer *pourTimer;
    int pourHeight;
    int maxPourHeight;

    QSoundEffect *pourSound;
    double totalPureAlcoholMl;
};
#endif // BARWIDGET_H