#include "reportstring.h"

ReportString::ReportString(QString zkhNumber, QString status): zkhNumber(zkhNumber), status(status)
{

}

bool ReportString::operator>(ReportString &other)
{
    if(getStreet(this->adress) != getStreet(other.adress))
    {
        return getStreet(this->adress) > getStreet(other.adress);
    }else if(getHouse(this->adress) != getHouse(other.adress))
    {
        return getHouse(this->adress) > getHouse(other.adress);
    }else
    {
        return getFlat(this->adress) > getFlat(other.adress);
    }
}

bool ReportString::operator<(ReportString &other)
{
    if(getStreet(this->adress) != getStreet(other.adress))
    {
        return getStreet(this->adress) < getStreet(other.adress);
    }else if(getHouse(this->adress) != getHouse(other.adress))
    {
        return getHouse(this->adress) < getHouse(other.adress);
    }else
    {
        return getFlat(this->adress) < getFlat(other.adress);
    }
}

QString ReportString::getStreet(QString fullAdress)
{
    QStringList list = fullAdress.split(',');
    foreach(QString i, list)
    {
        if(i.startsWith(" ул ") || i.startsWith(" пер "))
            return i;
    }
    return "ERRORS";
}

QString ReportString::getHouse(QString fullAdress)
{
    QStringList list = fullAdress.split(',');
    QString result = "ERRORH";
    foreach(QString i, list)
    {
        if(i.startsWith(" д. "))
            result = i;
        if(result != "ERRORH" && i.startsWith(" к. "))
            result += "," + i;
    }
    return result;
}

int ReportString::getFlat(QString fullAdress)
{
    QStringList list = fullAdress.split(',');
    QString resultStr = list.last();
    resultStr.remove(0,5);
    return resultStr.toInt();
}
