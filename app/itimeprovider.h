#ifndef ITIMEPROVIDER_H
#define ITIMEPROVIDER_H

class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

    [[nodiscard]] virtual double getElapsedHours() const = 0;
};

#endif