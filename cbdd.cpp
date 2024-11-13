#include "cbdd.h"

using namespace std;

CBdd::CBdd(QObject *parent): QObject(parent)  // --------- 2024
{
//    _sqlPath = "/home/philippe/devQt/pmv_bdd/PMVBdd.sqlite";
    _sqlPath = "/home/pi/devQt/PMVBdd.sqlite";
    PMVBdd = QSqlDatabase::addDatabase("QSQLITE");
    PMVBdd.setDatabaseName(_sqlPath);
    if(!PMVBdd.open()) {
        qDebug() << "Unable to open database" << Qt::endl;
    } else {
        qDebug() << "Opened database" << Qt::endl;
    } // else
    _sqlQuery = new QSqlQuery(PMVBdd);
}

CBdd::~CBdd()
{
    delete _sqlQuery;
    PMVBdd.close();
}

bool CBdd::isSessionActive()  //------------- 2024
{
    _sqlQuery->prepare("SELECT * FROM Config LIMIT 1");
    _sqlQuery->exec();
    _sqlQuery->first();
    return (_sqlQuery->value(4).toString() == "1" ? true : false);
}

bool CBdd::getParamSession(T_PARAMSESSION &paramSession) //--- 2024
{
    _sqlQuery->prepare("SELECT * FROM Config LIMIT 1;");
    if (_sqlQuery->exec()) {
        if (_sqlQuery->first()) {
            paramSession.active = _sqlQuery->value(4).toBool();
            paramSession.typeCourse = (T_TYPE_COURSE)_sqlQuery->value(9).toInt();
            strcpy(paramSession.nom, _sqlQuery->value(3).toString().toStdString().c_str());
            strcpy(paramSession.dateDeb, _sqlQuery->value(5).toString().toStdString().c_str());
            strcpy(paramSession.dateFin, _sqlQuery->value(7).toString().toStdString().c_str());
            strcpy(paramSession.heureDeb, _sqlQuery->value(6).toString().toStdString().c_str());
            strcpy(paramSession.heureFin, _sqlQuery->value(8).toString().toStdString().c_str());
            return true;
        } // if first
    } // if exec
    return false;
}

bool CBdd::setParamSession(T_PARAMSESSION &paramSession) //------ 2024
{
    _sqlQuery->prepare("UPDATE Config SET ID_Session=1, Nom_Session=:NomSession, Active_Session=1, Date_Debut=:DateDeb, Heure_Debut=:HeureDeb, Type_Course=:TypeCourse LIMIT 1;");
    //_sqlQuery->prepare("UPDATE Config SET Active_Session=1, Nom_Session=:NomSession, Date_Debut=:DateDeb LIMIT 1;");
    _sqlQuery->bindValue(":NomSession", QString(paramSession.nom));
    _sqlQuery->bindValue(":DateDeb", QString(paramSession.dateDeb));
    _sqlQuery->bindValue(":HeureDeb", QString(paramSession.heureDeb));
    _sqlQuery->bindValue(":TypeCourse", paramSession.typeCourse);
    _sqlQuery->exec();
    qDebug() << _sqlQuery->lastError().text();
    return true;
}

void CBdd::setSessionActive(bool active)
{
    _sqlQuery->prepare("UPDATE Config SET Active_Session = :Active_Session;");
    _sqlQuery->bindValue(":Active_Session", active);
    _sqlQuery->exec();
}

void CBdd::setSessionName(QString nomSession) {
    _sqlQuery->prepare("UPDATE Config SET Nom_Session = :Nom_Session;");
    _sqlQuery->bindValue(":Nom_Session", nomSession);
    _sqlQuery->exec();
}

QString CBdd::getSessionName() {  //----------- 2024
    _sqlQuery->prepare("SELECT Nom_Session FROM Config;");
    _sqlQuery->exec();
    _sqlQuery->first();
    return _sqlQuery->value(3).toString();
}

void CBdd::setListeCoureurs(QStringList &liste) { //---- 2024
    // liste contient toujours 40 noms
    clearTableCoureur(); // efface la table coureurs
    qDebug() << "Effacement table Courses et ajout de : " << liste;

    // La liste contient 40 noms
    for(int i=0; i<20 ; i++) {
        _sqlQuery->prepare("INSERT INTO Courses VALUES (:idc, :ids, :Nom1, :Nom2, '0', '0', '0', '0', '0', '0');");
        _sqlQuery->bindValue(":Nom1", liste.at(2*i));
        _sqlQuery->bindValue(":Nom2", liste.at(2*i+1));
        _sqlQuery->bindValue(":idc", i+1);
        _sqlQuery->bindValue(":ids", 1);
        _sqlQuery->exec();
        qDebug() << "CBdd::setListeCoureurs: " << _sqlQuery->lastError().text();
    } // for
}

// Faire les modifications
void CBdd::getCourses(QList<T_2COUREURS> &courses) //---- 2024
{
    T_2COUREURS course;

    _sqlQuery->prepare("SELECT * FROM Courses;");
    _sqlQuery->exec();
    qDebug() << "CBdd::getListeCoureurs: " << _sqlQuery->lastError().text();

    for(int i=0 ; i<20 ; i++){
        if (!_sqlQuery->next()) {
        } else {
            strcpy(course.noms[0], _sqlQuery->value(2).toString().toStdString().c_str());
            strcpy(course.noms[1], _sqlQuery->value(3).toString().toStdString().c_str());
            course.temps[0] = _sqlQuery->value(4).toInt();
            course.temps[1] = _sqlQuery->value(5).toInt();
            course.vitesse[0] = _sqlQuery->value(6).toFloat();
            course.vitesse[1] = _sqlQuery->value(7).toFloat();
            course.vent = _sqlQuery->value(8).toFloat();
            course.dirVent = _sqlQuery->value(9).toInt();
            courses.append(course);
        } // else
    } // for
}

void CBdd::sauveCourse(int noCourse, T_2COUREURS coureurs)
{
    _sqlQuery->prepare("UPDATE Courses SET "
                       "Nom1 = :nom1, "
                       "Nom2 = :nom2, "
                       "Temps1 = :temps1, "
                       "Temps2 = :temps2, "
                       "Vitesse1 = :vitesse1, "
                       "Vitesse2 = :vitesse2, "
                       "Vent = :vent, "
                       "Dir = :dir "
                       "WHERE idCourse = :id;");
    _sqlQuery->bindValue(":nom1", QString(coureurs.noms[0]));
    _sqlQuery->bindValue(":nom2", QString(coureurs.noms[1]));
    _sqlQuery->bindValue(":temps1", coureurs.temps[0]);
    _sqlQuery->bindValue(":temps2", coureurs.temps[1]);
    _sqlQuery->bindValue(":vitesse1", coureurs.vitesse[0]);
    _sqlQuery->bindValue(":vitesse2", coureurs.vitesse[1]);
    _sqlQuery->bindValue(":vent", coureurs.vent);
    _sqlQuery->bindValue(":dir",coureurs.dirVent);
    _sqlQuery->bindValue(":id", noCourse);
    _sqlQuery->exec();
}

QList<QString> CBdd::extractDatasExport()
{
    // Déclaration de la variable tampon
    QList<QString> values;

    //getListeCoureurs();
    _sqlQuery->prepare("SELECT * FROM Config;");
    _sqlQuery->exec();

    /* Récupération des résultats contenus dans les colonnes et les lignes */

    /* Colonne Gauche */
    _sqlQuery->last();
    QString CGauche = _sqlQuery->value(0).toString();
    qDebug() << " Colonne Gauche: " << CGauche;

    _sqlQuery->first();
    QString Temps_1 = _sqlQuery->value(1).toString();
    QString VMoy_1 = _sqlQuery->value(2).toString();
    QString VMoyVent_1 = _sqlQuery->value(3).toString();

    for(int i = 0; i < CGauche.toInt(); i++){
        values.append(_sqlQuery->value(1).toString() + " " + _sqlQuery->value(2).toString() + " " + _sqlQuery->value(3).toString());
    }// for


    /* Colonne Droite */
    _sqlQuery->last();
    QString CDroite = _sqlQuery->value(4).toString();
    qDebug() << " Colonne Droite: " << CDroite;

    _sqlQuery->first();
    QString Temps_2 = _sqlQuery->value(5).toString();
    QString VMoy_2 = _sqlQuery->value(6).toString();
    QString VMoyVent_2 = _sqlQuery->value(7).toString();

    for(int i = 0; i < CDroite.toInt(); i++){
        values.append(_sqlQuery->value(5).toString() + " " + _sqlQuery->value(6).toString() + " " + _sqlQuery->value(7).toString());
    }// for

    return values;
}

bool CBdd::verifCodePIN(QString code) { //------------- 2024
    _sqlQuery->prepare("SELECT * FROM Config");
    _sqlQuery->exec();
    qDebug() << "CBdd::verifCodePIN: " << _sqlQuery->lastError().text();

    if (_sqlQuery->first()) {
        if(_sqlQuery->value(1).toString() == code){
            qDebug() << "CBdd::verifCodePIN: " << "CODE PIN OK";
            return true;
        } // if
    } // if first
    return false;
} // metho


void CBdd::clearTableCoureur()
{
    _sqlQuery->prepare("DELETE FROM Courses;");
    _sqlQuery->exec();
}
