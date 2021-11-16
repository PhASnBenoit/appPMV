#include "cbdd.h"

CBdd::CBdd(QObject *parent) : QObject(parent)
{
    QString driver("QMYSQL");
    if (QSqlDatabase::isDriverAvailable(driver)) {
        _db = QSqlDatabase::addDatabase(driver);
        _db.setHostName("localhost");
        _db.setDatabaseName("pmv");
        _db.setUserName("pmv");
        _db.setPassword("pmv");
        if (!_db.open()) {
            QMessageBox msgBox;
            msgBox.setText("ERREUR: " + _db.lastError().text());
            msgBox.setInformativeText("L'application ne fonctionnera pas !");
            msgBox.setStandardButtons(QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Cancel);
            msgBox.exec();
        } // if open
        QSqlQuery q("SELECT * FROM session");
        if (q.size() != 20)
            resetTableSession();
    } // if driver
}

CBdd::~CBdd()
{
    _db.close();
} // method

void CBdd::resetTableSession()
{
    QSqlQuery qeffsession("TRUNCATE TABLE session");
    // init table session
    int noc=1;
    for (int i=0 ; i<NB_ROWS ; i++) {
        QSqlQuery q;
        q.prepare("INSERT INTO session VALUES (:id, :c1, :t1, :v1, :c2, :t2, :v2)");
        q.bindValue(":id", i);
        q.bindValue(":c1", "Coureur"+QString::number(noc++));
        q.bindValue(":t1", "NC");
        q.bindValue(":v1", "NC");
        q.bindValue(":c2", "Coureur"+QString::number(noc++));
        q.bindValue(":t2", "NC");
        q.bindValue(":v2", "NC");
        q.exec();
    } // for i
}

bool CBdd::lireDataSession(QString &nom, QList<QList<QString> > &liste)
{
    QSqlQuery sqn("SELECT nomsession FROM params LIMIT 1");
    sqn.next();
    nom = sqn.value("nomsession").toString();

    QSqlQuery sql("SELECT couloir1, temps1, vent1, couloir2, temps2, vent2 FROM session LIMIT 20");
    for (int i=0 ; i<sql.size() ; i++) {
        QList<QString> ligne;
        sql.next();
        for (int j=0 ; j<NB_COLS ; j++) {
             ligne.append(sql.value(j).toString());
        } // for j
        liste.append(ligne);
    } // for i
    return true;
} // method

bool CBdd::isSessionActive()
{
    QSqlQuery sq("SELECT issession FROM params LIMIT 1");
    sq.next();
    int i = sq.value(0).toInt();
    return i;
} // method

bool CBdd::setSession(bool b)
{
    QSqlQuery sq;
    sq.prepare("UPDATE params SET issession=:act WHERE id=0 LIMIT 1");
    sq.bindValue(":act",b);
    return sq.exec();
} // method

bool CBdd::sauverCourse(int no, qint64 temps1, QString vent1, qint64 temps2, QString vent2)
{
    QSqlQuery sc;
    sc.prepare("UPDATE session SET temp1=:t1, vent1=:v1, temps2=:t2, vent2=:v2 WHERE id=:n");
    sc.bindValue(":t1",temps1);
    sc.bindValue(":v1",vent1);
    sc.bindValue(":t2",temps2);
    sc.bindValue(":v2",vent2);
    sc.bindValue(":n",no);
    return sc.exec();
} // method

void CBdd::sauverListe(QList<QList<QString>> liste)
{
    QList<QString> ligne;
    for (int i=0 ; i<liste.size() ; i++) {
        ligne = liste.at(i);
        QSqlQuery sc;
        sc.prepare("UPDATE session SET couloir1=:c1, temp1=:t1, vent1=:v2, couloir2=:c2, temps2=:t2, vent2=:v2 WHERE id=:n");
        sc.bindValue(":c1", ligne.at(0));
        sc.bindValue(":t1", ligne.at(1));
        sc.bindValue(":v1", ligne.at(2));
        sc.bindValue(":c2", ligne.at(3));
        sc.bindValue(":t2", ligne.at(4));
        sc.bindValue(":v2", ligne.at(5));
        sc.bindValue(":t1", i);
        sc.exec();
    } // for i
}

void CBdd::sauverNomSession(QString nomSession)
{
    QSqlQuery sc;
    sc.prepare("UPDATE params SET nomsession=:ns LIMIT 1");
    sc.bindValue(":ns",nomSession);
    sc.exec();
}

void CBdd::sauverNom(QString nom, int no, int position)
{
    QSqlQuery sc;
    if (position == 1)
        sc.prepare("UPDATE session SET couloir1=:v WHERE id=:n");
    else
        sc.prepare("UPDATE session SET couloir2=:v WHERE id=:n");
    sc.bindValue(":v",nom);
    sc.bindValue(":n",no);
    sc.exec();
} // method

void CBdd::sauverVent(float vent, int no, int position)
{
    QSqlQuery sc;
    if (position == 1)
        sc.prepare("UPDATE session SET vent1=:v WHERE id=:n");
    else
        sc.prepare("UPDATE session SET vent2=:v WHERE id=:n");
    sc.bindValue(":v",QString::number(vent));
    sc.bindValue(":n",no);
    sc.exec();
} // method

void CBdd::sauverTemps(qint64 temps, int no, int position)
{
    QSqlQuery sc;
    if (position == 1)
        sc.prepare("UPDATE session SET temps1=:t WHERE id=:n");
    else
        sc.prepare("UPDATE session SET temps2=:t WHERE id=:n");
    QDateTime dt;
    dt.setMSecsSinceEpoch(temps);
    sc.bindValue(":t",dt.toString("mm:ss:zzz"));
    sc.bindValue(":n", no);
    sc.exec();
} // method
