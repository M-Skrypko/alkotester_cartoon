#ifndef ALCOHOLENGINE_H
#define ALCOHOLENGINE_H

#include <QObject>
#include <QString>
#include <QMap>
#include <QVector>

class ITimeProvider;

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
    explicit AlcoholEngine(ITimeProvider *timeProvider, QObject *parent = nullptr);

    void setProfile(const QString &gender, int weight, const QString &country);

    [[nodiscard]] double getConsumedGrams() const noexcept;
    [[nodiscard]] QVector<QString> getAvailableCountries() const;
    [[nodiscard]] QString getLastError() const noexcept;

public slots:
    void resetData() noexcept;
    void addAlcoholGrams(double grams) noexcept;
    void generateVerdict();

signals:
    void verdictReady(double bac, const QString &lawText, int outcomeType);

private:
    QString m_gender;
    QString m_country;
    int m_weight{0};
    double m_consumedGrams{0.0};

    ITimeProvider *m_timeProvider{nullptr};
    QMap<QString, CountryLaw> m_laws;
    QString m_lastError;

    void initLaws();
    [[nodiscard]] double calculateBAC() const;
};

#endif