#ifndef MOCKTIMEPROVIDER_H
#define MOCKTIMEPROVIDER_H

#include "itimeprovider.h"

class MockTimeProvider : public ITimeProvider {
public:
    MockTimeProvider() : m_fakeHours(0.0) {}

    double getElapsedHours() const override {
        return m_fakeHours;
    }

    // Жестко устанавливаем время симуляции в тестах
    void setHours(double hours) {
        m_fakeHours = hours;
    }

private:
    double m_fakeHours;
};

#endif // MOCKTIMEPROVIDER_H