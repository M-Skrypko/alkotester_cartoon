#ifndef POLICEWIDGET_H
#define POLICEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSoundEffect>
#include <QShowEvent>

class PoliceWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PoliceWidget(QWidget *parent = nullptr);
    void playSiren();
    void stopSiren();

public slots:
    void restartGame();
    void displayVerdict(double bac, const QString &lawText, int outcomeType);

signals:
    void restartRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void updateScanner();
    void startFinalScenario();
    void startSirenWithDelay();

    double resultPromille{0.0};
    double displayedPromille{0.0};
    QString currentLawText;
    int currentOutcome{0};

    QLabel *lblAlcoResult{nullptr};
    QLabel *lblVerdict{nullptr};
    QPushButton *btnAction{nullptr};
    QSoundEffect *sirenSound{nullptr};
    QTimer *scanTimer{nullptr};
    QWidget *infoCard{nullptr};
    QLabel *lblResultText{nullptr};
    QPushButton *btnRestart{nullptr};
    QWidget *finalOverlay{nullptr};
    QLabel *lblFinalText{nullptr};
    QPushButton *btnFinalRestart{nullptr};
    QGraphicsOpacityEffect *fadeEffect{nullptr};
};

#endif