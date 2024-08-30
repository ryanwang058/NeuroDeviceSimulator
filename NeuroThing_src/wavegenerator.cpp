#include "wavegenerator.h"
#include <QDebug>

WaveGenerator::WaveGenerator(QObject *parent, int eegIndex)
    : QObject{parent}, eegIndex(eegIndex)
{
    
}

QList<float> WaveGenerator::generateBetaWave()
{
    return generateCustomWave(12, 30, "beta");
}

QList<float> WaveGenerator::generateAlphaWave()
{
    return generateCustomWave(8, 12, "alpha");
}

QList<float> WaveGenerator::generateThetaWave()
{
    return generateCustomWave(4, 8, "theta");
}

QList<float> WaveGenerator::generateDeltaWave()
{
    return generateCustomWave(1, 4, "delta");
}

QMap<QString, QList<float>> WaveGenerator::generateWave()
{

    QList<float> betaWave = generateBetaWave();
    QList<float> alphaWave = generateAlphaWave();
    QList<float> thetaWave = generateThetaWave();
    QList<float> deltaWave = generateDeltaWave();

    waveMap.insert("beta", betaWave);
    waveMap.insert("alpha", alphaWave);
    waveMap.insert("theta", thetaWave);
    waveMap.insert("delta", deltaWave);

    return waveMap;
}

QMap<QString, QPair<float, float>> WaveGenerator::getAmpFreqMap()
{
    return ampFreqMap;
}

QList<float> WaveGenerator::generateCustomWave(int minFreq, int maxFreq, QString waveBand)
{
    QList<float> sampleList;

    // Use the time value as the seed for the random number generator
    auto currentTime = std::chrono::system_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();
    std::default_random_engine generator(static_cast<unsigned int>(time) * (eegIndex+1));
    std::uniform_real_distribution<float> distribution(-0.1f, 0.1f);

    float amp_1 = (float) 1 / 3 + distribution(generator);
    float amp_2 = (float) 1 / 3 + distribution(generator);
    float amp_3 = (float) 1 / 3 + distribution(generator);

    float freq_1 = minFreq;
    float freq_2 = 0.5*(minFreq + maxFreq);
    float freq_3 = maxFreq;

    if (waveBand != "combined"){
        freq_1 *= (1 + distribution(generator));
        freq_2 *= (1 + distribution(generator));
        freq_3 *= (1 + distribution(generator));
    }

    ampFreqMap.insert(waveBand, {(amp_1 + amp_2 + amp_3), (freq_1 + freq_2 + freq_3) / 3.0});

    for (int i = 0; i < SAMPLE_COUNT; ++i) {
        float x = (float) i / SAMPLE_RATE;
        float value = amp_1 * qSin(2 * M_PI * freq_1 * x) +
                      amp_2 * qSin(2 * M_PI * freq_2 * x) +
                      amp_3 * qSin(2 * M_PI * freq_3 * x);
        sampleList.append(value);
    }

    return sampleList;
}


