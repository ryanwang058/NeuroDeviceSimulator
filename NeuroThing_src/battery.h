#ifndef BATTERY_H
#define BATTERY_H

#include <QObject>

class Battery : public QObject
{
    Q_OBJECT
public:
    explicit Battery(QObject *parent = nullptr);

    void reduceBattery(int amount);
    void chargeBattery(int amount);
    int getBatteryLevel();
private:
    int batteryLevel;
};

#endif // BATTERY_H
