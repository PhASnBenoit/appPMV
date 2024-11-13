#include "cgiroanemometre.h"

CGiroAnemometre::CGiroAnemometre(QObject *parent, int adrI2c) : QObject(parent),
    _adrI2c(adrI2c)
{
    unsigned char trame[2];

    _i2c = CI2c::getInstance();
    trame[0] = 0xAA; // accès reg SETUP Vref ext, Clk ext, unipol, no RST
    _i2c->ecrire(_adrI2c, trame, 1);
    usleep(500000);
}

CGiroAnemometre::~CGiroAnemometre()
{
    _i2c->freeInstance();
}

float CGiroAnemometre::lireVent() //-- 2024
{
    unsigned char trame[4];
    uint16_t resultat_ADC_AN0;
    float tension_AN0;
    float vitesseKMH = 0;

    trame[0] = 0x61; // accès CONFIG scan ANA0
    _i2c->ecrire(_adrI2c, trame, 1);
    usleep(500000);
    _i2c->lire(_adrI2c, trame, 2);
    usleep(500000);

    resultat_ADC_AN0 = ((trame[0]& 0x03)*256 + trame[1]);
    tension_AN0 = resultat_ADC_AN0*3.0/1023;
    vitesseKMH = (tension_AN0*24) - 30.6;
    if (vitesseKMH<0) vitesseKMH=0.0;
    return vitesseKMH;
}

float CGiroAnemometre::lireDirectionVent() //-- 2024
{
    unsigned char datas[10];
    uint16_t resultat_ADC_AN1;
    float tension_AN1;
    float  directionVentDegre= 0;

    datas[0] = 0x63; // accès CONFIG scan ANA1
    _i2c->ecrire(_adrI2c, datas, 1);
    usleep(500000);
    _i2c->lire(_adrI2c, datas, 2);
    usleep(500000);

    resultat_ADC_AN1 = ((datas[0]& 0x03)*256 + datas[1]);
    tension_AN1 = resultat_ADC_AN1*3.0/1023;
    directionVentDegre = tension_AN1*360.0/2.5;
    return directionVentDegre;
}

void CGiroAnemometre::on_goTravail() //-- 2024
{
    float vit;
    int dirVent;
    while (!QThread::currentThread()->isInterruptionRequested()) {
        vit = lireVent();
        dirVent = lireDirectionVent();
        emit sig_VentDir(vit, dirVent);
    }// wh
}
