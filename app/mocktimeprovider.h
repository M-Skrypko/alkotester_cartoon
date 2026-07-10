#ifndef MOCKTIMEPROVIDER_H
#define MOCKTIMEPROVIDER_H

#include "itimeprovider.h"

class MockTimeProvider final : public ITimeProvider {
public:
    MockTimeProvider() = default;

    [[nodiscard]] double getElapsedHours() const override {
        return m_fakeHours;
    }

    void setHours(double hours) noexcept {
        m_fakeHours = hours;
    }

private:
    double m_fakeHours{0.0};
};

#endif