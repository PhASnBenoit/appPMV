#ifndef CGERERCLIENT_H
#define CGERERCLIENT_H

#include <QObject>
#include <QLineEdit>
#include <QTcpSocket>
#include <QHostAddress>
#include <unistd.h>

#include "cformatjson.h"
#include "cbdd.h"
#include "czdc.h"

class CGererClient : public QObject
{
    Q_OBJECT
public:
    explicit CGererClient(QTcpSocket *sock = nullptr, CZdc *zdc = nullptr, CBdd *bdd = nullptr, QObject *parent = nullptr);
    ~CGererClient();

public slots:
    void on_readyRead();
    void on_clientGetControl();
    void on_remoteNewBtnToTablette(T_BUTTONS buttons);

private:
    CFormatJson _prot;
    QTcpSocket *_sock;
    QString _sessionName;
    int emettreVersClient(QString mess);
    CZdc *_zdc;
    CBdd *_bdd;

signals:
    void sig_affClient(QString adrIpClient, QString data);
    void sig_remoteGetControl();
    void sig_newBtnState(T_BUTTONS state);
};

#endif // CGERERCLIENT_H

