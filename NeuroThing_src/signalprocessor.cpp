# include "signalprocessor.h"
# include "eeg.h"

SignalProcessor::SignalProcessor(QObject *parent)
    : QObject{parent}, waveGenerator(nullptr)
{
    waveGenerator = new WaveGenerator();
}

SignalProcessor::~SignalProcessor() {
    delete waveGenerator;
    waveGenerator = nullptr;
}

void SignalProcessor::processDominantFrequency(QList<EEG*> eegList) {
    eegWaveMapList.clear();
    eegDomFreqList.clear();
    for (EEG* eeg : eegList) {
        eeg->generateWave();
        QMap<QString, QList<float>> waveMap = eeg->getWaveMap();
        QMap<QString, QPair<float, float>> ampFreqMap = eeg->getAmpFreqMap();
        if (ampFreqMap.isEmpty()) {
            qWarning() << "processDominantFrequency: ampfreqMap is empty";
        }

        // get Dominant Frequency
        EEGDomFreq eegDomFreq;
        eegDomFreq.eegIndex = eeg->getIndex();
        float dominantFreq = calculateDominantFrequency(ampFreqMap);
        eegDomFreq.domFreq = dominantFreq;

        eegDomFreqList.append(eegDomFreq);
        
        // generate combined wave using dominant frequency        
        EEGWaveMap eegWaveMap;
        QList<float> combinedWave = waveGenerator->generateCustomWave(dominantFreq, dominantFreq, "combined");
        waveMap.insert("combined", combinedWave);
        eegWaveMap.eegIndex = eeg->getIndex();
        eegWaveMap.waveMap = waveMap;

        eegWaveMapList.append(eegWaveMap);
    }

    emit sendDominantFrequencyToSM(eegDomFreqList);
    emit sendWaveMapToSM(eegWaveMapList);

    
}


float SignalProcessor::calculateDominantFrequency(QMap<QString, QPair<float, float>> ampFreqMap) {
    float dominantFreq = 0.0f;
    float numerator = 0.0f;
    float denominator = 0.0f;
    for (const QPair<float, float>& pair : ampFreqMap) {
        float amp = pair.first;
        float freq = pair.second;
        numerator += freq * std::pow(amp, 2);
        denominator += std::pow(amp, 2);
    }
    if (denominator == 0.0f) {
        qWarning() << "Err: Denominator is zero; cannot calculate dominant frequency";
    }
    dominantFreq = numerator / denominator;
    return dominantFreq;
}


