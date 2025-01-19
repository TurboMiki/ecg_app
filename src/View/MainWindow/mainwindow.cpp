#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"

#include "basic_plot.h"
#include "waves_plot.h"
#include "scatter_plot.h"
#include "histogram_plot.h"

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
    fileReader.set_path(ui->linePath->text().toStdString());
    fileReader.read_file();
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

void MainWindow::on_btnHRV_1_clicked()
{
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing HRV...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);

        // Get the ECG signal
        Signal inputSignal = fileReader.read_MLII();
        progress.setValue(20);

        // Filter the signal
        Signal filteredSignal = baseline.filterSignal(inputSignal);
        progress.setValue(40);

        // Detect R-peaks if not already detected
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
        progress.setValue(60);

        // Create R-peaks signal for HRV analysis
        std::vector<double> peakTimes;
        for (int idx : r_peak_positions) {
            peakTimes.push_back(filteredSignal.getX()[idx]);
        }
        Signal rPeaksSignal(peakTimes, std::vector<double>(peakTimes.size(), 1.0), 
                           filteredSignal.getSamplingRate());

        // Process HRV
        HRV_1 hrvAnalyzer(rPeaksSignal, filteredSignal);
        hrvAnalyzer.process();
        progress.setValue(80);

        // Get results and display them
        auto timeParams = hrvAnalyzer.getTimeParams();
        auto freqParams = hrvAnalyzer.getFreqParams();
        displayHRVResults(timeParams, freqParams);
        
        progress.setValue(100);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process HRV: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing HRV.");
    }
}

void MainWindow::on_btnHRV2_PC_clicked()
{
    qDebug() << "Starting HRV2 Poincaré plot analysis...";

    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing HRV...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);

        // Get and process the ECG signal
        Signal inputSignal = fileReader.read_MLII();
        qDebug() << "Input signal stats - Size:" << inputSignal.getSize() 
                 << "Sampling rate:" << inputSignal.getSamplingRate();
        progress.setValue(20);

        Signal filteredSignal = baseline.filterSignal(inputSignal);
        progress.setValue(30);
        
        // Clear and detect R-peaks
        r_peak_positions.clear();
        std::vector<int> peaks;
        rPeaks.setParams("PAN_TOMPKINS", 3, 0.018);
        
        if (!rPeaks.detectRPeaks(filteredSignal.getY(), inputSignal.getSamplingRate(), peaks)) {
            throw std::runtime_error("R-peaks detection failed");
        }
        
        qDebug() << "Detected" << peaks.size() << "R-peaks";
        if (peaks.size() < 2) {
            throw std::runtime_error("Insufficient R-peaks for HRV analysis");
        }
        progress.setValue(50);

        // Convert peaks to time domain
        std::vector<double> peakTimes;
        for (int idx : peaks) {
            if (idx >= 0 && idx < filteredSignal.getX().size()) {
                // Convert to seconds
                peakTimes.push_back(idx / static_cast<double>(inputSignal.getSamplingRate()));
            }
        }
        
        qDebug() << "Converting" << peakTimes.size() << "peaks to signal";
        progress.setValue(60);

        // Create signal for HRV analysis
        Signal rPeaksSignal(peakTimes, 
                          std::vector<double>(peakTimes.size(), 1.0),
                          inputSignal.getSamplingRate());

        // Process HRV
        qDebug() << "Starting HRV2 processing...";
        hrv2.process(rPeaksSignal);
        progress.setValue(70);

        // Get Poincaré plot data and validate
        Signal poincarePlot = hrv2.getPoincarePlot();
        auto params = hrv2.getParams();
        
        qDebug() << "Retrieved Poincaré plot data - Size:" << poincarePlot.getSize();
        progress.setValue(80);

        if (poincarePlot.getSize() == 0) {
            throw std::runtime_error("Invalid Poincaré plot data generated");
        }

        // Update UI
        QLayout* layout = ui->frame_2->layout();
        if (!layout) {
            layout = new QVBoxLayout(ui->frame_2);
            ui->frame_2->setLayout(layout);
        }

        // Clear existing widgets
        while (QLayoutItem* item = layout->takeAt(0)) {
            delete item->widget();
            delete item;
        }
        progress.setValue(90);

        // Create and setup scatter plot
        Scatter_Plot* plotWidget = new Scatter_Plot();
        layout->addWidget(plotWidget);

        // From HRV_2 parameters:
        // params[6] = SD1
        // params[7] = SD2
        // params[8] = Center X (mean RR)
        // params[9] = Center Y (mean RR)
        
        qDebug() << "Updating Poincaré plot with:"
                 << "\nSD1:" << params[6]
                 << "\nSD2:" << params[7]
                 << "\nCenter X:" << params[8]
                 << "\nCenter Y:" << params[9];

        plotWidget->updateScatterPlot(
            poincarePlot,          // Signal containing RRn vs RRn+1 points
            params[8],             // Center X (mean RR)
            params[9],             // Center Y (mean RR)
            params[6],             // SD1 (shorter axis)
            params[7],             // SD2 (longer axis)
            "RR Intervals",        // Legend for points
            "SD Ellipse",          // Legend for ellipse
            "Poincaré Plot",       // Title
            "RR(n) [s]",          // X-axis label
            "RR(n+1) [s]"         // Y-axis label
        );

        // Show SD1/SD2 results
        QString results = QString("Poincaré Plot Parameters:\n")
                         + QString("SD1: %1 ms\n").arg(params[6] * 1000, 0, 'f', 2)
                         + QString("SD2: %1 ms\n").arg(params[7] * 1000, 0, 'f', 2)
                         + QString("SD1/SD2: %1").arg(params[6] / params[7], 0, 'f', 3);
        QMessageBox::information(this, "HRV Parameters", results);

        progress.setValue(100);

    } catch (const std::exception& e) {
        qDebug() << "Error in HRV2 Poincaré plot analysis:" << e.what();
        QMessageBox::critical(this, "Error", 
            QString("Failed to process HRV: %1").arg(e.what()));
    }
}

void MainWindow::on_btnHRV2_hist_clicked()
{
    qDebug() << "Starting HRV2 histogram analysis...";

    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Get and process the ECG signal
        Signal inputSignal = fileReader.read_MLII();
        qDebug() << "Input signal stats - Size:" << inputSignal.getSize() 
                 << "Sampling rate:" << inputSignal.getSamplingRate();

        Signal filteredSignal = baseline.filterSignal(inputSignal);
        
        // Clear and detect R-peaks
        r_peak_positions.clear();
        std::vector<int> peaks;
        rPeaks.setParams("PAN_TOMPKINS", 3, 0.018);
        
        if (!rPeaks.detectRPeaks(filteredSignal.getY(), inputSignal.getSamplingRate(), peaks)) {
            throw std::runtime_error("R-peaks detection failed");
        }
        
        qDebug() << "Detected" << peaks.size() << "R-peaks";
        if (peaks.size() < 2) {
            throw std::runtime_error("Insufficient R-peaks for HRV analysis");
        }

        // Convert peaks to time domain
        std::vector<double> peakTimes;
        for (int idx : peaks) {
            if (idx >= 0 && idx < filteredSignal.getX().size()) {
                // Convert to seconds
                peakTimes.push_back(idx / static_cast<double>(inputSignal.getSamplingRate()));
            }
        }
        
        qDebug() << "Converting" << peakTimes.size() << "peaks to signal";
        
        // Validate peak times
        if (peakTimes.size() < 2) {
            throw std::runtime_error("Insufficient valid peak times for analysis");
        }

        // Create signal for HRV analysis
        Signal rPeaksSignal(peakTimes, 
                          std::vector<double>(peakTimes.size(), 1.0),
                          inputSignal.getSamplingRate());

        // Process HRV
        qDebug() << "Starting HRV2 processing...";
        hrv2.process(rPeaksSignal);

        // Get histogram and validate
        Signal histogram = hrv2.getRHist();
        qDebug() << "Retrieved histogram - Size X:" << histogram.getX().size() 
                 << "Y:" << histogram.getY().size();

        if (histogram.getX().empty() || histogram.getY().empty()) {
            throw std::runtime_error("Invalid histogram data generated");
        }

        // Update UI
        QLayout* layout = ui->frame_2->layout();
        if (!layout) {
            layout = new QVBoxLayout(ui->frame_2);
            ui->frame_2->setLayout(layout);
        }

        // Clear existing widgets
        while (QLayoutItem* item = layout->takeAt(0)) {
            delete item->widget();
            delete item;
        }

        // Create and setup histogram plot
        Histogram_Plot* plotWidget = new Histogram_Plot();
        layout->addWidget(plotWidget);

        qDebug() << "Updating histogram plot";
        plotWidget->updateHistogramPlot(
            histogram,
            "RR Intervals Histogram",
            "Frequency",
            "RR Interval [s]",
            "Count"
        );

        // Show results
        auto params = hrv2.getParams();
        QString results = QString("HRV Results:\n")
                         + QString("TINN: %1 ms\n").arg(params[4], 0, 'f', 2)
                         + QString("Triangular Index: %1").arg(params[5], 0, 'f', 2);
        QMessageBox::information(this, "HRV Parameters", results);

    } catch (const std::exception& e) {
        qDebug() << "Error in HRV2 histogram analysis:" << e.what();
        QMessageBox::critical(this, "Error", 
            QString("Failed to process HRV: %1").arg(e.what()));
    }
}

void MainWindow::on_btnHRV_DFA_clicked()
{
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing DFA...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);

        // Get the ECG signal
        Signal inputSignal = fileReader.read_MLII();
        progress.setValue(20);

        // Filter the signal
        Signal filteredSignal = baseline.filterSignal(inputSignal);
        progress.setValue(40);

        // Detect R-peaks if not already detected
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
        progress.setValue(60);

        // Calculate RR intervals
        std::vector<double> rr_intervals;
        for (int i = 1; i < r_peak_positions.size(); ++i) {
            double interval = (filteredSignal.getX()[r_peak_positions[i]] - 
                             filteredSignal.getX()[r_peak_positions[i-1]]);
            rr_intervals.push_back(interval);
        }
        
        // Process DFA
        HRV_DFA dfa;
        dfa.process(rr_intervals);
        progress.setValue(80);

        // Log results
        qDebug() << "DFA Results:";
        qDebug() << "α1 (short-term):" << dfa.getA1();
        qDebug() << "α2 (long-term):" << dfa.getA2();

        progress.setValue(100);

        // Show success message
        QMessageBox::information(this, "Success", 
            QString("DFA Analysis completed successfully.\nα1: %1\nα2: %2")
                .arg(dfa.getA1())
                .arg(dfa.getA2()));

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process DFA: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing DFA.");
    }
}

void MainWindow::displayHRVResults(const std::array<double, 5>& timeParams,const std::array<double, 6>& freqParams)
{
    // Prepare data for the table
    QVector<QVector<QString>> tableData;
    
    // Headers
    tableData.append({"Parameter", "Value", "Unit"});
    
    // Time domain parameters
    tableData.append({"RR Mean", QString::number(timeParams[0], 'f', 2), "ms"});
    tableData.append({"SDNN", QString::number(timeParams[1], 'f', 2), "ms"});
    tableData.append({"RMSSD", QString::number(timeParams[2], 'f', 2), "ms"});
    tableData.append({"NN50", QString::number(timeParams[3], 'f', 0), "count"});
    tableData.append({"pNN50", QString::number(timeParams[4], 'f', 2), "%"});
    
    // Frequency domain parameters
    tableData.append({"HF", QString::number(freqParams[0], 'f', 2), "ms²"});
    tableData.append({"LF", QString::number(freqParams[1], 'f', 2), "ms²"});
    tableData.append({"VLF", QString::number(freqParams[2], 'f', 2), "ms²"});
    tableData.append({"ULF", QString::number(freqParams[3], 'f', 2), "ms²"});
    tableData.append({"Total Power", QString::number(freqParams[4], 'f', 2), "ms²"});
    tableData.append({"LF/HF Ratio", QString::number(freqParams[5], 'f', 2), "-"});

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

    // Create and set up the table
    hrvTable = new Table(ui->frame_2);
    layout->addWidget(hrvTable);
    
    hrvTable->setTitle("Heart Rate Variability Analysis Results");
    hrvTable->setData(tableData);
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