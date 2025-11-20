#include "ihmdanger.h"
#include "ui_ihmdanger.h"
#include "QMessageBox"
#include <QIntValidator>
#include <QScrollBar>
#include <algorithm>

QByteArray IHMDanger::trameExtraite;

IHMDanger::IHMDanger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IHMDanger)
{
    ui->setupUi(this);

    QIntValidator *valid = new QIntValidator(0, 99, ui->lineEdit_signal_nb_chiffre_afficher);
    ui->lineEdit_signal_nb_chiffre_afficher->setValidator(valid);

    isPortOpen = false;
    trameExtraite = "";
    isInTrame = false;
}

IHMDanger::~IHMDanger()
{
    if(isPortOpen)
        on_pushButton_com_deconnexion_clicked();
    delete ui;
}

void IHMDanger::reception()
{
    // Récupération des octets disponibles et lecture des octets
    int nb = portConnexion->bytesAvailable();
    QByteArray OctetsRecu = portConnexion->readAll();
    //

    // On extrait la trame
    auto itfirst = std::find(OctetsRecu.begin(), OctetsRecu.end(), '#');
    auto itlast = std::find(OctetsRecu.begin(), OctetsRecu.end(), '\n');

    /// Si l'octet a un # dedans, on commence a recupérer la trame
    if(itfirst != OctetsRecu.end())
    {
        trameExtraite += OctetsRecu;
        isInTrame = true;
    }
    /// Si l'octet a un \n dedans, on récupere les derniers octets
    /// et on affiche puis reinitialise la trame
    else if(itlast != OctetsRecu.end())
    {
        trameExtraite += OctetsRecu;
        isInTrame = false;

        /// Trame extraite
        ui->lineEdit_wd_trame_extraite->setText(trameExtraite);

        /// nb octets trame extraite
        ui->lineEdit_wd_nb_octets_trame_extr->setText(QString::number(trameExtraite.size()));

        trameExtraite = "";
    }
    /// Si la récupération de trame a commencé, on recupere l'octet
    else if(isInTrame){
        trameExtraite += OctetsRecu;
    }
    //

    // Rempli les linEdit
    if(OctetsRecu.size() != 0)
    {
        /// nb Octets lu
        ui->lineEdit_wd_nb_octets_lus->setText(QString::number(OctetsRecu.size()));

        /// nb Octets disponible
        ui->lineEdit_wd_nb_octets_disp->setText(QString::number(nb));


        QString StrameExtraite = (QString)trameExtraite;
        QStringList ListTrameExtraite = StrameExtraite.split(';');
        /// #;5.00;5.00;5.00;18.43;1016;led1:1;led2:1;15;1;
        /// 0  1   2    3    4     5    6      7      8  9
        if(ui->pushButton_wd_affichage_ameliore->isChecked())
        {
            if(ListTrameExtraite.size() == 10)
            {
                QString donneeAmelioree = "";

                donneeAmelioree += "\n------------------------------";
                donneeAmelioree += "\nSlider 1 : " + ListTrameExtraite[1];
                donneeAmelioree += "\nSlider 2 : " + ListTrameExtraite[2];
                donneeAmelioree += "\nSlider 3 : " + ListTrameExtraite[3];
                donneeAmelioree += "\nTempérature : " + ListTrameExtraite[4];
                donneeAmelioree += "\nLuminosité : " + ListTrameExtraite[5];

                QStringList led1 = ListTrameExtraite[6].split(':');
                if(led1[1] == "1"){
                    donneeAmelioree += "\nLed 1 : Allumée";
                }
                else
                {
                    donneeAmelioree += "\nLed 1 : Eteinte";
                }

                QStringList led2 = ListTrameExtraite[7].split(':');
                if(led2[1] == "1"){
                    donneeAmelioree += "\nLed 2 : Allumée";
                }
                else
                {
                    donneeAmelioree += "\nLed 2 : Eteinte";
                }

                if(ListTrameExtraite[8] == "XX"){
                    donneeAmelioree += "\nAfficheur : Eteinte";
                }
                else
                {
                    donneeAmelioree += "\nAfficheur : Allumée; Valeure : " + ListTrameExtraite[8];
                }

                ///Donnee Amelioré
                ui->textEdit_wd_donnee_lues->insertPlainText(donneeAmelioree);
            }
        }
        else
        {
            /// Donnee lues brut
            ui->textEdit_wd_donnee_lues->insertPlainText(OctetsRecu);
        }
    }
    else
    {
        ui->textEdit_wd_donnee_lues->insertPlainText("|None|");
    }
    //
}

void IHMDanger::autoScroll()
{
    ui->textEdit_wd_donnee_lues->verticalScrollBar()->setValue(ui->textEdit_wd_donnee_lues->verticalScrollBar()->maximum());
}

void IHMDanger::on_pushButton_com_connexion_clicked()
{
    // Création classe du port com
    portConnexion = new QSerialPort("COM" + ui->spinBox_com_portcom->text());

    // Configuration de la classe
    portConnexion->setBaudRate(QSerialPort::Baud9600);
    portConnexion->setDataBits(QSerialPort::Data8);
    portConnexion->setParity(QSerialPort::NoParity);
    portConnexion->setStopBits(QSerialPort::OneStop);
    portConnexion->setFlowControl(QSerialPort::NoFlowControl);
    //

    // Connection au Port COM
    if (portConnexion->open(QIODevice::ReadWrite))
    {
        connect(portConnexion, SIGNAL(readyRead()), this, SLOT(reception()));
        connect(ui->textEdit_wd_donnee_lues, SIGNAL(textChanged()), this, SLOT(autoScroll()));

        isPortOpen = true;

        ui->label_com_status_output->setText("Connecté");
        QMessageBox::information(this, "INFO", "Port ouvert avec succès");
    }
    else
    {
        QMessageBox::critical(this, "ERROR", "Erreur d'ouverture : " + portConnexion->errorString());
    }
    //
}

void IHMDanger::on_pushButton_com_deconnexion_clicked()
{
    if (isPortOpen)
    {
        portConnexion->close();
        delete portConnexion;

        isPortOpen = false;

        ui->label_com_status_output->setText("Non Connecté");
        QMessageBox::information(this, "INFO", "Port série fermé.");
    }
    else
    {
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::envoieTrame(QString trame)
{
    if (isPortOpen)
    {
        qint64 OctetsEnvoyes = portConnexion->write(trame.toStdString().c_str());
        if(OctetsEnvoyes == -1){
            QMessageBox::critical(this, "ERROR", "Erreur d'envoi : " + portConnexion->errorString());
        }
        else{
            QMessageBox::information(this, "INFO", "Envoi effectué");
        }
    }
    else
    {
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::on_pushButton_signal_led1_clicked()
{
    envoieTrame("$;led1;XXXX;XXXXX;XX;\n");
}

void IHMDanger::on_pushButton_signal_led2_clicked()
{
    envoieTrame("$;XXXX;led2;XXXXX;XX;\n");
}

void IHMDanger::on_pushButton_signal_afficher_clicked()
{
    QString nb = ui->lineEdit_signal_nb_chiffre_afficher->text();
    if(nb.size() == 2)
    {
        envoieTrame("$;XXXX;XXXX;segON;" + nb + ";\n");
    }
    else
    {
        QMessageBox::critical(this, "ERROR", "Nombre invalide ou vide.");
    }
}

void IHMDanger::on_pushButton_signal_eteindre_clicked()
{
    envoieTrame("$;XXXX;XXXX;segOF;XX;\n");
}
