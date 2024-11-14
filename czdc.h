#ifndef CZDC_H
#define CZDC_H

#include <QObject>
#include <QSharedMemory>
#include <QStringList>
#include <string.h>

#define NB_COURSES 40

typedef enum {
    DEPART_LANCE=1,  // 1 seul coureur
    DEPART_ARRETE      // 2 coureurs en //
} T_TYPE_COURSE;

typedef enum {
    ETEINT,
    FIXE,
    LENT,
    RAPIDE
} T_MODE_FEU;

typedef struct s_buttons { //--- 2024
    bool btnSession;
    bool btnPreparation;
    bool btnAVM;
    bool btnReady;
    bool btnGo;
    bool btnArret;
} T_BUTTONS;

typedef struct s_paramSession { //----- 2024
    bool active;
    char nom[50];
    char dateDeb[15];
    char dateFin[15];
    char heureDeb[15];
    char heureFin[15];
    T_TYPE_COURSE typeCourse;
} T_PARAMSESSION;

typedef struct s_2coureurs { //--- 2024
    bool arrived[2];  // true si arrivé
    quint64 temps[2];  // temps en ms pour les 2 coureurs d'une course
    float vitesse[2]; // vitesse calculéé
    bool barriere[2];  // true si coureur arrivé
    float vent; // vitesse du vent en km/h pour le coureur au moment de sa course
    int dirVent; // direction du vent pendant la course du coureur
    char noms[2][50];
} T_2COUREURS;

typedef struct s_data { //--- 2024
    T_BUTTONS buttons;
    T_MODE_FEU modeDeFonctionnement;
    T_PARAMSESSION paramSession;
    bool activeSignalisation;
    bool controleLocal;
    char chaine[2][50];
    float ventRT;  // vitesse du vent en km/h en temps réel
    int dirVentRT; // direction du vent en ° en temps reel
    T_2COUREURS coureurs[NB_COURSES];
} T_DATAS;


class CZdc : public QSharedMemory
{
    Q_OBJECT

public:
    CZdc(QObject *parent = nullptr);
    ~CZdc();
    void getParamSession(T_PARAMSESSION &paramSession);  //--- 2024
    void setParamSession(T_PARAMSESSION &paramSession);  //--- 2024

    bool getControleLocal();
    void setControleLocal(bool etat);

    void setNomCoureur(int noCourse, int pos, QString nom);
    QString getNomCoureur(int noCourse, int pos);

    void setListeCoureurs(QList<T_2COUREURS> courses); //--- 2024
    void getListeCoureurs(QStringList &coureurs); //--- 2024

    void setCoureurArrived(int noCourse, int pos, bool etat);
    bool getCoureurArrived(int noCourse, int pos);

    void getCourse(int noCourse, T_2COUREURS &coureurs);
    void sauveDatas(T_DATAS &datas);
    void clearDatas();
    void getDatas(T_DATAS &datas);

    void sauveButtons(T_BUTTONS &buttons);
    void getButtons(T_BUTTONS &buttons);
    void setButtons(T_BUTTONS buttons);

    void setTempsCoureur(int noCourse, int noCoureur, quint64 temps); //--- 2024
    quint64 getTemps(int ordre);

    void setVitesseCoureur(int noCourse, int noCoureur, float vit); //--- 2024
    float getVitesseCoureur(int ordre);

    void setVitesseVent(int ordre, float vitesse, bool init);
    float getVitesseVent(int ordre);

    void setDirectionVent(int ordre, int dir);
    int getDirectionVent(int ordre);


    void setVitesseVentRT(float vitesse);
    float getVitesseVentRT();

    void setDirectionVentRT(int dir);
    int getDirectionVentRT();

    T_TYPE_COURSE getTypeCourse();
    void setTypeCourse(T_TYPE_COURSE tc);
    void setCourses(QList<T_2COUREURS> courses);

signals:
    void sig_newDatas();
    void sig_addrClient();
    void sig_newBtnState(T_BUTTONS buttons);

private:
    T_DATAS *_datas;
};

#endif // CZDC_H
