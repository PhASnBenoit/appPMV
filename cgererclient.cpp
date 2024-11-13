#include "cgererclient.h"

CGererClient::CGererClient(QTcpSocket *sock, CZdc *zdc, CBdd *bdd, QObject *parent) :
    QObject(parent), _sock(sock), _zdc(zdc), _bdd(bdd)
{
    connect(_sock, &QTcpSocket::readyRead, this, &CGererClient::on_readyRead);
}

CGererClient::~CGererClient(){

}

void CGererClient::on_readyRead() //--- 2024
{
    QList<T_2COUREURS> nomCoureurs;
    QByteArray ba;
    QString trame, command;
    QTcpSocket *client = static_cast<QTcpSocket*>(sender());

    ba = client->readAll();
    qDebug() << "CGererClient::on_readyRead (brut) message de tablette : " << ba;

    command = _prot.parseJsonCommande(ba);
    if (command.isEmpty())
        return;

    if(command == "authCheck") {  // 2 trames à envoyer
        QString codeAuthentification;
        if(_prot.parseJsonAuthCheck(QString (ba), codeAuthentification)){ // récupère code d'authentification
            trame = _prot.prepareJsonAuthCheck(_bdd->verifCodePIN(codeAuthentification), _bdd); // contrôle code et forme json réponse
            client->write(trame.toStdString().c_str());
qDebug() << "CGererClient::on_readyRead (authCheck) Reponse vers tablette : " << trame;
            client->write("\r\n");
            client->flush(); // force l'émission de la trame
            emit sig_affClient(client->peerAddress().toString(), trame);

            usleep(500000);  // 0,5 seconde avant l'envoi de la trame suivante

            T_PARAMSESSION paramSession;
            _zdc->getParamSession(paramSession);
            _sessionName = QString(paramSession.nom);
            T_TYPE_COURSE typeCourse = _zdc->getTypeCourse();
            _bdd->getCourses(nomCoureurs);
            // former la trame d'envoi
            trame = _prot.prepareJsonTransfertAllRunners(_sessionName, typeCourse, nomCoureurs);
            // envoyer la trame
qDebug() << "CGererClient::on_readyRead (TransfertAllRunners) Reponse vers tablette :" << trame;
            client->write(trame.toStdString().c_str());
            client->write("\r\n");
            client->flush();
            emit sig_affClient(client->peerAddress().toString(), trame);
        } //if
    } // if

    if(command == "getCsv"){  // Pourquoi cette demande de la tablette ?
        if(_prot.parseJsonGetCsv(QString(ba))){
            T_CSV csv[40] = {
                {1, "coureur1", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {2, "coureur2", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {3, "coureur3", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {4, "coureur4", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {5, "coureur5", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {6, "coureur6", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {7, "coureur7", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {8, "coureur8", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {9, "coureur9", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {10, "coureur10", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {11, "coureur11", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {12, "coureur12", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {13, "coureur13", "00:03:01.230", "5.6 km/h", "4.9 km/h"},
                {14, "coureur14", "00:03:01.230", "5.6 km/h", "4.9 km/h"}
            };
            trame = _prot.prepareJsonTransfertCsv(14, csv);
            client->write(trame.toStdString().c_str());
            client->write("\r\n");
            client->flush();
        } //if
    } //if

    if(command == "getControl"){
        emit sig_affClient(client->peerAddress().toString(),"Reprise de contrôle par la tablette.");
        emit sig_remoteGetControl(); // obj : La tablette pilote la session
    }

    if(command == "btnState") {
        T_BUTTONS state;
        if(_prot.parseJsonBtnState(QString(ba), state)){
            emit sig_newBtnState(state);
        } //if
    } //if

    //emit sig_affClient(client->peerAddress().toString(), QString(ba));
}

void CGererClient::on_clientGetControl()
{ // appli vers tablette
    QString trame;
    trame = _prot.prepareJsonGetControl();
    emettreVersClient(trame);
}

void CGererClient::on_remoteNewBtnToTablette(T_BUTTONS buttons)
{ // appli vers tablette
  QString trame;
  trame = _prot.prepareJsonBtnState(buttons);
  qDebug() << "CGererClient::on_remoteNewBtn : " << trame;
  emettreVersClient(trame);
}

int CGererClient::emettreVersClient(QString mess){
    _sock->write(mess.toStdString().c_str());
    _sock->write("\r\n");
    _sock->flush();
    qDebug() << "Envois vers " << _sock->peerAddress().toString() << "de : " << mess;
    return 1;
}
