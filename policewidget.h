#ifndef POLICEWIDGET_H
#define POLICEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSoundEffect>

struct PunishmentTier {
    double upToLimit;
    QString text;
    int outcomeType;
};


struct CountryLaw {
    double legalLimit;
    QVector<PunishmentTier> tiers;
};
class PoliceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PoliceWidget(QWidget *parent = nullptr);
    void calculateAndShowResult(QString country, QString gender, int weight, double alcoholGrams, double timeInHours);
    void playSiren();
    void stopSiren();

signals:
void restartRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateScanner();
    void startFinalScenario();
public slots:
    void restartGame();
private:
    double resultPromille;
    double displayedPromille;
    QString currentLawText;
    QLabel *lblAlcoResult;
    QLabel *lblVerdict;
    QPushButton *btnAction;
    int currentOutcome;
    QSoundEffect *sirenSound;
    void startSirenWithDelay();
    QTimer *scanTimer;
    QWidget *infoCard;
    QLabel *lblResultText;
    QPushButton *btnRestart;
    QWidget *finalOverlay;
    QLabel *lblFinalText;
    QPushButton *btnFinalRestart;

    QGraphicsOpacityEffect *fadeEffect;
};

#endif