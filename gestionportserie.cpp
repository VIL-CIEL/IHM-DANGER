#include "gestionportserie.h"
#include "QMessageBox"
#include <QIntValidator>
#include <QScrollBar>
#include <algorithm>
#include <QDateTime>

QByteArray GestionPortSerie::trameExtraite;

GestionPortSerie::GestionPortSerie(QString portC, int BaudRate, QObject *parent)
    : QObject{parent}
{
    trameExtraite = "";
    isInTrame = false;

    // Création classe du port com
    portConnexion = new QSerialPort("COM" + portC);

    // Configuration de la classe
    portConnexion->setBaudRate(BaudRate);
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

    // Rempli les lineEdit
    if(OctetsRecu->size() != 0)
    {
        QString StrameExtraite = (QString)trameExtraite;
        QStringList ListTrameExtraite = StrameExtraite.split(';');
        /// #;5.00;5.00;5.00;18.43;1016;led1:1;led2:1;15;1;
        /// 0  1   2    3    4     5    6      7      8  9
        /// && ListTrameExtraite[8].toInt() == checkSum(trameExtraite)
        if(ListTrameExtraite.size() == 10)
        {
            // Recupere la chaine utilisé pour faire le checksum
            QByteArray trameCheck = trameExtraite.slice(1, trameExtraite.size()-1-ListTrameExtraite[9].size());

            if(ListTrameExtraite[9].toInt() == checkSum(trameCheck)) // Vérifie le CheckSum
            {
                QString sDate = QDateTime::currentDateTime().toString("dd/MM/yy");
                QString sHeure = QDateTime::currentDateTime().toString("hh:mm:ss");

                /// Donnee Extraites

                //Donnee des sliders
                *donneeAmelioree += ListTrameExtraite[1] + "//" +
                                    ListTrameExtraite[2] + "//" +
                                    ListTrameExtraite[3] + "//";

                // Donnee Lumieres
                *donneeAmelioree += ListTrameExtraite[5] + "//";

                // Donnee Temperature
                *donneeAmelioree += ListTrameExtraite[4] + "//";

                // Donnee Afficheur
                *donneeAmelioree += ListTrameExtraite[8] + "//";

                //Etat Led
                QStringList led1 = ListTrameExtraite[6].split(':');
                QStringList led2 = ListTrameExtraite[7].split(':');

                *donneeAmelioree += led1[1] + "//" +
                                    led2[1] + "//";

                // Date et heure
                *donneeAmelioree += sDate + "//" + sHeure;
                ///
            }
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

int GestionPortSerie::checkSum(QByteArray str)
{
    int checkSum = 0;

    for (int i = 0; i < str.length(); i++)
    {
        checkSum = checkSum ^ str.at(i);
    }
    return checkSum;
}
