#ifndef POLICEWIDGET_H
#define POLICEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSoundEffect>
#include <QShowEvent> // Добавлено для отслеживания показа экрана

class PoliceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PoliceWidget(QWidget *parent = nullptr);
    void playSiren();
    void stopSiren();

signals:
    void restartRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override; // Новая логика старта анимации

private slots:
    void updateScanner();
    void startFinalScenario();

public slots:
    void restartGame();
    void displayVerdict(double bac, const QString &lawText, int outcomeType);

private:
    double resultPromille;
    double displayedPromille;
    QString currentLawText;
    int currentOutcome;

    QLabel *lblAlcoResult;
    QLabel *lblVerdict;
    QPushButton *btnAction;

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

#endif // POLICEWIDGET_H