#include "cgirouette.h"

CGirouette::CGirouette(QObject *parent, int adrI2c)
    : QObject{parent}, _adrI2c(adrI2c)
{
    unsigned char datas[2];

    _i2c = CI2c::getInstance();
    datas[0] = 0xAA; // accès reg SETUP Vref ext, Clk ext, unipol, no RST
    _i2c->ecrire(_adrI2c, datas, 1);
    usleep(500000); // 500 ms
    datas[0] = 0x63; // accès CONFIG scan ANA1
    _i2c->ecrire(_adrI2c, datas, 1);
    usleep(500000); // 500 ms
}

CGirouette::~CGirouette()
{
    _i2c->freeInstance();
}

float CGirouette::lireDirectionVent()
{
    unsigned char datas[10];
    uint16_t resultat_ADC_AN1;
    float tension_AN1;
    float  directionVentDegre= 0;

    _i2c->lire(_adrI2c, datas, 2);

    resultat_ADC_AN1 = ((datas[0]& 0x03)*256 + datas[1]);
    tension_AN1 = resultat_ADC_AN1*3.0/1023;
    directionVentDegre = tension_AN1*360.0/2.5;
    return directionVentDegre;
}

void CGirouette::on_goTravail()
{
    while (!QThread::currentThread()->isInterruptionRequested()) {
       int dir = lireDirectionVent();
       emit sig_dir(dir);
       sleep(1);
    }// wh
}
