#ifndef CGUISESSION_H
#define CGUISESSION_H

#include <QMainWindow>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>
#include <QTableWidget>
#include <QLineEdit>
#include <QAction>
#include <QComboBox>
#include <QTimer>
#include <QList>
#include <QStringList>
#include "cbdd.h"
#include "czdc.h"

QT_BEGIN_NAMESPACE
namespace Ui { class CGuiSession; }
QT_END_NAMESPACE

class CGuiSession : public QMainWindow
{
    Q_OBJECT

public:
    CGuiSession(QWidget *parent = nullptr);
    ~CGuiSession();

private slots:
    void on_pbPreparation_clicked();
    void on_pbAvm_clicked();
    void on_pbPret_clicked();
    void on_pbPartez_clicked();
    void on_pbArret_clicked();
    void on_pbStart_clicked();// Start la session => Devient Stop la session
    void on_reboot();
    void on_shutdown();
    void on_quitterApp();
    void on_changerCodePIN();
    void on_pbControl_clicked();
    void on_pbLeft_clicked();
    void on_pbRight_clicked();
    void on_chooseExportFile();
    void on_timeout();

signals:
    void sig_timerStart();
    void sig_runnersImport(QStringList nomsEleves);
    void sig_runnersExport();
    void sig_getControl();
    void sig_toWorkerThread();
    void sig_ihmGetControl();
    void sig_finCourse();
    void sig_newBtnStateToTablette(T_BUTTONS buttons);
    void sig_goSession();
    void sig_newSession(QString nomSession);
    void sig_endSession();
    void sig_setNoCourse(int noCourse);
    void sig_changeNoms1Course(int noCourse, QString nom1, QString nom2);

public slots:
    void on_appRemoteGetControl();
    void on_newBtnStateFromTablette(T_BUTTONS buttons); //-- 2024
    void on_afficherCourse(int noCourse);
    void on_stopRun();
    void on_finCourse();
    void on_setNoCourse(int noCourse);
    void on_bloqueCoureur(); //-- 2024
    void on_debloqueCoureur(); //-- 2024

private:
    Ui::CGuiSession *ui;
    CZdc *_zdc;
    QTimer tmrAffVentDir;

    void idPage();
    void mdpPage();
    int cpt1;
    int cpt2;
    int _noCourse; //--- 2024

    T_PARAMSESSION _paramSession;
    QStringList _coureurs;

    QList<QLineEdit *> _lineEdits;
    QStringList _nom;
    QStringList _temps;
    QStringList _vitesse;
    QStringList _vent;
    QStringList _listTpm;
    T_BUTTONS _buttons;
    QLineEdit *_leAuthentification;
    int _typeCourse;  //--- 2024  type de course  2 : 2 coureurs   1 : 1 coureur

    void changeNomCoureursDansListe(int noCourse, QString nom1, QString nom2);
    QString conversionTempsCoureurToMSCString(int t);
    QString conversionVitesseCoureurToString(float v);
    void fillCombosNomsCoureurs(); //-- 2024
    void fillListe();
    void majAff1Course(int noCourse); //-- 2024
};
#endif // CGUISESSION_H
