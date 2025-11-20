#ifndef IHMDANGER_H
#define IHMDANGER_H

#include <QMainWindow>
#include "QtSerialPort/QSerialPort"

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

    void on_pushButton_com_connexion_clicked();

    void on_pushButton_com_deconnexion_clicked();

    void envoieTrame(QString);

    void on_pushButton_signal_led1_clicked();

    void on_pushButton_signal_led2_clicked();

    void on_pushButton_signal_afficher_clicked();

    void on_pushButton_signal_eteindre_clicked();

private:
    Ui::IHMDanger *ui;
    QSerialPort *portConnexion;

    bool isPortOpen;
    static QByteArray trameExtraite;
    bool isInTrame;
};
#endif // IHMDANGER_H
