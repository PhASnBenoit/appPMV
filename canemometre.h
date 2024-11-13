#ifndef CANEMOMETRE_H
#define CANEMOMETRE_H

#include <QObject>
#include <QString>
#include <QThread>
#include "biblis/ci2c.h"

#define NO_ESCLAVE_ANEMOMETRE 0x34

class CAnemometre : public QObject
{
    Q_OBJECT
public:
    explicit CAnemometre(QObject *parent = nullptr, int adrI2c = 0x34);
    ~CAnemometre();

    /* Lire la Vitesse du Vent */
    float lireVent();


private:
    CI2c *_i2c;
    int _adrI2c;

signals:
    void sig_vent(float vent);


public slots:
    void on_goTravail();

};

#endif // CANEMOMETRE_H
