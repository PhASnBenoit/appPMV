#include "ccsv.h"

CCsv::CCsv(QObject *parent) : QObject(parent)
{
}


// A montrer !
QStringList CCsv::importCSV(QString path, T_TYPE_COURSE typeCourse) //--- 2024
{
    QMessageBox msgBox;
    QFileDialog myDialog;
    QStringList contenuFichier;

    _fileImport.setFileName(path);
    _fileImport.open(QIODevice::ReadOnly);
    while(!_fileImport.atEnd()){
        QString ligne = _fileImport.readLine();
        ligne = ligne.remove("\n");
        if (!ligne.isEmpty())
            contenuFichier.append(ligne);
    } // wh
    _fileImport.close();

    int noFormat=1;
    QStringList liste;// Liste de l'ensemble des Prénoms dans l'ordre
    _nomsEleves.clear();
    _ligne = contenuFichier.at(0);  // on isole la première ligne pour déterminer le type de syntaxe
    liste = _ligne.split(DELIMITER); // Split à partir du ";" qui a été défini dans un define "DELIMITER"

    if (liste.size() > 2) {
        msgBox.setText("Votre fichier contient des caractères qui peuvent perturber le programmme.\n");
        msgBox.setInformativeText("Si c'est ce symbole ou autres qui sont contenus dans votre fichier -> ';' ou 'ahfhfhdfjdf' alors veuillez le modifier.\n");
        msgBox.exec();
        return _nomsEleves; // erreur de format de fichier
    } // if > 2

    if (liste.size() == 2)
        noFormat = (liste.at(1).isEmpty()?1:2);

    switch (noFormat) {
    case 1:// 1er Format CSV
        for (int i=0 ; i<contenuFichier.size() ; i++) {
            _ligne = contenuFichier.at(i);
            liste = _ligne.split(DELIMITER);// split
            _nomsEleves.append(liste.at(0));
            if (typeCourse == DEPART_LANCE)
                _nomsEleves.append("noname");
        } // for
        break;
    case 2: // 2ème Format CSV
        for (int i=0 ; i<contenuFichier.size() ; i++) {
            _ligne = contenuFichier.at(i);
            liste = _ligne.split(DELIMITER);// split
            _nomsEleves.append(liste.at(0));
            _nomsEleves.append(liste.at(1));
        } // for
        break;
    } // sw
    return _nomsEleves;
}

QString CCsv::chooseImportFile() //----- 2024
{
    QFileDialog myDialog;
    QString path = myDialog.getOpenFileName(); // popup
    if (!path.isEmpty())
        return path;
    return "";
}

QString CCsv::chooseExportFile()
{
    QFileDialog myDialog;
    QString path = myDialog.getSaveFileName(nullptr, tr("Save file"), "/home/pi/Documents/", tr("Text Editor(*.csv)")); // popup
    if (!path.isEmpty())
        return path;
    return "";
}

void CCsv::exportCSV(QString path)
{
    T_DATAS datas;
    CZdc zdc;
    zdc.getDatas(datas);

    _fileExport.setFileName(path);
    _fileExport.open(QIODevice::WriteOnly);
    QTextStream output(&_fileExport);

    output << datas.paramSession.nom
           << " commencé le " << datas.paramSession.dateDeb
           << " à " << datas.paramSession.heureDeb
           << " terminé le " << datas.paramSession.dateFin
           << " à " << datas.paramSession.heureFin << ".\n";
    output << "Type de course : " << datas.paramSession.typeCourse << ".\n";
    output << "Nom Prénom;Temps coureur;Vitesse coureur;Force du vent;Direction du vent\n";
    for(int i=0; i < 20; i++)
    {
        output << datas.coureurs[i].noms[0] << ";" << datas.coureurs[i].temps[0] << ";" << datas.coureurs[i].vitesse[0] << ";" << datas.coureurs[i].vent << ";" << datas.coureurs[i].dirVent << "\n";
        output << datas.coureurs[i].noms[1] << ";" << datas.coureurs[i].temps[1] << ";" << datas.coureurs[i].vitesse[1] << ";" << datas.coureurs[i].vent << ";" << datas.coureurs[i].dirVent << "\n";
    }
    _fileExport.close();
}
