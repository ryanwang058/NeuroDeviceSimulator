#ifndef EEG_H
#define EEG_H

#include <QObject>
#include <QDebug>
#include "defs.h"
#include "wavegenerator.h"

class Device;

class EEG : public QObject
{
    Q_OBJECT
public:
    explicit EEG(Device *device = nullptr, int index = 0);
    ~EEG();

    int getIndex() const;

    bool startConnection();
    bool stopConnection();

    QMap<QString, QPair<float, float>> getAmpFreqMap();
    void generateWave();
    QList<float> getAlphaWave();
    QList<float> getBetaWave();
    QList<float> getThetaWave();
    QList<float> getDeltaWave();
    QList<float> getCombinedWave();
    QMap<QString, QList<float>> getWaveMap();
    void simulateConnLoss();
    void simulateConnBack();

signals:
    void sendConnLossToDevice(EEG* eeg);
    void sendConnBackToDevice(EEG* eeg);

private:
    QMap<QString, QList<float>> waveMap;
    QMap<QString, QPair<float, float>> ampFreqMap;
    Device *device;
    int index;
    bool isConnected;
    WaveGenerator *waveGenerator;
};

#endif // EEG_H
