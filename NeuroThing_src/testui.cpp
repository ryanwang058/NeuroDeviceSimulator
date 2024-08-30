#include "testui.h"
#include "ui_testUI.h"

TestUI::TestUI(QWidget *parent)
    : QWidget{parent}, ui(new Ui::TestUI), currentRound(1)
{

    ui->setupUi(this);
    this->setWindowTitle("Brain Wave Plots");

    // Initialize QCustomPlot
    plot = new QCustomPlot(ui->plotFrame);
    plot->setFixedWidth(ui->plotFrame->width());
    plot->setFixedHeight(ui->plotFrame->height());
    plot->xAxis->setRange(1, SAMPLE_COUNT);
    plot->yAxis->setRange(-2, 2);

    // Initialize waveTypeComboBox
    ui->bandComboBox->addItem("alpha");
    ui->bandComboBox->addItem("beta");
    ui->bandComboBox->addItem("theta");
    ui->bandComboBox->addItem("delta");
    ui->bandComboBox->addItem("combined");

    // Initialize eegIndexComboBox
    for (int i = 0; i < EEG_COUNT; i++)
        ui->eegComboBox->addItem(QString::number(i+1));

    // Connect signals and slots
    connect(ui->bandComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TestUI::updatePlot);
    connect(ui->eegComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TestUI::updatePlot);
    connect(ui->roundComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &TestUI::updatePlot);

    // dummy values
    // QMap<QPair<int, QString>, QList<float>> roundWaveData;
    // roundWaveData[QPair<int, QString>(1, "alpha")] = {0.1, 0.2, 0.3, 0.4};
    // roundWaveData[QPair<int, QString>(1, "beta")] = {0.3, 0.4, 0.5, 0.6};
    // roundWaveData[QPair<int, QString>(1, "theta")] = {0.2, 0.3, 0.4, 0.5};
    // roundWaveData[QPair<int, QString>(1, "delta")] = {0.4, 0.5, 0.6, 0.7};
    //  roundWaveData[QPair<int, QString>(1, "combined")] = {0.4, 0.5, 0.6, 0.7};
    // updateData(roundWaveData);
    // updateData(roundWaveData);
}

TestUI::~TestUI()
{
    delete ui;
    delete plot;
}

void TestUI::clearData()
{
    currentRound = 1;
    ui->roundComboBox->clear();
    sessionWaveData.clear();
}

void TestUI::updateData(const QMap<QPair<int, QString>, QList<float> > &roundWaveData)
{
    // make sure <= 5 round data shown
    const int maxItems = 5;
    if (ui->roundComboBox->count() < maxItems) {
        ui->roundComboBox->addItem(QString::number(currentRound));
        sessionWaveData[currentRound++] = roundWaveData;
        updatePlot();
    }
}

void TestUI::updatePlot()
{
    // Retrieve selected wave type, index, and treatment round
    QString selectedWaveType = ui->bandComboBox->currentText();
    int selectedEEGIndex = ui->eegComboBox->currentText().toInt();
    int selectedTreatmentRound = ui->roundComboBox->currentText().toInt();

    // Fetch wave data for the selected treatment round
    QMap<QPair<int, QString>, QList<float>> roundWaveData = sessionWaveData[selectedTreatmentRound];

    // Fetch wave values for the selected wave type and EEG index
    QList<float> waveValues = roundWaveData[QPair<int, QString>(selectedEEGIndex, selectedWaveType)];

    // converting to QVector<double> keys and values
    QList<double> keys;
    QList<double> values;
    for (int i = 0; i < waveValues.size(); ++i) {
        keys.append(i);
        values.append(waveValues.at(i));
    }

    // Update the plot
    plot->clearGraphs(); // Clear existing graph
    plot->addGraph();
    plot->graph(0)->setData(QVector<double>::fromList(keys), QVector<double>::fromList(values));
    plot->xAxis->setLabel("Sample No.");
    plot->yAxis->setLabel("Amplitude");
    plot->replot();
}
