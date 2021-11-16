#ifndef CCAPTEURPASSAGE_H
#define CCAPTEURPASSAGE_H

#include <QObject>
#include <QFileSystemWatcher>
#include <QDateTime>
#include "../biblis/cgpio.h"

#define GPIO_CP1 1 // A DEFINIR !!!
#define GPIO_CP2 2 // A DEFINIR !!!

class CCapteurPassage : public QObject
{
    Q_OBJECT
public:
    explicit CCapteurPassage(int gpio = 4, QObject *parent = nullptr);

private:
    CGpio *_in;
    QFileSystemWatcher _file;

signals:
    void sig_coureurArrived(QDateTime dt);

public slots:
    void on_fileChanged(QString path);
};

#endif // CCAPTEURPASSAGE_H
