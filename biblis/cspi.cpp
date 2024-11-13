#include "cspi.h"

CSpi::CSpi(QObject *parent, char noCs, int speed, bool csHigh, int mode) :
    QObject(parent)
{
    m_mode = mode;
    m_csHigh = csHigh;
    m_noCe = noCs;   // chip select
    m_speed = speed;
    qDebug() << "Démarrage de l'objet CSpi";
    init();
} // constructeur

/////////////////////////////////////////////////////////////////
int CSpi::lireNOctets(quint8 *buffer, int n)
{
    int nb = read(m_fileSpi, buffer, n);
    if (nb != n)
        emit sigErreur("CSpi::ecrireNOctets ERREUR lecture");
//    qDebug() << buffer[0] << "  " << buffer[1];
    return nb;
} // lire

/////////////////////////////////////////////////////////////////
int CSpi::ecrireNOctets(quint8 *buffer, int n)
{
    int nb = write(m_fileSpi, buffer, n);
    if (nb !=n)
        emit sigErreur("CSpi::ecrireNOctets ERREUR écriture");
    return nb;
}

int CSpi::lireEcrire(quint8 *em, int nbTotal)
{
    struct spi_ioc_transfer tr[5];

    for(int i=0 ; i<nbTotal ; i++) {
        memset(tr+i, 0, sizeof(tr[i]));
        tr[i].tx_buf = (unsigned long) em+i;
        tr[i].rx_buf = (unsigned long) em+i;
        tr[i].len = 1;
        tr[i].speed_hz = 0;  // par défaut
        tr[i].bits_per_word = 8; // nbre de bit par échange
        tr[i].delay_usecs= 0;  // par défaut
        tr[i].cs_change = 0;  // ne pas changer le CS, reste haut.
    } // for
    int ret = ioctl(m_fileSpi, SPI_IOC_MESSAGE(nbTotal), &tr);
//    qDebug() << "CSpi::lireEcrire retour ioctl : " << ret;
    return ret;
} // lireecrire

/////////////////////////////////////////////////////////////////
int CSpi::init()
{
    char filename[20];

    sprintf(filename, "/dev/spidev0.%c", m_noCe);
    m_fileSpi=open(filename, O_RDWR);
    if(m_fileSpi==-1) {  // ouvre le fichier virtuel d'accès à SPI
        QString mess="CSpi::init Erreur ouverture acces au bus SPI";
        emit sigErreur(mess);
        return -1;
    } // if open
    quint8 mode=(m_csHigh?SPI_CS_HIGH:0)|m_mode;
    if (ioctl(m_fileSpi, SPI_IOC_WR_MODE, &mode) != 0) {
        QString mess="CSpi::init Erreur ouverture acces au bus SPI";
         emit sigErreur(mess);
         return -1;
    } // if
    if (ioctl(m_fileSpi, SPI_IOC_WR_MAX_SPEED_HZ, & m_speed) != 0) {
        QString mess="CSpi::init Erreur ouverture acces au bus SPI";
         emit sigErreur(mess);
         return -1;
    } // if
    return m_fileSpi;
} // init
