#ifndef CGPIO_H
#define CGPIO_H

#include <QObject>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <unistd.h>

#define IN  0
#define OUT 1
#define SWITCH_ON 1
#define SWITCH_OFF 0
#define BOTH 2

class CGpio : public QObject
{
    Q_OBJECT

public :
    explicit CGpio(QObject *parent = 0, int addr = 4, int dir = OUT);
    ~CGpio();
    int lire();
    int ecrire(int value);

private :
    char m_filename[50];
    int m_addr;
    QObject *m_parent;
    QString m_nomFic;
    int init();
    int gpioExport();
    int gpioUnexport();
    int gpioEdge(int edge);
    int gpioDirection(int dir);
    int gpioRead();
    int gpioWrite(int value);

signals:
    void sigErreur(QString msg);
};

#endif // CGPIO_H
