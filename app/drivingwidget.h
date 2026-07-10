#ifndef DRIVINGWIDGET_H
#define DRIVINGWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSoundEffect>

class DrivingWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DrivingWidget(QWidget *parent = nullptr);
    void startSimulation(int durationSeconds);
    void setAlcoholGrams(double grams);
    QSoundEffect *sirenSound{nullptr};

signals:
    void policePulledOver(double timeInHours);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void updateAnimation();
    void handleTimeOut();

    QTimer *animationTimer{nullptr};
    QTimer *rideDurationTimer{nullptr};
    double speedMultiplier{14.0};
    int roadOffset{0};
    int timeLeft{10};
    double virtualTimeHours{0.0};

    bool policeAppearing{false};
    int policeX{0};
    int carX{120};
};

#endif