#ifndef CGIROUETTE_H
#define CGIROUETTE_H

#include <QObject>
#include <QString>
#include <QThread>
#include "biblis/ci2c.h"

#define NO_ESCLAVE_ANEMOMETRE 0x34

class CGirouette : public QObject
{
    Q_OBJECT
public:
    explicit CGirouette(QObject *parent = nullptr, int adrI2c = 0x34);
    ~CGirouette();

    /* Lire la direction du Vent */
    float lireDirectionVent();

private:
    CI2c *_i2c;
    int _adrI2c;
    bool _continu;

signals:
    void sig_dir(int dir);

public slots:
        void on_goTravail();
};

#endif // CGIROUETTE_H
