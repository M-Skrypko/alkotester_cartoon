#include "alcoholmath.h"
#include <algorithm>

namespace AlcoholMath {
double calculateBAC(double pureAlcoholGrams, double weightKg, double genderFactor, double hoursElapsed) {
    if (weightKg <= 30.0 || genderFactor <= 0.0) {
        return 0.0;
    }

    double maxBAC = pureAlcoholGrams / (weightKg * genderFactor);

    double beta = 0.15;
    double eliminated = beta * hoursElapsed;
    double currentBAC = maxBAC - eliminated;
    return std::max(0.0, currentBAC);
}
}