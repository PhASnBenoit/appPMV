#include "capp.h"

CApp::CApp(QObject *parent) : QObject(parent)
{
    _cp1 = _cp2 = nullptr;
    _courseEnCours=false;
} // method

CApp::~CApp()
{
    on_stopCourse();
} // method

QByteArray CApp::getDataSessionToCsv()
{
    // A FAIRE
    return QByteArray();
} // method


void CApp::on_startNewSession(QString nomSession)
{
    // RAZ BDD
    _bdd.resetTableSession();  // reset de la BDD
    _bdd.sauverNomSession(nomSession);
    _session=true;
    _bdd.setSession(true);
//    QList<QList<QString>>liste;
//    _bdd.lireDataSession(nomSession, liste);
//    emit sig_afficherTableau(liste);
} // method

void CApp::on_stopSession(QList<QList<QString>> liste)
{
    if (_courseEnCours) {
        on_stopCourse();
        emit sig_courseFinished();
    } // if course
    _session=false;
    _bdd.setSession(false);
    _bdd.sauverListe(liste);
} // method

void CApp::on_getFichierCSV()
{
    // A FAIRE
} // method

void CApp::on_verifSession()
{
     QString nomSession;
     QList<QList<QString>> liste;
     _bdd.lireDataSession(nomSession, liste);
     if (_bdd.isSessionActive()) {
         emit sig_afficherNomSessionActive(nomSession);
         emit sig_afficherTableau(liste);
         _session=true;
     } else {
         _session=false;
     } // else
} // method

void CApp::on_majCouloir(QString nom, int no, int position)
{
    _bdd.sauverNom(nom, no,position);
} // method

void CApp::on_coureurArrived(QDateTime dt)
{
    qint64 deltat = _dt0.msecsTo(dt);
    CCapteurPassage *c = qobject_cast<CCapteurPassage*>(sender());
    if (c == _cp1) {
        _bdd.sauverTemps(deltat, _no, 1); // 1er coureur
        _finCoureur1 = true;
        delete _cp1;
        _cp1=nullptr;
        dt.setMSecsSinceEpoch(deltat);
        emit sig_afficherTemps(dt, _no, 1);
    } // if cp1
    if (c == _cp2) {
        _bdd.sauverTemps(deltat, _no, 2);
        _finCoureur2 = true;
        delete _cp2;
        _cp2=nullptr;
        dt.setMSecsSinceEpoch(deltat);
        emit sig_afficherTemps(dt, _no, 2);
    } // if cp2
    if (_finCoureur1 && _finCoureur2) { // course terminée
        _courseEnCours=false;
        emit sig_courseFinished();
    } // if course finie
} // method

void CApp::startCoureurs()
{
    // mesure du temps
    _dt0 = QDateTime::currentDateTimeUtc();
    qDebug() << "Départ : " << _dt0.toString("hh:mm:ss");
    // mesure du vent
    float force = _vent.lireVent();
    // sauver le vent dans BDD
    _bdd.sauverVent(force, _no, 1);  // coureur 1
    _bdd.sauverVent(force, _no, 2);  // coureur 2
    // instancier les détecteurs d'arrivée
    _cp1 = new CCapteurPassage(GPIO_CP1);
    _cp2 = new CCapteurPassage(GPIO_CP2);
    connect(_cp1, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
    connect(_cp2, &CCapteurPassage::sig_coureurArrived, this, &CApp::on_coureurArrived);
} // method

void CApp::on_aVosMarques()
{
    _finCoureur2 = _finCoureur1 = false; // flag pas arrivés
    _feu.sequenceAVosMarques(3); // durée
} // method

void CApp::on_pret()
{
    _feu.sequencePret(3);
} // method
void CApp::on_partez()
{
    _feu.sequencePartez(0); // retour immédiat de la méthode
    startCoureurs();
} // method

void CApp::on_startCourse(int no)
{
    if (_courseEnCours==true) {
        QMessageBox msgBox;
        msgBox.setText("Attention, vous ne pouvez pas lancer une autres course.");
        msgBox.setInformativeText("Erreur !");
        msgBox.setStandardButtons(QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        msgBox.exec();
        return;
    } // if
    _no = no;  // mémorisation du numéro de la ligne de course
    _courseEnCours=true;
    startCoureurs();  // 2 coureurs
} // method

void CApp::on_avantCourse()
{
    _feu.sequenceAvantCourse(0); // indéfini
} // method

void CApp::on_stopCourse()
{
    if (_cp1 != nullptr) delete _cp1;
    if (_cp2 != nullptr) delete _cp2;
    _cp1 = _cp2 = nullptr;
    _courseEnCours=false;
} // method
