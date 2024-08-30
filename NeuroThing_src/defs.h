#ifndef DEFS_H
#define DEFS_H

#include <QMap>
#include <QList>
#include <QMetaType>
#include <QTimer>

// Device
static constexpr int EEG_COUNT = 7;

// SessionManager
static constexpr int TREATMENT_ROUNDS = 4;
static constexpr int ANALYSIS_DURATION = 5;
static constexpr int TREATMENT_DURATION = 1;
static constexpr int SESSION_DURATION = TREATMENT_ROUNDS * (ANALYSIS_DURATION + TREATMENT_DURATION) + ANALYSIS_DURATION; // seconds
static constexpr int EEG_TO_DISCONNECT = 0;
static constexpr int BRAINWAVE_OFFSET = 5;

// WaveGenerator
static constexpr int SAMPLE_COUNT = 100;
static constexpr int SAMPLE_RATE = 100;

// MainWindow
static constexpr int MAX_CONNECTION_TIMEOUT = 10;
static constexpr float TREATMENT_PROGRESS_INC = 0.493;
static constexpr float WAVE_CALC_PROGRESS_INC = 17.255; // Wave time should be 5 times of EEG_COUNT * TREATMENT_PROGRESS_INC
static constexpr int BATTERY_INC = 2;
static constexpr int BATTERY_INC_INTERVAL = 2000;
static constexpr int CRITICAL_BATTERY_LEVEL = 5;
static constexpr int LOW_BATTERY_LEVEL = 20;

// TestUI
static constexpr bool TESTUI_ON = true; // if false, TestUI is not launched


struct EEGWaveMap {
  int eegIndex;
  QMap<QString, QList<float>> waveMap;
};

struct EEGDomFreq {
  int eegIndex;
  float domFreq;
};

Q_DECLARE_METATYPE(EEGWaveMap);
Q_DECLARE_METATYPE(EEGDomFreq);


struct BrainwaveFrequencyData {
  int eegIndex;
  int brainwaveIndex;
  float frequencyValue;
};


enum PHASES{
    EEG_CONNECTION,
    WAVE_CALCULATION,
    TREATMENT
};

#endif // DEFS_H
