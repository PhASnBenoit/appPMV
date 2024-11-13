#ifndef CAPP_H
#define CAPP_H

#include <QObject>
#include <QDateTime>
#include <QFile>
#include <QString>
#include <QStringList>
#include <QThread>

#include "cguisession.h"
#include "cguilogin.h"
#include "cguitypecourse.h"

#include "ccapteurpassage.h"
#include "cgiroanemometre.h"
#include "csignalisation.h"

#include "cserveur.h"
#include "ccsv.h"
#include "cbdd.h"
#include "czdc.h"

class CApp : public QObject
{
    Q_OBJECT
public:
    explicit CApp(QObject *parent = nullptr);
    ~CApp();

private:
    CGuiLogin *_uiLogin;   //----- 2024
    CGuiSession *_uiSession;   //------ 2024
    CGuiTypeCourse *_uiTypeCourse;  //----- 2024

    CGiroAnemometre *_giroAne; //--- 2024
    CCapteurPassage *_capteurPassage1; //--- 2024
    CCapteurPassage *_capteurPassage2; //--- 2024
    CSignalisation *_sign; //--- 2024
    CZdc *_zdc; //--- 2024
    CCsv *_csv; //--- 2024
    CServeur *_serv; //--- 2024
    CBdd *_bdd; //--- 2024

    QThread *_thSign;
    QThread *_thGiroAne;

    T_TYPE_COURSE _typeCourse; //--- 2024
    T_PARAMSESSION _paramSession; //--- 2024
    QList<T_2COUREURS> _courses; //--- 2024
    QDateTime _td, _t1, _t2; //--- 2024
    int _noCourse; //--- 2024

    qint64 calculerDeltaT(QDateTime t1, QDateTime t2); //--- 2024
    float calculerVitesse(float tms); //--- 2024
    void creerSession(QString nomSession); //-- 2024
    void creerUiSession(); //-- 2024
    void setCoureursInCourses(QStringList coureurs); //-- 2024

    quint64 _deltaTm;
    quint64 _deltaTs;
    quint64 _deltaTms;
    quint64 _kmPerHour;
    quint64 _km;
    quint64 _h;

    QString lireFichier();
    void calculateSpeed(int ordre);

public slots:

private slots:
    void on_getControl();
    void on_srvRemoteGetControl();
    void on_exit(); //--- 2024
    void on_coureurArrived(int noCapteur, QDateTime t); //--- 2024
    void on_departCourse(); //--- 2024
    void on_setNoCourse(int no); //--- 2024
    void on_newSession(QString nomSession); //-- 2024
    void on_endSession();
    void on_runnersExport(); //-- 2024
    void on_ventDirRT(float vent, int dir); //-- 2024

signals:
    void sig_timerStart();
    void sig_timerStop();
    void sig_resTemps(QString resultatTemps, int ordre, int ligne);
    void sig_resVitesse(QString resultatVitesse, int ordre, int ligne);
    void sig_resVitesseMoyenneVent(QString resultatVitesseMoy, int ordre, int ligne);// signal pour la vitesse du vent
    void sig_readBdd(QString data);
    void sig_srvGetControl(); //--- 2024
    void sig_appRemoteGetControl(); //--- 2024
    void sig_workerThread();
    void sig_sessionName(QString nomSession);
    void sig_endRun();
    void sig_runAcqAneGiro();
    void sig_resTemps(int noCourse, int noCoureur, QString resultatTemps); //--- 2024
    void sig_majCourse(int noCourse); //--- 2024
    void sig_extractDatasExport(QStringList nomsEleves);//Nouveau Signal
    void sig_affSession(QStringList coureurs, QStringList courses);// Affichage de la session
    void sig_importFile(QString path);
    void sig_setNoCoureur(int no); //--- 2024
};

#endif // CAPP_H
