#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QtConcurrent>
#include "eeg.h"
#include "defs.h"
#include "signalprocessor.h"
#include <unistd.h>

#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>

class SessionManager : public QObject
{
    Q_OBJECT
public:
    explicit SessionManager(QDateTime dateTime = QDateTime());
    ~SessionManager();
    void runSession(QList<EEG*> eegList);
    bool isFinished = false;

signals:
    void dominantFrequencyReady(QDateTime startTime, QMap<QString, QList<float>> data);
    void roundDataReady(QMap<QPair<int, QString>, QList<float>> data);

    void sessionFinished(int therapyStatus); // signal to notify the Device that the session is complete
    void sendPhaseToDevice(PHASES, int);
    void sendPauseToDevice();
    void sendResumeToDevice();

public slots:
    void onSessionStarted(QList<EEG*> eegList);
    void onSessionPaused();
    void onSessionResumed();
    void onSessionStopped();
    
    // handle signals sent by SignalProcessor
    void handleRoundWaveData(QList<EEGWaveMap> data);
    void handleDominantFrequency(QList<EEGDomFreq> data);



private:
    QDateTime startTime;
    SignalProcessor* signalProcessor;
    int currentRound; // variable to keep tract of the round
    int therapyStatus = 0; // 0: running, 1: paused, 2: stopped
    QList<EEG*> eegList;
    QList<float> dominantFrequency;
    QMap<QString, QList<float>> dominantFrequencyMap;

    QMap<QPair<int, QString>, QList<float>> roundWaveData;
    QList<QMap<QPair<int, QString>, QList<float>>> roundWaveDataList;

    void startSession();
};

#endif // SESSIONMANAGER_H
