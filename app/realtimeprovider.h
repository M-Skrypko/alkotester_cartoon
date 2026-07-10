#ifndef REALTIMEPROVIDER_H
#define REALTIMEPROVIDER_H

#include "itimeprovider.h"

class RealTimeProvider final : public ITimeProvider {
public:
    RealTimeProvider() = default;

    [[nodiscard]] double getElapsedHours() const override {
        return m_elapsedHours;
    }

    void advanceTime(double hours) noexcept {
        if (hours > 0.0) {
            m_elapsedHours += hours;
        }
    }

    void reset() noexcept {
        m_elapsedHours = 0.0;
    }

private:
    double m_elapsedHours{0.0};
};

#endif