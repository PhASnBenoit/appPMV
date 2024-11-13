#include "canemometre.h"

CAnemometre::CAnemometre(QObject *parent, int adrI2c) : QObject(parent),
    _adrI2c(adrI2c)
{
    unsigned char trame[2];

    _i2c = CI2c::getInstance();
    trame[0] = 0xAA; // accès reg SETUP Vref ext, Clk ext, unipol, no RST
    _i2c->ecrire(_adrI2c, trame, 1);
    usleep(500000); // 500 ms
    trame[0] = 0x61; // accès CONFIG scan ANA0
    _i2c->ecrire(_adrI2c, trame, 1);
    usleep(500000); // 500 ms
}

CAnemometre::~CAnemometre()
{
    _i2c->freeInstance();
}

float CAnemometre::lireVent()
{
    unsigned char trame[4];
    uint16_t resultat_ADC_AN0;
    float tension_AN0;
    float vitesseKMH = 0;

    _i2c->lire(_adrI2c, trame, 2);

    resultat_ADC_AN0 = ((trame[0]& 0x03)*256 + trame[1]);
    tension_AN0 = resultat_ADC_AN0*3.0/1023;
    vitesseKMH = (tension_AN0*24) - 30.6;
    if (vitesseKMH<0) vitesseKMH=0.0;
    return vitesseKMH;
}

void CAnemometre::on_goTravail()
{
    float vit;
    while (!QThread::currentThread()->isInterruptionRequested()) {
        vit = lireVent();
        emit sig_vent(vit);
        sleep(1);
    }// wh
}
