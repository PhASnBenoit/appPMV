#ifndef CBDD_H
#define CBDD_H

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSql>
#include <QDebug>
#include <QList>
#include <QString>
#include <QDateTime>
#include "czdc.h"

class CBdd : public QObject
{
    Q_OBJECT

public:
    CBdd(QObject *parent = nullptr);
    ~CBdd();

    bool getParamSession(T_PARAMSESSION &paramSession); //--- 2024
    bool setParamSession(T_PARAMSESSION &paramSession); //--- 2024

    bool isSessionActive (); //------- 2024
    void setSessionActive(bool active);

    QString getSessionName(); //-------- 2024
    void setSessionName(QString nomSession);

    void setListeCoureurs(QStringList &liste); //--- 2024
    void getCourses(QList<T_2COUREURS> &courses); //--- 2024

    void sauveCourse(int noCourse, T_2COUREURS coureurs);

    QList<QString> extractDatasExport();// Méthode pour l'extraction du fichier
    bool verifCodePIN(QString code); //---------- 2024

private:
    QSqlDatabase PMVBdd;
    QString _sqlPath;
    QSqlQuery *_sqlQuery;
    void clearTableCoureur();
};
#endif // CBDD_H
