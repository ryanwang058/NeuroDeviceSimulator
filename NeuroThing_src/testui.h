#ifndef TESTUI_H
#define TESTUI_H

#include <QObject>
#include <QMap>
#include <QComboBox>
#include <QWidget>
#include "qcustomplot.h"
#include "defs.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class TestUI;
}
QT_END_NAMESPACE


class TestUI : public QWidget
{
    Q_OBJECT
public:
    explicit TestUI(QWidget *parent = nullptr);
    ~TestUI();

    void clearData();

public slots:
    void updateData(const QMap<QPair<int, QString>, QList<float> > &roundWaveData);

private slots:
    void updatePlot();

private:
    Ui::TestUI *ui;
    QCustomPlot *plot;
    QMap<int, QMap<QPair<int, QString>, QList<float>>> sessionWaveData;
    int currentRound;
};

#endif // TESTUI_H
