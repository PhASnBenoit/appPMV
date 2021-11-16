#ifndef CAPP_H
#define CAPP_H

#include <QObject>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>
#include "cbdd.h"
#include "ccapteurpassage.h"
#include "csignalisation.h"
#include "canemometre.h"

class CApp : public QObject
{
    Q_OBJECT
public:
    explicit CApp(QObject *parent = nullptr);
    ~CApp();
    QByteArray getDataSessionToCsv();
    void startCoureurs();

private:
    CBdd _bdd;
    CSignalisation _feu;
    CAnemometre _vent;
    CCapteurPassage *_cp1, *_cp2;
    QDateTime _dt0; // temps de départ
    bool _session;
    bool _courseEnCours;
    bool _finCoureur1, _finCoureur2;
    int _no; // numéro de la ligne de course

signals:
    void sig_afficherTemps(QDateTime dt, int no, int position);
    void sig_courseFinished();
    void sig_afficherNomSessionActive(QString nom);
    void sig_afficherTableau(QList<QList<QString> > liste);

public slots:
    void on_startNewSession(QString noSession);
    void on_stopSession(QList<QList<QString> > liste);

    void on_startCourse(int no);
    void on_avantCourse();
    void on_aVosMarques();
    void on_pret();
    void on_partez();
    void on_coureurArrived(QDateTime dt);
    void on_stopCourse();

    void on_getFichierCSV();
    void on_verifSession();
    void on_majCouloir(QString nom, int no, int position);
};

#endif // CAPP_H
