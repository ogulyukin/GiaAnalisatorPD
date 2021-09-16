#ifndef DETAILSSTR_H
#define DETAILSSTR_H

#include <QString>

class detailsStr
{
public:
    detailsStr(QString service, QString ammount, QString status);
    QString service;
    QString ammount;
    QString status;
};

#endif // DETAILSSTR_H
