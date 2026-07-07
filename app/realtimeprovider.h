#ifndef REALTIMEPROVIDER_H
#define REALTIMEPROVIDER_H

#include "itimeprovider.h"

class RealTimeProvider : public ITimeProvider {
public:
    RealTimeProvider() : m_elapsedHours(0.0) {}

    double getElapsedHours() const override {
        return m_elapsedHours;
    }

    void advanceTime(double hours) {
        if (hours > 0.0) {
            m_elapsedHours += hours;
        }
    }

    void reset() {
        m_elapsedHours = 0.0;
    }

private:
    double m_elapsedHours;
};

#endif // REALTIMEPROVIDER_H