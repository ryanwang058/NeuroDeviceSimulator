#ifndef PC_H
#define PC_H

#include <QObject>
#include <QTableWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>
#include "sessionmanager.h"
#include <QWidget>
#include "qcustomplot.h"
#include <QVector>

class PC : public QObject
{
    Q_OBJECT

public:
    explicit PC(QObject *parent = nullptr, QTableWidget *tableWidget = nullptr, QVBoxLayout *sessionInfoDisplay = nullptr);
    ~PC();

private:
    QTableWidget *tableWidget;
    QVBoxLayout *sessionInfoDisplay;
    int currentTableIndex;
    QList<QMap<QString, QVariant>> uploadedSessions;
    void displaySession(const QMap<QString, QVariant> &sessionInfo);
    void saveSessionToDatabase(const QMap<QString, QVariant> &sessionInfo);
    void removeDatabase();

public slots:
    void onPreviousPressed();
    void onNextPressed();
    void handleSessionInfoData(const QMap<QString, QVariant> &sessionInfo);
};

#endif // PC_H
