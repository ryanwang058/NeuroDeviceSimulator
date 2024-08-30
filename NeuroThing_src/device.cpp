#include "device.h"


Device::Device(QObject *parent)
    : QObject{parent}, sessionManager(nullptr), sessionThread(nullptr),
    currentDateTime(QDateTime::currentDateTime()), battery(new Battery())
{
    for (int i = 0; i < EEG_COUNT; i++)
        eegList.append(new EEG(this, i+1));

}


bool Device::startSession()
{
    // send signal to MainWindow to start the timer
    emit startDisplayTimer();

    for (EEG* eeg : eegList) {
        battery->reduceBattery(1);
        // emit phaseProgress(EEG_CONNECTION, eeg->getIndex());
        if (!eeg->startConnection()){
            qDebug() << "Connection to EEG " << eeg->getIndex() << "failed";
            QString message = "Connection to EEG " + QString::number(eeg->getIndex()) + " failed";
            emit sendConnLossToGUI(message);
            return false;
        }
    }


    sessionManager = new SessionManager(currentDateTime);

    // receive signals from sessionManager to handle session data and session finished
    connect(sessionManager, &SessionManager::sessionFinished, this, &Device::onSessionFinished);
    connect(sessionManager, &SessionManager::roundDataReady, this, &Device::handleRoundWaveData);
    connect(sessionManager, &SessionManager::dominantFrequencyReady, this, &Device::handleDominantFrequency);
    connect(sessionManager, &SessionManager::sendPauseToDevice, this, &Device::pauseGuiSession);
    connect(sessionManager, &SessionManager::sendResumeToDevice, this, &Device::resumeGuiSession);
    connect(sessionManager, &SessionManager::sendPhaseToDevice, this, &Device::receiveSessionPhase);
    
    // send signal to sessionManager to manipulate the session
    connect(this, &Device::sendStartToSession, sessionManager, &SessionManager::onSessionStarted);
    connect(this, &Device::sendPauseToSession, sessionManager, &SessionManager::onSessionPaused);
    connect(this, &Device::sendResumeToSession, sessionManager, &SessionManager::onSessionResumed);
    connect(this, &Device::sendStopToSession, sessionManager, &SessionManager::onSessionStopped);

    // sessionThread->start();
    emit sendStartToSession(eegList);

    return true;
}

// This is used to emit pause session to the gui to stop the timer. THis is done such that the timer stops only when the session has really stopped.
void Device:: pauseGuiSession(){
    emit sendPauseToGUI();
}

// This is used to emit resume session to the gui to resume the timer. THis is done such that the timer resumes only when the session has really stopped.
void Device:: resumeGuiSession(){
    emit sendResumeToGUI();
}

void Device::pauseSession()
{
    emit sendPauseToSession();
}


void Device::resumeSession()
{
    emit sendResumeToSession();
}


void Device::stopSession()
{
    emit sendStopToSession();
}


void Device::receiveEEGConnLoss(EEG *eeg)
{
    QString message = QString("EEG No. %1 got disconnected.").arg(eeg->getIndex());
    pauseSession();
    emit sendConnLossToGUI(message);
}


void Device::receiveEEGConnBack(EEG *eeg)
{
    QString message = QString("EEG No. %1 is back online.").arg(eeg->getIndex());
    emit sendConnBackToGUI(message);
    resumeSession();
}


/* When a session is finished normally or abnormally:
 *      1. EEG connections are terminated
 *      2. a signal is sent to MainWindow
 *      3. sessionThread is deleted
 *      4. sessionManager and sessionThread are set to null
 */
void Device::onSessionFinished(int therapyStatus)
{
    if (therapyStatus == 0 || therapyStatus == 2) {
        qInfo() << "Session Finished";
        qInfo() << "================================================================";
        for (EEG* eeg : eegList)
            eeg->stopConnection();

        sessionManager->disconnect();
        delete sessionManager;
        sessionManager = nullptr;

        emit sendSessionTermToGUI();
    }
    else if (therapyStatus == 1) {
        qInfo() << "Session Paused";
        qInfo() << "================================================================";
        sessionManager->isFinished = true;

    }
}


void Device::handleDominantFrequency(QDateTime dateTime, QMap<QString, QList<float>> data)
{    
    qInfo() << "++++++++++++++++++++++++++++++++++++++++";
    QMap<QString, QVariant> sessionInfoMap;
    sessionInfoMap["time"] = dateTime.toString("hh:mm:ss");
    sessionInfoMap["date"] = dateTime.toString("dd/MM/yyyy");

    sessionInfoMap["initialDominantFrequency"] = QVariant::fromValue(data["initialDominantFrequency"]);
    sessionInfoMap["finalDominantFrequency"] = QVariant::fromValue(data["finalDominantFrequency"]);
    loggedSessionList.append(sessionInfoMap);
}

void Device::handleRoundWaveData(QMap<QPair<int, QString>, QList<float>> data)
{
    emit sendRoundDataToMainWindow(data);
}


void Device::chargeBattery(int amount)
{
    battery->chargeBattery(amount);
}


void Device:: setDateTime(QDateTime dateTime){
    currentDateTime = dateTime;
}

int Device:: getBatteryLevel(){
    return battery->getBatteryLevel();
}

void Device:: receiveSessionPhase(PHASES phase, int index){
    switch(phase) {
    case TREATMENT:
        battery->reduceBattery(1);
        break;
    case WAVE_CALCULATION:
        battery->reduceBattery(1);
        break;
    default:
        break;
    }
    emit phaseProgress(phase, index);
}


QList<QMap<QString, QVariant>> Device:: getLoggedSessions(){
    return loggedSessionList;
}

void Device::onUploadToPCPressed(int selectedIndex){
    if (selectedIndex == -1){
        qDebug() << "No session selected.";
        return;
    } else {
        qDebug() << "User wants to upload session at index" << selectedIndex << "to PC";
        emit sendSessionInfoToPC(loggedSessionList.at(selectedIndex));
    }
}


QDateTime Device::getDateTime(){
    return currentDateTime;
}

void Device::disconnectEEG(){
    eegList.at(EEG_TO_DISCONNECT)->simulateConnLoss();
}

void Device::reconnectEEG(){
    eegList.at(EEG_TO_DISCONNECT)->simulateConnBack();
}
