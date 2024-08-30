#ifndef WAVEGENERATOR_H
#define WAVEGENERATOR_H

#include <QObject>
#include <QtMath>
#include <chrono>
#include <random>
#include <QMap>
#include "defs.h"

class WaveGenerator : public QObject
{
    Q_OBJECT
public:
    explicit WaveGenerator(QObject *parent = nullptr, int eegIndex = 0);

    QMap<QString, QList<float>> generateWave();
    QMap<QString, QPair<float, float>> getAmpFreqMap();
    QList<float> generateCustomWave(int minFreq, int maxFreq, QString waveBand);

signals:

private:
    int eegIndex;
    QMap<QString, QPair<float, float>> ampFreqMap; // key: alpha, beta, delta, thata; value: pair of amplitude-frequency
    QMap<QString, QList<float>> waveMap; // key: alpha, beta, delta, thata; value: wave frequency

    QList<float> generateBetaWave();
    QList<float> generateAlphaWave();
    QList<float> generateThetaWave();
    QList<float> generateDeltaWave();
};

#endif // WAVEGENERATOR_H
