#ifndef ITIMEPROVIDER_H
#define ITIMEPROVIDER_H

class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

    // Возвращает прошедшее время симуляции в часах
    virtual double getElapsedHours() const = 0;
};

#endif // ITIMEPROVIDER_H