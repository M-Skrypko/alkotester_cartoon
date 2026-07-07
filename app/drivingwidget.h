#ifndef DRIVINGWIDGET_H
#define DRIVINGWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QSoundEffect>
class DrivingWidget : public QWidget
{
    Q_OBJECT

public:
    QSoundEffect *sirenSound;
    explicit DrivingWidget(QWidget *parent = nullptr);
    void startSimulation(int durationSeconds);
    void setAlcoholGrams(double grams);

signals:

    void policePulledOver(double timeInHours);

protected:
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

private slots:
    void updateAnimation();
    void handleTimeOut();

private:
    QTimer *animationTimer;
    QTimer *rideDurationTimer;
double speedMultiplier;
    int roadOffset;
    int timeLeft;
    double virtualTimeHours;

    bool policeAppearing;
    int policeX;
    int carX;
};

#endif // DRIVINGWIDGET_H