#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"
#include "basic_plot.h"
#include <QDebug>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>
#include <exception>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , plotWidget(nullptr)
{
    ui->setupUi(this);

    // Set icons
    ui->START->setIcon(QIcon(":/icons/play.png"));
    ui->Config->setIcon(QIcon(":/icons/gear.png"));

    // Initialize SettingsForm
    ptrSettingsForm = new SettingsForm(this);

    // Connect signals and slots
    connect(ptrSettingsForm, &SettingsForm::pass_values, this, &MainWindow::get_settings);
    connect(this, &MainWindow::requestData, ptrSettingsForm, &SettingsForm::pass_values);
}

MainWindow::~MainWindow()
{
    delete ptrSettingsForm;
    if (plotWidget) {
        delete plotWidget;
    }
    delete ui;
}

void MainWindow::on_START_clicked()
{
    // Implementation for START button
}

void MainWindow::on_Config_clicked()
{
    if (ptrSettingsForm) {
        ptrSettingsForm->show();
    }
}

void MainWindow::get_settings(const QStringList &data)
{
    write_settings(data);
}

void MainWindow::write_settings(const QStringList &data)
{
    this->parameter1 = data[0];
    this->parameter2 = data[1];
    this->parameter3 = data[2];
    this->parameter4 = data[3];
    this->parameter5 = data[4];
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
    QString fileFilter = "All Supported Files (*.dat *.csv *.xlsx);;Data Files (*.dat);;CSV Files (*.csv);;Excel Files (*.xlsx)";
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
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        fileReader.setPath(ui->linePath->text().toStdString());
        fileReader.readFile();
        
        // Create the plot if it doesn't exist
        if (!plotWidget) {
            plotWidget = new Basic_Plot(ui->frame_2);
            
            // Create a QVBoxLayout for frame_2 if it doesn't have one
            if (!ui->frame_2->layout()) {
                QVBoxLayout* layout = new QVBoxLayout(ui->frame_2);
                layout->setContentsMargins(0, 0, 0, 0);
                ui->frame_2->setLayout(layout);
            }
            
            // Add the plot to the layout
            ui->frame_2->layout()->addWidget(plotWidget);
        }

        // Get the MLII signal
        Signal signal = fileReader.read_MLII();
        
        // Verify the data
        if(signal.getX().empty() || signal.getY().empty()) {
            QMessageBox::warning(this, "Error", "No data available to plot! Signal is empty.");
            return;
        }

        // Debug output
        qDebug() << "Signal size X:" << signal.getX().size();
        qDebug() << "Signal size Y:" << signal.getY().size();
        if (!signal.getX().empty() && !signal.getY().empty()) {
            qDebug() << "First X value:" << signal.getX().front();
            qDebug() << "First Y value:" << signal.getY().front();
        }

        QVector<int> highlights;
        plotWidget->updateBasicPlot(signal, highlights, "MLII Signal", "ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process data: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing the data.");
    }
}