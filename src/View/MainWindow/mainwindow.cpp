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

    // Set icons
    //ui->START->setIcon(QIcon(":/icons/play.png"));
    //ui->Config->setIcon(QIcon("C:/Projects/ecg_app_GUI/src/View/MainWindow/icons/gear.png"));
    //ui->START->setIconSize(QSize(50, 50));

    // Initialize SettingsForm
    ptrSettingsForm = new SettingsForm(this);

    // Connect signals and slots
    connect(ptrSettingsForm, &SettingsForm::pass_values, this, &MainWindow::get_settings);
    connect(this, &MainWindow::requestData, ptrSettingsForm, &SettingsForm::pass_values);
    connect(ui->showTable, &QCheckBox::stateChanged, this, &MainWindow::on_showTable_stateChanged);
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
    if (isFileSelected){
        //Set progress bar
        QProgressDialog progress("Processing HRV...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(2);
        progress.setValue(0);

        // Rpeaks
        std::vector<int> peaks;
        Signal filtered = baseline.getSignal();
        rPeaks.setParams("PAN_TOMPKINS", 0, 0);
        // rPeaks.setParams("HILBERT", 200, 1.5, static_cast<int>(0.8 * inputSignal.getSamplingRate()));

        if (rPeaks.detectRPeaks(filtered.getY(), filtered.getSamplingRate(), peaks)) {

            // Convert peaks to QList whyyyyyyyyyyyyyyyy
            r_peak_positions.clear();
            r_peak_positions.reserve(peaks.size());
            for (const auto& peak : peaks) {
                r_peak_positions.append(peak);
            }
        }
        progress.setValue(10);

        // Waves
        waveDetector.setRPeaks(r_peak_positions);
        if (!waveDetector.detectWaves(filtered)) {
            throw std::runtime_error("Wave detection failed");
        }
        progress.setValue(20);

        // HRV_1
        // whyyyyyyyyyyyyyyyyy
        std::vector<double> peakTimes;
        for (int idx : r_peak_positions) {
            peakTimes.push_back(filtered.getX()[idx]);
        }

        Signal rPeaksSignal(peakTimes, std::vector<double>(peakTimes.size(), 1.0),
                            filtered.getSamplingRate());

        HRV_1 hrvAnalyzer(rPeaksSignal, filtered);
        hrvAnalyzer.process();

        // Get results
        timeParams = hrvAnalyzer.getTimeParams();
        freqParams = hrvAnalyzer.getFreqParams();
        progress.setValue(40);

        // HRV_2
        hrv2.process(rPeaksSignal);
        progress.setValue(60);

        // HRV_DFA

        std::vector<double> rr_intervals;
        for (int i = 1; i < r_peak_positions.size(); ++i) {
            double interval = (filtered.getX()[r_peak_positions[i]] -
                               filtered.getX()[r_peak_positions[i-1]]);
            rr_intervals.push_back(interval);
        }

        dfa.process(rr_intervals);
        progress.setValue(100);
        isSignalAnalyzed = true;

        // HearthClass
    }else {
        if (ui->linePath->text().isEmpty()) {
            QMessageBox::warning(this, "Warning", "Please select a file first!");
            return;
        }else {
            QMessageBox::warning(this, "Warning", "Error");
        }
    }
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
        fileReader.set_path(ui->linePath->text().toStdString());
        fileReader.read_file();
        fileReader.write_measured_time();

        auto movingMeanFilter = std::make_unique<MovingMeanFilter>();
        movingMeanFilter->set(5);  // window length of 15 points

        baseline.setFilter(std::move(movingMeanFilter));
        baseline.filterSignal(fileReader.read_MLII());
        isFileSelected = true;
        isSignalAnalyzed = false;

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
    QLayout* layout = ui->frame_2->layout();
    if (currentPlot!=PLOT_TYPE::RAW_PLOT && currentPlot!= PLOT_TYPE::FILTERED_PLOT){
        currentPlot = PLOT_TYPE::RAW_PLOT;
    }
    if (state == Qt::Checked){
        ui->checkBoxQRS->setCheckState(Qt::Unchecked);
    }
    createPlot(layout,currentPlot);
}

void MainWindow::on_checkBoxQRS_stateChanged(int state)
{
    QLayout* layout = ui->frame_2->layout();
    if (currentPlot!=PLOT_TYPE::RAW_PLOT && currentPlot!= PLOT_TYPE::FILTERED_PLOT){
        currentPlot = PLOT_TYPE::RAW_PLOT;
    }
    if (state == Qt::Checked){
        ui->checkBoxRP->setCheckState(Qt::Unchecked);
    }
    createPlot(layout,currentPlot);
}

void MainWindow::on_showTable_stateChanged(int state)
{
    for(int i=0;i<2;i++) {
        // Pobierz układ przypisany do frame_2
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->frame_2->layout());
        if (!layout) {
            layout = new QVBoxLayout(ui->frame_2);
            ui->frame_2->setLayout(layout);
        }

        // Jeśli checkbox jest zaznaczony
        if (state == Qt::Checked) {
            ui->pushButton->setDisabled(true);
            if (!tableWidget) {
                // Tworzenie nowej tabeli, jeśli jeszcze nie istnieje
                tableWidget = new Table(this);
                //tableWidget->setMinimumWidth(200);
                //tableWidget->setMinimumHeight(0);
                //tableWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                //tableWidget->setTitle("ECG Signal Data");

                // Przykładowe dane dla tabeli
                QVector<QVector<QString>> tableData;
                Signal signal = fileReader.read_MLII();
                for (int i = 0; i < signal.getY().size(); ++i) {
                    QVector<QString> row;
                    row.append(QString::number(signal.getX()[i]));  // Czas
                    row.append(QString::number(signal.getY()[i]));  // Napięcie
                    tableData.append(row);
                }

                // Ustaw dane w tabeli
                qobject_cast<Table*>(tableWidget)->setData(tableData);
            }

            // Dodaj tabelę do układu, jeśli jej nie ma

            layout->addWidget(tableWidget);

            // Dostosuj proporcje: wykres (1 część), tabela (1 część)
            if (currentPlotWidget) {
                //currentPlotWidget->setVisible(true);
            }
            //layout->setStretch(0, 1); // Wykres
            //layout->setStretch(1, 1); // Tabela
            tableWidget->show();

            // Jeśli checkbox jest odznaczony
            // Ukryj tabelę
            if(i<1){
                if (tableWidget) {
                    layout->removeWidget(tableWidget);
                    tableWidget->hide();
                    ui->pushButton->setDisabled(false);
                    //delete tableWidget;
                }

                // Dostosuj proporcje: wykres zajmuje całą wysokość
                if (currentPlotWidget) {
                    currentPlotWidget->setVisible(true);
                }
                layout->setStretch(0, 2); // Wykres zajmuje pełną wysokość
                resizeLayout();
            }
        } else {  // Jeśli checkbox jest odznaczony
            // Ukryj tabelę, i usuń ją
            if (tableWidget) {
                layout->removeWidget(tableWidget);
                tableWidget->hide();
                ui->pushButton->setDisabled(false);
                //delete tableWidget;
            }

            // Dostosuj proporcje: wykres zajmuje całą wysokość
            if (currentPlotWidget) {
                currentPlotWidget->setVisible(true);
            }
            layout->setStretch(0, 2); // Wykres zajmuje pełną wysokość
        }

        // Dostosuj geometrię układu
        resizeLayout();
    }
}

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
    if (ui->linePath->text().isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select a file first!");
        return;
    }

    try {
        // Show processing dialog
        QProgressDialog progress("Processing heart classification...", "Cancel", 0, 100, this);
        progress.setWindowModality(Qt::WindowModal);
        progress.setMinimumDuration(0);
        progress.setValue(0);

        // Get and process the signal
        Signal inputSignal = fileReader.read_MLII();
        Signal filteredSignal = baseline.filterSignal(inputSignal);
        progress.setValue(20);

        // Detect R-peaks if not already detected
        std::vector<int> peaks;
        if (r_peak_positions.isEmpty()) {
            rPeaks.setParams("PAN_TOMPKINS", 15, 0.3);
            if (!rPeaks.detectRPeaks(filteredSignal.getY(), inputSignal.getSamplingRate(), peaks)) {
                throw std::runtime_error("R-peaks detection failed");
            }
            r_peak_positions.reserve(peaks.size());
            for (const auto& peak : peaks) {
                r_peak_positions.append(peak);
            }
        } else {
            // Convert QList to vector
            peaks.reserve(r_peak_positions.size());
            for (const auto& peak : r_peak_positions) {
                peaks.push_back(peak);
            }
        }
        progress.setValue(40);

        // Detect waves if needed
        Waves waveDetector(filteredSignal, r_peak_positions);
        if (!waveDetector.detectWaves()) {
            throw std::runtime_error("Wave detection failed");
        }
        progress.setValue(60);

        // Get wave points and convert QVector to std::vector
        QVector<int> qrsEndsQV = waveDetector.getQRSEnds();
        QVector<int> qrsOnsetsQV = waveDetector.getQRSOnsets();
        QVector<int> pEndsQV = waveDetector.getPEnds();

        // Convert to std::vector
        std::vector<int> qrsEndsVec(qrsEndsQV.begin(), qrsEndsQV.end());
        std::vector<int> qrsOnsetsVec(qrsOnsetsQV.begin(), qrsOnsetsQV.end());
        std::vector<int> pEndsVec(pEndsQV.begin(), pEndsQV.end());
        
        // Process heart classification
        heartClassifier.process(peaks, 
                              pEndsVec,  // Using P-ends for P-wave points
                              qrsEndsVec,
                              qrsOnsetsVec,
                              inputSignal.getSamplingRate());
        progress.setValue(80);

        // Get results
        const auto& activations = heartClassifier.getActivations();

        // Show results in message box
        QString resultMessage = QString("Heart Classification Results:\nTotal activations detected: %1")
                                .arg(activations.size());
        
        progress.setValue(100);
        QMessageBox::information(this, "Heart Classification", resultMessage);

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
                          + QString("TINN: %1 ms\n").arg(params[4], 0, 'f', 2)
                          + QString("Triangular Index: %1").arg(params[5], 0, 'f', 2);
        QMessageBox::information(this, "HRV Parameters", results);
        break;
    }

    case PLOT_TYPE::HRV_TABLE:{
        QVector<QVector<QString>> tableData;

        // Headers
        tableData.append({"Parameter", "Value", "Unit"});

        // Time domain parameters
        tableData.append({"RR Mean", QString::number(timeParams[0], 'e', 2), "ms"});
        tableData.append({"SDNN", QString::number(timeParams[1], 'e', 2), "ms"});
        tableData.append({"RMSSD", QString::number(timeParams[2], 'e', 2), "ms"});
        tableData.append({"NN50", QString::number(timeParams[3], 'e', 2), "count"});
        tableData.append({"pNN50", QString::number(timeParams[4], 'e', 2), "%"});

        // Frequency domain parameters
        tableData.append({"HF", QString::number(freqParams[0], 'e', 2), "ms²"});
        tableData.append({"LF", QString::number(freqParams[1], 'e', 2), "ms²"});
        tableData.append({"VLF", QString::number(freqParams[2], 'e', 2), "ms²"});
        tableData.append({"ULF", QString::number(freqParams[3], 'e', 2), "ms²"});
        tableData.append({"Total Power", QString::number(freqParams[4], 'e', 2), "ms²"});
        tableData.append({"LF/HF Ratio", QString::number(freqParams[5], 'e', 2), "-"});

        // Create and set up the table
        hrvTable = new Table(ui->frame_2);
        layout->addWidget(hrvTable);

        hrvTable->setTitle("Heart Rate Variability Analysis Results");
        hrvTable->setData(tableData);
        break;
    }

    default:
        break;
    }
}

