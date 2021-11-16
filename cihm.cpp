#include "cihm.h"
#include "ui_cihm.h"

CIhm::CIhm(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CIhm)
{
    _no=0;
    ui->setupUi(this);
    ui->twListe->setEnabled(false);
    ui->pbFichier->setEnabled(true);
    ui->pbAvantCourse->setEnabled(true);
    ui->gbOrdres->setEnabled(false);
    ui->pbPartez->setEnabled(false);
    ui->pbPret->setEnabled(false);
    ui->pbStopCourse->setEnabled(false);
    // dimension du tableau
    ui->twListe->setRowCount(NB_ROWS);
    ui->twListe->setColumnCount(NB_COLS);
    connect(ui->twListe, &QTableWidget::cellClicked, this, &CIhm::on_cellClicked);

    ui->leNomSession->setText("Session du "+
                              QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    // création des objets du tableau et remplissage
    initTableau();

    _app = new CApp();
    connect(this, &CIhm::sig_startNewSession, _app, &CApp::on_startNewSession);
    connect(this, &CIhm::sig_stopSession, _app, &CApp::on_stopSession);
    connect(this, &CIhm::sig_startCourse, _app, &CApp::on_startCourse);
    connect(this, &CIhm::sig_stopCourse, _app, &CApp::on_stopCourse);
    connect(this, &CIhm::sig_getFichierCSV, _app, &CApp::on_getFichierCSV);
    connect(this, &CIhm::sig_aVosMarques, _app, &CApp::on_aVosMarques);
    connect(this, &CIhm::sig_pret, _app, &CApp::on_pret);
    connect(this, &CIhm::sig_partez, _app, &CApp::on_partez);
    connect(this, &CIhm::sig_stopCourse, _app, &CApp::on_stopCourse);
    connect(this, &CIhm::sig_verifSession, _app, &CApp::on_verifSession);
    connect(this, &CIhm::sig_majCouloir, _app, &CApp::on_majCouloir);

    connect(_app, &CApp::sig_afficherTemps, this, &CIhm::on_afficherTemps);
    connect(_app, &CApp::sig_courseFinished, this, &CIhm::on_courseFinished);
    connect(_app, &CApp::sig_afficherNomSessionActive, this, &CIhm::on_afficherNomSessionActive);
    connect(_app, &CApp::sig_afficherTableau, this, &CIhm::on_afficheTableau);

    emit sig_verifSession();
} // method

CIhm::~CIhm()
{
    ui->twListe->clear();
    delete _app;
    delete ui;
} // method

void CIhm::initTableau()
{
    ui->twListe->clearContents();

    // création des objets dans le tableau
    QDateTime dt;
    int no=0;
    for (int i=0 ; i<NB_ROWS ; i++) {
        _w = new QTableWidgetItem("Coureur "+QString::number(no++));
        ui->twListe->setItem(i,0,_w);
        _w = new QTableWidgetItem("NC");
        ui->twListe->setItem(i,1,_w);
        _w = new QTableWidgetItem("0 m/s");
        ui->twListe->setItem(i,2,_w);
        _w = new QTableWidgetItem("Coureur "+QString::number(no++));
        ui->twListe->setItem(i,3,_w);
        _w = new QTableWidgetItem("NC");
        ui->twListe->setItem(i,4,_w);
        _w = new QTableWidgetItem("0 m/s");
        ui->twListe->setItem(i,5,_w);
    } // for i

    // background un ligne sur deux
    for (int i=0 ; i<NB_ROWS ; i+=2) {
        for (int j=0 ; j<NB_COLS ; j++) {
            _w = ui->twListe->item(i,j);
            _w->setBackground(QBrush(QColor(Qt::cyan)));
        } // for j
    } // for i
} // method

void CIhm::on_cellClicked(int r, int c)
{
    c+=0; // enlève le warning c unused.
    _no = r;
    ui->twListe->selectRow(_no);
    ui->lNo->setText(QString::number(_no+1));
    ui->pbMarques->setEnabled(false);
    ui->gbOrdres->setEnabled(true);
    ui->pbPartez->setEnabled(false);
} // method

void CIhm::on_pbFichier_clicked()
{
    // demander ou stocker le fichier CSV à créer
    emit sig_getFichierCSV();
} // method

void CIhm::on_pbSS_clicked()
{
    if (ui->pbSS->text() == "START") {
        if (ui->leNomSession->text().isEmpty()) {
            ui->statusBar->showMessage("Saisissez un nom de session.", 10000);
            return;
        } // if isempty

        QMessageBox msgBox;
        msgBox.setText("Attention, toutes les données de la session en cours seront effacées.");
        msgBox.setInformativeText("Voulez-vous continuer ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            ui->pbSS->setText("STOP");
            ui->twListe->setEnabled(true);
            ui->pbFichier->setEnabled(false);
            ui->leNomSession->setEnabled(false);
            initTableau();
            emit sig_startNewSession(ui->leNomSession->text());
        } // if ret

    } else {
        QMessageBox msgBox;
        msgBox.setText("Attention, les données de la session en cours ne seront plus modifiables.");
        msgBox.setInformativeText("Voulez-vous continuer ?");
        msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret = msgBox.exec();
        if (ret == QMessageBox::Yes) {
            ui->gbOrdres->setEnabled(false);
            ui->pbSS->setText("START");
            ui->pbFichier->setEnabled(true);
            ui->twListe->setEnabled(false);
            ui->leNomSession->setEnabled(true);
            // sauve les valeurs du tableau
            QList<QList<QString>> liste;
            QList<QString> ligne;
            for(int i=0 ; i<NB_ROWS ; i++) {
                ligne.clear();
                for (int j=0 ; j<NB_COLS ; j++) {
                    QTableWidgetItem *item = ui->twListe->item(i,j);
                    ligne.append(item->text());
                } // for j
                liste.append(ligne);
            } // for i
            emit sig_stopSession(liste);
        } // if yes
    } // else
}

void CIhm::on_afficherTemps(QDateTime dt, int no, int position)
{
    QTableWidgetItem *w;
    w = ui->twListe->item(no, (position==1?COL_T1:COL_T2));
    w->setText(dt.toString("mm:ss:zzz"));
} // method

void CIhm::on_courseFinished()
{
    ui->twListe->setEnabled(true);
    ui->gbOrdres->setEnabled(true);
    ui->pbMarques->setEnabled(true);
    ui->pbPret->setEnabled(false);
    ui->pbPartez->setEnabled(false);
    ui->pbPartez->setText("Partez !");
    ui->pbStopCourse->setEnabled(false);
    ui->pbAvantCourse->setEnabled(true);
    _no++;
    if (_no>=NB_ROWS-1) _no = 0;
    ui->twListe->selectRow(_no);
    ui->lNo->setText(QString::number(_no+1));
} // method

void CIhm::on_pbAvantCourse_clicked()
{
    ui->pbMarques->setEnabled(true);
    ui->pbPret->setEnabled(false);
    ui->pbPartez->setEnabled(false);
    ui->pbStopCourse->setEnabled(false);
    emit sig_avantCourse();
} // method

void CIhm::on_afficherNomSessionActive(QString nomSession)
{
    _no=0;
    ui->lNo->setText(QString::number(_no));
    ui->twListe->selectRow(_no);
    ui->leNomSession->setText(nomSession);
    ui->leNomSession->setEnabled(false);
    ui->pbSS->setText("STOP");
    ui->pbSS->setEnabled(true);
    ui->pbFichier->setEnabled(false);
    ui->twListe->setEnabled(true);
    ui->gbOrdres->setEnabled(true);
    ui->statusBar->showMessage("Session active en cours rechargée !", 10000);
} // method

void CIhm::on_afficheTableau(QList<QList<QString> > liste)
{
    QList<QString> ligne;
    // sauve les valeurs du tableau
    for(int i=0 ; i<NB_ROWS ; i++) {
        ligne = liste.at(i);
        for (int j=0 ; j<NB_COLS ; j++) {
            QTableWidgetItem *item = ui->twListe->item(i,j);
            item->setText(ligne.at(j));
        } // for j
    } // for i
}

void CIhm::on_pbMarques_clicked()
{
    emit sig_aVosMarques();
    ui->twListe->setEnabled(false);
    ui->pbPret->setEnabled(true);
    ui->pbPartez->setEnabled(false);
    ui->pbStopCourse->setEnabled(true);
} // method

void CIhm::on_pbPret_clicked()
{
    emit sig_pret();
    ui->pbPartez->setEnabled(true);
} // method

void CIhm::on_pbPartez_clicked()
{
    ui->pbAvantCourse->setEnabled(false);
    ui->pbMarques->setEnabled(false);
    ui->pbPret->setEnabled(false);
    ui->pbPartez->setText("En cours...");
    ui->pbPartez->setEnabled(false);
    emit sig_partez();
    emit sig_startCourse(_no);
} // method

void CIhm::on_pbStopCourse_clicked()
{
    QMessageBox msgBox;
    msgBox.setText("Attention, vous stoppez la course manuellement.");
    msgBox.setInformativeText("Voulez-vous continuer ?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Yes) {
        ui->twListe->setEnabled(true);
        ui->pbAvantCourse->setEnabled(true);
        ui->pbMarques->setEnabled(false);
        ui->pbPret->setEnabled(false);
        ui->pbPartez->setEnabled(false);
        ui->pbPartez->setText("Partez !");
        emit sig_stopCourse();
    } // id yes
} // method


void CIhm::on_twListe_cellChanged(int row, int column)
{
    if (column==COL_C1 || column==COL_C2) {
        QTableWidgetItem *w = ui->twListe->item(row, column);
        emit sig_majCouloir(w->text(), row, column);
    } // if couloirs modified
}
