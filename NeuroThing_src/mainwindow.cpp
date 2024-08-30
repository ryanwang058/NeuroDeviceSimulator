#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <functional>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), menuChoice(Treatment), deviceOn(false),
    sessionPaused(false), sessionStarted(false), testUI(nullptr), remainedTime(0), selectedSessionHist(-1)
{
    ui->setupUi(this);
    connect(ui->Menu_select, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index)
            {
        switch (index) {
        case 0:
            chooseMenu(Treatment);
            break;
        case 1:
            chooseMenu(SessionHist);
            break;
        case 2:
            chooseMenu(SetDateTime);
            break;
        default:
            // Handle default case
            break;
        } });

    connect(ui->deviceSessionData, &QTableView::clicked, this, &MainWindow::on_tableView_clicked);

    // Create a thread for the device
    device = new Device();
    deviceThread = new QThread();
    deviceThread->start();
    device->moveToThread(deviceThread);

    QDateTime dateTimeValue = device->getDateTime();
    ui->dateTimeEdit->setDateTime(dateTimeValue);

    // Device signals
    connect(device, &Device::startDisplayTimer, this, &MainWindow::startTimer);
    connect(device, &Device::sendConnLossToGUI, this, &MainWindow::handleConnectionLost);
    connect(device, &Device::sendConnBackToGUI, this, &MainWindow::handleConnectionBack);
    connect(device, &Device::sendSessionTermToGUI, this, &MainWindow::handleSessionTerm);
    connect(device, &Device::phaseProgress, this, &MainWindow::phaseUpdate);
    connect(device, &Device::sendRoundDataToMainWindow, this, &MainWindow::handleRoundWaveData);
    connect(device, &Device::sendPauseToGUI, this, &MainWindow::pauseTimer);
    connect(device, &Device::sendResumeToGUI, this, &MainWindow::resumeTimer);
    connect(this, &MainWindow::pauseSession, device, &Device::pauseSession);
    connect(this, &MainWindow::stopSession, device, &Device::stopSession);

    // Create a thread for the testUI
    if (TESTUI_ON){
        if (!testUI){
            testUI = new TestUI();
            connect(this, &MainWindow::sendDataToTestUI, testUI, &TestUI::updateData);
            testUI->setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint);


        } else {
            testUI->clearData();
        }
    }

    // button actions
    connect(ui->play_btn, &QPushButton::released, this, &MainWindow::playPressed);
    connect(ui->pause_btn, &QPushButton::released, this, &MainWindow::pausePressed);
    connect(ui->stop_btn, &QPushButton::released, this, &MainWindow::stopPressed);
    connect(ui->power_btn, &QPushButton::pressed, this, &MainWindow::togglePower);
    connect(ui->eeg_disconnect_btn, &QPushButton::released, device, &Device::disconnectEEG);
    connect(ui->eeg_reconnect_btn, &QPushButton::released, device, &Device::reconnectEEG);

    updateBatteryLevel();

    // remained timer actions
    remainedTimer.setInterval(1000);
    QObject::connect(&remainedTimer, &QTimer::timeout, [&](){
        // Increment the elapsed time by 1 second
        remainedTime--;

        // Calculate minutes and seconds
        int minutes = remainedTime / 60;
        int seconds = remainedTime % 60;
        QTime time(0, minutes, seconds);
        // update the timer
        ui->remainedTimeLCD->display(time.toString("mm:ss"));
    });

    connect(&remainedTimer, &QTimer::timeout, this, &MainWindow::updateProgressBar);

    //connection Lost Timer
    ui->ConnLostTime_label->setVisible(false);
    ui->ConnLostTime_value->setVisible(false);
    connLostTimer.setInterval(1000);
    QObject::connect(&connLostTimer, &QTimer::timeout, [&]()
    {
        // Increment the elapsed time by 1 second
        connLostTimeSeconds++;
        if(connLostTimeSeconds % 2 == 0){
            qInfo() << "BEEP BEEP....";   // alarm every 2 seconds
        }
        if(connLostTimeSeconds == MAX_CONNECTION_TIMEOUT){
            QString message = "Connection lost for " + QString::number(MAX_CONNECTION_TIMEOUT) + " seconds. Shutting Down...";
            ui->treatment_screen->setText(message);
            QTimer::singleShot(3000, this, &MainWindow::shutDown);
            return;
        }
        // Calculate minutes and seconds
        int minutes = connLostTimeSeconds / 60;
        int seconds = connLostTimeSeconds % 60;

        // Update the label text with the formatted elapsed time
        ui->ConnLostTime_value->setText(QString("%1:%2")
                          .arg(minutes, 2, 10, QLatin1Char('0'))
                          .arg(seconds, 2, 10, QLatin1Char('0')));
    });

    //dateTime display timer
    dateTimeTimer.setInterval(1000);
    QObject::connect(&dateTimeTimer, &QTimer::timeout, [&]()
    {
        QDateTime dateTimeValue = device->getDateTime();
        QDateTime newTime = dateTimeValue.addSecs(1);
        device->setDateTime(newTime);
        if(deviceOn) {
            ui->dateTimeDisplay->setText(newTime.toString("yyyy-MM-dd hh:mm:ss"));
        }else{
            ui->dateTimeDisplay->setText("");
        }
    });

    //chargerTimer
    chargerTimer.setInterval(BATTERY_INC_INTERVAL);
    QObject::connect(&chargerTimer, &QTimer::timeout, [&]()
    {
            device->chargeBattery(BATTERY_INC);
            if(deviceOn) updateBatteryLevel();
    });

    updateChargingStatus();
    connect(ui->chargeBtn, &QCheckBox::clicked, this, &MainWindow::updateChargingStatus);

    pc = new PC(this, ui->tableWidget, ui->sessionDisplay);
    connect(ui->previousButton, &QPushButton::released, this->pc, &PC::onPreviousPressed);
    connect(ui->nextButton, &QPushButton::released, this->pc, &PC::onNextPressed);
    connect(ui->uploadButton, &QPushButton::released, this, [=]() {
        if (this->device)
            this->device->onUploadToPCPressed(selectedSessionHist);
    });
    connect(device, &Device::sendSessionInfoToPC, this->pc, &PC::handleSessionInfoData);

}


MainWindow::~MainWindow()
{
    deviceThread->quit();
    deviceThread->wait();
    delete deviceThread;
    delete device;
    delete ui;
}


void MainWindow::chooseMenu(MenuOptions choice)
{
    menuChoice = choice;
    switch (choice)
    {
    case Treatment:
        ui->stackedWidget->setCurrentIndex(0);
        break;
    case SessionHist:
        ui->stackedWidget->setCurrentIndex(1);
        viewDeviceSession();
        break;
    case SetDateTime:
        ui->stackedWidget->setCurrentIndex(2);
        break;
    default:
        break;
    }
    updateBatteryLevel();
}


void MainWindow::togglePower()
{
    deviceOn = !deviceOn;
    if (deviceOn)
    {
        startDevice();
        qInfo() << "Device is on!";
    }
    else
    {
        shutDown();
        qInfo() << "Device is off!";
    }
}


void MainWindow::playPressed()
{
    if (!deviceOn)
    {
        return;
    }
    switch (menuChoice)
    {
    case Treatment:
        /*
        case 1: if session is not started, start new session
            case 1.1: if session is paused without started, start new session
            case 1.2: if session is stopped without started, start new session
        case 2: if session is started 
            case 2.1 then paused, resume session
            case 2.2 then stopped, start new session
            case 2.3 then paused then stopped, start new session
            case 2.4 then stopped then paused, start new session
            case 2.5 then start, do nothing
        */

        // start a new session
        if(connLost){
            ui->treatment_screen->setText("Connection still lost. Cannot start session...");
        }else if (sessionStarted == false){
            ui->treatment_screen->setText("Starting session...");
            sessionStarted = true;
            ui->blue_ind->setStyleSheet("background-color: " + blue_ind_on_color.name() + ";");
            remainedTime = SESSION_DURATION;
            device->startSession(); // This will trigger startSession() in Device
            if (testUI)
                testUI->clearData();
        }else if (sessionPaused == true){
            ui->treatment_screen->setText("Resuming session...");
            // remainedTime = qMax(SESSION_DURATION, remainedTime + ANALYSIS_DURATION + TREATMENT_DURATION); // Reduce elapsed time by ANALYSIS_DURATION + TREATMENT_DURATION (6) seconds
            // remainedTimer.start();
            device->resumeSession();
            sessionPaused = false;
        }else
        {
            qInfo() << "Session already started!";
        }
        break;
    case SessionHist:
        break;
    case SetDateTime:
        QDateTime dateTimeValue = ui->dateTimeEdit->dateTime();
        device->setDateTime(dateTimeValue);
        break;
    }
    updateBatteryLevel();
}

void MainWindow::pausePressed()
{
    if (!deviceOn)
    {
        return;
    }

    switch (menuChoice)
    {
    case Treatment:
        if (remainedTime > 0 && !sessionPaused){
            sessionPaused = true;
            ui->treatment_screen->setText("Trying to pause... Please wait..");
            emit pauseSession();
        }
        break;
    case SessionHist:
        break;
    case SetDateTime:
        break;
    }
    updateBatteryLevel();
}

void MainWindow::stopPressed()
{
    if (!deviceOn)
    {
        return;
    }

    sessionPaused = false;
    sessionStarted = false;
    
    switch (menuChoice)
    {
    case Treatment:
        if (remainedTime > 0 && !sessionPaused){
            ui->treatment_screen->setText("Preparing to stop session...");
            emit stopSession();
        }
        break;
    case SessionHist:
        break;
    case SetDateTime:
        break;
    }
    updateBatteryLevel();
}

void MainWindow::startTimer()
{
    qInfo() << "Timer started";
    remainedTimer.start();
}

void MainWindow::stopTimer()
{
    remainedTimer.stop();
    connLostTimer.stop();
    connLostTimeSeconds = 0;
    remainedTime = 0;
    ui->treatment_screen->setText("Session Over.");
    ui->blue_ind->setStyleSheet("background-color: " + blue_ind_off_color.name() + ";");
    ui->green_ind->setStyleSheet("background-color: " + green_ind_off_color.name() + ";");
    sessionStarted = false;
    sessionPaused = false;
}


void MainWindow:: pauseTimer(){
    ui->treatment_screen->setText("Paused session...");
    ui->green_ind->setStyleSheet("background-color: " + green_ind_off_color.name() + ";");
    remainedTimer.stop();

    QList<int> intervals;
    intervals.append(SESSION_DURATION);
    for (int i = SESSION_DURATION - ANALYSIS_DURATION-1; i > ANALYSIS_DURATION; i = i - (ANALYSIS_DURATION + TREATMENT_DURATION)){
        intervals.append(i);
    }
    intervals.append(ANALYSIS_DURATION);
    intervals.append(-1);

    for (int i = 0; i < intervals.size(); ++i) {
        if (remainedTime > intervals[i]) {
            remainedTime = intervals[i - 1];
            break;
        }
    }

    sessionPaused = true;
}

void MainWindow::resumeTimer(){
    ui->treatment_screen->setText("Resuming session...");
    remainedTimer.start();
    sessionPaused = false;
}


void MainWindow::handleConnectionLost(QString message)
{
    ui->ConnLostTime_label->setVisible(true);
    ui->ConnLostTime_value->setVisible(true);
    ui->red_ind->setStyleSheet("background-color: " + red_ind_on_color.name() + ";");
    ui->treatment_screen->setText(message);
    qInfo() << message;
    connLostTimer.start();
    connLost = true;
}

void MainWindow::startDevice(){
    ui->power_btn->setText("DEVICE ON");
    ui->power_btn->setStyleSheet(ui->power_btn->styleSheet() + "color: green;");
    if(device->getBatteryLevel() <= CRITICAL_BATTERY_LEVEL){
        shutDown();
        return;
    }else{
        dateTimeTimer.start();
        updateBatteryLevel();
    }
}


void MainWindow::shutDown()
{
    if(sessionStarted) device->stopSession();
    ui->power_btn->setText("DEVICE OFF");
    ui->power_btn->setStyleSheet(ui->power_btn->styleSheet() + "color: red;");
    ui->green_ind->setStyleSheet("background-color: " + green_ind_off_color.name() + ";");
    ui->red_ind->setStyleSheet("background-color: " + red_ind_off_color.name() + ";");
    ui->blue_ind->setStyleSheet("background-color: " + blue_ind_off_color.name() + ";");
    ui->Menu_select->setCurrentIndex(0);
    stopTimer();
    ui->treatment_screen->setText("");
    ui->battery_ind->setText("");
    ui->ConnLostTime_value->setVisible(false);
    ui->ConnLostTime_label->setVisible(false);

    ui->Battery_label->setText("");
    deviceOn = false;
    ui->dateTimeDisplay->setText("");
}

// TO TEST LATER WHEN IMPLEMENTATION DONE
void MainWindow::handleConnectionBack(QString message)
{
    connLost = false;
    ui->red_ind->setStyleSheet("background-color: " + red_ind_off_color.name() + ";");
    ui->ConnLostTime_label->setVisible(false);
    ui->ConnLostTime_value->setVisible(false);
    ui->treatment_screen->setText(message);
    qInfo() << message;
    if(sessionStarted) startTimer();
    connLostTimer.stop();
    connLostTimeSeconds = 0;
}

void MainWindow::handleRoundWaveData(const QMap<QPair<int, QString>, QList<float> > &roundWaveData)
{
    if (testUI)
        testUI->show();
    emit sendDataToTestUI(roundWaveData);
}

void MainWindow::handleSessionTerm()
{
    ui->blue_ind->setStyleSheet("background-color: " + blue_ind_off_color.name() + ";");
    stopTimer();
    sessionStarted = false;
    sessionPaused = false;
    if(device->getBatteryLevel() <= CRITICAL_BATTERY_LEVEL){  // doing this because to stop, the session needs to go through the complete round
        shutDown();
    }
}

void MainWindow:: updateBatteryLevel()
{
        int level = device->getBatteryLevel();
        ui->battery_ind->setText(QString::number(level) + "%");
        if(level > LOW_BATTERY_LEVEL){
            ui->battery_ind->setStyleSheet("border: 1px solid #f4f7f0; background-color: #85cf1d; color: #f4f7f0;");
            ui->Battery_label->setText("");
        }else if(level <= CRITICAL_BATTERY_LEVEL){
            if(level < 0) ui->battery_ind->setText("0%");
            ui->battery_ind->setStyleSheet("border: 1px solid #f4f7f0; background-color: red; color: #f4f7f0;");
            ui->Battery_label->setText("Critically low!");
            ui->treatment_screen->setText("Battery Critically Low! Preparing to shut down...");
            QTimer::singleShot(1000, device, &Device::stopSession);
        }else{
            ui->battery_ind->setStyleSheet("border: 1px solid #f4f7f0; background-color: orange; color: #f4f7f0;");
            ui->Battery_label->setText("Battery Low!");
        }
    if(deviceOn == false){
        ui->battery_ind->setText("");
        ui->Battery_label->setText("");
    }
}


void MainWindow:: phaseUpdate(PHASES phase, int index){
    switch(phase) {
        case TREATMENT:
            ui->green_ind->setStyleSheet("background-color: " + green_ind_on_color.name() + ";");
            ui->treatment_screen->setText(QString("Round #%1 of Treatment").arg(index+1));
            break;
        case WAVE_CALCULATION:
            ui->green_ind->setStyleSheet("background-color: " + green_ind_off_color.name() + ";");
            ui->treatment_screen->setText(QString("Round #%1 of Wave Analysis").arg(index+1));
            break;
        default:
            break;
    }
    updateBatteryLevel();

}

void MainWindow:: updateProgressBar(){
    ui->session_progress->setValue(100 - (static_cast<double>(remainedTime) / SESSION_DURATION * 100));
}


void MainWindow::viewDeviceSession() {
    QList<QMap<QString, QVariant>> sessionList = device->getLoggedSessions();
    QStandardItemModel *model = new QStandardItemModel(sessionList.size(), 2);
    model->setHorizontalHeaderLabels({"Date", "Time"});
    qInfo() << "Session List Size: " << sessionList.size();
    for(int row = 0; row < sessionList.size(); ++row) {
        const QMap<QString, QVariant> &session = sessionList.at(row);
        qInfo() << session.value("date") << " " << session.value("time");
        QStandardItem *date = new QStandardItem(session.value("date").toString());
        QStandardItem *time = new QStandardItem(session.value("time").toString());
        model->setItem(row, 0, date);
        model->setItem(row, 1, time);
    }

    ui->deviceSessionData->setModel(model);
}

void MainWindow::on_tableView_clicked(const QModelIndex &index)
{
    selectedSessionHist = index.row();  // Store the selected row
    qInfo() << "Selected row: " << selectedSessionHist;
}

void MainWindow:: updateChargingStatus(){
    if(ui->chargeBtn->isChecked()){
        chargerTimer.start();
    }else{
        chargerTimer.stop();
    }
}
