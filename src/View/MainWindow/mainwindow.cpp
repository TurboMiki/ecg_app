#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"

#include "basic_plot.h"


#include "MovingMeanFilter.h"
#include "ButterworthFilter.h"
#include "SavitzkyGolayFilter.h"
#include "LMSFilter.h"

#include <algorithm>
#include <QDebug>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QMessageBox>
#include <exception>
#include <QProgressDialog>
#include <QElapsedTimer>

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
    fileReader.readFile();
}

void MainWindow::on_btnRaw_clicked()
{
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Get the input signal from file reader
        Signal inputSignal = fileReader.read_MLII();
        
        // Ensure frame_2 has a layout
        QLayout* layout = ui->frame_2->layout();
        if (!layout) {
            layout = new QVBoxLayout(ui->frame_2);
            ui->frame_2->setLayout(layout);
        }

        // Clear any existing widgets in the layout
        QLayoutItem* child;
        while ((child = layout->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }

        // Create and add plot
        Basic_Plot* plotWidget = new Basic_Plot();
        layout->addWidget(plotWidget);
        QVector<int> highlights;
        plotWidget->updateBasicPlot(inputSignal, highlights, 
            "Raw ECG Signal", "ECG Signal (MLII)", "Time [s]", "Voltage [mV]");

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process data: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing the data.");
    }
}

void MainWindow::on_pushButton_clicked()
{
    QWidget *newWindow = new QWidget();
    newWindow->setWindowTitle("Layout in New Window");

    // Check if ui->frame_2 has a layout
    QLayout *layout = new QVBoxLayout(newWindow);
    createPlot(layout,PLOT_TYPE::RAW_PLOT);
    newWindow->resize(800,500);
    newWindow->show();
}

void MainWindow::createPlot(QLayout* layout,PLOT_TYPE plotType){
    switch (plotType) {
    case PLOT_TYPE::RAW_PLOT:{
        Basic_Plot* plotWidget = new Basic_Plot();
        Signal signal = fileReader.read_MLII();
        QVector<int> highlights;

        layout->addWidget(plotWidget);
        // Verify the data
        if(signal.getX().empty() || signal.getY().empty()) {
            QMessageBox::warning(this, "Error", "No data available to plot! Signal is empty.");
            return;
        }
        //
        plotWidget->updateBasicPlot(signal, highlights, "MLII Signal", "ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        break;
    }
    default:
        break;
    }
}

void MainWindow::on_btnFECG_clicked()
{
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing signal...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);
        
        // Get the input signal
        Signal inputSignal = fileReader.read_MLII();
        progress.setValue(20);
        
        if (inputSignal.getY().empty()) {
            QMessageBox::warning(this, "Warning", "Input signal is empty!");
            return;
        }

        // Create LMS filter
        auto lmsFilter = std::make_unique<LMSFilter>();
        progress.setValue(40);

        try {
            // Create a reference signal (in this case, let's use a delayed version of the input)
            Signal refSignal = inputSignal;  // Create copy
            std::vector<double> refY = refSignal.getY();
            
            // Shift the reference signal by a few samples
            const int delay = 5;
            std::rotate(refY.begin(), refY.begin() + delay, refY.end());
            refSignal.setY(refY);
            
            // Apply adaptive filtering
            Signal filteredSignal = lmsFilter->adaptiveFilter(inputSignal, refSignal);
            progress.setValue(60);

            if (filteredSignal.getY().empty()) {
                QMessageBox::warning(this, "Warning", "Filtering resulted in empty signal!");
                return;
            }

            // Ensure frame_2 has a layout
            QLayout* layout = ui->frame_2->layout();
            if (!layout) {
                layout = new QVBoxLayout(ui->frame_2);
                ui->frame_2->setLayout(layout);
            }
            progress.setValue(80);

            // Clear any existing widgets in the layout
            QLayoutItem* child;
            while ((child = layout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }

            // Create and add filtered signal plot
            Basic_Plot* filteredPlotWidget = new Basic_Plot();
            layout->addWidget(filteredPlotWidget);
            QVector<int> highlights;
            filteredPlotWidget->updateBasicPlot(filteredSignal, highlights,
                "LMS Filtered ECG Signal", "ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
            progress.setValue(100);

        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", 
                QString("Filter application failed: %1").arg(e.what()));
            return;
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process data: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing the data.");
    }
}