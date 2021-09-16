#include "fileio.h"
#include "xlsxdocument.h"
#include "xlsxrichstring.h"

fileIO::fileIO(QObject *parent) : QObject(parent)
{

}

bool fileIO::loadAccounts(QHash<QString, QString> *accountNumbers, QHash<QString, QString> *accountAdresses, QString filename)
{
    //    QTextCodec *codec = QTextCodec::codecForName("cp1251");; //set codec for WIN coding
    //    QTextCodec::setCodecForLocale(codec);
    QXlsx::Document doc(filename);
    if (!doc.load())
    {
        qDebug() << "Невозможно открыть файл с лицевыми счетами!";
        return false;
    }
    int xCount = 2;
    QString numberAcc = doc.read(xCount,1).toString();
    QString numberAcc2 = doc.read(xCount,3).toString();
    QString adressAcc = doc.read(xCount,5).toString();
    while(numberAcc != "")
    {
        accountNumbers->insert(numberAcc, numberAcc2);
        accountAdresses->insert(numberAcc, adressAcc);
        xCount++;
        numberAcc = doc.read(xCount,1).toString();
        numberAcc2 = doc.read(xCount,3).toString();
        adressAcc = doc.read(xCount,5).toString();
        //qDebug() << "Загрузка данных ЛС: " << numberAcc << " " << numberAcc2 << " " << adressAcc;
    }
    if(accountNumbers->size() > 0 )
        return true;
    return false;
}

bool fileIO::loadGis(QHash<QString, QString>* accountNumbers, QMap<QString, ReportString*> *gisResults, QString& period, QString filename, int& emptyStr)
{
    QXlsx::Document doc(filename);
    if (!doc.load())
    {
        qDebug() << "Невозможно открыть файл ГИС ЖКХ!";
        return false;
    }
    doc.selectSheet("Разделы 3-6");
    int xCount = 5;
    int resColumn = 0;
    for (int i = 20; i < 50; i++)
    {
        if(doc.read(1, i ).toString() == "Статус обработки")
        {
            resColumn = i;
            i = 100;
        }
    }
    int ammountCol = 0;
    for(int i = 5; i < 50; i++)
    {
        if(doc.read(2, i ).toString() == "Всего")
        {
            ammountCol = i;
            i = 100;
        }
    }
    QString number = doc.read(xCount,1).toString();
    QMultiHash<QString, detailsStr*> details;
    while(number != "")
    {
        detailsStr* detail = new detailsStr(doc.read(xCount,2).toString(), doc.read(xCount, ammountCol).toString(),
                                            doc.read(xCount, resColumn).toString());
        details.insert(number, detail);
        xCount++;
        number = doc.read(xCount,1).toString();
    }
    doc.selectSheet("Разделы 1-2");
    resColumn = 0;
    for (int i = 20; i < 50; i++)
    {
        if(doc.read(1, i ).toString() == "Статус обработки")
        {
            resColumn = i;
            i = 100;
        }
    }
    qDebug() << "Определение колонки с результатом: " << resColumn;
    xCount = 4;
    QString numberAcc = doc.read(xCount,1).toString();
    QString status = doc.read(xCount, resColumn).toString();
    period = doc.read(xCount, 4).toString();
    QString numberPD = doc.read(xCount, 3).toString();
    while(numberPD != "")
    {
        if(numberAcc != "")
        {
            ReportString *rstr = new ReportString(numberAcc, status);
            if(status != "OK")
                fillRowsOnError(rstr, numberPD, &details);
            gisResults->insert(accountNumbers->key(numberAcc), rstr);
        }else
        {
            emptyStr++;
        }
        xCount++;
        numberAcc = doc.read(xCount,1).toString();
        status = doc.read(xCount, resColumn).toString();
        numberPD = doc.read(xCount, 3).toString();
        qDebug() << "Загрузка данных ГИС: " << numberAcc << " " << status;
    }
    qDebug() << "ЛС без кодов ЕЛС: " << emptyStr;
    if(gisResults->size() > 0 )
        return true;
    return false;
}


void fileIO::fillRowsOnError(ReportString *rstr, QString numberPD, QMultiHash<QString, detailsStr*> *details)
{
    if(numberPD.length() < 15)
    {
        rstr->rows.append(detailsStr("Ошибка номера", numberPD, "Неверный номер ПД"));
        return;
    }

    for(auto it = details->begin(); it != details->end(); it++)
    {
        if(it.key() == numberPD)
        {
            if(it.value()->status != "")
            {
                rstr->rows.append(*it.value());
            }
        }
    }
}

bool fileIO::saveResult(QMap<QString, ReportString*>* gisResults, QString& period, QString resultPath, QString organisation, int& emptyStr)
{
    resultPath += QDir::separator() + organisation + period.remove('.') + ".xlsx";
    qDebug() << "ResultPah: " << resultPath;
    QXlsx::Document doc;
    QXlsx::Format fontFormat;
    fontFormat.setFontSize(20);
    fontFormat.setFontBold(true);
    QXlsx::RichString mStr1;
    mStr1.addFragment(organisation, fontFormat);
    mStr1.addFragment(" - " + period, fontFormat);
    doc.write(1,1, mStr1);
    doc.write(2,1, ("Всего обработано: " + QString::number(gisResults->count())));
    doc.write(2,3, ("Обнаружено " + QString::number(emptyStr) + " лицевых счетов без идентификатора ЖКУ."));
    int rowCounter = 4;
    fontFormat.setFontSize(12);
    fontFormat.setFontBold(false);
    for (auto it = gisResults->begin(); it != gisResults->end(); it++)
    {
        fontFormat.setFontItalic(false);
        if(it.value()->status == "OK")
        {
            fontFormat.setFontColor(Qt::darkGreen);
        }else
        {
            fontFormat.setFontColor(Qt::darkRed);
        }
        doc.write(rowCounter, 1, it.key(), fontFormat);
        doc.write(rowCounter, 2, it.value()->zkhNumber);
        doc.write(rowCounter, 3, it.value()->adress);
        QXlsx::RichString mStrRep;
        mStrRep.addFragment(it.value()->status, fontFormat);
        doc.write(rowCounter, 4, mStrRep);
        qDebug() << rowCounter << ". Записываю: " << it.key() << " " << it.value()->status;
        for(auto it2 = it.value()->rows.begin(); it2 != it.value()->rows.end();it2++)
        {
            fontFormat.setFontItalic(true);
            fontFormat.setFontColor(Qt::blue);
            rowCounter++;
            QXlsx::RichString mStrDet1;
            QXlsx::RichString mStrDet2;
            QXlsx::RichString mStrDet3;
            mStrDet1.addFragment(it2->service, fontFormat);
            mStrDet2.addFragment(it2->ammount, fontFormat);
            mStrDet3.addFragment(it2->status, fontFormat);
            doc.write(rowCounter, 1, mStrDet1);
            doc.write(rowCounter, 2, mStrDet2);
            doc.write(rowCounter, 4, mStrDet3);
            qDebug() << rowCounter << ". Записываю деталировку: " << it2->service << " " << it2->status;
        }
        rowCounter++;
        //qDebug() << "В процессе: " << rowCounter;
    }
    doc.setRowHeight(1,20);
    doc.setColumnWidth(1,20);
    doc.setColumnWidth(2,20);
    doc.setColumnWidth(3,100);
    if(!doc.saveAs(resultPath))
    {
        qDebug() << "Unable to save file: " << resultPath;
        return false;
    }

    return true;
}
