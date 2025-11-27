#ifndef IHMDANGER_H
#define IHMDANGER_H

#include <QMainWindow>
#include "gestionportserie.h"

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

private:
    Ui::IHMDanger *ui;
    GestionPortSerie *GestPorts;
    QTimer *timer;

    bool isTimerStarted;
};
#endif // IHMDANGER_H
