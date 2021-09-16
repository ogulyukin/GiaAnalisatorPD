#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextCodec>
#include <QPair>
#include "reportstring.h"
#include "detailsstr.h"

class fileIO : public QObject
{
    Q_OBJECT
public:
    explicit fileIO(QObject *parent = nullptr);
    static bool loadAccounts(QHash<QString, QString>* accountNumbers, QHash<QString, QString>* accountAdresses, QString filename);
    static bool loadGis(QHash<QString, QString>* accountNumbers, QMap<QString, ReportString*>* gisResults, QString& period, QString filename, int& emptyStr);
    static bool saveResult(QMap<QString, ReportString*>* gisResults, QString& period, QString resultPath, QString organisation, int& emptyStr);

private:
    static void fillRowsOnError(ReportString* rstr, QString numberPD, QMultiHash<QString, detailsStr*> *details);
    static QString findNumber(QHash<QString, QString>* accountNumbers, QString zkhNumber);

signals:
};

#endif // FILEIO_H
