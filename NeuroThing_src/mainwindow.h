#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QDebug>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QListWidget>
#include <QLCDNumber>
#include <QProgressBar>
#include <QTableWidget>
#include <QTimer>
#include <QTime>
#include <QThread>
#include "device.h"
#include "pc.h"
#include "testui.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum MenuOptions {
    Treatment,
    SessionHist,
    SetDateTime
};



const QColor blue_ind_on_color("#03c2fc");
const QColor blue_ind_off_color("#3859ab");

const QColor red_ind_on_color("#f00000");
const QColor red_ind_off_color("#913411");

const QColor green_ind_on_color("#00f000");
const QColor green_ind_off_color("#17700f");


    class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void startNewSession();
    void pauseSession();
    void resumeSession();
    void stopSession();
    // QPair<int, QString>: QPair<eegIndex, waveType>,
    // QList<float>: wave data
    // waveType: "alpha", "beta", "delta", "theta", "combined"
    void sendDataToTestUI(const QMap<QPair<int, QString>, QList<float> > &roundWaveData);

private:
    Ui::MainWindow *ui;

    MenuOptions menuChoice = Treatment;
    bool deviceOn;
    bool sessionPaused;
    bool sessionStarted;

    Device *device;
    QThread *deviceThread;
    TestUI *testUI;
    PC *pc;
    int remainedTime;
    int connLostTimeSeconds = 0;
    int selectedSessionHist;
    bool connLost = false;
    QTimer remainedTimer;
    QTimer connLostTimer;
    QTimer dateTimeTimer;
    QTimer chargerTimer;

private slots:
    void chooseMenu(MenuOptions menuChoice);
    void togglePower();
    void playPressed();
    void pausePressed();
    void stopPressed();

    void startTimer();
    void stopTimer();
    void pauseTimer();
    void resumeTimer();

    void startDevice();
    void shutDown();
    void updateBatteryLevel();

    void handleConnectionLost(QString message);
    void handleSessionTerm();
    void handleConnectionBack(QString message);
    void handleRoundWaveData(const QMap<QPair<int, QString>, QList<float> > &roundWaveData);
    void phaseUpdate(PHASES phase, int eegNum);
    void updateProgressBar();
    void viewDeviceSession();
    void on_tableView_clicked(const QModelIndex &index);
    void updateChargingStatus();

};

#endif // MAINWINDOW_H
