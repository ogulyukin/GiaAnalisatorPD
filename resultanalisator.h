#ifndef RESULTANALISATOR_H
#define RESULTANALISATOR_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFileDialog>
#include "fileio.h"
#include "reportstring.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ResultAnalisator; }
QT_END_NAMESPACE

class ResultAnalisator : public QMainWindow
{
    Q_OBJECT

public:
    ResultAnalisator(QWidget *parent = nullptr);
    ~ResultAnalisator();
signals:

public slots:
    void messageChanged(const QString &message);

private slots:
    void on_action_triggered();

    void on_action_Qt_triggered();

    void on_accountsButton_clicked();

    void on_gisFilesButton_clicked();

    void on_resultPathButton_clicked();

    void on_goButton_clicked();

private:
    int emptyZKHNumbers;
    QString accountFile;
    QStringList gisFiles;
    QString resultPath;
    QString organisation;
    QString period;
    QHash<QString, QString> accountNumbers;
    QHash<QString, QString> accountAdresses;
    QMap<QString, ReportString*> gisResults;
    Ui::ResultAnalisator *ui;
    void fillGisResult();
    void clearData();
};
#endif // RESULTANALISATOR_H
