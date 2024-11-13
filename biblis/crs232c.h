#ifndef CRS232C_H
#define CRS232C_H

#include <QSerialPort>
#include <QTimer>
#include <QDebug>
#include <QThread>

#ifndef ERREUR
   #define ERREUR (char)-1
#endif
#define OK (char)0
#define TO 3000  // 3s timeout réception

class CRs232c : public QObject
{
    Q_OBJECT

public:
    explicit CRs232c(QObject *parent = 0, const QString &nomPort = "/dev/ttyUSB0");
    ~CRs232c();
    int initialiser(QSerialPort::BaudRate vitesse, QSerialPort::DataBits data,
                    QSerialPort::Parity parity, QSerialPort::StopBits nbStop,
                    QSerialPort::FlowControl flow);
    int ouvrirPort();
    char ecrire(const char *trame, int nbOctets);

private:
    QSerialPort *m_sp;
    QObject *m_parent;

signals:
    void sigErreur(QSerialPort::SerialPortError err);
    void sigData(QByteArray ba);

private  slots:
    void onReadyRead();
    void onErreur(QSerialPort::SerialPortError err);
};

#endif // CRS232C_H
