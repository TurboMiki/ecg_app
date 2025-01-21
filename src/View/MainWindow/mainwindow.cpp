#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingsform.h"

#include "basic_plot.h"
#include "waves_plot.h"
#include "scatter_plot.h"
#include "histogram_plot.h"
#include "table.h"

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

void clearLayout(QLayout* layout){
    QLayoutItem* child;
    while ((child = layout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , plotWidget(nullptr)
{
    ui->setupUi(this);
    ui->showTable->setEnabled(false); 

    // Initialize SettingsForm
    ptrSettingsForm = new SettingsForm(this);

    // Connect signals and slots
    // connect(ptrSettingsForm, &SettingsForm::pass_values, this, &MainWindow::get_settings);
    connect(ptrSettingsForm, &SettingsForm::settingsChanged, this, &MainWindow::onSettingsChanged);
    // connect(this, &MainWindow::requestData, ptrSettingsForm, &SettingsForm::pass_values);
    // connect(ui->showTable, &QCheckBox::stateChanged, this, &MainWindow::on_showTable_stateChanged);

    // Initialize button states
    isFileSelected = false;
    isSignalAnalyzed = false;
    setDefaultParameters();
    updateButtonStates();
}

MainWindow::~MainWindow()
{
    delete ptrSettingsForm;
    if (plotWidget) {
        delete plotWidget;
    }
    delete ui;
}

void MainWindow::updateButtonStates() {
    // Buttons that require file selection
    ui->btnRaw->setEnabled(isFileSelected);
    ui->btnFECG->setEnabled(isFileSelected);
    ui->START->setEnabled(isFileSelected);
    ui->checkBoxRP->setEnabled(isFileSelected);
    ui->checkBoxQRS->setEnabled(isFileSelected);
    ui->pushButton->setEnabled(isFileSelected);

    // Buttons that require signal analysis
    ui->btnHRV_1->setEnabled(isSignalAnalyzed);
    ui->btnHRV2_PC->setEnabled(isSignalAnalyzed);
    ui->btnHRV2_hist->setEnabled(isSignalAnalyzed);
    ui->btnHRV_DFA->setEnabled(isSignalAnalyzed);
    ui->btnHeartClass->setEnabled(isSignalAnalyzed);
}

void MainWindow::on_START_clicked()
{
    // Validate file selection
    if (!isFileSelected || ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    // Validate parameters
    if (baselineParams.isEmpty() || rpeaksParams.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Configuration parameters are not set!");
        return;
    }
    
    // Setup progress dialog
    QProgressDialog progress("Processing signal...", "Cancel", 0, 100, this);
    progress.setWindowModality(Qt::WindowModal);
    progress.setMinimumDuration(0);
    progress.setValue(0);

    try {
        // Apply baseline filter
        progress.setLabelText("Applying baseline filter...");
        Signal inputSignal = fileReader.read_MLII();
        if (inputSignal.getY().empty()) {
            throw std::runtime_error("Input signal is empty");
        }

        // Select and apply appropriate baseline filter
        if (currentBaselineMethod == "MM") {
            auto movingMeanFilter = std::make_unique<MovingMeanFilter>();
            movingMeanFilter->set(baselineParams["Window Length"]);
            baseline.setFilter(std::move(movingMeanFilter));
        }
        else if (currentBaselineMethod == "Btw") {
            auto butterworthFilter = std::make_unique<ButterworthFilter>();
            butterworthFilter->set(
                baselineParams["Filter Order"],
                baselineParams["Upper Frequency"],
                baselineParams["Lower Frequency"]
            );
            baseline.setFilter(std::move(butterworthFilter));
        }
        else if (currentBaselineMethod == "SG") {
            auto sgFilter = std::make_unique<SavitzkyGolayFilter>();
            sgFilter->set(
                baselineParams["Window Length"],
                baselineParams["Filter Order"]
            );
            baseline.setFilter(std::move(sgFilter));
        }
        else if (currentBaselineMethod == "LMS") {
            auto lmsFilter = std::make_unique<LMSFilter>();
            baseline.setFilter(std::move(lmsFilter));
        }
        else {
            throw std::runtime_error("Invalid baseline filter method selected");
        }

        try {
            baseline.filterSignal(inputSignal);
            progress.setValue(20);
        } catch (const std::exception& e) {
            QMessageBox::critical(this, "Filtering Error",
                QString("Failed to filter signal: %1").arg(e.what()));
            return;
        } catch (...) {
            QMessageBox::critical(this, "Filtering Error",
                "An unknown error occurred while filtering the signal");
            return;
        }

        // RPeaks detection
        progress.setLabelText("Detecting R peaks...");
        std::vector<int> peaks;
        Signal filtered = baseline.getSignal();
        
        if (filtered.getY().empty()) {
            throw std::runtime_error("Filtered signal is empty");
        }

        if (currentRPeaksMethod == "PT") {
            rPeaks.setParams("PAN_TOMPKINS", rpeaksParams["Window Length"], rpeaksParams["Threshold"]);
        }
        else if (currentRPeaksMethod == "Hilbert") {
            rPeaks.setParams("HILBERT",rpeaksParams["Proximity"],rpeaksParams["Threshold"]);
        }
        else {
            throw std::runtime_error("Invalid R-peaks detection method selected");
        }

        if (!rPeaks.detectRPeaks(filtered.getY(), filtered.getSamplingRate(), peaks)) {
            throw std::runtime_error("R-peak detection failed");
        }

        if (peaks.empty()) {
            throw std::runtime_error("No R peaks detected");
        } else {
            cout << peaks.size() << endl;
        }

        // Convert peaks to QList
        r_peak_positions.clear();
        r_peak_positions.reserve(peaks.size());
        std::copy(peaks.begin(), peaks.end(), std::back_inserter(r_peak_positions));
        
        progress.setValue(40);

        // Waves detection
        progress.setLabelText("Detecting wave components...");
        waveDetector.setRPeaks(r_peak_positions);
        if (!waveDetector.detectWaves(filtered)) {
            throw std::runtime_error("Wave detection failed");
        }
        progress.setValue(60);

        // HRV_1 analysis
        progress.setLabelText("Performing HRV analysis...");
        std::vector<double> peakTimes;
        peakTimes.reserve(r_peak_positions.size());
        for (int idx : r_peak_positions) {
            peakTimes.push_back(filtered.getX()[idx]);
        }

        Signal rPeaksSignal(peakTimes, std::vector<double>(peakTimes.size(), 1.0),filtered.getSamplingRate());

        HRV_1 hrvAnalyzer(rPeaksSignal, filtered);
        hrvAnalyzer.process();

        // Get HRV_1 results
        timeParams = hrvAnalyzer.getTimeParams();
        freqParams = hrvAnalyzer.getFreqParams();
        progress.setValue(70);

        // HRV_2 analysis
        progress.setLabelText("Calculating HRV_2 parameters...");
        hrv2.process(rPeaksSignal);
        progress.setValue(80);

        // HRV_DFA analysis
        progress.setLabelText("Performing DFA analysis...");
        std::vector<double> rr_intervals;
        rr_intervals.reserve(r_peak_positions.size() - 1);
        for (int i = 1; i < r_peak_positions.size(); ++i) {
            double interval = (filtered.getX()[r_peak_positions[i]] - filtered.getX()[r_peak_positions[i-1]]);
            rr_intervals.push_back(interval);
        }

        dfa.process(rr_intervals);
        progress.setValue(90);

        // Update GUI state
        isSignalAnalyzed = true;
        updateButtonStates();

        // Update current plot if showing filtered or raw signal
        if (currentPlot == PLOT_TYPE::FILTERED_PLOT || 
            currentPlot == PLOT_TYPE::RAW_PLOT) {
            createPlot(ui->frame_2->layout(), currentPlot);
        }
        
        progress.setValue(100);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Analysis failed: %1").arg(e.what()));
        isSignalAnalyzed = false;
        updateButtonStates();
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred during analysis.");
        isSignalAnalyzed = false;
        updateButtonStates();
    }
}

void MainWindow::on_Config_clicked()
{
    if (ptrSettingsForm) {
        ptrSettingsForm->show();
    }
}

void MainWindow::onSettingsChanged(const QString &baselineMethod,const QMap<QString, double> &baselineParams,const QString &rpeaksMethod,const QMap<QString, double> &rpeaksParams)
{
    // Store new parameters
    this->currentBaselineMethod = baselineMethod;
    this->currentRPeaksMethod = rpeaksMethod;
    this->baselineParams = baselineParams;
    this->rpeaksParams = rpeaksParams;

    // If file is loaded, reprocess with new parameters
    if (isFileSelected) {
        on_START_clicked();
    }
}

void MainWindow::on_btnPath_clicked()
{
    QString fileFilter = "All Supported Files (*.dat *.csv *.xlsx);;Data Files (*.dat);;CSV Files (*.csv);;Excel Files (*.xlsx)";
    QString selectedFile = QFileDialog::getOpenFileName(this, "Choose file", QDir::homePath(), fileFilter);
    if (!selectedFile.isEmpty())
    {
        this->filePath = selectedFile;
        ui->linePath->setText(this->filePath);
        fileReader.set_path(ui->linePath->text().toStdString());
        fileReader.read_file();
        fileReader.write_measured_time();

        setDefaultParameters(); 
        // Apply default filter with default parameters
        auto movingMeanFilter = std::make_unique<MovingMeanFilter>();
        movingMeanFilter->set(baselineParams["Window Length"]);  // Use stored default parameter

        baseline.setFilter(std::move(movingMeanFilter));
        baseline.filterSignal(fileReader.read_MLII());
        isFileSelected = true;
        isSignalAnalyzed = false;
        updateButtonStates();  // Update button states after file selection

        // Ensure frame_2 has a layout
        QLayout* layout = ui->frame_2->layout();
        if (!layout) {
            layout = new QVBoxLayout(ui->frame_2);
            ui->frame_2->setLayout(layout);
        }
        // Create and add plot
        currentPlot = PLOT_TYPE::RAW_PLOT;
        createPlot(layout,currentPlot);
    }
}

void MainWindow::on_btnRaw_clicked()
{
    ui->showTable->setDisabled(true);//?

    // Create and add plot
    if (isFileSelected){
    QLayout* layout = ui->frame_2->layout();
    currentPlot = PLOT_TYPE::RAW_PLOT;
    createPlot(ui->frame_2->layout(),currentPlot);
    }
}

void MainWindow::on_btnFECG_clicked()
{
    // Create and add plot
    if (isFileSelected){
    QLayout* layout = ui->frame_2->layout();
    currentPlot = PLOT_TYPE::FILTERED_PLOT;
    createPlot(ui->frame_2->layout(),currentPlot);
    }
}

void MainWindow::on_pushButton_clicked()
{
    if (isFileSelected){
    QWidget *newWindow = new QWidget();
    newWindow->setWindowTitle("Layout in New Window");

    QLayout *layout = new QVBoxLayout(newWindow);
    // Create and add plot in new window
    createPlot(layout,currentPlot);
    newWindow->resize(800,500);
    newWindow->show();
    }
}

void MainWindow::on_btnHRV_1_clicked()
{
    if (isSignalAnalyzed){
        QLayout* layout = ui->frame_2->layout();
        currentPlot = PLOT_TYPE::HRV_TABLE;
        createPlot(ui->frame_2->layout(),currentPlot);
    }
}

void MainWindow::on_btnHRV2_PC_clicked()
{
    // Get Poincaré plot data and validate
    if (isSignalAnalyzed){
        currentPlot = PLOT_TYPE::POINCARE;
        createPlot(ui->frame_2->layout(),currentPlot);
    }
}

void MainWindow::on_btnHRV2_hist_clicked()
{
    if (isSignalAnalyzed){
        currentPlot = PLOT_TYPE::HISTOGRAM;
        createPlot(ui->frame_2->layout(),currentPlot);
    }
}

void MainWindow::on_btnHRV_DFA_clicked()
{
    QMessageBox::information(this, "Success",
                             QString("DFA Analysis completed successfully.\nα1: %1\nα2: %2")
                                 .arg(dfa.getA1())
                                 .arg(dfa.getA2()));
}

void MainWindow::on_checkBoxRP_stateChanged(int state)
{
    // If RP checkbox is checked, uncheck QRS
    if (state == Qt::Checked) {
        ui->checkBoxQRS->blockSignals(true);  // Prevent recursive signal handling
        ui->checkBoxQRS->setCheckState(Qt::Unchecked);
        ui->checkBoxQRS->blockSignals(false);
    }

    QLayout* layout = ui->frame_2->layout();
    if (currentPlot != PLOT_TYPE::RAW_PLOT && currentPlot != PLOT_TYPE::FILTERED_PLOT) {
        currentPlot = PLOT_TYPE::RAW_PLOT;
    }
    createPlot(layout, currentPlot);
}

void MainWindow::on_checkBoxQRS_stateChanged(int state)
{
    // If QRS checkbox is checked, uncheck RP
    if (state == Qt::Checked) {
        ui->checkBoxRP->blockSignals(true);  // Prevent recursive signal handling
        ui->checkBoxRP->setCheckState(Qt::Unchecked);
        ui->checkBoxRP->blockSignals(false);
    }

    QLayout* layout = ui->frame_2->layout();
    if (currentPlot != PLOT_TYPE::RAW_PLOT && currentPlot != PLOT_TYPE::FILTERED_PLOT) {
        currentPlot = PLOT_TYPE::RAW_PLOT;
    }
    createPlot(layout, currentPlot);
}

void MainWindow::setDefaultParameters()
{
    // Set default Baseline parameters
    baselineParams.clear();
    if (currentBaselineMethod == "MM") {
        baselineParams["Window Length"] = 5;
    } 
    else if (currentBaselineMethod == "Btw") {
        baselineParams["Filter Order"] = 3;
        baselineParams["Upper Frequency"] = 15.0;
        baselineParams["Lower Frequency"] = 1.0;
    }
    else if (currentBaselineMethod == "SG") {
        baselineParams["Window Length"] = 5;
        baselineParams["Filter Order"] = 3;
    }
    // LMS parameters are handled via file selection

    // Set default RPeaks parameters
    rpeaksParams.clear();
    if (currentRPeaksMethod == "PT") {
        rpeaksParams["Window Length"] = 0;
        rpeaksParams["Threshold"] = 0;
    }
    else if (currentRPeaksMethod == "Hilbert") {
        rpeaksParams["Proximity"] = 0;
        rpeaksParams["Threshold"] = 0;
    }
}

// void MainWindow::on_showTable_stateChanged(int state)
// {
//     for(int i=0;i<2;i++) {
//         // Pobierz układ przypisany do frame_2
//         QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->frame_2->layout());
//         if (!layout) {
//             layout = new QVBoxLayout(ui->frame_2);
//             ui->frame_2->setLayout(layout);
//         }
//
//         // Jeśli checkbox jest zaznaczony
//         if (state == Qt::Checked) {
//             ui->pushButton->setDisabled(true);
//             if (!tableWidget) {
//                 // Tworzenie nowej tabeli, jeśli jeszcze nie istnieje
//                 tableWidget = new Table(this);
//                 //tableWidget->setMinimumWidth(200);
//                 //tableWidget->setMinimumHeight(0);
//                 //tableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
//                 //tableWidget->setTitle("ECG Signal Data");
//
//                 // Przykładowe dane dla tabeli
//                 QVector<QVector<QString>> tableData;
//                 Signal signal = fileReader.read_MLII();
//                 for (int i = 0; i < signal.getY().size(); ++i) {
//                     QVector<QString> row;
//                     row.append(QString::number(signal.getX()[i]));  // Czas
//                     row.append(QString::number(signal.getY()[i]));  // Napięcie
//                     tableData.append(row);
//                 }
//
//                 // Ustaw dane w tabeli
//                 qobject_cast<Table*>(tableWidget)->setData(tableData);
//             }
//
//             // Dodaj tabelę do układu, jeśli jej nie ma
//
//             layout->addWidget(tableWidget);
//
//             // Dostosuj proporcje: wykres (1 część), tabela (1 część)
//             if (currentPlotWidget) {
//                 //currentPlotWidget->setVisible(true);
//             }
//             //layout->setStretch(0, 1); // Wykres
//             //layout->setStretch(1, 1); // Tabela
//             tableWidget->show();
//
//             // Jeśli checkbox jest odznaczony
//             // Ukryj tabelę
//             if(i<1){
//                 if (tableWidget) {
//                     layout->removeWidget(tableWidget);
//                     tableWidget->hide();
//                     ui->pushButton->setDisabled(false);
//                     //delete tableWidget;
//                 }
//
//                 // Dostosuj proporcje: wykres zajmuje całą wysokość
//                 if (currentPlotWidget) {
//                     currentPlotWidget->setVisible(true);
//                 }
//                 layout->setStretch(0, 2); // Wykres zajmuje pełną wysokość
//                 resizeLayout();
//             }
//         } else {  // Jeśli checkbox jest odznaczony
//             // Ukryj tabelę, i usuń ją
//             if (tableWidget) {
//                 layout->removeWidget(tableWidget);
//                 tableWidget->hide();
//                 ui->pushButton->setDisabled(false);
//                 //delete tableWidget;
//             }
//
//             // Dostosuj proporcje: wykres zajmuje całą wysokość
//             if (currentPlotWidget) {
//                 currentPlotWidget->setVisible(true);
//             }
//             layout->setStretch(0, 2); // Wykres zajmuje pełną wysokość
//         }
//
//         // Dostosuj geometrię układu
//         resizeLayout();
//     }
// }

void MainWindow::resizeLayout()
{
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->frame_2->layout());
    if (!layout) return;

    // Pobierz geometrię całego układu
    QRect frameGeometry = ui->frame_2->geometry();

    // Oblicz wymiary dla każdego widgetu
    int totalHeight = frameGeometry.height();
    int plotHeight = ui->showTable->isChecked() ? totalHeight / 2 : totalHeight; // Wykres zajmuje całą wysokość, jeśli tabela jest ukryta
    int tableHeight = ui->showTable->isChecked() ? totalHeight / 2 : 0;         // Tabela zajmuje połowę, jeśli checkbox jest zaznaczony

    // Przypisz nowe wymiary do widgetów
    if (currentPlotWidget) {
        QRect plotGeometry = QRect(frameGeometry.x(), frameGeometry.y(), frameGeometry.width(), plotHeight);
        currentPlotWidget->setGeometry(plotGeometry);
    }

    if (tableWidget && ui->showTable->isChecked()) {
        QRect tableGeometry = QRect(frameGeometry.x(), frameGeometry.y() + plotHeight, frameGeometry.width(), tableHeight);
        tableWidget->setGeometry(tableGeometry);
    }
}

void MainWindow::on_btnHeartClass_clicked()
{
    if (!isSignalAnalyzed) {
        QMessageBox::warning(this, "Warning", "Please analyze the signal first by clicking START!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing heart classification...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);

        // Get filtered signal
        Signal filteredSignal = baseline.getSignal();
        progress.setValue(20);

        // Convert QList r_peak_positions to std::vector
        std::vector<int> rPeaksVec(r_peak_positions.begin(), r_peak_positions.end());
        
        // Get wave points and convert QVector to std::vector
        QVector<int> qrsEndsQV = waveDetector.getQRSEnds();
        QVector<int> qrsOnsetsQV = waveDetector.getQRSOnsets();
        QVector<int> pOnsetsQV = waveDetector.getPOnsets();

        // Convert to std::vector
        std::vector<int> qrsEndsVec(qrsEndsQV.begin(), qrsEndsQV.end());
        std::vector<int> qrsOnsetsVec(qrsOnsetsQV.begin(), qrsOnsetsQV.end());
        std::vector<int> pOnsetsVec(pOnsetsQV.begin(), pOnsetsQV.end());
        
        progress.setValue(60);

        // Process heart classification with new interface
        heartClassifier.process(rPeaksVec, 
                              pOnsetsVec,
                              qrsEndsVec,
                              qrsOnsetsVec,
                              filteredSignal.getSamplingRate());
        
        progress.setValue(80);

        // Get results using the new getter methods
        QString resultMessage = QString("Heart Classification Results:\n\n")
            + QString("Supraventricular beats: %1\n").arg(heartClassifier.getSupraventricularCount())
            + QString("Ventricular beats: %1\n").arg(heartClassifier.getVentricularCount())
            + QString("Other cardiac conditions: %1\n").arg(heartClassifier.getDiffDiseaseCount())
            + QString("Possible artifacts: %1\n").arg(heartClassifier.getArtifactCount())
            + QString("\nTotal analyzed beats: %1").arg(heartClassifier.getTotalCount());

        progress.setValue(100);
        QMessageBox::information(this, "Heart Classification Results", resultMessage);

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
            QString("Failed to process heart classification: %1").arg(e.what()));
    } catch (...) {
        QMessageBox::critical(this, "Error", 
            "An unknown error occurred while processing heart classification.");
    }
}

void MainWindow::createPlot(QLayout* layout,PLOT_TYPE plotType){

    // Clear any existing widgets in the layout
    clearLayout(layout);

    switch (plotType) {
    case PLOT_TYPE::RAW_PLOT:{
        Signal signal = fileReader.read_MLII();
        // Verify the data
        if(signal.getX().empty() || signal.getY().empty()) {
            QMessageBox::warning(this, "Error", "No data available to plot! Signal is empty.");
            return;
        }

        if (ui->checkBoxQRS->isChecked() && isSignalAnalyzed){
            Waves_Plot* plotWidget = new Waves_Plot();
            layout->addWidget(plotWidget);

            // Update plot with all wave components
            plotWidget->updateWavesPlot(
                signal,
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
        }else if (ui->checkBoxRP->isChecked() && isSignalAnalyzed){
            Basic_Plot* plotWidget = new Basic_Plot();
            layout->addWidget(plotWidget);
            plotWidget->updateBasicPlot(signal, r_peak_positions, "MLII Signal","Indeksy R","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        }else{
            Basic_Plot* plotWidget = new Basic_Plot();
            layout->addWidget(plotWidget);
            plotWidget->updateBasicPlot(signal, {}, "MLII Signal","","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        }
        break;
    }

    case PLOT_TYPE::FILTERED_PLOT:{
        Signal signal = baseline.getSignal();
        if(signal.getX().empty() || signal.getY().empty()) {
            QMessageBox::warning(this, "Error", "No data available to plot! Signal is empty.");
            return;
        }

        if (ui->checkBoxQRS->isChecked() && isSignalAnalyzed){
            Waves_Plot* plotWidget = new Waves_Plot();
            layout->addWidget(plotWidget);

            // Update plot with all wave components
            plotWidget->updateWavesPlot(
                signal,
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
        }else if (ui->checkBoxRP->isChecked() && isSignalAnalyzed){
            Basic_Plot* plotWidget = new Basic_Plot();
            layout->addWidget(plotWidget);
            plotWidget->updateBasicPlot(signal, r_peak_positions, "MLII Signal","Indeksy R","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        }else{
            Basic_Plot* plotWidget = new Basic_Plot();
            layout->addWidget(plotWidget);
            plotWidget->updateBasicPlot(signal, {}, "MLII Signal","","ECG Signal (MLII)", "Time [s]", "Voltage [mV]");
        }
        break;
    }

    case PLOT_TYPE::POINCARE :{
        Signal poincarePlot = hrv2.getPoincarePlot();
        auto params = hrv2.getParams();

        Scatter_Plot* plotWidget = new Scatter_Plot();
        layout->addWidget(plotWidget);

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
        break;
    }
    
    case PLOT_TYPE::HISTOGRAM :{
        Signal histogram = hrv2.getRHist();

        // Create and setup histogram plot
        Histogram_Plot* plotWidget = new Histogram_Plot();
        layout->addWidget(plotWidget);

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
                            + QString("Opimal N [s]: %1 ms\n").arg(params[2], 0, 'f', 2)
                            + QString("Opimal M [s]: %1 ms\n").arg(params[3], 0, 'f', 2)
                            + QString("TINN: %1 ms\n").arg(params[4], 0, 'f', 2)
                            + QString("Triangular Index: %1\n").arg(params[5], 0, 'f', 2)
                            + QString("SD1: %1 ms\n").arg(params[6], 0, 'f', 2)
                            + QString("SD2: %1 ms\n").arg(params[7], 0, 'f', 2);
        QMessageBox::information(this, "HRV Parameters", results);
        break;
    }

    case PLOT_TYPE::HRV_TABLE:{
    QVector<QVector<QString>> tableData;
    
    tableData.append({"Parameter", "Value", "Unit"});

    tableData.append({"RR Mean", QString::number(timeParams[0], 'e', 2), "ms"});
    tableData.append({"SDNN", QString::number(timeParams[1], 'e', 2), "ms"});
    tableData.append({"RMSSD", QString::number(timeParams[2], 'e', 2), "ms"});
    tableData.append({"NN50", QString::number(timeParams[3], 'f', 0), "count"});
    tableData.append({"pNN50", QString::number(timeParams[4], 'e', 2), "%"});

    tableData.append({"HF", QString::number(freqParams[0], 'e', 2), "ms²"});
    tableData.append({"LF", QString::number(freqParams[1], 'e', 2), "ms²"});
    tableData.append({"VLF", QString::number(freqParams[2], 'e', 2), "ms²"});
    tableData.append({"ULF", QString::number(freqParams[3], 'e', 2), "ms²"});
    tableData.append({"Total Power", QString::number(freqParams[4], 'e', 2), "ms²"});
    tableData.append({"LF/HF Ratio", QString::number(freqParams[5], 'e', 3), "-"});

    Table* tableWidget = new Table(ui->frame_2);
    layout->addWidget(tableWidget);
    tableWidget->setTitle("Heart Rate Variability Analysis Results");
    tableWidget->setData(tableData);

    hrvTable = tableWidget;
    break;
    }

    default:
        break;
    }
}
