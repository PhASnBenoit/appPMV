#ifndef CGIROANEMOMETRE_H
#define CGIROANEMOMETRE_H

#include <QObject>
#include <QString>
#include <QThread>
#include "biblis/ci2c.h"

#define NO_ESCLAVE_GIROANEMO 0x34

class CGiroAnemometre : public QObject
{
    Q_OBJECT
public:
    explicit CGiroAnemometre(QObject *parent = nullptr, int adrI2c = 0x34);
    ~CGiroAnemometre();

    /* Lire la Vitesse du Vent */
    float lireVent();
    float lireDirectionVent();

private:
    CI2c *_i2c;
    int _adrI2c;

signals:
    void sig_VentDir(float vent, int dir);


public slots:
    void on_goTravail();

};

#endif // CGIROANEMOMETRE_H
