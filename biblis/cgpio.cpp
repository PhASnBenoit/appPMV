#include "cgpio.h"

CGpio::CGpio(QObject *parent, int addr, int dir)
{
    m_parent=parent;
    m_addr = addr;
    gpioExport();
    init();
    gpioDirection(dir);
    gpioEdge(BOTH);
} // constructeur

CGpio::~CGpio()
{
  gpioUnexport();
} // destructeur

int CGpio::init()
{
    usleep(50000);
    sprintf(m_filename,"/sys/class/gpio/gpio%d/",m_addr);
    QString command = "sudo"+QString(m_filename);
    QStringList args;
    args.append("chmod");
    args.append("-R");
    args.append("777");
    QProcess *proc= new QProcess(this);
    connect(proc, SIGNAL(finished(int)), proc, SLOT(deleteLater()));
    proc->start(command, args);
    return 1;
}

int CGpio::gpioUnexport()
{
    char buffer[3];

    QFile fUnexport("/sys/class/gpio/unexport");
    bool res = fUnexport.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioUnexport: Erreur d'ouverture du fichier !";
        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open
    sprintf(buffer,"%d", m_addr);
    qint64 nbw = fUnexport.write(buffer, strlen(buffer));
    if (nbw != int(strlen(buffer))) {
        QString mess="CGpio::gpioUnexport: Erreur écriture dans fichier !";
        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    fUnexport.close();
    return 0;
}

int CGpio::gpioDirection(int dir)
{
    char buffer[5];
    QString ficDirection;

    ficDirection = QString("/sys/class/gpio/gpio%1/direction").arg(m_addr,0,10);
    QFile fDirection(ficDirection);
    bool res = fDirection.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioDirection: Erreur d'ouverture du fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open
    if (dir==IN)
      strcpy(buffer,"in");
    else
      strcpy(buffer,"out");
    qint64 nbw = fDirection.write(buffer, strlen(buffer));
    if (nbw != int(strlen(buffer))) {
        QString mess="CGpio::gpioDirection: Erreur écriture dans fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    fDirection.close();
    return 0;
}

int CGpio::gpioEdge(int edge)
{
    char buffer[5];
    QString ficEdge;

    ficEdge = QString("/sys/class/gpio/gpio%1/edge").arg(m_addr,0,10);
    QFile fEdge(ficEdge);
    bool res = fEdge.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioEdge: Erreur d'ouverture du fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open
    if (edge==BOTH)
      strcpy(buffer,"both");
    else
      strcpy(buffer,"none");
    qint64 nbw = fEdge.write(buffer, strlen(buffer));
    if (nbw != int(strlen(buffer))) {
        QString mess="CGpio::gpioEdge: Erreur écriture dans fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    fEdge.close();
    return 0;
}



int CGpio::gpioExport()
{
    char buffer[3];

    QFile fExport("/sys/class/gpio/export");
    bool res = fExport.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioExport: Erreur d'ouverture du fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open
    sprintf(buffer,"%d", m_addr);
    qint64 nbw = fExport.write(buffer, strlen(buffer));
    if (nbw != int(strlen(buffer))) {
        QString mess="CGpio::gpioExport: Erreur écriture dans fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    fExport.close();
    return 0;
}
	

int CGpio::lire()
{
    char buffer[3];
    QString ficValue;

    ficValue = QString("/sys/class/gpio/gpio%1/value").arg(m_addr,0,10);
    QFile fValue(ficValue);
    bool res = fValue.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioLire: Erreur d'ouverture du fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open

    qint64 nbr = fValue.read(buffer, sizeof(buffer));
    if (nbr == -1) {
        QString mess="CGpio::gpioLire: Erreur lecture dans fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    buffer[nbr]=0;  // car NULL
    fValue.close();
    return atoi(buffer);
}
	
int CGpio::ecrire(int value)
{
    char buffer[3]={'0', '1'};
    QString ficValue;

    ficValue = QString("/sys/class/gpio/gpio%1/value").arg(m_addr,0,10);
    QFile fValue(ficValue);
    bool res = fValue.open(QIODevice::WriteOnly | QIODevice::Text);
    if (!res) {
        QString mess="CGpio::gpioEcrire: Erreur d'ouverture du fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if erreur open

    qint64 nbw = fValue.write(&buffer[(value==0?0:1)], 1);
    if (nbw == -1) {
        QString mess="CGpio::gpioEcrire: Erreur écriture dans fichier !";
//        qDebug() << mess;
        emit sigErreur(mess);
        return -1;
    } // if nbw
    fValue.close();
    return 0;
}	
