#include "ccapteurpassage.h"

CCapteurPassage::CCapteurPassage(int gpio, QObject *parent) : QObject(parent)
{
    //_in = new CGpio(this, gpio, IN);
//    QString path("/sys/class/gpio/gpio"+QString::number(gpio)+"/value");
    QString path("essai"+QString::number(gpio));
    _file.addPath(path);
    connect(&_file, &QFileSystemWatcher::fileChanged, this, &CCapteurPassage::on_fileChanged);
} // method

void CCapteurPassage::on_fileChanged(QString path)
{
    // slot qui se produit lorque le fichier surveillé change de contenu.
    // transmettre le temps
    QDateTime dt = QDateTime::currentDateTimeUtc();
    path.clear(); // pour éviter le warning
    emit sig_coureurArrived(dt);
} // method
