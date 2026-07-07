#include "alcoholengine.h"
#include "itimeprovider.h" // Подключаем интерфейс
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QStringList>
#include <QDebug>
#include <algorithm>

AlcoholEngine::AlcoholEngine(ITimeProvider *timeProvider, QObject *parent)
    : QObject(parent), m_timeProvider(timeProvider)
{
    resetData();
    initLaws();
}

void AlcoholEngine::resetData() {
    m_consumedGrams = 0.0;
}

void AlcoholEngine::setProfile(const QString &gender, int weight, const QString &country) {
    m_gender = gender;
    m_weight = weight;
    m_country = country;
}

double AlcoholEngine::getConsumedGrams() const {
    return m_consumedGrams;
}

QVector<QString> AlcoholEngine::getAvailableCountries() const {
    QVector<QString> countries = m_laws.keys().toVector();
    std::sort(countries.begin(), countries.end());
    return countries;
}

QString AlcoholEngine::getLastError() const {
    return m_lastError;
}

void AlcoholEngine::addAlcoholGrams(double grams) {
    m_consumedGrams += grams;
}

// Формула Видмарка теперь запрашивает время у интерфейса
double AlcoholEngine::calculateBAC() const {
    double hours = m_timeProvider->getElapsedHours();

    if (m_weight <= 30) return 0.0;

    double r = (m_gender == "Мужской") ? 0.7 : 0.6;
    double b = 0.15;
    double C = (m_consumedGrams / (m_weight * r)) - (b * hours);

    return std::max(0.0, C); // Предотвращение отрицательных промилле
}

void AlcoholEngine::generateVerdict() {
    double bac = calculateBAC();
    CountryLaw currentLaw = m_laws.value(m_country, {0.499, {{99.0, "Нарушение закона.", 1}}});

    QString currentLawText;
    int currentOutcome = 0;

    if (bac <= currentLaw.legalLimit) {
        currentLawText = "Закон не нарушен. Счастливого пути!";
        currentOutcome = 0;
    } else {
        for (const PunishmentTier &tier : currentLaw.tiers) {
            if (bac <= tier.upToLimit) {
                currentLawText = tier.text;
                currentOutcome = tier.outcomeType;
                break;
            }
        }
    }

    emit verdictReady(bac, currentLawText, currentOutcome);
}

void AlcoholEngine::initLaws() {
    m_laws.clear();
    m_lastError.clear();

    QFile file(":/laws.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        m_lastError = "Не удалось загрузить базу законов!\nУбедитесь, что файл laws.txt добавлен в ресурсы (qrc).";
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#")) continue;

        QStringList parts = line.split('|');
        if (parts.size() < 5) continue;

        CountryLaw law;
        QString countryName = parts[0].trimmed();
        law.legalLimit = parts[1].toDouble();

        PunishmentTier tier1;
        tier1.upToLimit = parts[2].toDouble();
        tier1.text = parts[3].trimmed();
        tier1.outcomeType = parts[4].toInt();
        law.tiers.append(tier1);

        if (parts.size() >= 8) {
            PunishmentTier tier2;
            tier2.upToLimit = parts[5].toDouble();
            tier2.text = parts[6].trimmed();
            tier2.outcomeType = parts[7].toInt();
            law.tiers.append(tier2);
        }

        m_laws.insert(countryName, law);
    }
    file.close();
}