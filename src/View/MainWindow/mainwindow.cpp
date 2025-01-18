#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"

#include "basic_plot.h"
#include "waves_plot.h"
#include "scatter_plot.h"

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
    //ui->START->setIcon(QIcon(":/icons/play.png"));
    //ui->Config->setIcon(QIcon("C:/Projects/ecg_app_GUI/src/View/MainWindow/icons/gear.png"));
    //ui->START->setIconSize(QSize(50, 50));

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
    fileReader.write_measured_time(); 
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
            "Raw ECG Signal", "Hajlajts","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");

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
        plotWidget->updateBasicPlot(signal, highlights, "MLII Signal","Indeksy","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        break;
    }
    default:
        break;
    }
}

void MainWindow::on_checkBoxRP_stateChanged(int state)
{
    if (state == Qt::Checked) {
        try {
            qDebug() << "Starting R-peaks detection...";
            
            // Get and filter the signal
            Signal inputSignal = fileReader.read_MLII();
            Signal filteredSignal = baseline.filterSignal(inputSignal);
            
            // Detect R-peaks using Pan-Tompkins
            std::vector<int> peaks;
            rPeaks.setParams("PAN_TOMPKINS", 3, 0.018);
            // rPeaks.setParams("HILBERT", 200, 1.5, static_cast<int>(0.8 * inputSignal.getSamplingRate()));
            
            if (rPeaks.detectRPeaks(filteredSignal.getY(), inputSignal.getSamplingRate(), peaks)) {
                qDebug() << "R-peaks detection successful";
                qDebug() << "Total peaks detected:" << peaks.size();
                
                // Convert peaks to QList
                r_peak_positions.clear();
                r_peak_positions.reserve(peaks.size());
                for (const auto& peak : peaks) {
                    r_peak_positions.append(peak);
                }

                // Update the plot
                QLayout* layout = ui->frame_2->layout();
                if (!layout) {
                    layout = new QVBoxLayout(ui->frame_2);
                    ui->frame_2->setLayout(layout);
                }

                // Clear existing widgets
                QLayoutItem* child;
                while ((child = layout->takeAt(0)) != nullptr) {
                    delete child->widget();
                    delete child;
                }

                // Create new plot with R-peaks highlighted
                Basic_Plot* plotWidget = new Basic_Plot();
                layout->addWidget(plotWidget);
                plotWidget->updateBasicPlot(filteredSignal, r_peak_positions,
                    "Moving Mean Filtered ECG Signal", "RPeaks", "ECG Signal (MLII)", 
                    "Time [s]", "Voltage [mV]");
            }
        } catch (const std::exception& e) {
            qDebug() << "Error during R-peaks detection:" << e.what();
            ui->checkBoxRP->setChecked(false);
        }
    } else {
        // When unchecked, clear R-peaks and redraw
        r_peak_positions.clear();
        
        try {
            Signal inputSignal = fileReader.read_MLII();
            Signal filteredSignal = baseline.filterSignal(inputSignal);
            
            QLayout* layout = ui->frame_2->layout();
            if (!layout) {
                layout = new QVBoxLayout(ui->frame_2);
                ui->frame_2->setLayout(layout);
            }

            // Clear existing widgets
            QLayoutItem* child;
            while ((child = layout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }

            // Create new plot without R-peaks
            Basic_Plot* plotWidget = new Basic_Plot();
            layout->addWidget(plotWidget);
            QList<int> empty_highlights;
            plotWidget->updateBasicPlot(filteredSignal, empty_highlights,
                "Moving Mean Filtered ECG Signal", "Indeksy","ECG Signal (MLII)", 
                "Time [s]", "Voltage [mV]");
        } catch (const std::exception& e) {
            qDebug() << "Error updating plot:" << e.what();
        }
    }
}

void MainWindow::on_checkBoxQRS_stateChanged(int state)
{
    if (state == Qt::Checked) {
        try {
            qDebug() << "Starting wave detection...";
            
            // Get and filter the signal
            Signal inputSignal = fileReader.read_MLII();
            Signal filteredSignal = baseline.filterSignal(inputSignal);
            
            // First detect R-peaks if not already detected
            if (r_peak_positions.isEmpty()) {
                std::vector<int> peaks;
                rPeaks.setParams("PAN_TOMPKINS", 15, 0.3);
                if (!rPeaks.detectRPeaks(filteredSignal.getY(), inputSignal.getSamplingRate(), peaks)) {
                    throw std::runtime_error("R-peaks detection failed");
                }
                r_peak_positions.reserve(peaks.size());
                for (const auto& peak : peaks) {
                    r_peak_positions.append(peak);
                }
            }

            // Create Waves detector and process
            Waves waveDetector(filteredSignal, r_peak_positions);
            if (!waveDetector.detectWaves()) {
                throw std::runtime_error("Wave detection failed");
            }

            // Update the plot
            QLayout* layout = ui->frame_2->layout();
            if (!layout) {
                layout = new QVBoxLayout(ui->frame_2);
                ui->frame_2->setLayout(layout);
            }

            // Clear existing widgets
            QLayoutItem* child;
            while ((child = layout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }

            // Create new waves plot
            Waves_Plot* plotWidget = new Waves_Plot();
            layout->addWidget(plotWidget);
            
            // Update plot with all wave components
            plotWidget->updateWavesPlot(
                filteredSignal,
                "ECG Signal",                               // Main signal legend
                waveDetector.getQRSOnsets(), "QRS Onset",   // First highlight set
                waveDetector.getQRSEnds(), "QRS End",       // Second highlight set
                waveDetector.getPOnsets(), "P Onset",       // Third highlight set
                waveDetector.getPEnds(), "P End",           // Fourth highlight set
                waveDetector.getTEnds(), "T End",           // Fifth highlight set
                "ECG Signal with Wave Components",          // Title
                "Time [s]",                                 // X-axis label
                "Voltage [mV]"                              // Y-axis label
            );
            
        } catch (const std::exception& e) {
            qDebug() << "Error during wave detection:" << e.what();
            ui->checkBoxQRS->setChecked(false);
            QMessageBox::warning(this, "Error", 
                QString("Failed to detect waves: %1").arg(e.what()));
        }
    } else {
        // When unchecked, clear highlights and redraw
        try {
            Signal inputSignal = fileReader.read_MLII();
            Signal filteredSignal = baseline.filterSignal(inputSignal);
            
            QLayout* layout = ui->frame_2->layout();
            if (!layout) {
                layout = new QVBoxLayout(ui->frame_2);
                ui->frame_2->setLayout(layout);
            }

            // Clear existing widgets
            QLayoutItem* child;
            while ((child = layout->takeAt(0)) != nullptr) {
                delete child->widget();
                delete child;
            }

            // Create new waves plot without highlights
            Waves_Plot* plotWidget = new Waves_Plot();
            layout->addWidget(plotWidget);
            
            // Update plot with no highlights
            plotWidget->updateWavesPlot(
                filteredSignal,
                "ECG Signal",              // Main signal legend
                QVector<int>(), "",        // Empty highlight sets
                QVector<int>(), "",
                QVector<int>(), "",
                QVector<int>(), "",
                QVector<int>(), "",
                "Filtered ECG Signal",     // Title
                "Time [s]",                // X-axis label
                "Voltage [mV]"             // Y-axis label
            );
        } catch (const std::exception& e) {
            qDebug() << "Error updating plot:" << e.what();
            QMessageBox::warning(this, "Error", 
                QString("Failed to update plot: %1").arg(e.what()));
        }
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

        // Create and apply Moving Mean filter
        auto movingMeanFilter = std::make_unique<MovingMeanFilter>();
        movingMeanFilter->set(3);  // window length of 15 points
        progress.setValue(40);

        try {
            baseline.setFilter(std::move(movingMeanFilter));
            Signal filteredSignal = baseline.filterSignal(inputSignal);
            progress.setValue(60);

            if (filteredSignal.getY().empty()) {
                QMessageBox::warning(this, "Warning", "Filtering resulted in empty signal!");
                return;
            }

            // Use current R-peaks if they exist
            QList<int> highlights = r_peak_positions;

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
            filteredPlotWidget->updateBasicPlot(filteredSignal, highlights,
                "Moving Mean Filtered ECG Signal", "QRS","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
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