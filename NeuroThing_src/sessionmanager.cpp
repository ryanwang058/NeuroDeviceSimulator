#include "sessionmanager.h"

SessionManager::SessionManager(QDateTime dateTime)
    : signalProcessor(new SignalProcessor(this)), currentRound(0)
{   
    dominantFrequency = QList<float>();
    startTime = dateTime;
    connect(signalProcessor, &SignalProcessor::sendWaveMapToSM, this, &SessionManager::handleRoundWaveData);
    connect(signalProcessor, &SignalProcessor::sendDominantFrequencyToSM, this, &SessionManager::handleDominantFrequency);
}

SessionManager::~SessionManager() {
    delete signalProcessor;
}



void SessionManager::onSessionStarted(const QList<EEG*> eegList)
{
    qInfo() << "SessionManager: Session Started";
    this->eegList = eegList;
    therapyStatus = 0; // running

    startSession();

}

void SessionManager::startSession(){
    // QList<EEG*> eegListCopy = eegList;

    // Set up the future watcher
    QFutureWatcher<void>* watcher = new QFutureWatcher<void>(this);
    connect(watcher, &QFutureWatcher<void>::finished, this, [this, watcher]() {
        emit sessionFinished(therapyStatus);
        watcher->deleteLater(); // Clean up the watcher
    });

    // Start the concurrent task and set the watcher to monitor it
    QFuture<void> future = QtConcurrent::run([&]() {
        runSession(eegList);
    });
    watcher->setFuture(future);

}


void SessionManager::runSession(QList<EEG *> eegList)
{
    for (int i = currentRound; i < TREATMENT_ROUNDS + 1; ++i) {

        switch (therapyStatus) {
            case 0:
                currentRound++;
                qInfo() << "----------------------------------------------------------------";
                if (currentRound != TREATMENT_ROUNDS + 1)
                    qInfo() << "SessionManager: Starting Round" << i+1 << "of Treatment";
                else
                    qInfo() << "SessionManager: Post-Session Analysis -- Analyzing Brain Wave and Get Dominant Frequency(5s)";


                emit sendPhaseToDevice(WAVE_CALCULATION, i);
                roundWaveData.clear();

                // Sleep 5s for analysis
                // Dominant Frequency is calculated -> Will be saved and logged
                // Round Wave Data is updated -> Will be sent to TestUI to plot waves
                QThread::msleep(3000);
                qInfo() << "SessionManager: Analyzing Brain Wave and Get Dominant Frequency(5s)";
                signalProcessor->processDominantFrequency(eegList);

                if (currentRound == TREATMENT_ROUNDS + 1){
                    QThread::msleep(2000);
                    return;
                }

                if (therapyStatus == 0){
                    qInfo() << "Plotting Brain Wave...";
                    QThread::msleep(2000);
                }
                
                // Sleep 1s for treatment
                qInfo() << "SessionManager: Starting Treatment for 1 sec";
                QThread::msleep(300);

                for (EEG* eeg : eegList) {
                    if (therapyStatus != 0){
                        currentRound--;
                        break;
                    }

                    switch (i) {
                        case 0:
                        case 1:
                        case 2:
                        case 3:{
                            int frequency = (i + 1) * BRAINWAVE_OFFSET; // 5hz, 10hz, 15hz, 20hz for (i = 0, 1, 2, 3)
                            QThread::msleep(100);
                            qInfo() << "Applying treatment of" << frequency << "Hz (offset) +" << dominantFrequency[i] << "Hz (dominant) through EEG#" << eeg->getIndex();
                            emit sendPhaseToDevice(TREATMENT, i);
                            break;
                        }
                        default:
                            break;
                    }
                }
                break;
            case 1:
                qInfo () << "Session Manager: Pausing session";
                emit sendPauseToDevice();
                return;
            case 2:
                qInfo() << "SessionManager: Session Stopped";
                return;
        }
    }

    if(therapyStatus == 2) emit sessionFinished(therapyStatus);

}

void SessionManager::onSessionPaused()
{
    therapyStatus = 1;
}


void SessionManager::onSessionResumed()
{
    qInfo() << "Session Manager: Resuming session from Round " << currentRound + 1 << "(Overwrite Previous Round)";
    therapyStatus = 0; // running

    emit sendResumeToDevice();

    startSession();
}

/*
Session Stops Manually
when Device sendStopToSession, the session stops
SessionManager will tell Device session finished
*/
void SessionManager::onSessionStopped()
{   
    // if the session is paused
    if (therapyStatus == 1) {
        therapyStatus = 2;
        if (isFinished)  emit sessionFinished(therapyStatus);
        return;
    }
    therapyStatus = 2;
}


void SessionManager::handleDominantFrequency(QList<EEGDomFreq> data) 
{
    if (currentRound == 1) {
        dominantFrequency.clear();
        for (EEGDomFreq eegDomFreq : data) {
            dominantFrequency.append(eegDomFreq.domFreq);
        }
        dominantFrequencyMap.insert("initialDominantFrequency", dominantFrequency);
    }
    else if (currentRound == TREATMENT_ROUNDS + 1) {
        emit sendPhaseToDevice(WAVE_CALCULATION, TREATMENT_ROUNDS);

        for (EEGDomFreq eegDomFreq : data) {
            dominantFrequency.append(eegDomFreq.domFreq);
        }
        dominantFrequencyMap.insert("finalDominantFrequency", dominantFrequency);

        emit dominantFrequencyReady(startTime, dominantFrequencyMap);
    }
    else {
        dominantFrequency.clear();
        for (EEGDomFreq eegDomFreq : data) {
            dominantFrequency.append(eegDomFreq.domFreq);
        }
    }
}


void SessionManager::handleRoundWaveData(QList<EEGWaveMap> data) {
    roundWaveData.clear();

    for (EEGWaveMap eegWaveMap : data) {
        int eegIndex = eegWaveMap.eegIndex;
        QMap<QString, QList<float>> waveMap = eegWaveMap.waveMap;

        roundWaveData.insert({eegIndex, "alpha"}, waveMap["alpha"]);
        roundWaveData.insert({eegIndex, "beta"}, waveMap["beta"]);
        roundWaveData.insert({eegIndex, "delta"}, waveMap["delta"]);
        roundWaveData.insert({eegIndex, "theta"}, waveMap["theta"]);
        roundWaveData.insert({eegIndex, "combined"}, waveMap["combined"]);  
        
    }
    emit roundDataReady(roundWaveData);  
}

