# OO Design model

## UML Sequence Diagram

### Use Case 1: Treatment Session

```mermaid
sequenceDiagram
    actor User
    participant MainWindow
    participant Device
    participant SessionManager
    participant SignalProcessor
    participant EEG
    participant WaveGenerator
    participant PC

	User->>MainWindow: Turn On The Device
  User->>MainWindow: Choose Treatment Mode
  MainWindow->>Device: Start the session
  Device->>EEG: Start Connection
	Device->>MainWindow: Turn on the blue light
	Device->>SessionManager: Start the Session
	Device->>MainWindow: Start the session timer

	loop 4 rounds of treatments(5hz/10hz/15hz/20hz offset)
		%% Analysis
		WaveGenerator->>EEG: Create 4 Bands of Wave Forms
		EEG->>SignalProcessor: Send 4 Bands of Wave Forms
		SignalProcessor->>SignalProcessor: Calculate Dominant Frequency and generate combined wave
		SignalProcessor->>SessionManager: Send Dominant Frequency for All Site (5s)
		SignalProcessor->>SessionManager: Send Wave Drawing Data(RoundWaveData)
		SessionManager->>Device: Send Dominant Frequency for All Site
		SessionManager->>Device: Send Wave Drawing Data(RoundWaveData)
		Device->>MainWindow: Plot Brain Wave

		%% Treatment Starts
		Device->>MainWindow: Turn on the green light (indicate treatment)
		SignalProcessor->>EEG: Add 5/10/15/20 hz offset frequency to Dominant Frequency
		EEG->>SignalProcessor: Send Treatment Feedback(1s for All EEGs)
		SignalProcessor->>SessionManager: Send Treatment Feedback(1s for All EEGs)
	end

	par Post Session Analysis
		%% Analysis
		WaveGenerator->>EEG: Create 4 Bands of Wave Forms
		EEG->>SignalProcessor: Send 4 Bands of Wave Forms
		SignalProcessor->>SignalProcessor: Calculate Dominant Frequency and generate combined wave
		SignalProcessor->>SessionManager: Send Dominant Frequency for All Site (5s)
		SignalProcessor->>SessionManager: Send Wave Drawing Data(RoundWaveData)
		SessionManager->>Device: Send Dominant Frequency for All Site
		SessionManager->>Device: Send Wave Drawing Data(RoundWaveData)
		Device->>MainWindow: Plot Brain Wave
    end
        SessionManager->>Device: Finish session
        Device->>MainWindow: Show Session End Message
```

---

### Use Case 2: View Time and Date of the Sessions on the Device

```mermaid
sequenceDiagram
    actor User
    participant MainWindow
    participant Device
    participant SessionManager

    User->>MainWindow: Choose Session History
    MainWindow->>Device: Get Logged Sessions
    MainWindow->>User: Display Logged Sessions
```

---

### Use Case 3: Upload Session Log History to a PC

```mermaid
sequenceDiagram
    actor User
    participant MainWindow
    participant Device
    participant SessionManager

    participant PC

    User->>MainWindow: Choose Session History
    MainWindow->>Device: Get Logged Sessions
    MainWindow->>User: Display Logged Sessions

    User->>MainWindow: Select Session
    MainWindow->>Device: Send Selected Index
    Device ->> PC: Send Selected Session
    PC ->> PC: Add Selected Session
    PC ->> PC: Save Selected Session to Database
    PC ->> User: Display Selected Session
```

---

### Use Case 4: Setting Device Time and Date

```mermaid
sequenceDiagram
	participant User
	participant MainWindow
	participant Device

	User->> MainWindow: Date/Time setting mode
	User ->> MainWindow: Set date/time
	MainWindow ->> Device: Set date/time
	MainWindow ->> MainWindow: Render the new Date/time


```

---

### Use Case 5: Low Battery

```mermaid
sequenceDiagram
    participant User
    participant MainWindow
    participant Device
    participant Battery
    participant SessionManager

    alt Battery level is below 20%
        Battery->>Device: Notify low battery (below 20%)
        Device->>MainWindow: Display low battery warning
        alt Battery level goes down to 5% and User does not charge the device
            Battery->>Device: Notify critical battery level (below 5%)
            Device->>SessionManager: Terminate session
            Device->>Device: Shut down
        else User charges the device before reaching 5%
            Battery->>Device: Notify battery is charging
            Device->>MainWindow: Display charging status
        end
    end
```

---

### Use Case 6: Lost Connection

```mermaid
sequenceDiagram
	participant User
	participant MainWindow
	participant Device
	participant SessionManager
	participant EEG
	%% participant Display

	User ->> MainWindow : Click Start new session btn
	MainWindow ->> Device : Start new Session
	Device ->> SessionManager: Start new Session
	SessionManager ->> EEG : Initialize Connection
	EEG ->> EEG : Detect connection loss
	EEG ->> Device : Notify connection loss
	Device ->> MainWindow : Notify connection loss
	MainWindow ->> MainWindow : Turn Red light on
	MainWindow ->> MainWindow: Show warning message
  Device->>SessionManager: Pause the session
  Device->>SessionManager: Pause the session timer
  Device->>SessionManager: Turn on the pause timer
  alt Session contact is lost for 5 minutes
    Device->>SessionManager: Terminate session
	    Device ->> MainWindow : Notify Termination
    MainWindow ->> MainWindow : Notify error.
  else Session contact is back
    Device->>SessionManager: Resume the session
    Device->>SessionManager: Resume the session timer
  end
```

## UML Class Diagram

Presents the structure of the system with classes, attributes, operations, and the relationships among them.

```mermaid
classDiagram

   class MainWindow {
      - ui: Ui::MainWindow*
      - deviceThread: QThread*
	    - elapsedTimer: QTimer
	    - connLostTimer: QTimer
	    - connLostTimeSeconds: int
	    - elapsedTimeSeconds: int
	    - selectedSessionHist: int
	    - menuChoice: MenuOptions
      - deviceOn: bool
      - sessionPaused: bool
      - sessionStarted: bool

	    %% signals
	    + startNewSession() void
      + pauseSession() void
      + resumeSession() void
      + stopSession() void
      + sendDataToTestUI(QMap<QPair<int, QString>, QList<float> >) void

	    %% slots
	    - chooseMenu(MenuOptions) void
	    - togglePower() void
	    - playPressed() void
	    - pausePressed() void
	    - stopPressed() void

	    - startTimer() void
	    - stopTimer() void
	    - pauseTimer() void

	    - startDevice() void
	    - shutDown() void
	    - updateBatteryLevel() void

	    - handleConnectionLost(QString) void
	    - handleSessionTerm() void
	    - handleConnectionBack(QString) void
	    - handleRoundWaveData(QMap<QPair<int, QString>, QList<float> >) void
	    - phaseUpdate(PHASES, int) void
	    - updateProgress(int) void
	    - viewDeviceSession() void
	    - on_tableView_clicked(QModelIndex) void
		}


    class Device {
	    - eegList: QList<EEG*>
      - currentDateTime: QDateTime
      - loggedSessionList: QList<QMap<QString, QVariant>>
      - dominantFrequencyData: QMap<QString, QList<float>>
      - battery: Battery*

			%% slots
			+ startSession() bool
      + pauseSession() void
      + resumeSession() void
      + stopSession() void
      + receiveEEGConnLoss(EEG*) void
      + receiveEEGConnBack(EEG*) void
      + onSessionFinished(int) void
      + chargeBattery(int) void
      + setDateTime(QDateTime) void
      + getBatteryLevel() int

      + handleDominantFrequency(QDateTime dateTime, QMap<QString, QList<float>>) void
      + handleRoundWaveData(QMap<QPair<int, QString>, QList<float>>) void
      + receiveSessionPhase(PHASES, int) void
      + getLoggedSessions() QList<QMap<QString, QVariant>>
      + onUploadToPCPressed(int) void

			%% siganls
			+ startDisplayTimer() void
      + sendConnLossToGUI(QString) void
      + sendConnBackToGUI(QString) void
      + sendStartToSession(QList<EEG*>) void
      + sendPauseToSession() void
      + sendResumeToSession() void
      + sendStopToSession() void
      + sendSessionTermToGUI() void
      + sendLoggedSessions() void
      + sendRoundDataToMainWindow(QMap<QPair<int, QString>, QList<float> >) void
      + sendDataToTestUI(QMap<QPair<int, QString>, QList<float> >) void
      + phaseProgress(PHASES, int) void
      + sendSessionInfoToPC(QMap<QString, QVariant>) void

    }


    class SessionManager{
		  - startTime: QDateTime
      - currentRound: int
      - therapyStatus: int
      - eegList: QList<EEG*>
      - dominantFrequency: QList<float>
      - dominantFrequencyMap: QMap<QString, QList<float>>
      - roundWaveData: QMap<QPair<int, QString>, QList<float>>
      - roundWaveDataList: QList<QMap<QPair<int, QString>, QList<float>>>
      - isFinished: bool

      + runSession(QList<EEG*>, int) void

      %%signals:
      + dominantFrequencyReady(QDateTime, QMap<QString, QList<float>> ) void
      + roundDataReady(QMap<QPair<int, QString>, QList<float>> ) void
      + sessionFinished(int) void
      + sendPhaseToDevice(PHASES, int) void

      %% slots:
      + onSessionStarted(QList<EEG*>) void
      + onSessionPaused() void
      + onSessionResumed() void
      + onSessionStopped() void
      - handleRoundWaveData(QList<EEGWaveMap>) void
      - handleDominantFrequency(QList<EEGDomFreq>) void
    }


    class EEG {
      - waveMap QMap<QString, QList<float>>
      - ampFreqMap QMap<QString, QPair<float, float>>
      - index int
      - isConnected bool
      - waveGenerator WaveGenerator*

	    + startConnection() bool
      + stopConnection() bool
      + generateWave() void
      + getWaveMap() QMap<QString, QList<float>>
      + getAmpFreqMap() QMap<QString, QPair<float, float>>

	    %%signals
		  + sendConnLossToDevice(EEG*): void
		  + sendConnBackToDevice(EEG*): void
    }


		class SignalProcessor {
      - eegDomFreqList QList<EEGDomFreq>
      - eegWaveMapList QList<EEGWaveMap>

		  - calculateDominantFrequency(QMap<QString, QPair<float, float>>) float
		  + processDominantFrequency(QList<EEG*>) void

    %% signals:
      + sendDominantFrequencyToSM(QList<EEGDomFreq>) void
      + sendWaveMapToSM(QList<EEGWaveMap>) void

		}

		class PC {
			- tableWidget QTableWidget*
      - sessionInfoDisplay QVBoxLayout*
      - currentTableIndex int
      - uploadedSessions QList<QMap<QString, QVariant>>

      - displaySession(QMap<QString, QVariant>) void
      - saveSessionToDatabase(QMap<QString, QVariant>) void
      - removeDatabase() void

      %% slots:
      + onPreviousPressed() void
      + onNextPressed() void
      + handleSessionInfoData(QMap<QString, QVariant>) void
		}


		class TestUI {
			- ui Ui::TestUI*
      - plot QCustomPlot
      - sessionWaveData QMap<int, QMap<QPair<int, QString>, QList<float>>>
      - currentRound int

      + clearData() void

		 %% slots:
      + updateData(QMap<QPair<int, QString>, QList<float> >) void
      - updatePlot() void
		}


    class Battery {
      - batteryLevel: int

	    + reduceBattery(int) void
	    + chargeBattery(int) void
    }

    class WaveGenerator{
      - eegIndex int
      - ampFreqMap QMap<QString, QPair<float, float>>
      - waveMap QMap<QString, QList<float>>

      - generateBetaWave() QList<float>
      - generateAlphaWave() QList<float>
      - generateThetaWave() QList<float>
      - generateDeltaWave() QList<float>

      + generateWave() QMap<QString, QList<float>>
      + getAmpFreqMap() QMap<QString, QPair<float, float>>
      + generateCustomWave(int, int, QString) QList<float>
    }

    %% add composition relationships:
    %% A --* B: B contains A
		Device --* MainWindow
		SessionManager --* Device
		EEG --* Device
		Battery --* Device
		PC --* MainWindow
		TestUI --* MainWindow
    SignalProcessor --* SessionManager
    WaveGenerator --* EEG
    WaveGenerator --* SignalProcessor
```

## UML State Machine Diagrams

Depicts the state changes of the device during different operational phases including initialization, treatment, pausing, and connection handling.

```mermaid
stateDiagram
%% for device
	[*] --> CheckSignal : Device Started
	CheckSignal --> TreatmentMode: In treatment mode

	TreatmentMode --> Initializing: Starting a new session
	Initializing --> Calculating: Calculating frequencies
	Calculating --> Treating: Applying offset frequencies
	Treating --> Logging: Storing session log
	Logging --> Displaying_results: Display session result
	Displaying_results --> CheckSignal
		Calculating --> Paused: Pause signal
	Treating --> Paused: Pause signal

	Paused --> Calculating: Resume Operation

	Paused --> Treating: Resume Operation

	CheckSignal --> SessionHistMode: In session history viewing mode
	SessionHistMode --> SelectHistory: Selecting history to upload
	SelectHistory --> Uploading: Upload session data to PC
	Uploading --> CheckSignal

	CheckSignal --> DateTimeSetMode: In Date time menu mode
	DateTimeSetMode --> ChoosingDateTime: Choosing date time to update
	ChoosingDateTime --> SettingDateTime: Set the chosen dateTime
	SettingDateTime --> CheckSignal

	CheckSignal --> StoppingSession
	StoppingSession --> CheckSignal

	CheckSignal --> [*]: ShutDown




```
