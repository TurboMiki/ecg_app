#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"
#include "basic_plot.h"
#include "histogram_plot.h"
#include "waves_plot.h"

#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    //, plotWidget(nullptr)
{
    ui->setupUi(this);

    // Ustawienie ikon
    ui->START->setIcon(QIcon(":/icons/play.png"));
    ui->Config->setIcon(QIcon(":/icons/gear.png"));

    // Inicjalizacja SettingsForm
    ptrSettingsForm = new SettingsForm(this);

    // Połączenia sygnałów i slotów
    connect(ptrSettingsForm, &SettingsForm::pass_values, this, &MainWindow::get_settings);
    connect(this, &MainWindow::requestData, ptrSettingsForm, &SettingsForm::pass_values);
    //connect(ui->START, &QPushButton::clicked, this, &MainWindow::on_START_clicked);

    // Plots
    connect(ui->btnRaw, &QPushButton::clicked, this, &MainWindow::on_btnRaw_clicked);

}

MainWindow::~MainWindow()
{
    delete ptrSettingsForm;
    /*
    if (plotWidget) {
        delete plotWidget;
    }
    */
    delete ui;
}

void MainWindow::on_START_clicked()
{
    // tu przekazanie parametrów filtrów i uruchomienie algorytmu
}

void MainWindow::on_Config_clicked()
{
    if (ptrSettingsForm) {
        ptrSettingsForm->show(); // Pokaż okno ustawień
    }
}

void MainWindow::get_settings(const QStringList &data)
{
    /*
    if (data.size() >= 5) {
        parameter1 = data[0];
        parameter2 = data[1];
        parameter3 = data[2];
        parameter4 = data[3];
        parameter5 = data[4];

        qDebug() << "Settings received:";
        qDebug() << "Parameter 1:" << parameter1;
        qDebug() << "Parameter 2:" << parameter2;
        qDebug() << "Parameter 3:" << parameter3;
        qDebug() << "Parameter 4:" << parameter4;
        qDebug() << "Parameter 5:" << parameter5;
    }
    */
    write_settings(data);
}

void MainWindow::write_settings(const QStringList &data)
{
    this->parameter1=data[0];
    this->parameter2=data[1];
    this->parameter3=data[2];
    this->parameter4=data[3];
    this->parameter5=data[4];
    //debug_settings();
}

void MainWindow::debug_settings()
{
    qDebug() << this->parameter1;
    qDebug() << this->parameter2;
    qDebug() << this->parameter3;
    qDebug() << this->parameter4;
    qDebug() << this->parameter5;
}

void MainWindow::on_btnPath_clicked()
{
    QString fileFilter = "Excel Files (*.xlsx)";
    QString selectedFile = QFileDialog::getOpenFileName(this, "Choose file", QDir::homePath(), fileFilter);

    if (!selectedFile.isEmpty())
    {
        this->filePath = selectedFile;
        ui->linePath->setText(this->filePath);
    }
    fileReader.setPath(ui->linePath->text().toStdString());
}


void MainWindow::on_btnRaw_clicked()
{
    fileReader.readFile();
    // Basic_Plot rawPlot = Basic_Plot(ui->frame_2);
    // QVector<int> Highlight = {};
    // rawPlot.updateBasicPlot(fileReader.read_V(),Highlight,"Raw","Raw","x","y");
    /*
    if (!plotWidget) {
        plotWidget = new Basic_Plot(this);
        plotWidget->setGeometry(100, 100, 400, 300);
        plotWidget->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        plotWidget->setAttribute(Qt::WA_DeleteOnClose, false);
        plotWidget->show();
    }
    */

}


