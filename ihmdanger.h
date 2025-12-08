#ifndef IHMDANGER_H
#define IHMDANGER_H

#include <QMainWindow>
#include "gestionportserie.h"
#include <QIntValidator>

QT_BEGIN_NAMESPACE
namespace Ui {
class IHMDanger;
}
QT_END_NAMESPACE

class IHMDanger : public QMainWindow
{
    Q_OBJECT

public:
    IHMDanger(QWidget *parent = nullptr);
    ~IHMDanger();
    bool estCarteBranly;

private slots:

    void reception();

    void autoScroll();

    void on_Timeout();

    void on_pushButton_com_connexion_clicked();

    void on_pushButton_com_deconnexion_clicked();

    void on_pushButton_signal_led1_clicked();

    void on_pushButton_signal_led2_clicked();

    void on_pushButton_signal_afficher_clicked();

    void on_pushButton_signal_eteindre_clicked();

    void on_pushButton_horo_stop_clicked();

    void on_pushButton_horo_effacer_clicked();

    void on_pushButton_wd_sauvegarder_clicked();

    void on_pushButton_wd_quitter_clicked();

    void on_checkBox_signal_carte_branly_stateChanged(int arg1);

    void on_comboBox_horo_type_affichage_currentTextChanged(const QString &arg1);

private:
    Ui::IHMDanger *ui;
    GestionPortSerie *GestPorts;
    QTimer *timer;
    QIntValidator *valid;

    bool isTimerStarted;
    bool isAffichageAmeliore;
};
#endif // IHMDANGER_H
