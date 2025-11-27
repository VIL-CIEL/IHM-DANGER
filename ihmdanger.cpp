#include "ihmdanger.h"
#include "ui_ihmdanger.h"
#include "QMessageBox"
#include <QIntValidator>
#include <QScrollBar>

IHMDanger::IHMDanger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IHMDanger)
{
    ui->setupUi(this);

    QIntValidator *valid = new QIntValidator(0, 99, ui->lineEdit_signal_nb_chiffre_afficher);
    ui->lineEdit_signal_nb_chiffre_afficher->setValidator(valid);

    GestPorts = nullptr;
}

IHMDanger::~IHMDanger()
{
    if(GestPorts != nullptr){
        delete GestPorts;
        QMessageBox::information(this, "INFO", "Port série fermé.");
    }
    delete ui;
}

void IHMDanger::reception()
{
    QByteArray OctetsRecu;
    QString donneeAmelioree;

    GestPorts->reception(&OctetsRecu, &donneeAmelioree, ui->pushButton_horo_affichage_ameliore->isChecked());
    QStringList LDonnee = donneeAmelioree.split("/!/");

    if(LDonnee.size() == 2) // Si on a la validation du check et le reste des valeurs
    {
        QString ischeckStr = LDonnee[0];
        LDonnee = LDonnee[1].split("/:/:/");


            if(OctetsRecu.size() == 0) // Vérifie qu'on a reçu des données
            {
                ui->textEdit_horo_donnee_lues->insertPlainText("|None|");
            }
            else
            {
                if(ischeckStr == "isChecked") // Regarde si on doit afficher les valeurs détaillées
                {
                    ///Donnee Amelioré
                    ui->textEdit_horo_donnee_lues->insertPlainText(LDonnee[0]);
                }
                else // Afficher les données brut
                {
                    /// Donnee lues brut
                    ui->textEdit_horo_donnee_lues->insertPlainText(OctetsRecu);
                }
            }

            if(LDonnee.size() == 2) // Si on a les valeurs d'affichage + et les valeurs de progressbar/lcd
            {
                LDonnee = LDonnee[1].split("//");

                if(LDonnee.size() == 8) // Vérifie si on a les valeurs de tout les capteurs tel que :
                // 0 : slider 1, 1 : slider 2, 2 : slider 3, 3 : lumieres, 4 : temperature, 5 : afficheur
                {
                    /// Valeurs des progress bar
                    ui->progressBar_valm_slider1->setValue(LDonnee[0].toFloat()*20);
                    ui->progressBar_valm_slider2->setValue(LDonnee[1].toFloat()*20);
                    ui->progressBar_valm_slider3->setValue(LDonnee[2].toFloat()*20);

                    /// Valeurs des LCD Number
                    ui->lcdNumber_valm_slider1->display(LDonnee[0].toFloat());
                    ui->lcdNumber_valm_slider2->display(LDonnee[1].toFloat());
                    ui->lcdNumber_valm_slider3->display(LDonnee[2].toFloat());
                    ui->lcdNumber_signal_luminosite->display(LDonnee[3].toFloat());
                    ui->lcdNumber_valm_temperature->display(LDonnee[4].toFloat());
                    if(LDonnee[5] == "XX"){
                        ui->lcdNumber_signal_nb_affiche->display("-----");
                    }
                    else
                    {
                        ui->lcdNumber_signal_nb_affiche->display(LDonnee[5].toFloat());
                    }

                    if(LDonnee[6] == "1")
                    {
                        ui->pushButton_signal_led1->setStyleSheet("background-color: green;");
                    }
                    else
                    {
                        ui->pushButton_signal_led1->setStyleSheet("");
                    }
                    if(LDonnee[7] == "1")
                    {
                        ui->pushButton_signal_led2->setStyleSheet("background-color: green;");
                    }
                    else
                    {
                        ui->pushButton_signal_led2->setStyleSheet("");
                    }

                }
        }
    }

}

void IHMDanger::autoScroll()
{
    ui->textEdit_horo_donnee_lues->verticalScrollBar()->setValue(ui->textEdit_horo_donnee_lues->verticalScrollBar()->maximum());
}


void IHMDanger::on_pushButton_com_connexion_clicked()
{
    GestPorts = new GestionPortSerie(ui->spinBox_com_portcom->text());

    // Connection au Port COM
    if (GestPorts->portConnexion->open(QIODevice::ReadWrite))
    {
        connect(GestPorts->portConnexion, SIGNAL(readyRead()), this, SLOT(reception()));
        connect(ui->textEdit_horo_donnee_lues, SIGNAL(textChanged()), this, SLOT(autoScroll()));

        ui->label_com_status_output->setText("Connecté");
        QMessageBox::information(this, "INFO", "Port ouvert avec succès");
    }
    else
    {
        QMessageBox::critical(this, "ERROR", "Erreur d'ouverture : " + GestPorts->portConnexion->errorString());
    }
    //
}

void IHMDanger::on_pushButton_com_deconnexion_clicked()
{
    if(GestPorts != nullptr){
        delete GestPorts;
        GestPorts = nullptr;

        ui->label_com_status_output->setText("Non Connecté");
        QMessageBox::information(this, "INFO", "Port série fermé.");
    }
    else{
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::on_pushButton_signal_led1_clicked()
{
    if(GestPorts != nullptr){
        QString err = GestPorts->envoieTrame("$;led1;XXXX;XXXXX;XX;\n");
        QStringList e = err.split("/");
        if(e[0] == "ERROR")
        {
            QMessageBox::critical(this, e[0], e[1]);
        }
        else
        {
            QMessageBox::information(this, e[0], e[1]);
        }
    }
    else{
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::on_pushButton_signal_led2_clicked()
{
    if(GestPorts != nullptr){
        QString err = GestPorts->envoieTrame("$;XXXX;led2;XXXXX;XX;\n");

        QStringList e = err.split("/");
        if(e[0] == "ERROR")
        {
            QMessageBox::critical(this, e[0], e[1]);
        }
        else
        {
            QMessageBox::information(this, e[0], e[1]);
        }
    }
    else{
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::on_pushButton_signal_afficher_clicked()
{
    if(GestPorts != nullptr){
        QString nb = ui->lineEdit_signal_nb_chiffre_afficher->text();
        if(nb.size() == 2)
        {
            QString err = GestPorts->envoieTrame("$;XXXX;XXXX;segON;" + nb + ";\n");

            QStringList e = err.split("/");
            if(e[0] == "ERROR")
            {
                QMessageBox::critical(this, e[0], e[1]);
            }
            else
            {
                QMessageBox::information(this, e[0], e[1]);
            }
        }
        else
        {
            QMessageBox::critical(this, "ERROR", "Nombre invalide ou vide.");
        }
    }
    else{
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}

void IHMDanger::on_pushButton_signal_eteindre_clicked()
{
    if(GestPorts != nullptr){

        QString err =  GestPorts->envoieTrame("$;XXXX;XXXX;segOF;XX;\n");
        QStringList e = err.split("/");
        if(e[0] == "ERROR")
        {
            QMessageBox::critical(this, e[0], e[1]);
        }
        else
        {
            QMessageBox::information(this, e[0], e[1]);
        }
    }
    else{
        QMessageBox::critical(this, "ERROR", "Le port n'est pas ouvert.");
    }
}
