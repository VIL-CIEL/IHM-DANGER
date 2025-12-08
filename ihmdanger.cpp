#include "ihmdanger.h"
#include "ui_ihmdanger.h"
#include "QMessageBox"
#include <QScrollBar>
#include <QTimer>
#include <QFile>
#include <QCoreApplication>
#include <QDir>

IHMDanger::IHMDanger(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::IHMDanger)
{
    ui->setupUi(this);

    // Config Carte Branly
    estCarteBranly = true;

    // Config Digit Carte
    if(estCarteBranly)
        valid = new QIntValidator(0, 99, ui->lineEdit_signal_nb_chiffre_afficher);
    else
        valid = new QIntValidator(0, 9, ui->lineEdit_signal_nb_chiffre_afficher);
    ui->lineEdit_signal_nb_chiffre_afficher->setValidator(valid);

    GestPorts = nullptr;

    // timer
    timer = new QTimer(this);
    timer->setInterval(1000); // 1000ms = 1s
    connect(timer, SIGNAL(timeout()), this, SLOT(on_Timeout()));
    isTimerStarted = false;

    // Configuration Type d'Affichage
    on_comboBox_horo_type_affichage_currentTextChanged(ui->comboBox_horo_type_affichage->currentText());
    //
}

IHMDanger::~IHMDanger()
{
    timer->stop();
    if(GestPorts != nullptr){
        delete GestPorts;
        QMessageBox::information(this, "INFO", "Port série fermé.");
    }
    delete valid;
    delete timer;
    delete ui;
}

void IHMDanger::reception()
{
    QByteArray OctetsRecu;
    QString donneeAmelioree;

    GestPorts->reception(&OctetsRecu, &donneeAmelioree, isAffichageAmeliore);

    if(OctetsRecu.size() == 0) // Vérifie qu'on a reçu des données
    {
        ui->textEdit_horo_donnee_lues->insertPlainText("|None|");
    }
    else
    {
        if(!isAffichageAmeliore) // Afficher les données brut
        {
            /// Donnee lues brut
            ui->textEdit_horo_donnee_lues->insertPlainText(OctetsRecu);
        }

        QStringList LDonnee = donneeAmelioree.split("//");

        if(LDonnee.size() == 10) // Vérifie si on a les valeurs de tout les capteurs tel que :
        // 0 : slider 1, 1 : slider 2, 2 : slider 3, 3 : lumieres, 4 : temperature, 5 : afficheur, 6 : led1, 7 : led2, 8 : date, 9 : heure
        {
            if(isAffichageAmeliore) // Regarde si on doit afficher les valeurs détaillées
            {
                if(ui->comboBox_horo_type_affichage->currentText() == "Liste")
                ///Donnee Amelioré en liste
                {
                    QString ListeDonnee = QString("\n-----------------------------"
                                                  "\nSlider 1 : %1"
                                                  "\nSlider 2 : %2"
                                                  "\nSlider 2 : %3"
                                                  "\nTemperature : %4"
                                                  "\nLuminosité : %5"
                                                  "\nLed 1 : %6"
                                                  "\nLed 2 : %7"
                                                  "\n Afficheur : %8"
                                                  "\n Date : %9 %10")
                                              .arg(LDonnee[0])
                                              .arg(LDonnee[1])
                                              .arg(LDonnee[2])
                                              .arg(LDonnee[4])
                                              .arg(LDonnee[3])
                                              .arg(LDonnee[6])
                                              .arg(LDonnee[7])
                                              .arg(LDonnee[5])
                                              .arg(LDonnee[8])
                                              .arg(LDonnee[9]);
                    ui->textEdit_horo_donnee_lues->insertPlainText(ListeDonnee);
                }
                ///Donnee Amelioré en Tableau
                else
                {
                    // Ajoute une ligne
                    ui->tableWidget_horo_table->insertRow( ui->tableWidget_horo_table->rowCount());

                    // Modifie la case Date
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 0, new QTableWidgetItem(LDonnee[8]));

                    // Modifie la case Heure
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 1, new QTableWidgetItem(LDonnee[9]));

                    // Modifie la case Slider 1
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 2, new QTableWidgetItem(LDonnee[0]));

                    // Modifie la case Slider 2
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 3, new QTableWidgetItem(LDonnee[1]));

                    // Modifie la case Slider 3
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 4, new QTableWidgetItem(LDonnee[2]));

                    // Modifie la case Temperature
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 5, new QTableWidgetItem(LDonnee[4]));

                    // Modifie la case Luminosite
                    ui->tableWidget_horo_table->setItem( ui->tableWidget_horo_table->rowCount()-1, 6, new QTableWidgetItem(LDonnee[3]));

                    // Scroll jursqu'a la fin du tableau
                    ui->tableWidget_horo_table->scrollToBottom();
                }
            }

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

            /// Valeurs d'afficheur, LCD Number
            if(LDonnee[5] == "XX" || LDonnee[5] == "X"){
                ui->lcdNumber_signal_nb_affiche->display("-----");
            }
            else
            {
                ui->lcdNumber_signal_nb_affiche->display(LDonnee[5].toFloat());
            }

            /// Valeurs de led1 : changement couleur bouton
            if(LDonnee[6] == "1")
            {
                ui->pushButton_signal_led1->setStyleSheet("background-color: green;");
            }
            else
            {
                ui->pushButton_signal_led1->setStyleSheet("");
            }

            /// Valeurs de led2 : changement couleur bouton
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

void IHMDanger::autoScroll()
{
    ui->textEdit_horo_donnee_lues->verticalScrollBar()->setValue(ui->textEdit_horo_donnee_lues->verticalScrollBar()->maximum());
}

void IHMDanger::on_Timeout()
{
    if(GestPorts->portConnexion->canReadLine())
        reception();
}


void IHMDanger::on_pushButton_com_connexion_clicked()
{
    GestPorts = new GestionPortSerie(ui->spinBox_com_portcom->text(), ui->comboBox_com_baud->currentText().toInt());

    // Connection au Port COM
    if (GestPorts->portConnexion->open(QIODevice::ReadWrite))
    {
        //connect(GestPorts->portConnexion, SIGNAL(readyRead()), this, SLOT(reception()));
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
        //Gestion Carte Branly
        QString err;
        if(estCarteBranly)
            err = GestPorts->envoieTrame("$;led1;XXXX;XXXXX;XX;\n");
        else
            err = GestPorts->envoieTrame("$;led1;XXXX;XXXXX;X;\n");

        // Gestion D'erreur
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
        //Gestion Carte Branly
        QString err;
        if(estCarteBranly)
            err = GestPorts->envoieTrame("$;XXXX;led2;XXXXX;XX;\n");
        else
            err = GestPorts->envoieTrame("$;XXXX;led2;XXXXX;X;\n");

        // Gestion D'erreur
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

        // Gestion Carte Branly
        //    -> Soit c'est une carte branly donc nb.size = 2
        //    -> Ou ce n'est pas une carte branly donc on a nb.size = 1
        if(nb.size() == 2 || (!estCarteBranly && nb.size() == 1))
        {
            QString err = GestPorts->envoieTrame("$;XXXX;XXXX;segON;" + nb + ";\n");

            // Gestion D'erreur
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

        //Gestion Carte Branly
        QString err;
        if(estCarteBranly)
            err = GestPorts->envoieTrame("$;XXXX;XXXX;segOF;XX;\n");
        else
            err = GestPorts->envoieTrame("$;XXXX;XXXX;segOF;X;\n");

        // Gestion D'erreur
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

void IHMDanger::on_pushButton_horo_stop_clicked()
{
    if(!isTimerStarted)
    {
        if(ui->lineEdit_horo_interval->text().size() != 0){
            timer->start(ui->lineEdit_horo_interval->text().toInt()*1000);
            ui->pushButton_horo_stop->setText("Stop");
            isTimerStarted = true;
        }
        else{
            QMessageBox::critical(this, "ERROR", "Merci de renseigner une intervalle.");
        }
    }
    else
    {
        timer->stop();
        ui->pushButton_horo_stop->setText("Début Acquisition");
        isTimerStarted = false;
    }
}


void IHMDanger::on_pushButton_horo_effacer_clicked()
{
    if(ui->comboBox_horo_type_affichage->currentText() != "Tableau")
        ui->textEdit_horo_donnee_lues->clear();
    else
        ui->tableWidget_horo_table->setRowCount(0);
}


void IHMDanger::on_pushButton_wd_sauvegarder_clicked()
{
    QString appDir = QCoreApplication::applicationDirPath();
    QString filePath = QDir(appDir).absoluteFilePath("sauvegarde.txt");

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

        QTextStream stream(&file);
        stream << ui->textEdit_horo_donnee_lues->toPlainText();

        if(file.flush())
        {
            QMessageBox::information(this, "INFO", "Données Sauvegardées");
        }
        else
        {
            QMessageBox::critical(this, "ERROR", "Les données n'ont pas été sauvegardées.");
        }
        file.close();

    } else {
        QMessageBox::critical(this, "ERROR", "Impossible d'ouvrir le fichier de sauvegarde.");
    }
}


void IHMDanger::on_pushButton_wd_quitter_clicked()
{
    QApplication::quit();
}


void IHMDanger::on_checkBox_signal_carte_branly_stateChanged(int arg1)
{
    if(arg1 == 2)
    {
        estCarteBranly = true;
    }
    else
    {
        estCarteBranly = false;
    }

    ui->lineEdit_signal_nb_chiffre_afficher->clear();
    if(estCarteBranly)
        valid = new QIntValidator(0, 99, ui->lineEdit_signal_nb_chiffre_afficher);
    else
        valid = new QIntValidator(0, 9, ui->lineEdit_signal_nb_chiffre_afficher);
    ui->lineEdit_signal_nb_chiffre_afficher->setValidator(valid);
}


void IHMDanger::on_comboBox_horo_type_affichage_currentTextChanged(const QString &arg1)
{
    // Configuration Type d'Affichage
    if(arg1 == "Liste")
    {
        isAffichageAmeliore = true;
        ui->textEdit_horo_donnee_lues->show();
        ui->tableWidget_horo_table->hide();
        // Facultatif
        ui->tableWidget_horo_table->setRowCount(0);
    }
    else if(arg1 == "Tableau")
    {
        isAffichageAmeliore = true;
        ui->tableWidget_horo_table->show();
        ui->textEdit_horo_donnee_lues->hide();
        // Facultatif
        ui->textEdit_horo_donnee_lues->clear();
    }
    else
    {
        isAffichageAmeliore = false;
        ui->textEdit_horo_donnee_lues->show();
        ui->tableWidget_horo_table->hide();
        // Facultatif
        ui->tableWidget_horo_table->setRowCount(0);
    }
    //
}

