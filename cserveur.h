#ifndef CSERVEUR_H
#define CSERVEUR_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QList>

#include "cgererclient.h"
#include "czdc.h"
#include "cbdd.h"
#include "cformatjson.h"

#define PORT 5555

class CServeur : public QObject
{
    Q_OBJECT
public:
    CServeur(CBdd *bdd, QObject *parent = nullptr);
    ~CServeur();

public slots:
    void on_disconnectedClient();
    void on_srvGetControl();
    void on_remoteGetControl();
    void on_remoteNewBtnToTablette(T_BUTTONS buttons);

private:
    QTcpServer _serv;
    QTcpSocket *_sock;
    CGererClient *_client;
    quint16 _noPort;
    QString cgererclient;
    CBdd *_bdd;
    CZdc *_zdc;

signals:
    void sig_evenementServeur(QString eve);
    void sig_erreur(QAbstractSocket::SocketError err);
    void sig_adrClient(QString adrClient);
    void sig_maJClients(QList<QTcpSocket *> liste);
    void sig_clientGetControl();
    void sig_srvRemoteGetControl();
    void sig_newBtnStateFromTablette(T_BUTTONS state);  //-- 2024 tablette vers appli
    void sig_remoteNewBtnToTablette(T_BUTTONS buttons);  //-- 2024 appli vers tablette

private slots:
    void on_newConnection();
    void on_newBtnState(T_BUTTONS state);

};

#endif // CSERVEUR_H
