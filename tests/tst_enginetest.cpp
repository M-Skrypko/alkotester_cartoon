#include <QtTest>
#include <QSignalSpy>
#include <cmath>
#include "alcoholengine.h"
#include "mocktimeprovider.h"

class EngineTest : public QObject
{
    Q_OBJECT

private slots:
    // Будет выполняться перед каждым тестом (сброс настроек)
    void init();

    // Тест 1: Трезвый водитель (0 грамм = 0 промилле)
    void test_soberDriver();

    // Тест 2: Мужчина 90 кг, 40 грамм спирта (проверка базовой математики)
    void test_manStandardCalculation();

    // Тест 3: Элиминация времени (выпивка -> промотка на 2 часа через Mock -> вычитание 0.3 промилле)
    void test_timeElimination();

private:
    MockTimeProvider *m_mockTime;
    AlcoholEngine *m_engine;
};

void EngineTest::init()
{
    // Перед каждым тестом создаем чистое окружение
    m_mockTime = new MockTimeProvider();
    m_engine = new AlcoholEngine(m_mockTime);
}

void EngineTest::test_soberDriver()
{
    // Настраиваем профиль трезвого водителя
    m_engine->setProfile("Мужской", 90, "Беларусь");
    m_engine->addAlcoholGrams(0.0); // ничего не пил
    m_mockTime->setHours(0.0);      // время сразу после бара

    // Подключаем шпиона к сигналу движка verdictReady
    QSignalSpy spy(m_engine, &AlcoholEngine::verdictReady);

    // Просим движок посчитать результат
    m_engine->generateVerdict();

    // Проверяем: сигнал должен был прийти ровно 1 раз
    QCOMPARE(spy.count(), 1);

    // Достаем аргументы, которые движок передал вместе с сигналом
    QList<QVariant> arguments = spy.takeFirst();
    double bac = arguments.at(0).toDouble();       // Первый аргумент: промилле (bac)
    QString lawText = arguments.at(1).toString();  // Второй аргумент: текст вердикта

    // Проверки (Ассерты)
    QCOMPARE(bac, 0.0); // Должно быть строго 0.0
    QCOMPARE(lawText, QString("Закон не нарушен. Счастливого пути!"));
}

void EngineTest::test_manStandardCalculation()
{
    // Входные данные из твоего отчета: Мужчина, 90 кг, 40г чистого этанола
    // Эталонный расчет: C = 40 / (90 * 0.7) = 40 / 63 = 0.63492...
    m_engine->setProfile("Мужской", 90, "Беларусь");
    m_engine->addAlcoholGrams(40.0);
    m_mockTime->setHours(0.0); // Сразу после бара, печень еще не выводила алкоголь

    QSignalSpy spy(m_engine, &AlcoholEngine::verdictReady);
    m_engine->generateVerdict();

    QCOMPARE(spy.count(), 1);
    double bac = spy.takeFirst().at(0).toDouble();

    // При работе с числами с плавающей точкой (double) сравнивать напрямую через QCOMPARE нельзя
    // из-за микроскопических погрешностей процессора. Используем проверку допуска (дельта 0.001)
    double expectedBAC = 40.0 / (90.0 * 0.7);
    QVERIFY(std::abs(bac - expectedBAC) < 0.001);
}

void EngineTest::test_timeElimination()
{
    // Мужчина, 90 кг, 40г этанола (Стартовое значение ~0.6349 промилле)
    m_engine->setProfile("Мужской", 90, "Беларусь");
    m_engine->addAlcoholGrams(40.0);

    // МАГИЯ МОКА: Прошло 2 часа реального времени поездки.
    // Вместо ожидания у монитора, мы перематываем время за 1 наносекунду!
    m_mockTime->setHours(2.0);

    // За это время печень должна вывести: 0.15 * 2 = 0.3 промилле
    // Ожидаемый остаток: 0.6349 - 0.3000 = 0.3349 промилле

    QSignalSpy spy(m_engine, &AlcoholEngine::verdictReady);
    m_engine->generateVerdict();

    QCOMPARE(spy.count(), 1);
    double bac = spy.takeFirst().at(0).toDouble();

    double expectedBAC = (40.0 / (90.0 * 0.7)) - (0.15 * 2.0);

    // Проверяем, что алкоголь честно вычелся за время случайной поездки
    QVERIFY(std::abs(bac - expectedBAC) < 0.001);
}

// Запуск тестового класса
QTEST_APPLESS_MAIN(EngineTest)

#include "tst_enginetest.moc"