#include "czdc.h"

CZdc::CZdc(QObject *parent) : QSharedMemory(parent)
{
    setKey("PMV");
    int res = create(sizeof(T_DATAS));
    if (!res) {
        attach();
        _datas = static_cast<T_DATAS *>(data());
    } else {
        _datas = static_cast<T_DATAS *>(data());
        clearDatas(); // 1 seule fois au démarrage
    } // else
}

CZdc::~CZdc()
{
    detach();
}

void CZdc::getParamSession(T_PARAMSESSION &paramSession) //----- 2024
{
    lock();
        memcpy(&paramSession, &_datas->paramSession, sizeof(T_PARAMSESSION));
    unlock();
}

void CZdc::setParamSession(T_PARAMSESSION &paramSession) //----- 2024
{
    lock();
        memcpy(&_datas->paramSession, &paramSession, sizeof(T_PARAMSESSION));
    unlock();
}

bool CZdc::getControleLocal()
{
    bool state;
    lock();
        state = _datas->controleLocal;
    unlock();
    return state;
}

void CZdc::setControleLocal(bool etat)
{
    lock();
        _datas->controleLocal = etat;
    unlock();
}

void CZdc::setNomCoureur(int noCourse, int pos, QString nom)
{
    lock();
        memcpy(_datas->coureurs[noCourse-1].noms[pos], nom.toStdString().c_str(), 50);
    unlock();
}

QString CZdc::getNomCoureur(int noCourse, int pos)
{
    lock();
        return QString(_datas->coureurs[noCourse].noms[pos]);
    unlock();
}

void CZdc::setListeCoureurs(QList<T_2COUREURS> courses) //--- 2024
{
    lock();
        for(int i=0 ; i<20 ; i++) {
            memcpy(&_datas->coureurs[i], &courses.at(i), sizeof(T_2COUREURS));
        } // for
    unlock();
}

void CZdc::getListeCoureurs(QStringList &coureurs) //--- 2024
{
    coureurs.clear();
    lock();
        for(int i=0 ; i<20 ; i++) {
            coureurs.append(_datas->coureurs[i].noms[0]);
            coureurs.append(_datas->coureurs[i].noms[1]);
        } // for
        unlock();
}

void CZdc::setCoureurArrived(int noCourse, int pos, bool etat) //-- 2024
{
    lock();
        _datas->coureurs[noCourse-1].arrived[pos] = etat;
    unlock();
}

bool CZdc::getCoureurArrived(int noCourse, int pos) //-- 2024
{
    bool etat;
    lock();
        etat = _datas->coureurs[noCourse-1].arrived[pos];
    unlock();
    return etat;
}

void CZdc::getCourse(int noCourse, T_2COUREURS &coureurs)
{
    lock();
        memcpy(&coureurs, &_datas->coureurs[noCourse-1], sizeof(T_2COUREURS));
    unlock();
}

void CZdc::setVitesseVent(int ordre, float vitesse, bool init)
{
    lock();
    if (!init) {
        if (vitesse > _datas->coureurs[ordre-1].vent)
            _datas->coureurs[ordre-1].vent = vitesse;
    } else
        _datas->coureurs[ordre-1].vent = vitesse;
    unlock();
}

float CZdc::getVitesseVent(int ordre)
{
    lock();
        int vent = _datas->coureurs[ordre-1].vent;
    unlock();
    return vent;
}

void CZdc::setDirectionVent(int ordre, int dir)
{
    lock();
        _datas->coureurs[ordre-1].dirVent = dir;
    unlock();
}

int CZdc::getDirectionVent(int ordre)
{
    lock();
        int dir = _datas->coureurs[ordre-1].dirVent;
    unlock();
    return dir;
}

void CZdc::sauveDatas(T_DATAS &datas) //-- 2024
{
    lock();
    memcpy(_datas, &datas, sizeof(T_DATAS));
    unlock();
    emit sig_newDatas();
}

void CZdc::clearDatas() //-- 2024
{
  lock();
    memset(_datas, 0, sizeof(T_DATAS));
  unlock();
} // sauveMesures

void CZdc::getDatas(T_DATAS &datas) //-- 2024
{
    lock();
    memcpy(&datas, _datas, sizeof(T_DATAS));
    unlock();
}

/* Mise à jour des Boutons */
void CZdc::sauveButtons(T_BUTTONS &buttons)
{
    lock();
        _datas->buttons = buttons;
    unlock();
    //emit sig_newBtnState(buttons);
}

/* Initialisation de boutons */
void CZdc::getButtons(T_BUTTONS &buttons)
{
    lock();
        buttons = _datas->buttons;
    unlock();
}

void CZdc::setButtons(T_BUTTONS buttons)
{
    lock();
        memcpy(&_datas->buttons, &buttons, sizeof(T_BUTTONS));
    unlock();
}

void CZdc::setTempsCoureur(int noCourse, int noCoureur, quint64 temps) { //--- 2024
    lock();
        _datas->coureurs[noCourse-1].temps[noCoureur] = temps;
    unlock();
}

quint64 CZdc::getTemps(int ordre) { //--
    lock();
        quint64 tmps = _datas->coureurs[ordre-1].temps[(ordre%2?1:0)];
    unlock();
    return tmps;
}


/* Obtenir la vitesse du coureur */
void CZdc::setVitesseCoureur(int noCourse, int noCoureur, float vit) //-- 2024
{
    lock();
        _datas->coureurs[noCourse-1].vitesse[noCoureur] = vit;
    unlock();
}

float CZdc::getVitesseCoureur(int ordre) //-- 2024
{
    lock();
        float vitesse = _datas->coureurs[ordre-1].vitesse[(ordre%2?1:0)];
    unlock();
    return vitesse;
}

// init false sauve max, true reset
void CZdc::setVitesseVentRT(float vitesse) //-- 2024
{
    lock();
        _datas->ventRT= vitesse;
    unlock();
}

float CZdc::getVitesseVentRT() //-- 2024
{
    lock();
        float vent = _datas->ventRT;
    unlock();
    return vent;
}

void CZdc::setDirectionVentRT(int dir)
{
    lock();
    _datas->dirVentRT = dir;
    unlock();

}

int CZdc::getDirectionVentRT()
{
    lock();
        int dir = _datas->dirVentRT;
    unlock();
    return dir;
}

T_TYPE_COURSE CZdc::getTypeCourse()
{
     lock();
         T_TYPE_COURSE mc = _datas->paramSession.typeCourse;
     unlock();
     return mc;
}

void CZdc::setTypeCourse(T_TYPE_COURSE tc)
{
    lock();
        _datas->paramSession.typeCourse = tc;
        unlock();
}

void CZdc::setCourses(QList<T_2COUREURS> courses)
{
    lock();
    for(int i=0 ; i<20 ; i++) {
        memcpy(&_datas->coureurs[i], &courses.at(i), sizeof(T_2COUREURS));
    }
    unlock();
}
