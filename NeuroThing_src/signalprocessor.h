#ifndef SIGNALPROCESSOR_H
#define SIGNALPROCESSOR_H

#include <QObject>
#include "defs.h"
#include "eeg.h"

class SignalProcessor : public QObject
{
    Q_OBJECT
public:
    explicit SignalProcessor(QObject *parent = nullptr);
    ~SignalProcessor();
    void processDominantFrequency(QList<EEG*> eegList);
    
signals:
    void sendDominantFrequencyToSM(QList<EEGDomFreq> eegDomFreqList);
    void sendWaveMapToSM(QList<EEGWaveMap> eegWaveMapList);


private:
    float calculateDominantFrequency(QMap<QString, QPair<float, float>> data);
    WaveGenerator *waveGenerator;

    QList<EEGDomFreq> eegDomFreqList = QList<EEGDomFreq>();
    QList<EEGWaveMap> eegWaveMapList = QList<EEGWaveMap>();
};

#endif // SIGNALPROCESSOR_H
