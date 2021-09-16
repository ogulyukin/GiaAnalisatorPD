#ifndef REPORTSTRING_H
#define REPORTSTRING_H

#include <QString>
#include <QList>
#include "detailsstr.h"

class ReportString
{
public:
    ReportString(QString zkhNumber, QString status);
    QString zkhNumber;
    QString adress;
    QString status;
    QList<detailsStr> rows;
    bool operator<(ReportString &other);
    bool operator>(ReportString &other);

private:
    QString getStreet(QString fullAdress);
    QString getHouse(QString fullAdress);
    int getFlat(QString fullAdress);
};

#endif // REPORTSTRING_H
