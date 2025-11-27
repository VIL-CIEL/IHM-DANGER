#ifndef GESTIONPORTSERIE_H
#define GESTIONPORTSERIE_H

#include <QObject>
#include "QtSerialPort/QSerialPort"
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>

class GestionPortSerie : public QObject
{
    Q_OBJECT
public:
    explicit GestionPortSerie(QString, QObject *parent = nullptr);
    ~GestionPortSerie();
    QSerialPort *portConnexion;

    static QByteArray trameExtraite;
    bool isInTrame;

    void reception(QByteArray*, QString*, bool);
    QString envoieTrame(QString);

signals:
};

#endif // GESTIONPORTSERIE_H
