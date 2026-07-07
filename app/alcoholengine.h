#ifndef ALCOHOLENGINE_H
#define ALCOHOLENGINE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class ITimeProvider; // Объявляем, что такой класс существует

struct PunishmentTier {
    double upToLimit;
    QString text;
    int outcomeType;
};

struct CountryLaw {
    double legalLimit;
    QVector<PunishmentTier> tiers;
};

class AlcoholEngine : public QObject {
    Q_OBJECT
public:
    // Теперь движок рождается только если ему дать провайдер времени!
    explicit AlcoholEngine(ITimeProvider *timeProvider, QObject *parent = nullptr);

    void setProfile(const QString &gender, int weight, const QString &country);

    double getConsumedGrams() const;
    QVector<QString> getAvailableCountries() const;
    QString getLastError() const;

public slots:
    void resetData();
    void addAlcoholGrams(double grams);

    // Метод, который генерирует результат на основе текущего времени
    void generateVerdict();

signals:
    void verdictReady(double bac, const QString &lawText, int outcomeType);

private:
    QString m_gender;
    QString m_country;
    int m_weight;
    double m_consumedGrams;

    ITimeProvider *m_timeProvider; // Храним зависимость
    QMap<QString, CountryLaw> m_laws;
    QString m_lastError;

    void initLaws();
    double calculateBAC() const;
};

#endif // ALCOHOLENGINE_H