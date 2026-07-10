#include "alcoholengine.h"
#include "itimeprovider.h"
#include <QFile>
#include <QTextStream>
#include <QStringConverter>
#include <QStringList>
#include <QDebug>
#include <algorithm>

AlcoholEngine::AlcoholEngine(ITimeProvider *timeProvider, QObject *parent)
    : QObject(parent), m_timeProvider(timeProvider)
{
    initLaws();
}

void AlcoholEngine::resetData() noexcept {
    m_consumedGrams = 0.0;
}

void AlcoholEngine::setProfile(const QString &gender, int weight, const QString &country) {
    m_gender = gender;
    m_weight = weight;
    m_country = country;
}

double AlcoholEngine::getConsumedGrams() const noexcept {
    return m_consumedGrams;
}

QVector<QString> AlcoholEngine::getAvailableCountries() const {
    auto countries = m_laws.keys().toVector();
    std::sort(countries.begin(), countries.end());
    return countries;
}

QString AlcoholEngine::getLastError() const noexcept {
    return m_lastError;
}

void AlcoholEngine::addAlcoholGrams(double grams) noexcept {
    m_consumedGrams += grams;
}

double AlcoholEngine::calculateBAC() const {
    if (!m_timeProvider || m_weight <= 30) {
        return 0.0;
    }

    const double hours = m_timeProvider->getElapsedHours();
    const double r = (m_gender == "Мужской") ? 0.7 : 0.6;
    constexpr double b = 0.15;

    const double C = (m_consumedGrams / (m_weight * r)) - (b * hours);

    return std::max(0.0, C);
}

void AlcoholEngine::generateVerdict() {
    const double bac = calculateBAC();
    const CountryLaw currentLaw = m_laws.value(m_country, {0.499, {{99.0, "Нарушение закона.", 1}}});

    QString currentLawText = "Закон не нарушен. Счастливого пути!";
    int currentOutcome = 0;

    if (bac > currentLaw.legalLimit) {
        for (const auto& [upToLimit, text, outcomeType] : currentLaw.tiers) {
            if (bac <= upToLimit) {
                currentLawText = text;
                currentOutcome = outcomeType;
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
        const QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith(QLatin1Char('#'))) continue;

        const auto parts = line.split(QLatin1Char('|'));
        if (parts.size() < 5) continue;

        CountryLaw law;
        const QString countryName = parts[0].trimmed();
        law.legalLimit = parts[1].toDouble();

        law.tiers.append({parts[2].toDouble(), parts[3].trimmed(), parts[4].toInt()});

        if (parts.size() >= 8) {
            law.tiers.append({parts[5].toDouble(), parts[6].trimmed(), parts[7].toInt()});
        }

        m_laws.insert(countryName, law);
    }
}