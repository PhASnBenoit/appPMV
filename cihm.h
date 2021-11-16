#ifndef CIHM_H
#define CIHM_H

#include <QMainWindow>
#include <QTableWidget>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>

#include "capp.h"
#include "commun.h"

#define COL_C1 0
#define COL_T1 1
#define COL_V1 2
#define COL_C2 3
#define COL_T2 4
#define COL_V2 5

namespace Ui {
class CIhm;
}

class CIhm : public QMainWindow
{
    Q_OBJECT

public:
    explicit CIhm(QWidget *parent = nullptr);
    ~CIhm();

private:
    Ui::CIhm *ui;
    QTableWidgetItem *_w;
    CApp *_app;
    void initTableau();
    int _no;
    QTimer *_tmr; // scrutation BDD pour ordre.

signals:
    void sig_startNewSession(QString nomSession);
    void sig_stopSession(QList<QList<QString>> liste);
    void sig_startCourse(int no);
    void sig_stopCourse();
    void sig_getFichierCSV();
    void sig_avantCourse();
    void sig_aVosMarques();
    void sig_pret();
    void sig_partez();
    void sig_verifSession();
    void sig_majCouloir(QString nom, int no, int position);

private slots:
    void on_cellClicked(int r, int c);
    void on_pbFichier_clicked();
    void on_pbSS_clicked();
    void on_afficherTemps(QDateTime dt, int no, int position);
    void on_courseFinished();
    void on_pbMarques_clicked();
    void on_pbPret_clicked();
    void on_pbPartez_clicked();
    void on_pbStopCourse_clicked();
    void on_pbAvantCourse_clicked();
    void on_afficherNomSessionActive(QString nomSession);
    void on_afficheTableau(QList<QList<QString> > liste);
    void on_twListe_cellChanged(int row, int column);
};

#endif // CIHM_H
