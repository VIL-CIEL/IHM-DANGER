#include "gestionportserie.h"
#include "QMessageBox"
#include <QIntValidator>
#include <QScrollBar>
#include <algorithm>
#include <QDateTime>

QByteArray GestionPortSerie::trameExtraite;

GestionPortSerie::GestionPortSerie(QString portC, QObject *parent)
    : QObject{parent}
{
    trameExtraite = "";
    isInTrame = false;

    // Création classe du port com
    portConnexion = new QSerialPort("COM" + portC);

    // Configuration de la classe
    portConnexion->setBaudRate(QSerialPort::Baud9600);
    portConnexion->setDataBits(QSerialPort::Data8);
    portConnexion->setParity(QSerialPort::NoParity);
    portConnexion->setStopBits(QSerialPort::OneStop);
    portConnexion->setFlowControl(QSerialPort::NoFlowControl);
    //
}

GestionPortSerie::~GestionPortSerie()
{
    portConnexion->close();
    delete portConnexion;
}

void GestionPortSerie::reception(QByteArray* OctetsRecu, QString* donneeAmelioree, bool isChecked)
{
    trameExtraite.clear();
    // Récupération des octets disponibles et lecture des octets
    *OctetsRecu = portConnexion->readAll();
    //

    // Extraction de la trame
    int startPos = OctetsRecu->indexOf('#');
    int endPos = OctetsRecu->indexOf('\n', startPos);;

    if (startPos != -1 && endPos != -1) {  // Si '#' est trouvé
        trameExtraite = OctetsRecu->mid(startPos, endPos - startPos -1);
    }
    //

    // Rempli les linEdit

    if(isChecked)
    {
        *donneeAmelioree = "isChecked/!/";
    }
    else
    {
        *donneeAmelioree = "noChecked/!/";
    }

    if(OctetsRecu->size() != 0)
    {
        QString StrameExtraite = (QString)trameExtraite;
        QStringList ListTrameExtraite = StrameExtraite.split(';');
        /// #;5.00;5.00;5.00;18.43;1016;led1:1;led2:1;15;1;
        /// 0  1   2    3    4     5    6      7      8  9
        if(ListTrameExtraite.size() == 10)
        {
            *donneeAmelioree += "\n------------------------------";
            *donneeAmelioree += "\nSlider 1 : " + ListTrameExtraite[1];
            *donneeAmelioree += "\nSlider 2 : " + ListTrameExtraite[2];
            *donneeAmelioree += "\nSlider 3 : " + ListTrameExtraite[3];
            *donneeAmelioree += "\nTempérature : " + ListTrameExtraite[4];
            *donneeAmelioree += "\nLuminosité : " + ListTrameExtraite[5];

            QStringList led1 = ListTrameExtraite[6].split(':');
            if(led1[1] == "1"){
                *donneeAmelioree += "\nLed 1 : Allumée";
            }
            else
            {
                *donneeAmelioree += "\nLed 1 : Eteinte";
            }

            QStringList led2 = ListTrameExtraite[7].split(':');
            if(led2[1] == "1"){
                *donneeAmelioree += "\nLed 2 : Allumée";
            }
            else
            {
                *donneeAmelioree += "\nLed 2 : Eteinte";
            }

            if(ListTrameExtraite[8] == "XX"){
                *donneeAmelioree += "\nAfficheur : Eteinte";
            }
            else
            {
                *donneeAmelioree += "\nAfficheur : Allumée; Valeure : " + ListTrameExtraite[8];
            }

            QString sDate = QDateTime::currentDateTime().toString("dd/MM/yy hh:mm:ss");
            *donneeAmelioree += "\nDate : " + sDate;

            *donneeAmelioree += "/:/:/";

            //Donnee des sliders
            *donneeAmelioree += ListTrameExtraite[1] + "//" + ListTrameExtraite[2] + "//" + ListTrameExtraite[3] + "//";

            // Donnee Lumieres
            *donneeAmelioree += ListTrameExtraite[5] + "//";

            // Donnee Temperature
            *donneeAmelioree += ListTrameExtraite[4] + "//";

            // Donnee Afficheur
            *donneeAmelioree += ListTrameExtraite[8] + "//";

            //Etat Led
            *donneeAmelioree += led1[1] + "//" + led2[1];

        }
    }
    //
}

QString GestionPortSerie::envoieTrame(QString trame)
{
    qint64 OctetsEnvoyes = portConnexion->write(trame.toStdString().c_str());
    if(OctetsEnvoyes == -1){
        return (QString)"ERROR/Erreur d'envoi : " + portConnexion->errorString();
    }
    else{
        return (QString)"INFO/Envoi effectué";
    }
}
