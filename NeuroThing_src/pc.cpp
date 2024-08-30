#include "pc.h"

PC::PC(QObject *parent, QTableWidget *tableWidget, QVBoxLayout *sessionInfoDisplay)
    : QObject(parent), tableWidget(tableWidget), sessionInfoDisplay(sessionInfoDisplay), currentTableIndex(-1)
{
    QString currentDir = QCoreApplication::applicationDirPath();
    QString separator = QDir::separator();
    int neuroThingIndex = currentDir.lastIndexOf(separator + "NeuroThing" + separator);
    if (neuroThingIndex == -1)
    {
        qDebug() << "Path error";
        return;
    }

    QString basePath = currentDir.left(neuroThingIndex + separator.length() + QString("NeuroThing").length());

    QString jsonFilePath = basePath + separator + "database" + separator + "uploadedSessions.json";

    // check if the database exists
    QFile file(jsonFilePath);
    if (!file.exists()) {
        return;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Failed to open file:" << file.errorString();
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull() || !doc.isArray()) {
        qDebug() << "Failed to parse JSON";
        return;
    }

    // get session info from database
    QJsonArray sessionsArray = doc.array();
    for (int i = 0; i < sessionsArray.size(); ++i) {
        QJsonObject sessionObject = sessionsArray.at(i).toObject();
        QMap<QString, QVariant> sessionInfo;
        sessionInfo.insert("date", sessionObject.value("date").toString());
        sessionInfo.insert("time", sessionObject.value("time").toString());

        QVariantList finalFreqList, initialFreqList;
        QJsonArray finalFreqArray = sessionObject.value("finalDominantFrequency").toArray();
        QJsonArray initialFreqArray = sessionObject.value("initialDominantFrequency").toArray();
        for (int j = 0; j < finalFreqArray.size(); ++j) {
            finalFreqList.append(finalFreqArray.at(j).toDouble());
            initialFreqList.append(initialFreqArray.at(j).toDouble());
        }
        sessionInfo.insert("finalDominantFrequency", finalFreqList);
        sessionInfo.insert("initialDominantFrequency", initialFreqList);

        uploadedSessions.append(sessionInfo);
    }

    displaySession(uploadedSessions.at(0));
    currentTableIndex = 0;
}

PC::~PC()
{
    //removeDatabase();
}

void PC::saveSessionToDatabase(const QMap<QString, QVariant> &sessionInfo)
{
    qDebug() << "Saving session to database...";

    QString currentDir = QCoreApplication::applicationDirPath();

    //get the separator based on different os system
    QString separator = QDir::separator();
    int neuroThingIndex = currentDir.lastIndexOf(separator + "NeuroThing" + separator);
    if (neuroThingIndex == -1)
    {
        qDebug() << "Path error";
        return;
    }

    QString basePath = currentDir.left(neuroThingIndex + separator.length() + QString("NeuroThing").length());

    QString jsonFilePath = basePath + separator + "database" + separator + "uploadedSessions.json";

    QFile jsonFile(jsonFilePath);
    if (!jsonFile.exists()) {
        QDir().mkpath(QFileInfo(jsonFile).absolutePath());
        if (!jsonFile.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            qDebug() << "Failed to create JSON file: " << jsonFilePath;
            return;
        }
        jsonFile.close();
    }

    if (!jsonFile.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug() << "Failed to open JSON file for writing: " << jsonFilePath;
        return;
    }

    QByteArray fileData = jsonFile.readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(fileData);
    QJsonArray jsonArray;

    if (jsonDocument.isArray())
    {
        jsonArray = jsonDocument.array();
    }

    QJsonObject sessionObject;
    QMapIterator<QString, QVariant> iter(sessionInfo);
    while (iter.hasNext())
    {
        iter.next();
        if (iter.key() == "initialDominantFrequency" || iter.key() == "finalDominantFrequency") {
            QJsonArray frequencyArray;
            foreach(const QVariant &value, iter.value().toList()) {
                frequencyArray.append(QJsonValue::fromVariant(value));
            }
            sessionObject.insert(iter.key(), frequencyArray);
        } else {
            sessionObject.insert(iter.key(), QJsonValue::fromVariant(iter.value()));
        }
    }

    jsonArray.append(sessionObject);

    jsonFile.resize(0);
    jsonFile.write(QJsonDocument(jsonArray).toJson());
    jsonFile.close();

    qDebug() << "JSON file written successfully: " << jsonFilePath;
}

void PC::displaySession(const QMap<QString, QVariant> &sessionInfo)
{
    // session display
    while (!sessionInfoDisplay->isEmpty())
    {
        delete sessionInfoDisplay->takeAt(0)->widget();
    }

    QLabel *sessionDateLabel = new QLabel("Session Date: " + sessionInfo.value("date").toString());
    QLabel *sessionTimeLabel = new QLabel("Session Time: " + sessionInfo.value("time").toString());

    sessionInfoDisplay->addWidget(sessionDateLabel);
    sessionInfoDisplay->addWidget(sessionTimeLabel);

    QList<QVariant> initialFreqList = sessionInfo.value("initialDominantFrequency").toList();
    QList<QVariant> finalFreqList = sessionInfo.value("finalDominantFrequency").toList();

    int columnCount = 3;
    tableWidget->setColumnCount(columnCount);
    QStringList headers;
    headers << "EEG Number" << "Initial Dominant Frequency" << "Final Dominant Frequency";
    tableWidget->setHorizontalHeaderLabels(headers);

    int rowCount = initialFreqList.size();
    tableWidget->setRowCount(rowCount);

    for (int i = 0; i < rowCount; ++i) {
        QTableWidgetItem *eegNumberItem = new QTableWidgetItem(QString::number(i + 1));
        tableWidget->setItem(i, 0, eegNumberItem);

        double initialFreqValue = (i < initialFreqList.size()) ? initialFreqList[i].toDouble() : 0.0;
        QTableWidgetItem *initialFreqItem = new QTableWidgetItem(QString::number(initialFreqValue));
        tableWidget->setItem(i, 1, initialFreqItem);

        double finalFreqValue = (i < finalFreqList.size()) ? finalFreqList[i].toDouble() : 0.0;
        QTableWidgetItem *finalFreqItem = new QTableWidgetItem(QString::number(finalFreqValue));
        tableWidget->setItem(i, 2, finalFreqItem);
    }

    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
    tableWidget->resizeColumnsToContents();
}

void PC::onPreviousPressed()
{
    if (currentTableIndex - 1 < 0) {
        qDebug() << "No previous uploaded session available.";
    } else {
        displaySession(uploadedSessions.at(currentTableIndex - 1));
        currentTableIndex--;
    }
}

void PC::onNextPressed()
{
    if (currentTableIndex + 1 >= uploadedSessions.size()) {
        qDebug() << "No next uploaded session available.";
    } else {
        displaySession(uploadedSessions.at(currentTableIndex + 1));
        currentTableIndex++;
    }
}

void PC::handleSessionInfoData(const QMap<QString, QVariant> &sessionInfo){
    qDebug() << "handle session info data...";
    uploadedSessions.append(sessionInfo);
    currentTableIndex = uploadedSessions.size() - 1;
    saveSessionToDatabase(sessionInfo);
    displaySession(sessionInfo);
}

// void PC::removeDatabase(){
//     QString currentDir = QCoreApplication::applicationDirPath();
//     QString jsonFilePath = currentDir + "/database/uploadedSessions.json";

//     QFile jsonFile(jsonFilePath);
//     if (jsonFile.exists()) {
//         if (!jsonFile.remove()) {
//             qDebug() << "Failed to remove JSON file: " << jsonFilePath;
//         } else {
//             qDebug() << "JSON file removed successfully: " << jsonFilePath;
//         }
//     } else {
//         qDebug() << "JSON file does not exist: " << jsonFilePath;
//     }

//     QDir databaseDir(currentDir + "/database");
//     if (databaseDir.exists()) {
//         if (!databaseDir.removeRecursively()) {
//             qDebug() << "Failed to remove database folder: " << databaseDir.path();
//         } else {
//             qDebug() << "Database folder removed successfully: " << databaseDir.path();
//         }
//     } else {
//         qDebug() << "Database folder does not exist: " << databaseDir.path();
//     }
// }
