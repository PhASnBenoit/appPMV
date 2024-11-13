#include "crs232c.h"

CRs232c::CRs232c(QObject *parent, const QString &nomPort)
{
    m_parent = parent;
    m_sp = new QSerialPort(parent);
    connect(m_sp, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
    connect(m_sp, SIGNAL(error(QSerialPort::SerialPortError)), this, SLOT(onErreur(QSerialPort::SerialPortError)));
    qDebug() << "L'objet CRs232c est créé par " << m_parent->thread();
    m_sp->setPortName(nomPort);
    qDebug() << nomPort;
}

CRs232c::~CRs232c()
{
    m_sp->close();
    delete m_sp;
    qDebug() << "L'objet CRs232c est détruit par " << m_parent->thread();
}

int CRs232c::initialiser(QSerialPort::BaudRate vitesse, QSerialPort::DataBits data,
                         QSerialPort::Parity parity, QSerialPort::StopBits nbStop,
                         QSerialPort::FlowControl flow)
{
    m_sp->setBaudRate(vitesse);
    m_sp->setDataBits(data);
    m_sp->setParity(parity);
    m_sp->setStopBits(nbStop);
    m_sp->setFlowControl(flow);
    return 0;
}

int CRs232c::ouvrirPort()
{
    bool res=false;
    res=m_sp->open(QIODevice::ReadWrite);
    if (!res) {
//        emit sigErreur(m_sp->error());
        m_sp->close();
    } // if res
    return res;
}

char CRs232c::ecrire(const char *trame, int nbOctets)
{
    int lg = m_sp->write(trame, nbOctets);
    if ( lg < nbOctets) {
        emit sigErreur(QSerialPort::WriteError);
        return ERREUR;
    } // if erreur
    return lg;
}

void CRs232c::onReadyRead()
{
    QByteArray ba;
    ba = m_sp->readAll();
    emit sigData(ba);
}

void CRs232c::onErreur(QSerialPort::SerialPortError err)
{
    emit sigErreur(err);
}
