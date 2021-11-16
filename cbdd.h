#ifndef CBDD_H
#define CBDD_H

#include <QObject>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QDateTime>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

#include "commun.h"

class CBdd : public QObject
{
    Q_OBJECT
public:
    explicit CBdd(QObject *parent = nullptr);
    ~CBdd();
    bool sauverCourse(int no, qint64 temps1, QString vent1, qint64 temps2, QString vent2);
    void sauverListe(QList<QList<QString>> liste);
    void sauverNomSession(QString nomSession);
    void sauverNom(QString nom, int no, int position);
    void sauverVent(float vent, int no, int position);
    void sauverTemps(qint64 temps, int no, int position);
    void resetTableSession();
    bool lireDataSession(QString &nom, QList<QList<QString> > &liste);
    bool isSessionActive();
    bool setSession(bool b);

private:
    QSqlDatabase _db;

signals:

public slots:
};

#endif // CBDD_H
