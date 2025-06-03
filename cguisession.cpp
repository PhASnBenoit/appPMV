#include "cguisession.h"
#include "ui_cguisession.h"

CGuiSession::CGuiSession(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CGuiSession)
{
    _zdc = new CZdc();
    /* Lancement de l'interface principale */
    ui->setupUi(this);
    _noCourse = 1;
    _typeCourse = _zdc->getTypeCourse();
    emit sig_setNoCourse(_noCourse); // vers CApp

    tmrAffVentDir.setInterval(1000);
    tmrAffVentDir.start();

    connect(&tmrAffVentDir, &QTimer::timeout, this, &CGuiSession::on_timeout);

    if (_typeCourse == DEPART_LANCE) {
        ui->lTypeCourse->setText("Départ lancé, un seul coureur !");
        ui->gbCoureur2->setEnabled(false);
    } else {
        ui->pbPreparation->setEnabled(true);
        ui->lTypeCourse->setText("Départ arrêté, deux coureurs !");
    } // else

    // Obtention des données session et course
    _zdc->getParamSession(_paramSession);
    _zdc->getListeCoureurs(_coureurs);  // toujours 40 noms
    ui->lGestionSession->setText("GESTION SESSION : EN COURS !");
    ui->leNomSession->setText(QString(_paramSession.nom));
    ui->leNomSession->setDisabled(true);
    majAff1Course(_noCourse);

    ui->pbStart->setText("FERMER");

    /* Tableau de valeurs (en responsive)+ ComboBox des coureurs après import CSV */
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // création et remplissage des QlineEdits
    for(int i = 0; i < (2*NB_COURSES); i++) {
        _lineEdits.append(new QLineEdit);
        _lineEdits.last()->setText(_coureurs.at(i));
        _lineEdits.last()->setReadOnly(true);
        if ( (_paramSession.typeCourse==1) && (i%2) )
            _lineEdits.last()->setEnabled(false);
        ui->tableWidget->setCellWidget(i/2, (i%2?3:0),_lineEdits.last());
    } // for
qDebug() << "CGuiSession::CGuiSession: Remplissage des combos de la zone courses.";
    fillCombosNomsCoureurs();
qDebug() << "CGuiSession::CGuiSession: Remplissage de la liste.";
    fillListe();

    // TO DO verrouillage des colonnes du coureur 2 si course type 1
    //ui->tableWidget->setCurrentCell(0,4);

    /* Execution commandes système */
    connect(ui->actionQuitter, &QAction::triggered, this, &CGuiSession::on_quitterApp);
    connect(ui->actionRedemarrage,&QAction::triggered,this,&CGuiSession::on_reboot);
    connect(ui->actionArreter, &QAction::triggered, this, &CGuiSession::on_shutdown);
    connect(ui->actionExport_CSV_2, &QAction::triggered, this, &CGuiSession::on_chooseExportFile); // Exportation du Fichier
    //connect(ui->actionChanger_le_code, &QAction::triggered, this, &CGuiSession::on_changerCodePIN);// Changer le code

/* Execution des commandes CSV
    connect(ui->actionImport_CSV,&QAction::triggered, _csv, &CCsv::on_chooseImportFile); // Importation du Fichier
    connect(_csv, &CCsv::sig_afficherNomsEleves, this, &CGuiSession::on_afficherNomsEleves);
    connect(_csv, &CCsv::sig_exporterResultats, this, &CGuiSession::on_exporterSession);
*/

    for(int i = 0; i < _lineEdits.count(); i++) {
        ui->tableWidget->setCellWidget(20, 8, _lineEdits.at(i));
    }// for

    /* Etat des boutons au démarrage */
    //ui->pbControl->setEnabled(false);
    //ui->pbControl->setVisible(false);
    switch(_typeCourse) {
    case DEPART_ARRETE:
        ui->pbAvm->setDisabled(true);
        ui->pbPret->setDisabled(true);
        ui->pbPartez->setDisabled(true);
        ui->pbArret ->setDisabled(true);
        break;
    case DEPART_LANCE:
        ui->pbPreparation->setEnabled(true);
        ui->pbAvm->setEnabled(false);
        ui->pbPret->setEnabled(false);
        ui->pbPartez->setEnabled(false);
        ui->pbArret ->setEnabled(false);
        break;
    }


    memset(&_buttons, 0, sizeof(T_BUTTONS));  // init donnée membre image des boutons
    _zdc->sauveButtons(_buttons); // synchro avec mem partagée
}

CGuiSession::~CGuiSession()
{
    for(int i = 0; i < _lineEdits.count(); i++){
        delete _lineEdits.at(i);
    }
    _lineEdits.clear();
    delete _zdc;
    delete ui;
}


        /* ------------------------- */
        /*          BOUTONS          */
        /* ------------------------- */

void CGuiSession::on_pbPreparation_clicked()
{
    T_DATAS datas;
    _zdc->getDatas(datas);
    datas.activeSignalisation = true;
    datas.modeDeFonctionnement = RAPIDE;
    _zdc->sauveDatas(datas);

    ui->pbAvm->setEnabled(true);
    emit sig_toWorkerThread();
    if (_zdc->getControleLocal()) {  // pourrait être la cause d'un renvoi de trame, idem pour les autres boutons.
        T_BUTTONS buttons;
        _zdc->getButtons(buttons);
        memset(&buttons, 0, sizeof(T_BUTTONS));
        buttons.btnPreparation = 1;
        _zdc->setButtons(buttons);
        emit sig_newBtnStateToTablette(buttons);
    } // if

    switch(_typeCourse) {
    case DEPART_ARRETE:
        ui->pbPreparation->setEnabled(false);
        ui->pbAvm->setEnabled(true);
        ui->pbPret->setEnabled(false);
        ui->pbPartez->setEnabled(false);
        ui->pbArret->setEnabled(true);
        break;
    case DEPART_LANCE:
        ui->pbPreparation->setEnabled(false);
        ui->pbAvm->setEnabled(false);
        ui->pbPartez->setEnabled(true);
        ui->pbArret->setEnabled(true);
        break;
    } // sw
}

void CGuiSession::on_pbAvm_clicked()
{
    T_DATAS datas;
    _zdc->getDatas(datas);
    datas.activeSignalisation = true;
    datas.modeDeFonctionnement = LENT;
    _zdc->sauveDatas(datas);
    if (_zdc->getControleLocal()) {
        T_BUTTONS buttons;
        _zdc->getButtons(buttons);
        memset(&buttons, 0, sizeof(T_BUTTONS));
        buttons.btnAVM = 1;
        _zdc->setButtons(buttons);
        emit sig_newBtnStateToTablette(buttons);
    } // if

    ui->pbPreparation->setEnabled(true);
    ui->pbAvm->setEnabled(false);
    ui->pbPret->setEnabled(true);
    ui->pbPartez->setEnabled(false);
    ui->pbArret->setEnabled(true);
}

void CGuiSession::on_pbPret_clicked()
{
    T_DATAS datas;
    _zdc->getDatas(datas);
    datas.activeSignalisation = true;
    datas.modeDeFonctionnement = ETEINT;
    _zdc->sauveDatas(datas);
    if (_zdc->getControleLocal()) {
        T_BUTTONS buttons;
        _zdc->getButtons(buttons);
        memset(&buttons, 0, sizeof(T_BUTTONS));
        buttons.btnReady = 1;
        _zdc->setButtons(buttons);
        emit sig_newBtnStateToTablette(buttons);
    } // if

    ui->pbPreparation->setEnabled(false);
    ui->pbAvm->setEnabled(true);
    ui->pbPret->setEnabled(false);
    ui->pbPartez->setEnabled(true);
    ui->pbArret->setEnabled(true);
}

void CGuiSession::on_pbPartez_clicked()
{
    T_DATAS datas;
    _zdc->getDatas(datas);
    datas.activeSignalisation = false;
    datas.modeDeFonctionnement = FIXE;
    _zdc->sauveDatas(datas);

    if (_zdc->getControleLocal()) {
        T_BUTTONS buttons;
        _zdc->getButtons(buttons);
        memset(&buttons, 0, sizeof(T_BUTTONS));
        buttons.btnGo = 1;
        _zdc->setButtons(buttons);
        emit sig_newBtnStateToTablette(buttons);
    } // if
    switch(_typeCourse) {
    case DEPART_ARRETE:
        ui->pbPreparation->setEnabled(false);
        ui->pbAvm->setEnabled(false);
        ui->pbPret->setEnabled(false);
        ui->pbPartez->setEnabled(false);
        ui->pbArret->setEnabled(true);
        break;
    case DEPART_LANCE:
        ui->pbPartez->setEnabled(false);
        ui->pbArret->setEnabled(true);
        //emit sig_toWorkerThread();
        break;
    } //sw

    emit sig_timerStart(); // vers _app pour lancer la course
}

void CGuiSession::on_pbArret_clicked()
{
    T_DATAS datas;
    QMessageBox msgBox;
    if (_zdc->getControleLocal()) {
        msgBox.setText("Voulez-vous arrêter la course ?");
        msgBox.setInformativeText("Sans attendre la détection de l'arrivée ?");
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        int reponse = msgBox.exec();
        if(reponse == QMessageBox::Yes)
        {
            _zdc->getDatas(datas);
            datas.activeSignalisation = false;
            datas.modeDeFonctionnement = ETEINT;
            _zdc->sauveDatas(datas);
            T_BUTTONS buttons;
            _zdc->getButtons(buttons);
            memset(&buttons, 0, sizeof(T_BUTTONS));
            buttons.btnArret = 1;
            _zdc->setButtons(buttons);
            emit sig_newBtnStateToTablette(buttons);
            switch(_typeCourse) {
            case DEPART_ARRETE:
                ui->pbPreparation->setEnabled(true);
                ui->pbAvm->setEnabled(false);
                ui->pbPret->setEnabled(false);
                ui->pbPartez->setEnabled(false);
                ui->pbArret->setEnabled(false);
                break;
            case DEPART_LANCE:
                ui->pbPreparation->setEnabled(true);
                ui->pbPartez->setEnabled(false);
                ui->pbArret ->setEnabled(false);
                break;
            } //sw
            //on_stopRun();// Slot pour la fin de chaque course
            emit sig_finCourse(); // autorise le changement de coureur
        } // if yes
    } // if
}

// CHANGEMENT DE SESSION
void CGuiSession::on_pbStart_clicked()
{
    QMessageBox msgBox;
    if(ui->pbStart->text()=="DEMARRER") {
            msgBox.setText("Voulez-vous lancer une nouvelle session?");
            msgBox.setInformativeText("N'oubliez pas de donner un nom à la session.");
            msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
            msgBox.setDefaultButton(QMessageBox::No);
            int reponse = msgBox.exec();
            if(reponse == QMessageBox::Yes)
            {
                ui->lGestionSession->setText("GESTION SESSION : EN COURS !");
                ui->pbStart->setText("FERMER");
                if (_typeCourse == DEPART_ARRETE)
                    ui->pbPreparation->setEnabled(true);
                QDateTime dt = QDateTime::currentDateTime();
                emit sig_newSession("Session_"+dt.toString("dd-MM-yyyy_hh:mm:ss")); // vers CApp
            } // if yes
    } else {
        msgBox.setText("Voulez-vous arrêter la session en cours?");
        msgBox.setInformativeText("Pensez à sauver les données (menu CSV/Export CSV)");
        msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
        msgBox.setDefaultButton(QMessageBox::No);
        int reponse = msgBox.exec();
        if(reponse == QMessageBox::Yes)
        {
            ui->pbStart->setText("DEMARRER");
            ui->lGestionSession->setText("GESTION SESSION : FERMEE !");
            ui->lTypeCourse->setText("Session fermée");
            ui->gbCourses->setEnabled(false);
            ui->leNomSession->setEnabled(true);
            ui->pbPreparation->setDisabled(true);
            ui->pbAvm->setDisabled(true);
            ui->pbPret->setDisabled(true);
            ui->pbPartez->setDisabled(true);
            emit sig_endSession(); // vers CApp pour changer session_active en false
            //close();
        } // if yes
     }// else
}

//----BARRE DE MENU----//

void CGuiSession::on_quitterApp()
{
     QMessageBox msgBox;
     msgBox.setText("Voulez-vous quitter l'application?");
     msgBox.setInformativeText("Pensez à STOPPER et EXPORTER la session");
     msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
     msgBox.setDefaultButton(QMessageBox::No);
     int reponse = msgBox.exec();
     if(reponse == QMessageBox::No){
     }else
         QCoreApplication::quit();
}// Quitter

void CGuiSession::on_changerCodePIN()
{
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous changer votre code d'Authentification ?");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int reponse = msgBox.exec();
    if(reponse == QMessageBox::No){}
    else {
        /* Entrer le code */
        QMessageBox::information(this, "Changer le code d'authentification", "Veuillez entrer votre nouveau code d'Authentification");
        _leAuthentification->text();
        msgBox.setDefaultButton(QMessageBox::Ok);

        /* Confirmer le code */
        QMessageBox::information(this, "Confirmation du nouveau code d'Authentification", "Confirmer votre nouveau code");
        _leAuthentification->text();
        msgBox.setDefaultButton(QMessageBox::Ok);
    }
}

void CGuiSession::on_reboot()
{
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous redémarrer l'appareil??");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int reponse = msgBox.exec();
    if(reponse == QMessageBox::No){}
    else
    {
    system("/usr/bin/systemctl reboot");
    }
}// Redémarrage

void CGuiSession::on_shutdown()
{
    QMessageBox msgBox;
    msgBox.setText("Voulez-vous éteindre l'appareil??");
    msgBox.setStandardButtons(QMessageBox::No | QMessageBox::Yes);
    msgBox.setDefaultButton(QMessageBox::No);
    int reponse = msgBox.exec();
    if(reponse == QMessageBox::No){}
    else
    {
    system("sudo shutdown -h now");
    }
}// Eteindre

            //----BARRE DE MENU----//


            //----AFFICHAGE IHM----//

void CGuiSession::fillListe() {
    T_DATAS datas;
    _zdc->getDatas(datas);

    for(int i=0 ; i<NB_COURSES ; i++) {
        _lineEdits.at(i*2)->setText(QString(datas.coureurs[i].noms[0]));
        _lineEdits.at(i*2+1)->setText(QString(datas.coureurs[i].noms[1]));

        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(conversionTempsCoureurToMSCString(datas.coureurs[i].temps[0]))); // temps1
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(conversionTempsCoureurToMSCString(datas.coureurs[i].temps[1]))); // temps2

        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(conversionVitesseCoureurToString(datas.coureurs[i].vitesse[0]))); // vitesse1
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(conversionVitesseCoureurToString(datas.coureurs[i].vitesse[1]))); // vitesse2

        ui->tableWidget->setItem(i, 6, new QTableWidgetItem(QString::number(datas.coureurs[i].vent,'f',0)+" km/h")); // vent
        ui->tableWidget->setItem(i, 7, new QTableWidgetItem(convAngleToPointCardinal(datas.coureurs[i].dirVent))); // dir
    } // for
}

void CGuiSession::on_afficherCourse(int noCourse)
{
    fillListe(); // maj de la liste

    // si les deux coureurs sont arrivés
    if (_zdc->getCoureurArrived(noCourse, 0) && _zdc->getCoureurArrived(noCourse, 1)) {
        ui->pbPreparation->setEnabled(true);
        ui->pbAvm->setEnabled(false);
        ui->pbPret->setEnabled(false);
        ui->pbPartez->setEnabled(false);
        ui->pbArret->setEnabled(false);
        ui->pbLeft->setEnabled(true);
        ui->pbRight->setEnabled(true);
    }
    majAff1Course(noCourse);
}

QString CGuiSession::conversionTempsCoureurToMSCString(int t)
{
    int deltaTms, ts, tms;
    //int tmn;
    QString temps="";

    deltaTms = t;
    //tmn = deltaTms / 60000;
    //deltaTms -= tmn*60000;
    ts = deltaTms/1000;
    deltaTms -= ts*1000;
    tms = deltaTms; // ce qui reste en ms
    //temps = QString::number(tmn)+":";
    temps = QString::number(ts)+",";
    temps += QString::number(tms)+" s";
    return temps;
}

QString CGuiSession::conversionVitesseCoureurToString(float v)
{
    QString vitesse="";
    float vit;

    vit = v;
    vitesse = QString::number(vit,'f', 1)+" km/h";
    return vitesse;
}

//----AFFICHAGE IHM----//

void CGuiSession::fillCombosNomsCoureurs() //-- 2024
{
        ui->cbNom1->clear();
        ui->cbNom1->addItems(_coureurs);
        ui->cbNom1->setCurrentIndex((_noCourse-1));
        ui->cbNom2->clear();
        ui->cbNom2->addItems(_coureurs);
        ui->cbNom2->setCurrentIndex(_noCourse);
    //emit sig_runnersImport(_nomsEleves); // a voir
}

void CGuiSession::on_appRemoteGetControl()
{
    ui->tableWidget->setDisabled(true);
    ui->pbAvm->setDisabled(true);
    ui->pbPreparation->setDisabled(true);
    ui->pbPret->setDisabled(true);
    ui->pbPartez->setDisabled(true);
    ui->pbArret->setDisabled(true);

    //ui->pbControl->setVisible(true);
    //ui->pbControl->setDisabled(false);
}

void CGuiSession::on_newBtnStateFromTablette(T_BUTTONS buttons) //-- 2024
{
    // synchro suite à une état bouton de la tablette
    _zdc->setButtons(buttons);
    if(buttons.btnPreparation){
        on_pbPreparation_clicked();
    }
    if(buttons.btnAVM){
        on_pbAvm_clicked();
    }
    if(buttons.btnReady){
        on_pbPret_clicked();
    }
    if(buttons.btnGo){
        on_pbPartez_clicked();
    }
    if(buttons.btnArret){
        on_pbArret_clicked();
    }
}


void CGuiSession::on_stopRun()
{
 // rien dedans
}

void CGuiSession::on_finCourse()
{
    ui->pbArret->setEnabled(true);
}

void CGuiSession::on_setNoCourse(int noCourse)
{
    _noCourse = noCourse;
}

void CGuiSession::changeNomCoureursDansListe(int noCourse, QString nom1, QString nom2)
{
  // forcer la mise à jour du tableau
  _lineEdits.at((noCourse-1)*2)->setText(nom1);
  _lineEdits.at((noCourse-1)*2+1)->setText(nom2);
}

void CGuiSession::on_pbControl_clicked()
{
    ui->tableWidget->setDisabled(false);
    _zdc->getButtons(_buttons);
    if(_buttons.btnPreparation){
       ui->pbAvm->setEnabled(true);
    }
    if(_buttons.btnAVM){
        ui->pbPreparation->setEnabled(true);
        ui->pbPret->setEnabled(true);
    }
    if(_buttons.btnReady){
         ui->pbAvm->setEnabled(true);
         ui->pbPartez->setEnabled(true);
    }
    if(_buttons.btnGo){
        ui->pbArret->setEnabled(true);
    }
    if(_buttons.btnArret){
        ui->pbPreparation->setEnabled(true);
    }
    emit sig_ihmGetControl();
}

void CGuiSession::on_pbLeft_clicked()
{
    // maj éventuelle des noms des coureurs
    _zdc->setNomCoureur(_noCourse, 0, ui->cbNom1->currentText());
    _zdc->setNomCoureur(_noCourse, 1, ui->cbNom2->currentText());
    ui->cbNom1->setItemText((_noCourse-1)*2, ui->cbNom1->currentText());
    ui->cbNom2->setItemText((_noCourse-1)*2+1, ui->cbNom2->currentText());
    changeNomCoureursDansListe(_noCourse, ui->cbNom1->currentText(), ui->cbNom2->currentText());

    _noCourse--;
    if (_noCourse<1)
        _noCourse=NB_COURSES;
    // afficher les paramètres du coureur
    majAff1Course(_noCourse);
    emit sig_setNoCourse(_noCourse); // change noCourse dans CApp
}

void CGuiSession::on_pbRight_clicked()
{
    // maj éventuelle des noms des coureurs
    _zdc->setNomCoureur(_noCourse, 0, ui->cbNom1->currentText());
    _zdc->setNomCoureur(_noCourse, 1, ui->cbNom2->currentText());
    ui->cbNom1->setItemText((_noCourse-1)*2, ui->cbNom1->currentText());
    ui->cbNom2->setItemText((_noCourse-1)*2+1, ui->cbNom2->currentText());
    changeNomCoureursDansListe(_noCourse, ui->cbNom1->currentText(), ui->cbNom2->currentText());

    _noCourse++;
    if (_noCourse>NB_COURSES)
        _noCourse=1;
    // afficher les paramètres du coureur
    majAff1Course(_noCourse);
    emit sig_setNoCourse(_noCourse);
}

void CGuiSession::on_chooseExportFile()
{
    emit sig_runnersExport();
}

void CGuiSession::majAff1Course(int noCourse) {
    // afficher les paramètres des 2 coureurs
    T_DATAS datas;
    _zdc->getDatas(datas);
    ui->lcdNumber->display(noCourse);
    ui->cbNom1->setCurrentIndex((noCourse-1)*2);
    ui->cbNom2->setCurrentIndex((noCourse-1)*2+1);
    ui->leT1->setText(conversionTempsCoureurToMSCString(datas.coureurs[noCourse-1].temps[0]));
    ui->leT2->setText(conversionTempsCoureurToMSCString(datas.coureurs[noCourse-1].temps[1]));
    ui->leVit1->setText(conversionVitesseCoureurToString(datas.coureurs[noCourse-1].vitesse[0]));
    ui->leVit2->setText(conversionVitesseCoureurToString(datas.coureurs[noCourse-1].vitesse[1]));
    ui->leVent1->setText(QString::number(datas.coureurs[noCourse-1].vent,'f', 0)+" km/h");
    ui->leVent2->setText(QString::number(datas.coureurs[noCourse-1].vent,'f', 0)+" km/h");
    ui->leDir1->setText(convAngleToPointCardinal(datas.coureurs[noCourse-1].dirVent));
    ui->leDir2->setText(convAngleToPointCardinal(datas.coureurs[noCourse-1].dirVent));
}

void CGuiSession::on_timeout()
{
    float vitVentRT = _zdc->getVitesseVentRT();
    ui->leVent->setText(QString::number(vitVentRT,'f',0)+" km/h");

    int dirVentRT = _zdc->getDirectionVentRT();
    ui->leDir->setText(convAngleToPointCardinal(dirVentRT));
}

void CGuiSession::on_bloqueCoureur()
{
    ui->pbRight->setEnabled(false);
    ui->pbLeft->setEnabled(false);
}

void CGuiSession::on_debloqueCoureur()
{
    ui->pbRight->setEnabled(true);
    ui->pbLeft->setEnabled(true);
}

QString CGuiSession::convAngleToPointCardinal(int angleDegrees)
{
    // Déterminer le point cardinal
    if (angleDegrees >= 337.5 || angleDegrees < 22.5) return "Nord";
    if (angleDegrees >= 22.5 && angleDegrees < 67.5) return "Nord-Est";
    if (angleDegrees >= 67.5 && angleDegrees < 112.5) return "Est";
    if (angleDegrees >= 112.5 && angleDegrees < 157.5) return "Sud-Est";
    if (angleDegrees >= 157.5 && angleDegrees < 202.5) return "Sud";
    if (angleDegrees >= 202.5 && angleDegrees < 247.5) return "Sud-Ouest";
    if (angleDegrees >= 247.5 && angleDegrees < 292.5) return "Ouest";
    if (angleDegrees >= 292.5 && angleDegrees < 337.5) return "Nord-Ouest";

    return "Inconnu"; // Par sécurité, mais on ne devrait jamais atteindre ce cas
}


