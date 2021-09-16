#include "resultanalisator.h"
#include "ui_resultanalisator.h"


#define version "1.0"

ResultAnalisator::ResultAnalisator(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ResultAnalisator)
{
    ui->setupUi(this);
    ResultAnalisator::setWindowTitle("Анализатор результатов выгрузки ПД в ГИС ЖКХ");
    connect(this->ui->statusbar, &QStatusBar::messageChanged, this, &ResultAnalisator::messageChanged);
}

ResultAnalisator::~ResultAnalisator()
{
    delete ui;
}

void ResultAnalisator::on_action_triggered()
{
    QMessageBox::about(this, "О Программе", "Гулюкин О.В. 2021 \nПрограмма для анализа результатов\nзагрузки платежных документов\nшаблонами Exel ГИС ЖКХ\nВерсия " + QString(version));
}


void ResultAnalisator::on_action_Qt_triggered()
{
    QApplication::aboutQt();
}


void ResultAnalisator::on_accountsButton_clicked()
{
    accountFile = QFileDialog::getOpenFileName(this, "Open File",
                                               QDir::currentPath(),
                                               tr("xlsx files (*.xlsx);;All files (*.*)"));
    ui->accountsLabel->setText(accountFile);
}


void ResultAnalisator::on_gisFilesButton_clicked()
{
    gisFiles = QFileDialog::getOpenFileNames(this, "Open File",
                                             QDir::currentPath(),
                                             tr("xlsx files (*.xlsx);;All files (*.*)"));
    QString temp = "";
    for(auto it = gisFiles.begin(); it != gisFiles.end(); it++)
    {
        temp += *it + "\n";
    }
    ui->gisFilesLabel->setText(temp);
}


void ResultAnalisator::on_resultPathButton_clicked()
{
    resultPath = QFileDialog::getExistingDirectory(this, "Open Directory", QDir::currentPath(),
                                                   QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->resultPathLabel->setText(resultPath);
}


void ResultAnalisator::on_goButton_clicked()
{
    organisation = ui->organisationLineEdit->text();
    emptyZKHNumbers = 0;
    if(organisation.isEmpty())
    {
        QMessageBox::information(this, "Ошибка", "Вы не ввели название организации!");
        return;
    }
    if(accountFile.isEmpty())
    {
        QMessageBox::information(this, "Ошибка", "Вы не выбрали файл с лицевыми счетами!");
        return;
    }
    if(gisFiles.count() == 0)
    {
        QMessageBox::information(this, "Ошибка", "Вы не выбрали файлы с результатами с ГИС ЖКХ!");
        return;
    }
    if(resultPath.isEmpty())
    {
        QMessageBox::information(this, "Ошибка", "Вы не выбрали путь для сохранения результатов!");
        return;
    }
    ui->statusbar->showMessage("Загружаем данные по лицевым счетам");
    bool resultCheck = fileIO::loadAccounts(&accountNumbers, &accountAdresses, accountFile);
    if(!resultCheck)
    {
        QMessageBox::information(this, "Ошибка", "не удалось загрузить информацию\nс лицевыми счетами!");
        return;
    }

    ui->statusbar->showMessage("Загружаем данные из ГИС ЖКХ");
}

void ResultAnalisator::messageChanged(const QString &message)
{
    this->repaint();
    bool resultCheck = false;
    if (message == "Загружаем данные из ГИС ЖКХ")
    {
        for(auto it = gisFiles.begin(); it != gisFiles.end(); it++)
        {
            resultCheck = fileIO::loadGis(&accountNumbers, &gisResults, period, *it, emptyZKHNumbers);
            if(!resultCheck)
            {
                QMessageBox::information(this, "Ошибка", "не удалось загрузить информацию\nиз файла ГМС ЖКХ!");
                return;
            }
        }
        ui->statusbar->showMessage("Кое что обрабатываем");
    }else if (message == "Кое что обрабатываем")
    {
        fillGisResult();
        ui->statusbar->showMessage("Формируем итоговый файл");
    }else if (message == "Формируем итоговый файл")
    {
        resultCheck = fileIO::saveResult(&gisResults, period, resultPath, organisation, emptyZKHNumbers);
        ui->statusbar->showMessage("Готово!");
        //emit progressMSG("Готово!");
        if(!resultCheck)
        {
            QMessageBox::information(this, "Ошибка", "не удалось записать информацию\nв результирующий файл");
            return;
        }else
        {
            QMessageBox::information(this, "Успех", "Формирование файла результата\nзавершено\nобнаружено " + QString::number(emptyZKHNumbers) + " ЛС без кодов ГИС");
        }
        clearData();
    }
}

void ResultAnalisator::fillGisResult()
{
    for (auto it = gisResults.begin(); it != gisResults.end(); it++)
    {
        it.value()->adress = accountAdresses.value(it.key());
    }
}

void ResultAnalisator::clearData()
{
    accountNumbers.clear();
    accountAdresses.clear();
    gisResults.clear();
    accountFile = "";
    emptyZKHNumbers = 0;
    gisFiles.clear();
    ui->accountsLabel->setText("Выберите файл");
    ui->gisFilesLabel->setText("Выберите файлы");
}
