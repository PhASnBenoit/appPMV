#include "cformatjson.h"


CFormatJson::CFormatJson(QObject *parent) : QObject(parent)
{

}

CFormatJson::~CFormatJson()
{

}

QString CFormatJson::prepareJsonTransfertRunner(int idCourse, QList<int> idRunners){
    QString json;
    json = addEnteteJson("transferRunner");
    json += addSectionJson("data");
    json += addParamIntJson("id", idCourse, 4);
    json += addParamTableauJson("runners", idRunners, 4, false);
    json += addPiedJson(2);
    return json;
} //prepareJsonTransferRunner

bool CFormatJson::parseJsonBtnState(QString trame, T_BUTTONS &state){
    if(trame.indexOf("btnState") < 0)
        return false;
    QStringList elements = trame.split(','); //6 éléments
    state.btnSession =     (elements[1].split(':')[2].indexOf('1') > 0 ? true : false);
    state.btnPreparation = (elements[2].split(':')[1].indexOf('1') > 0 ? true : false);
    state.btnAVM =         (elements[3].split(':')[1].indexOf('1') > 0 ? true : false);
    state.btnReady =       (elements[4].split(':')[1].indexOf('1') > 0 ? true : false);
    state.btnGo =          (elements[5].split(':')[1].indexOf('1') > 0 ? true : false);
    state.btnArret =       (elements[6].split(':')[1].indexOf('1') > 0 ? true : false);
    return true;
} //parseJsonBtnState

bool CFormatJson::parseJsonAuthCheck(QString trame, QString &codeAuthentification){ // QString trame, QString &login, QString &mdp
    if(trame.indexOf("authCheck") < 0)
        return false;
    QStringList elements = trame.split(',');
    codeAuthentification = elements[1].split(':')[2].split('"')[1];
    return true;
} //parseJsonAuthCheck

bool CFormatJson::parseJsonGetCsv(QString trame){
    // Retourne -1 si incorrect sinon >= 0
    return trame.indexOf("getCsv");
} //parseJsonGetCsv

bool CFormatJson::parseJsonGetControl(QString trame)
{
    return trame.indexOf("getControl");

} //parseJsonGetControl

QString CFormatJson::prepareJsonTimeRun(int idCourse, T_TIMERUN timeRun){
    QString json;
    json = addEnteteJson("timeRun");
    json += addSectionJson("data");
    json += addParamTexteJson("id", QString::number(idCourse), 4);
    json += addParamTexteJson("time1", timeRun.time1, 4);
    json += addParamTexteJson("wind1", timeRun.wind1, 4);
    json += addParamTexteJson("speed1", timeRun.speed1, 4);
    json += addParamTexteJson("time2", timeRun.time2, 4);
    json += addParamTexteJson("wind2", timeRun.wind2, 4);
    json += addParamTexteJson("speed2", timeRun.speed2, 4, false);
    json += addPiedJson(2);
    return json;
} //prepareJsonTimeRun

QString CFormatJson::prepareJsonTransfertCsv(int nbCoureurs, T_CSV *csv){
    QString json;
    json = addEnteteJson("transferCsv");
    json = addSectionJson("data");
    json += addParamTexteJson("runnersCnt", QString::number(nbCoureurs), 4);

    for(int i=0; i < nbCoureurs; i++){
        json += addSectionJson("runner" + QString::number(i+1), 4);
        json += addParamTexteJson("idRun", QString::number(csv[i].idRun), 6);
        json += addParamTexteJson("name", csv[i].name, 6);
        json += addParamTexteJson("time", csv[i].time, 6);
        json += addParamTexteJson("wind", csv[i].wind, 6);
        json += addParamTexteJson("speed", csv[i].speed, 6, false);
        json += addFinSectionJson(4, (i < (nbCoureurs - 1) ? true : false));
    } //for
    json += addPiedJson(2);
    return json;
} //prepareJsonTransfertCsv

QString CFormatJson::prepareJsonAuthCheck(bool res, CBdd *bdd){
    QString json;
    json = addEnteteJson("authCheck");
    json += addSectionJson("data");
    if(res){
        json += addParamIntJson("success", (bdd->isSessionActive() ? 1 : 2 ), 4, false);  // 1 Bon PIN session ouverte  2 bon PIN session fermée
    }else{
        json += addParamIntJson("success", 0, 4, false); // 0 PIN mauvais
    }
    json += addPiedJson(2);
    return json;
} //prepareJsonAuthCheck

QString CFormatJson::prepareJsonTransfertAllRunners(QString sessionName, T_TYPE_COURSE typeCourse, QList<T_2COUREURS> courses) //---- 2024
{
    QString json;
    json = addEnteteJson("transfertAllRunners");
    json += addSectionJson("data");
    json += addParamTexteJson("sessionName", sessionName, false);
    json += addParamIntJson("typeCourse", typeCourse, false);
    json += addParamIntJson("runnersCnt", courses.size()*2, true);
    for (int i=0 ; i<courses.size() ; i++) {
        json += addSectionJson("runner"+QString::number(i+1),4);
        json += addParamTexteJson("name", QString(courses.at(i).noms[0]), 1, false);
        json += addFinSectionJson(4, true);
        json += addSectionJson("runner"+QString::number(i+1),4);
        json += addParamTexteJson("name", QString(courses.at(i).noms[0]), 1, false);
        json += addFinSectionJson(4, (i<(courses.size()-1)?true:false));
    } // for i
    json += addPiedJson(2);
    return json;
} //prepareJsonTransfertAllRunners

QString CFormatJson::prepareJsonBtnState(T_BUTTONS state){
    QString json;
    json = addEnteteJson("btnState");
    json += addSectionJson("data");
    json += addParamTexteJson("btnSess", QString::number(state.btnSession), 4);
    json += addParamTexteJson("btnPrep", QString::number(state.btnPreparation), 4);
    json += addParamTexteJson("btnAVM", QString::number(state.btnAVM), 4);
    json += addParamTexteJson("btnReady", QString::number(state.btnReady), 4);
    json += addParamTexteJson("btnGo", QString::number(state.btnGo), 4);
    json += addParamTexteJson("btnStop", QString::number(state.btnArret), 4, false);
    json += addPiedJson(2);
    return json;
} //prepareJsonBtnState

QString CFormatJson::prepareJsonGetControl()
{
    QString json;
    json = addEnteteJson("getControl", false);
    json += addPiedJson(1);
    return json;
}

QString CFormatJson::parseJsonCommande(QByteArray ba)
{
    QByteArray command;
    if((ba.indexOf(':') >= 0) && (ba.indexOf('"') >= 0))
        command = ba.split(':')[1].split('"')[1];
    return command;
} // decodeCommandeJson

//QString CProtocole::prepareJsonTransfertSession(int runNumber, &QList<QList<QString>> runs)
//{
//    QString json;
//    json = adddEnteteJson("sessionTransfert");
//} //prepareJsonTransfertSession

/////////////////////////////////////////////////////////////////////
/// \brief CFormatJson::addEnteteJson
/// \param commande
/// \param suite
/// \return
///
QString CFormatJson::addEnteteJson(QString commande, bool suite){
    QString json;
    json.append("{\"command\": \"" + commande + "\"");
    if(suite)
        json.append(",");
    return json;
} //addEnteteJson

QString CFormatJson::addPiedJson(int nb){
    QString json;
    if(nb > 0){
        for(int i = 0; i < nb; i++){
            json.append("}");
        }
    }
    return json;
} //addPiedJson

QString CFormatJson::addSectionJson(QString nom, int dec){
    QString json;
    json.fill(' ', dec);
    json.append("\"" + nom + "\": {");
    return json;
} //addSectionJson

QString CFormatJson::addFinSectionJson(int dec, int suite){
    QString json;
    json.fill(' ', dec);
    json.append("}");
    if(suite)
        json.append(",");
    return json;
} //addFinSectionJson

QString CFormatJson::addParamTexteJson(QString nom, QString valeur, int dec, bool suite){
    QString json;
    json.fill(' ', dec);
    json.append("\"" + nom + "\": \"" + valeur + "\"");
    if(suite)
        json.append(',');
    return json;
} //addParamTexteJson

QString CFormatJson::addParamIntJson(QString nom, int valeur, int dec, bool suite){
    QString json;
    json.fill(' ', dec);
    json.append("\"" + nom + "\": " + QString::number(valeur));
    if(suite)
        json.append(",");
    return json;
} //addParamIntJson

QString CFormatJson::addParamTableauJson(QString nom, QList<int> valeur, int dec, bool suite){
    QString json;
    json.fill(' ', dec);
    json.append("\""+nom+"\": [");
    for (int i=0 ; i<valeur.size() ; i++) {
        json.fill(' ', dec+2);
        json.append(QString::number(valeur.at(i)));
        if (i < valeur.size()-1)
            json.append(",");
    } // for
    json.fill(' ', dec);
    json.append("]");
    if (suite)
        json.append(",");
    return json;
} //addParamTableauJson

QString CFormatJson::filtreAvantParser(QString trame)
{
    int pos;
    do {
        pos = trame.indexOf("\r\n");
        if (pos >= 0)
            trame.remove(pos, 2);

    } while(pos >= 0);
    do {
        pos = trame.indexOf("\n");
        if (pos >= 0)
            trame.remove(pos, 1);
    } while(pos >= 0);
    do {
        pos = trame.indexOf("\r");
        if (pos >= 0)
            trame.remove(pos, 1);
    } while(pos >= 0);
    return trame;
}//filtreAvantParser
