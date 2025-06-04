#include "capp.h"

CApp::CApp(QObject *parent) : QObject(parent)
{
    _zdc = new CZdc();
    _zdc->setControleLocal(true);
    _bdd = new CBdd();
    _csv = new CCsv();

    // authentification
    QMessageBox msgBox;
    QString codePIN;
    int nbEssais = 3;
    do {
        _uiLogin = new CGuiLogin(nullptr, &codePIN);
        _uiLogin->exec(); //---------------------------------- IHM
        delete _uiLogin;
        if(!_bdd->verifCodePIN(codePIN)){
            // ouvrir popup code mauvais
            msgBox.setText("Ce n'est pas le bon code PIN, veuillez réessayer.");
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();
        } else
            break;
        nbEssais--;
    } while(nbEssais);  // prévoir nombre d'essai limite
    if (!nbEssais)
        on_exit();

    // LE code PIN est bon
    bool res = _bdd->getParamSession(_paramSession);
    if (!res) {
        msgBox.setText("ERREUR : Table 'Config' non initialisée dans la BDD.");
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    } // if !res
    // sauver params session dans ZDC
    _zdc->setParamSession(_paramSession);

    //  test session active
    if (_paramSession.active) {
        // lire les coureurs dans la BDD et param session et stocker dans ZDC
        _typeCourse = _paramSession.typeCourse;
        _bdd->getCourses(_courses);
        // sauver dans la ZDC
        _zdc->setCourses(_courses);
    } else { // session non active
        // création d'une session
        creerSession("Session"+QDateTime::currentDateTime().toString("ddMMyyyyhhmm")); // Utilisé aussi par on_new_session()
    } // else session non active

    // -----------------------------------------------------------------
    creerUiSession(); //-- IHM

    // tout est initialisé dans BDD et ZDC, la session peut commmencer
    // lancement du serveur TCP pour la tablette
/*    _serv = new CServeur(_bdd);
    // Connexion des signaux de contrôle de la tablette
    connect(this, &CApp::sig_srvGetControl, _serv, &CServeur::on_srvGetControl);
    connect(_serv, &CServeur::sig_srvRemoteGetControl, this, &CApp::on_srvRemoteGetControl);
    connect(_serv, &CServeur::sig_newBtnStateFromTablette, _uiSession, &CGuiSession::on_newBtnStateFromTablette);
    connect(_uiSession, &CGuiSession::sig_newBtnStateToTablette, _serv, &CServeur::on_remoteNewBtnToTablette);
*/
    // Initialisation de la signalisation worker thread
    _thSign = new QThread();
    _sign = new CSignalisation();
    _sign->moveToThread(_thSign);
    // Connexion et lancement des signaux du thread
    connect(_thSign, &QThread::finished, _sign, &QObject::deleteLater);
    connect(_uiSession, &CGuiSession::sig_toWorkerThread, _sign, &CSignalisation::on_goTravail);
    _thSign->start(); // signal émis dans pbPreparation

    // Initialisation des objets E/S
    _giroAne = new CGiroAnemometre();
    _thGiroAne = new QThread();
    _giroAne->moveToThread(_thGiroAne);
    connect(_thGiroAne, &QThread::finished, _giroAne, &QObject::deleteLater);
    connect(this, &CApp::sig_runAcqAneGiro, _giroAne, &CGiroAnemometre::on_goTravail);
    // Qt::DirectConnection important pour emettre le signal
    // dans une boucle infinie du thread
    connect(_giroAne, &CGiroAnemometre::sig_VentDir, this, &CApp::on_ventDirRT, Qt::DirectConnection);
    _thGiroAne->start();
    emit sig_runAcqAneGiro(); // lance l'acquisition du giro et anemo

    _capteurPassage1 = new CCapteurPassage(nullptr, 17, 1);
    _capteurPassage2 = new CCapteurPassage(nullptr, 27, 2);
    // les connects sont au démarrage de la course
} // constructeur CApp

CApp::~CApp()
{
    delete _sign;
    _thSign->quit();
    _thSign->wait();
    delete _giroAne;
//    delete _serv;

    delete _csv;
    delete _bdd;
    delete _zdc;
}

qint64 CApp::calculerDeltaT(QDateTime t1, QDateTime t2) //-- 2024
{
    quint64 deltaTms = static_cast<quint64> (t1.msecsTo(t2));
    return deltaTms;
}

float CApp::calculerVitesse(float tms) //-- 2024
{
    float vitesse;
    vitesse = ((_typeCourse==DEPART_ARRETE?50.0:10.0)/(tms/1000))*3.6;  // km/h
    return vitesse;
}

void CApp::creerSession(QString nomSession) //-- 2024
{
   // initialisation de l'application pour une nouvelle session de course
    // vider la BDD
    T_2COUREURS coureur;
    memset(&coureur, 0, sizeof(T_2COUREURS));
    for (int i= NB_COURSES ; i>0 ; i--)
        _bdd->sauveCourse(i, coureur);

    // Choix du type de courses
    _typeCourse=DEPART_ARRETE;  // par défaut 2 coureurs
    _uiTypeCourse = new CGuiTypeCourse(nullptr, &_typeCourse);
//        connect(_uiTypeCourse, &CGuiTypeCourse::finished, this, &CApp::on_exit);
    _uiTypeCourse->exec(); //------------------------------------------------IHM
    delete _uiTypeCourse;

    // constituer les paramSession et les sauver dans BDD et ZDC
    _paramSession.active = true;
    _paramSession.typeCourse = _typeCourse;
    memcpy(_paramSession.dateDeb, QDate::currentDate().toString("dd-MM-yy").toStdString().c_str(),sizeof(_paramSession.dateDeb));
    memcpy(_paramSession.heureDeb, QTime::currentTime().toString().toStdString().c_str(), sizeof(_paramSession.heureDeb));
    memcpy(_paramSession.nom, nomSession.toStdString().c_str(), sizeof(_paramSession.nom));
    _bdd->setParamSession(_paramSession);
    _zdc->setParamSession(_paramSession);

    // Choisir le fichier contenant les noms des coureurs
    QString pathFile = _csv->chooseImportFile();
    QStringList coureurs;
    if (!pathFile.isEmpty()) { // si fichier rempli
        // lire fichier
        coureurs = _csv->importCSV(pathFile, _typeCourse);
    } // if chemin pas vide

    // nom par défaut des coureurs restant
    for (int i=coureurs.size() ; i<(2*NB_COURSES) ; i++) { // termine le fichier au cas ou !
        coureurs.append("Coureur "+QString::number(i+1));
        if (_typeCourse==DEPART_LANCE) {
            coureurs.append("noname");
            i++;
        } // if départ lancé
    } // for
    // placer les coureurs dans _courses
qDebug() << "CApp::creerSession: sauve coureurs dans BDD";
    _bdd->setListeCoureurs(coureurs);
    qDebug() << "CApp::creerSession: converti les coureurs (QStringList) en _courses (QList<T_2COUREURS>";
    setCoureursInCourses(coureurs);
    qDebug() << "CApp::creerSession: sauve coureurs dans ZDC";
    _zdc->setListeCoureurs(_courses);
    // -----------------------------------------------------------------
}

void CApp::creerUiSession() { //-- 2024
    // N° de la course
    _noCourse = 1;

    // lance l'IHM de session sans bloquer CApp
    _uiSession = new CGuiSession(nullptr);
    //connect(_uiSession, &CGuiSession::destroyed, this, &CApp::on_exit);
    //connect(_uiSession, &CGuiSession::sig_toWorkerThread, _sign, &CSignalisation::on_goTravail);
    connect(_uiSession, &CGuiSession::sig_timerStart, this, &CApp::on_departCourse);
    connect(_uiSession, &CGuiSession::sig_setNoCourse, this, &CApp::on_setNoCourse);
    connect(_uiSession, &CGuiSession::sig_toWorkerThread, _uiSession, &CGuiSession::on_bloqueCoureur);
    connect(_uiSession, &CGuiSession::sig_finCourse, _uiSession, &CGuiSession::on_debloqueCoureur);
    connect(_uiSession, &CGuiSession::sig_newSession, this, &CApp::on_newSession);
    connect(_uiSession, &CGuiSession::sig_endSession, this, &CApp::on_endSession);
    connect(_uiSession, &CGuiSession::sig_runnersExport, this, &CApp::on_runnersExport);
    connect(this, &CApp::sig_majCourse, _uiSession, &CGuiSession::on_afficherCourse);
    _uiSession->show(); //--------------------------------------------------------------IHM
}

void CApp::setCoureursInCourses(QStringList coureurs)
{
    T_2COUREURS cour2;
    memset(&cour2, 0, sizeof(T_2COUREURS));
    for(int i=0 ; i<NB_COURSES ; i++) {
        _courses.append(cour2);
        sprintf(_courses[i].noms[0],"%s", (char *)coureurs.at(2*i).toStdString().c_str());
        memcpy(_courses[i].noms[1], coureurs.at(2*i+1).toStdString().c_str(), 49);
    } // for
}

void CApp::on_runnersExport() //-- 2024
{
    QString path = _csv->chooseExportFile();
    _csv->exportCSV(path);
}

void CApp::on_ventDirRT(float vent, int dir)
{
    _zdc->setVitesseVentRT(vent);
    _zdc->setDirectionVentRT(dir);
}


void CApp::on_getControl()
{
    emit sig_srvGetControl(); // intéret ?
    // débloquer l'IHM locale
}

void CApp::on_srvRemoteGetControl()
{
    emit sig_appRemoteGetControl(); // je vois pas l'intérêt !!!
    // il faut bloquer l'IHM locale en lecture seule
}

void CApp::on_exit() //--- 2024
{
    _uiSession->close();
    QApplication::exit(0);
}

void CApp::on_coureurArrived(int noCapteur, QDateTime t)
{
    float tms, vit;
    int pos=0;

    // selon le numéro du capteur
    switch(noCapteur) {
    case 1: // capteur 1
        _t1 = t;
        qDebug() << "Capteur 1";
        disconnect(_capteurPassage1, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
        if (_typeCourse == DEPART_ARRETE) {
            _zdc->setCoureurArrived(_noCourse, pos, true);
            tms = calculerDeltaT(_td, _t1);
            _zdc->setTempsCoureur(_noCourse, pos, tms);
            vit = calculerVitesse(tms);
            _zdc->setVitesseCoureur(_noCourse, pos, vit);
            float vitVent = _zdc->getVitesseVentRT();
            _zdc->setVitesseVent(_noCourse, vitVent, false);
            int dirVent = _zdc->getDirectionVentRT();
            _zdc->setDirectionVent(_noCourse, dirVent);
            emit sig_majCourse(_noCourse); // demande de mise à jour de l'affichage
        } else {
            connect(_capteurPassage2, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
        } // else DEPART_LANCE
        break;
    case 2:  // capteur 2
        _t2 = t;
        qDebug() << "Capteur 2";
        disconnect(_capteurPassage2, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
        pos=1;
        _zdc->setCoureurArrived(_noCourse, pos, true);
        if (_typeCourse == DEPART_ARRETE) {
            tms = calculerDeltaT(_td, _t2);
        } else { // départ lancé
            pos=0;
            tms = calculerDeltaT(_t1, _t2);
        } // else
        _zdc->setTempsCoureur(_noCourse, pos, tms);
        vit = calculerVitesse(tms);
        _zdc->setVitesseCoureur(_noCourse, pos, vit);
        float vitVent = _zdc->getVitesseVentRT();
        _zdc->setVitesseVent(_noCourse, vitVent, false);
        int dirVent = _zdc->getDirectionVentRT();
        _zdc->setDirectionVent(_noCourse, dirVent);
        emit sig_majCourse(_noCourse); // demande de mise à jour de l'affichage
        break;
    } // sw
    if (_zdc->getCoureurArrived(_noCourse, 0) && _zdc->getCoureurArrived(_noCourse, 1)) {
        T_2COUREURS coureurs;
        _zdc->getCourse(_noCourse, coureurs);
        _bdd->sauveCourse(_noCourse, coureurs);
    } // if fini
} // method

void CApp::on_departCourse() //--- 2024
{
    _td = QDateTime::currentDateTime(); // utile pour type course 1
    connect(_capteurPassage1, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
    if (_typeCourse == DEPART_ARRETE) {
        connect(_capteurPassage2, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
    }
    _zdc->setVitesseVent(_noCourse, 0, true);  // init du vent au départ de la course
}

void CApp::on_setNoCourse(int no) //-- 2024
{
    _noCourse = no;
}

void CApp::on_newSession(QString nomSession) //-- 2024
{
    disconnect(_uiSession, &CGuiSession::sig_toWorkerThread, _sign, &CSignalisation::on_goTravail);
    _uiSession->close();
    delete _uiSession;
    creerSession(nomSession);
    creerUiSession();
    connect(_uiSession, &CGuiSession::sig_toWorkerThread, _sign, &CSignalisation::on_goTravail);
}

void CApp::on_endSession() //-- 2024
{
    // bdd et zdc mettre session inactive et laisser les donner
    _bdd->setSessionActive(false);
    _paramSession.active = false;
    _zdc->setParamSession(_paramSession);
}
