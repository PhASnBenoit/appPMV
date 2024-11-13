#ifndef CCSV_H
#define CCSV_H

#include <QObject>
#include <QFile>
#include <QFileDialog>
#include <QTime>
#include <QTableWidget>
#include <QTextStream>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QList>
#include <iostream>
#include <fstream>
#include <QMessageBox>
#include <QDir>
#include <QIODevice>
#include "czdc.h"

#define DELIMITER ";"

class CCsv : public QObject
{
    Q_OBJECT
public:
    explicit CCsv(QObject *parent = nullptr);

public slots:
    QStringList importCSV(QString path, T_TYPE_COURSE typeCourse); //--- 2024
    void exportCSV(QString path);
    QString chooseImportFile(); //---- 2024
    QString chooseExportFile(); //---- 2024

private:
    QFile _fileImport;
    QFile _fileExport;
    QStringList _nomsEleves;
    QString _ligne;
signals:

};

#endif // CCSV_H
