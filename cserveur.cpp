#include "cserveur.h"

CServeur::CServeur(CBdd *bdd, QObject *parent) : QObject(parent), _bdd(bdd) {
    _zdc = new CZdc();
    _serv.listen(QHostAddress::Any, PORT);
    connect(&_serv, &QTcpServer::newConnection, this, &CServeur::on_newConnection);
    _noPort = PORT;
    _sock = nullptr;
}

CServeur::~CServeur()
{
    if (_sock) delete _client;
    delete _zdc;
}

void CServeur::on_newConnection(){
    QString mess = "Un client vient de se connecter";
    qDebug() << mess;
    _sock = _serv.nextPendingConnection();
    qDebug() << "Nouvelle connexion : " << _sock;
    _client = new CGererClient(_sock, _zdc, _bdd);

    connect(_sock, &QTcpSocket::readyRead, _client, &CGererClient::on_readyRead);
//    connect(_sock, &QAbstractSocket::disconnected, this, &CServeur::deleteLater);
    connect(_sock, &QAbstractSocket::disconnected, this, &CServeur::on_disconnectedClient);
    connect(_client, &CGererClient::sig_remoteGetControl, this, &CServeur::on_remoteGetControl);
    connect(_client, &CGererClient::sig_newBtnState, this, &CServeur::on_newBtnState);
    connect(this, &CServeur::sig_remoteNewBtnToTablette, _client, &CGererClient::on_remoteNewBtnToTablette);
    connect(_zdc, &CZdc::sig_newBtnState, this, &CServeur::on_newBtnState);
    connect(this, &CServeur::sig_clientGetControl, _client, &CGererClient::on_clientGetControl);
    connect(_sock, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::errorOccurred),
              [=](QAbstractSocket::SocketError err)
        {
        QString txt;
        switch(err) {
        case QAbstractSocket::ConnectionRefusedError:
            txt="The connection was refused by the peer (or timed out).";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            txt="The remote host closed the connection. Note that the client socket (i.e., this socket) will be closed after the remote close notification has been sent.";
            break;
        case QAbstractSocket::HostNotFoundError:
            txt="The host address was not found.";
            break;
        case QAbstractSocket::SocketAccessError:
            txt="The socket operation failed because the application lacked the required privileges.";
            break;
        case QAbstractSocket::SocketResourceError:
            txt="The local system ran out of resources (e.g., too many sockets).";
            break;
        case QAbstractSocket::SocketTimeoutError:
            txt="The socket operation timed out.";
            break;
        case QAbstractSocket::DatagramTooLargeError:
            txt="The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).";
            break;
        case QAbstractSocket::NetworkError:
            txt="An error occurred with the network (e.g., the network cable was accidentally plugged out).";
            break;
        case QAbstractSocket::AddressInUseError:
            txt="The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.";
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            txt="The address specified to QAbstractSocket::bind() does not belong to the host.";
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            txt="The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            txt="The socket is using a proxy, and the proxy requires authentication.";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            txt="The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)";
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            txt="Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).";
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            txt="Could not contact the proxy server because the connection to that server was denied";
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            txt="The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            txt="The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
            break;
        case QAbstractSocket::ProxyNotFoundError:
            txt="The proxy address set with setProxy() (or the application proxy) was not found.";
            break;
        case QAbstractSocket::ProxyProtocolError:
            txt="The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.";
            break;
        case QAbstractSocket::OperationError:
            txt="An operation was attempted while the socket was in a state that did not permit it.";
            break;
        case QAbstractSocket::SslInternalError:
            txt="The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.";
            break;
        case QAbstractSocket::SslInvalidUserDataError:
            txt="Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.";
            break;
        case QAbstractSocket::TemporaryError:
            txt="A temporary error occurred (e.g., operation would block and socket is non-blocking).";
            break;
        case QAbstractSocket::UnknownSocketError:
            txt="An unidentified error occurred.";
            break;
        } // sw
        qDebug() << txt;
    });
}

void CServeur::on_newBtnState(T_BUTTONS state) //--- 2024
{
    // relais du signal du client vers le serveur
    emit sig_newBtnStateFromTablette(state);
}


void CServeur::on_disconnectedClient()
{
    _sock->close();
    _sock = nullptr;
    delete _client;
    emit sig_evenementServeur("Déconnexion du client");
}

void CServeur::on_srvGetControl()
{

    emit sig_clientGetControl();
}

void CServeur::on_remoteGetControl()
{
    emit sig_srvRemoteGetControl();
}

void CServeur::on_remoteNewBtnToTablette(T_BUTTONS buttons)
{
  // emettre vers le client le signal de mise à jour des boutons
    emit sig_remoteNewBtnToTablette(buttons);
}

