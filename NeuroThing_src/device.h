#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QDateTime>
#include <QThread>
#include <QDebug>
#include "eeg.h"
#include "sessionmanager.h"
#include "battery.h"
#include "defs.h"

class Device : public QObject
{
    Q_OBJECT
public:
    explicit Device(QObject *parent = nullptr);


public slots:
    bool startSession();
    void pauseSession();
    void resumeSession();
    void stopSession();
    void receiveEEGConnLoss(EEG* eeg);
    void receiveEEGConnBack(EEG* eeg);
    void onSessionFinished(int therapyStatus);
    void chargeBattery(int amount);
    void setDateTime(QDateTime dateTime);
    int  getBatteryLevel();
    QDateTime getDateTime();
    void handleDominantFrequency(QDateTime dateTime, QMap<QString, QList<float>> data);
    void handleRoundWaveData(QMap<QPair<int, QString>, QList<float>> data);
    void receiveSessionPhase(PHASES, int);
    QList<QMap<QString, QVariant>> getLoggedSessions();
    void onUploadToPCPressed(int selectedIndex);
    void pauseGuiSession();
    void resumeGuiSession();
    void disconnectEEG();
    void reconnectEEG();

signals:
    void startDisplayTimer();
    void sendConnLossToGUI(QString message);
    void sendConnBackToGUI(QString message);
    void sendPauseToGUI();
    void sendResumeToGUI();

    void sendStartToSession(QList<EEG*> eegList);
    void sendPauseToSession();
    void sendResumeToSession();
    void sendStopToSession();

    void sendSessionTermToGUI();
    void sendLoggedSessions();
    void sendRoundDataToMainWindow(const QMap<QPair<int, QString>, QList<float> > &roundWaveData);
    // QPair<int, QString>: QPair<eegIndex, waveType>, 
    // QList<float>: wave data
    // waveType: "alpha", "beta", "delta", "theta", "combined"
    void sendDataToTestUI(const QMap<QPair<int, QString>, QList<float> > &roundWaveData);
    void phaseProgress(PHASES, int);
    void sendSessionInfoToPC(const QMap<QString, QVariant> &sessionInfo);

private:
    SessionManager *sessionManager;
    QThread *sessionThread;
    QList<EEG*> eegList;
    QDateTime currentDateTime;

    QList<QMap<QString, QVariant>> loggedSessionList;

    QMap<QString, QList<float>> dominantFrequencyData = {};
    Battery *battery;

};

#endif // DEVICE_H
