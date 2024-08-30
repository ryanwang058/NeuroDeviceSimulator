#include "eeg.h"
#include "device.h"
#include <QRandomGenerator>

EEG::EEG(Device *device, int index)
    : device(device), index(index), isConnected(false), waveGenerator(nullptr)
{
    waveGenerator = new WaveGenerator(this, index);
    connect(this, &EEG::sendConnLossToDevice, device, &Device::receiveEEGConnLoss);
    connect(this, &EEG::sendConnBackToDevice, device, &Device::receiveEEGConnBack);
}

EEG::~EEG() {
    delete waveGenerator;
    waveGenerator = nullptr;
}

int EEG::getIndex() const
{
    return index;
}

bool EEG::startConnection()
{
    return true;
}

bool EEG::stopConnection()
{
    return true;
}

void EEG::generateWave()
{
    waveMap = waveGenerator->generateWave();
}

QMap<QString, QPair<float, float>> EEG::getAmpFreqMap()
{
    return waveGenerator->getAmpFreqMap();
}

QList<float> EEG::getAlphaWave()
{
    return waveMap["alpha"];
}

QList<float> EEG::getBetaWave()
{
    return waveMap["beta"];
}

QList<float> EEG::getThetaWave()
{
    return waveMap["theta"];
}

QList<float> EEG::getDeltaWave()
{
    return waveMap["delta"];
}

QList<float> EEG::getCombinedWave()
{
    return waveMap["combined"];
}

QMap<QString, QList<float>> EEG::getWaveMap()
{
    return waveMap;
}

void EEG:: simulateConnLoss()
{
    isConnected = false;
    emit sendConnLossToDevice(this);
}

void EEG:: simulateConnBack()
{
    isConnected = true;
    emit sendConnBackToDevice(this);
}
